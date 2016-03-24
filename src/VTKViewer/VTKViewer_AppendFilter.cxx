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
//  File   : VTKViewer_GeometryFilter.cxx
//  Author : 

#include "VTKViewer_AppendFilter.h"

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkDataSetAttributes.h>
#include <vtkDataSetCollection.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include <vtkPoints.h>

#if defined __GNUC__
  #if __GNUC__ == 2
    #define __GNUC_2__
  #endif
#endif

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
::SetSharedPointsDataSet(vtkPointSet* thePointsDataSet)
{
  if(GetSharedPointsDataSet() == thePointsDataSet)
    return;

  mySharedPointsDataSet = thePointsDataSet;

  Modified();
}

vtkPointSet*
VTKViewer_AppendFilter
::GetSharedPointsDataSet()
{
  return mySharedPointsDataSet.GetPointer();
}

int
VTKViewer_AppendFilter
::RequestData(
              vtkInformation *request,
              vtkInformationVector **inputVector,
              vtkInformationVector *outputVector)
{
  int aRet = 0;
  if(GetSharedPointsDataSet())
    aRet = MakeOutput(request,inputVector,outputVector);
  else
    aRet = Superclass::RequestData(request,inputVector,outputVector);

  if(myDoMappingFlag)
    DoMapping();
  
  return aRet;
}


void 
VTKViewer_AppendFilter
::DoMapping()
{
  myNodeRanges.clear();
  myCellRanges.clear();

  vtkIdType aPntStartId = 0;
  vtkIdType aCellStartId = 0;

  for(vtkIdType aDataSetId = 0; aDataSetId < this->GetNumberOfInputPorts(); ++aDataSetId){
    vtkDataSet* aDataSet = (vtkDataSet *)(this->GetInput(aDataSetId));
    // Do mapping of the nodes
    if(!GetSharedPointsDataSet()){
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
  if(GetSharedPointsDataSet())
    return theInputID;

  return GetOutputID(theInputID,theInputDataSetID,myNodeRanges);
}


vtkIdType 
VTKViewer_AppendFilter
::GetCellOutputID(vtkIdType theInputID,
                   vtkIdType theInputDataSetID)
{
  if(GetSharedPointsDataSet())
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
  if(GetSharedPointsDataSet()) {
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


int
VTKViewer_AppendFilter
::MakeOutput(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  int idx;
  vtkIdType numPts, numCells, cellId;
  // vtkCellData *cd;
  vtkIdList *ptIds;
  vtkDataSet *ds;
  int numInputs = this->GetNumberOfInputConnections(0);
  
  // get the output info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
  //
  numPts = mySharedPointsDataSet->GetNumberOfPoints();
  if (numPts < 1) {
    return 0;
  }
  //
  numCells = 0;
  vtkInformation *inInfo = 0;
  for (idx = 0; idx < numInputs;++idx) {
    inInfo = inputVector[0]->GetInformationObject(idx);
    ds = 0;
    if (inInfo)
      {
      ds = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
      }
    if (ds != NULL)  {
      if ( ds->GetNumberOfPoints() <= 0 && ds->GetNumberOfCells() <= 0 )  {
        continue; //no input, just skip
      }
      numCells += ds->GetNumberOfCells();
    }//if non-empty dataset
  }//for all inputs
  if (numCells < 1) {
    return 0;
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
  output->SetPoints(GetSharedPointsDataSet()->GetPoints());
  output->GetPointData()->PassData(GetSharedPointsDataSet()->GetPointData());
  // 2.cells
  for (idx = 0; idx < numInputs; ++idx) {
    inInfo = inputVector[0]->GetInformationObject(idx);
    ds = 0;
    if (inInfo)
      {
      ds = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
      }
    if (ds != NULL) {
      
      numCells = ds->GetNumberOfCells(); 
      // cd = ds->GetCellData();
      // copy cell and cell data
      for (cellId=0; cellId<numCells; cellId++)  {
        ds->GetCellPoints(cellId, ptIds);
        output->InsertNextCell(ds->GetCellType(cellId), ptIds);
      }
    }
  }
  //
  ptIds->Delete();
  return 1;
}

int VTKViewer_AppendFilter::FillInputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
  return 1;
}
