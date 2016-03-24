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

//  SALOME FILTER : interactive object for VISU entities implementation
//  File   : SALOME_TransformFilter.h
//  Author : Laurent CORNABE with help of Nicolas REJNERI
//  Module : SALOME
//
#include "VTKViewer_TransformFilter.h"
#include "VTKViewer_Transform.h"

#include <vtkObjectFactory.h>
#include <vtkPointSet.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkPoints.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

vtkStandardNewMacro(VTKViewer_TransformFilter);

/*!Execution method. Calculate output.*/
int VTKViewer_TransformFilter::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPointSet *input = vtkPointSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPointSet *output = vtkPointSet::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPoints *inPts;
  vtkPoints *newPts;
  int numPts/*, numCells*/;
  vtkPointData *pd=input->GetPointData(), *outPD=output->GetPointData();
  vtkCellData *cd=input->GetCellData(), *outCD=output->GetCellData();
  output->CopyStructure( input );
  if(Transform){
    bool anIsIdentity = true;
    if(VTKViewer_Transform* aTransform = dynamic_cast<VTKViewer_Transform*>(Transform))
      anIsIdentity = aTransform->IsIdentity() != 0;
    inPts = input->GetPoints();
    if(!anIsIdentity && inPts){
      numPts = inPts->GetNumberOfPoints();
      // numCells = input->GetNumberOfCells();
      newPts = vtkPoints::New();
      newPts->Allocate(numPts);
      this->UpdateProgress(.2);
      this->Transform->TransformPoints(inPts,newPts);
      this->UpdateProgress(.8);
      output->SetPoints(newPts);
      newPts->Delete();
    }
  }
  outPD->PassData(pd);
  outCD->PassData(cd);
  InvokeEvent("VTKViewer_TransformFilter::TransformationFinished");
  
  return 1;
}
