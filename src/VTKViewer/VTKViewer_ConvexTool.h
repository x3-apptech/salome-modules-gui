//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef _VTKViewer_ConvexTool_H
#define _VTKViewer_ConvexTool_H

#include "VTKViewer.h"

#include <vector>

#include <vtkSystemIncludes.h>

class vtkUnstructuredGrid;
class vtkGeometryFilter;
class vtkGenericCell;
class vtkDelaunay3D;
class vtkPolyData;
class vtkCellData;
class vtkPoints;
class vtkIdList;
class vtkCell;

class VTKVIEWER_EXPORT VTKViewer_Triangulator
{
 public:
  VTKViewer_Triangulator();

  ~VTKViewer_Triangulator();

  bool 
  Execute(vtkUnstructuredGrid *theInput,
	  vtkCellData* thInputCD,
	  vtkIdType theCellId,
	  int theShowInside,
	  int theAllVisible,
	  const char* theCellsVisibility,
	  vtkPolyData *theOutput,
	  vtkCellData* theOutputCD,
	  int theStoreMapping,
	  std::vector<vtkIdType>& theVTK2ObjIds,
	  bool theIsCheckConvex);

 protected:
  vtkIdList* myCellIds;

  vtkUnstructuredGrid *myInput;
  vtkIdType myCellId;
  int myShowInside;
  int myAllVisible;
  const char* myCellsVisibility;

  virtual
  vtkPoints* 
  InitPoints() = 0;

  virtual
  vtkIdType 
  GetNbOfPoints() = 0;

  virtual
  vtkIdType 
  GetPointId(vtkIdType thePointId) = 0;

  virtual
  vtkFloatingPointType 
  GetCellLength() = 0;

  virtual
  vtkIdType 
  GetNumFaces() = 0;

  virtual
  vtkCell* 
  GetFace(vtkIdType theFaceId) = 0;

  virtual
  void 
  GetCellNeighbors(vtkIdType theCellId,
		   vtkCell* theFace,
		   vtkIdList* theCellIds) = 0;

  virtual
  vtkIdType 
  GetConnectivity(vtkIdType thePntId) = 0;
};


class VTKVIEWER_EXPORT VTKViewer_OrderedTriangulator : public VTKViewer_Triangulator
{
 public:

  VTKViewer_OrderedTriangulator();

  ~VTKViewer_OrderedTriangulator();

 protected:
  vtkGenericCell *myCell;

  virtual
  vtkPoints* 
  InitPoints();

  virtual
  vtkIdType 
  GetNbOfPoints();

  vtkIdType 
  GetPointId(vtkIdType thePointId);

  virtual
  vtkFloatingPointType 
  GetCellLength();

  virtual
  vtkIdType 
  GetNumFaces();

  virtual
  vtkCell* 
  GetFace(vtkIdType theFaceId);

  virtual
  void 
  GetCellNeighbors(vtkIdType theCellId,
		   vtkCell* theFace,
		   vtkIdList* theCellIds);

  virtual
  vtkIdType 
  GetConnectivity(vtkIdType thePntId);
};


class VTKVIEWER_EXPORT VTKViewer_DelaunayTriangulator : public VTKViewer_Triangulator
{
 public:

  VTKViewer_DelaunayTriangulator();

  ~VTKViewer_DelaunayTriangulator();

 protected:
  vtkUnstructuredGrid* myUnstructuredGrid;
  vtkGeometryFilter* myGeometryFilter;
  vtkDelaunay3D* myDelaunay3D;
  vtkPolyData* myPolyData;
  vtkIdType *myPointIds;
  vtkIdList* myFaceIds;
  vtkPoints* myPoints;

  virtual
  vtkPoints* 
  InitPoints();

  virtual
  vtkIdType 
  GetNbOfPoints();

  vtkIdType 
  GetPointId(vtkIdType thePointId);

  virtual
  vtkFloatingPointType 
  GetCellLength();

  virtual
  vtkIdType 
  GetNumFaces();

  virtual
  vtkCell* 
  GetFace(vtkIdType theFaceId);

  virtual
  void 
  GetCellNeighbors(vtkIdType theCellId,
		   vtkCell* theFace,
		   vtkIdList* theCellIds);

  virtual
  vtkIdType 
  GetConnectivity(vtkIdType thePntId);
};


#endif // _VTKViewer_ConvexTool_H
