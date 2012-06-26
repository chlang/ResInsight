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

#include "cvfObject.h"
#include "cvfVector3.h"
#include "cvfMatrix4.h"


namespace cvf {

//=================================================================================================
// 
// Plane class
// 
//=================================================================================================
class Plane : public Object
{
public:
    Plane();
    Plane(double A, double B, double C, double D);
    Plane(const Plane& other);
    ~Plane();

    const Plane&    operator=(const Plane& other);
    bool            operator==(const Plane& other) const;
    bool            operator!=(const Plane& other) const;

    inline double   A() const   { return m_A; }     ///< Get plane coefficient A
    inline double   B() const   { return m_B; }     ///< Get plane coefficient B     
    inline double   C() const   { return m_C; }     ///< Get plane coefficient C
    inline double   D() const   { return m_D; }     ///< Get plane coefficient D

    bool            isValid() const;

    void            set(double A, double B, double C, double D);
    bool            setFromPointAndNormal(const Vec3d& point, const Vec3d& normal);
    bool            setFromPoints(const Vec3d& p1, const Vec3d& p2, const Vec3d& p3);

    Vec3d           normal() const;
    Vec3d           pointInPlane() const;

    void            flip();
    void            transform(const Mat4d& matrix);

    double          distance(const Vec3d& point) const;
    double          distanceSquared(const Vec3d& point) const;
    double          distanceToOrigin() const;

    bool            projectVector(const Vec3d& vector, Vec3d* projectedVector) const;
    Vec3d           projectPoint(const Vec3d& point) const;

    bool            intersect(const Plane& other, Vec3d* point, Vec3d* direction = NULL) const;

private:
    double m_A;     // Plane equation coefficients
    double m_B;     //
    double m_C;     //
    double m_D;     //
};

}