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
//  File   : PyInterp_base.cxx
//  Author : Christian CAREMOLI, Paul RASCLE, EDF
//  Module : SALOME
//  $Header$


#include <string>
#include <vector>

#include "PyInterp_base.h" // this include must be first (see PyInterp_base.h)!
#include <cStringIO.h>

using namespace std;

PyLockWrapper::PyLockWrapper(PyThreadState* theThreadState): 
  myThreadState(theThreadState),
  mySaveThreadState(0)
{
#if defined(USE_GILSTATE)
  if (myThreadState->interp == PyInterp_base::_interp) {
    _savestate = PyGILState_Ensure();
  } else {
    PyEval_AcquireThread(myThreadState);
  }
#else 
  PyEval_AcquireThread(myThreadState);
#endif
}

PyLockWrapper::~PyLockWrapper()
{
#if defined(USE_GILSTATE)
  if (myThreadState->interp == PyInterp_base::_interp) {
    PyGILState_Release(_savestate);
  } else {
    PyEval_ReleaseThread(myThreadState);
  }
#else 
  PyEval_ReleaseThread(myThreadState);
#endif
}

class PyReleaseLock{
public:
  ~PyReleaseLock(){
    PyEval_ReleaseLock();
  }
};


PyLockWrapper PyInterp_base::GetLockWrapper(){
  return _tstate;
}


// main python interpreter (static attributes)

int PyInterp_base::_argc = 1;
char* PyInterp_base::_argv[] = {""};

PyObject *PyInterp_base::builtinmodule = NULL;

PyThreadState *PyInterp_base::_gtstate = NULL;
PyInterpreterState *PyInterp_base::_interp = NULL;


/*!
 * basic constructor here : herited classes constructors must call initalize() method
 * defined here.
 */
PyInterp_base::PyInterp_base(): _tstate(0), _vout(0), _verr(0), _g(0), _atFirst(true)
{
}

PyInterp_base::~PyInterp_base()
{
}


/*!
 * Must be called by herited classes constructors. initialize() calls virtuals methods
 * initstate & initcontext, not defined here in base class. initstate & initcontext methods
 * must be implemented in herited classes, following the Python interpreter policy
 * (mono or multi interpreter...).
 */
void PyInterp_base::initialize()
{
  _history.clear();       // start a new list of user's commands 
  _ith = _history.begin();

  init_python();
  // Here the global lock is released

  initState();

  PyLockWrapper aLock= GetLockWrapper();

  initContext();

  // used to interpret & compile commands
  PyObjWrapper m(PyImport_ImportModule("codeop"));
  if(!m){
    PyErr_Print();
    return;
  }

  // Create cStringIO to capture stdout and stderr
  PycString_IMPORT;
  if (PycStringIO) { // CTH11627 : additional check
    _vout = PycStringIO->NewOutput(128);
    _verr = PycStringIO->NewOutput(128);
  }

  // All the initRun outputs are redirected to the standard output (console)
  initRun();
}

void PyInterp_base::init_python()
{
  _atFirst = false;
  if (Py_IsInitialized())
    return;

  // Python is not initialized
  Py_SetProgramName(_argv[0]);
  Py_Initialize(); // Initialize the interpreter
  PySys_SetArgv(_argc, _argv);
  PyEval_InitThreads(); // Create (and acquire) the interpreter lock
  _interp = PyThreadState_Get()->interp;
  _gtstate = PyEval_SaveThread(); // Release global thread state
}

string PyInterp_base::getbanner()
{
 // Should we take the lock ?
 // PyEval_RestoreThread(_tstate);
  string aBanner("Python ");
  aBanner = aBanner + Py_GetVersion() + " on " + Py_GetPlatform() ;
  aBanner = aBanner + "\ntype help to get general information on environment\n";
  //PyEval_SaveThread();
  return aBanner;
}


int PyInterp_base::initRun()
{
  PySys_SetObject("stderr",_verr);
  PySys_SetObject("stdout",_vout);

  PyObjWrapper verr(PyObject_CallMethod(_verr,"reset",""));
  PyObjWrapper vout(PyObject_CallMethod(_vout,"reset",""));

  //PyObject *m = PyImport_GetModuleDict();
  
  PySys_SetObject("stdout",PySys_GetObject("__stdout__"));
  PySys_SetObject("stderr",PySys_GetObject("__stderr__"));

  return 0;
}


/*!
 * This function compiles a string (command) and then evaluates it in the dictionnary
 * context if possible.
 * Returns :
 * -1 : fatal error 
 *  1 : incomplete text
 *  0 : complete text executed with success
 */
int compile_command(const char *command,PyObject *context)
{
  PyObject *m = PyImport_AddModule("codeop");
  if(!m){ // Fatal error. No way to go on.
    PyErr_Print();
    return -1;
  }
  PyObjWrapper v(PyObject_CallMethod(m,"compile_command","s",command));
  if(!v){
    // Error encountered. It should be SyntaxError,
    //so we don't write out traceback
    PyObjWrapper exception, value, tb;
    PyErr_Fetch(&exception, &value, &tb);
    PyErr_NormalizeException(&exception, &value, &tb);
    PyErr_Display(exception, value, NULL);
    return -1;
  }else if (v == Py_None){
    // Incomplete text we return 1 : we need a complete text to execute
    return 1;
  }else{
    // Complete and correct text. We evaluate it.
    //#if PY_VERSION_HEX < 0x02040000 // python version earlier than 2.4.0
    //    PyObjWrapper r(PyEval_EvalCode(v,context,context));
    //#else
    PyObjWrapper r(PyEval_EvalCode((PyCodeObject *)(void *)v,context,context));
    //#endif
    if(!r){
      // Execution error. We return -1
      PyErr_Print();
      return -1;
    }
    // The command has been successfully executed. Return 0
    return 0;
  }
}


int PyInterp_base::run(const char *command)
{
  if(_atFirst){
    int ret = 0;
    ret = simpleRun("from Help import *");
    if (ret) { 
      _atFirst = false;
      return ret;
    }
    ret = simpleRun("import salome");
    if (ret) { 
      _atFirst = false;
      return ret;
    }
    ret = simpleRun("salome.salome_init(0,1)");
    if (ret) { 
      _atFirst = false;
      return ret;
    }
    _atFirst = false;
  }
  return simpleRun(command);
}


int PyInterp_base::simpleRun(const char *command)
{
  if( !_atFirst && strcmp(command,"") != 0 ) {
    _history.push_back(command);
    _ith = _history.end();
  }

  // We come from C++ to enter Python world
  // We need to acquire the Python global lock
  //PyLockWrapper aLock(_tstate); // san - lock is centralized now

  // Reset redirected outputs before treatment
  PySys_SetObject("stderr",_verr);
  PySys_SetObject("stdout",_vout);

  PyObjWrapper verr(PyObject_CallMethod(_verr,"reset",""));
  PyObjWrapper vout(PyObject_CallMethod(_vout,"reset",""));

  int ier = compile_command(command,_g);

  // Outputs are redirected on standards outputs (console)
  PySys_SetObject("stdout",PySys_GetObject("__stdout__"));
  PySys_SetObject("stderr",PySys_GetObject("__stderr__"));

  return ier;
}


const char * PyInterp_base::getPrevious()
{
  if(_ith != _history.begin()){
    _ith--;
    return (*_ith).c_str();
  }
  else
    return BEGIN_HISTORY_PY;
}


const char * PyInterp_base::getNext()
{
  if(_ith != _history.end()){
    _ith++;
  }
  if (_ith == _history.end())
    return TOP_HISTORY_PY;
  else
    return (*_ith).c_str();
}


string PyInterp_base::getverr(){ 
  //PyLockWrapper aLock(_tstate);
  PyObjWrapper v(PycStringIO->cgetvalue(_verr));
  string aRet(PyString_AsString(v));
  return aRet;
}


string PyInterp_base::getvout(){  
  //PyLockWrapper aLock(_tstate);
  PyObjWrapper v(PycStringIO->cgetvalue(_vout));
  string aRet(PyString_AsString(v));
  return aRet;
}
