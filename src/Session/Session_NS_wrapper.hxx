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

#pragma once

#include "SALOME_NamingService.hxx"
#include "SALOME_Session.hxx"

#include "omniORB4/CORBA.h"

class QMutex;
class QWaitCondition;

class Abstract_Engines_Container_i;

class SESSION_EXPORT OldStyleNS
{
public:
  using RealNS = SALOME_NamingService;
public:
  OldStyleNS(CORBA::ORB_ptr orb):_NS(orb) {}
  void Register(CORBA::Object_ptr ObjRef, const char* Path) { _NS.Register(ObjRef,Path); }
  CORBA::Object_ptr Resolve(const char* Path) { return _NS.Resolve(Path); }
  RealNS *getNS() { return &_NS; }
  Abstract_Engines_Container_i *activateContainer(CORBA::ORB_var orb, PortableServer::POA_var poa, int argc, char** argv);
  static void defineDefaultSALOMEKERNELNamingService() { /* nothing to do by default true NamingService server is considered */ }
  static void activateContainerManager(CORBA::ORB_var orb);
  static void activateSession(CORBA::ORB_var orb, PortableServer::POA_var poa, QMutex *GUIMutex, QWaitCondition *GUILauncher, int argc, char ** argv);
  static void activateSALOMEDS(CORBA::ORB_var orb, PortableServer::POA_var poa);
  static CORBA::Object_var forServerChecker(const char *NSName, int argc, char **argv);
  static CosNaming::NamingContext_var checkTrueNamingServiceIfExpected(int argc, char **argv, bool& forceOK);
  static const char LibName[];
private:
  RealNS _NS;
};

#include "SALOME_Fake_NamingService.hxx"

class SESSION_EXPORT NewStyleNS
{
public:
  using RealNS = SALOME_Fake_NamingService;
public:
  NewStyleNS(CORBA::ORB_ptr orb):_NS(orb) {}
  void Register(CORBA::Object_ptr ObjRef, const char* Path) { _NS.Register(ObjRef,Path); }
  CORBA::Object_ptr Resolve(const char* Path) { return _NS.Resolve(Path); }
  RealNS *getNS() { return &_NS; }
  Abstract_Engines_Container_i *activateContainer(CORBA::ORB_var orb, PortableServer::POA_var poa, int argc, char** argv);
  static void defineDefaultSALOMEKERNELNamingService();
  static void activateContainerManager(CORBA::ORB_var orb);
  static void activateSession(CORBA::ORB_var orb, PortableServer::POA_var poa, QMutex *GUIMutex, QWaitCondition *GUILauncher, int argc, char ** argv);
  static void activateSALOMEDS(CORBA::ORB_var orb, PortableServer::POA_var poa);
  static CORBA::Object_var forServerChecker(const char *NSName, int argc, char **argv);
  static CosNaming::NamingContext_var checkTrueNamingServiceIfExpected(int argc, char **argv, bool& forceOK);
  static const char LibName[];
private:
  RealNS _NS;
};
