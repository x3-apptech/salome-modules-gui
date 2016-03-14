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
// File   : SALOME_PYQT_Module.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef SALOME_PYQT_MODULE_H
#define SALOME_PYQT_MODULE_H

#include "SALOME_PYQT_GUI.h"

#include "PyInterp_Interp.h" // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!
#include "SalomeApp_Module.h"

class PyModuleHelper;

class SALOME_PYQT_EXPORT SALOME_PYQT_Module: public SalomeApp_Module
{
  Q_OBJECT;

public:
  SALOME_PYQT_Module();
  ~SALOME_PYQT_Module();

  // engine management
  QString         engineIOR() const;

  // module activation, preferences, menus
  void            initialize( CAM_Application* );
  bool            activateModule( SUIT_Study* );
  bool            deactivateModule( SUIT_Study* );
  void            windows( QMap<int, int>& ) const;
  void            viewManagers( QStringList& ) const;
  void            studyActivated();
  void            contextMenuPopup( const QString&, QMenu*, QString& );
  void            createPreferences();
  void            preferencesChanged( const QString&, const QString& );
  void            studyClosed( SUIT_Study* );

  // drag-n-drop support
  bool            isDraggable( const SUIT_DataObject* ) const;
  bool            isDropAccepted( const SUIT_DataObject* ) const;
  void            dropObjects( const DataObjectList&, SUIT_DataObject*,
			       const int, Qt::DropAction );
private:
  PyModuleHelper* myHelper;
};

#endif // SALOME_PYQT_MODULE_H
