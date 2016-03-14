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

//  SALOME Session : implementation of Session_ServerThread.cxx
//  File   : Session_ServerThread.cxx
//  Author : Paul RASCLE, EDF

#include "Session_ServerThread.hxx"

#include <SALOME_NamingService.hxx>
#include <SALOME_Container_i.hxx>
#include <SALOME_Launcher.hxx>
#include <SALOMEDSClient_ClientFactory.hxx>
#include <SALOME_ModuleCatalog_impl.hxx>
#include <RegistryService.hxx>

#include "Session_Session_i.hxx"

#include <Utils_ORB_INIT.hxx>
#include <Utils_SINGLETON.hxx>
#include <Utils_SALOME_Exception.hxx>
#include <Basics_Utils.hxx>
#include <NamingService_WaitForServerReadiness.hxx>
#include <utilities.h>

#include <cstdlib>
#include <ctime>

#include <QMutex>
#include <QWaitCondition>

const int Session_ServerThread::NB_SRV_TYP = 6;
const char* Session_ServerThread::_serverTypes[NB_SRV_TYP] = {"Container",
                                                              "ModuleCatalog",
                                                              "Registry",
                                                              "SALOMEDS",
                                                              "Session",
                                                              "ContainerManager"};

/*! 
  default constructor not for use
*/
Session_ServerThread::Session_ServerThread()
{
  ASSERT(0); // must not be called
}

/*! 
  constructor
*/
Session_ServerThread::Session_ServerThread(int argc,
                                           char ** argv, 
                                           CORBA::ORB_ptr orb, 
                                           PortableServer::POA_ptr poa)
{
  //MESSAGE("Session_ServerThread Constructor " << argv[0]);
  _argc = argc;
  _argv = new char*[ _argc + 1 ];
  _argv[_argc] = 0;
  for (int i = 0; i < _argc; i++ )
    _argv[i] = strdup( argv[i] );

  _orb = CORBA::ORB::_duplicate(orb);
  _root_poa = PortableServer::POA::_duplicate(poa);
  _servType =-1;
  _NS = new SALOME_NamingService(_orb); // one instance per server to limit
                                        // multi thread coherence problems
  _container = 0;                       // embedded container
}

/*! 
  destructor 
*/
Session_ServerThread::~Session_ServerThread()
{
  //MESSAGE("~Session_ServerThread "<< _argv[0]);
  delete _NS;
  for (int i = 0; i <_argc ; i++ )
    free( _argv[i] );
  delete[] _argv;
}

/*! 
  run the thread : activate one servant, the servant type is given by
  argument _argv[0]
*/
void Session_ServerThread::Init()
{
  MESSAGE("Session_ServerThread::Init "<< _argv[0]); 

  int i;
  for (i=0; i<_argc; i++) SCRUTE(_argv[i]);

  for (i=0; i<NB_SRV_TYP; i++) {
    if (strcmp(_argv[0],_serverTypes[i])==0) {
      _servType = i;
      MESSAGE("Server Thread type : "<<_serverTypes[i]);
      switch (_servType) {
      case 0:  // Container
        {
          NamingService_WaitForServerReadiness(_NS,"/Registry");
          NamingService_WaitForServerReadiness(_NS,"/ContainerManager");
          ActivateContainer(_argc, _argv);
          break;
        }
      case 1:  // ModuleCatalog
        {
          NamingService_WaitForServerReadiness(_NS,"/Registry");
          ActivateModuleCatalog(_argc, _argv);
          break;
        }
      case 2:  // Registry
        {
          NamingService_WaitForServerReadiness(_NS,"");
          ActivateRegistry(_argc, _argv);
          break;
        }
      case 3:  // SALOMEDS
        {
          NamingService_WaitForServerReadiness(_NS,"/Kernel/ModulCatalog");
          ActivateSALOMEDS(_argc, _argv);
          break;
        }
      case 4:  // Session
        {
          NamingService_WaitForServerReadiness(_NS,"/myStudyManager");
          std::string containerName = "/Containers/";
          containerName = containerName + Kernel_Utils::GetHostname();
          containerName = containerName + "/FactoryServer";
          NamingService_WaitForServerReadiness(_NS,containerName);
          ActivateSession(_argc, _argv);
          break;
        }
      case 5: // Container Manager
        {
          NamingService_WaitForServerReadiness(_NS,"");
          ActivateContainerManager(_argc, _argv);
          break;
        }
      default:
        {
          ASSERT(0);
          break;
        }
      }
    }
  }
}

void Session_ServerThread::Shutdown()
{
  if ( _container ) _container->Shutdown();
}

void Session_ServerThread::ActivateModuleCatalog(int argc,
                                                 char ** argv)
{
  try {
    MESSAGE("ModuleCatalog thread started");
    // allocation on heap to allow destruction by POA
    
    SALOME_ModuleCatalogImpl* Catalogue_i
      = new SALOME_ModuleCatalogImpl(argc, argv);
    
    // Tell the POA that the objects are ready to accept requests.
    
    PortableServer::ObjectId_var id = _root_poa->activate_object (Catalogue_i);
    Catalogue_i->_remove_ref();
    
    CORBA::Object_var myCata = Catalogue_i->_this();
    _NS->Register(myCata ,"/Kernel/ModulCatalog");
  }
  catch(CORBA::SystemException&) {
    INFOS( "Caught CORBA::SystemException." );
  }
  catch(CORBA::Exception&) {
    INFOS( "Caught CORBA::Exception." );
  }
  catch(omniORB::fatalException& fe) {
    INFOS( "Caught omniORB::fatalException:" );
    INFOS( "  file: " << fe.file() );
    INFOS( "  line: " << fe.line() );
    INFOS( "  mesg: " << fe.errmsg() );
  }
  catch(...) {
    INFOS( "Caught unknown exception." );
  }
}

void Session_ServerThread::ActivateSALOMEDS(int argc,
                                            char ** argv)
{
  try {
    MESSAGE("SALOMEDS thread started");
    // We allocate the objects on the heap.  Since these are reference
    // counted objects, they will be deleted by the POA when they are no
    // longer needed.    
    
    ClientFactory::createStudyManager(_orb,_root_poa);
  }
  catch(CORBA::SystemException&) {
    INFOS( "Caught CORBA::SystemException." );
  }
  catch(CORBA::Exception&) {
    INFOS( "Caught CORBA::Exception." );
  }
  catch(omniORB::fatalException& fe) {
    INFOS( "Caught omniORB::fatalException:" );
    INFOS( "  file: " << fe.file() );
    INFOS( "  line: " << fe.line() );
    INFOS( "  mesg: " << fe.errmsg() );
  }
  catch(...) {
    INFOS( "Caught unknown exception." );
  }
}

void Session_ServerThread::ActivateRegistry(int argc,
                                            char ** argv)
{
  MESSAGE("Registry thread started");
  SCRUTE(argc); 
  if ( argc<3 ) {
    INFOS("you must provide the Salome session name when you call SALOME_Registry_Server");
    throw CommException("you must provide the Salome session name when you call SALOME_Registry_Server");
  }
  const char *ptrSessionName=0;

  int k=0 ;
  for ( k=1 ; k<argc ; k++ ) {
    if ( strcmp(argv[k],"--salome_session")==0 ) {
      ptrSessionName=argv[k+1];
      break;
    }
  }
  ASSERT(ptrSessionName) ;
  ASSERT(strlen( ptrSessionName )>0);
  const char *registryName = "Registry";
  Registry::Components_var varComponents;
  try {
    RegistryService *ptrRegistry = new RegistryService;
    ptrRegistry->SessionName( ptrSessionName );
    ptrRegistry->SetOrb(_orb);
    varComponents = ptrRegistry->_this();
    ptrRegistry->_remove_ref(); //let poa manage registry service deletion
    // The RegistryService must not already exist.
    
    try {
      CORBA::Object_var pipo = _NS->Resolve( registryName );
      if (CORBA::is_nil(pipo) )  throw ServiceUnreachable();
      INFOS("RegistryService servant already existing" );
      ASSERT(0);
    }
    catch( const ServiceUnreachable &/*ex*/ ) {
    }
    catch( const CORBA::Exception &/*exx*/ ) {
    }
    std::string absoluteName = std::string("/") + registryName;
    _NS->Register( varComponents , absoluteName.c_str() );
  }
  catch( const SALOME_Exception &ex ) {
    INFOS( "Communication Error : " << ex.what() );
    ASSERT(0);
  }
}

void Session_ServerThread::ActivateContainerManager(int argc,
                                             char ** argv)
{
  try {
    PortableServer::POA_var root_poa=PortableServer::POA::_the_root_poa();
    std::cout << "Activate SalomeLauncher ......!!!! " << std::endl;
    new SALOME_Launcher(_orb,root_poa);
  }
  catch(CORBA::SystemException&) {
    INFOS("Caught CORBA::SystemException.");
  }
  catch(PortableServer::POA::WrongPolicy&) {
    INFOS("Caught CORBA::WrongPolicyException.");
  }
  catch(PortableServer::POA::ServantAlreadyActive&) {
    INFOS("Caught CORBA::ServantAlreadyActiveException");
  }
  catch(CORBA::Exception&) {
    INFOS("Caught CORBA::Exception.");
  }
  catch(...) {
    INFOS("Caught unknown exception.");
  }
}

void Session_ServerThread::ActivateContainer(int argc,
                                             char ** argv)
{
  try {
    MESSAGE("Container thread started");
    
    // get or create the child POA
    
    PortableServer::POA_var factory_poa;
    try {
      factory_poa = _root_poa->find_POA("factory_poa",0);
      // 0 = no activation (already done if exists)
    }
    catch (PortableServer::POA::AdapterNonExistent&) {
      MESSAGE("factory_poa does not exists, create...");
      // define policy objects     
      PortableServer::ImplicitActivationPolicy_var implicitActivation =
        _root_poa->create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);
      // default = NO_IMPLICIT_ACTIVATION
      PortableServer::ThreadPolicy_var threadPolicy =
        _root_poa->create_thread_policy(PortableServer::ORB_CTRL_MODEL);
      // default = ORB_CTRL_MODEL, other choice SINGLE_THREAD_MODEL
      
      // create policy list
      CORBA::PolicyList policyList;
      policyList.length(2);
      policyList[0] = PortableServer::ImplicitActivationPolicy::
        _duplicate(implicitActivation);
      policyList[1] = PortableServer::ThreadPolicy::
        _duplicate(threadPolicy);
      
      PortableServer::POAManager_var nil_mgr
        = PortableServer::POAManager::_nil();
      factory_poa = _root_poa->create_POA("factory_poa",
                                          nil_mgr,
                                          policyList);
      //with nil_mgr instead of pman,
      //a new POA manager is created with the new POA
      
      // destroy policy objects
      implicitActivation->destroy();
      threadPolicy->destroy();
      
      // obtain the factory poa manager
      PortableServer::POAManager_var pmanfac = factory_poa->the_POAManager();
      pmanfac->activate();
      MESSAGE("pmanfac->activate()");
    }
    
    char *containerName = (char*)"";
    if (argc >1) {
      containerName = argv[1];
    }
    
    _container = new Engines_Container_i(_orb, _root_poa, containerName , argc , argv , true , false);
  }
  catch(CORBA::SystemException&) {
    INFOS("Caught CORBA::SystemException.");
  }
  catch(PortableServer::POA::WrongPolicy&) {
    INFOS("Caught CORBA::WrongPolicyException.");
  }
  catch(PortableServer::POA::ServantAlreadyActive&) {
    INFOS("Caught CORBA::ServantAlreadyActiveException");
  }
  catch(CORBA::Exception&) {
    INFOS("Caught CORBA::Exception.");
  }
  catch(...) {
    INFOS("Caught unknown exception.");
  }
}

void Session_ServerThread::ActivateSession(int argc,
                                           char ** argv)
{
  MESSAGE("Session_ServerThread::ActivateSession() not implemented!");
}

/*! 
  constructor 
*/
Session_SessionThread::Session_SessionThread(int argc,
                                             char** argv, 
                                             CORBA::ORB_ptr orb, 
                                             PortableServer::POA_ptr poa,
                                             QMutex* GUIMutex,
                                             QWaitCondition* GUILauncher)
: Session_ServerThread(argc, argv, orb, poa),
  _GUIMutex( GUIMutex ),
  _GUILauncher( GUILauncher )
{
}

/*! 
  destructor 
*/
Session_SessionThread::~Session_SessionThread()
{
}

void Session_SessionThread::ActivateSession(int argc,
                                            char ** argv)
{
  try {
    MESSAGE("Session thread started");
    SALOME_Session_i * mySALOME_Session
      = new SALOME_Session_i(argc, argv, _orb, _root_poa, _GUIMutex, _GUILauncher) ;
    PortableServer::ObjectId_var mySALOME_Sessionid
      = _root_poa->activate_object(mySALOME_Session);
    MESSAGE("poa->activate_object(mySALOME_Session)");
    
    CORBA::Object_var obj = mySALOME_Session->_this();
    CORBA::String_var sior(_orb->object_to_string(obj));
    mySALOME_Session->_remove_ref();
    
    mySALOME_Session->NSregister();
  }
  catch (CORBA::SystemException&) {
    INFOS("Caught CORBA::SystemException.");
  }
  catch (CORBA::Exception&) {
    INFOS("Caught CORBA::Exception.");
  }
  catch (...) {
    INFOS("Caught unknown exception.");
  }
}
