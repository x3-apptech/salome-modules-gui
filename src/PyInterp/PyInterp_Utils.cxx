// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  Author : Adrien BRUNETON
//

#include "PyInterp_Utils.h"

#include <iostream>

/*!
  \class PyLockWrapper
  \brief Python GIL wrapper.
*/

/*!
  \brief Constructor. Automatically acquires GIL.
*/
PyLockWrapper::PyLockWrapper()
{
  _gil_state = PyGILState_Ensure();
  // Save current thread state for later comparison
  _state = PyGILState_GetThisThreadState();
}

/*!
  \brief Destructor. Automatically releases GIL.
*/
PyLockWrapper::~PyLockWrapper()
{
  PyThreadState * _currState = PyGILState_GetThisThreadState();
  if (_currState != _state)
    {
      std::cout << "!!!!!!!!! PyLockWrapper inconsistency - now entering infinite loop for debugging\n";
      while(1);
    }

  PyGILState_Release(_gil_state);
}

