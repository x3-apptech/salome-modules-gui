// SALOME Session : implementation of Session.idl
//
// Copyright (C) 2003 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org
//
//
//
// File : SALOME_Session_Server.cxx
// Author : Paul RASCLE, EDF
// Module : SALOME

#include <Container_init_python.hxx>
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "SALOME_NamingService.hxx"
#include "SALOMETraceCollector.hxx"

#include "InquireServersQThread.h" // splash

#include <iostream>
#ifndef WNT
#include <unistd.h>
#endif

#include <qdir.h>
#include <qfile.h>
#include <qapplication.h>
#include <qwaitcondition.h>

#include "Utils_SALOME_Exception.hxx"
#include "Utils_CorbaException.hxx"
#include "SALOME_Event.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Session)
#include CORBA_SERVER_HEADER(SALOMEDS)

#include <utilities.h>
#include "Session_Session_i.hxx"
#include "Session_ServerLauncher.hxx"

#include "SUIT_Tools.h"
#include "SUIT_Session.h"
#include "SUIT_Application.h"
#include "SUIT_MessageBox.h"
#include "SUIT_ResourceMgr.h"

#include "SUIT_ExceptionHandler.h"

extern "C" int HandleSignals( QApplication *theQApplication );

/*! - read arguments, define list of server to launch with their arguments.
 * - wait for naming service
 * - create and run a thread for launch of all servers
 *
*/

//! CORBA server for SALOME Session
/*!
 * SALOME_Session Server launches a SALOME session servant.
 * The servant registers to the Naming Service.
 * See SALOME_Session.idl for interface specification.
 *
 * Main services offered by the servant are:
 * - launch GUI
 * - stop Session ( must be idle )
 * - get session state
 */

PyObject* salome_shared_modules_module = 0;

void MessageOutput( QtMsgType type, const char* msg )
{
  switch ( type )
  {
  case QtDebugMsg:
    MESSAGE( "Debug: " << msg );
    break;
  case QtWarningMsg:
    MESSAGE( "Warning: " << msg );
    break;
  case QtFatalMsg:
    MESSAGE( "Fatal: " << msg );
    break;
  }
}

/* XPM */
static const char* pixmap_not_found_xpm[] = {
"16 16 3 1",
"       c None",
".      c #000000",
"+      c #A80000",
"                ",
"                ",
"    .     .     ",
"   .+.   .+.    ",
"  .+++. .+++.   ",
"   .+++.+++.    ",
"    .+++++.     ",
"     .+++.      ",
"    .+++++.     ",
"   .+++.+++.    ",
"  .+++. .+++.   ",
"   .+.   .+.    ",
"    .     .     ",
"                ",
"                ",
"                "};

QString salomeVersion()
{
  QString path( ::getenv( "GUI_ROOT_DIR" ) );
  if ( !path.isEmpty() )
    path += QDir::separator();
  path += QString( "bin/salome/VERSION" );

  QFile vf( path );
  if ( !vf.open( IO_ReadOnly ) )
    return QString::null;

  QString line;
  vf.readLine( line, 1024 );
  vf.close();

  if ( line.isEmpty() )
    return QString::null;

  while ( !line.isEmpty() && line.at( line.length() - 1 ) == QChar( '\n' ) )
    line.remove( line.length() - 1, 1 );

  QString ver;
  int idx = line.findRev( ":" );
  if ( idx != -1 )
    ver = line.mid( idx + 1 ).stripWhiteSpace();

  return ver;
}

class SALOME_ResourceMgr : public SUIT_ResourceMgr
{
public:
  SALOME_ResourceMgr( const QString& app, const QString& resVarTemplate ) : SUIT_ResourceMgr( app, resVarTemplate )
  {
    setCurrentFormat( "xml" );
    setOption( "translators", QString( "%P_msg_%L.qm|%P_icons.qm|%P_images.qm" ) );
    setDefaultPixmap( QPixmap( pixmap_not_found_xpm ) );
  }
  static void initResourceMgr()
  {
    if ( myExtAppName.isNull() || myExtAppVersion.isNull() ) {
      SALOME_ResourceMgr resMgr( "SalomeApp", QString( "%1Config" ) );
      resMgr.loadLanguage( "SalomeApp", "en" );

      myExtAppName = QObject::tr( "APP_NAME" ).stripWhiteSpace();
      if ( myExtAppName == "APP_NAME" || myExtAppName.lower() == "salome" ) 
        myExtAppName = "SalomeApp";
      myExtAppVersion = QObject::tr( "APP_VERSION" );
      if ( myExtAppVersion == "APP_VERSION" ) {
        if ( myExtAppName != "SalomeApp" )
          myExtAppVersion = "";
	else myExtAppVersion = salomeVersion();
      }
    }
  }
  QString version() const { return myExtAppVersion; }

protected:
  QString userFileName( const QString& appName ) const
  { 
    if ( version().isNull()  ) return ""; 
    return SUIT_ResourceMgr::userFileName( myExtAppName );
  }

public:
  static QString myExtAppName;
  static QString myExtAppVersion;
};

QString SALOME_ResourceMgr::myExtAppName    = QString::null;
QString SALOME_ResourceMgr::myExtAppVersion = QString::null;

class SALOME_Session : public SUIT_Session
{
public:
  SALOME_Session() : SUIT_Session() {}
  virtual ~SALOME_Session() {}

protected:
  virtual SUIT_ResourceMgr* createResourceMgr( const QString& appName ) const
  {
    SALOME_ResourceMgr::initResourceMgr();
    SALOME_ResourceMgr* resMgr = new SALOME_ResourceMgr( appName, QString( "%1Config" ) );
    return resMgr;
  }
};

class SALOME_QApplication : public QApplication
{
public:
  SALOME_QApplication( int& argc, char** argv ) : QApplication( argc, argv ), myHandler ( 0 ) {}

  virtual bool notify( QObject* receiver, QEvent* e )
  {
    return myHandler ? myHandler->handle( receiver, e ) :
      QApplication::notify( receiver, e );
  }
  SUIT_ExceptionHandler* handler() const { return myHandler; }
  void setHandler( SUIT_ExceptionHandler* h ) { myHandler = h; }

private:
  SUIT_ExceptionHandler* myHandler;
};

// class which calls SALOME::Session::GetInterface() from another thread
// to avoid mutual lock ( if called from the same thread as main()
class GetInterfaceThread : public QThread
{
public:
  GetInterfaceThread( SALOME::Session_var s ) : session ( s ) {}
protected:
  virtual void run()
  {
    if ( !CORBA::is_nil( session ) )
      session->GetInterface();
    else
      printf( "\nFATAL ERROR: SALOME::Session object is nil! Can not display GUI\n\n" );
  }
private:
  SALOME::Session_var session;
};

// returns true if 'str' is found in argv
bool isFound( const char* str, int argc, char** argv )
{
  for ( int i = 1; i <= ( argc-1 ); i++ )
    if ( !strcmp( argv[i], str ) )
      return true;
  return false;
}

// ---------------------------- MAIN -----------------------
int main( int argc, char **argv )
{
  qInstallMsgHandler( MessageOutput );

  // QApplication should be create before all other operations
  // When uses QApplication::libraryPaths() ( example, QFile::encodeName() )
  // qApp used for detection of the executable dir path.
  SALOME_QApplication _qappl( argc, argv );
  ASSERT( QObject::connect( &_qappl, SIGNAL( lastWindowClosed() ), &_qappl, SLOT( quit() ) ) );

  QString path = QDir::convertSeparators( SUIT_Tools::addSlash( QString( ::getenv( "GUI_ROOT_DIR" ) ) ) + QString( "bin/salome" ) );
  _qappl.addLibraryPath( path );
  
  _qappl.setStyle( "salome" );

  int result = -1;

  CORBA::ORB_var orb;
  PortableServer::POA_var poa;

  SUIT_Session* aGUISession = 0;
  SALOME_NamingService* _NS = 0;
  GetInterfaceThread* guiThread = 0;
  Session_ServerLauncher* myServerLauncher = 0;

  try {
    
    // Python initialisation : only once

    int _argc = 1;
    char* _argv[] = {""};
    KERNEL_PYTHON::init_python( _argc,_argv );
    PyEval_RestoreThread( KERNEL_PYTHON::_gtstate );
    if ( !KERNEL_PYTHON::salome_shared_modules_module ) // import only once
      KERNEL_PYTHON::salome_shared_modules_module = PyImport_ImportModule( "salome_shared_modules" );
    if ( !KERNEL_PYTHON::salome_shared_modules_module )
    {
      INFOS( "salome_shared_modules_module == NULL" );
      PyErr_Print();
    }
    PyEval_ReleaseThread( KERNEL_PYTHON::_gtstate );

    // Create ORB, get RootPOA object, NamingService, etc.
    ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance();
    ASSERT( SINGLETON_<ORB_INIT>::IsAlreadyExisting() );
    int orbArgc = 1;
    orb = init( orbArgc, argv );

    // Install SALOME thread event handler
    SALOME_Event::GetSessionThread();

    CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
    poa = PortableServer::POA::_narrow( obj );

    PortableServer::POAManager_var pman = poa->the_POAManager();
    pman->activate() ;
    INFOS( "pman->activate()" );

    _NS = new SALOME_NamingService( orb );

    result = 0;
  }
  catch ( SALOME_Exception& e ) {
    INFOS( "run(): SALOME::SALOME_Exception is caught: "<<e.what() );
  }
  catch ( CORBA::SystemException& e ) {
    INFOS( "Caught CORBA::SystemException." );
  }
  catch ( CORBA::Exception& e ) {
    INFOS( "Caught CORBA::Exception." );
    CORBA::Any tmp;
    tmp<<= e;
    CORBA::TypeCode_var tc = tmp.type();
    const char *p = tc->name();
    INFOS ( "run(): CORBA exception of the kind : "<<p<< " is caught" );
  }
  catch ( exception& e ) {
    INFOS( "run(): An exception has been caught: " <<e.what() );
  }
  catch (...) {
    INFOS( "Caught unknown exception." );
  }

  // CORBA Servant Launcher
  QMutex _GUIMutex;
  QWaitCondition _ServerLaunch, _SessionStarted;

  if ( !result )
  {
    _GUIMutex.lock();  // to block Launch server thread until wait( mutex )

    // Activate embedded CORBA servers: Registry, SALOMEDS, etc.
    myServerLauncher = new Session_ServerLauncher( argc, argv, orb, poa, &_GUIMutex, &_ServerLaunch, &_SessionStarted );
    myServerLauncher->start();

    _ServerLaunch.wait( &_GUIMutex ); // to be reseased by Launch server thread when ready:
    
    // show splash screen if "SPLASH" parameter was passed ( default )
    if ( isFound( "SPLASH", argc, argv ) )
    {
      // create temporary resource manager just to load splash icon
      SUIT_ResourceMgr resMgr( "SalomeApp", QString( "%1Config" ) );
      resMgr.setCurrentFormat( "xml" );
      resMgr.loadLanguage( "LightApp", "en" );

      // create splash object: widget ( splash with progress bar ) and "pinging" thread
      InquireServersGUI splash;
      splash.setPixmap( resMgr.loadPixmap( "LightApp", QObject::tr( "ABOUT_SPLASH" ) ) );
      SUIT_Tools::centerWidget( &splash, _qappl.desktop() );
      
      _qappl.setMainWidget( &splash );
      QObject::connect( &_qappl, SIGNAL( lastWindowClosed() ), &_qappl, SLOT( quit() ) );
      splash.show(); // display splash with running progress bar
      _qappl.exec(); // wait untill splash closes ( progress runs till end or Cancel is pressed )
      
      result = splash.getExitStatus(); // 1 is error
    }
    else
      _SessionStarted.wait();
  }

  // call Session::GetInterface() if "GUI" parameter was passed ( default )
  if ( !result && isFound( "GUI", argc, argv ) )
  {
    CORBA::Object_var obj = _NS->Resolve( "/Kernel/Session" );
    SALOME::Session_var session = SALOME::Session::_narrow( obj ) ;
    ASSERT ( ! CORBA::is_nil( session ) );

    INFOS( "Session activated, Launch IAPP..." );
    guiThread = new GetInterfaceThread( session );
    guiThread->start();
  }

  if ( !result )
  {

    // GUI activation
    // Allow multiple activation/deactivation of GUI
    while ( true )
    {
      MESSAGE( "waiting wakeAll()" );
      _ServerLaunch.wait( &_GUIMutex ); // to be reseased by Launch server thread when ready:
      // atomic operation lock - unlock on mutex
      // unlock mutex: serverThread runs, calls _ServerLaunch->wakeAll()
      // this thread wakes up, and lock mutex

      _GUIMutex.unlock();

      // SUIT_Session creation
      aGUISession = new SALOME_Session();

      // Load SalomeApp dynamic library
      INFOS( "creation SUIT_Application" );
      SUIT_Application* aGUIApp = aGUISession->startApplication( "SalomeApp", 0, 0 );
      if ( aGUIApp )
      {
	if ( !isFound( "noexcepthandler", argc, argv ) )
	  _qappl.setHandler( aGUISession->handler() ); // after loading SalomeApp application
	                                               // aGUISession contains SalomeApp_ExceptionHandler
	// Run GUI loop
	MESSAGE( "run(): starting the main event loop" );
	result = _qappl.exec();

	if ( result == SUIT_Session::FROM_GUI ) // desktop is closed by user from GUI
	  break;
      }

      delete aGUISession;
      aGUISession = 0;

      // Prepare _GUIMutex for a new GUI activation
      _GUIMutex.lock();
    }
  }

  if ( myServerLauncher )
    myServerLauncher->KillAll(); // kill embedded servers

  delete aGUISession;
  delete guiThread;
  delete myServerLauncher;
  delete _NS;

  LocalTraceBufferPool *bp1 = LocalTraceBufferPool::instance();
  LocalTraceBufferPool::deleteInstance(bp1);

  return result;
}
