// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "OCCViewer_ClipPlane.h"

#include <gp_Pln.hxx>

/*!
  Constructor of class OrientedPlane
 */
OrientedPlane::OrientedPlane():
  Orientation (0),
  Distance (0.5),
  Rotation1 (0),
  Rotation2 (0)
{
}

/*!
  Operator of another OrientedPlane  assignment.
  @param   other the OrientedPlane to assign.
  @return  the assigned OrientedPlane.
*/
OrientedPlane& OrientedPlane::operator = ( const OrientedPlane& other )
{
  Orientation = other.Orientation;
  Distance = other.Distance;
  Rotation1 = other.Rotation1;
  Rotation2 = other.Rotation2;
  return *this;
}

OCCViewer_ClipPlane::OCCViewer_ClipPlane() :
  RelativeMode(),
  X (0.0), Y (0.0), Z (0.0),
  Dx(1.0), Dy(1.0), Dz(1.0),
  Orientation (0),
  IsInvert (false),
  PlaneMode (Absolute),
  IsOn(true)
{
}

/*!
  Operator of another OCCViewer_ClipPlane assignment.
  @param   other the OCCViewer_ClipPlane to assign.
  @return  the assigned OCCViewer_ClipPlane.
*/
OCCViewer_ClipPlane& OCCViewer_ClipPlane::operator = ( const OCCViewer_ClipPlane& other )
{
  RelativeMode = other.RelativeMode;
  X = other.X;
  Y = other.Y;  
  Z = other.Z;
  Dx = other.Dx;
  Dy = other.Dy;
  Dz = other.Dz;
  Orientation = other.Orientation;
  IsInvert = other.IsInvert;
  PlaneMode = other.PlaneMode;
  IsOn = other.IsOn;
  return *this;
}
