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
//  File   : SALOME_Selection.h
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$

#ifndef SVTK_SELECTOR_H
#define SVTK_SELECTOR_H
#include <NCollection_DataMap.hxx>
#include <vtkSmartPointer.h>
#include <TColStd_MapOfInteger.hxx>

#include "SVTK_Selection.h"

#include "SALOME_ListIO.hxx"
#include "SALOME_InteractiveObject.hxx"
#include "SALOME_DataMapOfIOMapOfInteger.hxx"

class SALOME_Actor;

class SVTK_Viewer;
class SVTK_ViewWindow;

class SVTK_Selector
{
public:
  SVTK_Selector();
  virtual ~SVTK_Selector();

  void SetSelectionMode( Selection_Mode theMode );
  Selection_Mode SelectionMode() const { return mySelectionMode; }

  void
  ClearIObjects();

  SALOME_Actor* 
  GetActor(const Handle(SALOME_InteractiveObject)& theIO) const;
  bool
  IsSelected(const Handle(SALOME_InteractiveObject)& theIO) const;
  bool
  IsSelected(SALOME_Actor* theActor) const;

  bool
  AddIObject(const Handle(SALOME_InteractiveObject)& theIO);
  bool
  AddIObject(SALOME_Actor* theActor);

  bool 
  RemoveIObject(const Handle(SALOME_InteractiveObject)& theIO);
  bool 
  RemoveIObject(SALOME_Actor* theActor);

  const SALOME_ListIO& 
  StoredIObjects() const;

  int 
  IObjectCount() const;

  bool 
  HasIndex(const Handle(SALOME_InteractiveObject)& theIO ) const;

  void 
  GetIndex( const Handle(SALOME_InteractiveObject)& theIO, 
	    TColStd_IndexedMapOfInteger& theIndex );
	
  bool 
  AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    const TColStd_IndexedMapOfInteger& theIndices, 
		    bool theIsModeShift);
  bool 
  AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    const TColStd_MapOfInteger& theIndices, 
		    bool theIsModeShift);
  bool
  AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    int theIndex, 
		    bool theIsModeShift);
  void 
  RemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
	       int theIndex);
  bool 
  IsIndexSelected(const Handle(SALOME_InteractiveObject)& theIO, 
		  int theIndex) const;
  void 
  ClearIndex();

private:
  typedef NCollection_DataMap<Handle(SALOME_InteractiveObject),
                              vtkSmartPointer<SALOME_Actor> > TIO2Actors;
  TIO2Actors myIO2Actors;
  Selection_Mode mySelectionMode;
  SALOME_ListIO myIObjects;
  SALOME_DataMapOfIOMapOfInteger myMapIOSubIndex;
};


#endif
