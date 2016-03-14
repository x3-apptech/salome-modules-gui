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
// File:      GLViewer_CoordSystem.h
// Created:   November, 2004
//
/*! Class GLViewer_CoordSystem
 *  Class implementing mathematical model of 2D coordinate system 
 */

#ifndef GLVIEWER_COORDSYSTEM_H
#define GLVIEWER_COORDSYSTEM_H

#include "GLViewer.h"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  \class GLViewer_CoordSystem
  \brief Class implementing mathematical model of 2D coordinate system 
*/
class GLVIEWER_API GLViewer_CoordSystem
{
public:
  //! A type of coordinate system
  enum CSType
  {
    Cartesian,
    Polar     
  };
  
private:
  //! The coordinates of origin in the reference CS
  double myX0, myY0;
  //! The lengths of axis units in the reference unit
  double myXUnit, myYUnit;
  //! The rotation in radians relative to reference CS
  double myRotation;
  
  //! In the polar CS myYUnit is ignored, but myXUnit is the unit of polar radius  
  CSType myType;
  
public:
  //! A constructor ( by default new system is identical to reference )
  GLViewer_CoordSystem( CSType aType, double X0 = 0.0, double Y0 = 0.0, 
                        double XUnit = 1.0, double YUnit = 1.0, double Rotation = 0.0 );
  
  //! Returns the origin in reference system
  void getOrigin( double& x, double& y ) const;
  //! A function installing the origin in reference system
  void setOrigin( double x, double y );
  
  //! Returns units along axes
  void getUnits( double& x, double& y ) const;
  //! A function installing units along axes
  void setUnits( double x, double y );
  
  //! Returns rotation angle of axes in reference system
  double getRotation() const;
  //! A function installing rotation angle of axes in reference system
  void   setRotation( double rotation );
  
  //! Returns type of system
  CSType getType() const;
  //! A function installing type of system
  void setType( CSType type );
  
  //! Transform the coordinates x, y from current CS to aSystem
  void transform( GLViewer_CoordSystem& aSystem, double& x, double& y );
  
  
  //! Return how many times line width in aSystem system bigger than in current
  virtual void getStretching( GLViewer_CoordSystem& aSystem, double& theX, double& theY );
  
  
protected:
  //! A function transforms system coords to coords in reference system
  virtual void toReference( double& x, double& y );
  //! A function transforms from coords in reference system to system coords
  virtual void fromReference( double& x, double& y );
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
