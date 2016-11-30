// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : SALOME_PYQT_ModuleLight.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef SALOME_PYQT_MODULELIGHT_H
#define SALOME_PYQT_MODULELIGHT_H

#include "SALOME_PYQT_GUILight.h"

#include "PyInterp_Interp.h" // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!
#include "LightApp_Module.h"

#include <QIcon>
#include <QMap>
#include <QStringList>

class QMenu;
class PyModuleHelper;
class SALOME_PYQT_DataObjectLight;

class SALOME_PYQT_LIGHT_EXPORT SALOME_PYQT_ModuleLight: public LightApp_Module
{
  Q_OBJECT;

public:
  SALOME_PYQT_ModuleLight();
  ~SALOME_PYQT_ModuleLight();

public:
  // module activation, preferences, menus
  void            initialize( CAM_Application* );
  bool            activateModule( SUIT_Study* );
  bool            deactivateModule( SUIT_Study* );
  void            onModelClosed();
  void            windows( QMap<int, int>& ) const;
  void            viewManagers( QStringList& ) const;
  void            studyActivated();
  void            contextMenuPopup( const QString&, QMenu*, QString& );
  void            createPreferences();
  void            preferencesChanged( const QString&, const QString& );

  // persistence & dump python
  void            save( QStringList&, const QString& );
  bool            load( const QStringList&, const QString& );
  void            dumpPython( QStringList& );

  // drag-n-drop support
  bool            isDraggable( const SUIT_DataObject* ) const;
  bool            isDropAccepted( const SUIT_DataObject* ) const;
  void            dropObjects( const DataObjectList&, SUIT_DataObject*,
			       const int, Qt::DropAction );

  // data model management
  QString         createObject( const QString& );
  QString         createObject( const QString&, 
                                const QString&,
                                const QString&,
                                const QString& );

  void            setName( const QString&, const QString& );
  QString         getName( const QString& ) const;

  void            setIcon( const QString&, const QString& );

  void            setToolTip( const QString&, const QString& );
  QString         getToolTip( const QString& ) const;

  void            setColor( const QString&, const QColor& );
  QColor          getColor( const QString& ) const;

  void            setReference( const QString&, const QString& );
  QString         getReference( const QString& ) const;

  void            removeObject( const QString& );
  void            removeChildren( const QString& );

  void            setObjectPosition( const QString&, int );
  int             getObjectPosition( const QString& );

  QStringList     getChildren( const QString&, const bool = false ) const;

protected:
  CAM_DataModel*  createDataModel();

private:
  SALOME_PYQT_DataObjectLight* 
                  findObject( const QString& ) const;

private:
  PyModuleHelper* myHelper;
};

#endif // SALOME_PYQT_MODULELIGHT_H
