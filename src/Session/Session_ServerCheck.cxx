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
// File:      Session_ServerCheck.cxx
// Author:    Vadim SANDLER

#include "Session_ServerCheck.hxx"
#include <QtxSplash.h>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Session)
#include CORBA_CLIENT_HEADER(SALOME_Registry)
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOME_ModuleCatalog)
#include CORBA_CLIENT_HEADER(SALOME_Component)

#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "SALOME_NamingService.hxx"
#include "utilities.h"
#include "OpUtil.hxx"

// Default settings
const int __DEFAULT__ATTEMPTS__ = 300;      // number of checks attemtps
                                            // can be overrided by CSF_RepeatServerRequest
                                            // environment variable
const int __DEFAULT__DELAY__    = 100000;   // delay between attempts (microseconds)
                                            // can be overrided by CSF_DelayServerRequest
                                            // environment variable

/*!
  Constructor
*/
Session_ServerCheck::Session_ServerCheck( QMutex* mutex, QWaitCondition* wc )
  : QThread(),
    myMutex( mutex ),
    myWC( wc ),
    myCheckCppContainer( false ),
    myCheckPyContainer( false ),
    myCheckSVContainer( false ),
    myAttempts( __DEFAULT__ATTEMPTS__ ),
    myDelay   ( __DEFAULT__DELAY__ )
{
  char* cenv;
  // try to get nb of attempts from environment variable
  if ( ( cenv = getenv( "CSF_RepeatServerRequest" ) ) && atoi( cenv ) > 0 )
    myAttempts = atoi( cenv );
  // try to get delay between attempts from environment variable
  if ( ( cenv = getenv( "CSF_DelayServerRequest" ) ) && atoi( cenv ) > 0 )
    myDelay = atoi( cenv );

  // check if it is necessary to wait containers
  for ( int i = 1; i < qApp->argc(); i++ ) {
    if ( !strcmp( qApp->argv()[i], "CPP" ) )
      myCheckCppContainer = true;
    if ( !strcmp( qApp->argv()[i], "PY" ) )
      myCheckPyContainer = true;
    if ( !strcmp( qApp->argv()[i], "SUPERV" ) )
      myCheckSVContainer = true;
  }
  
  // start thread
  start();
}

/*!
  Destructor
*/
Session_ServerCheck::~Session_ServerCheck()
{
}

/*!
  Thread loop. Checnk SALOME servers and shows status message
  in the splash screen.
*/
void Session_ServerCheck::run()
{
  // automatic locker
  class Locker
  {
  public:
    QMutex*         _m;
    QWaitCondition* _wc;
    Locker( QMutex* m, QWaitCondition* wc ) : _m( m ), _wc( wc )
    {
      _m->lock();
      _m->unlock(); 
    }
    ~Locker()
    {
      _wc->wakeAll();
    }
  };

  // lock mutex (ensure splash is shown)
  Locker locker( myMutex, myWC );

  // set initial splash status
  QtxSplash* splash = QtxSplash::splash();

  int cnt = 5;                       // base servers
  if ( myCheckCppContainer ) cnt++;  // + C++ container
  if ( myCheckPyContainer )  cnt++;  // + Python container
  if ( myCheckSVContainer )  cnt++;  // + supervision container

  splash->setProgress( 0, cnt * myAttempts );
  QString initialInfo = splash->message();
  QString info = initialInfo.isEmpty() ? "%1" : QString( "%1\n%2" ).arg( initialInfo );

  // start check servers
  int i;
  int current = 0;
  bool bOk;
  QString error;
  int    argc = qApp->argc();
  char** argv = qApp->argv();

  // 1. Check naming service
  bOk = false;
  for ( i = 0; i < myAttempts ; i++ ) {
    QtxSplash::setStatus( info.arg( "Waiting for naming service..." ), current * myAttempts + i );
    QThread::usleep( i == 0 ? 500000 : myDelay );
    try {
      CORBA::ORB_var orb = CORBA::ORB_init( argc, argv );
      CORBA::Object_var obj = orb->resolve_initial_references( "NameService" );
      CosNaming::NamingContext_var _root_context = CosNaming::NamingContext::_narrow( obj );
      if ( !CORBA::is_nil( _root_context ) ) {
	bOk = true;
	break;
      }
    }
    catch( CORBA::COMM_FAILURE& ) {
      MESSAGE( "CORBA::COMM_FAILURE: unable to contact the naming service" );
    }
    catch( ... ) {
      MESSAGE( "Unknown Exception: unable to contact the naming service" );
    }
  }
  if ( !bOk ) {
    QtxSplash::error( "Unable to contact the naming service.\n%1" );
    return;
  }
  QtxSplash::setStatus( info.arg( "Waiting for naming service...OK" ), ++current * myAttempts );
  QThread::usleep( 300000 );
  
  // 2. Check registry server
  bOk = false;
  for ( i = 0; i < myAttempts ; i++ ) {
    QtxSplash::setStatus( info.arg( "Waiting for registry server..." ), current * myAttempts + i );
    QThread::usleep( i == 0 ? 500000 : myDelay );
    try {
      CORBA::ORB_var orb = CORBA::ORB_init( argc, argv );
      SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
      ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
      NS.init_orb( orb );
      CORBA::Object_var obj = NS.Resolve( "/Registry" );
      Registry::Components_var registry = Registry::Components::_narrow( obj );
      if ( !CORBA::is_nil( registry ) ) {
	MESSAGE( "/Registry is found" );
	registry->ping();
	MESSAGE( "Registry was activated" );
	bOk = true;
	break;
      }
    }
    catch ( ServiceUnreachable& ) {
      MESSAGE( "Caught exception: Naming Service unreachable." );
      error = "Naming service unreachable";
    }
    catch ( CORBA::COMM_FAILURE& ) {
      MESSAGE( "Caught CORBA::SystemException CommFailure." );
      error = "Caught CORBA::SystemException CommFailure.";
    }
    catch ( CORBA::SystemException& ) {
      MESSAGE( "Caught CORBA::SystemException." );
      error = "Caught CORBA::SystemException.";
    }
    catch ( CORBA::Exception& ) {
      MESSAGE( "Caught CORBA::Exception." );
      error = "Caught CORBA::Exception.";
    }
    catch (...) {
      MESSAGE( "Caught unknown exception." );
      error = "Caught unknown exception.";
    }
  }
  if ( !bOk ) {
    QtxSplash::error( QString( "Registry server is not found.\n%1" ).arg ( error ) );
    return;
  }
  QtxSplash::setStatus( info.arg( "Waiting for registry server...OK" ), ++current * myAttempts );
  QThread::usleep( 300000 );

  // 3. Check data server
  bOk = false;
  for ( i = 0; i < myAttempts ; i++ ) {
    QtxSplash::setStatus( info.arg( "Waiting for study server..." ), current * myAttempts + i );
    QThread::usleep( i == 0 ? 500000 : myDelay );
    try {
      CORBA::ORB_var orb = CORBA::ORB_init( argc, argv );
      SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
      ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
      NS.init_orb( orb );
      CORBA::Object_var obj = NS.Resolve( "/myStudyManager" );
      SALOMEDS::StudyManager_var studyManager = SALOMEDS::StudyManager::_narrow( obj );
      if ( !CORBA::is_nil( studyManager ) ) {
	MESSAGE( "/myStudyManager is found" );
	studyManager->ping();
	MESSAGE( "StudyManager was activated" );
	bOk = true;
	break;
      }
    }
    catch ( ServiceUnreachable& ) {
      MESSAGE( "Caught exception: Naming Service unreachable." );
      error = "Naming service unreachable";
    }
    catch ( CORBA::COMM_FAILURE& ) {
      MESSAGE( "Caught CORBA::SystemException CommFailure." );
      error = "Caught CORBA::SystemException CommFailure.";
    }
    catch ( CORBA::SystemException& ) {
      MESSAGE( "Caught CORBA::SystemException." );
      error = "Caught CORBA::SystemException.";
    }
    catch ( CORBA::Exception& ) {
      MESSAGE( "Caught CORBA::Exception." );
      error = "Caught CORBA::Exception.";
    }
    catch (...) {
      MESSAGE( "Caught unknown exception." );
      error = "Caught unknown exception.";
    }
  }
  if ( !bOk ) {
    QtxSplash::error( QString( "Study server is not found.\n%1" ).arg ( error ) );
    return;
  }
  QtxSplash::setStatus( info.arg( "Waiting for study server...OK" ), ++current * myAttempts );
  QThread::usleep( 300000 );

  // 4. Check module catalogue server
  bOk = false;
  for ( i = 0; i < myAttempts ; i++ ) {
    QtxSplash::setStatus( info.arg( "Waiting for module catalogue server..." ), current * myAttempts + i );
    QThread::usleep( i == 0 ? 500000 : myDelay );
    try {
      CORBA::ORB_var orb = CORBA::ORB_init( argc, argv );
      SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
      ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
      NS.init_orb( orb );
      CORBA::Object_var obj = NS.Resolve( "/Kernel/ModulCatalog" );
      SALOME_ModuleCatalog::ModuleCatalog_var catalog = SALOME_ModuleCatalog::ModuleCatalog::_narrow( obj );
      if ( !CORBA::is_nil( catalog ) ){
	MESSAGE( "/Kernel/ModulCatalog is found" );
	catalog->ping();
	MESSAGE( "ModuleCatalog was activated" );
	bOk = true;
	break;
      }
    }
    catch ( ServiceUnreachable& ) {
      MESSAGE( "Caught exception: Naming Service unreachable." );
      error = "Naming service unreachable";
    }
    catch ( CORBA::COMM_FAILURE& ) {
      MESSAGE( "Caught CORBA::SystemException CommFailure." );
      error = "Caught CORBA::SystemException CommFailure.";
    }
    catch ( CORBA::SystemException& ) {
      MESSAGE( "Caught CORBA::SystemException." );
      error = "Caught CORBA::SystemException.";
    }
    catch ( CORBA::Exception& ) {
      MESSAGE( "Caught CORBA::Exception." );
      error = "Caught CORBA::Exception.";
    }
    catch (...) {
      MESSAGE( "Caught unknown exception." );
      error = "Caught unknown exception.";
    }
  }
  if ( !bOk ) {
    QtxSplash::error( QString( "Module catalogue server is not found.\n%1" ).arg ( error ) );
    return;
  }
  QtxSplash::setStatus( info.arg( "Waiting for module catalogue server...OK" ), ++current * myAttempts );
  QThread::usleep( 300000 );

  // 5. Check data server
  bOk = false;
  for ( i = 0; i < myAttempts ; i++ ) {
    QtxSplash::setStatus( info.arg( "Waiting for session server..." ), current * myAttempts + i );
    QThread::usleep( i == 0 ? 500000 : myDelay );
    try {
      CORBA::ORB_var orb = CORBA::ORB_init( argc, argv );
      SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
      ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
      NS.init_orb( orb );
      CORBA::Object_var obj = NS.Resolve( "/Kernel/Session" );
      SALOME::Session_var session = SALOME::Session::_narrow( obj );
      if ( !CORBA::is_nil( session ) ) {
	MESSAGE( "/Kernel/Session is found" );
	session->ping();
	MESSAGE( "SALOME_Session was activated" );
	bOk = true;
	break;
      }
    }
    catch ( ServiceUnreachable& ) {
      MESSAGE( "Caught exception: Naming Service unreachable." );
      error = "Naming service unreachable";
    }
    catch ( CORBA::COMM_FAILURE& ) {
      MESSAGE( "Caught CORBA::SystemException CommFailure." );
      error = "Caught CORBA::SystemException CommFailure.";
    }
    catch ( CORBA::SystemException& ) {
      MESSAGE( "Caught CORBA::SystemException." );
      error = "Caught CORBA::SystemException.";
    }
    catch ( CORBA::Exception& ) {
      MESSAGE( "Caught CORBA::Exception." );
      error = "Caught CORBA::Exception.";
    }
    catch (...) {
      MESSAGE( "Caught unknown exception." );
      error = "Caught unknown exception.";
    }
  }
  if ( !bOk ) {
    QtxSplash::error( QString( "Session server is not found.\n%1" ).arg ( error ) );
    return;
  }
  QtxSplash::setStatus( info.arg( "Waiting for session server...OK" ), ++current * myAttempts );
  QThread::usleep( 300000 );

  // 6. Check C++ container
  if ( myCheckCppContainer ) {
    bOk = false;
    for ( i = 0; i < myAttempts ; i++ ) {
      QtxSplash::setStatus( info.arg( "Waiting for C++ container..." ), current * myAttempts + i );
      QThread::usleep( i == 0 ? 500000 : myDelay );
      try {
	CORBA::ORB_var orb = CORBA::ORB_init( argc, argv );
	SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
	ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
	NS.init_orb( orb );
	QString containerName = QString( "/Containers/%1/FactoryServer" ).arg( GetHostname().c_str() );
	CORBA::Object_var obj = NS.Resolve( containerName.latin1() );
	Engines::Container_var FScontainer = Engines::Container::_narrow( obj );
	if ( !CORBA::is_nil( FScontainer ) ) {
	  MESSAGE( containerName.latin1() << " is found" );
	  FScontainer->ping();
	  MESSAGE( "FactoryServer container was activated" );
	  bOk = true;
	  break;
	}
      }
      catch ( ServiceUnreachable& ) {
	MESSAGE( "Caught exception: Naming Service unreachable." );
	error = "Naming service unreachable";
      }
      catch ( CORBA::COMM_FAILURE& ) {
	MESSAGE( "Caught CORBA::SystemException CommFailure." );
	error = "Caught CORBA::SystemException CommFailure.";
      }
      catch ( CORBA::SystemException& ) {
	MESSAGE( "Caught CORBA::SystemException." );
	error = "Caught CORBA::SystemException.";
      }
      catch ( CORBA::Exception& ) {
	MESSAGE( "Caught CORBA::Exception." );
	error = "Caught CORBA::Exception.";
      }
      catch (...) {
	MESSAGE( "Caught unknown exception." );
	error = "Caught unknown exception.";
      }
    }
    if ( !bOk ) {
      QtxSplash::error( QString( "C++ container is not found.\n%1" ).arg ( error ) );
      return;
    }
    QtxSplash::setStatus( info.arg( "Waiting for C++ container...OK" ), ++current * myAttempts );
    QThread::usleep( 300000 );
  }

  // 7. Check Python container
  if ( myCheckPyContainer ) {
    bOk = false;
    for ( i = 0; i < myAttempts ; i++ ) {
      QtxSplash::setStatus( info.arg( "Waiting for Python container..." ), current * myAttempts + i );
      QThread::usleep( i == 0 ? 500000 : myDelay );
      try {
	CORBA::ORB_var orb = CORBA::ORB_init( argc, argv );
	SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
	ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
	NS.init_orb( orb );
	QString containerName = QString( "/Containers/%1/FactoryServerPy" ).arg( GetHostname().c_str() );
	CORBA::Object_var obj = NS.Resolve( containerName.latin1() );
	Engines::Container_var FSPcontainer = Engines::Container::_narrow( obj );
	if ( !CORBA::is_nil( FSPcontainer ) ) {
	  MESSAGE( containerName.latin1() << " is found" );
	  FSPcontainer->ping();
	  MESSAGE("FactoryServerPy container was activated");
	  bOk = true;
	  break;
	}
      }
      catch ( ServiceUnreachable& ) {
	MESSAGE( "Caught exception: Naming Service unreachable." );
	error = "Naming service unreachable";
      }
      catch ( CORBA::COMM_FAILURE& ) {
	MESSAGE( "Caught CORBA::SystemException CommFailure." );
	error = "Caught CORBA::SystemException CommFailure.";
      }
      catch ( CORBA::SystemException& ) {
	MESSAGE( "Caught CORBA::SystemException." );
	error = "Caught CORBA::SystemException.";
      }
      catch ( CORBA::Exception& ) {
	MESSAGE( "Caught CORBA::Exception." );
	error = "Caught CORBA::Exception.";
      }
      catch (...) {
	MESSAGE( "Caught unknown exception." );
	error = "Caught unknown exception.";
      }
    }
    if ( !bOk ) {
      QtxSplash::error( QString( "Python container is not found.\n%1" ).arg ( error ) );
      return;
    }
    QtxSplash::setStatus( info.arg( "Waiting for Python container...OK" ), ++current * myAttempts );
    QThread::usleep( 300000 );
  }

  // 8. Check supervision container
  if ( myCheckSVContainer ) {
    bOk = false;
    for ( i = 0; i < myAttempts ; i++ ) {
      QtxSplash::setStatus( info.arg( "Waiting for Supervision container..." ), current * myAttempts + i );
      QThread::usleep( i == 0 ? 500000 : myDelay );
      try {
	CORBA::ORB_var orb = CORBA::ORB_init( argc, argv );
	SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
	ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
	NS.init_orb( orb );
	QString containerName = QString( "/Containers/%1/SuperVisionContainer" ).arg( GetHostname().c_str() );
	CORBA::Object_var obj = NS.Resolve( containerName.latin1() );
	Engines::Container_var SVcontainer = Engines::Container::_narrow( obj );
	if ( !CORBA::is_nil( SVcontainer ) ) {
	  MESSAGE( containerName.latin1() << " is found" );
	  SVcontainer->ping();
	  MESSAGE("SuperVisionContainer container was activated");
	  bOk = true;
	  break;
	}
      }
      catch ( ServiceUnreachable& ) {
	MESSAGE( "Caught exception: Naming Service unreachable." );
	error = "Naming service unreachable";
      }
      catch ( CORBA::COMM_FAILURE& ) {
	MESSAGE( "Caught CORBA::SystemException CommFailure." );
	error = "Caught CORBA::SystemException CommFailure.";
      }
      catch ( CORBA::SystemException& ) {
	MESSAGE( "Caught CORBA::SystemException." );
	error = "Caught CORBA::SystemException.";
      }
      catch ( CORBA::Exception& ) {
	MESSAGE( "Caught CORBA::Exception." );
	error = "Caught CORBA::Exception.";
      }
      catch (...) {
	MESSAGE( "Caught unknown exception." );
	error = "Caught unknown exception.";
      }
    }
    if ( !bOk ) {
      QtxSplash::error( QString( "Supervision container is not found.\n%1" ).arg ( error ) );
      return;
    }
    QtxSplash::setStatus( info.arg( "Waiting for Supervision container...OK" ), ++current * myAttempts );
    QThread::usleep( 300000 );
  }
  // clear splash status
  splash->setProgress( 0, 0 );
  splash->setStatus( initialInfo );
}
