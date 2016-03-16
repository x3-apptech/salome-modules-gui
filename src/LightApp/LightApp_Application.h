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

// File:      LightApp_Application.h
// Created:   6/20/2005 18:39:25 PM
// Author:    OCC team

#ifndef LIGHTAPP_APPLICATION_H
#define LIGHTAPP_APPLICATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LightApp.h"
#include <SUIT_TreeModel.h>
#include <SUIT_DataObject.h>
#include <CAM_Application.h>

#include <QPointer>
#include <QStringList>

class LogWindow;
#ifndef DISABLE_PYCONSOLE
class PyConsole_Console;
class PyConsole_Interp;
#endif
class LightApp_WidgetContainer;
class LightApp_Preferences;
class LightApp_SelectionMgr;
class LightApp_FullScreenHelper;
class LightApp_DataObject;
class SUIT_DataBrowser;
class SUIT_Study;
class SUIT_Accel;
class SUIT_ViewModel;
class CAM_Module;

class QString;
class QWidget;
class QStringList;
class QDockWidget;
class QTimer;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  Description : Application containing only LightApp module
*/

class LIGHTAPP_EXPORT LightApp_Application : public CAM_Application, public SUIT_DataSearcher
{
  Q_OBJECT

public:
  typedef enum { WT_ObjectBrowser,
#ifndef DISABLE_PYCONSOLE
                 WT_PyConsole,
#endif
                 WT_LogWindow,
                 WT_User }
  WindowTypes;

  enum { MenuWindowId = 6 };

  enum { RenameId = CAM_Application::UserID,
         CloseId, CloseAllId, GroupAllId,
         PreferencesId, MRUId, ModulesListId,
         NewGLViewId, NewPlot2dId, NewOCCViewId, NewVTKViewId,
         NewQxSceneViewId, NewGraphicsViewId, NewPVViewId, NewPyViewerId, StyleId, FullScreenId,
         UserID };

protected:
  enum { NewStudyId = 1, OpenStudyId };

  enum BrowsePolicy { BP_Never = 0, BP_ApplyAndClose, BP_Always };

public:
  LightApp_Application();
  virtual ~LightApp_Application();

  virtual QString                     applicationName() const;
  virtual QString                     applicationVersion() const;

  virtual CAM_Module*                 loadModule( const QString&, const bool = true );
  virtual bool                        activateModule( const QString& );

  LightApp_SelectionMgr*              selectionMgr() const;

  LogWindow*                          logWindow();
  SUIT_DataBrowser*                   objectBrowser();
#ifndef DISABLE_PYCONSOLE
  PyConsole_Console*                  pythonConsole(const bool force = false);
#endif

  virtual void                        updateObjectBrowser( const bool = true );

  LightApp_Preferences*               preferences() const;

  virtual QString                     getFileFilter() const;

  virtual QString                     getFileName( bool open, const QString& initial, const QString& filters,
                                                   const QString& caption, QWidget* parent );
  virtual QString                     getDirectory( const QString& initial, const QString& caption, QWidget* parent );
  virtual QStringList                 getOpenFileNames( const QString& initial, const QString& filters,
                                                        const QString& caption, QWidget* parent );

  void                                updateActions();

  SUIT_ViewManager*                   getViewManager( const QString&, const bool );
  virtual void                        addViewManager( SUIT_ViewManager* );
  virtual void                        removeViewManager( SUIT_ViewManager* );
  virtual SUIT_ViewManager*           createViewManager( const QString& vmType );
  virtual SUIT_ViewManager*           createViewManager( const QString& vmType, QWidget* w );
  virtual SUIT_ViewManager*           createViewManager( SUIT_ViewModel* );

  QWidget*                            getWindow( const int, const int = -1 );
  QWidget*                            dockWindow( const int ) const;
  void                                removeDockWindow( const int );
  void                                insertDockWindow( const int, QWidget* );
  void                                placeDockWindow( const int, Qt::DockWidgetArea );

  virtual void                        start();
  virtual void                        closeApplication();

  virtual void                        contextMenuPopup( const QString&, QMenu*, QString& );

  virtual void                        createEmptyStudy();

  virtual void                        setDesktop( SUIT_Desktop* );

  SUIT_Accel*                         accel() const;

  void                                setDefaultStudyName( const QString& theName );

  static int                          studyId();

  virtual bool                        event( QEvent* );

  virtual bool                        checkDataObject( LightApp_DataObject* theObj );

  virtual void                        updateDesktopTitle();

  //! Returns list of view manager types which are supported by this application
  QStringList                          viewManagersTypes() const;

  //! Removes ViewManagers only of known type
  virtual void                        clearKnownViewManagers();

  virtual QString                     browseObjects( const QStringList& theEntryList,
                                                     const bool theIsApplyAndClose = true,
                                                     const bool theIsOptimizedBrowsing = false );

  virtual SUIT_DataObject*            findObject( const QString& ) const;

  virtual bool                        renameAllowed( const QString& ) const;
  virtual bool                        renameObject( const QString&, const QString& );

  void                                emitOperationFinished( const QString&, const QString&, const QStringList& );

  void                                updateVisibilityState( DataObjectList& theList,
                                                             SUIT_ViewModel* theViewModel );  

  virtual bool                        checkExistingDoc();

#ifndef DISABLE_PYCONSOLE
  PyConsole_Interp*                   getPyInterp();
#endif

signals:
  void                                studyOpened();
  void                                studySaved();
  void                                studyClosed();
  void                                preferenceChanged( const QString&, const QString&, const QString& );
  void                                preferenceResetToDefaults();
  void                                operationFinished( const QString&, const QString&, const QStringList& );

public slots:
  virtual void                        onHelpContentsModule();
  virtual void                        onHelpContextModule( const QString&, const QString&, const QString& = QString() );
  virtual void                        onNewDoc();
  virtual void                        onOpenDoc();

  virtual void                        onHelpAbout();
  virtual bool                        onOpenDoc( const QString& );
  virtual void                        onCopy();
  virtual void                        onPaste();
  virtual void                        onSelectionChanged();
  virtual void                        onDockWindowVisibilityChanged( bool );

protected:
  virtual void                        createActions();
  virtual void                        createActionForViewer( const int id,
                                                             const int parentId,
                                                             const QString& suffix,
                                                             const int accel );
  virtual SUIT_Study*                 createNewStudy();
  virtual QWidget*                    createWindow( const int );
  virtual void                        defaultWindows( QMap<int, int>& ) const;
  void                                defaultViewManagers( QStringList& ) const;

  virtual void                        setActiveStudy( SUIT_Study* );
  virtual void                        updateCommandsStatus();

  virtual void                        beforeCloseDoc( SUIT_Study* );
  virtual void                        afterCloseDoc();
  virtual void                        moduleAdded( CAM_Module* );
  virtual bool                        activateModule( CAM_Module* = 0 );

  LightApp_Preferences*               preferences( const bool ) const;
  virtual void                        createPreferences( LightApp_Preferences* );
  virtual void                        preferencesChanged( const QString&, const QString& );

  virtual void                        loadPreferences();
  virtual void                        savePreferences();

  virtual QMap<int, QString>          activateModuleActions() const;
  virtual void                        moduleActionSelected( const int );

#ifndef DISABLE_PYCONSOLE
  virtual PyConsole_Interp*           createPyInterp();
#endif

protected slots:
  virtual void                        onDesktopActivated();
  virtual void                        onViewManagerRemoved( SUIT_ViewManager* );
  virtual void                        onWindowActivated( SUIT_ViewWindow* theViewWindow );

  void                                onNewWindow();
  void                                onModuleActivation( const QString& );
  void                                onCloseView( SUIT_ViewManager* );

  virtual void                        onStudyCreated( SUIT_Study* );
  virtual void                        onStudyOpened( SUIT_Study* );
  virtual void                        onStudySaved( SUIT_Study* );
  virtual void                        onStudyClosed( SUIT_Study* );

  void                                onWCDestroyed( QObject* );

  void                                onMRUActivated( const QString& );

  void                                onStylePreferences();
  void                                onFullScreen();

  virtual void                        onDesktopMessage( const QString& );

private slots:
  void                                onSelection();
  void                                onRefresh();
  void                                onDropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction );
  void                                onPreferences();
  void                                onPreferenceChanged( QString&, QString&, QString& );
  void                                onRenameWindow();
  void                                onCloseWindow();
  void                                onCloseAllWindow();
  void                                onGroupAllWindow();
  void                                onRenamed();
  //void                                onMoved();

protected:
  void                                updateWindows();
  void                                updateViewManagers();
  void                                updateModuleActions();
  void                                removeModuleAction( const QString& );

  void                                loadDockWindowsState();
  void                                saveDockWindowsState();

  virtual void                        studyOpened( SUIT_Study* );
  virtual void                        studyCreated( SUIT_Study* );
  virtual void                        studySaved( SUIT_Study* );

  void                                updatePreference( const QString&, const QString&, const QString& );

  QString                             defaultModule() const;
  virtual void                        currentWindows( QMap<int, int>& ) const;
  void                                currentViewManagers( QStringList& ) const;
  void                                moduleIconNames( QMap<QString, QString>& ) const;

  bool                                isLibExists( const QString& ) const;

  QDockWidget*                        windowDock( QWidget* ) const;
  QByteArray                          dockWindowsState( const QMap<QString, bool>&, const QMap<QString, bool>& ) const;
  void                                dockWindowsState( const QByteArray&, QMap<QString, bool>&, QMap<QString, bool>& ) const;

  virtual int                         openChoice( const QString& );
  virtual bool                        openAction( const int, const QString& );

  void                                showPreferences( const QString& = QString() );

private:
  void                                emptyPreferences( const QString& );
  QList<QToolBar*>                    findToolBars( const QStringList& names = QStringList() );
  
  QByteArray                          processState(QByteArray& input, 
						   const bool processWin,
						   const bool processTb,
						   const bool isRestoring,
						   QByteArray defaultState = QByteArray());

protected:
  typedef QPointer<QWidget>         WinPtr;
  typedef QMap<int, WinPtr>         WinMap;
  typedef QMap<QString, QByteArray> WinVis;
  typedef QMap<QString, QByteArray> WinGeom;

  enum { OpenReload = CAM_Application::OpenExist + 1 };

protected:
  LightApp_Preferences*               myPrefs;
  LightApp_SelectionMgr*              mySelMgr;

  LightApp_FullScreenHelper*          myScreenHelper;

  WinMap                              myWin;
  WinVis                              myWinVis;

  SUIT_Accel*                         myAccel;
  QTimer*                             myAutoSaveTimer;

  static LightApp_Preferences*        _prefs_;

  static int                          lastStudyId;
  QStringList                         myUserWmTypes;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
