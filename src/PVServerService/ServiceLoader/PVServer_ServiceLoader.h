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

#ifndef PVSERVERSERVICELOADER_H_
#define PVSERVERSERVICELOADER_H_

#include "PVServerServiceLoader.h"
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)
#include <string>
#include <exception>

class SALOME_LifeCycleCORBA;

class PVSERVERSERVICELOADER_EXPORT PVServer_ServiceLoader_Exception: public std::exception
{
public:
  PVServer_ServiceLoader_Exception(const std::string & what):_what(what) {}
  ~PVServer_ServiceLoader_Exception() throw () {}
  virtual const char* what() const throw() { return _what.c_str(); }
private:
  std::string _what;
};

class PVSERVERSERVICELOADER_EXPORT PVServer_ServiceLoader
{
public:
  PVServer_ServiceLoader() throw(PVServer_ServiceLoader_Exception);
  virtual ~PVServer_ServiceLoader();

  //! Get the IOR of the CORBA service handling the PVServer
  std::string findOrLoadService( const std::string& );

private:
  SALOME_LifeCycleCORBA* lcc();
  std::string findService( const std::string& );
  std::string loadService( const std::string& );
  Engines::Container_ptr getContainer( const std::string&, const std::string& );

private:
  SALOME_LifeCycleCORBA* myLcc;
};

#endif /* PVSERVERSERVICELOADER_H_ */
