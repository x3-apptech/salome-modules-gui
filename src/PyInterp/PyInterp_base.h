// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  File   : PyInterp_base.h
//  Author : Christian CAREMOLI, Paul RASCLE, EDF
//  Module : SALOME

#ifndef _PYINTERP_BASE_H_
#define _PYINTERP_BASE_H_

#include "PyInterp.h"

#include <list>
#include <string>
#include <iostream>

// include order important!
// pthread then python then qt
//#include <pthread.h>  // must be before Python.h !

#include <Python.h>   // must be before qt includes ...
#include <compile.h>   // Python include needed for versions before 2.4. Included in Python.h now.
#include <eval.h>   // Python include needed for versions before 2.4. Included in Python.h now.

//#if PY_VERSION_HEX < 0x02040000 // python version earlier than 2.4.0
//extern "C" PyObject * PyEval_EvalCode(PyObject *co, PyObject *g, PyObject *l);
//#endif

/* For 2.3, use the PyGILState_ calls */
#if (PY_VERSION_HEX >= 0x02030000)
#define USE_GILSTATE
#endif

#define TOP_HISTORY_PY "--- top of history ---"
#define BEGIN_HISTORY_PY "--- begin of history ---"

class PYINTERP_EXPORT PyLockWrapper
{
  PyThreadState* myThreadState;
  PyThreadState* mySaveThreadState;
#if defined(USE_GILSTATE)
  PyGILState_STATE _savestate ;
#endif
 public:
  PyLockWrapper(PyThreadState* theThreadState);
  ~PyLockWrapper();
};

class PYINTERP_EXPORT PyInterp_base{
 public:
  static int _argc;
  static char* _argv[];
  static PyObject *builtinmodule;
  static PyThreadState *_gtstate;
  static PyInterpreterState *_interp;
  
  PyInterp_base();
  ~PyInterp_base();
  
  virtual void initialize();
  virtual void init_python();
  // init_python() made virtual to:
  // 1. Remove dependency on KERNEL in light SALOME configuration
  // 2. Allow redefinition of this method in SalomeApp_PyInterp class (it should be empty there and rely on KERNEL_PYTHON)

  virtual int run(const char *command); 

  PyLockWrapper GetLockWrapper();

  std::string getbanner(); 
  std::string getverr();
  std::string getvout();  

  const char * getPrevious();
  const char * getNext();    

 protected:
  PyThreadState * _tstate;
  PyObject * _vout;
  PyObject * _verr;
  PyObject * _g;
  PyObject * _codeop;
  std::list<std::string> _history;
  std::list<std::string>::iterator _ith;
  bool _atFirst;

  int simpleRun(const char* command);
  int initRun();

  virtual bool initState() = 0;
  virtual bool initContext() = 0;  
};


class PYINTERP_EXPORT PyObjWrapper{
  PyObject* myObject;
public:
  PyObjWrapper(PyObject* theObject): myObject(theObject) {}
  PyObjWrapper(): myObject(0) {}
  operator PyObject*(){
    return myObject;
  }
  PyObject* operator->(){
    return myObject;
  }
  PyObject* get(){
    return myObject;
  }
  bool operator!(){
    return !myObject;
  }
  bool operator==(PyObject* theObject){
    return myObject == theObject;
  }
  PyObject** operator&(){
    return &myObject;
  }
  PyObjWrapper& operator=(PyObjWrapper* theObjWrapper){
    Py_XDECREF(myObject);
    myObject = theObjWrapper->myObject;
    return *this;
  }
  virtual ~PyObjWrapper(){ 
    Py_XDECREF(myObject);
  }
};

#endif
