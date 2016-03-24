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

#ifndef SUIT_SESSION_H
#define SUIT_SESSION_H

#include "SUIT.h"
#include "SUIT_Application.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef WIN32
#define LIB_HANDLE HINSTANCE
#else
#define LIB_HANDLE void*
#endif

class SUIT_ResourceMgr;
class SUIT_ExceptionHandler;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif
/*!
  The class Sesssion manages launching of Applications. Application must be returned \n
  by static function "createApplication" in external library. The Library must be loaded with \n
  loadLibrary method and after that application can be started.
*/
class SUIT_EXPORT SUIT_Session : public QObject
{
  Q_OBJECT

public:
  typedef LIB_HANDLE AppLib;

  enum { ASK = 0, SAVE, DONT_SAVE } CloseMode;
  enum { NORMAL = 0, FORCED } ExitStatus;

public:
  SUIT_Session();
  SUIT_Session( int, char** );
  virtual ~SUIT_Session();

  static SUIT_Session*         session();

  QStringList                  arguments();

  SUIT_Application*            startApplication( const QString&, int = 0, char** = 0 );

  QList<SUIT_Application*>     applications() const;
  SUIT_Application*            activeApplication() const;

  SUIT_ResourceMgr*            resourceMgr() const;

  void                         closeSession( int mode = ASK, int flags = 0 );
  int                          exitFlags() const;

  SUIT_ExceptionHandler*       handler() const;

  void                         insertApplication( SUIT_Application* );

signals:
  void                         applicationClosed( SUIT_Application* );

protected:
  virtual SUIT_ResourceMgr*    createResourceMgr( const QString& ) const;

private slots:
  void                         onApplicationClosed( SUIT_Application* );
  void                         onApplicationActivated( SUIT_Application* ); 

private:
  typedef QList<SUIT_Application*> AppList;
  typedef QMap<QString, AppLib>    AppLibMap;

private:
  QString                      lastError() const;
  AppLib                       loadLibrary( const QString&, QString& );
  QString                      applicationName( const QString& ) const;

private:
  QStringList                  myArguments;

  SUIT_ResourceMgr*            myResMgr;
  AppList                      myAppList;
  AppLibMap                    myAppLibs;
  SUIT_Application*            myActiveApp;

  SUIT_ExceptionHandler*       myHandler;
  static SUIT_Session*         mySession;

  int                          myExitStatus;
  int                          myExitFlags;
};

#endif
