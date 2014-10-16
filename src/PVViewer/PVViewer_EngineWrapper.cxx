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

#include "PVViewer_EngineWrapper.h"
#include <Utils_SALOME_Exception.hxx>

//#include <SalomeApp_Application.h>
#include <SUIT_Session.h>

PVViewer_EngineWrapper * PVViewer_EngineWrapper::instance = NULL;

PVViewer_EngineWrapper * PVViewer_EngineWrapper::GetInstance()
{
  if (!instance)
    instance = new PVViewer_EngineWrapper();
  return instance;
}

//static SalomeApp_Application* getApplication()
//{
//  if ( SUIT_Session::session() )
//    return dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
//  return 0;
//}
//
//PVViewer_EngineWrapper::PVViewer_EngineWrapper()
//// : paravisEngine(NULL)
//{
//  _component = getApplication()->lcc()->FindOrLoad_Component( "FactoryServer", "PARAVIS" );
//}
//
//bool PVViewer_EngineWrapper::GetGUIConnected()
//{
//  CORBA::Request_var req = _component->_request("GetGUIConnected");
//  req->set_return_type(CORBA::_tc_boolean);
//  req->invoke();
//  CORBA::Exception *exc =req->env()->exception();
//  if( exc )
//    throw SALOME_Exception("Unable to invoke PARAVIS engine!");
//  CORBA::Any & ret = req->return_value();
//  CORBA::Boolean bo;
//
//  if (ret >>= bo)
//    return bool(bo);
//  else
//    throw SALOME_Exception("Unable to convert engine result!");
//}
//
//void PVViewer_EngineWrapper::SetGUIConnected(bool isConnected)
//{
//  CORBA::Request_var req = _component->_request("SetGUIConnected");
//  CORBA::Boolean arg = isConnected;
//  req->add_in_arg() <<= arg;
//  req->set_return_type(CORBA::_tc_void);
//  req->invoke();
//  CORBA::Exception *exc =req->env()->exception();
//  if( exc )
//    throw SALOME_Exception("Unable to invoke PARAVIS engine!");
//}
//
//std::string PVViewer_EngineWrapper::FindOrStartPVServer(int port)
//{
//  CORBA::Request_var req = _component->_request("FindOrStartPVServer");
//  CORBA::Long arg = port;
//  req->add_in_arg() <<= arg;
//  req->set_return_type(CORBA::_tc_string);
//  req->invoke();
//  CORBA::Exception *exc =req->env()->exception();
//  if( exc )
//    throw SALOME_Exception("Unable to invoke PARAVIS engine!");
//
//  const char* ret;
//  if(req->return_value() >>= ret)
//    return std::string(ret);
//  else
//    throw SALOME_Exception("Unable to convert engine result!");
//}

PVViewer_EngineWrapper::PVViewer_EngineWrapper() :
    paravisEngine(NULL)
{
//  const char * cmd = "import PARAVIS_utils;e=";
  PyLockWrapper lock;
  const char* code = "import PARAVIS_utils as pa;__enginePARAVIS=pa.getEngine()";
  int ret = PyRun_SimpleString(const_cast<char*>(code));

  if (ret == -1)
    throw SALOME_Exception("Unable to retrieve PARAVIS engine!");

  // Now get the reference to __engine and save the pointer.
  PyObject* main_module = PyImport_AddModule((char*)"__main__");
  PyObject* global_dict = PyModule_GetDict(main_module);
  PyObjWrapper tmp(PyDict_GetItemString(global_dict, "__enginePARAVIS"));
  paravisEngine = tmp;
}



bool PVViewer_EngineWrapper::GetGUIConnected()
{
  PyLockWrapper lock;
  PyObjWrapper obj(PyObject_CallMethod(paravisEngine, (char*)("GetGUIConnected"), NULL));
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PARAVIS engine!");
    }
  return PyObject_IsTrue(obj);
}

void PVViewer_EngineWrapper::SetGUIConnected(bool isConnected)
{
  PyLockWrapper lock;

  PyObjWrapper obj(PyObject_CallMethod(paravisEngine, (char*)("SetGUIConnected"),
                                       (char *)"i", (int)isConnected ) );
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PARAVIS engine!");
    }
}

std::string PVViewer_EngineWrapper::FindOrStartPVServer(int port)
{
  PyLockWrapper lock;
  PyObjWrapper obj(PyObject_CallMethod(paravisEngine, (char*)("FindOrStartPVServer"),
                                         (char *)"i", port ) );
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PARAVIS engine!");
    }
  char * s = PyString_AsString(obj);

  return std::string(s);
}

void PVViewer_EngineWrapper::PutPythonTraceStringToEngine(const char * str)
{
  PyLockWrapper lock;
  PyObjWrapper obj(PyObject_CallMethod(paravisEngine, (char*)("PutPythonTraceStringToEngine"),
                                       (char *)"s",  str) );
  if (!obj)
    {
      PyErr_Print();
      throw SALOME_Exception("Unable to invoke PARAVIS engine!");
    }
}
