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

//  SALOME OBJECT : kernel of SALOME component
//  File   : SVTK_CubeAxesActor2D.h
//  Author : Eugeny Nikolaev

#ifndef __SVTK_CubeAxesActor2D_h
#define __SVTK_CubeAxesActor2D_h

#include <vtkCubeAxesActor2D.h>
#include <vtkSmartPointer.h>

class vtkActor;
class vtkAxisActor2D;
class vtkCamera;
class vtkDataSet;
class vtkTextProperty;
class vtkPolyDataMapper;
class vtkRectilinearGridGeometryFilter;
class VTKViewer_Transform;

#include "SVTK.h"
#include "VTKViewer.h"

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

#ifndef WIN32
class VTKRENDERINGANNOTATION_EXPORT SVTK_CubeAxesActor2D : public vtkCubeAxesActor2D
#else
class SVTK_EXPORT SVTK_CubeAxesActor2D : public vtkCubeAxesActor2D
#endif
{
public:
  vtkTypeMacro(SVTK_CubeAxesActor2D,vtkCubeAxesActor2D);

  // Description:
  // Instantiate object with bold, italic, and shadow enabled; font family
  // set to Arial; and label format "6.3g". The number of labels per axis
  // is set to 3.
  static SVTK_CubeAxesActor2D *New();
  
  // Description:
  // Draw the axes as per the vtkProp superclass' API.
  int RenderOverlay(vtkViewport*);
  int RenderOpaqueGeometry(vtkViewport*);

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  
  vtkActor* GetWireActorXY(){return this->wireActorXY;}
  vtkActor* GetWireActorYZ(){return this->wireActorYZ;}
  vtkActor* GetWireActorXZ(){return this->wireActorXZ;}

  void SetTransform(VTKViewer_Transform* theTransform);
  VTKViewer_Transform* GetTransform();
  
protected:
  SVTK_CubeAxesActor2D();
  ~SVTK_CubeAxesActor2D();

  vtkActor* wireActorXY;
  vtkActor* wireActorYZ;
  vtkActor* wireActorXZ;

private:
  // hide the superclass' ShallowCopy() from the user and the compiler.
  void ShallowCopy(vtkProp *prop) { this->vtkProp::ShallowCopy( prop ); };

  vtkRectilinearGridGeometryFilter *planeXY; // rectilinear grid XY
  vtkPolyDataMapper *rgridMapperXY;
  
  vtkRectilinearGridGeometryFilter *planeYZ; // rectilinear grid YZ
  vtkPolyDataMapper *rgridMapperYZ;

  vtkRectilinearGridGeometryFilter *planeXZ; // rectilinear grid XZ
  vtkPolyDataMapper *rgridMapperXZ;
  
  vtkSmartPointer<VTKViewer_Transform> m_Transform;
private:
  SVTK_CubeAxesActor2D(const SVTK_CubeAxesActor2D&);  // Not implemented.
  void operator=(const SVTK_CubeAxesActor2D&);  // Not implemented.
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
