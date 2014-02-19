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
//  File   : SalomeApp_PyInterp.cxx
//  Author : Nicolas REJNERI

#include "SalomeApp_PyInterp.h"

#include <utilities.h>
#include <Container_init_python.hxx>

#include "PyInterp.h" // this include must be first (see PyInterp_base.h)!

/*!
 * constructor : multi Python interpreter, one per SALOME study.
 * calls initialize method defined in base class, which calls virtual methods
 * initstate & initcontext redefined here.
 */
SalomeApp_PyInterp::SalomeApp_PyInterp(): 
  PyConsole_EnhInterp(), myFirstRun( true )
{
}

/*!
 * Destructor.
 */
SalomeApp_PyInterp::~SalomeApp_PyInterp()
{
}
 
/*!
  Do nothing (we could rely on the test done in the implementation of this method in the super
  class PyInterp_Interp, but in this context we are sure the initialization has been done in main()
  of SALOME_Session_Server)
 */
void SalomeApp_PyInterp::initPython()
{
  MESSAGE("SalomeApp_PyInterp::initPython - does nothing");
}

/*!
  Called before each Python command running.
*/
int SalomeApp_PyInterp::beforeRun()
{
  if ( myFirstRun ) {
    myFirstRun = false;
    int ret = simpleRun( "from Help import *", false );
    if ( ret )
      return ret;
    ret = simpleRun( "import salome", false );
    if (ret)
      return ret;
    ret = simpleRun( "salome.salome_init(0,1)", false );
    if (ret)
      return ret;
  }
  return true;
}
