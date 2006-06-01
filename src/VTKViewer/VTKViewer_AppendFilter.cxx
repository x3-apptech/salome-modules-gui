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
//  Author : 
//  Module : SALOME
//  $Header$

#include "VTKViewer_AppendFilter.h"

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkDataSetAttributes.h>
#include <vtkDataSetCollection.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>

#include <vtkPoints.h>

#if defined __GNUC__
  #if __GNUC__ == 2
    #define __GNUC_2__
  #endif
#endif

vtkCxxRevisionMacro(VTKViewer_AppendFilter, "$Revision$");
vtkStandardNewMacro(VTKViewer_AppendFilter);

VTKViewer_AppendFilter
::VTKViewer_AppendFilter() 
{
  myDoMappingFlag = false;
}

VTKViewer_AppendFilter
::~VTKViewer_AppendFilter()
{}

void
VTKViewer_AppendFilter
::SetDoMappingFlag(const bool theFlag)
{
  if(myDoMappingFlag == theFlag)
    return;

  myDoMappingFlag = theFlag;

  this->Modified();
}

bool 
VTKViewer_AppendFilter
::DoMappingFlag() const
{
  return myDoMappingFlag;
}

void
VTKViewer_AppendFilter
::SetPoints(vtkPoints* thePoints)
{
  if(GetPoints() == thePoints)
    return;

  myPoints = thePoints;

  this->Modified();
}

vtkPoints*
VTKViewer_AppendFilter
::GetPoints()
{
  return myPoints.GetPointer();
}

void
VTKViewer_AppendFilter
::Execute()
{
  if(myPoints.GetPointer())
    MakeOutput();
  else
    Superclass::Execute();

  if(myDoMappingFlag)
    DoMapping();
}


void 
VTKViewer_AppendFilter
::DoMapping()
{
  myNodeRanges.clear();
  myCellRanges.clear();

  vtkIdType aPntStartId = 0;
  vtkIdType aCellStartId = 0;

  for(vtkIdType aDataSetId = 0; aDataSetId < this->NumberOfInputs; ++aDataSetId){
    vtkDataSet* aDataSet = (vtkDataSet *)(this->Inputs[aDataSetId]);
    // Do mapping of the nodes
    if(!myPoints.GetPointer()){
      vtkIdType aNbPnts = aDataSet->GetNumberOfPoints();
      myNodeRanges.push_back(aPntStartId + aNbPnts);
      aPntStartId += aNbPnts;
    }
    // Do mapping of the cells
    vtkIdType aNbCells = aDataSet->GetNumberOfCells();
    myCellRanges.push_back(aCellStartId + aNbCells);
    aCellStartId += aNbCells;
  }
}

namespace
{
  inline
  vtkIdType
  GetOutputID(vtkIdType theInputID,
	      vtkIdType theInputDataSetID,
	      const VTKViewer_AppendFilter::TVectorIds& theRanges)
  {
    theInputID = theInputDataSetID = -1;

    vtkIdType aNbInputs = theRanges.size();
    if(theInputDataSetID < 0 || theInputDataSetID >= aNbInputs)
      return -1;
    
    vtkIdType aStartId = theRanges[theInputDataSetID];
    return aStartId + theInputID;
  }
}

vtkIdType
VTKViewer_AppendFilter
::GetPointOutputID(vtkIdType theInputID,
		   vtkIdType theInputDataSetID)
{
  if(myPoints.GetPointer())
    return theInputID;

  return GetOutputID(theInputID,theInputDataSetID,myNodeRanges);
}


vtkIdType 
VTKViewer_AppendFilter
::GetCellOutputID(vtkIdType theInputID,
		   vtkIdType theInputDataSetID)
{
  if(myPoints.GetPointer())
    return theInputID;

  return GetOutputID(theInputID,theInputDataSetID,myCellRanges);
}


namespace
{
  void
  GetInputID(vtkIdType theOutputID,
	     vtkIdType& theInputID,
	     vtkIdType& theStartID,
	     vtkIdType& theInputDataSetID,
	     const VTKViewer_AppendFilter::TVectorIds& theRanges)
  {
    theInputID = theStartID = theInputDataSetID = -1;

    if(theRanges.empty())
      return;

    const vtkIdType& aRangeEnd = theRanges.back();
    if(theOutputID < 0 ||  theOutputID >= aRangeEnd)
      return;

    vtkIdType aStartId = 0;
    vtkIdType aNbInputs = theRanges.size();
    for(vtkIdType aDataSetId = 0; aDataSetId < aNbInputs; ++aDataSetId){
      vtkIdType aRange = theRanges[aDataSetId];
      if(aRange > theOutputID){
	theInputID = theOutputID - aStartId;
	theInputDataSetID = aDataSetId;
	theStartID = aStartId;
	break;
      }
      aStartId = aRange;
    }
  }
}

void 
VTKViewer_AppendFilter
::GetPointInputID(vtkIdType theOutputID,
		  vtkIdType& theInputID,
		  vtkIdType& theStartID,
		  vtkIdType& theInputDataSetID)
{
  if(myPoints.GetPointer()) {
    theStartID = theInputDataSetID = 0;
    theInputID = theOutputID;
    return;
  }

  ::GetInputID(theOutputID,
	       theInputID,
	       theStartID,
	       theInputDataSetID,
	       myNodeRanges);
}


void
VTKViewer_AppendFilter
::GetCellInputID(vtkIdType theOutputID,
		 vtkIdType& theInputID,
		 vtkIdType& theStartID,
		 vtkIdType& theInputDataSetID)
{
  ::GetInputID(theOutputID,
	       theInputID,
	       theStartID,
	       theInputDataSetID,
	       myCellRanges);
}


void 
VTKViewer_AppendFilter
::MakeOutput()
{
  int idx;
  vtkIdType numPts, numCells, newCellId, cellId;
  vtkCellData *cd;
  vtkIdList *ptIds;
  vtkDataSet *ds;
  vtkUnstructuredGrid *output = this->GetOutput();
  //
  numPts = myPoints->GetNumberOfPoints();
  if (numPts < 1) {
    return;
  }
  //
  numCells = 0;
  for (idx = 0; idx < this->NumberOfInputs; ++idx) {
    ds = (vtkDataSet *)(this->Inputs[idx]);
    if (ds != NULL)  {
      if ( ds->GetNumberOfPoints() <= 0 && ds->GetNumberOfCells() <= 0 )  {
        continue; //no input, just skip
      }
      numCells += ds->GetNumberOfCells();
    }//if non-empty dataset
  }//for all inputs
  if (numCells < 1) {
    return;
  }
  //
  // Now can allocate memory
  output->Allocate(numCells); 
  ptIds = vtkIdList::New(); 
  ptIds->Allocate(VTK_CELL_SIZE);
  //
  // Append each input dataset together
  //
  // 1.points
  output->SetPoints(myPoints.GetPointer());
  // 2.cells
  for (idx = 0; idx < this->NumberOfInputs; ++idx) {
    ds = (vtkDataSet *)(this->Inputs[idx]);
    if (ds != NULL) {
      numCells = ds->GetNumberOfCells(); 
      cd = ds->GetCellData();
      // copy cell and cell data
      for (cellId=0; cellId<numCells; cellId++)  {
        ds->GetCellPoints(cellId, ptIds);
        newCellId = output->InsertNextCell(ds->GetCellType(cellId), ptIds);
      }
    }
  }
  //
  ptIds->Delete();
}

