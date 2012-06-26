//##################################################################################################
//
//   Custom Visualization Core library
//   Copyright (C) 2011-2012 Ceetron AS
//    
//   This library is free software: you can redistribute it and/or modify 
//   it under the terms of the GNU General Public License as published by 
//   the Free Software Foundation, either version 3 of the License, or 
//   (at your option) any later version. 
//    
//   This library is distributed in the hope that it will be useful, but WITHOUT ANY 
//   WARRANTY; without even the implied warranty of MERCHANTABILITY or 
//   FITNESS FOR A PARTICULAR PURPOSE.   
//    
//   See the GNU General Public License at <<http://www.gnu.org/licenses/gpl.html>> 
//   for more details. 
//
//##################################################################################################

#include "cvfBase.h"
#include "cvfTextDrawer.h"
#include "cvfFont.h"
#include "cvfGlyph.h"
#include "cvfTextureImage.h"
#include "cvfOpenGL.h"
#include "cvfOpenGLResourceManager.h"
#include "cvfShaderProgram.h"
#include "cvfUniform.h"
#include "cvfRenderState.h"
#include "cvfCamera.h"
#include "cvfViewport.h"
#include "cvfBoundingBox.h"
#include "cvfShaderProgramGenerator.h"
#include "cvfShaderSourceProvider.h"
#include "cvfMatrixState.h"

#ifndef CVF_OPENGL_ES
#include "cvfRenderState_FF.h"
#endif

namespace cvf {


//==================================================================================================
///
/// \class cvf::TextDrawer
/// \ingroup Render
///
/// Contains text which can be rendered with a given font
//==================================================================================================

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
TextDrawer::TextDrawer(Font* font)
:   m_font(font),
    m_drawBackground(false),
    m_drawBorder(false),
    m_textColor(Color3::GRAY),
    m_backgroundColor(1.0f, 1.0f, 0.8f),
    m_borderColor(Color3::DARK_GRAY),
    m_verticalAlignment(0)  // BASELINE
{
    CVF_ASSERT(font);
    CVF_ASSERT(!font->isEmpty());
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
TextDrawer::~TextDrawer()
{
}


// --------------------------------------------------------------------------------------------------
/// Add text to be drawn
// --------------------------------------------------------------------------------------------------
void TextDrawer::addText(const String& text, const Vec2f& pos)
{
    CVF_ASSERT(!text.isEmpty());
    CVF_ASSERT(!pos.isUndefined());

    m_texts.push_back(text);
    m_positions.push_back(Vec3f(pos));
}


//--------------------------------------------------------------------------------------------------
/// Remove all texts in the drawer
//--------------------------------------------------------------------------------------------------
void TextDrawer::removeAllTexts()
{
    m_texts.clear();
    m_positions.clear();
}


//--------------------------------------------------------------------------------------------------
/// Set color of the text to be rendered
//--------------------------------------------------------------------------------------------------
void TextDrawer::setTextColor(const Color3f& color)
{
    m_textColor = color;
}


//--------------------------------------------------------------------------------------------------
/// Set color of the text to be rendered
//--------------------------------------------------------------------------------------------------
void TextDrawer::setBackgroundColor(const Color3f& color)
{
    m_backgroundColor = color;
}


//--------------------------------------------------------------------------------------------------
/// Set color of the text to be rendered
//--------------------------------------------------------------------------------------------------
void TextDrawer::setBorderColor(const Color3f& color)
{
    m_borderColor = color;
}


//--------------------------------------------------------------------------------------------------
/// Set vertical alignment for horizontal text
//--------------------------------------------------------------------------------------------------
void TextDrawer::setVerticalAlignment(TextDrawer::Alignment alignment)
{
    CVF_ASSERT(m_font.notNull());
    CVF_ASSERT(!m_font->isEmpty());

    switch (alignment)
    {
        case TextDrawer::TOP:
        {
            // Character assumed to reach all the way up
            ref<Glyph> glyph_top = m_font->getGlyph(L'`');
            m_verticalAlignment = static_cast<short>(-glyph_top->horizontalBearingY());
            break;
        }

        case TextDrawer::CENTER:
        {
            // Center around A
            ref<Glyph> glyph_top = m_font->getGlyph(L'A');
            m_verticalAlignment = static_cast<short>(-((glyph_top->horizontalBearingY() + 1) >> 1));
            break;
        }

        case TextDrawer::BASELINE:
        {
            m_verticalAlignment = 0;
            break;
        }

        case TextDrawer::BOTTOM:
        {
            // Character assumed to reach all the way down
            ref<Glyph> glyph_bottom = m_font->getGlyph(L'g');
            m_verticalAlignment = static_cast<short>(static_cast<short>(glyph_bottom->height()) + glyph_bottom->horizontalBearingY());
            break;
        }

        default:
        {
            CVF_FAIL_MSG("Unsupported alignment type");
            break;
        }
    }
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void TextDrawer::setDrawBackground(bool drawBackground)
{
    m_drawBackground = drawBackground;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void TextDrawer::setDrawBorder(bool drawBorder)
{
    m_drawBorder = drawBorder;
}


//--------------------------------------------------------------------------------------------------
/// Draw text based using OpenGL shader programs
//--------------------------------------------------------------------------------------------------
void TextDrawer::render(OpenGLContext* oglContext, const MatrixState& matrixState)
{
    doRender2d(oglContext, matrixState, false);
}


//--------------------------------------------------------------------------------------------------
/// Draw text based using OpenGL shader programs
//--------------------------------------------------------------------------------------------------
void TextDrawer::renderSoftware(OpenGLContext* oglContext, const MatrixState& matrixState)
{
    doRender2d(oglContext, matrixState, true);
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void TextDrawer::doRender2d(OpenGLContext* oglContext, const MatrixState& matrixState, bool softwareRendering)
{
    CVF_CALLSITE_OPENGL(oglContext);
    CVF_ASSERT(m_positions.size() == m_texts.size());

    static const ushort connects[]     = { 0, 1, 2, 0, 2, 3 };
    static const ushort lineConnects[] = { 0, 1, 1, 2, 2, 3, 3, 0 };

    float vertexArray[12];
    float textureCoords[8];                 // Will be updated for each glyph 

    float* v1 = &vertexArray[0]; 
    float* v2 = &vertexArray[3];
    float* v3 = &vertexArray[6];
    float* v4 = &vertexArray[9];
    v1[2] = v2[2] = v3[2] = v4[2] = 0.0f;

    // Prepare 2D pixel exact projection to draw texts
    Camera projCam;
    projCam.setViewport(matrixState.viewportPosition().x(), matrixState.viewportPosition().y(), matrixState.viewportSize().x(), matrixState.viewportSize().y());
    projCam.setProjectionAsPixelExact2D();
    projCam.setViewMatrix(Mat4d::IDENTITY);

    MatrixState projMatrixState(projCam);

    // Turn off depth test
    Depth depth(false, Depth::LESS, false);
    depth.applyOpenGL(oglContext);

    // Setup viewport
    projCam.viewport()->applyOpenGL(oglContext, Viewport::DO_NOT_CLEAR);

    if (softwareRendering)
    {
        if (ShaderProgram::supportedOpenGL(oglContext))
        {
            ShaderProgram::useNoProgram(oglContext);
        }

#ifndef CVF_OPENGL_ES
        Material_FF mat;
        mat.enableColorMaterial(true);
        mat.applyOpenGL(oglContext);

        Lighting_FF light(false);
        light.applyOpenGL(oglContext);
#endif
        projCam.applyOpenGL();
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glEnableVertexAttribArray(ShaderProgram::VERTEX);
        glVertexAttribPointer(ShaderProgram::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, vertexArray);
    }

    // Render background and border
    // -------------------------------------------------------------------------
    if (m_drawBackground || m_drawBorder)
    {
        ref<ShaderProgram> backgroundShader;

        if (!softwareRendering)
        {
            backgroundShader = oglContext->resourceManager()->getLinkedUnlitColorShaderProgram(oglContext);
            if (backgroundShader->useProgram(oglContext))
            {
                backgroundShader->clearUniformApplyTracking();
                backgroundShader->applyFixedUniforms(oglContext, projMatrixState);
            }
        }

        // Figure out margin
        ref<Glyph> glyph = m_font->getGlyph(L'A');
        float charHeight = static_cast<float>(glyph->height());
        float charWidth  = static_cast<float>(glyph->width());

        size_t numTexts = m_texts.size();
        size_t i, j;
        for (i = 0; i < numTexts; i++)
        {
            Vec3f pos  = m_positions[i];
            String text = m_texts[i];
            BoundingBox textBB;

            // Cursor incrementor
            Vec2f cursor(pos);
            size_t numCharacters = text.size();

            for (j = 0; j < numCharacters; j++)
            {
                wchar_t character = text[j];
                ref<Glyph> glyph = m_font->getGlyph(character);

                float textureWidth = static_cast<float>(glyph->width());
                float textureHeight = static_cast<float>(glyph->height());

                // Lower left corner
                v1[0] = cursor.x() + static_cast<float>(glyph->horizontalBearingX());
                v1[1] = cursor.y() + static_cast<float>(glyph->horizontalBearingY()) - textureHeight + static_cast<float>(m_verticalAlignment);

                // Lower right corner
                v2[0] = v1[0] + textureWidth;
                v2[1] = v1[1];

                // Upper right corner
                v3[0] = v2[0];
                v3[1] = v1[1] + textureHeight;

                // Upper left corner
                v4[0] = v1[0];
                v4[1] = v3[1];

                textBB.add(Vec3f(v1[0], v1[1], 0.0f));
                textBB.add(Vec3f(v2[0], v2[1], 0.0f));
                textBB.add(Vec3f(v3[0], v3[1], 0.0f));
                textBB.add(Vec3f(v4[0], v4[1], 0.0f));

                // Jump to the next character in the string, if any
                if (j < (numCharacters - 1))
                {
                    float advance = static_cast<float>(m_font->advance(character, text[j + 1]));
                    cursor.x() += advance;
                }
            }

            Vec3f min = Vec3f(textBB.min());
            Vec3f max = Vec3f(textBB.max());
            min.x() -= charWidth*0.5f;
            max.x() += charWidth*0.5f;
            min.y() -= charHeight*0.5f;
            max.y() += charHeight*0.5f;

            // Draw the background triangle
            v1[0] = min.x(); v1[1] = min.y();
            v2[0] = max.x(); v2[1] = min.y();
            v3[0] = max.x(); v3[1] = max.y();
            v4[0] = min.x(); v4[1] = max.y();

            if (m_drawBackground)
            {
                if (softwareRendering)
                {
#ifndef CVF_OPENGL_ES
                    glColor4fv(m_backgroundColor.ptr());
                    glBegin(GL_TRIANGLE_FAN);
                    glVertex3fv(v1);
                    glVertex3fv(v2);
                    glVertex3fv(v3);
                    glVertex3fv(v4);
                    glEnd();
#endif
                }
                else
                {
                    UniformFloat backgroundColor("u_color", Color4f(m_backgroundColor));
                    backgroundShader->applyUniform(oglContext, backgroundColor);

                    // Draw background
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, connects);
                }
            }

            if (m_drawBorder)
            {
                if (softwareRendering)
                {
#ifndef CVF_OPENGL_ES
                    glColor3fv(m_borderColor.ptr());
                    glBegin(GL_LINE_LOOP);
                    glVertex3fv(v1);
                    glVertex3fv(v2);
                    glVertex3fv(v3);
                    glVertex3fv(v4);
                    glEnd();
#endif
                }
                else
                {
                    UniformFloat borderColor("u_color", Color4f(m_borderColor));
                    backgroundShader->applyUniform(oglContext, borderColor);

                    // Draw border
                    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, lineConnects);
                }
            }
        }
    }

    // Render texts
    // -------------------------------------------------------------------------
    if (softwareRendering)
    {
#ifndef CVF_OPENGL_ES
        glEnable(GL_TEXTURE_2D);
        glColor3fv(m_textColor.ptr());
#endif
    }
    else
    {
        ref<ShaderProgram> textShader = oglContext->resourceManager()->getLinkedTextShaderProgram(oglContext);
        if (textShader->useProgram(oglContext))
        {
            textShader->clearUniformApplyTracking();
            textShader->applyFixedUniforms(oglContext, projMatrixState);

            UniformFloat uniformColor("u_color", m_textColor);
            textShader->applyUniform(oglContext, uniformColor);

            UniformInt uniformTexture("u_texture2D", 0);
            textShader->applyUniform(oglContext, uniformTexture);
        }

        glEnableVertexAttribArray(ShaderProgram::TEX_COORD_2F_0);
        glVertexAttribPointer(ShaderProgram::TEX_COORD_2F_0, 2, GL_FLOAT, GL_FALSE, 0, textureCoords);

        // Setup texture, Note: Each glyph will do additional setup
        glActiveTexture(GL_TEXTURE0);
    }

    Blending blending;
    blending.configureTransparencyBlending();
    blending.applyOpenGL(oglContext);

    wchar_t character;
    size_t numCharacters;
    size_t numTexts = m_texts.size();

    size_t i, j;
    for (i = 0; i < numTexts; i++)
    {
        Vec3f pos  = m_positions[i];
        String text = m_texts[i];
        
        // Need to round off to integer positions to avoid buggy text drawing on iPad2
        pos.x() = cvf::Math::floor(pos.x());
        pos.y() = cvf::Math::floor(pos.y());
        pos.z() = cvf::Math::floor(pos.z());

        // Cursor incrementor
        Vec2f cursor(pos);

        numCharacters = text.size();

        for (j = 0; j < numCharacters; j++)
        {
            character = text[j];
            ref<Glyph> glyph = m_font->getGlyph(character);

            float textureWidth = static_cast<float>(glyph->width());
            float textureHeight = static_cast<float>(glyph->height());

            // Lower left corner
            v1[0] = cursor.x() + static_cast<float>(glyph->horizontalBearingX());
            v1[1] = cursor.y() + static_cast<float>(glyph->horizontalBearingY()) - textureHeight + static_cast<float>(m_verticalAlignment);

            // Lower right corner
            v2[0] = v1[0] + textureWidth;
            v2[1] = v1[1];

            // Upper right corner
            v3[0] = v2[0];
            v3[1] = v1[1] + textureHeight;

            // Upper left corner
            v4[0] = v1[0];
            v4[1] = v3[1];

            glyph->setupAndBindTexture(oglContext, softwareRendering);

            // Get texture coordinates
            const FloatArray* textureCoordinates = glyph->textureCoordinates();
            CVF_ASSERT(textureCoordinates);
            CVF_ASSERT(textureCoordinates->size() == 8);
            const float* textureCoordinatesPtr = textureCoordinates->ptr();
            CVF_ASSERT(textureCoordinatesPtr);
            int t;
            for (t = 0; t < 8; t++)
            {
                textureCoords[t] = textureCoordinatesPtr[t];
            }

            if (softwareRendering)
            {
#ifndef CVF_OPENGL_ES
                glBegin(GL_TRIANGLES);

                // First triangle in quad
                glTexCoord2fv(&textureCoordinatesPtr[0]);
                glVertex3fv(v1);
                glTexCoord2fv(&textureCoordinatesPtr[2]);
                glVertex3fv(v2);
                glTexCoord2fv(&textureCoordinatesPtr[4]);
                glVertex3fv(v3);

                // Second triangle in quad
                glTexCoord2fv(&textureCoordinatesPtr[0]);
                glVertex3fv(v1);
                glTexCoord2fv(&textureCoordinatesPtr[4]);
                glVertex3fv(v3);
                glTexCoord2fv(&textureCoordinatesPtr[6]);
                glVertex3fv(v4);

                glEnd();
#endif
            }
            else
            {
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, connects);
            }

            // Jump to the next character in the string, if any
            if (j < (numCharacters - 1))
            {
                float advance = static_cast<float>(m_font->advance(character, text[j + 1]));
                cursor.x() += advance;
            }
        }
    }

    if (softwareRendering)
    {
#ifndef CVF_OPENGL_ES
        // apply the projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(matrixState.projectionMatrix().ptr());

        // apply the view matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(matrixState.viewMatrix().ptr());

        Material_FF mat;
        mat.applyOpenGL(oglContext);

        Lighting_FF light;
        light.applyOpenGL(oglContext);
#endif
    }
    else    
    {
        glDisableVertexAttribArray(ShaderProgram::TEX_COORD_2F_0);
        glDisableVertexAttribArray(ShaderProgram::VERTEX);

        ShaderProgram::useNoProgram(oglContext);
    }

    // Reset render states
    Blending resetBlending;
    resetBlending.applyOpenGL(oglContext);

    // Turn off depth test
    Depth resetDepth;
    resetDepth.applyOpenGL(oglContext);

    CVF_CHECK_OGL(oglContext);
}

}  // namespace cvf