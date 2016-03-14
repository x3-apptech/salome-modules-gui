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

#include "VTKViewer_CellCenters.h"

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>

vtkStandardNewMacro(VTKViewer_CellCenters);

/*!
 * Class       : VTKViewer_CellCenters
 * Description : Filter computing geometrical centers of given cells
 *               (differs from native vtk filter by small fix for VTK_CONVEX_POINT_SET cells)
 */

/*!
  Constructor
*/
VTKViewer_CellCenters::VTKViewer_CellCenters()
{
}

/*!
  Redefined main method
*/
int VTKViewer_CellCenters::RequestData(
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
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdType cellId, numCells;
  int subId;
  vtkCellData *inCD;
  vtkPointData *outPD;
  vtkPoints *newPts;
  vtkCell *cell;
  double x[3], pcoords[3];
  double *weights;

  inCD=input->GetCellData();
  outPD=output->GetPointData();

  if ( (numCells = input->GetNumberOfCells()) < 1 )
    {
    vtkWarningMacro(<<"No cells to generate center points for");
    return 1;
    }

  newPts = vtkPoints::New();
  newPts->SetNumberOfPoints(numCells);
  weights = new double [input->GetMaxCellSize()];

  int abort=0;
  vtkIdType progressInterval = numCells/10 + 1;
  int hasEmptyCells = 0;
  for (cellId=0; cellId < numCells && !abort; cellId++)
    {
    if ( ! (cellId % progressInterval) ) 
      {
      vtkDebugMacro(<<"Processing #" << cellId);
      this->UpdateProgress (0.5*cellId/numCells);
      abort = this->GetAbortExecute();
      }

    cell = input->GetCell(cellId);
    if (cell->GetCellType() != VTK_EMPTY_CELL)
      {
        // fix for VTK_CONVEX_POINT_SET cells
        if (cell->GetCellType() == VTK_CONVEX_POINT_SET )
        {
          x[0] = x[1] = x[2] = 0;
          vtkPoints* aPoints = cell->GetPoints();
          int aNbPoints = aPoints->GetNumberOfPoints();
          for( int i = 0; i < aNbPoints; i++ )
          {
            double aCoord[3];
            aPoints->GetPoint( i, aCoord );
            x[0] += aCoord[0];
            x[1] += aCoord[1];
            x[2] += aCoord[2];
          }
          x[0] /= aNbPoints;
          x[1] /= aNbPoints;
          x[2] /= aNbPoints;
        }
        else
        {
          subId = cell->GetParametricCenter(pcoords);
          cell->EvaluateLocation(subId, pcoords, x, weights);
        }
        newPts->SetPoint(cellId,x);
      }
    else
      {
      hasEmptyCells = 1;
      }
    }

  if ( this->VertexCells )
    {
    vtkIdType pts[1];
    vtkCellData *outCD=output->GetCellData();
    vtkCellArray *verts = vtkCellArray::New();
    verts->Allocate(verts->EstimateSize(1,numCells),1);

    for (cellId=0; cellId < numCells && !abort; cellId++)
      {
      if ( ! (cellId % progressInterval) ) 
        {
        vtkDebugMacro(<<"Processing #" << cellId);
        this->UpdateProgress (0.5+0.5*cellId/numCells);
        abort = this->GetAbortExecute();
        }

      cell = input->GetCell(cellId);
      if (cell->GetCellType() != VTK_EMPTY_CELL)
        {
        pts[0] = cellId;
        verts->InsertNextCell(1,pts);
        }
      }

    output->SetVerts(verts);
    verts->Delete();
    if (!hasEmptyCells)
      {
      outCD->PassData(inCD); //only if verts are generated
      }
    }

  // clean up and update output
  output->SetPoints(newPts);
  newPts->Delete();

  if (!hasEmptyCells)
    {
    outPD->PassData(inCD); //because number of points = number of cells
    }
  if (weights)
    {
    delete [] weights;
    }

  return 1;
}
