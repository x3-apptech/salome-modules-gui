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

#include "PVServer_ServiceWrapper.h"
#include <Utils_SALOME_Exception.hxx>

#include <PyInterp_Utils.h>

class PVServer_ServiceWrapper::Private
{
public:
  // Borrowed reference to the Python object representing the service
  PyObject* pvserverService;
};

PVServer_ServiceWrapper * PVServer_ServiceWrapper::instance = NULL;

PVServer_ServiceWrapper * PVServer_ServiceWrapper::GetInstance()
{
  if (!instance)
    instance = new PVServer_ServiceWrapper();
  return instance;
}

PVServer_ServiceWrapper::PVServer_ServiceWrapper()
{
  myData = new Private;
  PyLockWrapper lock;
#ifdef GUI_DISABLE_CORBA
  const char* code = "import PVSERVER_impl as pi;__servicePVSERVER=pi.PVSERVER_Impl()";
#else
  const char* code = "import PVSERVER_utils as pa;__servicePVSERVER=pa.getService()";
#endif

  int ret = PyRun_SimpleString(const_cast<char*>(code));
  if (ret == -1)
    throw SALOME_Exception("Unable to retrieve PVSERVER service!");

  // Now get the reference to __engine and save the pointer.
  // All the calls below returns *borrowed* references
  PyObject* main_module = PyImport_AddModule((char*)"__main__");
  PyObject* global_dict = PyModule_GetDict(main_module);
  PyObject* tmp = PyDict_GetItemString(global_dict, "__servicePVSERVER");
  myData->pvserverService = tmp;
}

PVServer_ServiceWrapper::~PVServer_ServiceWrapper()
{
  StopPVServer();
  delete myData;
}

bool PVServer_ServiceWrapper::GetGUIConnected()
{
  PyLockWrapper lock;
  PyObjWrapper obj(PyObject_CallMethod(myData->pvserverService, (char*)("GetGUIConnected"), NULL));
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PVSERVER engine!");
    }
  return PyObject_IsTrue(obj);
}

void PVServer_ServiceWrapper::SetGUIConnected(bool isConnected)
{
  PyLockWrapper lock;

  PyObjWrapper obj(PyObject_CallMethod(myData->pvserverService, (char*)("SetGUIConnected"),
                                       (char *)"i", (int)isConnected ) );
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PVSERVER service!");
    }
}

std::string PVServer_ServiceWrapper::FindOrStartPVServer(int port)
{
  PyLockWrapper lock;
  PyObjWrapper obj(PyObject_CallMethod(myData->pvserverService, (char*)("FindOrStartPVServer"),
                                         (char *)"i", port ) );
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PVSERVER service!");
    }
  return std::string(PyString_AsString(obj));
}


bool PVServer_ServiceWrapper::StopPVServer()
{
  PyLockWrapper lock;
  PyObjWrapper obj(PyObject_CallMethod(myData->pvserverService, (char*)("StopPVServer"), NULL ));
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PVSERVER service!");
    }
  return PyObject_IsTrue(obj);
}

