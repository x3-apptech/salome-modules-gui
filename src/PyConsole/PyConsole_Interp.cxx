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
//  File   : PyConsole_Interp.cxx
//  Author : Nicolas REJNERI, Adrien BRUNETON

#include "PyConsole_Interp.h"

/*!
  \class PyConsole_Interp
  \brief Python interpreter to be embedded to the SALOME study's GUI.

  There is only one Python interpreter for the whole SALOME environment.

  Call the initialize() method defined in the base class PyInterp_Interp,
  to initialize the interpreter after instance creation.

  The method initialize() calls virtuals methods
  - initPython()  to initialize global Python interpreter
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

/*! Sets the variable "__IN_SALOME_GUI_CONSOLE" to True.
* This is not attached to a module (like salome_iapp.IN_SALOME_GUI_CONSOLE)
* since modules are shared across all interpreters in SALOME.
*
* (GIL is already acquired here)
*/
int PyConsole_Interp::beforeRun()
{
  return PyRun_SimpleString("__builtins__.__IN_SALOME_GUI_CONSOLE=True");
}
 
int PyConsole_Interp::afterRun()
{
  return PyRun_SimpleString("__builtins__.__IN_SALOME_GUI_CONSOLE=False");
}

QStringList PyConsole_Interp::getLastMatches() const
{
  return QStringList();
}

QString PyConsole_Interp::getDocStr() const
{
  return QString();
}

int PyConsole_Interp::runDirCommand(const QString&, const QString& )
{
  return 0;
}

void PyConsole_Interp::clearCompletion()
{
}
