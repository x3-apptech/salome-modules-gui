// File:      GLViewer_CoordSystem.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_COORDSYSTEM_H
#define GLVIEWER_COORDSYSTEM_H

#include "GLViewer.h"

#ifdef WNT
#pragma warning( disable:4251 )
#endif

// Class:   GLViewer_CoordSystem

class GLVIEWER_API GLViewer_CoordSystem
{
public:
  enum CSType { Cartesian, Polar };
  
private:
  double myX0, myY0;       //The coordinates of origin in the reference CS
  double myXUnit, myYUnit; //The lengths of axis units in the reference unit
  double myRotation;       //The rotation in radians relative to reference CS
  
  //!!! In the polar CS myYUnit is ignored, but myXUnit is the unit of polar radius
  
  CSType myType;
  
public:
  GLViewer_CoordSystem( CSType aType, double X0 = 0.0, double Y0 = 0.0, 
                        double XUnit = 1.0, double YUnit = 1.0, double Rotation = 0.0 );
  
  void getOrigin( double& x, double& y ) const;
  void setOrigin( double x, double y );
  
  void getUnits( double& x, double& y ) const;
  void setUnits( double x, double y );
  
  double getRotation() const;
  void   setRotation( double rotation );
  
  CSType getType() const;
  void setType( CSType type );
  
  void transform( GLViewer_CoordSystem& aSystem, double& x, double& y );
  //Transform the coordinates x, y from current CS to aSystem
  
  virtual void getStretching( GLViewer_CoordSystem& aSystem, double& theX, double& theY );
  //Return how many times line width in aSystem system bigger than in current
  
protected:
  virtual void toReference( double& x, double& y );
  virtual void fromReference( double& x, double& y );
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
