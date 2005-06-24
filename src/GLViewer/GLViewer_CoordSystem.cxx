/***************************************************************************
**  Class:   GLViewer_CoordSystem
**  Descr:   
**  Module:  GLViewer
**  Created: UI team, 03.09.02
****************************************************************************/

//#include <GLViewerAfx.h>
#include "GLViewer_CoordSystem.h"
#include <math.h>

GLViewer_CoordSystem::GLViewer_CoordSystem( CSType aType, double X0, double Y0, 
                                            double XUnit, double YUnit, double Rotation )
{
    setType( aType );
    setOrigin( X0, Y0 );
    setUnits( XUnit, YUnit );
    setRotation( Rotation );
}

void GLViewer_CoordSystem::getOrigin( double& x, double& y ) const
{
    x = myX0;
    y = myY0;
}

void GLViewer_CoordSystem::setOrigin( double x, double y )
{
    myX0 = x;
    myY0 = y;
}

void GLViewer_CoordSystem::getUnits( double& x, double& y ) const
{
    x = myXUnit;
    y = myYUnit;
}

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

double GLViewer_CoordSystem::getRotation() const
{
    return myRotation;
}

void GLViewer_CoordSystem::setRotation( double rotation )
{
    myRotation = rotation;
}

GLViewer_CoordSystem::CSType GLViewer_CoordSystem::getType() const
{
    return myType;
}

void GLViewer_CoordSystem::setType( CSType type )
{
    myType = type;
}

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

        x = r;
        y = phi-myRotation;
    }
}

void GLViewer_CoordSystem::transform( GLViewer_CoordSystem& aSystem, double& x, double& y )
{
    toReference( x, y );
    aSystem.fromReference( x, y );
}

void GLViewer_CoordSystem::getStretching( GLViewer_CoordSystem& aSystem, double& theX, double& theY )
{
    theX = myXUnit / aSystem.myXUnit;
    theY = myYUnit / aSystem.myYUnit;
}
