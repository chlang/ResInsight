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
#include "cvfBase.h"
#include "cvfObject.h"
#include "cvfLegendScalarMapper.h"

namespace cvf {

//==================================================================================================
//
// Maps scalar values to texture coordinates/colors
//
//==================================================================================================
class ScalarMapperContinuousLog : public LegendScalarMapper
{
public:
    ScalarMapperContinuousLog();

    void                setRange(double min, double max);
    void                setMajorLevelCount(size_t levelCount, bool adjustLevels = true);

    void                setColors(const Color3ubArray& colorArray);
    void                setColors(ColorTable colorTable);

    // Scalarmapper interface
    virtual Vec2f       mapToTextureCoord(double scalarValue) const;
    virtual Color3ub    mapToColor(double scalarValue) const;

    virtual bool        updateTexture(TextureImage* image) const;

    // LegendScalarmapper interface

    virtual void        majorLevels(std::vector<double>* domainValues ) const;
    virtual double      normalizedLevelPosition( double domainValue ) const;
    virtual double      domainValue( double normalizedPosition ) const;

protected:
    double          m_rangeMin;
    double          m_rangeMax;
    unsigned int    m_decadeLevelCount;

private:
    size_t          m_majorLevelCount;
    bool            m_adjustLevels;

    Color3ubArray   m_colors;
    uint            m_textureSize;      // The size of texture that updateTexture() will produce. 
};

}