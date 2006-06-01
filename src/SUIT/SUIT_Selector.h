// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#include <qvaluelist.h>
#include <qobject.h>

class SUIT_SelectionMgr;
class SUIT_DataOwnerPtrList;

/*!
  \class SUIT_Selector
  Base class for all selectors used in SUIT-based applications.
  Provides functionality to get/set selection from/into some widget
  (ObjectBrowser, viewers, etc)
  Used by selection manager for selection synhronizing
*/
class SUIT_EXPORT SUIT_Selector : public QObject
{
  Q_OBJECT
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
  void               selectionModes( QValueList<int>& ) const;

protected:
  void               selectionChanged();
  virtual void       getSelection( SUIT_DataOwnerPtrList& ) const = 0;
  virtual void       setSelection( const SUIT_DataOwnerPtrList& ) = 0;

private:
  bool               myBlock;
  SUIT_SelectionMgr* mySelMgr;
  bool               myEnabled;
  bool               myAutoBlock;
};

#endif
