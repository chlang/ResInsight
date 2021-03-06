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
#include "cvfAssert.h"
#include "cvfMath.h"
#include "cvfPrimitiveSetIndexedUShortScoped.h"
#include "cvfOpenGL.h"
#include "cvfOpenGLResourceManager.h"
#include "cvfBufferObjectManaged.h"

namespace cvf {



//==================================================================================================
///
/// \class cvf::PrimitiveSetIndexedUShortScoped
/// \ingroup Render
///
/// 
///
//==================================================================================================

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
PrimitiveSetIndexedUShortScoped::PrimitiveSetIndexedUShortScoped(PrimitiveType primitiveType)
:   PrimitiveSet(primitiveType),
    m_firstElement(0),
    m_elementCount(0)
{
}


//--------------------------------------------------------------------------------------------------
/// Deletes OpenGL resources created by this primitive set
//--------------------------------------------------------------------------------------------------
PrimitiveSetIndexedUShortScoped::~PrimitiveSetIndexedUShortScoped()
{
    releaseBufferObjectsGPU();
}


//--------------------------------------------------------------------------------------------------
/// Render primitives in this primitive set using vertex arrays
/// 
/// \warning Requires at least OpenGL 1.5
//--------------------------------------------------------------------------------------------------
void PrimitiveSetIndexedUShortScoped::render(OpenGLContext* oglContext) const
{
    CVF_CALLSITE_OPENGL(oglContext);
    CVF_ASSERT(BufferObjectManaged::supportedOpenGL(oglContext));
    
    if (m_indices.isNull())
    {
        return;
    }

    GLsizei numIndices = static_cast<GLsizei>(m_elementCount);
    if (numIndices <= 0) 
    {
        return;
    }

    const GLvoid* ptrOrOffset = 0;
    if (m_indicesBO.notNull() && m_indicesBO->isUploaded())
    {
        m_indicesBO->bindBuffer(oglContext);
        ptrOrOffset = reinterpret_cast<GLvoid*>(m_firstElement*sizeof(GLushort));
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        ptrOrOffset = m_indices->ptr() + m_firstElement;
    }
    
    glDrawElements(primitiveTypeOpenGL(), numIndices, GL_UNSIGNED_SHORT, ptrOrOffset);

    CVF_CHECK_OGL(oglContext);
}


//--------------------------------------------------------------------------------------------------
/// Create buffer object and upload data to the GPU
/// 
/// \warning The current render context must support buffer objects (OGL version >= 1.5)
//--------------------------------------------------------------------------------------------------
void PrimitiveSetIndexedUShortScoped::createUploadBufferObjectsGPU(OpenGLContext* oglContext)
{
    CVF_TIGHT_ASSERT(oglContext);
    CVF_TIGHT_ASSERT(BufferObjectManaged::supportedOpenGL(oglContext));

    // Buffer object already in place?
    if (m_indicesBO.notNull() && m_indicesBO->isUploaded())
    {
        return;
    }

    if (m_indices.notNull())
    {
        size_t numIndices = m_indices->size();
        if (numIndices > 0) 
        {
            GLuint uiSizeInBytes = static_cast<GLuint>(numIndices*sizeof(GLushort));
            m_indicesBO = oglContext->resourceManager()->getOrCreateManagedBufferObject(oglContext, GL_ELEMENT_ARRAY_BUFFER, uiSizeInBytes, m_indices->ptr());
            CVF_CHECK_OGL(oglContext);
        }
    }
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void PrimitiveSetIndexedUShortScoped::releaseBufferObjectsGPU()
{
    m_indicesBO = NULL;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void PrimitiveSetIndexedUShortScoped::setIndices(UShortArray* indices, size_t firstElement, size_t elementCount)
{
    if (indices)
    {
        CVF_ASSERT(firstElement < indices->size());
        CVF_ASSERT(firstElement + elementCount <= indices->size());
        m_indices = indices;
        m_firstElement = firstElement;
        m_elementCount = elementCount;
    }
    else
    {
        CVF_ASSERT(firstElement == 0);
        CVF_ASSERT(elementCount == 0);
        m_indices = NULL;
        m_firstElement = 0;
        m_elementCount = 0;
    }

    m_indicesBO = NULL;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void PrimitiveSetIndexedUShortScoped::setScope(size_t firstElement, size_t elementCount)
{
    if (m_indices.notNull())
    {
        CVF_ASSERT(firstElement < m_indices->size());
        CVF_ASSERT(firstElement + elementCount <= m_indices->size());
        m_firstElement = firstElement;
        m_elementCount = elementCount;
    }
    else
    {
        CVF_ASSERT(firstElement == 0);
        CVF_ASSERT(elementCount == 0);
    }
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
size_t PrimitiveSetIndexedUShortScoped::scopeFirstElement() const
{
    return m_firstElement;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
size_t PrimitiveSetIndexedUShortScoped::scopeElementCount() const
{
    return m_elementCount;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
const UShortArray* PrimitiveSetIndexedUShortScoped::indices() const
{
    return m_indices.p();
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
size_t PrimitiveSetIndexedUShortScoped::indexCount() const
{
    return m_elementCount;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
uint PrimitiveSetIndexedUShortScoped::index(size_t i) const
{
    CVF_TIGHT_ASSERT(m_indices.notNull());
    return m_indices->get(m_firstElement + i);
}


} // namespace cvf

