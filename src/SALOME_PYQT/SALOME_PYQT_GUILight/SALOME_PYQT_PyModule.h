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

// File   : SALOME_PYQT_PyModule.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef SALOME_PYQT_PYMODULE_H
#define SALOME_PYQT_PYMODULE_H

#include "SALOME_PYQT_GUILight.h"

#include "PyInterp_Interp.h" // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include <QIcon>
#include <QList>
#include <QMap>
#include <QObject>
#include <QStringList>

class CAM_Application;
class LightApp_DataObject;
class LightApp_Module;
class PyInterp_Interp;
class QAction;
class QMenu;
class SUIT_DataObject;
class SUIT_Study;
class SUIT_ViewWindow;

typedef QList<SUIT_DataObject*> DataObjectList;

class SALOME_PYQT_LIGHT_EXPORT PyModuleHelper : public QObject
{
  Q_OBJECT

private:
  class XmlHandler;
  class InitLocker;

  typedef QMap<int, PyInterp_Interp*> InterpMap;

  static InterpMap           myInterpMap;          //!< study to Python subinterpreter map
  static LightApp_Module*    myInitModule;         //!< Python GUI being initialized (not zero only during the initialization)

  LightApp_Module*           myModule;             //!< GUI module
  PyObject*                  myPyModule;           //!< Python GUI module
  PyInterp_Interp*           myInterp;             //!< current Python subinterpreter
  XmlHandler*                myXmlHandler;         //!< XML resource file parser
  QMap<int, int>             myWindowsMap;         //!< windows map
  QStringList                myViewMgrList;        //!< compatible view managers list
  bool                       myLastActivateStatus; //!< latest module activation status

public:
  PyModuleHelper( LightApp_Module* );
  ~PyModuleHelper();

  static LightApp_Module*    getInitModule();
  static int                 defaultMenuGroup();

  LightApp_Module*           module() const;
  PyObject*                  pythonModule() const;

  void                       connectAction( QAction* );

  QMap<int, int>             windows() const;
  QStringList                viewManagers() const;

public slots:
  void                       initialize( CAM_Application* );
  bool                       activate( SUIT_Study* study );
  bool                       deactivate( SUIT_Study* study );
  void                       modelClosed( SUIT_Study* study );
  void                       preferencesChanged( const QString&, const QString& setting );
  void                       preferenceChanged( const QString&, const QString&, const QString& setting );
  void                       studyActivated( SUIT_Study* );
  void                       actionActivated();
  void                       contextMenu( const QString&, QMenu* );
  void                       createPreferences();
  void                       activeViewChanged( SUIT_ViewWindow* );
  void                       tryCloseView( SUIT_ViewWindow* );
  void                       closeView( SUIT_ViewWindow* );
  void                       cloneView( SUIT_ViewWindow* );
  void                       save( QStringList&, const QString& );
  bool                       load( const QStringList&, const QString& );
  void                       dumpPython( QStringList& files );
  bool                       isDraggable( const SUIT_DataObject* ) const;
  bool                       isDropAccepted( const SUIT_DataObject* ) const;
  void                       dropObjects( const DataObjectList&, SUIT_DataObject*,
                                          const int, Qt::DropAction );
  QString                    engineIOR() const;

  void                       onObjectBrowserClicked(SUIT_DataObject*, int);

private:
  void                       initInterp( int );
  void                       importModule();
  void                       setWorkSpace();

  void                       internalInitialize( CAM_Application* );
  void                       internalActivate( SUIT_Study* );
  void                       internalCustomize( SUIT_Study* );
  void                       internalDeactivate( SUIT_Study* );
  void                       internalClosedStudy( SUIT_Study* );
  void                       internalPreferencesChanged( const QString&, const QString& );
  void                       internalStudyChanged( SUIT_Study* );
  void                       internalActionActivated( int );
  void                       internalContextMenu( const QString&, QMenu* );
  void                       internalCreatePreferences();
  void                       internalActiveViewChanged( SUIT_ViewWindow* );
  void                       internalTryCloseView( SUIT_ViewWindow* );
  void                       internalCloseView( SUIT_ViewWindow* );
  void                       internalCloneView( SUIT_ViewWindow* );
  void                       internalSave( QStringList&, const QString& );
  void                       internalLoad( const QStringList&, const QString&, bool& );
  void                       internalDumpPython( QStringList& );
  bool                       internalIsDraggable( LightApp_DataObject* );
  bool                       internalIsDropAccepted( LightApp_DataObject* );
  void                       internalDropObjects( const DataObjectList&, SUIT_DataObject*,
                                                  const int, Qt::DropAction );
  QString                    internalEngineIOR() const;
  void                       internalOBClickedPython( const QString&, int );

  void                       connectView( SUIT_ViewWindow* );
};

#endif // SALOME_PYQT_PYMODULE_H
