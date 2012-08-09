// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File:    VTKViewer_ExtractUnstructuredGrid.cxx
// Author:  Alexey PETROV

#include "VTKViewer_ExtractUnstructuredGrid.h"
#include "VTKViewer_CellLocationsArray.h"

#include <vtkUnsignedCharArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkObjectFactory.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkCell.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkVersion.h>

#include "utilities.h"

#if defined __GNUC__
  #if __GNUC__ == 2
    #define __GNUC_2__
  #endif
#endif

#define VTK_XVERSION (VTK_MAJOR_VERSION*10000+VTK_MINOR_VERSION*100+VTK_BUILD_VERSION)

vtkStandardNewMacro(VTKViewer_ExtractUnstructuredGrid);


VTKViewer_ExtractUnstructuredGrid::VTKViewer_ExtractUnstructuredGrid():
  myExtractionMode(eCells), myChangeMode(ePassAll)
{}


VTKViewer_ExtractUnstructuredGrid::~VTKViewer_ExtractUnstructuredGrid(){}


void VTKViewer_ExtractUnstructuredGrid::RegisterCell(vtkIdType theCellId){
//  if(0 && MYDEBUG) MESSAGE("RegisterCell - theCellId = "<<theCellId);
  myCellIds.insert(theCellId);
  Modified();
}


void VTKViewer_ExtractUnstructuredGrid::RegisterCellsWithType(vtkIdType theCellType){
//  if(0 && MYDEBUG) MESSAGE("RegisterCellsWithType - theCellType = "<<theCellType);
  myCellTypes.insert(theCellType);
  //MESSAGE("myCellTypes.insert " << theCellType);
  Modified();
}


void VTKViewer_ExtractUnstructuredGrid::SetStoreMapping(int theStoreMapping){
  myStoreMapping = theStoreMapping != 0;
  this->Modified();
}

vtkIdType VTKViewer_ExtractUnstructuredGrid::GetInputId(int theOutId) const
{
  if ( myCellIds.empty() && myCellTypes.empty() )
    return theOutId;

  if ( theOutId<0 || theOutId >= (int)myOut2InId.size() )
    return -1;
  return myOut2InId[theOutId];
}

vtkIdType VTKViewer_ExtractUnstructuredGrid::GetOutputId(int theInId) const{
  if(myCellIds.empty() && myCellTypes.empty()) return theInId;
  TMapId::const_iterator anIter = myIn2OutId.find(theInId);
  if(anIter == myIn2OutId.end()) return -1;
  return anIter->second;
}


inline void InsertCell(vtkUnstructuredGrid *theInput,
                       vtkCellArray *theConnectivity, 
                       vtkUnsignedCharArray* theCellTypesArray,
                       vtkIdTypeArray*& theFaces,
                       vtkIdTypeArray*& theFaceLocations,
                       vtkIdType theCellId, 
                       vtkIdList *theIdList,
                       bool theStoreMapping,
                       vtkIdType theOutId, 
                       VTKViewer_ExtractUnstructuredGrid::TVectorId& theOut2InId,
                       VTKViewer_ExtractUnstructuredGrid::TMapId& theIn2OutId)
{
  vtkCell *aCell = theInput->GetCell(theCellId);
  vtkIdList *aPntIds = aCell->GetPointIds();
  vtkIdType aNbIds = aPntIds->GetNumberOfIds();
  theIdList->SetNumberOfIds(aNbIds);
  for(vtkIdType i = 0; i < aNbIds; i++){
    theIdList->SetId(i,aPntIds->GetId(i));
  }
  vtkIdType aCellType = aCell->GetCellType();
#if VTK_XVERSION > 50700
  if (aCellType != VTK_POLYHEDRON)
    {
#endif
      theConnectivity->InsertNextCell(theIdList);
      if (theFaceLocations)
        theFaceLocations->InsertNextValue(-1);
#if VTK_XVERSION > 50700
    }
  else
    {
      //MESSAGE("InsertCell type VTK_POLYHEDRON " << theStoreMapping);
      if (!theFaces)
        {
          theFaces = vtkIdTypeArray::New();
          theFaces->Allocate(theCellTypesArray->GetSize());
          theFaceLocations = vtkIdTypeArray::New();
          theFaceLocations->Allocate(theCellTypesArray->GetSize());
          // FaceLocations must be padded until the current position
          for (vtkIdType i = 0; i <= theCellTypesArray->GetMaxId(); i++)
            {
              theFaceLocations->InsertNextValue(-1);
            }
        }
      // insert face location
      theFaceLocations->InsertNextValue(theFaces->GetMaxId() + 1);

      // insert cell connectivity and faces stream
      vtkIdType nfaces;
      vtkIdType* face;
      vtkIdType realnpts;
      theInput->GetFaceStream(theCellId, nfaces, face);
      vtkUnstructuredGrid::DecomposeAPolyhedronCell(
          nfaces, face, realnpts, theConnectivity, theFaces);
    }
#endif

  theCellTypesArray->InsertNextValue(aCellType);
  if(theStoreMapping){
    theOut2InId.push_back(theCellId);
    theIn2OutId[theCellId] = theOutId;
  }
}

inline void InsertPointCell(vtkCellArray *theConnectivity, 
                            vtkUnsignedCharArray* theCellTypesArray,
                            vtkIdType theCellId,
                            vtkIdList *theIdList,
                            bool theStoreMapping,
                            vtkIdType theOutId, 
                            VTKViewer_ExtractUnstructuredGrid::TVectorId& theOut2InId,
                            VTKViewer_ExtractUnstructuredGrid::TMapId& theIn2OutId)
{
  theIdList->SetId(0,theCellId);
  theConnectivity->InsertNextCell(theIdList);
  theCellTypesArray->InsertNextValue(VTK_VERTEX);
  if(theStoreMapping){
    theOut2InId.push_back(theCellId);
    theIn2OutId[theCellId] = theOutId;
  }
}


// int VTKViewer_ExtractUnstructuredGrid::RequestData(
//   vtkInformation *vtkNotUsed(request),
//   vtkInformationVector **inputVector,
//   vtkInformationVector *outputVector)
void VTKViewer_ExtractUnstructuredGrid::Execute()
{
  /*
  not ported yet to the new executive-based pipeline architecture.

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkUnstructuredGrid *anInput = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *anOutput = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
  */
  vtkUnstructuredGrid *anInput = this->GetInput();
  vtkUnstructuredGrid *anOutput = this->GetOutput();
  
  myOut2InId.clear();  myIn2OutId.clear();

/*  if(MYDEBUG){
    MESSAGE("Execute - anInput->GetNumberOfCells() = "<<anInput->GetNumberOfCells());
    MESSAGE("Execute - myCellTypes.size() = "<<myCellTypes.size());
    MESSAGE("Execute - myCellIds.size() = "<<myCellIds.size());
    MESSAGE("Execute - myExtractionMode = "<<myExtractionMode);
    MESSAGE("Execute - myChangeMode = "<<myChangeMode);
  }*/
  if(myExtractionMode == eCells){
    if(myChangeMode == ePassAll || (myCellIds.empty() && myCellTypes.empty() && myChangeMode == eRemoving)){
      if(vtkIdType aNbElems = anInput->GetNumberOfCells()){
        if(myStoreMapping) myOut2InId.reserve(aNbElems);
        anOutput->ShallowCopy(anInput);
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          if(myStoreMapping){
            myOut2InId.push_back(aCellId);
            myIn2OutId[aCellId] = anOutId;
          }
        }
      }
    }else{
      vtkIdList *anIdList = vtkIdList::New();
      vtkCellArray *aConnectivity = vtkCellArray::New();
      vtkIdType aNbElems = anInput->GetNumberOfCells();
      aConnectivity->Allocate(2*aNbElems,0);
      vtkUnsignedCharArray* aCellTypesArray = vtkUnsignedCharArray::New();
      aCellTypesArray->SetNumberOfComponents(1);
      aCellTypesArray->Allocate(aNbElems*aCellTypesArray->GetNumberOfComponents());

      vtkIdTypeArray *newFaces = 0;
      vtkIdTypeArray *newFaceLocations = 0;

      if(!myCellIds.empty() && myCellTypes.empty()){
        if(myStoreMapping) myOut2InId.reserve(myCellIds.size());
        if(myChangeMode == eAdding){
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            if(myCellIds.find(aCellId) != myCellIds.end()){
              InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                         myStoreMapping,anOutId,myOut2InId,myIn2OutId);
            }
          }
        }else{
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            if(myCellIds.find(aCellId) == myCellIds.end()){
              InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                         myStoreMapping,anOutId,myOut2InId,myIn2OutId);
            }
          }
        }
      }else if(myCellIds.empty() && !myCellTypes.empty()){
        if(myChangeMode == eAdding){
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            vtkIdType aType = anInput->GetCellType(aCellId);
            if(myCellTypes.find(aType) != myCellTypes.end()){
              InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                         myStoreMapping,anOutId,myOut2InId,myIn2OutId);
            }
          }
        }else{
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            vtkIdType aType = anInput->GetCellType(aCellId);
            if(myCellTypes.find(aType) == myCellTypes.end()){
              InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                         myStoreMapping,anOutId,myOut2InId,myIn2OutId);
            }
          }
        }
      }else if(!myCellIds.empty() && !myCellTypes.empty()){
        if(myChangeMode == eAdding){
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            vtkIdType aType = anInput->GetCellType(aCellId);
            if(myCellTypes.find(aType) != myCellTypes.end()){
              if(myCellIds.find(aCellId) != myCellIds.end()){
                InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                           myStoreMapping,anOutId,myOut2InId,myIn2OutId);
              }
            }
          }
        }else{
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            vtkIdType aType = anInput->GetCellType(aCellId);
            if(myCellTypes.find(aType) == myCellTypes.end()){
              if(myCellIds.find(aCellId) == myCellIds.end()){
                InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                           myStoreMapping,anOutId,myOut2InId,myIn2OutId);
              }
            }
          }
        }
      }
      if((aNbElems = aConnectivity->GetNumberOfCells())){
        VTKViewer_CellLocationsArray* aCellLocationsArray = VTKViewer_CellLocationsArray::New();
        aCellLocationsArray->SetNumberOfComponents(1);
        aCellLocationsArray->SetNumberOfTuples(aNbElems);
        aConnectivity->InitTraversal();
        for(vtkIdType i = 0, *pts, npts; aConnectivity->GetNextCell(npts,pts); i++){
          aCellLocationsArray->SetValue(i,aConnectivity->GetTraversalLocation(npts));
        }
#if VTK_XVERSION > 50700
        anOutput->SetCells(aCellTypesArray,aCellLocationsArray,aConnectivity,newFaceLocations,newFaces);
#else
        anOutput->SetCells(aCellTypesArray,aCellLocationsArray,aConnectivity);
#endif
        anOutput->SetPoints(anInput->GetPoints());
        aCellLocationsArray->Delete();
      }
      aCellTypesArray->Delete();
      aConnectivity->Delete();
      anIdList->Delete();
    }
  }else{
    vtkIdList *anIdList = vtkIdList::New();
    anIdList->SetNumberOfIds(1);
    vtkCellArray *aConnectivity = vtkCellArray::New();
    vtkIdType aNbElems = anInput->GetNumberOfPoints();
    aConnectivity->Allocate(2*aNbElems,0);
    vtkUnsignedCharArray* aCellTypesArray = vtkUnsignedCharArray::New();
    aCellTypesArray->SetNumberOfComponents(1);
    aCellTypesArray->Allocate(aNbElems*aCellTypesArray->GetNumberOfComponents());
    // additional condition has been added to treat a case described in IPAL21372
    // note that it is significant only when myExtractionMode == ePoints
    if(myChangeMode == ePassAll || (myCellIds.empty() && myCellTypes.empty() && myChangeMode == eRemoving) ||
       !anInput->GetCellTypesArray()){
      if(myStoreMapping) myOut2InId.reserve(aNbElems);
      for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
        InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                        myStoreMapping,anOutId,myOut2InId,myIn2OutId);
      }
    }else if(!myCellIds.empty() && myCellTypes.empty()){
      if(myStoreMapping) myOut2InId.reserve(myCellIds.size());
      if(myChangeMode == eAdding){
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          if(myCellIds.find(aCellId) != myCellIds.end()){
            InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                            myStoreMapping,anOutId,myOut2InId,myIn2OutId);
          }
        }
      }else{
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          if(myCellIds.find(aCellId) == myCellIds.end()){
            InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                            myStoreMapping,anOutId,myOut2InId,myIn2OutId);
          }
        }
      }
    }else if(myCellIds.empty() && !myCellTypes.empty()){
      if(myChangeMode == eAdding){
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          vtkIdType aType = anInput->GetCellType(aCellId);
          if(myCellTypes.find(aType) != myCellTypes.end()){
            InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                            myStoreMapping,anOutId,myOut2InId,myIn2OutId);
          }
        }
      }else{
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          vtkIdType aType = anInput->GetCellType(aCellId);
          if(myCellTypes.find(aType) == myCellTypes.end()){
            InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                            myStoreMapping,anOutId,myOut2InId,myIn2OutId);
          }
        }
      }
    }else if(!myCellIds.empty() && !myCellTypes.empty()){
      if(myChangeMode == eAdding){
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          vtkIdType aType = anInput->GetCellType(aCellId);
          if(myCellTypes.find(aType) != myCellTypes.end()){
            if(myCellIds.find(aCellId) != myCellIds.end()){
              InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                              myStoreMapping,anOutId,myOut2InId,myIn2OutId);
            }
          }
        }
      }else{
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          vtkIdType aType = anInput->GetCellType(aCellId);
          if(myCellTypes.find(aType) == myCellTypes.end()){
            if(myCellIds.find(aCellId) == myCellIds.end()){
              InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                              myStoreMapping,anOutId,myOut2InId,myIn2OutId);
            }
          }
        }
      }
    }
    if((aNbElems = aConnectivity->GetNumberOfCells())){
      VTKViewer_CellLocationsArray* aCellLocationsArray = VTKViewer_CellLocationsArray::New();
      aCellLocationsArray->SetNumberOfComponents(1);
      aCellLocationsArray->SetNumberOfTuples(aNbElems);
      aConnectivity->InitTraversal();
      for(vtkIdType i = 0, *pts, npts; aConnectivity->GetNextCell(npts,pts); i++){
        aCellLocationsArray->SetValue(i,aConnectivity->GetTraversalLocation(npts));
      }
#if VTK_XVERSION > 50700
      anOutput->SetCells(aCellTypesArray,aCellLocationsArray,aConnectivity,0, 0);
#else
      anOutput->SetCells(aCellTypesArray,aCellLocationsArray,aConnectivity);
#endif
      anOutput->SetPoints(anInput->GetPoints());
      aCellLocationsArray->Delete();
    }
    aCellTypesArray->Delete();
    aConnectivity->Delete();
    anIdList->Delete();
  }
/*  if(MYDEBUG){
    MESSAGE("Execute - anOutput->GetNumberOfCells() = "<<anOutput->GetNumberOfCells());
    if(myStoreMapping){
      MESSAGE("Execute - myOut2InId.size() = "<<myOut2InId.size());
      MESSAGE("Execute - myIn2OutId.size() = "<<myIn2OutId.size());
    }
  }*/
//  return 1;
}
