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

INCLUDE(${VTK_USE_FILE})

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${PYTHON_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/Event
  ${PROJECT_SOURCE_DIR}/src/LightApp
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/SVTK
  ${PROJECT_SOURCE_DIR}/src/VTKViewer
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${QT_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${PYTHON_LIBRARIES}
  ${QT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${VTK_LIBRARIES}
  LightApp
)

# --- rules ---

ADD_LIBRARY(SalomePy SalomePy.cxx)
TARGET_LINK_LIBRARIES(SalomePy ${_link_LIBRARIES})
INSTALL(TARGETS SalomePy DESTINATION ${SALOME_INSTALL_LIBS})

# TODO: check on windows
IF(WIN32)
  SET_TARGET_PROPERTIES(SalomePy PROPERTIES PREFIX "lib" SUFFIX ".pyd" DEBUG_OUTPUT_NAME SalomePy_d RELEASE_OUTPUT_NAME SalomePy)
ENDIF(WIN32)

# TODO: check if PYTHON_ADD_MODULE() macro can be used to build this target