//  SALOME OBJECT : kernel of SALOME component
//
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
//
//
//  File   : VTKViewer_GeometryFilter.cxx
//  Author : Michael ZORIN
//  Module : SALOME
//  $Header$

#include "VTKViewer_GeometryFilter.h"
#include "VTKViewer_ConvexTool.h"

#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkGenericCell.h>
#include <vtkHexahedron.h>
#include <vtkMergePoints.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPyramid.h>
#include <vtkStructuredGrid.h>
#include <vtkTetra.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVoxel.h>
#include <vtkWedge.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>

#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <set>

#if defined __GNUC__
  #if __GNUC__ == 2
    #define __GNUC_2__
  #endif
#endif

//#define USE_ROBUST_TRIANGULATION

vtkCxxRevisionMacro(VTKViewer_GeometryFilter, "$Revision$");
vtkStandardNewMacro(VTKViewer_GeometryFilter);

VTKViewer_GeometryFilter
::VTKViewer_GeometryFilter(): 
  myShowInside(0),
  myStoreMapping(0),
  myIsWireframeMode(0)
{}


VTKViewer_GeometryFilter
::~VTKViewer_GeometryFilter()
{}


int
VTKViewer_GeometryFilter
::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdType numCells=input->GetNumberOfCells();

  if (numCells == 0)
    {
      return 0;
    }
  
  if (input->GetDataObjectType() == VTK_UNSTRUCTURED_GRID){
    return this->UnstructuredGridExecute(input, output, outInfo);
  }else
    return Superclass::RequestData(request,inputVector,outputVector);

  return 1;
}


int
VTKViewer_GeometryFilter
::UnstructuredGridExecute(
			  vtkDataSet *dataSetInput,
			  vtkPolyData *output,
			  vtkInformation *outInfo)
{
  
  vtkUnstructuredGrid *input= (vtkUnstructuredGrid *)dataSetInput;
  vtkCellArray *Connectivity = input->GetCells();
  // Check input
  if ( Connectivity == NULL )
    {
    vtkDebugMacro(<<"Nothing to extract");
    return 0;
    }

  vtkIdType cellId;
  int i;
  int allVisible;
  vtkIdType npts = 0;
  vtkIdType *pts = 0;
  vtkPoints *p = input->GetPoints();
  vtkIdType numCells=input->GetNumberOfCells();
  vtkPointData *pd = input->GetPointData();
  vtkCellData *cd = input->GetCellData();
  vtkPointData *outputPD = output->GetPointData();
  
  VTKViewer_OrderedTriangulator anOrderedTriangulator;
  VTKViewer_DelaunayTriangulator aDelaunayTriangulator;

  vtkCellData *outputCD = output->GetCellData();
  vtkGenericCell *cell = vtkGenericCell::New();

  vtkIdList *cellIds = vtkIdList::New();
  vtkIdList *faceIds = vtkIdList::New();

  char *cellVis;
  vtkIdType newCellId;
  int faceId, *faceVerts, numFacePts;
  vtkFloatingPointType *x;
  int PixelConvert[4], aNewPts[VTK_CELL_SIZE];
  // ghost cell stuff
  unsigned char  updateLevel = (unsigned char)(output->GetUpdateGhostLevel());
  unsigned char  *cellGhostLevels = 0;  
  
  PixelConvert[0] = 0;
  PixelConvert[1] = 1;
  PixelConvert[2] = 3;
  PixelConvert[3] = 2;
  
  vtkDebugMacro(<<"Executing geometry filter for unstructured grid input");

  vtkDataArray* temp = 0;
  if (cd)
    {
    temp = cd->GetArray("vtkGhostLevels");
    }
  if ( (!temp) || (temp->GetDataType() != VTK_UNSIGNED_CHAR)
    || (temp->GetNumberOfComponents() != 1))
    {
    vtkDebugMacro("No appropriate ghost levels field available.");
    }
  else
    {
    cellGhostLevels = ((vtkUnsignedCharArray*)temp)->GetPointer(0);
    }
  
  // Determine nature of what we have to do
  if ( (!this->CellClipping) && (!this->PointClipping) &&
       (!this->ExtentClipping) )
    {
    allVisible = 1;
    cellVis = NULL;
    }
  else
    {
    allVisible = 0;
    cellVis = new char[numCells];
    }

  // Just pass points through, never merge
  output->SetPoints(input->GetPoints());
  outputPD->PassData(pd);

  outputCD->CopyAllocate(cd,numCells,numCells/2);

  output->Allocate(numCells/4+1,numCells);
  
  // Loop over the cells determining what's visible
  if (!allVisible)
    {
    for (cellId=0, Connectivity->InitTraversal(); 
         Connectivity->GetNextCell(npts,pts); 
         cellId++)
      {
      cellVis[cellId] = 1;
      if ( this->CellClipping && cellId < this->CellMinimum ||
           cellId > this->CellMaximum )
        {
        cellVis[cellId] = 0;
        }
      else
        {
        for (i=0; i < npts; i++) 
          {
          x = p->GetPoint(pts[i]);
          if ( (this->PointClipping && (pts[i] < this->PointMinimum ||
                                        pts[i] > this->PointMaximum) ) ||
               (this->ExtentClipping && 
                (x[0] < this->Extent[0] || x[0] > this->Extent[1] ||
                 x[1] < this->Extent[2] || x[1] > this->Extent[3] ||
                 x[2] < this->Extent[4] || x[2] > this->Extent[5] )) )
            {
            cellVis[cellId] = 0;
            break;
            }//point/extent clipping
          }//for each point
        }//if point clipping needs checking
      }//for all cells
    }//if not all visible
  
  // Loop over all cells now that visibility is known
  // (Have to compute visibility first for 3D cell boundarys)
  int progressInterval = numCells/20 + 1;
  if(myStoreMapping){
    myVTK2ObjIds.clear();
    myVTK2ObjIds.reserve(numCells);
  }
  for (cellId=0, Connectivity->InitTraversal(); 
       Connectivity->GetNextCell(npts,pts); 
       cellId++)
    {
    //Progress and abort method support
    if ( !(cellId % progressInterval) )
      {
      vtkDebugMacro(<<"Process cell #" << cellId);
      this->UpdateProgress ((float)cellId/numCells);
      }

    // Handle ghost cells here.  Another option was used cellVis array.
    if (cellGhostLevels && cellGhostLevels[cellId] > updateLevel)
      { // Do not create surfaces in outer ghost cells.
      continue;
      }
    
    if (allVisible || cellVis[cellId])  //now if visible extract geometry
      {
      //special code for nonlinear cells - rarely occurs, so right now it
      //is slow.
      vtkIdType aCellType = input->GetCellType(cellId);
      switch (aCellType)
        {
        case VTK_EMPTY_CELL:
          break;

        case VTK_VERTEX:
        case VTK_POLY_VERTEX:
          newCellId = output->InsertNextCell(aCellType,npts,pts);
	  if(myStoreMapping){
	    myVTK2ObjIds.push_back(cellId); //apo
	  }
          outputCD->CopyData(cd,cellId,newCellId);
          break;

        case VTK_LINE: 
        case VTK_POLY_LINE:
          newCellId = output->InsertNextCell(aCellType,npts,pts);
	  if(myStoreMapping)
	    myVTK2ObjIds.push_back(cellId);
          outputCD->CopyData(cd,cellId,newCellId);
          break;

        case VTK_TRIANGLE:
        case VTK_QUAD:
        case VTK_POLYGON:
          newCellId = output->InsertNextCell(aCellType,npts,pts);
	  if(myStoreMapping)
	    myVTK2ObjIds.push_back(cellId);
          outputCD->CopyData(cd,cellId,newCellId);
          break;

        case VTK_TRIANGLE_STRIP:
          newCellId = output->InsertNextCell(aCellType,npts,pts);
	  if(myStoreMapping)
	    myVTK2ObjIds.push_back(cellId);
          outputCD->CopyData(cd,cellId,newCellId);
          break;

        case VTK_PIXEL:
          newCellId = output->InsertNextCell(aCellType,npts,pts);
	  if(myStoreMapping)
	    myVTK2ObjIds.push_back(cellId);
	  outputCD->CopyData(cd,cellId,newCellId);
          break;
	  
	case VTK_CONVEX_POINT_SET: {
	  bool anIsOk = anOrderedTriangulator.Execute(input,
						      cd,
						      cellId,
						      myShowInside,
						      allVisible,
						      cellVis,
						      output,
						      outputCD,
						      myStoreMapping,
						      myVTK2ObjIds,
						      true);
	  if(!anIsOk)
	    aDelaunayTriangulator.Execute(input,
					  cd,
					  cellId,
					  myShowInside,
					  allVisible,
					  cellVis,
					  output,
					  outputCD,
					  myStoreMapping,
					  myVTK2ObjIds,
					  false);
	      
	  break;
	}
        case VTK_TETRA: {
          for (faceId = 0; faceId < 4; faceId++)
            {
            faceIds->Reset();
            faceVerts = vtkTetra::GetFaceArray(faceId);
            faceIds->InsertNextId(pts[faceVerts[0]]);
            faceIds->InsertNextId(pts[faceVerts[1]]);
            faceIds->InsertNextId(pts[faceVerts[2]]);
	    aCellType = VTK_TRIANGLE;
            numFacePts = 3;
            input->GetCellNeighbors(cellId, faceIds, cellIds);
            if ( cellIds->GetNumberOfIds() <= 0 || myShowInside ||
                 (!allVisible && !cellVis[cellIds->GetId(0)]) )
              {
              for ( i=0; i < numFacePts; i++)
                aNewPts[i] = pts[faceVerts[i]];
              newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
              outputCD->CopyData(cd,cellId,newCellId);
              }
            }
          break;
	}
        case VTK_VOXEL: {
          for (faceId = 0; faceId < 6; faceId++)
            {
            faceIds->Reset();
            faceVerts = vtkVoxel::GetFaceArray(faceId);
            faceIds->InsertNextId(pts[faceVerts[0]]);
            faceIds->InsertNextId(pts[faceVerts[1]]);
            faceIds->InsertNextId(pts[faceVerts[2]]);
            faceIds->InsertNextId(pts[faceVerts[3]]);
	    aCellType = VTK_QUAD;
            numFacePts = 4;
            input->GetCellNeighbors(cellId, faceIds, cellIds);
            if ( cellIds->GetNumberOfIds() <= 0 || myShowInside || 
                 (!allVisible && !cellVis[cellIds->GetId(0)]) )
              {
              for ( i=0; i < numFacePts; i++)
                aNewPts[i] = pts[faceVerts[PixelConvert[i]]];
              newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
              outputCD->CopyData(cd,cellId,newCellId);
              }
            }
          break;
	}
        case VTK_HEXAHEDRON: {
          for (faceId = 0; faceId < 6; faceId++)
            {
            faceIds->Reset();
            faceVerts = vtkHexahedron::GetFaceArray(faceId);
            faceIds->InsertNextId(pts[faceVerts[0]]);
            faceIds->InsertNextId(pts[faceVerts[1]]);
            faceIds->InsertNextId(pts[faceVerts[2]]);
            faceIds->InsertNextId(pts[faceVerts[3]]);
	    aCellType = VTK_QUAD;
            numFacePts = 4;
            input->GetCellNeighbors(cellId, faceIds, cellIds);
            if ( cellIds->GetNumberOfIds() <= 0 || myShowInside ||
                 (!allVisible && !cellVis[cellIds->GetId(0)]) )
              {
              for ( i=0; i < numFacePts; i++)
                aNewPts[i] = pts[faceVerts[i]];
              newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
              outputCD->CopyData(cd,cellId,newCellId);
              }
            }
          break;
	}
        case VTK_WEDGE: {
          for (faceId = 0; faceId < 5; faceId++)
            {
            faceIds->Reset();
            faceVerts = vtkWedge::GetFaceArray(faceId);
            faceIds->InsertNextId(pts[faceVerts[0]]);
            faceIds->InsertNextId(pts[faceVerts[1]]);
            faceIds->InsertNextId(pts[faceVerts[2]]);
	    aCellType = VTK_TRIANGLE;
            numFacePts = 3;
            if (faceVerts[3] >= 0)
              {
              faceIds->InsertNextId(pts[faceVerts[3]]);
	      aCellType = VTK_QUAD;
              numFacePts = 4;
              }
            input->GetCellNeighbors(cellId, faceIds, cellIds);
            if ( cellIds->GetNumberOfIds() <= 0 || myShowInside || 
                 (!allVisible && !cellVis[cellIds->GetId(0)]) )
              {
              for ( i=0; i < numFacePts; i++)
                aNewPts[i] = pts[faceVerts[i]];
              newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
              outputCD->CopyData(cd,cellId,newCellId);
              }
            }
          break;
	}
        case VTK_PYRAMID: {
          for (faceId = 0; faceId < 5; faceId++)
            {
            faceIds->Reset();
            faceVerts = vtkPyramid::GetFaceArray(faceId);
            faceIds->InsertNextId(pts[faceVerts[0]]);
            faceIds->InsertNextId(pts[faceVerts[1]]);
            faceIds->InsertNextId(pts[faceVerts[2]]);
	    aCellType = VTK_TRIANGLE;
            numFacePts = 3;
            if (faceVerts[3] >= 0)
              {
              faceIds->InsertNextId(pts[faceVerts[3]]);
	      aCellType = VTK_QUAD;
              numFacePts = 4;
              }
            input->GetCellNeighbors(cellId, faceIds, cellIds);
            if ( cellIds->GetNumberOfIds() <= 0 || myShowInside || 
                 (!allVisible && !cellVis[cellIds->GetId(0)]) )
              {
              for ( i=0; i < numFacePts; i++)
                aNewPts[i] = pts[faceVerts[i]];
              newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
              outputCD->CopyData(cd,cellId,newCellId);
              }
            }
          break;
	}
        //Quadratic cells
        case VTK_QUADRATIC_EDGE:
        case VTK_QUADRATIC_TRIANGLE:
        case VTK_QUADRATIC_QUAD:
        case VTK_QUADRATIC_TETRA:
        case VTK_QUADRATIC_HEXAHEDRON:
	  if(!myIsWireframeMode){
	    input->GetCell(cellId,cell);
	    vtkIdList *pts = vtkIdList::New();  
	    vtkPoints *coords = vtkPoints::New();
	    vtkIdList *cellIds = vtkIdList::New();
	    vtkIdType newCellId;
	    
	    if ( cell->GetCellDimension() == 1 ) {
	      aCellType = VTK_LINE;
	      numFacePts = 2;
	      cell->Triangulate(0,pts,coords);
	      for (i=0; i < pts->GetNumberOfIds(); i+=2) {
		aNewPts[0] = pts->GetId(i);
		aNewPts[1] = pts->GetId(i+1);
		newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
		if(myStoreMapping)
		  myVTK2ObjIds.push_back(cellId);
		outputCD->CopyData(cd,cellId,newCellId);
              }
            }
	    else if ( cell->GetCellDimension() == 2 ) {
	      aCellType = VTK_TRIANGLE;
	      numFacePts = 3;
	      cell->Triangulate(0,pts,coords);
	      for (i=0; i < pts->GetNumberOfIds(); i+=3) {
		aNewPts[0] = pts->GetId(i);
		aNewPts[1] = pts->GetId(i+1);
		aNewPts[2] = pts->GetId(i+2);
		newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
		if(myStoreMapping)
		  myVTK2ObjIds.push_back(cellId);
		outputCD->CopyData(cd,cellId,newCellId);
              }
            } 
	    else //3D nonlinear cell
            {
	      aCellType = VTK_TRIANGLE;
	      numFacePts = 3;
	      for (int j=0; j < cell->GetNumberOfFaces(); j++){
		vtkCell *face = cell->GetFace(j);
		input->GetCellNeighbors(cellId, face->PointIds, cellIds);
		if ( cellIds->GetNumberOfIds() <= 0 || myShowInside ) {
		  face->Triangulate(0,pts,coords);
		  for (i=0; i < pts->GetNumberOfIds(); i+=3) {
		    aNewPts[0] = pts->GetId(i);
		    aNewPts[1] = pts->GetId(i+1);
		    aNewPts[2] = pts->GetId(i+2);
		    newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
		    if(myStoreMapping)
		      myVTK2ObjIds.push_back(cellId);
		    outputCD->CopyData(cd,cellId,newCellId);
                  }
                }
              }
            } //3d cell
	    cellIds->Delete();
	    coords->Delete();
	    pts->Delete();
	    break;
          }else{
	    switch(aCellType){
	    case VTK_QUADRATIC_EDGE: {
	      aCellType = VTK_POLY_LINE;
	      numFacePts = 3;
	      
	      aNewPts[0] = pts[0];
	      aNewPts[2] = pts[1];
	      aNewPts[1] = pts[2];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);
	      break;
	    }
	    case VTK_QUADRATIC_TRIANGLE: {
	      aCellType = VTK_POLYGON;
	      numFacePts = 6;
	      
	      aNewPts[0] = pts[0];
	      aNewPts[1] = pts[3];
	      aNewPts[2] = pts[1];
	      aNewPts[3] = pts[4];
	      aNewPts[4] = pts[2];
	      aNewPts[5] = pts[5];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);
	      break;
	    }
	    case VTK_QUADRATIC_QUAD: {
	      aCellType = VTK_POLYGON;
	      numFacePts = 8;
	      
	      aNewPts[0] = pts[0];
	      aNewPts[1] = pts[4];
	      aNewPts[2] = pts[1];
	      aNewPts[3] = pts[5];
	      aNewPts[4] = pts[2];
	      aNewPts[5] = pts[6];
	      aNewPts[6] = pts[3];
	      aNewPts[7] = pts[7];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);
	      break;
	    }
	    case VTK_QUADRATIC_TETRA: {
	      aCellType = VTK_POLYGON;
	      numFacePts = 6;
	      
	      //---------------------------------------------------------------
	      aNewPts[0] = pts[0];
	      aNewPts[1] = pts[4];
	      aNewPts[2] = pts[1];
	      aNewPts[3] = pts[5];
	      aNewPts[4] = pts[2];
	      aNewPts[5] = pts[6];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);

	      //---------------------------------------------------------------
	      aNewPts[0] = pts[0];
	      aNewPts[1] = pts[7];
	      aNewPts[2] = pts[3];
	      aNewPts[3] = pts[8];
	      aNewPts[4] = pts[1];
	      aNewPts[5] = pts[4];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);

	      //---------------------------------------------------------------
	      aNewPts[0] = pts[1];
	      aNewPts[1] = pts[8];
	      aNewPts[2] = pts[3];
	      aNewPts[3] = pts[9];
	      aNewPts[4] = pts[2];
	      aNewPts[5] = pts[5];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);

	      //---------------------------------------------------------------
	      aNewPts[0] = pts[2];
	      aNewPts[1] = pts[9];
	      aNewPts[2] = pts[3];
	      aNewPts[3] = pts[7];
	      aNewPts[4] = pts[0];
	      aNewPts[5] = pts[6];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);

	      break;
	    }
	    case VTK_QUADRATIC_HEXAHEDRON: {
	      aCellType = VTK_POLYGON;
	      numFacePts = 8;
	      
	      //---------------------------------------------------------------
	      aNewPts[0] = pts[0];
	      aNewPts[1] = pts[8];
	      aNewPts[2] = pts[1];
	      aNewPts[3] = pts[17];
	      aNewPts[4] = pts[5];
	      aNewPts[5] = pts[12];
	      aNewPts[6] = pts[4];
	      aNewPts[7] = pts[16];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);
	      
	      //---------------------------------------------------------------
	      aNewPts[0] = pts[1];
	      aNewPts[1] = pts[9];
	      aNewPts[2] = pts[2];
	      aNewPts[3] = pts[18];
	      aNewPts[4] = pts[6];
	      aNewPts[5] = pts[13];
	      aNewPts[6] = pts[5];
	      aNewPts[7] = pts[17];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);
	      
	      //---------------------------------------------------------------
	      aNewPts[0] = pts[2];
	      aNewPts[1] = pts[10];
	      aNewPts[2] = pts[3];
	      aNewPts[3] = pts[19];
	      aNewPts[4] = pts[7];
	      aNewPts[5] = pts[14];
	      aNewPts[6] = pts[6];
	      aNewPts[7] = pts[18];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);
	      
	      //---------------------------------------------------------------
	      aNewPts[0] = pts[3];
	      aNewPts[1] = pts[11];
	      aNewPts[2] = pts[0];
	      aNewPts[3] = pts[16];
	      aNewPts[4] = pts[4];
	      aNewPts[5] = pts[15];
	      aNewPts[6] = pts[7];
	      aNewPts[7] = pts[19];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);
	      
	      //---------------------------------------------------------------
	      aNewPts[0] = pts[0];
	      aNewPts[1] = pts[8];
	      aNewPts[2] = pts[1];
	      aNewPts[3] = pts[9];
	      aNewPts[4] = pts[2];
	      aNewPts[5] = pts[10];
	      aNewPts[6] = pts[3];
	      aNewPts[7] = pts[11];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);
	      
	      //---------------------------------------------------------------
	      aNewPts[0] = pts[4];
	      aNewPts[1] = pts[12];
	      aNewPts[2] = pts[5];
	      aNewPts[3] = pts[13];
	      aNewPts[4] = pts[6];
	      aNewPts[5] = pts[14];
	      aNewPts[6] = pts[7];
	      aNewPts[7] = pts[15];
	      
	      newCellId = output->InsertNextCell(aCellType,numFacePts,aNewPts);
	      if(myStoreMapping)
		myVTK2ObjIds.push_back(cellId);
	      
	      outputCD->CopyData(cd,cellId,newCellId);
	      
	      break;
	    }}
	  }
	} //switch
      } //if visible
    } //for all cells
  
  output->Squeeze();

  vtkDebugMacro(<<"Extracted " << input->GetNumberOfPoints() << " points,"
  << output->GetNumberOfCells() << " cells.");

  cell->Delete();

  cellIds->Delete();
  faceIds->Delete();

  if ( cellVis )
    {
    delete [] cellVis;
    }

  return 0;
}


void
VTKViewer_GeometryFilter
::SetInside(int theShowInside)
{
  if(myShowInside == theShowInside) 
    return;

  myShowInside = theShowInside;
  this->Modified();
}

int
VTKViewer_GeometryFilter
::GetInside()
{
  return myShowInside;
}


void 
VTKViewer_GeometryFilter
::SetWireframeMode(int theIsWireframeMode)
{
  if(myIsWireframeMode == theIsWireframeMode)
    return;

  myIsWireframeMode = theIsWireframeMode;
  this->Modified();
}

int
VTKViewer_GeometryFilter
::GetWireframeMode()
{
  return myIsWireframeMode;
}


void
VTKViewer_GeometryFilter
::SetStoreMapping(int theStoreMapping)
{
  if(myStoreMapping == theStoreMapping) 
    return;

  myStoreMapping = theStoreMapping;
  this->Modified();
}

int
VTKViewer_GeometryFilter
::GetStoreMapping()
{
  return myStoreMapping;
}


vtkIdType VTKViewer_GeometryFilter::GetElemObjId( int theVtkID )
{
  if( myVTK2ObjIds.empty() || theVtkID > (int)myVTK2ObjIds.size() )
    return -1;
#if defined __GNUC_2__
  return myVTK2ObjIds[theVtkID];
#else
  return myVTK2ObjIds.at(theVtkID);
#endif
}
