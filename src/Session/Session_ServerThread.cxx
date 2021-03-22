// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "Session_Promises.hxx"

#include <SALOME_NamingService.hxx>
#include <SALOME_Container_i.hxx>
#include <SALOME_Launcher.hxx>
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

template<class MY_NS>
const int Session_ServerThread<MY_NS>::NB_SRV_TYP = 6;

template<class MY_NS>
const char* Session_ServerThread<MY_NS>::_serverTypes[NB_SRV_TYP] = {"Container",
                                                                      "ModuleCatalog",
                                                                      "Registry",
                                                                      "SALOMEDS",
                                                                      "Session",
                                                                      "ContainerManager"};

/*! 
  default constructor not for use
*/
template<class MY_NS>
Session_ServerThread<MY_NS>::Session_ServerThread()
{
  ASSERT(0); // must not be called
}

/*! 
  constructor
*/
template<class MY_NS>
Session_ServerThread<MY_NS>::Session_ServerThread(int argc,
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
  _NS.reset( new MY_NS(_orb) ); // one instance per server to limit
                                               // multi thread coherence problems
  _container = nullptr;                        // embedded container
}

/*! 
  destructor 
*/
template<class MY_NS>
Session_ServerThread<MY_NS>::~Session_ServerThread()
{
  for (int i = 0; i <_argc ; i++ )
    free( _argv[i] );
  delete[] _argv;
}

/*! 
  run the thread : activate one servant, the servant type is given by
  argument _argv[0]
*/
template<class MY_NS>
void Session_ServerThread<MY_NS>::Init()
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
          NamingService_WaitForServerReadiness(this->getNS(),"/Registry");
          NamingService_WaitForServerReadiness(this->getNS(),"/ContainerManager");
          ActivateContainer(_argc, _argv);
          break;
        }
      case 1:  // ModuleCatalog
        {
          NamingService_WaitForServerReadiness(this->getNS(),"/Registry");
          ActivateModuleCatalog(_argc, _argv);
          break;
        }
      case 2:  // Registry
        {
          NamingService_WaitForServerReadiness(this->getNS(),"");
          ActivateRegistry(_argc, _argv);
          break;
        }
      case 3:  // SALOMEDS
        {
          NamingService_WaitForServerReadiness(this->getNS(),"/Kernel/ModulCatalog");
          ActivateSALOMEDS(_argc, _argv);
          break;
        }
      case 4:  // Session
        {
          NamingService_WaitForServerReadiness(this->getNS(),"/Study");
          std::string containerName = "/Containers/";
          containerName = containerName + Kernel_Utils::GetHostname();
          containerName = containerName + "/FactoryServer";
          NamingService_WaitForServerReadiness(this->getNS(),containerName);
          ActivateSession(_argc, _argv);
          break;
        }
      case 5: // Container Manager
        {
          NamingService_WaitForServerReadiness(this->getNS(),"");
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

template<class MY_NS>
void Session_ServerThread<MY_NS>::Shutdown()
{
  if ( _container ) _container->Shutdown();
}

template<class MY_NS>
void Session_ServerThread<MY_NS>::ActivateModuleCatalog(int argc, char ** argv)
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

template<class MY_NS>
void Session_ServerThread<MY_NS>::ActivateSALOMEDS(int /*argc*/, char** /*argv*/)
{
  this->_NS->activateSALOMEDS(this->_orb,this->_root_poa);
}

template<class MY_NS>
void Session_ServerThread<MY_NS>::ActivateRegistry(int argc, char ** argv)
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
    //
    CORBA::PolicyList policies;
    policies.length(1);
    PortableServer::ThreadPolicy_var threadPol(_root_poa->create_thread_policy(PortableServer::SINGLE_THREAD_MODEL));
    policies[0]=PortableServer::ThreadPolicy::_duplicate(threadPol);
    PortableServer::POAManager_var manager = _root_poa->the_POAManager();
    PortableServer::POA_var poa2(_root_poa->create_POA("SingleThreadPOA4RegistryEmbedded",manager,policies));
    threadPol->destroy();
    //
    PortableServer::ObjectId_var id(poa2->activate_object(ptrRegistry));
    CORBA::Object_var pipo=poa2->id_to_reference(id);
    varComponents = Registry::Components::_narrow(pipo) ;
    ptrRegistry->_remove_ref(); //let poa manage registryservice deletion
    
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

template<class MY_NS>
void Session_ServerThread<MY_NS>::ActivateContainerManager(int /*argc*/, char** /*argv*/)
{
  this->_NS->activateContainerManager(this->_orb);
}

template<class MY_NS>
typename MY_NS::RealNS *Session_ServerThread<MY_NS>::getNS()
{
  MY_NS *pt(_NS.get());
  if(!pt)
     THROW_SALOME_EXCEPTION("Session_ServerThread<MY_NS>::getNS : null pointer !");
  return pt->getNS();
}

template<class MY_NS>
void Session_ServerThread<MY_NS>::ActivateContainer(int argc, char** argv)
{
  _container = this->_NS->activateContainer(this->_orb,this->_root_poa,argc,argv);
}

template<class MY_NS>
void Session_ServerThread<MY_NS>::ActivateSession(int /*argc*/, char** /*argv*/)
{
  MESSAGE("Session_ServerThread::ActivateSession() not implemented!");
}

template<class MY_NS>
Session_SessionThread<MY_NS>::Session_SessionThread(int argc,
                                             char** argv, 
                                             CORBA::ORB_ptr orb, 
                                             PortableServer::POA_ptr poa,
                                             QMutex* GUIMutex,
                                             QWaitCondition* GUILauncher)
: Session_ServerThread<MY_NS>(argc, argv, orb, poa),
  _GUIMutex( GUIMutex ),
  _GUILauncher( GUILauncher )
{
}

template<class MY_NS>
Session_SessionThread<MY_NS>::~Session_SessionThread()
{
}

template<class MY_NS>
void Session_SessionThread<MY_NS>::ActivateSession(int argc, char ** argv)
{
  this->_NS->activateSession(this->_orb,this->_root_poa,_GUIMutex,_GUILauncher,argc,argv);
}

template class Session_ServerThread<OldStyleNS>;
template class Session_SessionThread<OldStyleNS>;

template class Session_ServerThread<NewStyleNS>;
template class Session_SessionThread<NewStyleNS>;
