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

// File   : Session_ServerCheck.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "Session_ServerCheck.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Session)
#include CORBA_CLIENT_HEADER(SALOME_Registry)
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOME_ModuleCatalog)
#include CORBA_CLIENT_HEADER(SALOME_Component)

#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "SALOME_NamingService.hxx"
#include "Basics_Utils.hxx"
#include "utilities.h"
#include "Qtx.h"

#include <QApplication> 
#include <QWaitCondition>
#include <QMutexLocker>
#include <QStringList>

//
// Default settings
//

/*!
  \brief Default number of attemtps to check SALOME server.

  This value can be changed by setting the CSF_RepeatServerRequest
  environment variable. For example, to set number of check attempts
  for each server to 1000:
  \code
  setenv CSF_RepeatServerRequest 1000
  \endcode
*/
const int __DEFAULT__ATTEMPTS__ = 300;

/*!
  \brief Default delay between attempts (in microseconds).

  This value can be changed by setting the CSF_DelayServerRequest
  environment variable. For example, to set delay between attemtps
  to check SALOME servers to 100000 (0.1 second):
  \code
  setenv CSF_DelayServerRequest 100000
  \endcode
*/
const int __DEFAULT__DELAY__ = 50000;

// The exception being thrown out of a destructor,
// that is not allowed by default in C++11.

#if __cplusplus >= 201103L
# define TYPE_NOEXCEPT noexcept(false)
#else
# define TYPE_NOEXCEPT
#endif

/*!
  \classSession_ServerCheck::Locker
  \brief Automatic locker/unlocker.
  \internal
*/

class Session_ServerCheck::Locker
{
public:
  /*!
    \brief Constructor. Tries to aquire lock.
  */
  Locker( Session_ServerCheck* sc ) 
    : myChecker( sc )
  {
    myChecker->myMutex->lock();
    myChecker->myMutex->unlock();
  }
  /*!
    \brief Destructor. Wakes the calling thread and goes sleeping.
  */
  ~Locker() TYPE_NOEXCEPT
  {
    myChecker->myWC->wakeAll();
    myChecker->usleep( myChecker->myDelay );
  }
private:
  Session_ServerCheck* myChecker;
};

/*!
  \class Session_ServerCheck
  \brief The class Session_ServerCheck is used to check SALOME
  servers availability.
  
  It runs in the secondrary thread. The number of attemts to check
  each SALOME server and the time delay between checks can be specified
  via setting the CSF_RepeatServerRequest and CSF_DelayServerRequest
  environment variables.

  Total number of the check attempts can be retrieved via totalSteps()
  method and current check step can be retrieved via currentStep() method.

  The method currentMessage() can be used to get the information message
  about what SALOME server is currently checked. If any error occured (some
  server could not be found) the thread loop is stopped and error status
  is set. Error message can be retrieved with the error() method.
*/

/*!
  \brief Constructor.
  \param mutex a mutex used to serialize progress operations (splash)
  \param wc a wait condition used in combination with \a mutex
*/
Session_ServerCheck::Session_ServerCheck( QMutex* mutex, QWaitCondition* wc )
: QThread(),
  myMutex( mutex ),
  myWC( wc ),
  myCheckCppContainer( false ),
  myCheckPyContainer( false ),
  myCheckSVContainer( false ),
  myAttempts( __DEFAULT__ATTEMPTS__ ),
  myDelay   ( __DEFAULT__DELAY__ ),
  myCurrentStep( 0 )
{
  char* cenv;
  // try to get nb of attempts from environment variable
  if ( ( cenv = getenv( "CSF_RepeatServerRequest" ) ) && atoi( cenv ) > 0 )
    myAttempts = atoi( cenv );
  // try to get delay between attempts from environment variable
  if ( ( cenv = getenv( "CSF_DelayServerRequest" ) ) && atoi( cenv ) > 0 )
    myDelay = atoi( cenv );

  // parse command line check if it is necessary to wait SALOME containers
  QStringList args = QApplication::arguments();
  for ( int i = 1; i < args.count(); i++ ) {
    myCheckCppContainer = myCheckCppContainer || args[i] == "CPP";
    myCheckPyContainer  = myCheckPyContainer  || args[i] == "PY";
    myCheckSVContainer  = myCheckSVContainer  || args[i] == "SUPERV";
  }
  
  // start thread
  start();
}

/*!
  \brief Destructor
*/
Session_ServerCheck::~Session_ServerCheck()
{
  terminate();
  while( isRunning() );
}

/*!
  \brief Get current information message.
  \return current message
*/
QString Session_ServerCheck::currentMessage()
{
  static QStringList messages;
  if ( messages.isEmpty() ) {
    messages << tr( "Waiting for naming service..." ); 
    messages << tr( "Waiting for registry server..." );
    messages << tr( "Waiting for study server..." );
    messages << tr( "Waiting for module catalogue server..." );
    messages << tr( "Waiting for session server..." );
    messages << tr( "Waiting for C++ container..." );
    messages << tr( "Waiting for Python container..." );
    messages << tr( "Waiting for Supervision container..." );
  }
  QMutexLocker locker( &myDataMutex );
  QString msg;
  int idx = myCurrentStep / myAttempts;
  if ( idx >= 0 && idx < messages.count() )
    msg = messages[ idx ];
  return msg;
}

/*!
  \brief Get error message.
  \return error message or null string of there was no any error
*/
QString Session_ServerCheck::error()
{
  QMutexLocker locker( &myDataMutex );
  return myError;
}

/*!
  \brief Get current step.
  \return current step
*/
int Session_ServerCheck::currentStep()
{
  QMutexLocker locker( &myDataMutex );
  return myCurrentStep;
}

/*!
  \brief Get total number of check steps.
  \return total number of steps
*/
int Session_ServerCheck::totalSteps()
{
  QMutexLocker locker( &myDataMutex );
  int cnt = 5;                       // base servers
  if ( myCheckCppContainer ) cnt++;  // + C++ container
  if ( myCheckPyContainer )  cnt++;  // + Python container
  if ( myCheckSVContainer )  cnt++;  // + supervision container
  return cnt * myAttempts;
}

/*!
  \brief Modify current step.
  \param step new current step value
*/
void Session_ServerCheck::setStep( const int step )
{
  QMutexLocker locker( &myDataMutex );
  myCurrentStep = step;
}

/*!
  \brief Set error message.
  \param msg error message
*/
void Session_ServerCheck::setError( const QString& msg )
{
  QMutexLocker locker( &myDataMutex );
  myError = msg;
}

/*!
  \brief Thread loop function. Performs SALOME servers check.
*/
void Session_ServerCheck::run()
{
  // start check servers
  int current = 0;
  QString error;
  Qtx::CmdLineArgs args;
  
  // 1. Check naming service
  for ( int i = 0; i < myAttempts; i++ ) {
    Locker locker( this );

    setStep( current * myAttempts + i );

    try {
      ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
      CORBA::ORB_var orb = init( args.argc(), args.argv() );
      CORBA::Object_var obj = orb->resolve_initial_references( "NameService" );
      CosNaming::NamingContext_var _root_context = CosNaming::NamingContext::_narrow( obj );
      if ( !CORBA::is_nil( _root_context ) ) {
        setStep( ++current * myAttempts );
        break;
      }
    }
    catch( CORBA::COMM_FAILURE& ) {
      MESSAGE( "CORBA::COMM_FAILURE: unable to contact the naming service" );
    }
    catch( ... ) {
      MESSAGE( "Unknown Exception: unable to contact the naming service" );
    }

    if ( i == myAttempts-1 ) {
      setError( tr( "Unable to contact the naming service.\n" ) );
      return;
    }
  }

  // 2. Check registry server
  for ( int i = 0; i < myAttempts ; i++ ) {
    Locker locker( this );

    setStep( current * myAttempts + i );

    try {
      ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
      CORBA::ORB_var orb = init( args.argc(), args.argv() );
      SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
      ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
      NS.init_orb( orb );
      CORBA::Object_var obj = NS.Resolve( "/Registry" );
      Registry::Components_var registry = Registry::Components::_narrow( obj );
      if ( !CORBA::is_nil( registry ) ) {
        MESSAGE( "/Registry is found" );
        registry->ping();
        MESSAGE( "Registry was activated" );
        setStep( ++current * myAttempts );
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

    if ( i == myAttempts-1 ) {
      setError( tr( "Registry server is not found.\n%1" ).arg ( error ) );
      return;
    }
  }

  // 3. Check data server
  for ( int i = 0; i < myAttempts ; i++ ) {
    Locker locker( this );

    setStep( current * myAttempts + i );

    try {
      ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
      CORBA::ORB_var orb = init( args.argc(), args.argv() );
      SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
      ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
      NS.init_orb( orb );
      CORBA::Object_var obj = NS.Resolve( "/myStudyManager" );
      SALOMEDS::StudyManager_var studyManager = SALOMEDS::StudyManager::_narrow( obj );
      if ( !CORBA::is_nil( studyManager ) ) {
        MESSAGE( "/myStudyManager is found" );
        studyManager->ping();
        MESSAGE( "StudyManager was activated" );
        setStep( ++current * myAttempts );
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

    if ( i == myAttempts-1 ) {
      setError( tr( "Study server is not found.\n%1" ).arg ( error ) );
      return;
    }
  }
  
  // 4. Check module catalogue server
  for ( int i = 0; i < myAttempts ; i++ ) {
    Locker locker( this );

    setStep( current * myAttempts + i );

    try {
      ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
      CORBA::ORB_var orb = init( args.argc(), args.argv() );
      SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
      ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
      NS.init_orb( orb );
      CORBA::Object_var obj = NS.Resolve( "/Kernel/ModulCatalog" );
      SALOME_ModuleCatalog::ModuleCatalog_var catalog = SALOME_ModuleCatalog::ModuleCatalog::_narrow( obj );
      if ( !CORBA::is_nil( catalog ) ){
        MESSAGE( "/Kernel/ModulCatalog is found" );
        catalog->ping();
        MESSAGE( "ModuleCatalog was activated" );
        setStep( ++current * myAttempts );
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

    if ( i == myAttempts-1 ) {
      setError( tr( "Module catalogue server is not found.\n%1" ).arg ( error ) );
      return;
    }
  }

  // 5. Check data server
  for ( int i = 0; i < myAttempts ; i++ ) {
    Locker locker( this );

    setStep( current * myAttempts + i );

    try {
      ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
      CORBA::ORB_var orb = init( args.argc(), args.argv() );
      SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
      ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
      NS.init_orb( orb );
      CORBA::Object_var obj = NS.Resolve( "/Kernel/Session" );
      SALOME::Session_var session = SALOME::Session::_narrow( obj );
      if ( !CORBA::is_nil( session ) ) {
        MESSAGE( "/Kernel/Session is found" );
        session->ping();
        MESSAGE( "SALOME_Session was activated" );
        setStep( ++current * myAttempts );
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

    if ( i == myAttempts-1 ) {
      setError( tr( "Session server is not found.\n%1" ).arg ( error ) );
      return;
    }
  }

  // 6. Check C++ container
  if ( myCheckCppContainer ) {
    for ( int i = 0; i < myAttempts ; i++ ) {
      Locker locker( this );
      
      setStep( current * myAttempts + i );

      try {
        ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
        CORBA::ORB_var orb = init( args.argc(), args.argv() );
        SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
        ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
        NS.init_orb( orb );
        QString containerName = QString( "/Containers/%1/FactoryServer" ).arg( Kernel_Utils::GetHostname().c_str() );
        CORBA::Object_var obj = NS.Resolve( containerName.toLatin1() );
        Engines::Container_var FScontainer = Engines::Container::_narrow( obj );
        if ( !CORBA::is_nil( FScontainer ) ) {
          MESSAGE( containerName.toLatin1().constData() << " is found" );
          FScontainer->ping();
          MESSAGE( "FactoryServer container was activated" );
          setStep( ++current * myAttempts );
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
      
      if ( i == myAttempts-1 ) {
        setError( tr( "C++ container is not found.\n%1" ).arg ( error ) );
        return;
      }
    }
  }

  // 7. Check Python container
  if ( myCheckPyContainer ) {
    for ( int i = 0; i < myAttempts ; i++ ) {
      Locker locker( this );
      
      setStep( current * myAttempts + i );

      try {
        ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
        CORBA::ORB_var orb = init( args.argc(), args.argv() );
        SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
        ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
        NS.init_orb( orb );
        QString containerName = QString( "/Containers/%1/FactoryServerPy" ).arg( Kernel_Utils::GetHostname().c_str() );
        CORBA::Object_var obj = NS.Resolve( containerName.toLatin1() );
        Engines::Container_var FSPcontainer = Engines::Container::_narrow( obj );
        if ( !CORBA::is_nil( FSPcontainer ) ) {
          MESSAGE( containerName.toLatin1().constData() << " is found" );
          FSPcontainer->ping();
          MESSAGE("FactoryServerPy container was activated");
          setStep( ++current * myAttempts );
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

      if ( i == myAttempts-1 ) {
        setError( tr( "Python container is not found.\n%1" ).arg ( error ) );
        return;
      }
    }
  }

  // 8. Check supervision container
  if ( myCheckSVContainer ) {
    for ( int i = 0; i < myAttempts ; i++ ) {
      Locker locker( this );
      
      setStep( current * myAttempts + i );

      try {
        ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
        CORBA::ORB_var orb = init( args.argc(), args.argv() );
        SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
        ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
        NS.init_orb( orb );
        QString containerName = QString( "/Containers/%1/SuperVisionContainer" ).arg( Kernel_Utils::GetHostname().c_str() );
        CORBA::Object_var obj = NS.Resolve( containerName.toLatin1() );
        Engines::Container_var SVcontainer = Engines::Container::_narrow( obj );
        if ( !CORBA::is_nil( SVcontainer ) ) {
          MESSAGE( containerName.toLatin1().constData() << " is found" );
          SVcontainer->ping();
          MESSAGE("SuperVisionContainer container was activated");
          setStep( ++current * myAttempts );
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
    
      if ( i == myAttempts-1 ) {
        setError( tr( "Supervision container is not found.\n%1" ).arg ( error ) );
        return;
      }
    }
  }
}
