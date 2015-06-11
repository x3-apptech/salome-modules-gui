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

#include "PVViewer_EngineWrapper.h"
#include <Utils_SALOME_Exception.hxx>

#include <SUIT_Session.h>
#include <PyInterp_Utils.h>

class PVViewer_EngineWrapper::Private
{
public:
  PyObjWrapper pvserverEngine;
};

PVViewer_EngineWrapper * PVViewer_EngineWrapper::instance = NULL;

PVViewer_EngineWrapper * PVViewer_EngineWrapper::GetInstance()
{
  if (!instance)
    instance = new PVViewer_EngineWrapper();
  return instance;
}

PVViewer_EngineWrapper::PVViewer_EngineWrapper()
{
  myData = new Private;
  PyLockWrapper lock;
  const char* code = "import PVSERVER_utils as pa;__enginePVSERVER=pa.getEngine()";
  int ret = PyRun_SimpleString(const_cast<char*>(code));

  if (ret == -1)
    throw SALOME_Exception("Unable to retrieve PVSERVER engine!");

  // Now get the reference to __engine and save the pointer.
  PyObject* main_module = PyImport_AddModule((char*)"__main__");
  PyObject* global_dict = PyModule_GetDict(main_module);
  PyObjWrapper tmp(PyDict_GetItemString(global_dict, "__enginePVSERVER"));
  myData->pvserverEngine = tmp;
}

PVViewer_EngineWrapper::~PVViewer_EngineWrapper()
{
  delete myData;
}

bool PVViewer_EngineWrapper::GetGUIConnected()
{
  PyLockWrapper lock;
  PyObjWrapper obj(PyObject_CallMethod(myData->pvserverEngine, (char*)("GetGUIConnected"), NULL));
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PVSERVER engine!");
    }
  return PyObject_IsTrue(obj);
}

void PVViewer_EngineWrapper::SetGUIConnected(bool isConnected)
{
  PyLockWrapper lock;

  PyObjWrapper obj(PyObject_CallMethod(myData->pvserverEngine, (char*)("SetGUIConnected"),
                                       (char *)"i", (int)isConnected ) );
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PVSERVER service!");
    }
}

std::string PVViewer_EngineWrapper::FindOrStartPVServer(int port)
{
  PyLockWrapper lock;
  PyObjWrapper obj(PyObject_CallMethod(myData->pvserverEngine, (char*)("FindOrStartPVServer"),
                                         (char *)"i", port ) );
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PVSERVER service!");
    }
  char * s = PyString_AsString(obj);

  return std::string(s);
}

void PVViewer_EngineWrapper::PutPythonTraceStringToEngine(const char * str)
{
  PyLockWrapper lock;
  PyObjWrapper obj(PyObject_CallMethod(myData->pvserverEngine, (char*)("PutPythonTraceStringToEngine"),
                                       (char *)"s",  str) );
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PVSERVER service!");
    }
}
