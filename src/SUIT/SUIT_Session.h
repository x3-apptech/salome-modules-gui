#ifndef SUIT_SESSION_H
#define SUIT_SESSION_H

#include "SUIT.h"

#include "SUIT_Application.h"
#include "SUIT_ResourceMgr.h"

#include <qobject.h>
#include <qptrlist.h>
#include <qptrvector.h>
#include <qstringlist.h>

#ifdef WIN32
#define LIB_HANDLE HINSTANCE
#else
#define LIB_HANDLE void*
#endif

class SUIT_ResourceMgr;
class SUIT_ExceptionHandler;

/*!
  The class Sesssion manages launching of Applications. Application must be returned
  by static function "createApplication" in external library. The Library must be loaded with 
  loadLibrary method and after that application can be started.
*/

#ifdef WNT
#pragma warning( disable:4251 )
#endif

class SUIT_EXPORT SUIT_Session: public QObject
{
  Q_OBJECT

public:
  typedef LIB_HANDLE AppLib;

  enum { ASK = 0, SAVE, DONT_SAVE } CloseMode;
  enum { FROM_GUI = 0, FROM_CORBA_SESSION } ExitStatus;

public:
  SUIT_Session();
  virtual ~SUIT_Session();

  static SUIT_Session*         session();

  SUIT_Application*            startApplication( const QString&, int = 0, char** = 0 );

  QPtrList<SUIT_Application>   applications() const;
  SUIT_Application*            activeApplication() const;

  SUIT_ResourceMgr*            resourceMgr() const;

  void                         closeSession( int mode = ASK );

  SUIT_ExceptionHandler*       handler() const;

signals:
  void                         applicationClosed( SUIT_Application* );

protected:
  virtual SUIT_ResourceMgr*    createResourceMgr( const QString& ) const;

private slots:
  void                         onApplicationClosed( SUIT_Application* );
  void                         onApplicationActivated( SUIT_Application* ); 

private:
  typedef QPtrList<SUIT_Application>         AppList;
  typedef QMap<QString, AppLib>              AppLibMap;
  typedef QPtrListIterator<SUIT_Application> AppListIterator;

private:
  QString                      lastError() const;
  AppLib                       loadLibrary( const QString& );
  QString                      applicationName( const QString& ) const;

private:
  SUIT_ResourceMgr*            myResMgr;
  AppList                      myAppList;
  AppLibMap                    myAppLibs;
  SUIT_Application*            myActiveApp;

  SUIT_ExceptionHandler*       myHandler;
  static SUIT_Session*         mySession;

  int                          myExitStatus;
};

#endif
