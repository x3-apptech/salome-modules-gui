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

INCLUDE(UseQt4Ext)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${PYTHON_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/PyConsole
  ${PROJECT_SOURCE_DIR}/src/Event
  ${PROJECT_SOURCE_DIR}/src/PyInterp
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${PYTHON_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${PYTHON_LIBRARIES} PyInterp Event qtx)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  PyConsole_ConsoleBase.h
  PyConsole_EditorBase.h
  PyConsole_EnhEditorBase.h
)

# header files / no moc processing
SET(_other_HEADERS
  PyConsole.h
  PyConsole_EnhInterp.h
  PyConsole_Event.h
  PyConsole_Interp.h
  PyConsole_Request.h
)

# header files / to install
SET(PyConsole_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/PyConsole_msg_en.ts
  resources/PyConsole_msg_fr.ts
  resources/PyConsole_msg_ja.ts
)

# --- sources ---

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  PyConsole_ConsoleBase.cxx
  PyConsole_EnhInterp.cxx
  PyConsole_Event.cxx
  PyConsole_Interp.cxx
  PyConsole_Request.cxx
  PyConsole_EditorBase.cxx
  PyConsole_EnhEditorBase.cxx
)

# sources / to compile
SET(PyConsole_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(PyConsole ${PyConsole_SOURCES})
TARGET_LINK_LIBRARIES(PyConsole ${_link_LIBRARIES})
INSTALL(TARGETS PyConsole EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${PyConsole_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT4_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
