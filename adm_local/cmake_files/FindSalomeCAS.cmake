# Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# OpenCascade detection for Salome

set(CAS_ROOT_DIR $ENV{CAS_ROOT_DIR} CACHE PATH "Path to OpenCascade directory")
if(EXISTS ${CAS_ROOT_DIR})
  set(CMAKE_INCLUDE_PATH ${CAS_ROOT_DIR}/inc)
  set(CMAKE_PROGRAM_PATH ${CAS_ROOT_DIR}/bin)
  if(WINDOWS)
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
      set(CMAKE_LIBRARY_PATH ${CAS_ROOT}/win32/libd)
    else(CMAKE_BUILD_TYPE STREQUAL Debug)
      set(CMAKE_LIBRARY_DIR ${CAS_ROOT}/win32/lib)
    endif(CMAKE_BUILD_TYPE STREQUAL Debug)
  else(WINDOWS)
    set(CMAKE_LIBRARY_PATH ${CAS_ROOT_DIR}/lib)
  endif(WINDOWS)
endif(EXISTS ${CAS_ROOT_DIR})
find_package(CAS)

IF(CAS_StdLPlugin)
  SET(CAS_STDPLUGIN StdPlugin)
ELSE(CAS_StdLPlugin)
  IF(CAS_TKStdSchema)
    SET(CAS_STDPLUGIN TKStdSchema)
  ENDIF(CAS_TKStdSchema)
ENDIF(CAS_StdLPlugin)