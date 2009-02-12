//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : PyInterp_Interp.h
//  Author : Christian CAREMOLI, Paul RASCLE, EDF
//  Module : SALOME
//
#ifndef PYINTERP_INTERP_H
#define PYINTERP_INTERP_H

#include "PyInterp.h"   // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include <list>
#include <string>

class PYINTERP_EXPORT PyLockWrapper
{
  PyThreadState* myThreadState;
  PyThreadState* mySaveThreadState;
  PyGILState_STATE _savestate;
public:
  PyLockWrapper(PyThreadState* theThreadState);
  ~PyLockWrapper();
};

typedef void PyOutChanged(void* data,char * c);

class PYINTERP_EXPORT PyInterp_Interp
{
public:
  static int _argc;
  static char* _argv[];
  static PyObject *builtinmodule;
  static PyThreadState *_gtstate;
  static PyInterpreterState *_interp;
  
  PyInterp_Interp();
  virtual ~PyInterp_Interp();
  
  void initialize();

  virtual int run(const char *command); 

  PyLockWrapper GetLockWrapper();

  std::string getbanner(); 
  void setverrcb(PyOutChanged*,void*);
  void setvoutcb(PyOutChanged*,void*);

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

  virtual int beforeRun() { return 0; }
  int simpleRun(const char* command, const bool addToHistory = true);

  virtual bool initRun();
  virtual void initPython();
  virtual bool initState() = 0;
  virtual bool initContext() = 0;  
};

class PYINTERP_EXPORT PyObjWrapper
{
  PyObject* myObject;
public:
  PyObjWrapper(PyObject* theObject) : myObject(theObject) {}
  PyObjWrapper() : myObject(0) {}
  virtual ~PyObjWrapper() { Py_XDECREF(myObject); }

  operator PyObject*()    { return myObject;  }
  PyObject* operator->()  { return myObject;  }
  PyObject* get()         { return myObject;  }
  bool operator!()        { return !myObject; }
  bool operator==(PyObject* theObject) { return myObject == theObject; }
  PyObject** operator&()  { return &myObject; }
  PyObjWrapper& operator=(PyObjWrapper* theObjWrapper)
  {
    Py_XDECREF(myObject);
    myObject = theObjWrapper->myObject;
    return *this;
  }
};

typedef struct {
  PyObject_HEAD
  int softspace;
  PyOutChanged* _cb;
  void* _data;
  bool _iscerr;
} PyStdOut;

#endif // PYINTERP_INTERP_H
