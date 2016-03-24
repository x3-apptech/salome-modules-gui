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

#ifndef CAM_MODULE_H
#define CAM_MODULE_H

#include "CAM.h"

#include <QObject>
#include <QPixmap>
#include <QString>
#include <QMap>
#include <QPair>
#include <QKeySequence>

class QAction;
class QMenu;
class QIcon;

class QtxActionGroup;
class QtxActionMenuMgr;
class QtxActionToolMgr;
class SUIT_Study;
class SUIT_Application;
class CAM_Study;
class CAM_DataModel;
class CAM_Application;

#ifdef WIN32
#pragma warning( disable: 4251 )
#endif

class CAM_EXPORT CAM_Module : public QObject
{
  Q_OBJECT

public:
  CAM_Module();
  CAM_Module( const QString& );
  virtual ~CAM_Module();

  virtual void           initialize( CAM_Application* );

  QString                name() const;
  QString                moduleName() const;
  virtual QPixmap        moduleIcon() const;
  virtual QString        iconName() const;

  CAM_DataModel*         dataModel() const;
  CAM_Application*       application() const;

  virtual void           contextMenuPopup( const QString&, QMenu*, QString& ) {};
  virtual void           updateCommandsStatus() {};

  virtual void           putInfo( const QString&, const int = -1 );

  bool                   isActiveModule() const;

  virtual void           setMenuShown( const bool );
  void                   setMenuShown( QAction*, const bool );
  void                   setMenuShown( const int, const bool );

  virtual void           setToolShown( const bool );
  void                   setToolShown( QAction*, const bool );
  void                   setToolShown( const int, const bool );

  virtual void           updateModuleVisibilityState();

  virtual bool           activateOperation( int actionId );
  virtual bool           activateOperation( const QString& actionId );
  virtual bool           activateOperation( const QString& actionId, const QString& pluginName );

  // actions/menu/toolbars management

  QtxActionMenuMgr*      menuMgr() const;
  QtxActionToolMgr*      toolMgr() const;

  virtual QAction*       action( const int ) const;
  virtual int            actionId( const QAction* ) const;
  virtual QAction*       createAction( const int, const QString&, const QIcon&, const QString&,
                                       const QString&, const int, QObject* = 0,
                                       const bool = false, QObject* = 0, const char* = 0, const QString& = QString() );
  virtual QAction*       createAction( const int, const QString&, const QIcon&, const QString&,
                                       const QString&, const QKeySequence&, QObject* = 0,
                                       const bool = false, QObject* = 0, const char* = 0, const QString& = QString() );
  QtxActionGroup*        createActionGroup( const int, const bool = true );

  int                    createTool( const QString&, const QString& = QString() );
  int                    createTool( const int, const int, const int = -1 );
  int                    createTool( const int, const QString&, const int = -1 );
  int                    createTool( QAction*, const int, const int = -1, const int = -1 );
  int                    createTool( QAction*, const QString&, const int = -1, const int = -1 );

  int                    createMenu( const QString&, const int, const int = -1, const int = -1, const int = -1,QMenu * = 0);
  int                    createMenu( const QString&, const QString&, const int = -1, const int = -1, const int = -1 );
  int                    createMenu( const int, const int, const int = -1, const int = -1 );
  int                    createMenu( const int, const QString&, const int = -1, const int = -1 );
  int                    createMenu( QAction*, const int, const int = -1, const int = -1, const int = -1 );
  int                    createMenu( QAction*, const QString&, const int = -1, const int = -1, const int = -1 );

  static QAction*        separator();

public slots:
  virtual bool           activateModule( SUIT_Study* );
  virtual bool           deactivateModule( SUIT_Study* );

  virtual void           connectToStudy( CAM_Study* );

  virtual void           studyClosed( SUIT_Study* );
  virtual void           studyChanged( SUIT_Study*, SUIT_Study* );

  virtual void           onApplicationClosed( SUIT_Application* );

private slots:
  void                   onInfoChanged( QString );

protected: 
  virtual bool           isSelectionCompatible();

  virtual CAM_DataModel* createDataModel();

  void                   setName( const QString& );
  virtual void           setModuleName( const QString& );

  int                    registerAction( const int, QAction* );
  bool                   unregisterAction( const int );
  bool                   unregisterAction( QAction* );
  // IMN 05/03/2015: we copied myActionMap for reset/unset actions accelerator keys
  // after activate/deactivate modules
  QMap<QAction*, QKeySequence> myActionShortcutMap; //!< copy actions shortcut map

  virtual bool           abortAllOperations();

private:
  CAM_Application*       myApp;             //!< parent application object
  QString                myName;            //!< module title (user name)
  QPixmap                myIcon;            //!< module icon
  QString                myInfo;            //!< latest info message
  CAM_DataModel*         myDataModel;       //!< data model
  QMap<int, QAction*>    myActionMap;       //!< menu actions
  bool                   myMenuShown;       //!< menu shown flag
  bool                   myToolShown;       //!< tool shown flag

  friend class CAM_Application;
};

#ifdef WIN32
#pragma warning( default: 4251 )
#endif

extern "C"
{
  typedef CAM_Module* (*GET_MODULE_FUNC)();
  typedef char* (*GET_VERSION_FUNC)();
}

#define GET_MODULE_NAME "createModule"
#define GET_VERSION_NAME "getModuleVersion"
#endif
