// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "SALOME_PYQT_PyInterp.h" // this include must be first !!

#include <SUITApp_init_python.hxx>
#include <PyInterp_Interp.h>
#include <utilities.h>

#ifndef GUI_DISABLE_CORBA
#include <Container_init_python.hxx>
#endif




/*!
 * constructor : the main SALOME Python interpreter is used for PyQt GUI.
 * calls initialize method defined in base class, which calls virtual methods
 * initstate & initcontext redefined here
 */
SALOME_PYQT_PyInterp::SALOME_PYQT_PyInterp(): PyInterp_Interp()
{
}

SALOME_PYQT_PyInterp::~SALOME_PYQT_PyInterp()
{
}

void SALOME_PYQT_PyInterp::initPython()
{
 /*
  * Do nothing
  * The initialization has been done in main
  */
  MESSAGE("SALOME_PYQT_PyInterp::initPython");
#ifndef GUI_DISABLE_CORBA
  if(SUIT_PYTHON::initialized) {
    ASSERT(SUIT_PYTHON::_gtstate); // initialisation in main
    SCRUTE(SUIT_PYTHON::_gtstate);
    _tstate = SUIT_PYTHON::_gtstate;
  }
  else {
    ASSERT(KERNEL_PYTHON::_gtstate); // initialisation in main
    SCRUTE(KERNEL_PYTHON::_gtstate);
    _tstate = KERNEL_PYTHON::_gtstate;
  }
#else
  SCRUTE(SUIT_PYTHON::_gtstate);
  _tstate = SUIT_PYTHON::_gtstate;
#endif
}

bool SALOME_PYQT_PyInterp::initState()
{
 /*
  * The GIL is assumed to not be held on the call
  * The GIL is acquired in initState and will be held on initState exit
  * It is the caller responsability to release the lock on exit if needed
  */
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
