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

#include "SVTK_Actor.h"
#include "SALOME_Actor.h"
#include "SVTK_Utils.h"

#include "SALOME_InteractiveObject.hxx"

// VTK Includes
#include <vtkObjectFactory.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderer.h>

#include <vtkCell.h>
#include <vtkVersion.h>
#define VTK_XVERSION (VTK_MAJOR_VERSION*10000+VTK_MINOR_VERSION*100+VTK_BUILD_VERSION)
#if VTK_XVERSION > 50700
#include <vtkPolyhedron.h>
#endif
#include <vtkPolyData.h>

#include "Utils_SALOME_Exception.hxx"
#include "utilities.h"


vtkStandardNewMacro(SVTK_Actor);

/*!
  Constructor
*/
SVTK_Actor
::SVTK_Actor():
  myUnstructuredGrid(vtkUnstructuredGrid::New())
{
  myIsShaded = true;
  myIsResolveCoincidentTopology = false;

  Visibility = Pickable = false;

  myUnstructuredGrid->Delete();
  myUnstructuredGrid->Allocate();
}

void
SVTK_Actor
::Initialize()
{
  SetInputData(GetSource());
}

void
SVTK_Actor
::SetSource(vtkUnstructuredGrid* theUnstructuredGrid)
{
  if(GetSource() == theUnstructuredGrid)
    return;

  myUnstructuredGrid = theUnstructuredGrid;

  SetInputData(theUnstructuredGrid);
}

vtkUnstructuredGrid*
SVTK_Actor
::GetSource()
{
  return myUnstructuredGrid.GetPointer();
}

/*!
  Destructor
*/
SVTK_Actor
::~SVTK_Actor()
{
}

const TColStd_IndexedMapOfInteger&
SVTK_Actor
::GetMapIndex() const
{
  return myMapIndex;
}

void
SVTK_Actor
::MapCells(SALOME_Actor* theMapActor,
           const TColStd_IndexedMapOfInteger& theMapIndex)
{
  myUnstructuredGrid->Initialize();
  myUnstructuredGrid->Allocate();

  vtkUnstructuredGrid * aSourceGrid = (vtkUnstructuredGrid *)theMapActor->GetInput();
  GetSource()->SetPoints( aSourceGrid->GetPoints() );

  int aNbOfParts = theMapIndex.Extent();
  for(int ind = 1; ind <= aNbOfParts; ind++){
    int aPartId = theMapIndex( ind );
    if(vtkCell* aCell = theMapActor->GetElemCell(aPartId))
      {
#if VTK_XVERSION > 50700
      if (aCell->GetCellType() != VTK_POLYHEDRON)
#endif
        myUnstructuredGrid->InsertNextCell(aCell->GetCellType(),aCell->GetPointIds());
#if VTK_XVERSION > 50700
      else
        {
          vtkPolyhedron *polyhedron = dynamic_cast<vtkPolyhedron*>(aCell);
          if (!polyhedron)
            throw SALOME_Exception(LOCALIZED ("not a polyhedron"));
          vtkIdType *pts = polyhedron->GetFaces();
          myUnstructuredGrid->InsertNextCell(aCell->GetCellType(),pts[0], pts+1);
        }
#endif
      }
  }

  UnShrink();
  if(theMapActor->IsShrunk()){
    SetShrinkFactor(theMapActor->GetShrinkFactor());
    SetShrink();
  }

  myMapIndex = theMapIndex;
}

void 
SVTK_Actor
::MapPoints(SALOME_Actor* theMapActor,
            const TColStd_IndexedMapOfInteger& theMapIndex)
{
  myUnstructuredGrid->Initialize();
  myUnstructuredGrid->Allocate();

  if(int aNbOfParts = theMapIndex.Extent()){
    vtkPoints *aPoints = vtkPoints::New();
    aPoints->SetNumberOfPoints(aNbOfParts);
    for(vtkIdType i = 0; i < aNbOfParts; i++){
      int aPartId = theMapIndex( i+1 );
      if(double* aCoord = theMapActor->GetNodeCoord(aPartId)){
        aPoints->SetPoint(i,aCoord);
        // Change the type from int to vtkIdType in order to avoid compilation errors while using VTK
        // from ParaView-3.4.0 compiled on 64-bit Debian platform with VTK_USE_64BIT_IDS = ON
        myUnstructuredGrid->InsertNextCell(VTK_VERTEX,(vtkIdType) 1,&i);
      }
    }
    myUnstructuredGrid->SetPoints(aPoints);
    aPoints->Delete();
  }

  UnShrink();

  myMapIndex = theMapIndex;
}

void
SVTK_Actor
::MapEdge(SALOME_Actor* theMapActor,
          const TColStd_IndexedMapOfInteger& theMapIndex)
{
  myUnstructuredGrid->Initialize();
  myUnstructuredGrid->Allocate();

  vtkUnstructuredGrid * aSourceGrid = (vtkUnstructuredGrid *)theMapActor->GetInput();
  GetSource()->SetPoints( aSourceGrid->GetPoints() );


  if(theMapIndex.Extent() == 2){
    int anEdgeId = theMapIndex(1) < 0 ? theMapIndex(1) : theMapIndex(2);
    int aCellId = theMapIndex(1) < 0 ? theMapIndex(2) : theMapIndex(1);

    if(aCellId > 0){
      if(vtkCell* aCell = theMapActor->GetElemCell(aCellId)){
        if(anEdgeId < 0){
          anEdgeId = -anEdgeId - 1;
          int aNbOfEdges = aCell->GetNumberOfEdges();
          if(0 <= anEdgeId || anEdgeId < aNbOfEdges){
            if(vtkCell* anEdge = aCell->GetEdge(anEdgeId))
              myUnstructuredGrid->InsertNextCell(VTK_LINE,anEdge->GetPointIds());
          }
        }
      }
    }
  }

  UnShrink();
  if(theMapActor->IsShrunk()){
    SetShrinkFactor(theMapActor->GetShrinkFactor());
    SetShrink();
  }

  myMapIndex = theMapIndex;
}

/*!
  To publish the actor an all its internal devices
*/
void
SVTK_Actor
::AddToRender(vtkRenderer* theRenderer)
{
  theRenderer->AddActor(this);
}

void
SVTK_Actor
::RemoveFromRender(vtkRenderer* theRenderer) 
{
  theRenderer->RemoveActor(this);
}
