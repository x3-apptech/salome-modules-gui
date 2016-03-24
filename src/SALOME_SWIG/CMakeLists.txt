# Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
IF(SALOME_USE_VTKVIEWER)
  INCLUDE(${VTK_USE_FILE})
ENDIF()

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${QWT_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/Event
  ${PROJECT_SOURCE_DIR}/src/LightApp
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/OCCViewer
  ${PROJECT_SOURCE_DIR}/src/Plot2d
  ${PROJECT_SOURCE_DIR}/src/Prs
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SOCC
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/SVTK
  ${PROJECT_SOURCE_DIR}/src/SPlot2d
  ${PROJECT_SOURCE_DIR}/src/VTKViewer
)

# swig flags
SET_SOURCE_FILES_PROPERTIES(libSALOME_Swig.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(libSALOME_Swig.i PROPERTIES SWIG_DEFINITIONS "-shadow")
SET_SOURCE_FILES_PROPERTIES(libSALOME_SwigPYTHON_wrap.cxx PROPERTIES COMPILE_FLAGS "-DHAVE_CONFIG_H")

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${QWT_DEFINITIONS} ${CAS_DEFINITIONS} ${PYTHON_DEFINITIONS})

# --- headers ---

# header files / to install
SET(libSALOME_Swig_HEADERS
  SALOMEGUI_Swig.hxx
  SALOMEGUI_Swig.i
  libSALOME_Swig.i
)

# --- scripts ---

# scripts / swig wrappings
SET(_swig_SCRIPTS
  ${CMAKE_CURRENT_BINARY_DIR}/libSALOME_Swig.py 
)

# scripts / static
SET(_other_SCRIPTS
  examplevtk1.py 
  test_table.py 
  test_big_table.py 
  test_many_objects.py 
  test_remove_ref.py
)

# --- rules ---

SWIG_ADD_MODULE(libSALOME_Swig python libSALOME_Swig.i SALOMEGUI_Swig.cxx)
SWIG_LINK_LIBRARIES(libSALOME_Swig ${PYTHON_LIBRARIES} LightApp)
IF(WIN32)
  SET_TARGET_PROPERTIES(_libSALOME_Swig PROPERTIES DEBUG_OUTPUT_NAME _libSALOME_Swig_d)
ENDIF(WIN32)

INSTALL(TARGETS _libSALOME_Swig DESTINATION ${SALOME_INSTALL_PYTHON})

SALOME_INSTALL_SCRIPTS("${_swig_SCRIPTS}"  ${SALOME_INSTALL_PYTHON})
SALOME_INSTALL_SCRIPTS("${_other_SCRIPTS}" ${SALOME_INSTALL_SCRIPT_DATA})

INSTALL(FILES ${libSALOME_Swig_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
