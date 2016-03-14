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

//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 22/06/2007
//
#ifndef _SUITAPP_INIT_PYTHON_
#define _SUITAPP_INIT_PYTHON_

#include <pthread.h> 

// rnv: avoid compilation warning on Linux : "_POSIX_C_SOURCE" and "_XOPEN_SOURCE" are redefined
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif 

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif 

#include <Python.h>

#ifdef WIN32
#  if defined SUITAPP_EXPORTS || defined SUITApp_EXPORTS
#    define SUITAPP_EXPORT __declspec(dllexport)
#  else
#   define SUITAPP_EXPORT __declspec(dllimport)
#  endif
#else
#  define SUITAPP_EXPORT
#endif

struct SUITAPP_EXPORT SUIT_PYTHON
{
  static bool initialized;
  static void init_python(int argc, char **argv);
};

#endif // _SUITAPP_INIT_PYTHON_
