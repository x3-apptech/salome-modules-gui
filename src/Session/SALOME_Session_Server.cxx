//  SALOME Session : implementation of Session.idl
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : SALOME_Session_Server.cxx
//  Author : Paul RASCLE, EDF
//  Module : SALOME
//  $Header$

#include <Container_init_python.hxx>
#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"
#include "SALOME_NamingService.hxx"
#include "SALOMETraceCollector.hxx"

#include <iostream>
#include <unistd.h>

#include <qthread.h> 
#include <qapplication.h>
#include <qlabel.h>
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

#include "SUIT_Session.h"
#include "SUIT_Application.h"
#include "SUIT_MessageBox.h"


#include "SalomeApp_ExceptionHandler.h"
using namespace std;

extern "C" int HandleSignals(QApplication *theQApplication);

/*! - read arguments, define list of server to launch with their arguments.
 *  - wait for naming service
 *  - create and run a thread for launch of all servers
 *  
 */

//! CORBA server for SALOME Session
/*!
 *  SALOME_Session Server launches a SALOME session servant.
 *  The servant registers to the Naming Service.
 *  See SALOME_Session.idl for interface specification.
 *
 *  Main services offered by the servant are:
 *   - launch GUI
 *   - stop Session (must be idle)
 *   - get session state
 */

#ifdef _DEBUG_
static int MYDEBUG = 0;
#else
static int MYDEBUG = 0;
#endif

PyObject *salome_shared_modules_module = NULL;
void MessageOutput( QtMsgType type, const char *msg )
{
  switch ( type ) {
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

class SALOME_Session : public SUIT_Session
{
public:
  SALOME_Session() : SUIT_Session() {}
  virtual ~SALOME_Session() {}

protected:
  virtual SUIT_ResourceMgr* createResourceMgr( const QString& appName ) const
  {
    SUIT_ResourceMgr* resMgr = new SUIT_ResourceMgr( appName, QString( "%1Config" ) );
    resMgr->setCurrentFormat( "xml" );
    return resMgr;
  }
};

class SALOME_QApplication : public QApplication 
{
public:
  SALOME_QApplication( int& argc, char** argv ) : QApplication( argc, argv ) 
  {
    myHandler = 0;
  }
  
  virtual bool notify( QObject* receiver, QEvent* e ) 
  {
    return myHandler ? myHandler->handle( receiver, e ) :
                       QApplication::notify( receiver, e );
  }
  SUIT_ExceptionHandler*  handler() const { return myHandler; }
  void setHandler( SUIT_ExceptionHandler* h ) { myHandler = h; }

private:
  SUIT_ExceptionHandler*  myHandler;
};




int main(int argc, char **argv)
{
  qInstallMsgHandler( MessageOutput );

  /*
   * Python initialisation : only once
   */
  int _argc = 1;
  char* _argv[] = {""};
  KERNEL_PYTHON::init_python(_argc,_argv);
  PyEval_RestoreThread(KERNEL_PYTHON::_gtstate);
  if(!KERNEL_PYTHON::salome_shared_modules_module) // import only once
    {
      KERNEL_PYTHON::salome_shared_modules_module =
	PyImport_ImportModule("salome_shared_modules");
    }
  if(!KERNEL_PYTHON::salome_shared_modules_module)
    {
      INFOS("salome_shared_modules_module == NULL");
      PyErr_Print();
      PyErr_Clear();
    }
  PyEval_ReleaseThread(KERNEL_PYTHON::_gtstate);

  int result = -1;

  // 1. create ORB, get RootPOA object, NamingService, etc.
  ORB_INIT &init = *SINGLETON_<ORB_INIT>::Instance() ;
  ASSERT(SINGLETON_<ORB_INIT>::IsAlreadyExisting()) ;
  int orbArgc = 1;
  CORBA::ORB_var &orb = init( orbArgc , argv ) ;
  SALOMETraceCollector *myThreadTrace = SALOMETraceCollector::instance(orb);

  try
    {
      SALOME_Event::GetSessionThread();

      CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);

      PortableServer::POAManager_var pman = poa->the_POAManager() ;
      pman->activate() ;
      INFOS("pman->activate()");

      SALOME_NamingService *_NS = new SALOME_NamingService(orb);

      // CORBA Servant Launcher
      QMutex _GUIMutex ;
      QWaitCondition _ServerLaunch;
      _GUIMutex.lock();     // to block Launch server thread until wait(mutex)

      // 2. activate embedded CORBA servers: Registry, SALOMEDS, etc.
      Session_ServerLauncher* myServerLauncher
	= new Session_ServerLauncher(argc, argv, orb, poa, &_GUIMutex, &_ServerLaunch);
      myServerLauncher->start();

      // 3. GUI activation
      // Allow multiple activation/deactivation of GUI
      //while ( 1 ) {
	MESSAGE("waiting wakeAll()");
	_ServerLaunch.wait(&_GUIMutex); // to be reseased by Launch server thread when ready:
	// atomic operation lock - unlock on mutex
	// unlock mutex: serverThread runs, calls  _ServerLaunch->wakeAll()
	// this thread wakes up, and lock mutex

	INFOS("Session activated, Launch IAPP...");

	int qArgc = 1;
	SALOME_QApplication* _qappl = new SALOME_QApplication( qArgc, argv );
	
	INFOS("creation QApplication");
	_GUIMutex.unlock();

	ASSERT ( QObject::connect(_qappl, SIGNAL(lastWindowClosed()), _qappl, SLOT(quit()) ) );

	// 3.1 SUIT_Session creation	
	SUIT_Session* aGUISession = new SALOME_Session();
	INFOS("creation SUIT_Application");

	SCRUTE(_NS);

	// 3.3 run GUI loop
	// T2.12 - catch exceptions thrown on attempts to modified a locked study
	while (1) {
	  try 
	  {
	    MESSAGE("run(): starting the main event loop");

// 3.2 load SalomeApp dynamic library
#ifdef WNT
	    SUIT_Application* aGUIApp = aGUISession->startApplication( "SalomeApp.dll", 0, 0 );
#else
            SUIT_Application* aGUIApp = aGUISession->startApplication( "libSalomeApp.so", 0, 0 );
#endif
	    if ( aGUIApp ) 
	    {
	      _qappl->setHandler( aGUISession->handler() ); // after loading SalomeApp application
                                                            // aGUISession contains SalomeApp_ExceptionHandler
	      result = _qappl->exec();
	    }
	    break;
	  }
	  catch (SALOME::SALOME_Exception& e)
	  {
	    INFOS("run(): SALOME_Exception was caught!");
	    QApplication::restoreOverrideCursor();
	    SUIT_MessageBox::warn1 ( 0,
				    QObject::tr("WRN_WARNING"), 
				    QObject::tr("SALOME_Exception was caught!"),
				    QObject::tr("BUT_OK") );
	    //QtCatchCorbaException(e);
	  }
	  catch(SALOMEDS::StudyBuilder::LockProtection&)
	  {
	    INFOS("run(): An attempt to modify a locked study has not been handled by QAD_Operation");
	    QApplication::restoreOverrideCursor();
	    SUIT_MessageBox::warn1 ( 0,
				    QObject::tr("WRN_WARNING"), 
				    QObject::tr("WRN_STUDY_LOCKED"),
				    QObject::tr("BUT_OK") );
	  }
	  catch (const CORBA::Exception& e)
	  {
	    CORBA::Any tmp;
	    tmp<<= e;
	    CORBA::TypeCode_var tc = tmp.type();
	    const char *p = tc->name();
	    INFOS ("run(): CORBA exception of the kind : "<<p<< " is caught");

	    QApplication::restoreOverrideCursor();
	    SUIT_MessageBox::error1 ( 0,
				     QObject::tr("ERR_ERROR"), 
				     QObject::tr("ERR_APP_EXCEPTION")
				     + QObject::tr(" CORBA exception ") + QObject::tr(p),
				     QObject::tr("BUT_OK") );
	  }
	  catch(exception& e)
	  {
	    INFOS("run(): An exception has been caught");
	    QApplication::restoreOverrideCursor();
	    SUIT_MessageBox::error1 ( 0,
				     QObject::tr("ERR_ERROR"), 
				     QObject::tr("ERR_APP_EXCEPTION")+ "\n" +QObject::tr(e.what()),
				     QObject::tr("BUT_OK") );
	  }
	  catch(...)
	  {
	    INFOS("run(): An exception has been caught");
	    QApplication::restoreOverrideCursor();
	    SUIT_MessageBox::error1 ( 0,
				     QObject::tr("ERR_ERROR"), 
				     QObject::tr("ERR_APP_EXCEPTION"),
				     QObject::tr("BUT_OK") );
	  }
	}
//}  end of "outer" while( 1 )

      // Prepare _GUIMutex for a new GUI activation
      //_GUIMutex.lock(); 
	//      }

      //orb->shutdown(0);
      //myServerLauncher->KillAll();
    }
  catch (SALOME_Exception& e)
    {
      INFOS("run(): SALOME::SALOME_Exception is caught: "<<e.what());
    }
  catch (CORBA::SystemException& e)
    {
      INFOS("Caught CORBA::SystemException.");
    }
  catch (CORBA::Exception& e)
    {
      INFOS("Caught CORBA::Exception.");
      CORBA::Any tmp;
      tmp<<= e;
      CORBA::TypeCode_var tc = tmp.type();
      const char *p = tc->name();
      INFOS ("run(): CORBA exception of the kind : "<<p<< " is caught");
    }
  catch(exception& e)
    {
      INFOS("run(): An exception has been caught: " <<e.what());
    }
  catch (...)
    {
      INFOS("Caught unknown exception.");
    }
  delete myThreadTrace;
  return result ;
}
