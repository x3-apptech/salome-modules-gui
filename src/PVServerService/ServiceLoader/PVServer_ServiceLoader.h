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

#ifndef PVSERVERSERVICELOADER_H_
#define PVSERVERSERVICELOADER_H_

#include "PVServerServiceLoader.h"

#include <SALOME_ContainerManager.hxx>

class SALOME_LifeCycleCORBA;
class SALOME_NamingService;

class PVSERVERSERVICELOADER_EXPORT PVServer_ServiceLoader
{
public:
  PVServer_ServiceLoader();
  virtual ~PVServer_ServiceLoader();

  //! Get the IOR of the CORBA service handling the PVServer
  std::string findOrLoadService(const char * containerName);

private:
  std::string findService(const char * containerName);
  std::string loadService(const char * containerName);

  SALOME_LifeCycleCORBA * _lcc;
  CORBA::ORB_ptr _orb;
  SALOME_NamingService * _ns;
  Engines::ContainerManager_ptr _cm;
};

#endif /* PVSERVERSERVICELOADER_H_ */
