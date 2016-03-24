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

#ifndef SUIT_CAMERAPROPERITES_H
#define SUIT_CAMERAPROPERITES_H

#include "SUIT.h"

class SUIT_EXPORT SUIT_CameraProperties
{
public:
  enum Dimension  { DimNone, Dim2D, Dim3D };
  enum ViewSide   { ViewNone, ViewXY, ViewXZ, ViewYZ };
  enum Projection { PrjOrthogonal, PrjPerspective };

  SUIT_CameraProperties();
  virtual ~SUIT_CameraProperties();

  bool       isValid() const;
  bool       isCompatible( const SUIT_CameraProperties& other );

  Dimension  getDimension() const;
  void       setDimension( const Dimension theDimension );

  ViewSide   getViewSide() const;
  void       setViewSide( const ViewSide theViewSide );

  Projection getProjection() const;
  void       setProjection( const Projection theProjection );

  void       getViewUp(double& theX, double& theY, double& theZ) const;
  void       setViewUp(const double theX, const double theY, const double theZ);

  void       getPosition(double& theX, double& theY, double& theZ) const;
  void       setPosition(const double theX, const double theY, const double theZ);

  void       getFocalPoint(double& theX, double& theY, double& theZ) const;
  void       setFocalPoint(const double theX, const double theY, const double theZ);

  double     getMappingScale() const;
  void       setMappingScale(const double theScale);

  void       getAxialScale(double& theScaleX, double& theScaleY, double& theScaleZ);
  void       setAxialScale(const double theScaleX, const double theScaleY, const double theScaleZ);

  double     getViewAngle() const;
  void       setViewAngle(const double theViewAngle);
  
// common properties for all viewers
private:
  Dimension  myDimension;        //!< dimension
  ViewSide   myViewSide;         //!< side view (for 2d viewer)
  Projection myProjection;       //!< projection mode (for 3d viewer): orthogonal / perspective
  double     myUpDir[3];         //!< camera up vector
  double     myPosition[3];      //!< camera position (eye).
  double     myFocalPoint[3];    //!< focal point position in world-space coordinates
  double     myMappingScale;     //!< window mapping scale (parallel projection scale)
  double     myAxialScale[3];    //!< scaling factors for world axes
  double     myViewAngle;        //!< perspective view angle in degrees
};

#endif // SUIT_CAMERAPROPERITES_H
