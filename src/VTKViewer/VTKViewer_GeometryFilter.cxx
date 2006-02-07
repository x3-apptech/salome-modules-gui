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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : VTKViewer_GeometryFilter.cxx
//  Author : Michael ZORIN
//  Module : SALOME
//  $Header$

#include "VTKViewer_GeometryFilter.h"

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

#include <vtkMath.h>
#include <vtkPlane.h>
#include <vtkDelaunay3D.h>
#include <vtkGeometryFilter.h>

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

#define USE_ROBUST_TRIANGULATION

//----------------------------------------------------------------------------
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


//----------------------------------------------------------------------------
void
VTKViewer_GeometryFilter
::Execute()
{
  vtkDataSet *input= this->GetInput();
  vtkIdType numCells=input->GetNumberOfCells();

  if (numCells == 0)
    {
      return;
    }
  
  if (input->GetDataObjectType() == VTK_UNSTRUCTURED_GRID){
    this->UnstructuredGridExecute();
    return;
  }else
    vtkGeometryFilter::Execute();
}


//----------------------------------------------------------------------------
void
VTKViewer_GeometryFilter
::UnstructuredGridExecute()
{
  vtkUnstructuredGrid *input= (vtkUnstructuredGrid *)this->GetInput();
  vtkCellArray *Connectivity = input->GetCells();
  // Check input
  if ( Connectivity == NULL )
    {
    vtkDebugMacro(<<"Nothing to extract");
    return;
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
  vtkPolyData *output = this->GetOutput();
  vtkPointData *outputPD = output->GetPointData();
  
#ifdef USE_ROBUST_TRIANGULATION
  vtkUnstructuredGrid* anUnstructuredGrid = vtkUnstructuredGrid::New();
  vtkPoints* aDelaunayPoints = vtkPoints::New();

  vtkDelaunay3D* aDelaunay3D = vtkDelaunay3D::New();
  aDelaunay3D->SetInput(anUnstructuredGrid);
  
  vtkGeometryFilter* aGeometryFilter = vtkGeometryFilter::New();
  aGeometryFilter->SetInput(aDelaunay3D->GetOutput());
#endif

  vtkCellData *outputCD = output->GetCellData();
  vtkGenericCell *cell = vtkGenericCell::New();


  vtkIdList *cellIds = vtkIdList::New();
  vtkIdList *faceIds = vtkIdList::New();

  char *cellVis;
  vtkIdType newCellId;
  int faceId, *faceVerts, numFacePts;
  float *x;
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
	  //cout<<"cellId = "<<cellId<<"\n";

	  vtkIdType aNumPts;
	  vtkPoints *aPoints;
#ifdef USE_ROBUST_TRIANGULATION
	  aPoints = aDelaunayPoints;
	  anUnstructuredGrid->Initialize();
	  anUnstructuredGrid->Allocate();
	  anUnstructuredGrid->SetPoints(aDelaunayPoints);

	  vtkIdType *aPts;
	  input->GetCellPoints(cellId,aNumPts,aPts); 
	  {
	    float aPntCoord[3];
	    aDelaunayPoints->SetNumberOfPoints(aNumPts);
	    vtkPoints *anInputPoints = input->GetPoints();
	    for (int aPntId = 0; aPntId < aNumPts; aPntId++) {
	      anInputPoints->GetPoint(aPts[aPntId],aPntCoord);
	      aDelaunayPoints->SetPoint(aPntId,aPntCoord);
	    }
	  }
#else
	  input->GetCell(cellId,cell);
	  aPoints = input->GetPoints();
	  aNumPts = cell->GetNumberOfPoints();
#endif
	  // To calculate the bary center of the cell
	  float aCellCenter[3] = {0.0, 0.0, 0.0};
	  {
	    float aPntCoord[3];
	    for (int aPntId = 0; aPntId < aNumPts; aPntId++) {
#ifdef USE_ROBUST_TRIANGULATION
	      aPoints->GetPoint(aPntId,aPntCoord);
#else
	      aPoints->GetPoint(cell->GetPointId(aPntId),aPntCoord);
#endif
	      //cout<<"\t\taPntId = "<<aPntId<<" {"<<aPntCoord[0]<<", "<<aPntCoord[1]<<", "<<aPntCoord[2]<<"}\n";
	      aCellCenter[0] += aPntCoord[0];
	      aCellCenter[1] += aPntCoord[1];
	      aCellCenter[2] += aPntCoord[2];
	    }
	    aCellCenter[0] /= aNumPts;
	    aCellCenter[1] /= aNumPts;
	    aCellCenter[2] /= aNumPts;
	  }

#ifdef USE_ROBUST_TRIANGULATION
	  aGeometryFilter->Update();
	  vtkPolyData* aPolyData = aGeometryFilter->GetOutput();

	  float aCellLength = aPolyData->GetLength();
          int aNumFaces = aPolyData->GetNumberOfCells();
#else
	  float aCellLength = sqrt(cell->GetLength2());
          int aNumFaces = cell->GetNumberOfFaces();
#endif
	  
	  static float EPS = 1.0E-5;
	  float aDistEps = aCellLength * EPS;

	  // To initialize set of points that belong to the cell
	  typedef std::set<vtkIdType> TPointIds;
	  TPointIds anInitialPointIds;
	  for(vtkIdType aPntId = 0; aPntId < aNumPts; aPntId++){
#ifdef USE_ROBUST_TRIANGULATION
	    anInitialPointIds.insert(aPntId);
#else
	    anInitialPointIds.insert(cell->GetPointId(aPntId));
#endif
	  }

	  // To initialize set of points by face that belong to the cell and backward
	  typedef std::set<vtkIdType> TFace2Visibility;
	  TFace2Visibility aFace2Visibility;

	  typedef std::set<TPointIds> TFace2PointIds;
	  TFace2PointIds aFace2PointIds;

          for (int aFaceId = 0; aFaceId < aNumFaces; aFaceId++) {
#ifdef USE_ROBUST_TRIANGULATION
            vtkCell* aFace = aPolyData->GetCell(aFaceId);
#else
            vtkCell* aFace = cell->GetFace(aFaceId);
#endif
	    vtkIdList *anIdList = aFace->PointIds;  
	    aNewPts[0] = anIdList->GetId(0);
	    aNewPts[1] = anIdList->GetId(1);
	    aNewPts[2] = anIdList->GetId(2);

#ifdef USE_ROBUST_TRIANGULATION
            faceIds->Reset();
            faceIds->InsertNextId(aPts[aNewPts[0]]);
            faceIds->InsertNextId(aPts[aNewPts[1]]);
            faceIds->InsertNextId(aPts[aNewPts[2]]);
            input->GetCellNeighbors(cellId, faceIds, cellIds);
#else
            input->GetCellNeighbors(cellId, anIdList, cellIds);
#endif
	    if((!allVisible && !cellVis[cellIds->GetId(0)]) || 
	       cellIds->GetNumberOfIds() <= 0 ||
	       myShowInside)
	    {
	      TPointIds aPointIds;
	      aPointIds.insert(aNewPts[0]);
	      aPointIds.insert(aNewPts[1]);
	      aPointIds.insert(aNewPts[2]);

	      aFace2PointIds.insert(aPointIds);
	      aFace2Visibility.insert(aFaceId);
	    }
	  }

          for (int aFaceId = 0; aFaceId < aNumFaces; aFaceId++) {
            if(aFace2Visibility.find(aFaceId) == aFace2Visibility.end())
	      continue;

#ifdef USE_ROBUST_TRIANGULATION
            vtkCell* aFace = aPolyData->GetCell(aFaceId);
#else
            vtkCell* aFace = cell->GetFace(aFaceId);
#endif
	    vtkIdList *anIdList = aFace->PointIds;
	    aNewPts[0] = anIdList->GetId(0);
	    aNewPts[1] = anIdList->GetId(1);
	    aNewPts[2] = anIdList->GetId(2);
	    
	    // To initialize set of points for the plane where the trinangle face belong to
	    TPointIds aPointIds;
	    aPointIds.insert(aNewPts[0]);
	    aPointIds.insert(aNewPts[1]);
	    aPointIds.insert(aNewPts[2]);

	    // To get know, if the points of the trinagle were already observed
	    bool anIsObserved = aFace2PointIds.find(aPointIds) == aFace2PointIds.end();
	    //cout<<"\taFaceId = "<<aFaceId<<"; anIsObserved = "<<anIsObserved;
	    //cout<<"; aNewPts = {"<<aNewPts[0]<<", "<<aNewPts[1]<<", "<<aNewPts[2]<<"}\n";
	      
	    if(!anIsObserved){
	      // To get coordinates of the points of the traingle face
	      float aCoord[3][3];
	      aPoints->GetPoint(aNewPts[0],aCoord[0]);
	      aPoints->GetPoint(aNewPts[1],aCoord[1]);
	      aPoints->GetPoint(aNewPts[2],aCoord[2]);
	      
	      // To calculate plane normal
	      float aVector01[3] = { aCoord[1][0] - aCoord[0][0],
				     aCoord[1][1] - aCoord[0][1],
				     aCoord[1][2] - aCoord[0][2] };
	      
	      float aVector02[3] = { aCoord[2][0] - aCoord[0][0],
				     aCoord[2][1] - aCoord[0][1],
				     aCoord[2][2] - aCoord[0][2] };
	      
	      float aCross21[3];
	      vtkMath::Cross(aVector02,aVector01,aCross21);
	      
	      vtkMath::Normalize(aCross21);
	      
	      // To calculate what points belong to the plane
	      // To calculate bounds of the point set
	      float aCenter[3] = {0.0, 0.0, 0.0};
	      {
		TPointIds::const_iterator anIter = anInitialPointIds.begin();
		TPointIds::const_iterator anEndIter = anInitialPointIds.end();
		for(; anIter != anEndIter; anIter++){
		  float aPntCoord[3];
		  vtkIdType aPntId = *anIter;
		  aPoints->GetPoint(aPntId,aPntCoord);
		  float aDist = vtkPlane::DistanceToPlane(aPntCoord,aCross21,aCoord[0]);
		  //cout<<"\t\taPntId = "<<aPntId<<" {"<<aPntCoord[0]<<", "<<aPntCoord[1]<<", "<<aPntCoord[2]<<"}; aDist = "<<aDist<<"\n";
		  if(fabs(aDist) < aDistEps){
		    aPointIds.insert(aPntId);
		    aCenter[0] += aPntCoord[0];
		    aCenter[1] += aPntCoord[1];
		    aCenter[2] += aPntCoord[2];
		  }
		}
		int aNbPoints = aPointIds.size();
		aCenter[0] /= aNbPoints;
		aCenter[1] /= aNbPoints;
		aCenter[2] /= aNbPoints;
	      }
	      
	      // To calculate the primary direction for point set
	      float aVector0[3] = { aCoord[0][0] - aCenter[0],
				    aCoord[0][1] - aCenter[1],
				    aCoord[0][2] - aCenter[2] };

	      //To sinchronize orientation of the cell and its face
	      float aVectorC[3] = { aCenter[0] - aCellCenter[0],
				    aCenter[1] - aCellCenter[1],
				    aCenter[2] - aCellCenter[2] };
	      vtkMath::Normalize(aVectorC);

	      float aDot = vtkMath::Dot(aCross21,aVectorC);
	      //cout<<"\t\taCross21 = {"<<aCross21[0]<<", "<<aCross21[1]<<", "<<aCross21[2]<<"}";
	      //cout<<"; aVectorC = {"<<aVectorC[0]<<", "<<aVectorC[1]<<", "<<aVectorC[2]<<"}\n";
	      //cout<<"\t\taDot = "<<aDot<<"\n";
	      if(aDot > 0){
		aCross21[0] = -aCross21[0];
		aCross21[1] = -aCross21[1];
		aCross21[2] = -aCross21[2];
	      }
		
	      vtkMath::Normalize(aVector0);
	      
	      //cout<<"\t\taCenter = {"<<aCenter[0]<<", "<<aCenter[1]<<", "<<aCenter[2]<<"}";
	      //cout<<"; aVector0 = {"<<aVector0[0]<<", "<<aVector0[1]<<", "<<aVector0[2]<<"}\n";

	      // To calculate the set of points by face those that belong to the plane
	      TFace2PointIds aRemoveFace2PointIds;
	      {
		TFace2PointIds::const_iterator anIter = aFace2PointIds.begin();
		TFace2PointIds::const_iterator anEndIter = aFace2PointIds.end();
		for(; anIter != anEndIter; anIter++){
		  const TPointIds& anIds = *anIter;
		  TPointIds anIntersection;
		  std::set_intersection(aPointIds.begin(),aPointIds.end(),
					anIds.begin(),anIds.end(),
					std::inserter(anIntersection,anIntersection.begin()));

		  if(anIntersection == anIds){
		    aRemoveFace2PointIds.insert(anIds);
		  }
		}
	      }

	      // To remove from the set of points by face those that belong to the plane
	      {
		TFace2PointIds::const_iterator anIter = aRemoveFace2PointIds.begin();
		TFace2PointIds::const_iterator anEndIter = aRemoveFace2PointIds.end();
		for(; anIter != anEndIter; anIter++){
		  const TPointIds& anIds = *anIter;
		  aFace2PointIds.erase(anIds);
		}
	      }

	      // To sort the planar set of the points accrding to the angle
	      {
		typedef std::map<float,vtkIdType> TSortedPointIds;
		TSortedPointIds aSortedPointIds;

		TPointIds::const_iterator anIter = aPointIds.begin();
		TPointIds::const_iterator anEndIter = aPointIds.end();
		for(; anIter != anEndIter; anIter++){
		  float aPntCoord[3];
		  vtkIdType aPntId = *anIter;
		  aPoints->GetPoint(aPntId,aPntCoord);
		  float aVector[3] = { aPntCoord[0] - aCenter[0],
				       aPntCoord[1] - aCenter[1],
				       aPntCoord[2] - aCenter[2] };
		  vtkMath::Normalize(aVector);

		  float aCross[3];
		  vtkMath::Cross(aVector,aVector0,aCross);
		  bool aGreaterThanPi = vtkMath::Dot(aCross,aCross21) < 0;
		  float aCosinus = vtkMath::Dot(aVector,aVector0);
		  if(aCosinus > 1.0)
		    aCosinus = 1.0;
		  if(aCosinus < -1.0)
		    aCosinus = -1.0;
		  static float a2Pi = 2.0 * vtkMath::Pi();
		  float anAngle = acos(aCosinus);
		  //cout<<"\t\taPntId = "<<aPntId<<" {"<<aPntCoord[0]<<", "<<aPntCoord[1]<<", "<<aPntCoord[2]<<"}";
		  //cout<<"; aGreaterThanPi = "<<aGreaterThanPi<<"; aCosinus = "<<aCosinus<<"; anAngle = "<<anAngle<<"\n";
		  if(aGreaterThanPi)
		    anAngle = a2Pi - anAngle;
		  aSortedPointIds[anAngle] = aPntId;
		  //cout<<"\t\t\tanAngle = "<<anAngle<<"\n";
		}
		if(!aSortedPointIds.empty()){
		  aCellType = VTK_POLYGON;
		  int numFacePts = aSortedPointIds.size();
		  std::vector<vtkIdType> aConnectivities(numFacePts);
		  TSortedPointIds::const_iterator anIter = aSortedPointIds.begin();
		  TSortedPointIds::const_iterator anEndIter = aSortedPointIds.end();
		  for(vtkIdType anId = 0; anIter != anEndIter; anIter++, anId++){
		    vtkIdType aPntId = anIter->second;
#ifdef USE_ROBUST_TRIANGULATION
		    aConnectivities[anId] = aPts[aPntId];
#else
		    aConnectivities[anId] = aPntId;
#endif
		  }
		  newCellId = output->InsertNextCell(aCellType,numFacePts,&aConnectivities[0]);
		  if(myStoreMapping)
		    myVTK2ObjIds.push_back(cellId);
		  outputCD->CopyData(cd,cellId,newCellId);
		}
	      }
	    }
	  }

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

#ifdef USE_ROBUST_TRIANGULATION
  anUnstructuredGrid->Delete();
  aDelaunayPoints->Delete();

  aDelaunay3D->Delete();
  aGeometryFilter->Delete();
#endif

  cell->Delete();

  cellIds->Delete();
  faceIds->Delete();

  if ( cellVis )
    {
    delete [] cellVis;
    }
}


//----------------------------------------------------------------------------
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


//----------------------------------------------------------------------------
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


//----------------------------------------------------------------------------
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


//----------------------------------------------------------------------------
vtkIdType VTKViewer_GeometryFilter::GetElemObjId(int theVtkID){
  if(myVTK2ObjIds.empty() || theVtkID > myVTK2ObjIds.size()) return -1;
#if defined __GNUC_2__
  return myVTK2ObjIds[theVtkID];
#else
  return myVTK2ObjIds.at(theVtkID);
#endif
}
