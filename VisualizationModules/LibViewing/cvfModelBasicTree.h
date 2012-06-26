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

#include "cvfModelBasicList.h"

namespace cvf {


//==================================================================================================
//
// A node in a ModelBasicTree model
//
//==================================================================================================
class ModelBasicTreeNode
{
public:
    ModelBasicTreeNode();
    virtual ~ModelBasicTreeNode();

    void                        addPart(Part* part);
    Part*                       part(uint index);
    uint                        partCount() const;
    void                        removePart(Part* part);

    void                        addChild(ModelBasicTreeNode* child);
    ModelBasicTreeNode*         child(uint index);
    const ModelBasicTreeNode*   child(uint index) const;
    uint                         childCount() const;
    void                        removeChild(ModelBasicTreeNode* child);

    void                        findVisibleParts(PartRenderHintCollection* visibleParts, const Camera& camera, const Frustum& frustum, const CullSettings& cullSettings, unsigned int enableMask);
    void                        allParts(Collection<Part>* partCollection);
    virtual void                mergeParts(double maxExtent, unsigned int minimumPrimitiveCount);

    void                        updateBoundingBoxesRecursive();
    const BoundingBox&          boundingBox() const;

    bool                        rayIntersect(const RayIntersectSpec& rayIntersectSpec, HitItemCollection* hitItemCollection);

    Part*                       findPartByID(int id);
    Part*                       findPartByName(String name);

private:
    size_t                      primitiveCount();

private:
    ref<ModelBasicList>                 m_partList;     // Reference to list of parts contained in this node. Null if no parts are present
    std::vector<ModelBasicTreeNode*>    m_children;     // Array of child nodes
    BoundingBox                         m_boundingBox;  // This node's aggregated bounding box
};



//==================================================================================================
//
// A simple model implementation using a tree of parts.
//
//==================================================================================================
class ModelBasicTree : public Model
{
public:
    ModelBasicTree();
    ~ModelBasicTree();

    virtual String              name() const;
    void                        setName(const String& name);

    void                        setRoot(ModelBasicTreeNode* root);
    ModelBasicTreeNode*         root();
    const ModelBasicTreeNode*   root() const;

    void                        removePart(Part* part);

    virtual void                findVisibleParts(PartRenderHintCollection* visibleParts, const Camera& camera, const CullSettings& cullSettings, unsigned int enableMask);
    virtual void                allParts(Collection<Part>* partCollection);
    virtual void                mergeParts(double maxExtent, unsigned int minimumPrimitiveCount);

    virtual void                updateBoundingBoxesRecursive();
    virtual BoundingBox         boundingBox() const;

    virtual bool                rayIntersect(const RayIntersectSpec& rayIntersectSpec, HitItemCollection* hitItemCollection);

    Part*                       findPartByID(int id);
    Part*                       findPartByName(String name);

protected:
    String              m_modelName;
    ModelBasicTreeNode* m_root;
    BoundingBox         m_boundingBox;
};

}