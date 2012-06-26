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

#pragma once

#include "cvfArray.h"

namespace cvf {

//=================================================================================================
//
// Triangle Vertex Splitter Class
//
//=================================================================================================
class TriangleVertexSplitter
{
public:
    TriangleVertexSplitter(double creaseAngle, const UIntValueArray& origTriangleIndices, const Vec3fValueArray& origVertexArray);

    ref<UIntArray>     triangleIndices();
    ref<Vec3fArray>    vertexArray();
    ref<Vec3fArray>    vertexNormals();
    ref<UIntArray>     perVertexOriginalIndices();        // Per vertex (source) indices into origVertexArray

private:
    void                splitVertices();
    uint                processVertex(uint origVertexIndex, const Vec3f& faceNormal);
    bool                isNormalDifferenceBelowThreshold(const Vec3f& n1, const Vec3f& n2);

private:
    double                  m_creaseAngle;
    const UIntValueArray&   m_origTriangleIndices;
    const Vec3fValueArray&  m_origVertexArray;

    bool                    m_isComputed;
    ref<UIntArray>          m_triangleIndices;
    UIntArray               m_origToUsedNodeMap;
    std::vector<uint>       m_nextSplitVertexIdx;
    std::vector<Vec3f>      m_vertexArray;
    std::vector<Vec3f>      m_normalArray;
};

}