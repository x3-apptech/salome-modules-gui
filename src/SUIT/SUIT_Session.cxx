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
#include "SUIT_Session.h"

#include "SUIT_Tools.h"
#include "SUIT_Desktop.h"
#include "SUIT_MessageBox.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"
#include "SUIT_ExceptionHandler.h"

#include <qtextcodec.h>
#include <qmessagebox.h>
#include <qapplication.h>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

static bool   SUIT_Session_IsPythonExecuted = false;
static QMutex SUIT_Session_PythonMutex;

SUIT_Session* SUIT_Session::mySession = 0;

/*! Constructor.*/

SUIT_Session::SUIT_Session()
: QObject(),
myResMgr( 0 ),
myHandler( 0 ),
myActiveApp( 0 ),
myExitStatus( NORMAL ),
myExitFlags ( 0 )
{
  SUIT_ASSERT( !mySession )

  mySession = this;

  myAppList.setAutoDelete( true );
}

/*!destructor. Clear applications list and set mySession to zero.*/
SUIT_Session::~SUIT_Session()
{
  myAppList.clear();

  if (myResMgr) {
    delete myResMgr;
    myResMgr = 0;
  }
  mySession = 0;
}

/*! \retval return mySession */
SUIT_Session* SUIT_Session::session()
{
  return mySession;
}

/*!
  Starts new application using "createApplication" function of loaded DLL.
*/

SUIT_Application* SUIT_Session::startApplication( const QString& name, int args, char** argv )
{
  AppLib libHandle = 0;

  QString appName = applicationName( name );
  if ( myAppLibs.contains( appName ) )
    libHandle = myAppLibs[appName];

  QString lib;
  if ( !libHandle )
    libHandle = loadLibrary( name, lib );

  if ( !libHandle )
  {
    SUIT_MessageBox::warn1( 0, tr( "Error" ),
                            tr( "Can not load application library \"%1\": %2").arg( lib ).arg( lastError() ), tr( "Ok" ) );
    return 0;
  }

  if ( !myAppLibs.contains( appName ) || !myAppLibs[appName] ) // jfa 22.06.2005
    myAppLibs.insert( appName, libHandle );

  APP_CREATE_FUNC crtInst = 0;

#ifdef WIN32
  crtInst = (APP_CREATE_FUNC)::GetProcAddress( libHandle, APP_CREATE_NAME );
#else
  crtInst = (APP_CREATE_FUNC)dlsym( libHandle, APP_CREATE_NAME );
#endif

  if ( !crtInst )
  {
    SUIT_MessageBox::warn1( 0, tr( "Error" ),
                            tr( "Can not find function \"%1\": %2" ).arg( APP_CREATE_NAME ).arg( lastError() ), tr( "Ok" ) );
    return 0;
  }

  // Prepare Resource Manager for the new application if it doesn't exist yet
  if ( !myResMgr )
  {
    myResMgr = createResourceMgr( appName );
    myResMgr->loadLanguage();
  }

  //jfa 22.06.2005:SUIT_Application* anApp = crtInst( args, argv );
  SUIT_Application* anApp = crtInst();
  if ( !anApp )
  {
    SUIT_MessageBox::warn1( 0, tr( "Error" ), tr( "Can not create application \"%1\": %2").arg( appName ).arg( lastError() ), tr( "Ok" ) );
    return 0;
  }

  anApp->setName( appName );

  connect( anApp, SIGNAL( applicationClosed( SUIT_Application* ) ),
           this, SLOT( onApplicationClosed( SUIT_Application* ) ) );
  connect( anApp, SIGNAL( activated( SUIT_Application* ) ), 
	         this, SLOT( onApplicationActivated( SUIT_Application* ) ) );

  myAppList.append( anApp );

  if ( !myHandler )
  {
    APP_GET_HANDLER_FUNC crtHndlr = 0;
#ifdef WIN32
    crtHndlr = (APP_GET_HANDLER_FUNC)::GetProcAddress( libHandle, APP_GET_HANDLER_NAME );
#else
    crtHndlr = (APP_GET_HANDLER_FUNC)dlsym( libHandle, APP_GET_HANDLER_NAME );
#endif
    if ( crtHndlr )
      myHandler = crtHndlr();
  }

  anApp->start();

  // Application can be closed during starting (not started).
  if ( !myAppList.contains( anApp ) )
    anApp = 0;

  return anApp;
}

/*!
  Gets the list of all applications
*/
QPtrList<SUIT_Application> SUIT_Session::applications() const
{
  QPtrList<SUIT_Application> apps;
  apps.setAutoDelete( false );

  for ( AppListIterator it( myAppList ); it.current(); ++it )
    apps.append( it.current() );

  return apps;
}

/*!
  Returns the active application
*/
SUIT_Application* SUIT_Session::activeApplication() const
{
  /*
  if ( myAppList.count() == 1 )
    return myAppList.getFirst();

  SUIT_Desktop* desktop = 0;
  for ( AppListIterator it( myAppList ); it.current() && !desktop; ++it )
  {
    SUIT_Desktop* desk = it.current()->desktop();
    if ( desk && desk->isActiveWindow() )
      desktop = desk;
  }

  if ( !desktop )
    return 0;

  SUIT_ViewWindow* win = desktop->activeWindow();
  if ( !win || !win->getViewManager() )
    return 0;

  SUIT_Study* study = win->getViewManager()->study();
  if ( !study )
    return 0;

  return study->application();
  */
  return myActiveApp;
}

/*!
  Returns the resource manager for the specified application name.
*/
SUIT_ResourceMgr* SUIT_Session::resourceMgr() const
{
  return myResMgr;
}

/*!
  Removes the application from the list of launched applications.
  If it is a last application the session will be closed.
*/
void SUIT_Session::onApplicationClosed( SUIT_Application* theApp )
{
  emit applicationClosed( theApp );

  myAppList.remove( theApp );
  if ( theApp == myActiveApp )
    myActiveApp = 0;

  if ( myAppList.isEmpty() )
  {
    printf( "Calling QApplication::exit() with exit code = %d\n", myExitStatus );
    qApp->exit( myExitStatus );
  }
}

/*!
  Destroys session by closing all applications.
*/
void SUIT_Session::closeSession( int mode, int flags )
{
  while ( !myAppList.isEmpty() )
  {
    SUIT_Application* app = myAppList.getFirst();
    bool closePermanently;
    if ( mode == ASK && !app->isPossibleToClose( closePermanently ) )
      return;
    else if ( mode == SAVE )
    {
      SUIT_Study* study = app->activeStudy();
      if ( study->isModified() && study->isSaved() )
	study->saveDocument();
    }
    else if ( mode == DONT_SAVE )
    {
      myExitStatus = FORCED;
    }

    app->closeApplication();
  }
  myExitFlags = flags;
}

/*!
  Get session exit flags.

  By default, exit flags are set to 0. You can use pass any flags to the
  closeSession() method if you need to process them later on application
  quiting.

  \return exit flags
*/
int SUIT_Session::exitFlags() const
{
  return myExitFlags;
}

/*! \retval return myHandler*/
SUIT_ExceptionHandler* SUIT_Session::handler() const
{
  return myHandler;
}

/*! \retval return last error string.*/
QString SUIT_Session::lastError() const
{
  QString str;
#ifdef WNT
  LPVOID lpMsgBuf;
  ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS, 0, ::GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, 0 );
  str = QString( (LPTSTR)lpMsgBuf );
  LocalFree( lpMsgBuf );
#else
  str = QString( dlerror() );
#endif
  return str;
}

/*! Load library to session.
 * \retval Loaded library.
 */
SUIT_Session::AppLib SUIT_Session::loadLibrary( const QString& name, QString& libName )
{
  QString libFile = SUIT_Tools::library( name );

  libName = libFile;
  if ( libFile.isEmpty() )
    return 0;

  AppLib lib = 0;
#ifdef WIN32
  lib = ::LoadLibrary( (char*)libFile.latin1() );
#else
  lib = dlopen( (char*)libFile.latin1(), RTLD_LAZY | RTLD_GLOBAL  );
#endif
  return lib;
}

/*! \retval Return file name by application name.*/
QString SUIT_Session::applicationName( const QString& str ) const
{
#ifdef WIN32
  return SUIT_Tools::file( str, false );
#else
  QString fileName = SUIT_Tools::file( str, false );
  if ( fileName.startsWith( "lib" ) )
    fileName = fileName.right( fileName.length() - 3 );
  return fileName;
#endif
}

/*!
  Virtual method, creates an instance of ResourceManager
*/
SUIT_ResourceMgr* SUIT_Session::createResourceMgr( const QString& appName ) const
{
  return new SUIT_ResourceMgr( applicationName( appName ) );
}

/*!
  Slot, called on activation of some application's desktop
*/
void SUIT_Session::onApplicationActivated( SUIT_Application* app )
{
  myActiveApp = app;
}

/*!
  \retval Return TRUE, if a command is currently executed in Python Console,
                 FALSE otherwise.
*/
bool SUIT_Session::IsPythonExecuted()
{
  bool ret;
  SUIT_Session_PythonMutex.lock();
  ret = SUIT_Session_IsPythonExecuted;
  SUIT_Session_PythonMutex.unlock();
  return ret;
}

/*!
  Set value of boolean flag, being returned by method \a IsPythonExecuted().
  It is supposed to set the flag to TRUE when any python command starts
  and reset it to FALSE when the command finishes.
*/
void SUIT_Session::SetPythonExecuted(bool isPythonExecuted)
{
  SUIT_Session_PythonMutex.lock();
  SUIT_Session_IsPythonExecuted = isPythonExecuted;
  SUIT_Session_PythonMutex.unlock();
}
