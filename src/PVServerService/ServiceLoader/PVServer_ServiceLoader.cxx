// Copyright (C) 2010-2015  CEA/DEN, EDF R&D
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
#include "PVServer_ServiceWrapper.h"

#include <SALOME_LifeCycleCORBA.hxx>
#include <SALOME_NamingService.hxx>
#include <utilities.h> // MESSAGE() macro

PVServer_ServiceLoader::PVServer_ServiceLoader():
     _lcc(0)
{
  _lcc = new SALOME_LifeCycleCORBA();
  _ns = _lcc->namingService();
  _cm = _lcc->getContainerManager();
  _orb = _lcc->orb();

}

PVServer_ServiceLoader::~PVServer_ServiceLoader()
{
  if (_lcc)
    delete _lcc;
  _lcc = 0;
}

std::string PVServer_ServiceLoader::findOrLoadService(const char * containerName)
{
  std::string ior = findService(containerName);
  if (ior == "")
    ior = loadService(containerName);
  return ior;
}

std::string PVServer_ServiceLoader::findService(const char * containerName)
{
  std::string path = std::string(containerName) + "/PVSERVER";
  CORBA::Object_ptr obj = _ns->Resolve(path.c_str());
  if(!CORBA::is_nil(obj))
    {
      MESSAGE("PVServer_ServiceLoader::findService(): Service found! ");
      char * ior = _orb->object_to_string(obj);
      return std::string(ior);
    }
  else
    {
      MESSAGE("PVServer_ServiceLoader::findService(): Service NOT found! ");
      return std::string("");
    }

}

std::string PVServer_ServiceLoader::loadService(const char * containerName)
{
  // Get the requested container
  Engines::ContainerParameters params;
  params.container_name = CORBA::string_dup(containerName);
  params.isMPI = false;
  params.mode = "getorstart";
  Engines::Container_ptr contain = _cm->GiveContainer(params);

  if (!CORBA::is_nil(contain))
    {
      char * reason;
      char * ior = contain->create_python_service_instance("PVSERVER", reason);
      std::string reas(reason);
      CORBA::string_free(reason);
      if (reas != "")
        {
          MESSAGE("PVServer_ServiceLoader::loadService(): Python service instance could not be created! ");
          MESSAGE("PVServer_ServiceLoader::loadService(): " << reas);
          return std::string("");
        }
      else
        {
          MESSAGE("PVServer_ServiceLoader::loadService(): Container and service loaded! ");
          return std::string(ior);
        }
    }
  else
    {
      MESSAGE("PVServer_ServiceLoader::loadService(): Container could not be retrieved! ");
      return std::string("");
    }
}

