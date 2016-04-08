// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author: Adrien Bruneton (CEA)

#include "PVServer_ServiceLoader.h"

#include <SALOME_LifeCycleCORBA.hxx>
#include <SALOME_NamingService.hxx>
#include <utilities.h> // MESSAGE() macro
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)

PVServer_ServiceLoader::PVServer_ServiceLoader() throw(PVServer_ServiceLoader_Exception):
  myLcc( 0 )
{
  try
  {
      myLcc = new SALOME_LifeCycleCORBA();
  }
  catch(...)
  {
      throw PVServer_ServiceLoader_Exception("Unable to instanciate SALOME_LifeCycleCORBA!");
  }
}

PVServer_ServiceLoader::~PVServer_ServiceLoader()
{
  delete myLcc;
}

SALOME_LifeCycleCORBA* PVServer_ServiceLoader::lcc()
{
  return myLcc;
}

std::string PVServer_ServiceLoader::findOrLoadService( const std::string& containerName )
{
  std::string ior = findService( containerName );
  if ( ior == "" )
    ior = loadService( containerName );
  return ior;
}

std::string PVServer_ServiceLoader::findService( const std::string& containerName )
{
  std::string ior = "";

  Engines::Container_var container = getContainer( containerName, "get" );

  if ( !CORBA::is_nil( container ) )
  {
    CORBA::String_var hName = container->getHostName();
    CORBA::Object_var obj = lcc()->namingService()->ResolveComponent( hName.in(),
                                                                      containerName.c_str(),
                                                                      "PVSERVER",
                                                                      0 );
    if ( !CORBA::is_nil( obj ) )
    {
      CORBA::ORB_var orb = lcc()->orb();
      CORBA::String_var cIor = orb->object_to_string( obj );
      ior = cIor.in();
    }
  }

  if ( ior == "" )
  {
    MESSAGE( "PVServer_ServiceLoader::findService(): Service NOT found!" );
  }
  else
  {
    MESSAGE( "PVServer_ServiceLoader::findService(): Service found!" );
  }

  return ior;
}

std::string PVServer_ServiceLoader::loadService( const std::string& containerName )
{
  std::string ior = "";

  Engines::Container_var container = getContainer( containerName, "getorstart" );

  if ( !CORBA::is_nil( container ) )
  {
    CORBA::String_var cReason;
    CORBA::String_var cIor = container->create_python_service_instance( "PVSERVER", cReason.out() );
    std::string reason = cReason.in();

    if ( reason != "" )
    {
      MESSAGE( "PVServer_ServiceLoader::loadService(): Python service instance could not be created!" );
      MESSAGE( "PVServer_ServiceLoader::loadService(): " << reason );
    }
    else
    {
      MESSAGE( "PVServer_ServiceLoader::loadService(): Container and service loaded!" );
      ior = cIor.in();
    }
  }
  else
  {
    MESSAGE( "PVServer_ServiceLoader::loadService(): Container could not be retrieved!" );
  }
  return ior;
}

Engines::Container_ptr PVServer_ServiceLoader::getContainer( const std::string& containerName,
                                                             const std::string& mode )
{
  Engines::ContainerParameters params;
  params.isMPI = false;
  params.mode = CORBA::string_dup( mode.c_str() );

  int rg = containerName.find( "/" );
  if ( rg < 0 )
  {
    params.container_name = CORBA::string_dup( containerName.c_str() );
  }
  else
  {
    params.resource_params.hostname = CORBA::string_dup( containerName.substr(0, rg).c_str() );
    params.container_name           = CORBA::string_dup( containerName.substr(rg+1).c_str() );
  }

  Engines::ContainerManager_var cm = lcc()->getContainerManager();
  Engines::Container_var container = cm->GiveContainer( params );
  return container._retn();
}
