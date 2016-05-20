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

INCLUDE(UseQtExt)
IF(SALOME_USE_VTKVIEWER)
  INCLUDE(${VTK_USE_FILE})
ENDIF()

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${SIP_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/LightApp
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SALOME_PYQT/SALOME_PYQT_GUILight
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/SalomeApp
  ${PROJECT_SOURCE_DIR}/tools/PyInterp/src
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${CAS_DEFINITIONS} 
  ${PYTHON_DEFINITIONS}
  ${BOOST_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${PYTHON_LIBRARIES}
  ${VTK_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${PLATFORM_LIBS}
  ${OMNIORB_LIBRARIES}
  SalomePyQtGUILight SalomeApp
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS SALOME_PYQT_Module.h)

# header files / no moc processing
SET(_other_HEADERS SALOME_PYQT_GUI.h)

# header files / to install
SET(SalomePyQtGUI_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES SALOME_PYQT_Module.cxx)

# sources / to compile
SET(SalomePyQtGUI_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---
ADD_LIBRARY(SalomePyQtGUI ${SalomePyQtGUI_SOURCES})
TARGET_LINK_LIBRARIES(SalomePyQtGUI ${_link_LIBRARIES})
INSTALL(TARGETS SalomePyQtGUI DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomePyQtGUI_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
