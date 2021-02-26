// Copyright (C) 2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "Session_NS_wrapper.hxx"

#include "SALOME_Fake_NamingService.hxx"
#include "SALOME_Container_i.hxx"
#include "SALOME_Launcher.hxx"
#include "SALOMEDSClient_ClientFactory.hxx"

#include "Session_Session_i.hxx"
#include "Session_Promises.hxx"
#include "utilities.h"

const char OldStyleNS::LibName[]="SalomeApp";
const char NewStyleNS::LibName[]="SalomeAppSL";

void CommonActivateSession(CORBA::ORB_var orb, PortableServer::POA_var poa, QMutex *GUIMutex, QWaitCondition *GUILauncher, SALOME_NamingService_Abstract *ns, int argc, char ** argv)
{
  try {
    MESSAGE("Session thread started");
    SALOME_Session_i * mySALOME_Session = nullptr;
    if(!ns)
      mySALOME_Session = new SALOME_Session_i(argc, argv, orb, poa, GUIMutex, GUILauncher);
    else
      mySALOME_Session = new SALOME_Session_i(argc, argv, orb, poa, GUIMutex, GUILauncher,ns);
    PortableServer::ObjectId_var mySALOME_Sessionid = poa->activate_object(mySALOME_Session);
    MESSAGE("poa->activate_object(mySALOME_Session)");
    
    CORBA::Object_var obj = mySALOME_Session->_this();
    SALOME::Session_var objC = SALOME::Session::_narrow(obj);
    GetSessionRefSingleton()->set_value(objC);
    CORBA::String_var sior(orb->object_to_string(obj));
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

Engines_Container_i *OldStyleNS::activateContainer(CORBA::ORB_var orb, PortableServer::POA_var poa, int argc, char **argv)
{
  Engines_Container_i *_container = nullptr;
  try
  {
    MESSAGE("Container thread started");

    // get or create the child POA

    PortableServer::POA_var factory_poa;
    try
    {
      factory_poa = poa->find_POA("factory_poa", 0);
      // 0 = no activation (already done if exists)
    }
    catch (PortableServer::POA::AdapterNonExistent &)
    {
      MESSAGE("factory_poa does not exists, create...");
      // define policy objects
      PortableServer::ImplicitActivationPolicy_var implicitActivation =
          poa->create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);
      // default = NO_IMPLICIT_ACTIVATION
      PortableServer::ThreadPolicy_var threadPolicy =
          poa->create_thread_policy(PortableServer::ORB_CTRL_MODEL);
      // default = ORB_CTRL_MODEL, other choice SINGLE_THREAD_MODEL

      // create policy list
      CORBA::PolicyList policyList;
      policyList.length(2);
      policyList[0] = PortableServer::ImplicitActivationPolicy::
          _duplicate(implicitActivation);
      policyList[1] = PortableServer::ThreadPolicy::
          _duplicate(threadPolicy);

      PortableServer::POAManager_var nil_mgr = PortableServer::POAManager::_nil();
      factory_poa = poa->create_POA("factory_poa",
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

    char *containerName = (char *)"";
    if (argc > 1)
    {
      containerName = argv[1];
    }
    _container = new Engines_Container_i(orb, poa, containerName, argc, argv, nullptr, false);
  }
  catch (CORBA::SystemException &)
  {
    INFOS("Caught CORBA::SystemException.");
  }
  catch (PortableServer::POA::WrongPolicy &)
  {
    INFOS("Caught CORBA::WrongPolicyException.");
  }
  catch (PortableServer::POA::ServantAlreadyActive &)
  {
    INFOS("Caught CORBA::ServantAlreadyActiveException");
  }
  catch (CORBA::Exception &)
  {
    INFOS("Caught CORBA::Exception.");
  }
  catch (...)
  {
    INFOS("Caught unknown exception.");
  }
  return _container;
}

void OldStyleNS::activateContainerManager(CORBA::ORB_var orb)
{
  try {
    PortableServer::POA_var root_poa=PortableServer::POA::_the_root_poa();
    std::cout << "Activate SalomeLauncher ......!!!! " << std::endl;
    new SALOME_Launcher(orb,root_poa);
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

void OldStyleNS::activateSession(CORBA::ORB_var orb, PortableServer::POA_var poa, QMutex *GUIMutex, QWaitCondition *GUILauncher, int argc, char ** argv)
{
  CommonActivateSession(orb,poa,GUIMutex,GUILauncher,nullptr,argc,argv);
}

void OldStyleNS::activateSALOMEDS(CORBA::ORB_var orb, PortableServer::POA_var poa)
{
  try {
    MESSAGE("SALOMEDS thread started");
    // We allocate the objects on the heap.  Since these are reference
    // counted objects, they will be deleted by the POA when they are no
    // longer needed.    
    
    ClientFactory::createStudy(orb,poa);
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

#include "Utils_ORB_INIT.hxx"
#include "Utils_SINGLETON.hxx"

CORBA::Object_var OldStyleNS::forServerChecker(const char *NSName, int argc, char **argv)
{
  ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
  CORBA::ORB_var orb = init( argc, argv );
  SALOME_NamingService &NS = *SINGLETON_<SALOME_NamingService>::Instance();
  ASSERT( SINGLETON_<SALOME_NamingService>::IsAlreadyExisting() );
  NS.init_orb( orb );
  CORBA::Object_var obj = NS.Resolve( NSName );
  return obj;
}

CosNaming::NamingContext_var OldStyleNS::checkTrueNamingServiceIfExpected(int argc, char **argv, bool& forceOK)
{
  forceOK = false;//tell checker : do as before
  ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
  CORBA::ORB_var orb = init( argc, argv );
  CORBA::Object_var obj = orb->resolve_initial_references( "NameService" );
  CosNaming::NamingContext_var _root_context = CosNaming::NamingContext::_narrow( obj );
  return _root_context;
}

#include "SALOME_KernelServices.hxx"

void NewStyleNS::defineDefaultSALOMEKERNELNamingService()
{
  KERNEL::assignNamingServiceSL();
}

Engines_Container_i *NewStyleNS::activateContainer(CORBA::ORB_var orb, PortableServer::POA_var poa, int argc, char **argv)
{
  return KERNEL::getContainerSA();
}

void NewStyleNS::activateContainerManager(CORBA::ORB_var orb)
{
  KERNEL::getLauncherSA();
}

void NewStyleNS::activateSession(CORBA::ORB_var orb, PortableServer::POA_var poa, QMutex *GUIMutex, QWaitCondition *GUILauncher, int argc, char ** argv)
{
  SALOME_Fake_NamingService *ns=new SALOME_Fake_NamingService;
  CommonActivateSession(orb,poa,GUIMutex,GUILauncher,ns,argc,argv);
}

void NewStyleNS::activateSALOMEDS(CORBA::ORB_var orb, PortableServer::POA_var poa)
{
  ClientFactory::createStudyWithoutNS(orb,poa);
}

CORBA::Object_var NewStyleNS::forServerChecker(const char *NSName, int argc, char **argv)
{
  SALOME_Fake_NamingService ns;
  return ns.Resolve(NSName);
}

CosNaming::NamingContext_var NewStyleNS::checkTrueNamingServiceIfExpected(int argc, char **argv, bool& forceOK)
{
  forceOK = true;//tell checker : forget it's always OK
  return CosNaming::NamingContext::_nil();
}
