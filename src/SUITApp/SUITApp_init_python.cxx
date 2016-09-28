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


#include "SUITApp_init_python.hxx"
#include <QString>

bool SUIT_PYTHON::initialized = false;

void SUIT_PYTHON::init_python(int argc, char **argv)
{
  if (Py_IsInitialized())
  {
    return;
  }
  Py_SetProgramName(argv[0]);
  Py_Initialize(); // Initialize the interpreter
  PySys_SetArgv(argc, argv);
  PyRun_SimpleString("import threading\n");
  // VSR (22/09/2016): This is a workaround to prevent invoking qFatal() from PyQt5
  // causing application aborting
  QString script;
  script += "def _custom_except_hook(exc_type, exc_value, exc_traceback):\n";
  script += "  import sys\n";
  script += "  sys.__excepthook__(exc_type, exc_value, exc_traceback)\n";
  script += "  pass\n";
  script += "\n";
  script += "import sys\n";
  script += "sys.excepthook = _custom_except_hook\n";
  script += "del _custom_except_hook, sys\n";
  int res = PyRun_SimpleString(qPrintable(script));
  // VSR (22/09/2016): end of workaround
  PyEval_InitThreads(); // Create (and acquire) the interpreter lock - can be called many times
  // Py_InitThreads acquires the GIL
  PyThreadState *pts = PyGILState_GetThisThreadState(); 
  PyEval_ReleaseThread(pts);
  SUIT_PYTHON::initialized = true;
}
