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

#ifndef SUIT_SELECTOR_H
#define SUIT_SELECTOR_H

#include "SUIT.h"

#include <QList>

class QObject;

class SUIT_SelectionMgr;
class SUIT_DataOwnerPtrList;

/*!
  \class SUIT_Selector
  Base class for all selectors used in SUIT-based applications.
  Provides functionality to get/set selection from/into some widget
  (ObjectBrowser, viewers, etc)
  Used by selection manager for selection synhronizing
*/

class SUIT_EXPORT SUIT_Selector
{
  class Destroyer;

public:
  SUIT_Selector( SUIT_SelectionMgr*, QObject* = 0 );
  virtual ~SUIT_Selector();

  virtual QString    type() const = 0;

  SUIT_SelectionMgr* selectionMgr() const;

  bool               isEnabled() const;
  virtual void       setEnabled( const bool );

  bool               autoBlock() const;
  virtual void       setAutoBlock( const bool );

  void               selected( SUIT_DataOwnerPtrList& ) const;
  void               setSelected( const SUIT_DataOwnerPtrList& );

  bool               hasSelectionMode( const int ) const;
  void               selectionModes( QList<int>& ) const;

  QObject*           owner() const;

protected:
  void               selectionChanged();
  virtual void       getSelection( SUIT_DataOwnerPtrList& ) const = 0;
  virtual void       setSelection( const SUIT_DataOwnerPtrList& ) = 0;

private:
  SUIT_SelectionMgr* mySelMgr;
  bool               myBlock;
  bool               myEnabled;
  bool               myAutoBlock;
  Destroyer*         myDestroyer;
};

#endif
