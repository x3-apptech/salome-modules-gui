//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
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
//  File   : SALOME_Selection.cxx
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$


#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>


#include "SALOME_Actor.h"
#include "SVTK_ViewModel.h"
#include "SVTK_ViewWindow.h"

#include "SVTK_SelectorDef.h"

SVTK_SelectorDef
::SVTK_SelectorDef()
{
}

SVTK_SelectorDef
::~SVTK_SelectorDef()
{
}

void 
SVTK_SelectorDef
::SetSelectionMode(Selection_Mode theMode)
{
  mySelectionMode = theMode;
}

void 
SVTK_SelectorDef
::ClearIObjects() 
{
  myIO2Actors.clear();
  myIObjects.clear();
  myMapIOSubIndex.clear();
}

//----------------------------------------------------------------------------
bool
SVTK_SelectorDef
::IsSelected(const Handle(SALOME_InteractiveObject)& theIO) const
{
  return myIObjects.find(theIO) != myIObjects.end();
}

bool
SVTK_SelectorDef
::IsSelected(SALOME_Actor* theActor) const
{
  const Handle(SALOME_InteractiveObject) anIO = theActor->getIO();
  return IsSelected(anIO) && myIO2Actors.find(anIO) != myIO2Actors.end();
}

SALOME_Actor*
SVTK_SelectorDef
::GetActor(const Handle(SALOME_InteractiveObject)& theIO) const
{
  TIO2Actors::const_iterator anIter = myIO2Actors.find(theIO);
  if(anIter != myIO2Actors.end())
    return anIter->second.GetPointer();
  return NULL;
}

//----------------------------------------------------------------------------
bool 
SVTK_SelectorDef
::AddIObject(const Handle(SALOME_InteractiveObject)& theIO) 
{
  if(!IsSelected(theIO)){
    myIObjects.insert(theIO);
    return true;
  }
  return false;
}

bool 
SVTK_SelectorDef
::AddIObject(SALOME_Actor* theActor) 
{
  const Handle(SALOME_InteractiveObject) anIO = theActor->getIO();

  bool anIsIOBound = IsSelected(anIO);
  if(!anIsIOBound)
    myIObjects.insert(anIO);

  bool anIsActorBound = myIO2Actors.find(anIO) != myIO2Actors.end();
  if(!anIsActorBound)
    myIO2Actors[anIO] = theActor;
  
  return !anIsIOBound || !anIsActorBound;
}

//----------------------------------------------------------------------------
bool 
SVTK_SelectorDef
::RemoveIObject(const Handle(SALOME_InteractiveObject)& theIO) 
{
  bool anIsIOBound = myIObjects.find(theIO) != myIObjects.end();

  myIObjects.erase(theIO);
  myIO2Actors.erase(theIO);

  return anIsIOBound;
}

bool 
SVTK_SelectorDef
::RemoveIObject(SALOME_Actor* theActor) 
{
  const Handle(SALOME_InteractiveObject) anIO = theActor->getIO();

  bool anIsActorBound = myIO2Actors.find(anIO) != myIO2Actors.end();
  if(anIsActorBound)
    myIO2Actors.erase(anIO);

  return RemoveIObject(anIO) || anIsActorBound;
}

//----------------------------------------------------------------------------
const SALOME_ListIO& 
SVTK_SelectorDef
::StoredIObjects() const
{
  myIObjectList.Clear();
  TIObjects::const_iterator anIter = myIObjects.begin();
  TIObjects::const_iterator anIterEnd = myIObjects.end();
  for(; anIter != anIterEnd; anIter++)
    myIObjectList.Append(*anIter);

  return myIObjectList;
}

int
SVTK_SelectorDef
::IObjectCount() const
{
  return myIObjects.size();
}

bool 
SVTK_SelectorDef
::HasIndex( const Handle(SALOME_InteractiveObject)& theIO) const
{
  return myMapIOSubIndex.find(theIO) != myMapIOSubIndex.end();
}

void 
SVTK_SelectorDef
::GetIndex( const Handle(SALOME_InteractiveObject)& theIO, 
	    TColStd_IndexedMapOfInteger& theIndex)
{
  TMapIOSubIndex::const_iterator anIter = myMapIOSubIndex.find(theIO);
  if(anIter != myMapIOSubIndex.end())
    theIndex = anIter->second.myMap;
  else
    theIndex.Clear();
}

bool
SVTK_SelectorDef
::IsIndexSelected(const Handle(SALOME_InteractiveObject)& theIO, 
		  int theIndex) const
{
  TMapIOSubIndex::const_iterator anIter = myMapIOSubIndex.find(theIO);
  if(anIter != myMapIOSubIndex.end()){
    const TColStd_IndexedMapOfInteger& aMapIndex = anIter->second.myMap;
    return aMapIndex.Contains(theIndex);
  }

  return false;
}

static 
bool
removeIndex(TColStd_IndexedMapOfInteger& theMapIndex,
	    const int theIndex)
{
  int anId = theMapIndex.FindIndex(theIndex); // i==0 if Index is not in the MapIndex
  if(anId){
    // only the last key can be removed
    int aLastId = theMapIndex.FindKey(theMapIndex.Extent());
    if(aLastId == anId)
      theMapIndex.RemoveLast();
    else{
      TColStd_IndexedMapOfInteger aNewMap;
      aNewMap.ReSize(theMapIndex.Extent()-1);
      for(int j = 1; j <= theMapIndex.Extent(); j++){
        int anIndex = theMapIndex(j);
        if ( anIndex != theIndex )
          aNewMap.Add( anIndex );
      }
      theMapIndex = aNewMap;
    }
  }
  return anId;
}


bool
SVTK_SelectorDef
::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    const TColStd_IndexedMapOfInteger& theIndices, 
		    bool theIsModeShift)
{
  TMapIOSubIndex::iterator aMapIter = myMapIOSubIndex.find(theIO);
  if(aMapIter == myMapIOSubIndex.end()){
    TIndexedMapOfInteger anEmpty;
    aMapIter = myMapIOSubIndex.
      insert(TMapIOSubIndex::value_type(theIO,anEmpty)).first;
  }
  TColStd_IndexedMapOfInteger& aMapIndex = aMapIter->second.myMap;

  if(!theIsModeShift)
    aMapIndex.Clear();
  
  for(int i = 1, iEnd = theIndices.Extent(); i <= iEnd; i++)
    aMapIndex.Add(theIndices(i));
  
  if(aMapIndex.IsEmpty())
    myMapIOSubIndex.erase(theIO);

  return !aMapIndex.IsEmpty();
}


bool
SVTK_SelectorDef
::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    const TColStd_MapOfInteger& theIndices, 
		    bool theIsModeShift)
{
  TMapIOSubIndex::iterator aMapIter = myMapIOSubIndex.find(theIO);
  if(aMapIter == myMapIOSubIndex.end()){
    TIndexedMapOfInteger anEmpty;
    aMapIter = myMapIOSubIndex.
      insert(TMapIOSubIndex::value_type(theIO,anEmpty)).first;
  }
  TColStd_IndexedMapOfInteger& aMapIndex = aMapIter->second.myMap;

  if(!theIsModeShift)
    aMapIndex.Clear();
  
  TColStd_MapIteratorOfMapOfInteger anIter(theIndices);
  for(; anIter.More(); anIter.Next())
    aMapIndex.Add(anIter.Key());
  
  if(aMapIndex.IsEmpty())
    myMapIOSubIndex.erase(theIO);

  return !aMapIndex.IsEmpty();
}


bool 
SVTK_SelectorDef
::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    int theIndex, 
		    bool theIsModeShift)
{
  TMapIOSubIndex::iterator anIter = myMapIOSubIndex.find(theIO);
  if(anIter == myMapIOSubIndex.end()){
    TIndexedMapOfInteger anEmpty;
    anIter = myMapIOSubIndex.
      insert(TMapIOSubIndex::value_type(theIO,anEmpty)).first;
  }
  TColStd_IndexedMapOfInteger& aMapIndex = anIter->second.myMap;

  bool anIsConatains = aMapIndex.Contains(theIndex);
  if(anIsConatains)
    removeIndex(aMapIndex,theIndex);
  
  if(!theIsModeShift)
    aMapIndex.Clear();
  
  if(!anIsConatains)
    aMapIndex.Add( theIndex );

  if( aMapIndex.IsEmpty())
    myMapIOSubIndex.erase(theIO);

  return false;
}


void
SVTK_SelectorDef
::RemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
	       int theIndex)
{
  if(IsIndexSelected(theIO,theIndex)){
    TMapIOSubIndex::iterator anIter = myMapIOSubIndex.find(theIO);
    TColStd_IndexedMapOfInteger& aMapIndex = anIter->second.myMap;
    removeIndex(aMapIndex,theIndex);
  }
}

void 
SVTK_SelectorDef
::ClearIndex()
{
  myMapIOSubIndex.clear();  
}
