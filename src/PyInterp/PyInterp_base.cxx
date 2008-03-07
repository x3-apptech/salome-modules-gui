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
#include <structmember.h>

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

static void
PyStdOut_dealloc(PyStdOut *self)
{
  PyObject_Del(self);
}

static PyObject *
PyStdOut_write(PyStdOut *self, PyObject *args)
{
  char *c;
  int l;
  if (!PyArg_ParseTuple(args, "t#:write",&c, &l))
    return NULL;
  if(self->_cb==NULL) {
    if ( self->_iscerr )
      std::cerr << c ;
    else
      std::cout << c ;
  }
  else {
    self->_cb(self->_data,c);
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyMethodDef PyStdOut_methods[] = {
  {"write",  (PyCFunction)PyStdOut_write,  METH_VARARGS,
    PyDoc_STR("write(string) -> None")},
  {NULL,    NULL}   /* sentinel */
};

static PyMemberDef PyStdOut_memberlist[] = {
  {"softspace", T_INT,  offsetof(PyStdOut, softspace), 0,
   "flag indicating that a space needs to be printed; used by print"},
  {NULL} /* Sentinel */
};



static PyTypeObject PyStdOut_Type = {
  /* The ob_type field must be initialized in the module init function
   * to be portable to Windows without using C++. */
  PyObject_HEAD_INIT(NULL)
  0,      /*ob_size*/
  "PyOut",   /*tp_name*/
  sizeof(PyStdOut),  /*tp_basicsize*/
  0,      /*tp_itemsize*/
  /* methods */
  (destructor)PyStdOut_dealloc, /*tp_dealloc*/
  0,      /*tp_print*/
  0, /*tp_getattr*/
  0, /*tp_setattr*/
  0,      /*tp_compare*/
  0,      /*tp_repr*/
  0,      /*tp_as_number*/
  0,      /*tp_as_sequence*/
  0,      /*tp_as_mapping*/
  0,      /*tp_hash*/
        0,                      /*tp_call*/
        0,                      /*tp_str*/
        PyObject_GenericGetAttr,                      /*tp_getattro*/
        /* softspace is writable:  we must supply tp_setattro */
        PyObject_GenericSetAttr,    /* tp_setattro */
        0,                      /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT,     /*tp_flags*/
        0,                      /*tp_doc*/
        0,                      /*tp_traverse*/
        0,                      /*tp_clear*/
        0,                      /*tp_richcompare*/
        0,                      /*tp_weaklistoffset*/
        0,                      /*tp_iter*/
        0,                      /*tp_iternext*/
        PyStdOut_methods,                      /*tp_methods*/
        PyStdOut_memberlist,                      /*tp_members*/
        0,                      /*tp_getset*/
        0,                      /*tp_base*/
        0,                      /*tp_dict*/
        0,                      /*tp_descr_get*/
        0,                      /*tp_descr_set*/
        0,                      /*tp_dictoffset*/
        0,                      /*tp_init*/
        0,                      /*tp_alloc*/
        0,                      /*tp_new*/
        0,                      /*tp_free*/
        0,                      /*tp_is_gc*/
};


#define PyStdOut_Check(v)  ((v)->ob_type == &PyStdOut_Type)

static PyStdOut * newPyStdOut( bool iscerr )
{
  PyStdOut *self;
  self = PyObject_New(PyStdOut, &PyStdOut_Type);
  if (self == NULL)
    return NULL;
  self->softspace = 0;
  self->_cb = NULL;
  self->_iscerr = iscerr;
  return self;
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

  // Create python objects to capture stdout and stderr
  _vout=(PyObject*)newPyStdOut( false ); // stdout 
  _verr=(PyObject*)newPyStdOut( true );  // stderr

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
  if (PyType_Ready(&PyStdOut_Type) < 0)
    {
      PyErr_Print();
    }
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
  // 
  // probably all below code isn't required
  //
  /*
  PySys_SetObject("stderr",_verr);
  PySys_SetObject("stdout",_vout);

  //PyObject *m = PyImport_GetModuleDict();
  
  PySys_SetObject("stdout",PySys_GetObject("__stdout__"));
  PySys_SetObject("stderr",PySys_GetObject("__stderr__"));
  */
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


void PyInterp_base::setvoutcb(PyOutChanged* cb, void* data)
{  
  ((PyStdOut*)_vout)->_cb=cb;
  ((PyStdOut*)_vout)->_data=data;
}


void PyInterp_base::setverrcb(PyOutChanged* cb, void* data)
{  
  ((PyStdOut*)_verr)->_cb=cb;
  ((PyStdOut*)_verr)->_data=data;
}
