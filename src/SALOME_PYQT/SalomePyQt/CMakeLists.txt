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
INCLUDE(UsePyQt)
IF(SALOME_USE_VTKVIEWER)
  INCLUDE(${VTK_USE_FILE})
ENDIF()

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${QWT_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${SIP_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/Event
  ${PROJECT_SOURCE_DIR}/src/LightApp
  ${PROJECT_SOURCE_DIR}/src/LogWindow
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/ObjBrowser
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SALOME_PYQT/SALOME_PYQT_GUILight
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/tools/PyInterp/src
  ${PROJECT_SOURCE_DIR}/tools/PyConsole/src
)

IF(SALOME_USE_OCCVIEWER)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/OCCViewer)
ENDIF(SALOME_USE_OCCVIEWER)
IF(SALOME_USE_PVVIEWER)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/PVViewer)
ENDIF(SALOME_USE_PVVIEWER)
IF(SALOME_USE_PLOT2DVIEWER)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Plot2d)
ELSE(SALOME_USE_PLOT2DVIEWER)
  #disable 'ENABLE_PLOT2D' feature
  SET(PYQT_SIPFLAGS ${PYQT_SIPFLAGS} "-xENABLE_PLOT2D;")
ENDIF(SALOME_USE_PLOT2DVIEWER)

IF(SALOME_LIGHT_ONLY)
  #disable 'ENABLE_CORBA' feature
  SET(PYQT_SIPFLAGS ${PYQT_SIPFLAGS} "-xENABLE_CORBA;")
ELSE(SALOME_LIGHT_ONLY)
  INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/SalomeApp
  )
ENDIF(SALOME_LIGHT_ONLY)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QWT_DEFINITIONS}
  ${QT_DEFINITIONS}
  ${CAS_DEFINITIONS}
  ${BOOST_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${PYTHON_LIBRARIES}
  ${VTK_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${QWT_LIBRARY}
  SalomePyQtGUILight
) 

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS SalomePyQt.h)

# --- resources ---

# sip files / to be processed by sip
SET(_sip_files SalomePyQt.sip)
IF(SALOME_USE_PLOT2DVIEWER)
  LIST(APPEND _sip_files SalomePyQt_Plot2d.sip)
ENDIF(SALOME_USE_PLOT2DVIEWER)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / sip wrappings
PYQT_WRAP_SIP(_sip_SOURCES ${_sip_files})

# sources / static
SET(_other_SOURCES SalomePyQt.cxx)

# sources / to compile
SET(SalomePyQt_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_sip_SOURCES})

# --- rules ---

ADD_LIBRARY(SalomePyQt ${SalomePyQt_SOURCES})
TARGET_LINK_LIBRARIES(SalomePyQt ${_link_LIBRARIES})
INSTALL(TARGETS SalomePyQt DESTINATION ${SALOME_INSTALL_LIBS})

IF(WIN32)
  SET_TARGET_PROPERTIES(SalomePyQt PROPERTIES SUFFIX ".pyd" DEBUG_OUTPUT_NAME SalomePyQt_d RELEASE_OUTPUT_NAME SalomePyQt)
ELSE()
  SET_TARGET_PROPERTIES(SalomePyQt PROPERTIES PREFIX "")
ENDIF()

FILE(GLOB make_clean_files ${CMAKE_CURRENT_BINARY_DIR}/sip*)
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${make_clean_files}")
