// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//  File   : SalomeApp_PyInterp.cxx
//  Author : Nicolas REJNERI

#include "SalomeApp_PyInterp.h"

/*!
  \brief Constructor
*/
SalomeApp_PyInterp::SalomeApp_PyInterp()
  : PyConsole_Interp(), myFirstRun( true ), myFirstInitStudy( false )
{
}

/*!
  \brief Destructor.
*/
SalomeApp_PyInterp::~SalomeApp_PyInterp()
{
}
 
/*!
 * Initialize context dictionaries. GIL is held already.
 * The code executed in an embedded interpreter is expected to be run at the module
 * level, in which case local and global context have to be the same dictionary.
 * See: http://stackoverflow.com/questions/12265756/c-python-running-python-code-within-a-context
 * for an explanation.
 */
bool SalomeApp_PyInterp::initContext()
{
  bool ok = PyConsole_Interp::initContext();
  if ( ok ) {
    int ret = PyRun_SimpleString( "import salome_iapp; salome_iapp.IN_SALOME_GUI = True" );
    ok = ok && (ret == 0);
  }
  return ok;
}

/*!
  \brief Called before each Python command running.
*/
int SalomeApp_PyInterp::beforeRun()
{
  if ( myFirstRun ) {
    myFirstRun = false;
    int ret = simpleRun( "from Help import *", false );
    if ( ret )
      return ret;
  }
  if ( myFirstInitStudy ) {
    myFirstInitStudy = false;
    int ret = simpleRun( "import salome", false );
    if ( ret )
      return ret;
    ret = simpleRun( "salome.salome_init(0,1)", false );
    if ( ret )
      return ret;
  }
  return PyConsole_Interp::beforeRun();
}

/*!
  \brief Called when study is initialized
 */
void SalomeApp_PyInterp::initStudy()
{
  myFirstInitStudy = true;
}

/*!
  \brief Called when study is closed
*/
void SalomeApp_PyInterp::closeContext()
{
  myFirstInitStudy = false;
  simpleRun( "import salome", false );
  simpleRun( "salome.salome_close()", false );
}
