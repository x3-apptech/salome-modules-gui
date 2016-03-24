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

// File   : LightApp_OBSelector.h
// Author :
//
#ifndef LIGHTAPP_OBSELECTOR_H
#define LIGHTAPP_OBSELECTOR_H

#include "LightApp.h"

#include <SUIT_Selector.h>
#include <SUIT_DataOwner.h>

#include <QObject>

class SUIT_DataBrowser;
class LightApp_DataObject;

class LIGHTAPP_EXPORT LightApp_OBSelector : public QObject, public SUIT_Selector
{
  Q_OBJECT

public:
  LightApp_OBSelector( SUIT_DataBrowser*, SUIT_SelectionMgr* );
  virtual ~LightApp_OBSelector();

  SUIT_DataBrowser*  browser() const;

  virtual QString    type() const;

  unsigned long      getModifiedTime() const;
  void               setModified();

private slots:
  void               onSelectionChanged();

protected:
  virtual void       getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void       setSelection( const SUIT_DataOwnerPtrList& );

private:
  void               fillEntries( QMap<QString, LightApp_DataObject*>& );

private:
  SUIT_DataBrowser*                   myBrowser;
  SUIT_DataOwnerPtrList               mySelectedList;
  QMap<QString, LightApp_DataObject*> myEntries;
  unsigned long                       myModifiedTime;
};

#endif
