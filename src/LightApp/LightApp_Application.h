// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef DISABLE_PYCONSOLE
  class PythonConsole;
#endif
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
  typedef enum { WT_ObjectBrowser, 
#ifndef DISABLE_PYCONSOLE
                 WT_PyConsole,
#endif
                 WT_LogWindow,
                 WT_User }
  WindowTypes;

  enum { MenuWindowId = 6 };

  enum { RenameId = CAM_Application::UserID,

#ifndef DISABLE_GLVIEWER
         NewGLViewId ,
#endif

#ifndef DISABLE_PLOT2DVIEWER
         NewPlot2dId,
#endif

#ifndef DISABLE_OCCVIEWER
         NewOCCViewId,
#endif

#ifndef DISABLE_VTKVIEWER
         NewVTKViewId,
#endif

         PreferencesId, MRUId, UserID };
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
#ifndef DISABLE_PYCONSOLE
  PythonConsole*                      pythonConsole(); 
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

  void                                setDefaultStudyName( const QString& theName );

  static int                          studyId();

  virtual bool                        event( QEvent* );

signals:
  void                                studyOpened();
  void                                studySaved();
  void                                studyClosed();

public slots:
  virtual void                        onHelpContentsModule();
  virtual void                        onHelpContextModule( const QString&, const QString& );
  virtual void                        onNewDoc();
  virtual void                        onOpenDoc();
  virtual void                        onHelpAbout();
  virtual bool                        onOpenDoc( const QString& );
  virtual bool                        onLoadDoc( const QString& );

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
  virtual void                        onSelectionChanged();

  virtual void                        beforeCloseDoc( SUIT_Study* );
  virtual void                        afterCloseDoc();
  virtual void                        moduleAdded( CAM_Module* );
  virtual bool                        activateModule( CAM_Module* = 0 );

  LightApp_Preferences*               preferences( const bool ) const;
  virtual void                        createPreferences( LightApp_Preferences* );
  virtual void                        preferencesChanged( const QString&, const QString& );
  virtual void                        savePreferences();
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
  void                                onVisibilityChanged( bool );

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
  bool                                isLibExists( const QString& ) const;

protected:
  typedef QMap<QString, QAction*>              ActionMap;
  typedef QMap<int, LightApp_WidgetContainer*> WindowMap;
  typedef QMap<int, bool>                      WindowVisibilityMap;

protected:
  LightApp_Preferences*               myPrefs;
  LightApp_SelectionMgr*              mySelMgr;
  ActionMap                           myActions;
  WindowMap                           myWindows;
  WindowVisibilityMap                 myWindowsVisible;

  SUIT_Accel*                         myAccel;

  static LightApp_Preferences*        _prefs_;

  static int                          lastStudyId;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
