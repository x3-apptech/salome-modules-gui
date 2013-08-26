# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE(UsePyQt4)
INCLUDE(${VTK_USE_FILE})

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${SIP_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/Event
  ${PROJECT_SOURCE_DIR}/src/LightApp
  ${PROJECT_SOURCE_DIR}/src/OCCViewer
  ${PROJECT_SOURCE_DIR}/src/Plot2d
  ${PROJECT_SOURCE_DIR}/src/PyConsole
  ${PROJECT_SOURCE_DIR}/src/PyInterp
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/SUITApp
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  "-DCALL_OLD_METHODS"
  ${QT_DEFINITIONS}
  ${CAS_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
)
IF(SALOME_LIGHT_ONLY)
  ADD_DEFINITIONS("-DGUI_DISABLE_CORBA")
ENDIF()

# libraries to link to
SET(_link_LIBRARIES
  ${PYTHON_LIBRARIES}
  ${VTK_LIBRARIES}
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
)

# header files / no moc processing
SET(_other_HEADERS
  SALOME_PYQT_DataObjectLight.h
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
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

# sources / sip wrappings
PYQT4_WRAP_SIP(_sip_SOURCES ${_sip_files})

# sources / static
SET(_other_SOURCES
  SALOME_PYQT_DataModelLight.cxx
  SALOME_PYQT_DataObjectLight.cxx
  SALOME_PYQT_ModuleLight.cxx
  SALOME_PYQT_PyInterp.cxx
  SALOME_PYQT_PyModule.cxx
)

# sources / to compile
SET(SalomePyQtGUILight_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_sip_SOURCES})

# --- rules ---

ADD_LIBRARY(SalomePyQtGUILight ${SalomePyQtGUILight_SOURCES})
TARGET_LINK_LIBRARIES(SalomePyQtGUILight ${_link_LIBRARIES})
INSTALL(TARGETS SalomePyQtGUILight DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomePyQtGUILight_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
