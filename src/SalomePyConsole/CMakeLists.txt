# Copyright (C) 2012-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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
  ${PROJECT_SOURCE_DIR}/src/SalomePyConsole
  ${PROJECT_SOURCE_DIR}/src/PyConsole
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/Event
  ${PROJECT_SOURCE_DIR}/src/PyInterp
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${PYTHON_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${PYTHON_LIBRARIES} qtx suit PyConsole)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  SalomePyConsole_Console.h
  SalomePyConsole_Editor.h
  SalomePyConsole_EnhEditor.h
)

# header files / no moc processing
SET(_other_HEADERS
  SalomePyConsole.h
)

# header files / to install
SET(SalomePyConsole_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# --- sources ---
# resource files / to be processed by lrelease

SET(_ts_RESOURCES
  resources/SalomePyConsole_msg_en.ts
  resources/SalomePyConsole_msg_fr.ts
  resources/SalomePyConsole_msg_ja.ts
)

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  SalomePyConsole_Console.cxx
  SalomePyConsole_EnhEditor.cxx
  SalomePyConsole_Editor.cxx
)

# sources / to compile
SET(PyConsole_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(SalomePyConsole ${PyConsole_SOURCES})
TARGET_LINK_LIBRARIES(SalomePyConsole ${_link_LIBRARIES})
INSTALL(TARGETS SalomePyConsole EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomePyConsole_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT4_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
