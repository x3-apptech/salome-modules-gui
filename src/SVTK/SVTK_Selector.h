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

//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//  File   : SALOME_Selection.h
//  Author : Nicolas REJNERI

#ifndef SVTK_SELECTOR_H
#define SVTK_SELECTOR_H

#include "SVTK.h"
#include "SVTK_Selection.h"
#include "SALOME_ListIO.hxx"

#include <VTKViewer_Filter.h>

#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <vtkObject.h>

class SALOME_InteractiveObject;
class SALOME_Actor;
class SVTK_SelectionEvent;
class vtkActorCollection;
class vtkRenderer;

//! Define an abstract interface for selection in SVTK package
/*!
  The class implements selection functionality through storing corresponding
  references to #SALOME_InteractiveObject. For implementation of subselection
  modes it keeps a corresponding map of sub indexes.
  \note
  Also, for each #SALOME_InteractiveObject it tries to keep corresponding refenrence to #SALOME_Actor
 */
class SVTK_EXPORT SVTK_Selector: public vtkObject
{
public:
  static SVTK_Selector* New();
  
  vtkTypeMacro(SVTK_Selector,vtkObject);

  //! To change current Selection_Mode (as outside effect, it invokes selectionChange signal)
  virtual
  void 
  SetSelectionMode(Selection_Mode theMode) = 0;

  //! Get current Selection_Mode
  virtual
  Selection_Mode
  SelectionMode() const = 0;

  //! Clear selection
  virtual
  void
  ClearIObjects() = 0;

  //! Try to find corresponding #SALOME_Actor for given reference on #SALOME_InteractiveObject
  virtual
  SALOME_Actor* 
  GetActor(const Handle(SALOME_InteractiveObject)& theIO) const = 0;

  //! Check, is the #SALOME_InteractiveObject is present into selection
  virtual
  bool
  IsSelected(const Handle(SALOME_InteractiveObject)& theIO) const = 0;

  //! Check, is the #SALOME_Actor is present into selection
  virtual
  bool
  IsSelected(SALOME_Actor* theActor) const = 0;

  //! Modify the selection by adding new reference on #SALOME_InteractiveObject
  virtual
  bool
  AddIObject(const Handle(SALOME_InteractiveObject)& theIO) = 0;

  //! Modify the selection by adding new reference on #SALOME_Actor
  virtual
  bool
  AddIObject(SALOME_Actor* theActor) = 0;

  //! Modify the selection by removing a reference on #SALOME_InteractiveObject
  virtual
  bool 
  RemoveIObject(const Handle(SALOME_InteractiveObject)& theIO) = 0;

  //! Modify the selection by removing a reference on #SALOME_Actor
  virtual
  bool 
  RemoveIObject(SALOME_Actor* theActor) = 0;

  //! Get all #SALOME_InteractiveObject references that is present into selection
  virtual
  const SALOME_ListIO& 
  StoredIObjects() const = 0;

  //! Get number of selected objects
  virtual
  int 
  IObjectCount() const = 0;

  //! Check, if the #SALOME_InteractiveObject has a subselection
  virtual
  bool 
  HasIndex(const Handle(SALOME_InteractiveObject)& theIO ) const = 0;

  //! Get indexes of subslection for given #SALOME_InteractiveObject
  virtual
  void 
  GetIndex( const Handle(SALOME_InteractiveObject)& theIO, 
            TColStd_IndexedMapOfInteger& theIndex ) = 0;
        
  //! Change indices of subselection for given #SALOME_InteractiveObject
  virtual
  bool 
  AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
                    const TColStd_IndexedMapOfInteger& theIndices, 
                    bool theIsModeShift) = 0;

  //! Change indexes of subslection for given #SALOME_InteractiveObject
  virtual
  bool 
  AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
                    const TColStd_MapOfInteger& theIndices, 
                    bool theIsModeShift) = 0;

  //! Change index of subslection for given #SALOME_InteractiveObject
  virtual
  bool
  AddOrRemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
                    int theIndex, 
                    bool theIsModeShift) = 0;

  //! Change index of subslection for given #SALOME_InteractiveObject
  virtual
  void 
  RemoveIndex( const Handle(SALOME_InteractiveObject)& theIO, 
               int theIndex) = 0;

  //! Check, if the given index is present in subselection 
  virtual
  bool 
  IsIndexSelected(const Handle(SALOME_InteractiveObject)& theIO, 
                  int theIndex) const = 0;

  //! Clear subselection
  virtual
  void 
  ClearIndex() = 0;

  //----------------------------------------------------------------------------
  typedef int TFilterID;

  //! To apply a filter on the selection
  virtual
  void 
  SetFilter(const Handle(VTKViewer_Filter)& theFilter) = 0;

  //! To get a section filter by its number
  virtual
  Handle(VTKViewer_Filter) 
  GetFilter(const TFilterID theId) const = 0;

  //! Check, if a filter with given number is applyed
  virtual
  bool
  IsFilterPresent(const TFilterID theId) const = 0;

  //! To remove a filter from the selection
  virtual
  void
  RemoveFilter(const TFilterID theId) = 0;

  //! Check, if the index satisfy to the installed filters
  virtual
  bool
  IsValid(SALOME_Actor* theActor,
          const int theId,
          const bool theIsNode = false) const = 0;
  
  //----------------------------------------------------------------------------
  virtual
  void 
  StartPickCallback() = 0;

  //! To invoke selectionChanged signals
  virtual
  void 
  EndPickCallback() = 0;

  //----------------------------------------------------------------------------
  virtual
  vtkActorCollection*
  Pick(const SVTK_SelectionEvent* theEvent, vtkRenderer* theRenderer) const = 0;

  virtual
  void
  SetTolerance(const double& theTolerance) = 0;

  virtual
  void
  SetDynamicPreSelection( bool theIsDynPreselect ) = 0;

  virtual
  bool
  GetDynamicPreSelection() const = 0;

  virtual
  void
  SetPreSelectionEnabled( bool theEnabled ) = 0;

  virtual
  bool
  IsPreSelectionEnabled() const = 0;

  virtual
  void
  SetSelectionEnabled( bool theEnabled ) = 0;

  virtual
  bool
  IsSelectionEnabled() const = 0;
};


#endif
