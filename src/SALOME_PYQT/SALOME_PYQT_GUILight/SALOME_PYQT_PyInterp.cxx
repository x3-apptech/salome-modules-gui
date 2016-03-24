// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
  MESSAGE("SALOME_PYQT_PyInterp::initPython - does nothing");
}

/*!
 * Override. Create a distinct context from the SALOME Python console.
 * Especially the global context is not connected to __main__ as in PyInterp_Interp
 */
bool SALOME_PYQT_PyInterp::initContext()
{
  /*
   * The GIL is assumed to be held
   * It is the caller responsability to acquire the GIL before calling initContext
   * It will still be held on initContext exit
   */
  _local_context = PyDict_New();
  _global_context = PyDict_New();
  return true;
}

int SALOME_PYQT_PyInterp::run(const char *command)
{
  MESSAGE("compile");
  PyObject *code = Py_CompileString((char *)command,"PyGUI",Py_file_input);
  if(!code){
    // An error occured - normally here a SyntaxError
    PyErr_Print();
    return -1;
  }
  PyObject *r = PyEval_EvalCode((PyCodeObject *)code,_global_context,_local_context);

  Py_DECREF(code);
  if(!r){
      // An error occured at execution
    PyErr_Print();
    return -1 ;
  }
  Py_DECREF(r);
  return 0;
}
