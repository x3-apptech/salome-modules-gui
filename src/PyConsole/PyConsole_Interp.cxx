// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//  File   : PyConsole_Interp.cxx
//  Author : Nicolas REJNERI
//  Module : SALOME
//
#include "PyConsole_Interp.h"

/*!
  \class PyConsole_Interp
  \brief Python interpreter to be embedded to the SALOME study's GUI.

  There is only one Python interpreter for the whole SALOME environment.

  Call the initialize() method defined in the base class PyInterp_Interp,
  to initialize the interpreter after instance creation.

  The method initialize() calls virtuals methods
  - initPython()  to initialize global Python interpreter
  //- initState()   to initialize embedded interpreter state (OBSOLETE)
  - initContext() to initialize interpreter internal context
  - initRun()     to prepare interpreter for running commands
*/

/*!
  \brief Constructor.

  Creates new python interpreter.
*/
PyConsole_Interp::PyConsole_Interp(): PyInterp_Interp()
{
}

/*!
  \brief Destructor.

  Does nothing for the moment.
*/
PyConsole_Interp::~PyConsole_Interp()
{
}
 
/*!
  \brief Initialize python interpeter context.

  The GIL is assumed to be held.
  It is the caller responsability to acquire the GIL.
  It must still be held on initContext() exit.

  \return \c true on success
*/
bool PyConsole_Interp::initContext()
{
  PyObject *m = PyImport_AddModule("__main__");  // interpreter main module (module context)
  if(!m){
    PyErr_Print();
    return false;
  }  
  _context = PyModule_GetDict(m);          // get interpreter dictionnary context
  return true;
}
