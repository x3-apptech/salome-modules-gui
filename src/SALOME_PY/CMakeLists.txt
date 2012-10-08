# Copyright (C) 2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../LightApp
  ${CMAKE_CURRENT_SOURCE_DIR}/../Event
  ${CMAKE_CURRENT_SOURCE_DIR}/../Session
  ${CMAKE_CURRENT_SOURCE_DIR}/../SVTK
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../CAM
  ${CMAKE_CURRENT_SOURCE_DIR}/../STD
  ${CMAKE_CURRENT_SOURCE_DIR}/../VTKViewer
  ${CMAKE_CURRENT_SOURCE_DIR}/../OBJECT
)

SET(COMMON_FLAGS "-DHAVE_CONFIG_H ${CAS_DEFINITIONS} ${QT_DEFINITIONS} ${BOOST_DEFINITIONS} ${VTK_DEFINITIONS} ${PYTHON_DEFINITIONS} ${PLATFORM_DEFINITIONS}")

SET(COMMON_LIBS
  ${PYTHON_LIBS}
  ${QT_MT_LIBS}
  ${VTK_LIBS}
  ${OGL_LIBS}
  LightApp
  ${vtkCommonPythonD}
  ${vtkGraphicsPythonD}
  ${vtkImagingPythonD}
  ${vtkPythonCore}
)

ADD_LIBRARY(SalomePy SHARED SalomePy.cxx)
SET_TARGET_PROPERTIES(SalomePy PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(SalomePy ${COMMON_LIBS})
INSTALL(TARGETS SalomePy DESTINATION ${GUI_salomelib_LIBS})

IF(WINDOWS)
  IF(CMAKE_BUILD_TYPE STREQUAL Release)
    INSTALL(FILES ${CMAKE_INSTALL_PREFIX}/${GUI_salomelib_LIBS}/SalomePy.dll DESTINATION ${GUI_salomelib_LIBS} RENAME libSalomePy.pyd)
  ELSE(CMAKE_BUILD_TYPE STREQUAL Release)
    INSTALL(FILES ${CMAKE_INSTALL_PREFIX}/${GUI_salomelib_LIBS}/SalomePy.dll DESTINATION ${GUI_salomelib_LIBS} RENAME libSalomePy_d.pyd)
  ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)
ENDIF(WINDOWS)
