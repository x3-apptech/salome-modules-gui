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

#include "SUIT_CameraProperties.h"

/*!
  \class SUIT_CameraProperties
  \brief Base class for Camera Properties
  
  This class provides common properties that
  can be used for any arbitrary camera. This
  properties are employed by camera synchronization
  algorithms.
*/

/*!
  \brief Default constructor. 

  Creates invalid camera properties data, i.e. IsValid() returns \c false.
*/
SUIT_CameraProperties::SUIT_CameraProperties()
{
  // init with some default values
  myUpDir[0] = 0.0;
  myUpDir[1] = 1.0;
  myUpDir[2] = 0.0;
  
  myPosition[0] = 0.0;
  myPosition[1] = 0.0;
  myPosition[2] = 500.0;
  
  myFocalPoint[0] = 0.0;
  myFocalPoint[1] = 0.0;
  myFocalPoint[2] = 0.0;
    
  myAxialScale[0] = 1.0;
  myAxialScale[1] = 1.0;
  myAxialScale[2] = 1.0;
  
  myMappingScale = 1000;

  myDimension   = DimNone;        // none dimension by default
  myProjection  = PrjOrthogonal;  // orthogonal projection by default
  myViewSide    = ViewNone;       // no side view by default
  
  myViewAngle = 45.0;
}

/*!
  \brief Destructor
*/
SUIT_CameraProperties::~SUIT_CameraProperties()
{
}

/*!
  \brief Check if camera properties are valid.
  \return \c true if camera properties data is valid.
  \sa setDimension()
*/
bool SUIT_CameraProperties::isValid() const
{
  return myDimension != DimNone;
}

/*!
  \brief Check if this camera properties data is compatible with other camera properties.
  \param other other properties data
  \return \c true if both camera properties sets are compatible
*/
bool SUIT_CameraProperties::isCompatible( const SUIT_CameraProperties& other )
{
  bool result = false;
  // check only valid data and data with same dimensions
  if ( isValid() && other.isValid() && getDimension() == other.getDimension() ) {
    switch( getDimension() ) {
    case SUIT_CameraProperties::Dim2D:
      // two 2d views are compatible if their view side is the same
      result = getViewSide() == other.getViewSide();
      break;
    case SUIT_CameraProperties::Dim3D:
      // two 3d views are compatible if their projection mode is the same
      result = getProjection() == other.getProjection();
      break;
    default:
      break;
    }
  }
  return result;
}

/*!
  \brief get dimension supported by camera.
  \return dimension mode.
  \sa setDimension()
*/
SUIT_CameraProperties::Dimension SUIT_CameraProperties::getDimension() const
{
  return myDimension;
}

/*!
  \brief set dimension supported by camera.
  \param theDimension [in] dimension mode.
  \sa getDimension()
*/
void SUIT_CameraProperties::setDimension( const SUIT_CameraProperties::Dimension theDimension )
{
  myDimension = theDimension;
}

/*!
  \brief get side view supported by camera (for 2d viewer).

  For 2d viewer, side view can be of following values:
  - SUIT_CameraProperties::ViewNone - no side view (for instance, for true 2d viewer)
  - SUIT_CameraProperties::ViewXY   - XY side view of 3d scene
  - SUIT_CameraProperties::ViewXZ   - XZ side view of 3d scene
  - SUIT_CameraProperties::ViewYZ   - YZ side view of 3d scene

  \return side view.
  \sa setViewSide()
*/
SUIT_CameraProperties::ViewSide SUIT_CameraProperties::getViewSide() const
{
  return myViewSide;
}

/*!
  \brief set side view supported by camera (for 2d viewer).

  For 2d viewer, side view can be of following values:
  - SUIT_CameraProperties::ViewNone - no side view (for instance, for true 2d viewer)
  - SUIT_CameraProperties::ViewXY   - XY side view of 3d scene
  - SUIT_CameraProperties::ViewXZ   - XZ side view of 3d scene
  - SUIT_CameraProperties::ViewYZ   - YZ side view of 3d scene

  \param theViewSide [in] view side.
  \sa getViewSide()
*/
void SUIT_CameraProperties::setViewSide( const SUIT_CameraProperties::ViewSide theViewSide )
{
  myViewSide = theViewSide;
}

/*!
  \brief get projection mode supported by camera (for 3d viewer).

  For 3d viewer, projection mode can be of following values:
  - SUIT_CameraProperties::PrjOrthogonal - orthogonal projection
  - SUIT_CameraProperties::PrjPerspective - perspective projection

  \return projection mode.
  \sa setProjection()
*/
SUIT_CameraProperties::Projection SUIT_CameraProperties::getProjection() const
{
  return myProjection;
}

/*!
  \brief set projection mode supported by camera (for 3d viewer).

  For 3d viewer, projection mode can be of following values:
  - SUIT_CameraProperties::PrjOrthogonal - orthogonal projection
  - SUIT_CameraProperties::PrjPerspective - perspective projection

  \param theProjection [in] projection mode.
  \sa getProjection()
*/
void SUIT_CameraProperties::setProjection( const SUIT_CameraProperties::Projection theProjection )
{
  myProjection = theProjection;
}

/*!
  \brief get camera up direction vector.
  \param theX [out] vector's x coordinate in world-coordinates space.
  \param theY [out] vector's y coordinate in world-coordinates space.
  \param theZ [out] vector's z coordinate in world-coordinates space.
  \sa setViewUp()
*/
void SUIT_CameraProperties::getViewUp(double& theX, double& theY, double& theZ) const
{
  theX = myUpDir[0];
  theY = myUpDir[1];
  theZ = myUpDir[2];
}

/*!
  \brief set camera up direction vector.

  It is recommended to set normalized vector coordinates for
  synchronization compatibility.
  
  \param theX [in] vector's x coordinate in world-coordinates space.
  \param theY [in] vector's y coordinate in world-coordinates space.
  \param theZ [in] vector's z coordinate in world-coordinates space.
  \sa getViewUp()
*/
void SUIT_CameraProperties::setViewUp(const double theX, const double theY, const double theZ)
{
  myUpDir[0] = theX;
  myUpDir[1] = theY;
  myUpDir[2] = theZ;
}

/*!
  \brief get camera's position (eye).
  \param theX [out] x coordinate in world-coordinates space.
  \param theY [out] y coordinate in world-coordinates space.
  \param theZ [out] z coordinate in world-coordinates space.
  \sa setPosition()
*/
void SUIT_CameraProperties::getPosition(double& theX, double& theY, double& theZ) const
{
  theX = myPosition[0];
  theY = myPosition[1];
  theZ = myPosition[2];
}

/*!
  \brief get camera's position (eye).
  \param theX [in] x coordinate in world-coordinates space.
  \param theY [in] y coordinate in world-coordinates space.
  \param theZ [in] z coordinate in world-coordinates space.
  \sa getPosition()
*/
void SUIT_CameraProperties::setPosition(const double theX, const double theY, const double theZ)
{
  myPosition[0] = theX;
  myPosition[1] = theY;
  myPosition[2] = theZ;
}

/*!
  \brief get camera's focal point (look point).
  \param theX [out] x coordinate in world-coordinates space.
  \param theY [out] y coordinate in world-coordinates space.
  \param theZ [out] z coordinate in world-coordinates space.
  \sa setFocalPoint()
*/
void SUIT_CameraProperties::getFocalPoint(double& theX, double& theY, double& theZ) const
{
  theX = myFocalPoint[0];
  theY = myFocalPoint[1];
  theZ = myFocalPoint[2];
}

/*!
  \brief set camera's focal point (look point).
  \param theX [in] x coordinate in world-coordinates space.
  \param theY [in] y coordinate in world-coordinates space.
  \param theZ [in] z coordinate in world-coordinates space.
  \sa getFocalPoint()
*/
void SUIT_CameraProperties::setFocalPoint(const double theX, const double theY, const double theZ)
{
  myFocalPoint[0] = theX;
  myFocalPoint[1] = theY;
  myFocalPoint[2] = theZ;
}

/*!
  \brief get window mapping scale (parallel scale).

  Mapping scale defines a mapping scaling factor for the height
  of the viewport in world-coordinate distances.

  \return scaling value.
  \sa setMappingScale()
*/
double SUIT_CameraProperties::getMappingScale() const
{
  return myMappingScale;
}

/*!
  \brief set window mapping scale (parallel scale).

  Mapping scale defines a mapping scaling factor for the height
  of the viewport in world-coordinate distances.

  \param theScale [in] the scaling.
  \sa getMappingScale()
*/
void SUIT_CameraProperties::setMappingScale(const double theScale)
{
  myMappingScale = theScale;
}

/*! 
  \brief get scaling factors for world-coordinate space axes.
  \param theScaleX [out] scale by x coordinate.
  \param theScaleY [out] scale by y coordinate.
  \param theScaleZ [out] scale by z coordinate.
  \sa setAxialScale()
*/
void SUIT_CameraProperties::getAxialScale(double& theScaleX, double& theScaleY, double& theScaleZ)
{
  theScaleX = myAxialScale[0];
  theScaleY = myAxialScale[1];
  theScaleZ = myAxialScale[2];
}

/*!
  \brief set scaling factors for world-coordinate space axes.
  \param theScaleX [in] scale by x coordinate.
  \param theScaleY [in] scale by y coordinate.
  \param theScaleZ [in] scale by z coordinate.
  \sa getAxialScale()
*/
void SUIT_CameraProperties::setAxialScale(const double theScaleX, const double theScaleY, const double theScaleZ)
{ 
  myAxialScale[0] = theScaleX;
  myAxialScale[1] = theScaleY;
  myAxialScale[2] = theScaleZ;
}

/*!
  \brief get angle (typically in degrees) of view for perpective projection mode.
  \return the angle of view.
  \sa setViewAngle()
*/
double SUIT_CameraProperties::getViewAngle() const
{
  return myViewAngle;
}

/*!
  \brief set angle (typically in degrees) of view for perpective projection mode.
  \param theViewAngle [in] the angle of view.
  \sa getViewAngle()
*/
void SUIT_CameraProperties::setViewAngle(const double theViewAngle)
{
  myViewAngle = theViewAngle;
}
