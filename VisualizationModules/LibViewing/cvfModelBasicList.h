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

#include "cvfModel.h"

namespace cvf {

class Frustum;


//==================================================================================================
//
// A simple model implementation using a list of parts.
//
//==================================================================================================
class ModelBasicList : public Model
{
public:
    virtual String          name() const;
    void                    setName(const String& name);

    void                    addPart(Part* part);
    Part*                   part(size_t index);
    size_t                  partCount() const;
    void                    removePart(Part* part);
    void                    removeAllParts();

    virtual void            findVisibleParts(PartRenderHintCollection* visibleParts, const Camera& camera, const CullSettings& cullSettings, unsigned int enableMask);
    virtual void            allParts(Collection<Part>* partCollection);
    virtual void            mergeParts(double maxExtent, unsigned int minimumPrimitiveCount);
    void                    doFindVisibleParts(PartRenderHintCollection* visibleParts, const Camera& camera, const Frustum& frustum, const CullSettings& cullSettings, unsigned int enableMask);

    virtual void            updateBoundingBoxesRecursive();
    virtual BoundingBox     boundingBox() const;

    virtual bool            rayIntersect(const RayIntersectSpec& rayIntersectSpec, HitItemCollection* hitItemCollection);

    Part*                   findPartByID(int64 id);
    Part*                   findPartByName(String name);

private:
    ref<Part>               mergeAndAddPart(Collection<Part>& partCollection) const;

protected:
    String           m_modelName;
    Collection<Part> m_parts;
    BoundingBox      m_boundingBox;
};

}