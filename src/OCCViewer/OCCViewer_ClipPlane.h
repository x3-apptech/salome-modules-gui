// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

enum ClipPlaneMode { Absolute, Relative };

/*!
  \class OrientedPlane
  \brief Parameters of clipping plane in relative mode
*/
class OCCVIEWER_EXPORT OrientedPlane {

 public:
  OrientedPlane();
  OrientedPlane& operator =(const OrientedPlane& other);

 public:
  Standard_Integer Orientation;
  Standard_Real Distance;
  Standard_Real Rotation1;
  Standard_Real Rotation2;  
};

class OCCVIEWER_EXPORT OCCViewer_ClipPlane
{
  
 public:
  OCCViewer_ClipPlane();
  OCCViewer_ClipPlane& operator =(const OCCViewer_ClipPlane& other);
  
 public:
  OrientedPlane RelativeMode;
  Standard_Real X;
  Standard_Real Y;
  Standard_Real Z;
  Standard_Real Dx;
  Standard_Real Dy;
  Standard_Real Dz;
  Standard_Integer Orientation;
  bool IsInvert;
  bool IsOn;
  ClipPlaneMode PlaneMode;

};

typedef std::vector<OCCViewer_ClipPlane> ClipPlanesList;

#endif
