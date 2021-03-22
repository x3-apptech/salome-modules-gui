# Copyright (C) 2012-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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
INCLUDE(UsePyQt)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${OpenCASCADE_INCLUDE_DIR}
  ${QT_INCLUDES}
  ${PYTHON_INCLUDE_DIRS}
  ${SIP_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/Event
  ${PROJECT_SOURCE_DIR}/src/LightApp
  ${PROJECT_SOURCE_DIR}/src/OCCViewer
  ${PROJECT_SOURCE_DIR}/src/Plot2d
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/SUITApp
  ${PROJECT_SOURCE_DIR}/src/ObjBrowser
  ${PROJECT_SOURCE_DIR}/tools/PyConsole/src
  ${PROJECT_SOURCE_DIR}/tools/PyInterp/src
)
IF(SALOME_USE_SALOMEOBJECT)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/OBJECT)
ENDIF()

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${OpenCASCADE_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${PYTHON_LIBRARIES}
  ${OPENGL_LIBRARIES}
  LightApp
)
IF(NOT SALOME_LIGHT_ONLY)
  LIST(APPEND _link_LIBRARIES ${KERNEL_SalomeContainer})
ENDIF()

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  SALOME_PYQT_DataModelLight.h
  SALOME_PYQT_ModuleLight.h
  SALOME_PYQT_PyModule.h
  SALOME_PYQT_Selector.h
)

# header files / no moc processing
SET(_other_HEADERS
  SALOME_PYQT_DataObjectLight.h
  SALOME_PYQT_BorrowedDataObjectLight.h
  SALOME_PYQT_GUILight.h
  SALOME_PYQT_PyInterp.h
)

# header files / to install
SET(SalomePyQtGUILight_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# sip files / to be processed by sip
SET(_sip_files SALOME_PYQT_GUILight.sip)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / sip wrappings
PYQT_WRAP_SIP(_sip_SOURCES ${_sip_files})

# sources / static
SET(_other_SOURCES
  SALOME_PYQT_DataModelLight.cxx
  SALOME_PYQT_DataObjectLight.cxx
  SALOME_PYQT_BorrowedDataObjectLight.cxx
  SALOME_PYQT_ModuleLight.cxx
  SALOME_PYQT_PyInterp.cxx
  SALOME_PYQT_PyModule.cxx
  SALOME_PYQT_Selector.cxx
)

# sources / to compile
SET(SalomePyQtGUILight_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_sip_SOURCES})

# --- rules ---

ADD_LIBRARY(SalomePyQtGUILight ${SalomePyQtGUILight_SOURCES})
TARGET_LINK_LIBRARIES(SalomePyQtGUILight ${_link_LIBRARIES})
INSTALL(TARGETS SalomePyQtGUILight DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomePyQtGUILight_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

FILE(GLOB make_clean_files ${CMAKE_CURRENT_BINARY_DIR}/sip*)
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${make_clean_files}")
