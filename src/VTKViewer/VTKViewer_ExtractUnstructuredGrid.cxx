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
#include <vtkCellData.h>
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
  myExtractionMode(eCells), myChangeMode(ePassAll), myStoreMapping( false ), myPassAll( false )
{}


VTKViewer_ExtractUnstructuredGrid::~VTKViewer_ExtractUnstructuredGrid()
{}

void VTKViewer_ExtractUnstructuredGrid::RegisterCell(vtkIdType theCellId)
{
  if ( myCellIds.insert(theCellId).second )
    Modified();
}


void VTKViewer_ExtractUnstructuredGrid::RegisterCellsWithType(vtkIdType theCellType)
{
  if ( myCellTypes.insert(theCellType).second )
    Modified();
}


void VTKViewer_ExtractUnstructuredGrid::SetStoreMapping(int theStoreMapping)
{
  if ( myStoreMapping != ( theStoreMapping != 0 ))
  {
    myStoreMapping = theStoreMapping != 0;
    Modified();
  }
}

vtkIdType VTKViewer_ExtractUnstructuredGrid::GetInputId(int theOutId) const
{
  if ( myPassAll || ( myCellIds.empty() && myCellTypes.empty() ))
    return theOutId;

  if ( theOutId<0 || theOutId >= (int)myOut2InId.size() )
    return -1;
  return myOut2InId[theOutId];
}

void VTKViewer_ExtractUnstructuredGrid::BuildOut2InMap()
{
  if ( myPassAll || !myOut2InId.empty() ) return;

  vtkUnstructuredGrid *anInput = dynamic_cast< vtkUnstructuredGrid*>( this->GetInput() );
  if ( !anInput ) return;

  // use a vector of cellTypes to avoid searching in myCellTypes map
  // for a better performance (IPAL53103)
  TVectorId cellTypesVec( VTK_NUMBER_OF_CELL_TYPES, -1 );
  for ( TSetId::iterator type = myCellTypes.begin(); type != myCellTypes.end(); ++type )
  {
    if ( *type >= (int)cellTypesVec.size() ) cellTypesVec.resize( *type+1, -1 );
    if ( *type > 0 )
      cellTypesVec[ *type ] = *type;
  }

  // same code as in RequestData() excluding cells copying
  vtkIdType aNbElems = 0;
  if ( myExtractionMode == eCells )
  {
    aNbElems = anInput->GetNumberOfCells();
    if (( myChangeMode == ePassAll ) ||
        ( myCellIds.empty() && myCellTypes.empty() && myChangeMode == eRemoving))
    {
      myPassAll = true;
    }
    else
    {
      if ( !myCellIds.empty() && myCellTypes.empty() )
      {
        if ( myChangeMode == eAdding )
        {
          myOut2InId.reserve(myCellIds.size());

          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
            if ( myCellIds.find(aCellId) != myCellIds.end() )
              myOut2InId.push_back( aCellId );
        }
        else
        {
          myOut2InId.reserve( std::max( vtkIdType(0), vtkIdType(aNbElems - myCellIds.size())));

          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
            if ( myCellIds.find(aCellId) == myCellIds.end() )
              myOut2InId.push_back( aCellId );
        }
      }
      else if ( myCellIds.empty() && !myCellTypes.empty() )
      {
        myOut2InId.reserve( aNbElems );
        if ( myChangeMode == eAdding )
        {
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
          {
            vtkIdType aType = anInput->GetCellType(aCellId);
            if ( cellTypesVec[ aType ] == aType )
              myOut2InId.push_back( aCellId );
          }
        }
        else
        {
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
          {
            vtkIdType aType = anInput->GetCellType(aCellId);
            if ( cellTypesVec[ aType ] != aType )
              myOut2InId.push_back( aCellId );
          }
        }
      }
      else if ( !myCellIds.empty() && !myCellTypes.empty())
      {
        myOut2InId.reserve( aNbElems );

        if ( myChangeMode == eAdding )
        {
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
          {
            vtkIdType aType = anInput->GetCellType(aCellId);
            if ( cellTypesVec[ aType ] == aType )
              if ( myCellIds.find(aCellId) != myCellIds.end() )
                myOut2InId.push_back( aCellId );
          }
        }
        else
        {
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
          {
            vtkIdType aType = anInput->GetCellType(aCellId);
            if ( cellTypesVec[ aType ] != aType )
              if ( myCellIds.find(aCellId) == myCellIds.end())
                myOut2InId.push_back( aCellId );
          }
        }
      }
    } // not ePassAll 
  }
  else // nodes
  {
    aNbElems = anInput->GetNumberOfPoints();

    if (( myChangeMode == ePassAll ) ||
        ( myCellIds.empty() && myCellTypes.empty() && myChangeMode == eRemoving) ||
        ( !anInput->GetCellTypesArray() ))
    {
      myPassAll = true;
    }
    else if ( !myCellIds.empty() && myCellTypes.empty())
    {
      if ( myChangeMode == eAdding )
      {
        myOut2InId.reserve( myCellIds.size() );

        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
          if ( myCellIds.find(aCellId) != myCellIds.end())
            myOut2InId.push_back( aCellId );
      }
      else
      {
        myOut2InId.reserve( std::max( vtkIdType(0), vtkIdType(aNbElems - myCellIds.size())));

        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
          if ( myCellIds.find(aCellId) == myCellIds.end())
            myOut2InId.push_back( aCellId );
      }
    }
    else if ( myCellIds.empty() && !myCellTypes.empty())
    {
      myOut2InId.reserve( aNbElems );

      if ( myChangeMode == eAdding )
      {
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
        {
          vtkIdType aType = anInput->GetCellType(aCellId);
          if ( cellTypesVec[ aType ] == aType )
            myOut2InId.push_back( aCellId );
        }
      }
      else
      {
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
        {
          vtkIdType aType = anInput->GetCellType(aCellId);
          if ( cellTypesVec[ aType ] != aType )
            myOut2InId.push_back( aCellId );
        }
      }
    }
    else if ( !myCellIds.empty() && !myCellTypes.empty() )
    {
      if ( myChangeMode == eAdding )
      {
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
        {
          vtkIdType aType = anInput->GetCellType(aCellId);
          if ( cellTypesVec[ aType ] == aType )
            if ( myCellIds.find(aCellId) != myCellIds.end())
              myOut2InId.push_back( aCellId );
        }
      }
      else
      {
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++)
        {
          vtkIdType aType = anInput->GetCellType(aCellId);
          if ( cellTypesVec[ aType ] != aType )
            if ( myCellIds.find(aCellId) == myCellIds.end())
              myOut2InId.push_back( aCellId );
        }
      }
    }
  } // nodes

  if ((vtkIdType) myOut2InId.size() == aNbElems )
  {
    myPassAll = true;
    TVectorId().swap( myOut2InId );
  }
  return;
}

// vtkIdType VTKViewer_ExtractUnstructuredGrid::GetOutputId(int theInId) const{
//   if(myCellIds.empty() && myCellTypes.empty()) return theInId;
//   TMapId::const_iterator anIter = myIn2OutId.find(theInId);
//   if(anIter == myIn2OutId.end()) return -1;
//   return anIter->second;
// }


inline int InsertCell(vtkUnstructuredGrid *theInput,
                      vtkCellArray *theConnectivity,
                      vtkUnsignedCharArray* theCellTypesArray,
                      vtkIdTypeArray*& theFaces,
                      vtkIdTypeArray*& theFaceLocations,
                      vtkIdType theCellId,
                      vtkIdList *theIdList,
                      bool theStoreMapping,
                      vtkIdType theOutId,
                      VTKViewer_ExtractUnstructuredGrid::TVectorId& theOut2InId/*,
                      VTKViewer_ExtractUnstructuredGrid::TMapId& theIn2OutId*/)
{
  vtkCell      *aCell = theInput->GetCell(theCellId);
  vtkIdType aCellType = aCell->GetCellType();
  vtkIdType  aCellId = -1;
#if VTK_XVERSION > 50700
  if (aCellType != VTK_POLYHEDRON)
  {
#endif
    aCellId = theConnectivity->InsertNextCell( aCell->GetPointIds() ); //theIdList);
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
    vtkIdType nfaces = 0;
    vtkIdType*  face = 0;
    vtkIdType realnpts;
    theInput->GetFaceStream(theCellId, nfaces, face);
    vtkUnstructuredGrid::DecomposeAPolyhedronCell(
                                                  nfaces, face, realnpts, theConnectivity, theFaces);
  }
#endif

  /*vtkIdType anID = */theCellTypesArray->InsertNextValue(aCellType);
  if(theStoreMapping){
    theOut2InId.push_back( theCellId );
    //theIn2OutId.insert( theIn2OutId.end(), std::make_pair( theCellId, theOutId ));
  }
  return aCellId;
}

inline void InsertPointCell(vtkCellArray *theConnectivity, 
                            vtkUnsignedCharArray* theCellTypesArray,
                            vtkIdType theCellId,
                            vtkIdList *theIdList,
                            bool theStoreMapping,
                            vtkIdType theOutId, 
                            VTKViewer_ExtractUnstructuredGrid::TVectorId& theOut2InId/*,
                            VTKViewer_ExtractUnstructuredGrid::TMapId& theIn2OutId*/)
{
  theIdList->SetId(0,theCellId);
  theConnectivity->InsertNextCell(theIdList);
  theCellTypesArray->InsertNextValue(VTK_VERTEX);
  if(theStoreMapping){
    theOut2InId.push_back(theCellId);
    //theIn2OutId.insert( theIn2OutId.end(), std::make_pair( theCellId, theOutId ));
  }
}


int VTKViewer_ExtractUnstructuredGrid::RequestData(vtkInformation *vtkNotUsed(request),
                                                   vtkInformationVector **inputVector,
                                                   vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkUnstructuredGrid *anInput =
    vtkUnstructuredGrid::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *anOutput =
    vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  //vtkUnstructuredGrid *anInput = this->GetInput();
  //vtkUnstructuredGrid *anOutput = this->GetOutput();

  myPassAll = false;
  TVectorId().swap( myOut2InId );

  // use a vector of cellTypes to avoid searching in myCellTypes map
  // for a better performance (IPAL53103)
  TVectorId cellTypesVec( VTK_NUMBER_OF_CELL_TYPES, -1 );
  for ( TSetId::iterator type = myCellTypes.begin(); type != myCellTypes.end(); ++type )
  {
    if ( *type >= (int)cellTypesVec.size() ) cellTypesVec.resize( *type+1, -1 );
    if ( *type > 0 )
      cellTypesVec[ *type ] = *type;
  }

/*  if(MYDEBUG){
    MESSAGE("Execute - anInput->GetNumberOfCells() = "<<anInput->GetNumberOfCells());
    MESSAGE("Execute - myCellTypes.size() = "<<myCellTypes.size());
    MESSAGE("Execute - myCellIds.size() = "<<myCellIds.size());
    MESSAGE("Execute - myExtractionMode = "<<myExtractionMode);
    MESSAGE("Execute - myChangeMode = "<<myChangeMode);
  }*/
  vtkIdType aNbElems = 0;
  if(myExtractionMode == eCells)
  {
    aNbElems = anInput->GetNumberOfCells();
    if(myChangeMode == ePassAll || (myCellIds.empty() && myCellTypes.empty() && myChangeMode == eRemoving)){
      myPassAll = true;
      if ( aNbElems > 0 ){
        anOutput->ShallowCopy(anInput);
        //if(myStoreMapping) myOut2InId.reserve(aNbElems);
        // if(myStoreMapping){
        //   for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
        //     myOut2InId.push_back(aCellId);
        //     //myIn2OutId.insert( myIn2OutId.end(), std::make_pair( aCellId, anOutId ));
        //   }
        // }
      }
    }else{
      vtkIdList *anIdList = vtkIdList::New();
      vtkCellArray *aConnectivity = vtkCellArray::New();
      aConnectivity->Allocate(2*aNbElems,0);
      vtkUnsignedCharArray* aCellTypesArray = vtkUnsignedCharArray::New();
      aCellTypesArray->SetNumberOfComponents(1);
      aCellTypesArray->Allocate(aNbElems*aCellTypesArray->GetNumberOfComponents());
      anOutput->GetCellData()->CopyAllocate(anInput->GetCellData(),aNbElems,aNbElems/2);

      vtkIdTypeArray *newFaces = 0;
      vtkIdTypeArray *newFaceLocations = 0;

      if(!myCellIds.empty() && myCellTypes.empty()){
        if(myStoreMapping) myOut2InId.reserve(myCellIds.size());
        if(myChangeMode == eAdding){
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            if(myCellIds.find(aCellId) != myCellIds.end()){
              vtkIdType newId = InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                                           myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
              anOutput->GetCellData()->CopyData(anInput->GetCellData(),aCellId,newId);
            }
          }
        }else{
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            if(myCellIds.find(aCellId) == myCellIds.end()){
              vtkIdType newId = InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                                           myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
              anOutput->GetCellData()->CopyData(anInput->GetCellData(),aCellId,newId);
            }
          }
        }
      }else if(myCellIds.empty() && !myCellTypes.empty()){
        if(myChangeMode == eAdding){
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            vtkIdType aType = anInput->GetCellType(aCellId);
            if ( cellTypesVec[ aType ] == aType ) {
              vtkIdType newId = InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                                           myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
              anOutput->GetCellData()->CopyData(anInput->GetCellData(),aCellId,newId);
            }
          }
        }else{
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            vtkIdType aType = anInput->GetCellType(aCellId);
            if ( cellTypesVec[ aType ] != aType ) {
              vtkIdType newId = InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                                           myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
              anOutput->GetCellData()->CopyData(anInput->GetCellData(),aCellId,newId);
            }
          }
        }
      }else if(!myCellIds.empty() && !myCellTypes.empty()){
        if(myChangeMode == eAdding){
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            vtkIdType aType = anInput->GetCellType(aCellId);
            if ( cellTypesVec[ aType ] == aType ) {
              if(myCellIds.find(aCellId) != myCellIds.end()){
                vtkIdType newId = InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                                             myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
                anOutput->GetCellData()->CopyData(anInput->GetCellData(),aCellId,newId);
              }
            }
          }
        }else{
          for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
            vtkIdType aType = anInput->GetCellType(aCellId);
            if ( cellTypesVec[ aType ] != aType ) {
              if(myCellIds.find(aCellId) == myCellIds.end()){
                vtkIdType newId = InsertCell(anInput,aConnectivity,aCellTypesArray,newFaces,newFaceLocations,aCellId,anIdList,
                                             myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
                anOutput->GetCellData()->CopyData(anInput->GetCellData(),aCellId,newId);
              }
            }
          }
        }
      }
      if ( vtkIdType newNbElems = aConnectivity->GetNumberOfCells() ) {
        VTKViewer_CellLocationsArray* aCellLocationsArray = VTKViewer_CellLocationsArray::New();
        aCellLocationsArray->SetNumberOfComponents(1);
        aCellLocationsArray->SetNumberOfTuples(newNbElems);
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
      if ( newFaceLocations ) newFaceLocations->Delete();
      if ( newFaces ) newFaces->Delete();
    }
  }
  else
  {
    vtkIdList *anIdList = vtkIdList::New();
    anIdList->SetNumberOfIds(1);
    vtkCellArray *aConnectivity = vtkCellArray::New();
    aNbElems = anInput->GetNumberOfPoints();
    aConnectivity->Allocate(2*aNbElems,0);
    vtkUnsignedCharArray* aCellTypesArray = vtkUnsignedCharArray::New();
    aCellTypesArray->SetNumberOfComponents(1);
    aCellTypesArray->Allocate(aNbElems*aCellTypesArray->GetNumberOfComponents());
    // additional condition has been added to treat a case described in IPAL21372
    // note that it is significant only when myExtractionMode == ePoints
    if(myChangeMode == ePassAll || (myCellIds.empty() && myCellTypes.empty() && myChangeMode == eRemoving) ||
       !anInput->GetCellTypesArray()){
      myPassAll = true;
      //if(myStoreMapping) myOut2InId.reserve(aNbElems);
      for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
        InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                        /*theStoreMapping=*/false,anOutId,myOut2InId/*,myIn2OutId*/);
      }
    }else if(!myCellIds.empty() && myCellTypes.empty()){
      if(myStoreMapping) myOut2InId.reserve(myCellIds.size());
      if(myChangeMode == eAdding){
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          if(myCellIds.find(aCellId) != myCellIds.end()){
            InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                            myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
          }
        }
      }else{
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          if(myCellIds.find(aCellId) == myCellIds.end()){
            InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                            myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
          }
        }
      }
    }else if(myCellIds.empty() && !myCellTypes.empty()){
      if(myChangeMode == eAdding){
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          vtkIdType aType = anInput->GetCellType(aCellId);
          if ( cellTypesVec[ aType ] == aType ) {
            InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                            myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
          }
        }
      }else{
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          vtkIdType aType = anInput->GetCellType(aCellId);
          if ( cellTypesVec[ aType ] != aType ) {
            InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                            myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
          }
        }
      }
    }else if(!myCellIds.empty() && !myCellTypes.empty()){
      if(myChangeMode == eAdding){
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          vtkIdType aType = anInput->GetCellType(aCellId);
          if ( cellTypesVec[ aType ] == aType ) {
            if(myCellIds.find(aCellId) != myCellIds.end()){
              InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                              myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
            }
          }
        }
      }else{
        for(vtkIdType aCellId = 0, anOutId = 0; aCellId < aNbElems; aCellId++,anOutId++){
          vtkIdType aType = anInput->GetCellType(aCellId);
          if ( cellTypesVec[ aType ] != aType ) {
            if(myCellIds.find(aCellId) == myCellIds.end()){
              InsertPointCell(aConnectivity,aCellTypesArray,aCellId,anIdList,
                              myStoreMapping,anOutId,myOut2InId/*,myIn2OutId*/);
            }
          }
        }
      }
    }
    if (vtkIdType newNbElems = aConnectivity->GetNumberOfCells() ) {
      VTKViewer_CellLocationsArray* aCellLocationsArray = VTKViewer_CellLocationsArray::New();
      aCellLocationsArray->SetNumberOfComponents(1);
      aCellLocationsArray->SetNumberOfTuples( newNbElems );
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

  if ( aNbElems == (vtkIdType) myOut2InId.size() )
  {
    myPassAll = true;
    TVectorId().swap( myOut2InId );
  }
/*  if(MYDEBUG){
    MESSAGE("Execute - anOutput->GetNumberOfCells() = "<<anOutput->GetNumberOfCells());
    if(myStoreMapping){
      MESSAGE("Execute - myOut2InId.size() = "<<myOut2InId.size());
      MESSAGE("Execute - myIn2OutId.size() = "<<myIn2OutId.size());
    }
  }*/
  return 1;
}
