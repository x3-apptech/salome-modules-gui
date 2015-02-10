// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 22/06/2007
//
#include "LightApp_PyInterp.h"

/*!
 * constructor : Python interpreter.
 */
LightApp_PyInterp::LightApp_PyInterp(): PyConsole_EnhInterp()
{
}

/*!
 * Destructor.
 */
LightApp_PyInterp::~LightApp_PyInterp()
{
}
 
/*!\class LightApp_PyInterp
 * [ABN] : there is now a single Python interpreter for the whole SALOME run.
 * Different execution environment are provided to emulate independent
 * "virtual" Python interpreters.
 */


/*!
  Do nothing
  The initialization has been done in main - see SUITApp/SUITApp.cxx - main()
 */
void LightApp_PyInterp::initPython()
{
}
