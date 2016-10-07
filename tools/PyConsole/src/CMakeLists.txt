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

IF(PYCONSOLE_BUILD_PYTHON)
  ADD_SUBDIRECTORY(python)
ENDIF(PYCONSOLE_BUILD_PYTHON)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${PYTHON_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/PyInterp
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${PYTHON_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${PYTHON_LIBRARIES} PyInterp)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  PyConsole_Console.h
  PyConsole_Editor.h
)

# header files / no moc processing
SET(_other_HEADERS
  PyConsole.h
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
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  PyConsole_Console.cxx
  PyConsole_Event.cxx
  PyConsole_Interp.cxx
  PyConsole_Request.cxx
  PyConsole_Editor.cxx
)

# sources / to compile
SET(PyConsole_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(PyConsole ${PyConsole_SOURCES})
TARGET_LINK_LIBRARIES(PyConsole ${_link_LIBRARIES})
INSTALL(TARGETS PyConsole EXPORT ${TOOLS_EXPORT_NAME}TargetGroup DESTINATION ${PYCONSOLE_INSTALL_LIBS})

INSTALL(FILES ${PyConsole_HEADERS} DESTINATION ${PYCONSOLE_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${PYCONSOLE_INSTALL_RES}")
