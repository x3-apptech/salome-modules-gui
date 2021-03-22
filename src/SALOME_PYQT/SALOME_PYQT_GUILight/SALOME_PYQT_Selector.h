// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : SALOME_PYQT_Selector.h
// Author :
//
#ifndef SALOME_PYQT_SELECTOR_H
#define SALOME_PYQT_SELECTOR_H

#include "SALOME_PYQT_GUILight.h"
#include "PyInterp_Interp.h" // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include "LightApp.h"

#include <SUIT_Selector.h>
#include <SUIT_DataOwner.h>

#include <QObject>

class SALOME_PYQT_ModuleLight;
class LightApp_DataObject;

class SALOME_PYQT_LIGHT_EXPORT SALOME_PYQT_Selector : public QObject, public SUIT_Selector
{
  Q_OBJECT

public:
  SALOME_PYQT_Selector( SALOME_PYQT_ModuleLight*, SUIT_SelectionMgr* );
  virtual ~SALOME_PYQT_Selector();

  SALOME_PYQT_ModuleLight*  pyModule() const;

  virtual QString    type() const;

  unsigned long      getModifiedTime() const;
  void               setModified();
  void               setLocalEntries(const QStringList& entries) { myLocalEntries = entries; };
  void               clear();

private slots:
  void               onSelectionChanged();

protected:
  virtual void       getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void       setSelection( const SUIT_DataOwnerPtrList& );

private:
  void               fillEntries( QStringList& );

private:
  QStringList                         myLocalEntries;
  SALOME_PYQT_ModuleLight*            myPyModule;
  SUIT_DataOwnerPtrList               mySelectedList;
  QStringList                         myEntries;
  unsigned long                       myModifiedTime;
};

#endif
