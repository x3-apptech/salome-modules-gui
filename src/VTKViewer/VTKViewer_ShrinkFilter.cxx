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
//  File   : SALOME_GeometryFilter.cxx
//  Author : Michael ZORIN

#include "VTKViewer_ShrinkFilter.h"

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkIdList.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

vtkStandardNewMacro(VTKViewer_ShrinkFilter);

/*!Constructor. Sets store mapping to zero.*/
VTKViewer_ShrinkFilter::VTKViewer_ShrinkFilter(): 
  myStoreMapping(0)
{}

/*!Destructor.*/
VTKViewer_ShrinkFilter::~VTKViewer_ShrinkFilter()
{}


/*!Execute method. Calculate output.*/
int VTKViewer_ShrinkFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPoints *newPts;
  int i, j, numIds, abort=0;
  vtkIdType cellId, numCells, numPts;
  vtkIdType oldId, newId;
  double center[3], *p, pt[3];
  vtkPointData *pd, *outPD;;
  vtkIdList *ptIds, *newPtIds;
  vtkIdType tenth;
  double decimal;

  vtkDebugMacro(<<"Shrinking cells");

  numCells=input->GetNumberOfCells();
  numPts = input->GetNumberOfPoints();
  if (numCells < 1 || numPts < 1)
    {
    vtkErrorMacro(<<"No data to shrink!");
    return 0;
    }

  ptIds = vtkIdList::New();
  ptIds->Allocate(VTK_CELL_SIZE);
  newPtIds = vtkIdList::New();
  newPtIds->Allocate(VTK_CELL_SIZE);

  output->Allocate(numCells);
  newPts = vtkPoints::New();
  newPts->Allocate(numPts*8,numPts);
  pd = input->GetPointData();
  outPD = output->GetPointData();
  outPD->CopyAllocate(pd,numPts*8,numPts);

  // Traverse all cells, obtaining node coordinates.  Compute "center" of cell,
  // then create new vertices shrunk towards center.
  //
  tenth   = numCells/10 + 1;
  decimal = 0.0;
  if(myStoreMapping){
    myVTK2ObjIds.clear();
    myVTK2ObjIds.reserve(numCells);
  }

  for (cellId=0; cellId < numCells && !abort; cellId++)
    {
    input->GetCellPoints(cellId, ptIds);
    numIds = ptIds->GetNumberOfIds();

    //abort/progress methods
    if (cellId % tenth == 0) 
      {
      decimal += 0.1;
      this->UpdateProgress (decimal);
      abort = this->GetAbortExecute();
      }

    // get the center of the cell
    center[0] = center[1] = center[2] = 0.0;
    for (i=0; i < numIds; i++)
      {
      p = input->GetPoint(ptIds->GetId(i));
      for (j=0; j < 3; j++)
        {
        center[j] += p[j];
        }
      }
    for (j=0; j<3; j++)
      {
      center[j] /= numIds;
      }

    // Create new points and cells
    newPtIds->Reset();
    for (i=0; i < numIds; i++)
      {
      p = input->GetPoint(ptIds->GetId(i));
      for (j=0; j < 3; j++)
        {
        pt[j] = center[j] + this->ShrinkFactor*(p[j] - center[j]);
        }

      oldId = ptIds->GetId(i);
      newId = newPts->InsertNextPoint(pt);
      if(myStoreMapping)
        myVTK2ObjIds.push_back(oldId);
      newPtIds->InsertId(i,newId);

      outPD->CopyData(pd, oldId, newId);
      }
    output->InsertNextCell(input->GetCellType(cellId), newPtIds);
    }//for all cells

  // Update ourselves and release memory
  //
  output->GetCellData()->PassData(input->GetCellData());

  output->SetPoints(newPts);
  output->Squeeze();

  ptIds->Delete();
  newPtIds->Delete();
  newPts->Delete();
  
  return 1;
}

/*!Sets store mapping.*/
void VTKViewer_ShrinkFilter::SetStoreMapping(int theStoreMapping){
  myStoreMapping = theStoreMapping;
  this->Modified();
}


/*!Return node object id by vtk node id.
 *\retval -1 - if no object, else return id.
 */
vtkIdType VTKViewer_ShrinkFilter::GetNodeObjId(int theVtkID)
{
  if ( myVTK2ObjIds.empty() || theVtkID > (int)myVTK2ObjIds.size() )
    return -1;
  return myVTK2ObjIds.at(theVtkID);
}
