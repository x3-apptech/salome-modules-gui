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

//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 22/06/2007
//
#include "SUITApp_init_python.hxx"


PyThreadState* SUIT_PYTHON::_gtstate                = NULL;
PyObject *SUIT_PYTHON::salome_shared_modules_module = NULL;
PyInterpreterState* SUIT_PYTHON::_interp            = NULL;
bool SUIT_PYTHON::initialized                       = false;

void SUIT_PYTHON::init_python(int argc, char **argv)
{
  if (Py_IsInitialized())
  {
    return;
  }
  Py_SetProgramName(argv[0]);
  Py_Initialize(); // Initialize the interpreter
  PySys_SetArgv(argc, argv);
  SUIT_PYTHON::_interp = PyThreadState_Get()->interp;
  PyEval_InitThreads(); // Create (and acquire) the interpreter lock
  SUIT_PYTHON::_gtstate = PyEval_SaveThread(); // Release global thread state
  SUIT_PYTHON::initialized = true;
}

