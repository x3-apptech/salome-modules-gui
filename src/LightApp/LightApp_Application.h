// File:      LightApp_Application.h
// Created:   6/20/2005 18:39:25 PM
// Author:    OCC team
// Copyright (C) CEA 2005

#ifndef LIGHTAPP_APPLICATION_H
#define LIGHTAPP_APPLICATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LightApp.h"
#include <CAM_Application.h>

class LogWindow;
class OB_Browser;
class PythonConsole;
class STD_Application;
class LightApp_WidgetContainer;
class LightApp_Preferences;
class LightApp_SelectionMgr;
class SUIT_Study;
class SUIT_Accel;
class CAM_Module;

class QString;
class QWidget;
class QStringList;
class QPixmap;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  Description : Application containing only LightApp module
*/

class LIGHTAPP_EXPORT LightApp_Application : public CAM_Application
{
  Q_OBJECT

public:
  typedef enum { WT_ObjectBrowser, WT_PyConsole, WT_LogWindow, WT_User } WindowTypes;

  enum { NewGLViewId = CAM_Application::UserID, NewPlot2dId, NewOCCViewId, NewVTKViewId,
         PreferencesId, MRUId, RenameId, UserID };
public:
  LightApp_Application();
  virtual ~LightApp_Application();

  virtual QString                     applicationName() const;
  virtual QString                     applicationVersion() const;

  virtual CAM_Module*                 loadModule( const QString& );
  virtual bool                        activateModule( const QString& );

  virtual bool                        useStudy( const QString& );

  LightApp_SelectionMgr*              selectionMgr() const;
  
  LogWindow*                          logWindow();
  OB_Browser*                         objectBrowser();
  PythonConsole*                      pythonConsole(); 

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
  virtual void                        removeViewManager( SUIT_ViewManager* );
  QWidget*                            getWindow( const int, const int = -1 );
  QWidget*                            window( const int, const int = -1 ) const;
  void                                addWindow( QWidget*, const int, const int = -1 );
  void                                removeWindow( const int, const int = -1 );

  bool                                isWindowVisible( const int ) const;
  void                                setWindowShown( const int, const bool );

  virtual void                        start();

  virtual void                        contextMenuPopup( const QString&, QPopupMenu*, QString& );

  virtual void                        createEmptyStudy();

  SUIT_Accel*                         accel() const;

signals:
  void                                studyOpened();
  void                                studySaved();
  void                                studyClosed();

public slots:
  virtual void                        onHelpContentsModule();
  virtual void                        onNewDoc();
  virtual void                        onOpenDoc();
  virtual void                        onHelpAbout();
  virtual bool                        onOpenDoc( const QString& );
  virtual bool                        onLoadDoc( const QString& );

protected:
  virtual void                        createActions();
  virtual SUIT_Study*                 createNewStudy();
  virtual QWidget*                    createWindow( const int );
  virtual void                        defaultWindows( QMap<int, int>& ) const;
  void                                defaultViewManagers( QStringList& ) const;

  virtual void                        setActiveStudy( SUIT_Study* );
  virtual void                        updateCommandsStatus();
  virtual void                        onSelectionChanged();

  virtual void                        beforeCloseDoc( SUIT_Study* );
  virtual void                        afterCloseDoc();
  virtual void                        moduleAdded( CAM_Module* );
  virtual bool                        activateModule( CAM_Module* = 0 );

  LightApp_Preferences*               preferences( const bool ) const;
  virtual void                        createPreferences( LightApp_Preferences* );
  virtual void                        preferencesChanged( const QString&, const QString& );
  virtual void                        updateDesktopTitle();

protected slots:
  virtual void                        onDesktopActivated();

  void                                onNewWindow();
  void                                onModuleActivation( QAction* );
  void                                onCloseView( SUIT_ViewManager* );

  void                                onStudyCreated( SUIT_Study* );
  void                                onStudyOpened( SUIT_Study* );
  void                                onStudySaved( SUIT_Study* );
  void                                onStudyClosed( SUIT_Study* );

  void                                onWCDestroyed( QObject* );

private slots:
  void                                onSelection();
  void                                onRefresh();
  void                                onPreferences();
  void                                onMRUActivated( QString );
  void                                onPreferenceChanged( QString&, QString&, QString& );
  void                                onRenameWindow();

protected:
  void                                updateWindows();
  void                                updateViewManagers();
  void                                updateModuleActions();

  void                                loadWindowsGeometry();
  void                                saveWindowsGeometry();

  void                                updatePreference( const QString&, const QString&, const QString& );

  QString                             defaultModule() const;
  void                                currentWindows( QMap<int, int>& ) const;
  void                                currentViewManagers( QStringList& ) const;
  virtual SUIT_ViewManager*           createViewManager( const QString& vmType );
  void                                moduleIconNames( QMap<QString, QString>& ) const;

  void                                activateWindows();

protected:
  typedef QMap<QString, QAction*>              ActionMap;
  typedef QMap<int, LightApp_WidgetContainer*> WindowMap;

protected:
  LightApp_Preferences*               myPrefs;
  LightApp_SelectionMgr*              mySelMgr;
  ActionMap                           myActions;
  WindowMap                           myWindows;

  SUIT_Accel*                         myAccel;

  static LightApp_Preferences*        _prefs_;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
