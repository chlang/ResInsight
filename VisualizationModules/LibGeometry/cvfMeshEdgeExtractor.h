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



//==================================================================================================
//
// 
//
//==================================================================================================
class MeshEdgeExtractor
{
public:
    MeshEdgeExtractor();

    void            addPrimitives(uint verticesPerPrimitive, const uint* indices, size_t indexCount);
    void            addPrimitives(uint verticesPerPrimitive, const UIntArray& indices);
    void            addFaceList(const UIntArray& faceList);

    ref<UIntArray>  lineIndices() const;

private:
    std::set<cvf::int64> m_edgeSet;
};


}


