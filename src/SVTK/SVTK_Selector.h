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

#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include "SVTK_Selection.h"
#include "SALOME_ListIO.hxx"

class SALOME_Actor;

class SVTK_Selector
{
public:
  virtual ~SVTK_Selector() = 0;

  virtual
  void 
  SetSelectionMode( Selection_Mode theMode ) = 0;

  virtual
  Selection_Mode
  SelectionMode() const = 0;

  virtual
  void
  ClearIObjects() = 0;

  virtual
  SALOME_Actor* 
  GetActor(const Handle(SALOME_InteractiveObject)& theIO) const = 0;

  virtual
  bool
  IsSelected(const Handle(SALOME_InteractiveObject)& theIO) const = 0;

  virtual
  bool
  IsSelected(SALOME_Actor* theActor) const = 0;

  virtual
  bool
  AddIObject(const Handle(SALOME_InteractiveObject)& theIO) = 0;

  virtual
  bool
  AddIObject(SALOME_Actor* theActor) = 0;

  virtual
  bool 
  RemoveIObject(const Handle(SALOME_InteractiveObject)& theIO) = 0;

  virtual
  bool 
  RemoveIObject(SALOME_Actor* theActor) = 0;

  virtual
  const SALOME_ListIO& 
  StoredIObjects() const = 0;

  virtual
  int 
  IObjectCount() const = 0;

  virtual
  bool 
  HasIndex(const Handle(SALOME_InteractiveObject)& theIO ) const = 0;

  virtual
  void 
  GetIndex( const Handle(SALOME_InteractiveObject)& theIO, 
	    TColStd_IndexedMapOfInteger& theIndex ) = 0;
	
  virtual
  bool 
  AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    const TColStd_IndexedMapOfInteger& theIndices, 
		    bool theIsModeShift) = 0;
  virtual
  bool 
  AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    const TColStd_MapOfInteger& theIndices, 
		    bool theIsModeShift) = 0;
  virtual
  bool
  AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
		    int theIndex, 
		    bool theIsModeShift) = 0;
  virtual
  void 
  RemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
	       int theIndex) = 0;
  virtual
  bool 
  IsIndexSelected(const Handle(SALOME_InteractiveObject)& theIO, 
		  int theIndex) const = 0;
  virtual
  void 
  ClearIndex() = 0;
};


#endif
