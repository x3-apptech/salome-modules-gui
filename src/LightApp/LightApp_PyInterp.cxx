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
#include "LightApp_PyInterp.h"

#include <SUITApp_init_python.hxx>
#include <PyConsole_Interp.h>

#include <pthread.h>

/*!
 * constructor : multi Python interpreter, one per SALOME study.
 * calls initialize method defined in base class, which calls virtual methods
 * initstate & initcontext redefined here.
 */
LightApp_PyInterp::LightApp_PyInterp(): PyConsole_Interp()
{
}

/*!
 * Destructor.
 */
LightApp_PyInterp::~LightApp_PyInterp()
{
}
 
/*!\class LightApp_PyInterp
 * EDF-CCAR
 * When SALOME uses multi Python interpreter feature,
 * Every study has its own interpreter and thread state (_tstate = Py_NewInterpreter())
 * This is fine because every study has its own modules (sys.modules) stdout and stderr
 * BUT some Python modules must be imported only once. In multi interpreter context Python
 * modules (*.py) are imported several times.
 * The pyqt module must be imported only once because it registers classes in a C module.
 * It's quite the same with omniorb modules (internals and generated with omniidl)
 * This problem is handled with "shared modules" defined in salome_shared_modules.py
 * These "shared modules" are imported only once and only copied in all the other interpreters
 * BUT it's not the only problem. Every interpreter has its own __builtin__ module. That's fine
 * but if we have copied some modules and imported others problems may arise with operations that
 * are not allowed in restricted execution environment. So we must impose that all interpreters
 * have identical __builtin__ module.
 * That's all, for the moment ...
 */


bool LightApp_PyInterp::initContext()
{
  /*!
   * The GIL is assumed to be held
   * It is the caller responsability caller to acquire the GIL
   * It will still be held on initContext output
   */
  if ( !PyConsole_Interp::initContext() )
    return false;
  
  //Import special module to change the import mechanism
  PyObjWrapper m1( PyImport_ImportModule( "import_hook" ) );
  if ( !m1 )
  {
    PyErr_Print();
    return false;
    }
  
  // Call init_shared_modules to initialize the shared import mechanism for modules 
  //that must not be imported twice
  PyObjWrapper m2( PyObject_CallMethod( m1, (char*)"init_shared_modules", (char*)"O", SUIT_PYTHON::salome_shared_modules_module ) );
  if ( !m2 )
  {
    PyErr_Print();
    return false;
  }
  return true;
}

/*!
  Do nothing
  The initialization has been done in main
 */
void LightApp_PyInterp::initPython()
{
  _gtstate=SUIT_PYTHON::_gtstate; // initialisation in main
  _interp=SUIT_PYTHON::_interp;
}
