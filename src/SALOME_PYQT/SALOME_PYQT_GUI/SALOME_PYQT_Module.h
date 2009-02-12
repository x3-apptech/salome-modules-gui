//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : SALOME_PYQT_Module.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef SALOME_PYQT_MODULE_H
#define SALOME_PYQT_MODULE_H

#include "SALOME_PYQT_GUI.h"
#include "SALOME_PYQT_PyInterp.h" // this include must be first (see PyInterp_Interp.h)!

#include <SalomeApp_Module.h>

#include <QStringList>
#include <QList>
#include <QMap>
#include <QIcon>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)

class SALOME_PYQT_PyInterp;
class SUIT_ViewWindow;
class QAction;
class QtxActionGroup;
class QMenu;

class SALOME_PYQT_EXPORT SALOME_PYQT_Module: public SalomeApp_Module
{
  Q_OBJECT;

private:
  class XmlHandler;

  //! study to Python subinterpreter map
  typedef QMap<int, SALOME_PYQT_PyInterp*> InterpMap;

  static InterpMap           myInterpMap;  //!< study to Python subinterpreter map
  SALOME_PYQT_PyInterp*      myInterp;     //!< current Python subinterpreter
  PyObjWrapper               myModule;     //!< Python GUI module
  static SALOME_PYQT_Module* myInitModule; //!< Python GUI being initialized (not zero only during the initialization)

  XmlHandler*                myXmlHandler; //!< XML resource file parser
  QMap<int, int>             myWindowsMap; //!< windows map
  QStringList                myViewMgrList;//!< compatible view managers list

  bool                       myLastActivateStatus; //!< latest module activation status

public:
  SALOME_PYQT_Module();
  ~SALOME_PYQT_Module();

public:
  static SALOME_PYQT_Module* getInitModule();

  void                       initialize( CAM_Application* );
  void                       windows( QMap<int, int>& ) const;
  void                       viewManagers( QStringList& ) const;
  void                       contextMenuPopup( const QString&, QMenu*, QString& );
  void                       createPreferences();
  QString                    engineIOR() const;
  void                       studyActivated();
  void                       preferencesChanged( const QString&, const QString& );

  static int                 defaultMenuGroup();

  int                        createTool( const QString& );
  int                        createTool( const int, const int, const int = -1 );
  int                        createTool( const int, const QString&, const int = -1 );
  int                        createTool( QAction*, const int, 
					 const int = -1, const int = -1 );
  int                        createTool( QAction*, const QString&, 
					 const int = -1, const int = -1 );

  int                        createMenu( const QString&, const int, 
					 const int = -1, const int = -1, const int = -1 );
  int                        createMenu( const QString&, const QString&, 
					 const int = -1, const int = -1, const int = -1 );
  int                        createMenu( const int, const int, 
					 const int = -1, const int = -1 );
  int                        createMenu( const int, const QString&, 
					 const int = -1, const int = -1 );
  int                        createMenu( QAction*, const int, 
					 const int = -1, const int = -1, const int = -1 );
  int                        createMenu( QAction*, const QString&, 
					 const int = -1, const int = -1, const int = -1 );

  QAction*                   separator();

  QAction*                   action( const int ) const;
  int                        actionId( const QAction* ) const;
  QAction*                   createAction( const int, const QString&, const QString&, 
					   const QString&, const QString&, const int, 
					   const bool = false, QObject* = 0 );
  QtxActionGroup*            createActionGroup( const int, const bool );


  QIcon                      loadIcon( const QString& fileName );

  int                        addGlobalPreference( const QString& );
  int                        addPreference( const QString& );
  int                        addPreference( const QString&, const int, const int = LightApp_Preferences::Auto,
					    const QString& = QString(),
					    const QString& = QString() );
  QVariant                   preferenceProperty( const int, const QString& ) const;
  void                       setPreferenceProperty( const int, const QString&, 
						    const QVariant& );

public slots:
  virtual bool               activateModule( SUIT_Study* );
  virtual bool               deactivateModule( SUIT_Study* );
  void                       preferenceChanged( const QString&, 
						const QString&, 
						const QString& );
  void                       onGUIEvent();

  void                       onActiveViewChanged( SUIT_ViewWindow* );
  void                       onViewClosed( SUIT_ViewWindow* );
  void                       onViewCloned( SUIT_ViewWindow* );

protected:
  Engines::Component_var     getEngine() const;

private:
  void                       init( CAM_Application* );
  void                       activate( SUIT_Study* );
  void                       deactivate( SUIT_Study* );
  bool                       lastActivationStatus() const;
  void                       customize( SUIT_Study* );
  void                       studyChanged( SUIT_Study* );
  void                       contextMenu( const QString&, QMenu* );
  void                       guiEvent( const int );
  void                       initPreferences();
  void                       prefChanged( const QString&, const QString& );

  void                       initInterp  ( int );
  void                       importModule();
  void                       setWorkSpace();
  
  void                       activeViewChanged( const SUIT_ViewWindow* );
  void                       viewClosed( const SUIT_ViewWindow* );
  void                       viewCloned( const SUIT_ViewWindow* );
  void                       connectView( const SUIT_ViewWindow* );

  friend class XmlHandler;
};

#endif // SALOME_PYQT_MODULE_H
