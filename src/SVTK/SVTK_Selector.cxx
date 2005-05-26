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
#include "SALOME_ListIteratorOfListIO.hxx"

#include "SVTK_Selector.h"

#include "utilities.h"

SVTK_Selector
::SVTK_Selector()
{
}

SVTK_Selector
::~SVTK_Selector()
{
}

void 
SVTK_Selector
::SetSelectionMode(Selection_Mode theMode)
{
  mySelectionMode = theMode;
}

void 
SVTK_Selector
::ClearIObjects() 
{
  myIO2Actors.Clear();
  myIObjects.Clear();
  myMapIOSubIndex.Clear();
}

//----------------------------------------------------------------------------
bool
SVTK_Selector
::IsSelected(const Handle(SALOME_InteractiveObject)& theObject) const
{
  SALOME_ListIteratorOfListIO anIter(myIObjects);
  for(; anIter.More(); anIter.Next()){
    if(theObject->isSame(anIter.Value())){
      return true;
    }
  }
  return false;
}

bool
SVTK_Selector
::IsSelected(SALOME_Actor* theActor) const
{
  const Handle(SALOME_InteractiveObject) anIO = theActor->getIO();
  return IsSelected(anIO) && myIO2Actors.IsBound(anIO);
}

SALOME_Actor*
SVTK_Selector
::GetActor(const Handle(SALOME_InteractiveObject)& theIO) const
{
  if(myIO2Actors.IsBound(theIO))
    return myIO2Actors.Find(theIO).GetPointer();
  return NULL;
}

//----------------------------------------------------------------------------
bool 
SVTK_Selector
::AddIObject(const Handle(SALOME_InteractiveObject)& theIO) 
{
  if(!IsSelected(theIO)){
    myIObjects.Append(theIO);
    return true;
  }
  return false;
}

bool 
SVTK_Selector
::AddIObject(SALOME_Actor* theActor) 
{
  const Handle(SALOME_InteractiveObject) anIO = theActor->getIO();

  bool anIsIOBound = IsSelected(anIO);
  if(!anIsIOBound)
    myIObjects.Append(anIO);

  bool anIsActorBound = myIO2Actors.IsBound(anIO);
  if(!anIsActorBound)
    myIO2Actors.Bind(anIO,theActor);
  
  return !anIsIOBound || !anIsActorBound;
}

//----------------------------------------------------------------------------
bool 
SVTK_Selector
::RemoveIObject(const Handle(SALOME_InteractiveObject)& theIO) 
{
  bool anIsIOBound = false;
  for(SALOME_ListIteratorOfListIO anIter(myIObjects); anIter.More(); anIter.Next()){
    if(theIO->isSame(anIter.Value())){
      if(myMapIOSubIndex.IsBound(theIO))
	myMapIOSubIndex.UnBind(theIO);
      myIObjects.Remove(anIter);
      anIsIOBound = true; 
    }
  }

  bool anIsActorBound = myIO2Actors.IsBound(theIO);
  if(anIsActorBound)
    myIO2Actors.UnBind(theIO);

  return anIsIOBound || anIsActorBound;
}

bool 
SVTK_Selector
::RemoveIObject(SALOME_Actor* theActor) 
{
  const Handle(SALOME_InteractiveObject) anIO = theActor->getIO();

  bool anIsActorBound = myIO2Actors.IsBound(anIO);
  if(anIsActorBound)
    myIO2Actors.UnBind(anIO);

  bool anIsIOBound = false;
  for(SALOME_ListIteratorOfListIO anIter(myIObjects); anIter.More(); anIter.Next()){
    if(anIO->isSame(anIter.Value())){
      if(myMapIOSubIndex.IsBound(anIO))
	myMapIOSubIndex.UnBind(anIO);
      myIObjects.Remove(anIter);
      anIsIOBound = true; 
    }
  }

  return anIsIOBound || anIsActorBound;
}

//----------------------------------------------------------------------------
const SALOME_ListIO& 
SVTK_Selector
::StoredIObjects() const
{
  return myIObjects;
}

int
SVTK_Selector
::IObjectCount() const
{
  return myIObjects.Extent();
}

bool 
SVTK_Selector
::HasIndex( const Handle(SALOME_InteractiveObject)& theIO) const
{
  return myMapIOSubIndex.IsBound(theIO);
}

void 
SVTK_Selector
::GetIndex( const Handle(SALOME_InteractiveObject)& theIO, 
	    TColStd_IndexedMapOfInteger& theIndex)
{
  if(myMapIOSubIndex.IsBound(theIO))
    theIndex = myMapIOSubIndex.Find(theIO);
  else
    theIndex.Clear();
}

bool
SVTK_Selector
::IsIndexSelected(const Handle(SALOME_InteractiveObject)& theIO, 
		  int theIndex) const
{
  if( !myMapIOSubIndex.IsBound(theIO))
    return false;

  const TColStd_IndexedMapOfInteger& aMapIndex = myMapIOSubIndex.Find(theIO);
  return aMapIndex.Contains(theIndex);
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
SVTK_Selector
::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    const TColStd_IndexedMapOfInteger& theIndices, 
		    bool theIsModeShift)
{
  if(!myMapIOSubIndex.IsBound(theIO))
    myMapIOSubIndex.Bind(theIO,TColStd_IndexedMapOfInteger());

  TColStd_IndexedMapOfInteger& aMapIndex = myMapIOSubIndex.ChangeFind(theIO);
  aMapIndex = theIndices;

  if(!theIsModeShift)
    aMapIndex.Clear();
  
  if(aMapIndex.IsEmpty()){
    myMapIOSubIndex.UnBind(theIO);
    RemoveIObject(theIO);
  }

  return !aMapIndex.IsEmpty();
}


bool
SVTK_Selector
::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    const TColStd_MapOfInteger& theIndices, 
		    bool theIsModeShift)
{
  if(!myMapIOSubIndex.IsBound(theIO))
    myMapIOSubIndex.Bind(theIO,TColStd_IndexedMapOfInteger());

  TColStd_IndexedMapOfInteger& aMapIndex = myMapIOSubIndex.ChangeFind(theIO);

  if(!theIsModeShift)
    aMapIndex.Clear();
  
  for(TColStd_MapIteratorOfMapOfInteger anIter(theIndices); anIter.More(); anIter.Next())
    aMapIndex.Add(anIter.Key());
  
  if(aMapIndex.IsEmpty()){
    myMapIOSubIndex.UnBind(theIO);
    RemoveIObject(theIO);
  }

  return !aMapIndex.IsEmpty();
}


bool 
SVTK_Selector
::AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    int theIndex, 
		    bool theIsModeShift)
{
  if(!myMapIOSubIndex.IsBound(theIO))
    myMapIOSubIndex.Bind(theIO,TColStd_IndexedMapOfInteger());
  
  TColStd_IndexedMapOfInteger& aMapIndex = myMapIOSubIndex.ChangeFind(theIO);

  bool anIsConatains = aMapIndex.Contains( theIndex );

  if (anIsConatains)
    removeIndex( aMapIndex, theIndex );
  
  if (!theIsModeShift)
    aMapIndex.Clear();
  
  if(!anIsConatains)
    aMapIndex.Add( theIndex );

  if ( aMapIndex.IsEmpty() ) {
    myMapIOSubIndex.UnBind(theIO);
    RemoveIObject(theIO);
  }

  return false;
}


void
SVTK_Selector
::RemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
	       int theIndex)
{
  if ( myMapIOSubIndex.IsBound( theIO ) ) {
    TColStd_IndexedMapOfInteger& aMapIndex = myMapIOSubIndex.ChangeFind( theIO );
    removeIndex( aMapIndex, theIndex );
  }
}

void 
SVTK_Selector
::ClearIndex()
{
  myMapIOSubIndex.Clear();  
}
