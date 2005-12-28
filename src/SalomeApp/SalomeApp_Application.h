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
#include <LightApp_Application.h>

#include <CORBA.h>

#include <SALOMEconfig.h>
//#include CORBA_CLIENT_HEADER(SALOMEDS)
#include <SALOME_NamingService.hxx>

#include "SALOMEDSClient.hxx"

class QAction;
class QComboBox;
class QDockWindow;

class LightApp_Preferences;
class SalomeApp_Module;

class SALOME_LifeCycleCORBA;

class QListViewItem;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  Description : Application containing SalomeApp module or LightApp module
*/

class SALOMEAPP_EXPORT SalomeApp_Application : public LightApp_Application
{
  Q_OBJECT

public:
  enum { DumpStudyId = LightApp_Application::UserID, LoadScriptId, PropertiesId,
         CatalogGenId, RegDisplayId, UserID };

public:
  SalomeApp_Application();
  virtual ~SalomeApp_Application();

  virtual void                        updateObjectBrowser( const bool = true );

  virtual QString                     getFileFilter() const;

  virtual void                        start();

  virtual void                        contextMenuPopup( const QString&, QPopupMenu*, QString& );

  static CORBA::ORB_var               orb();
  static SALOMEDSClient_StudyManager* studyMgr();
  static SALOME_NamingService*        namingService();
  static SALOME_LifeCycleCORBA*       lcc();
  static QString                      defaultEngineIOR();

public slots:
  virtual bool                        onOpenDoc( const QString& );
  virtual void                        onLoadDoc();
  virtual bool                        onLoadDoc( const QString& );
  virtual void                        onCopy();
  virtual void                        onPaste();

protected:
  virtual void                        createActions();
  virtual SUIT_Study*                 createNewStudy();
  virtual QWidget*                    createWindow( const int );

  virtual void                        updateCommandsStatus();
  virtual void                        onSelectionChanged();

  virtual void                        createPreferences( LightApp_Preferences* );
  virtual void                        updateDesktopTitle();

private slots:
  void                                onDeleteInvalidReferences();
  void                                onDblClick( QListViewItem* );
  void                                onProperties();
  void                                onDumpStudy();
  void                                onLoadScript(); 

  void                                onCatalogGen();
  void                                onRegDisplay();
  void                                onOpenWith();
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
