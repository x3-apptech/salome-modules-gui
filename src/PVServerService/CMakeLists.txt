# Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

ADD_SUBDIRECTORY(ENGINE)
IF(NOT SALOME_LIGHT_ONLY)
    ADD_SUBDIRECTORY(ServiceLoader)
ENDIF()    

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${KERNEL_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/tools/PyInterp/src
  )

ADD_DEFINITIONS(
  ${KERNEL_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
  )

SET(_link_LIBRARIES 
  ${KERNEL_OpUtil}        # class SALOME_Exception
  PyInterp                # class PyLockWrapper
  )

# --- headers ---
SET(PVServer_HEADERS
  PVServerService.h
  PVServer_ServiceWrapper.h  
)

# --- sources ---
SET(PVServer_SOURCES
    PVServer_ServiceWrapper.cxx
  )
  
SET(PVServer_SWIG
    PVServer_ServiceLoader.i
   )
  
# --- rules ---
ADD_LIBRARY(PVServerService ${PVServer_SOURCES})
TARGET_LINK_LIBRARIES(PVServerService ${_link_LIBRARIES})
INSTALL(TARGETS PVServerService EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

# --- header installation ---
INSTALL(FILES ${PVServer_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
