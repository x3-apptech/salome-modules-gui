//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : SALOME_PYQT_PyInterp.cxx
//  Author : Christian CAREMOLI, Paul RASCLE, EDF
//  Module : SALOME
//  $Header$

#include "SALOME_PYQT_PyInterp.h" // this include must be first (see PyInterp_base.h)!
#include "utilities.h"
#include "Container_init_python.hxx"

using namespace std;


/*!
 * constructor : the main SALOME Python interpreter is used for PyQt GUI.
 * calls initialize method defined in base class, which calls virtual methods
 * initstate & initcontext redefined here
 */
SALOME_PYQT_PyInterp::SALOME_PYQT_PyInterp(): PyInterp_base()
{
}

SALOME_PYQT_PyInterp::~SALOME_PYQT_PyInterp()
{
}

bool SALOME_PYQT_PyInterp::initState()
{
 /*
  * The GIL is assumed to not be held on the call
  * The GIL is acquired in initState and will be held on initState exit
  * It is the caller responsability to release the lock on exit if needed
  */
  SCRUTE(KERNEL_PYTHON::_gtstate);
  _tstate = KERNEL_PYTHON::_gtstate;
  PyEval_AcquireThread(_tstate);
  SCRUTE(_tstate);
  PyEval_ReleaseThread(_tstate);
  return true;
}

bool SALOME_PYQT_PyInterp::initContext()
{
  /*
   * The GIL is assumed to be held
   * It is the caller responsability to acquire the GIL before calling initContext
   * It will still be held on initContext exit
   */
  _g = PyDict_New();          // create interpreter dictionnary context
  PyObject *bimod = PyImport_ImportModule("__builtin__");
  PyDict_SetItemString(_g, "__builtins__", bimod);
  Py_DECREF(bimod);
  return true;
}

int SALOME_PYQT_PyInterp::run(const char *command)
{
  MESSAGE("compile");
  PyObject *code = Py_CompileString((char *)command,"PyGUI",Py_file_input);
  if(!code){
    // Une erreur s est produite en general SyntaxError
    PyErr_Print();
    return -1;
  }
  //#if PY_VERSION_HEX < 0x02040000 // python version earlier than 2.4.0
  //  PyObject *r = PyEval_EvalCode(code,_g,_g);
  //#else
  PyObject *r = PyEval_EvalCode((PyCodeObject *)code,_g,_g);
  //#endif
  Py_DECREF(code);
  if(!r){
    // Une erreur s est produite a l execution
    PyErr_Print();
    return -1 ;
  }
  Py_DECREF(r);
  return 0;
}
