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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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

#include <iostream>
#ifndef WNT
#include <unistd.h>
#endif

#include <qdir.h>
#include <qfile.h>
#include <qapplication.h>
#include <qwaitcondition.h>
#include <qregexp.h>

#include "Utils_SALOME_Exception.hxx"
#include "Utils_CorbaException.hxx"
#include "SALOME_Event.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Session)
#include CORBA_SERVER_HEADER(SALOMEDS)

#include <utilities.h>
#include "Session_Session_i.hxx"
#include "Session_ServerLauncher.hxx"
#include "Session_ServerCheck.hxx"

#include <QtxSplash.h>
#include "SUIT_Tools.h"
#include "SUIT_Session.h"
#include "SUIT_Application.h"
#include "SUIT_Desktop.h"
#include "SUIT_MessageBox.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_ExceptionHandler.h"

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
  QString userFileName( const QString& appName, const bool for_load ) const
  { 
    if ( version().isNull()  ) return ""; 
    return SUIT_ResourceMgr::userFileName( myExtAppName, for_load );
  }

  virtual int userFileId( const QString& _fname ) const
  {
    QRegExp exp( "\\.SalomeApprc\\.([a-zA-Z0-9.]+)$" );
    QRegExp vers_exp( "^([0-9]+)([A-Za-z]?)([0-9]*)$" );

    QString fname = QFileInfo( _fname ).fileName();
    if( exp.exactMatch( fname ) )
    {
      QStringList vers = QStringList::split( ".", exp.cap( 1 ) );
      int major=0, minor=0;
      major = vers[0].toInt();
      minor = vers[1].toInt();
      if( vers_exp.search( vers[2] )==-1 )
	return -1;
      int release = 0, dev1 = 0, dev2 = 0;
      release = vers_exp.cap( 1 ).toInt();
      dev1 = vers_exp.cap( 2 )[ 0 ].latin1();
      dev2 = vers_exp.cap( 3 ).toInt();

      int dev = dev1*100+dev2, id = major;
      id*=100; id+=minor;
      id*=100; id+=release;
      id*=10000; id+=dev;
      return id;
    }

    return -1;
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
  GetInterfaceThread( SALOME::Session_var s ) : session ( s )
  {
    start();
  }
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
  // Install Qt debug messages handler
  qInstallMsgHandler( MessageOutput );
  
  // Create Qt application instance;
  // this should be done the very first!
  SALOME_QApplication _qappl( argc, argv );
  ASSERT( QObject::connect( &_qappl, SIGNAL( lastWindowClosed() ), &_qappl, SLOT( quit() ) ) );

  // Add application library path (to search style plugin etc...)
  QString path = QDir::convertSeparators( SUIT_Tools::addSlash( QString( ::getenv( "GUI_ROOT_DIR" ) ) ) + QString( "bin/salome" ) );
  _qappl.addLibraryPath( path );
  
  // Set SALOME style to the application
  _qappl.setStyle( "salome" );

  bool isGUI    = isFound( "GUI",    argc, argv );
  bool isSplash = isFound( "SPLASH", argc, argv );
  // Show splash screen (only if both the "GUI" and "SPLASH" parameters are set)
  QtxSplash* splash = 0;
  if ( isGUI && isSplash ) {
    // ...create resource manager
    SUIT_ResourceMgr resMgr( "SalomeApp", QString( "%1Config" ) );
    resMgr.setCurrentFormat( "xml" );
    resMgr.loadLanguage( "LightApp", "en" );
    // ...get splash preferences
    QString splashIcon, splashInfo, splashTextColors, splashProgressColors;
    resMgr.value( "splash", "image",           splashIcon );
    resMgr.value( "splash", "info",            splashInfo, false );
    resMgr.value( "splash", "text_colors",     splashTextColors );
    resMgr.value( "splash", "progress_colors", splashProgressColors );
    QPixmap px( splashIcon );
    if ( px.isNull() ) // try to get splash pixmap from resources
      px = resMgr.loadPixmap( "LightApp", QObject::tr( "ABOUT_SPLASH" ) );
    if ( !px.isNull() ) {
      // ...set splash pixmap
      splash = QtxSplash::splash( px );
      // ...set splash text colors
      if ( !splashTextColors.isEmpty() ) {
	QStringList colors = QStringList::split( "|", splashTextColors );
	QColor c1, c2;
	if ( colors.count() > 0 ) c1 = QColor( colors[0] );
	if ( colors.count() > 1 ) c2 = QColor( colors[1] );
	splash->setTextColors( c1, c2 );
      }
      else {
	splash->setTextColors( Qt::white, Qt::black );
      }
      // ...set splash progress colors
      if ( !splashProgressColors.isEmpty() ) {
	QStringList colors = QStringList::split( "|", splashProgressColors );
	QColor c1, c2;
	int gradType = QtxSplash::Vertical;
	if ( colors.count() > 0 ) c1 = QColor( colors[0] );
	if ( colors.count() > 1 ) c2 = QColor( colors[1] );
	if ( colors.count() > 2 ) gradType = colors[2].toInt();
	splash->setProgressColors( c1, c2, gradType );
      }
      // ...set splash text font
      QFont f = splash->font();
      f.setBold( true );
      splash->setFont( f );
      // ...show splash initial status
      if ( !splashInfo.isEmpty() ) {
	splashInfo.replace( QRegExp( "%A" ),  QObject::tr( "APP_NAME" ) );
	splashInfo.replace( QRegExp( "%V" ),  QObject::tr( "ABOUT_VERSION" ).arg( salomeVersion() ) );
	splashInfo.replace( QRegExp( "%L" ),  QObject::tr( "ABOUT_LICENSE" ) );
	splashInfo.replace( QRegExp( "%C" ),  QObject::tr( "ABOUT_COPYRIGHT" ) );
	splashInfo.replace( QRegExp( "\\\\n" ), "\n" );
	splash->message( splashInfo );
      }
      // ...set 'hide on click' flag
#ifdef _DEBUG_
      splash->setHideOnClick( true );
#endif
      // ...show splash
      splash->show();
      qApp->processEvents();
    }
  }

  // Initialization
  int result = -1;

  CORBA::ORB_var orb;
  PortableServer::POA_var poa;

  SUIT_Session* aGUISession = 0;
  SALOME_NamingService* _NS = 0;
  GetInterfaceThread* guiThread = 0;
  Session_ServerLauncher* myServerLauncher = 0;

  try {
    // ...initialize Python (only once)
    int   _argc   = 1;
    char* _argv[] = {""};
    KERNEL_PYTHON::init_python( _argc,_argv );
    PyEval_RestoreThread( KERNEL_PYTHON::_gtstate );
    if ( !KERNEL_PYTHON::salome_shared_modules_module ) // import only once
      KERNEL_PYTHON::salome_shared_modules_module = PyImport_ImportModule( "salome_shared_modules" );
    if ( !KERNEL_PYTHON::salome_shared_modules_module ) {
      INFOS( "salome_shared_modules_module == NULL" );
      PyErr_Print();
    }
    PyEval_ReleaseThread( KERNEL_PYTHON::_gtstate );

    // ...create ORB, get RootPOA object, NamingService, etc.
    ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance();
    ASSERT( SINGLETON_<ORB_INIT>::IsAlreadyExisting() );
    int orbArgc = 1;
    orb = init( orbArgc, argv );

    // ...install SALOME thread event handler
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

  QMutex _GUIMutex, _SessionMutex, _SplashMutex;
  QWaitCondition _ServerLaunch, _SessionStarted, _SplashStarted;

  // lock session mutex to ensure that GetInterface is not called
  // until all initialization is done
  _SessionMutex.lock();

  if ( !result ) {
    // Start embedded servers launcher (Registry, SALOMEDS, etc.)
    // ...lock mutex to block embedded servers launching thread until wait( mutex )
    _GUIMutex.lock();  
    // ...create launcher
    myServerLauncher = new Session_ServerLauncher( argc, argv, orb, poa, &_GUIMutex, &_ServerLaunch, &_SessionMutex, &_SessionStarted );
    // ...block this thread until launcher is ready
    _ServerLaunch.wait( &_GUIMutex );
    
    // Start servers check thread (splash)
    if ( splash ) {
      // ...lock mutex to block splash thread until wait( mutex )
      _SplashMutex.lock();
      // ...create servers checking thread
      Session_ServerCheck sc( &_SplashMutex, &_SplashStarted );
      // ...block this thread until servers checking is finished
      _SplashStarted.wait( &_SplashMutex );
      // ...unlock mutex 'cause it is no more needed
      _SplashMutex.unlock();
      // get servers checking thread status
      result = splash->error();
      QString info = splash->message().isEmpty() ? "%1" : QString( "%1\n%2" ).arg( splash->message() );
      splash->setStatus( info.arg( "Activating desktop..." ) );
    }

    // Finalize embedded servers launcher 
    // ...block this thread until launcher is finished
    _ServerLaunch.wait( &_GUIMutex );
    // ...unlock mutex 'cause it is no more needed
    _GUIMutex.unlock();
  }

  if ( !result ) {
    // Launch GUI activator
    if ( isGUI ) {
      // ...retrieve Session interface reference
      CORBA::Object_var obj = _NS->Resolve( "/Kernel/Session" );
      SALOME::Session_var session = SALOME::Session::_narrow( obj ) ;
      ASSERT ( ! CORBA::is_nil( session ) );
      // ...create GUI launcher
      INFOS( "Session activated, Launch IAPP..." );
      guiThread = new GetInterfaceThread( session );
    }

    // GUI activation
    // Allow multiple activation/deactivation of GUI
    while ( true ) {
      MESSAGE( "waiting wakeAll()" );
      _SessionStarted.wait( &_SessionMutex ); // to be reseased by Launch server thread when ready:
      // atomic operation lock - unlock on mutex
      // unlock mutex: serverThread runs, calls _ServerLaunch->wakeAll()
      // this thread wakes up, and lock mutex

      _SessionMutex.unlock();

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

	if ( splash )
	  splash->finish( aGUIApp->desktop() );
	  
	result = _qappl.exec();
	
	if ( splash )
	  delete splash;
	splash = 0;

	if ( result == SUIT_Session::FROM_GUI ) // desktop is closed by user from GUI
	  break;
      }

      delete aGUISession;
      aGUISession = 0;

      // Prepare _GUIMutex for a new GUI activation
      _SessionMutex.lock();
    }
  }

  // unlock Session mutex
  _SessionMutex.unlock();
  
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
