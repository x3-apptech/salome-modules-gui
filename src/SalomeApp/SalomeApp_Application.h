// File:      SalomeApp_Application.h
// Created:   10/22/2004 3:37:25 PM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#ifndef SALOMEAPP_APPLICATION_H
#define SALOMEAPP_APPLICATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SalomeApp.h"
#include <CAM_Application.h>

#include <CORBA.h>

#include <SALOMEconfig.h>
//#include CORBA_CLIENT_HEADER(SALOMEDS)
#include <SALOME_NamingService.hxx>

#include "SALOMEDSClient.hxx"

class QAction;
class QComboBox;
class QDockWindow;

class LogWindow;
class OB_Browser;
class PythonConsole;
class SalomeApp_Module;
class SalomeApp_Preferences;
class SalomeApp_SelectionMgr;
class SalomeApp_WidgetContainer;
class SUIT_Accel;

class SALOME_LifeCycleCORBA;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  Description : Application containing SalomeApp module
*/

class SALOMEAPP_EXPORT SalomeApp_Application : public CAM_Application
{
  Q_OBJECT

public:
  typedef enum { WT_ObjectBrowser, WT_PyConsole, WT_LogWindow, WT_User } WindowTypes;

  enum { ModulesListId = STD_Application::UserID, NewGLViewId,
         NewPlot2dId, NewOCCViewId, NewVTKViewId, DumpStudyId,
	 LoadScriptId, PropertiesId, PreferencesId, MRUId, 
	 CatalogGenId, RegDisplayId, UserID };

public:
  SalomeApp_Application();
  virtual ~SalomeApp_Application();
  
  virtual QString                     applicationName() const;
  virtual QString                     applicationVersion() const;

  virtual CAM_Module*                 loadModule( const QString& );
  virtual bool                        activateModule( const QString& );
  
  virtual bool                        useStudy( const QString& );

  SalomeApp_SelectionMgr*             selectionMgr() const;

  LogWindow*                          logWindow();
  OB_Browser*                         objectBrowser();
  PythonConsole*                      pythonConsole();   

  virtual void                        updateObjectBrowser( const bool = true );

  SalomeApp_Preferences*              preferences() const;

  virtual QString                     getFileFilter() const;
  virtual QString                     getFileName( bool open, const QString& initial, const QString& filters, 
						   const QString& caption, QWidget* parent );
  virtual QString                     getDirectory( const QString& initial, const QString& caption, QWidget* parent );
  virtual QStringList                 getOpenFileNames( const QString& initial, const QString& filters, 
							const QString& caption, QWidget* parent );

  SUIT_ViewManager*                   getViewManager( const QString&, const bool );

  void                                updateActions();

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

  static CORBA::ORB_var               orb();
  static SALOMEDSClient_StudyManager* studyMgr();
  static SALOME_NamingService*        namingService();
  static SALOME_LifeCycleCORBA*       lcc();
  static QString                      defaultEngineIOR();

signals:
  void                                studyOpened();
  void                                studySaved();
  void                                studyClosed();

public slots:
  virtual void                        onNewDoc();
  virtual void                        onOpenDoc();
  virtual void                        onHelpAbout();
  virtual void                        onHelpContentsModule();
  virtual bool                        onOpenDoc( const QString& );
  virtual void                        onLoadDoc();
  virtual bool                        onLoadDoc( const QString& );
  virtual void                        onCopy();
  virtual void                        onPaste();  
  
private slots:
  void                                onSelection();
  void                                onRefresh();
  void                                onDeleteReferences();

protected:
  virtual void                        createActions();
  virtual SUIT_Study*                 createNewStudy();
  virtual QWidget*                    createWindow( const int );
  virtual void                        defaultWindows( QMap<int, int>& ) const;
  virtual void                        defaultViewManagers( QStringList& ) const;

  virtual void                        setActiveStudy( SUIT_Study* );

  virtual void                        updateCommandsStatus();

  virtual void                        onSelectionChanged();

  virtual void                        beforeCloseDoc( SUIT_Study* );
  virtual void                        afterCloseDoc();

  virtual void                        moduleAdded( CAM_Module* );
  virtual bool                        activateModule( CAM_Module* = 0 );

  SalomeApp_Preferences*              preferences( const bool ) const;

  virtual void                        createPreferences( SalomeApp_Preferences* );
  virtual void                        preferencesChanged( const QString&, const QString& );
  virtual void                        updateDesktopTitle();

protected slots:
  virtual void                        onDesktopActivated();

private slots:
  void                                onNewWindow();
  void                                onModuleActivation( QAction* );
  void                                onCloseView( SUIT_ViewManager* );

  void                                onStudyCreated( SUIT_Study* );
  void                                onStudyOpened( SUIT_Study* );
  void                                onStudySaved( SUIT_Study* );
  void                                onStudyClosed( SUIT_Study* );
  
  void                                onProperties();
  void                                onDumpStudy();
  void                                onLoadScript(); 

  void                                onPreferences();
  void                                onMRUActivated( QString );

  void                                onCatalogGen();
  void                                onRegDisplay();

  void                                onPreferenceChanged( QString&, QString&, QString& );
  void                                onOpenWith();

private:
  void                                updateWindows();
  void                                updateViewManagers();
  void                                updateModuleActions();

  void                                loadWindowsGeometry();
  void                                saveWindowsGeometry();

  void                                updatePreference( const QString&, const QString&, const QString& );

  QString                             defaultModule() const;
  void                                currentWindows( QMap<int, int>& ) const;
  void                                currentViewManagers( QStringList& ) const;
  SUIT_ViewManager*                   createViewManager( const QString& vmType );
  void                                moduleIconNames( QMap<QString, QString>& ) const;

  void                                activateWindows();

private:
  typedef QMap<QString, QAction*>               ActionMap;
  typedef QMap<int, SalomeApp_WidgetContainer*> WindowMap;

private:
  SalomeApp_Preferences*              myPrefs;
  SalomeApp_SelectionMgr*             mySelMgr;
  ActionMap                           myActions;
  WindowMap                           myWindows;

  SUIT_Accel*                         myAccel;

  static SalomeApp_Preferences*       _prefs_;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
