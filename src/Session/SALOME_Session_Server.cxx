//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME Session : implementation of Session.idl
//  File : SALOME_Session_Server.cxx
//  Author : Paul RASCLE, EDF
//  Module : SALOME
//
#include <Container_init_python.hxx>
#include <Utils_ORB_INIT.hxx>
#include <Utils_SINGLETON.hxx>
#include <SALOME_NamingService.hxx>
#include <SALOME_ModuleCatalog_impl.hxx>
#include <OpUtil.hxx>
#include <RegistryService.hxx>
#include <ConnectionManager_i.hxx>
#include <SALOME_LifeCycleCORBA.hxx>

#ifdef ENABLE_TESTRECORDER
  #include <TestApplication.h>
#endif

#include <QDir>
#include <QFile>
#include <QApplication>
#include <QMutex>
#include <QWaitCondition>
#include <QRegExp>
#include <QTextStream>

#include <Utils_SALOME_Exception.hxx>
#include <Utils_CorbaException.hxx>
#include <SALOME_Event.h>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Session)
#include CORBA_SERVER_HEADER(SALOMEDS)

#include <utilities.h>
#include "Session_ServerLauncher.hxx"
#include "Session_ServerCheck.hxx"

#include <QtxSplash.h>
#include <Style_Salome.h>
#include <SUIT_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_Application.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ExceptionHandler.h>

#include <Standard_Version.hxx>

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
  if ( !vf.open( QIODevice::ReadOnly ) )
    return QString();

  QString line( vf.readLine( 1024 ) );

  vf.close();

  if ( line.isEmpty() )
    return QString();

  while ( !line.isEmpty() && line.at( line.length() - 1 ) == QChar( '\n' ) )
    line.remove( line.length() - 1, 1 );

  QString ver;
  int idx = line.lastIndexOf( ":" );
  if ( idx != -1 )
    ver = line.mid( idx + 1 ).trimmed();

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
      resMgr.loadLanguage( "LightApp",  "en" );
      resMgr.loadLanguage( "SalomeApp", "en" );

      myExtAppName = QObject::tr( "APP_NAME" ).trimmed();
      if ( myExtAppName == "APP_NAME" || myExtAppName.toLower() == "salome" ) 
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
    if ( version().isEmpty()  ) return ""; 
    return SUIT_ResourceMgr::userFileName( myExtAppName, for_load );
  }

  virtual int userFileId( const QString& _fname ) const
  {
    if ( !myExtAppName.isEmpty() ) {
      QRegExp exp( QString( "\\.%1rc\\.([a-zA-Z0-9.]+)$" ).arg( myExtAppName ) );
      QRegExp vers_exp( "^([0-9]+)([A-Za-z]?)([0-9]*)$" );
      
      QString fname = QFileInfo( _fname ).fileName();
      if( exp.exactMatch( fname ) ) {
	QStringList vers = exp.cap( 1 ).split( ".", QString::SkipEmptyParts );
	int major=0, minor=0;
	major = vers[0].toInt();
	minor = vers[1].toInt();
	if( vers_exp.indexIn( vers[2] )==-1 )
	  return -1;
	int release = 0, dev1 = 0, dev2 = 0;
	release = vers_exp.cap( 1 ).toInt();
	dev1 = vers_exp.cap( 2 )[ 0 ].toLatin1();
	dev2 = vers_exp.cap( 3 ).toInt();
	
	int dev = dev1*100+dev2, id = major;
	id*=100; id+=minor;
	id*=100; id+=release;
	id*=10000;
	if ( dev > 0 ) id+=dev-10000;
	return id;
      }
    }

    return -1;
  }

public:
  static QString myExtAppName;
  static QString myExtAppVersion;
};

QString SALOME_ResourceMgr::myExtAppName    = QString();
QString SALOME_ResourceMgr::myExtAppVersion = QString();

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

#ifdef ENABLE_TESTRECORDER
  class SALOME_QApplication : public TestApplication
#else
  class SALOME_QApplication : public QApplication
#endif
{
public:
#ifdef ENABLE_TESTRECORDER
  SALOME_QApplication( int& argc, char** argv ) : TestApplication( argc, argv ), myHandler ( 0 ) {}
#else
  SALOME_QApplication( int& argc, char** argv ) : QApplication( argc, argv ), myHandler ( 0 ) {}
#endif

  virtual bool notify( QObject* receiver, QEvent* e )
  {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) < 0x060101
    // Disable GUI user actions while python command is executed
    if (SUIT_Session::IsPythonExecuted()) {
      // Disable mouse and keyboard events
      QEvent::Type aType = e->type();
      if (aType == QEvent::MouseButtonPress || aType == QEvent::MouseButtonRelease ||
          aType == QEvent::MouseButtonDblClick || aType == QEvent::MouseMove ||
          aType == QEvent::Wheel || aType == QEvent::ContextMenu ||
          aType == QEvent::KeyPress || aType == QEvent::KeyRelease ||
          aType == QEvent::Accel || aType == QEvent::AccelOverride)
        return false;
    }
#endif

#ifdef ENABLE_TESTRECORDER
    return myHandler ? myHandler->handle( receiver, e ) :
      TestApplication::notify( receiver, e );
#else
    return myHandler ? myHandler->handle( receiver, e ) :
      QApplication::notify( receiver, e );
#endif
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

void killOmniNames()
{
  SALOME_LifeCycleCORBA::killOmniNames();
}

// shutdown standalone servers
void shutdownServers( SALOME_NamingService* theNS )
{
  SALOME_LifeCycleCORBA lcc(theNS);
  lcc.shutdownServers();
}

// ---------------------------- MAIN -----------------------
int main( int argc, char **argv )
{
  // Install Qt debug messages handler
  qInstallMsgHandler( MessageOutput );
  
  // Create Qt application instance;
  // this should be done the very first!
  SALOME_QApplication _qappl( argc, argv );

  // Add application library path (to search style plugin etc...)
  QString path = QDir::convertSeparators( SUIT_Tools::addSlash( QString( ::getenv( "GUI_ROOT_DIR" ) ) ) + QString( "bin/salome" ) );
  _qappl.addLibraryPath( path );

  bool isGUI    = isFound( "GUI",    argc, argv );
  bool isSplash = isFound( "SPLASH", argc, argv );
  // Show splash screen (only if both the "GUI" and "SPLASH" parameters are set)
  // Note, that user preferences are not taken into account for splash settings -
  // it is a property of the application!
  QtxSplash* splash = 0;
  if ( isGUI && isSplash ) {
    // ...create resource manager
    SUIT_ResourceMgr resMgr( "SalomeApp", QString( "%1Config" ) );
    resMgr.setCurrentFormat( "xml" );
    resMgr.setWorkingMode( QtxResourceMgr::IgnoreUserValues );
    resMgr.loadLanguage( "LightApp", "en" );
    //
    splash = QtxSplash::splash( QPixmap() );
    splash->readSettings( &resMgr );
    if ( splash->pixmap().isNull() )
      splash->setPixmap( resMgr.loadPixmap( "LightApp", QObject::tr( "ABOUT_SPLASH" ) ) );
    if ( splash->pixmap().isNull() ) {
      delete splash;
      splash = 0;
    }
    else {
      splash->setOption( "%A", QObject::tr( "APP_NAME" ) );
      splash->setOption( "%V", QObject::tr( "ABOUT_VERSION" ).arg( salomeVersion() ) );
      splash->setOption( "%L", QObject::tr( "ABOUT_LICENSE" ) );
      splash->setOption( "%C", QObject::tr( "ABOUT_COPYRIGHT" ) );
      splash->show();
      QApplication::instance()->processEvents();
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

    CORBA::Object_var obj = orb->resolve_initial_references( "RootPOA" );
    poa = PortableServer::POA::_narrow( obj );

    PortableServer::POAManager_var pman = poa->the_POAManager();
    pman->activate() ;
    MESSAGE( "pman->activate()" );

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
      // ... set initial progress
      splash->setProgress( 0, sc.totalSteps() );
      // start check loop 
      while ( true ) {
	int step    = sc.currentStep();
	int total   = sc.totalSteps();
	QString msg = sc.currentMessage();
	QString err = sc.error();
	if ( !err.isEmpty() ) {
	  QtxSplash::setError( err );
	  QApplication::instance()->processEvents();
	  result = -1;
	  break;
	}
	QtxSplash::setStatus( msg, step );
	QApplication::instance()->processEvents();
	if ( step >= total )
	  break;
	// ...block this thread until servers checking is finished
	_SplashStarted.wait( &_SplashMutex );
      }
      // ...unlock mutex 'cause it is no more needed
      _SplashMutex.unlock();
    }

    // Finalize embedded servers launcher 
    // ...block this thread until launcher is finished
    _ServerLaunch.wait( &_GUIMutex );
    // ...unlock mutex 'cause it is no more needed
    _GUIMutex.unlock();
  }

  bool shutdown = false;
  if ( !result ) {
    // Launch GUI activator
    if ( isGUI ) {
      if ( splash )
	splash->setStatus( QApplication::translate( "", "Activating desktop..." ) );
      // ...retrieve Session interface reference
      CORBA::Object_var obj = _NS->Resolve( "/Kernel/Session" );
      SALOME::Session_var session = SALOME::Session::_narrow( obj ) ;
      ASSERT ( ! CORBA::is_nil( session ) );
      // ...create GUI launcher
      MESSAGE( "Session activated, Launch IAPP..." );
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
      MESSAGE( "creation SUIT_Application" );
      SUIT_Application* aGUIApp = aGUISession->startApplication( "SalomeApp", 0, 0 );
      if ( aGUIApp )
      {
	Style_Salome::initialize( aGUIApp->resourceMgr() );
	if ( aGUIApp->resourceMgr()->booleanValue( "Style", "use_salome_style", true ) )
	  Style_Salome::apply();

	if ( !isFound( "noexcepthandler", argc, argv ) )
	  _qappl.setHandler( aGUISession->handler() ); // after loading SalomeApp application
	                                               // aGUISession contains SalomeApp_ExceptionHandler
	// Run GUI loop
	MESSAGE( "run(): starting the main event loop" );

	if ( splash )
	  splash->finish( aGUIApp->desktop() );
	  
	result = _qappl.exec();
	
	splash = 0;

	if ( result == SUIT_Session::NORMAL ) { // desktop is closed by user from GUI
	  shutdown = aGUISession->exitFlags();
	  break;
	}
      }

      delete aGUISession;
      aGUISession = 0;

      // Prepare _GUIMutex for a new GUI activation
      _SessionMutex.lock();
    }
  }

  // unlock Session mutex
  _SessionMutex.unlock();
  
  if ( shutdown )
    shutdownServers( _NS );

  if ( myServerLauncher )
    myServerLauncher->KillAll(); // kill embedded servers

  delete aGUISession;
  delete guiThread;
  delete myServerLauncher;
  delete _NS;

  PyGILState_STATE gstate = PyGILState_Ensure();
  Py_Finalize();

  try  {
    orb->shutdown(0);
  }
  catch (...) {
    //////////////////////////////////////////////////////////////
    // VSR: silently skip exception:
    // CORBA.BAD_INV_ORDER.BAD_INV_ORDER_ORBHasShutdown 
    // exception is raised when orb->destroy() is called and
    // cpp continer is launched in the embedded mode
    //////////////////////////////////////////////////////////////
    // std::cerr << "Caught unexpected exception on destroy : ignored !!" << std::endl;
  }

  if ( shutdown )
    killOmniNames();

  return result;
}
