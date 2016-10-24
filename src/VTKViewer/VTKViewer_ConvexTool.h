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

#ifndef _VTKViewer_ConvexTool_H
#define _VTKViewer_ConvexTool_H

#include "VTKViewer.h"

#include <map>
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
class vtkCellArray;
class vtkTriangle;
class vtkOrderedTriangulator;


//----------------------------------------------------------------------------
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
          int theAppendCoincident3D,
          const char* theCellsVisibility,
          vtkPolyData *theOutput,
          vtkCellData* theOutputCD,
          int theStoreMapping,
          std::vector<vtkIdType>& theVTK2ObjIds,
          std::vector< std::vector<vtkIdType> >& theDimension2VTK2ObjIds,
          bool theIsCheckConvex);

 private:
  vtkIdList* myCellIds;

 protected:
  vtkIdType *myPointIds;
  vtkIdList* myFaceIds;
  vtkPoints* myPoints;

  virtual
  vtkPoints* 
  InitPoints(vtkUnstructuredGrid *theInput,
             vtkIdType theCellId);

  virtual
  vtkIdType 
  GetNumFaces() = 0;

  virtual
  vtkCell* 
  GetFace(vtkIdType theFaceId) = 0;

  vtkIdType 
  GetNbOfPoints();

  vtkIdType 
  GetPointId(vtkIdType thePointId);

  double 
  GetCellLength();

  void 
  GetCellNeighbors(vtkUnstructuredGrid *theInput,
                   vtkIdType theCellId,
                   vtkCell* theFace,
                   vtkIdList* theCellIds);

  vtkIdType 
  GetConnectivity(vtkIdType thePntId);
};


//----------------------------------------------------------------------------
class VTKVIEWER_EXPORT VTKViewer_OrderedTriangulator : public VTKViewer_Triangulator
{
 public:

  VTKViewer_OrderedTriangulator();

  ~VTKViewer_OrderedTriangulator();

 protected:
  vtkOrderedTriangulator *myTriangulator;
  vtkCellArray *myBoundaryTris;
  vtkTriangle  *myTriangle;

  virtual
  vtkPoints* 
  InitPoints(vtkUnstructuredGrid *theInput,
             vtkIdType theCellId);

  virtual
  vtkIdType 
  GetNumFaces();

  virtual
  vtkCell* 
  GetFace(vtkIdType theFaceId);
};


//----------------------------------------------------------------------------
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

  virtual
  vtkPoints* 
  InitPoints(vtkUnstructuredGrid *theInput,
             vtkIdType theCellId);

  virtual
  vtkIdType 
  GetNumFaces();

  virtual
  vtkCell* 
  GetFace(vtkIdType theFaceId);
};


#endif // _VTKViewer_ConvexTool_H
