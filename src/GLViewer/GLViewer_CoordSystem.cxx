// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  Author : OPEN CASCADE
// File:      GLViewer_Context.cxx
// Created:   November, 2004
//
#include "GLViewer_CoordSystem.h"
#include <math.h>

/*!
  Constructor
  \param aType - type of CS
  \param X0 - X of origin in reference CS
  \param Y0 - Y of origin in reference CS
  \param XUnit - X unit in reference CS
  \param YUnit - Y unit in reference CS
  \param Rotation - rotation relative reference CS
*/
GLViewer_CoordSystem::GLViewer_CoordSystem( CSType aType, double X0, double Y0, 
                                            double XUnit, double YUnit, double Rotation )
{
    setType( aType );
    setOrigin( X0, Y0 );
    setUnits( XUnit, YUnit );
    setRotation( Rotation );
}

/*!
  \return origin in reference CS
*/
void GLViewer_CoordSystem::getOrigin( double& x, double& y ) const
{
    x = myX0;
    y = myY0;
}

/*!
  Sets origin in reference CS
*/
void GLViewer_CoordSystem::setOrigin( double x, double y )
{
    myX0 = x;
    myY0 = y;
}

/*!
  \return units
*/
void GLViewer_CoordSystem::getUnits( double& x, double& y ) const
{
    x = myXUnit;
    y = myYUnit;
}

/*!
  Sets units
*/
void GLViewer_CoordSystem::setUnits( double x, double y )
{
    if( x>0 )
        myXUnit = x;
    else
        myXUnit = 1.0;

    if( y>0 )
        myYUnit = y;
    else
        myYUnit = 1.0;
}

/*!
  \return rotation
*/
double GLViewer_CoordSystem::getRotation() const
{
    return myRotation;
}

/*!
  Sets rotation
*/
void GLViewer_CoordSystem::setRotation( double rotation )
{
    myRotation = rotation;
}

/*!
  \return type
*/
GLViewer_CoordSystem::CSType GLViewer_CoordSystem::getType() const
{
    return myType;
}

/*!
  Sets type
*/
void GLViewer_CoordSystem::setType( CSType type )
{
    myType = type;
}

/*!
  Recalculate co-ordinates to reference co-ordinates
  \param x, y - co-ordinates
*/
void GLViewer_CoordSystem::toReference( double& x, double& y )
{
    if( myType==Cartesian )
    {
        double newx = myX0 + myXUnit*x*cos(myRotation) - myYUnit*y*sin(myRotation),
               newy = myY0 + myXUnit*x*sin(myRotation) + myYUnit*y*cos(myRotation);
        x = newx;
        y = newy;
    }
    else if( myType==Polar )
    {
        double r = x, phi = y;
        x = myX0 + myXUnit*r*cos(phi+myRotation);
        y = myY0 + myXUnit*r*sin(phi+myRotation);
    }
}

/*!
  Recalculate co-ordinates from reference co-ordinates
  \param x, y - co-ordinates
*/
void GLViewer_CoordSystem::fromReference( double& x, double& y )
{
    x = (x - myX0) / myXUnit;
    y = (y - myY0) / myYUnit;

    if( myType==Cartesian )
    {
        double newx =  x*cos(myRotation) + y*sin(myRotation),
               newy = -x*sin(myRotation) + y*cos(myRotation);
        x = newx;
        y = newy;
    }
    else if( myType==Polar )
    {
        double r = sqrt( x*x+y*y );
        double phi = 0.0;
        double eps = 1E-8, pi = 3.14159265;

        if( r>eps )
        {
            if( fabs(x)>eps )
            {
                double arg = y/x;
                phi = atan(arg);
                if( x<0 ) // 2-nd or 4-rd quarter
                    phi+=pi;
            }
            else if( y>0 )
                phi = pi/2.0;
            else
                phi = 3*pi/2.0;
        }

        x = r;
        y = phi-myRotation;
    }
}

/*!
  Recalculate co-ordinates to co-ordinates of other CS
  \param aSystem - other CS
  \param x, y - co-ordinates
*/
void GLViewer_CoordSystem::transform( GLViewer_CoordSystem& aSystem, double& x, double& y )
{
    toReference( x, y );
    aSystem.fromReference( x, y );
}

/*!
  \return stretching of CS along X and Y axis
*/
void GLViewer_CoordSystem::getStretching( GLViewer_CoordSystem& aSystem, double& theX, double& theY )
{
    theX = myXUnit / aSystem.myXUnit;
    theY = myYUnit / aSystem.myYUnit;
}
