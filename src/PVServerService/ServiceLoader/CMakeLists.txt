# Copyright (C) 2010-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE(${SWIG_USE_FILE})

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OMNIORB_INCLUDE_DIR}
  ${KERNEL_INCLUDE_DIRS}
  )

ADD_DEFINITIONS(
  ${OMNIORB_DEFINITIONS}
  ${KERNEL_DEFINITIONS}
  )

SET(_link_LIBRARIES 
  ${KERNEL_OpUtil}        # class SALOME_Exception
  ${KERNEL_SalomeNS}
  ${KERNEL_SalomeLifeCycleCORBA}
  ${KERNEL_SalomeIDLKernel}
  )

# --- headers ---
SET(_HEADERS
  PVServer_ServiceLoader.h
)

# --- sources ---
SET(_SOURCES
 PVServer_ServiceLoader.cxx
)
  
SET(_SWIG
  PVServer_ServiceLoader.i
)
  
# --- rules ---
ADD_LIBRARY(PVServerServiceLoader ${_SOURCES})
TARGET_LINK_LIBRARIES(PVServerServiceLoader ${_link_LIBRARIES})
INSTALL(TARGETS PVServerServiceLoader EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

# --- SWIG part ---
SET_SOURCE_FILES_PROPERTIES(${_SWIG} PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(${_SWIG} PROPERTIES SWIG_DEFINITIONS "-shadow")
SWIG_ADD_MODULE(PVServer_ServiceLoader python ${_SWIG})
IF(WIN32)
  SET_TARGET_PROPERTIES(_PVServer_ServiceLoader PROPERTIES DEBUG_OUTPUT_NAME _PVServer_ServiceLoader_d)
ENDIF(WIN32)
SWIG_LINK_LIBRARIES(PVServer_ServiceLoader ${PYTHON_LIBRARIES} PVServerServiceLoader)

INSTALL(TARGETS ${SWIG_MODULE_PVServer_ServiceLoader_REAL_NAME} EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_PYTHON})
SALOME_INSTALL_SCRIPTS(${CMAKE_CURRENT_BINARY_DIR}/PVServer_ServiceLoader.py ${SALOME_INSTALL_PYTHON})
  
# --- header installation ---
INSTALL(FILES ${_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
