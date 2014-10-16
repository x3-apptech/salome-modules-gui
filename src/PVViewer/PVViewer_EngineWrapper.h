// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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

#ifndef PVVIEWERENGINEWRAPPER_H_
#define PVVIEWERENGINEWRAPPER_H_

#include <PyInterp_Utils.h>
#include <string>

//#include <SALOMEconfig.h>
//#include <SALOME_LifeCycleCORBA.hxx>
//#include <SALOMEDS_SObject.hxx>
//#include CORBA_SERVER_HEADER(SALOME_ModuleCatalog)
//#include CORBA_SERVER_HEADER(SALOMEDS)


/*!
 * Class facilitating the access to the PARAVIS engine without having to link
 * to it. Documentation of the method is found in the PARAVIS module (in the idl directory).
 *
 * (Two implementations are provided: one with CORBA dynamic invokation from C++, one using
 * Python - see commented elements in the .cxx file)
 */
class PVViewer_EngineWrapper
{
public:
  //! Returns the unique instance of the engine.
  static PVViewer_EngineWrapper * GetInstance();

  bool GetGUIConnected();
  void SetGUIConnected(bool isConnected);
  std::string FindOrStartPVServer(int port);
  void PutPythonTraceStringToEngine(const char *);

private:
  PVViewer_EngineWrapper();
  virtual ~PVViewer_EngineWrapper() {}

  static PVViewer_EngineWrapper * instance;
  //Engines::EngineComponent_var _component;

  PyObjWrapper pvserverEngine;
};

#endif /* PVVIEWERENGINEWRAPPER_H_ */
