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

#ifndef _OCCViewer_ClipPlane_HeaderFile
#define _OCCViewer_ClipPlane_HeaderFile

#include "OCCViewer.h"

#include <Graphic3d_ClipPlane.hxx>
#include <vector>

/*!
  \class OCCViewer_ClipPlane
         Definition of OCC viewer clipping plane.
*/
class OCCVIEWER_EXPORT OCCViewer_ClipPlane
{
public:

  enum PlaneMode
  {
    Absolute,
    Relative
  };

  enum AbsoluteOrientationType
  {
    AbsoluteCustom,
    AbsoluteXY,
    AbsoluteYZ,
    AbsoluteZX
  };

  enum RelativeOrientationType
  {
    RelativeXY,
    RelativeYZ,
    RelativeZX
  };

public:

  OCCViewer_ClipPlane();
  OCCViewer_ClipPlane& operator =( const OCCViewer_ClipPlane& theOther );

// Parameters
public:

  // Plane definition mode
  PlaneMode Mode;

  // Is active
  bool IsOn;

  // Plane position
  double X;
  double Y;
  double Z;

  // Orientation type
  int OrientationType;

  // Plane orientation (depends on mode)
  union
  {
    struct
    {
      bool   IsInvert;
      double Dx;
      double Dy;
      double Dz;
    } AbsoluteOrientation;

    struct
    {
      double Rotation1;
      double Rotation2;
    } RelativeOrientation;
  };

// Tools
public:

  // Converts defined orientation to direction.
  void OrientationToXYZ( double& theDx, double& theDy, double& theDz ) const;

  // Converts absoulte orientation to relative equivalent.
  static void DXYZToRelative( const double theDx,
                              const double theDy,
                              const double theDz,
                              const int theRelativeType,
                              double& theRotation1,
                              double& theRotation2 );

  // Converts relative orientation to absolute equivalent.
  static void RelativeToDXYZ( const int theRelativeType,
                              const double theRotation1,
                              const double theRotation2,
                              double& theDx,
                              double& theDy,
                              double& theDz );
};

typedef std::vector<OCCViewer_ClipPlane> ClipPlanesList;

#endif
