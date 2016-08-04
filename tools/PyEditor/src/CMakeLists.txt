# Copyright (C) 2015-2016  OPEN CASCADE
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
# Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
#

INCLUDE(UseQtExt)

IF(PYEDITOR_BUILD_PYTHON)
  ADD_SUBDIRECTORY(python)
ENDIF(PYEDITOR_BUILD_PYTHON)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${PLATFORM_LIBS} ${QT_LIBRARIES})

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  PyEditor_Editor.h
  PyEditor_LineNumberArea.h
  PyEditor_PyHighlighter.h
  PyEditor_SettingsDlg.h
  PyEditor_Window.h
)

# header files / no moc processing
SET(_other_HEADERS
  PyEditor.h
  PyEditor_Settings.h
  PyEditor_StdSettings.h
)

# header files / to install
SET(PyEditor_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/translations/PyEditor_msg_en.ts
  resources/translations/PyEditor_msg_fr.ts
  resources/translations/PyEditor_msg_ja.ts
)

# resource files / to be processed by rcc
SET(_rcc_RESOURCES resources/PyEditor.qrc)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / rcc wrappings
QT_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

# sources / static
SET(_other_SOURCES
  PyEditor_Editor.cxx
  PyEditor_LineNumberArea.cxx
  PyEditor_PyHighlighter.cxx
  PyEditor_Settings.cxx
  PyEditor_SettingsDlg.cxx
  PyEditor_StdSettings.cxx
  PyEditor_Window.cxx
)

# sources / to compile
# - for library
SET(PyEditor_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_rcc_SOURCES})
# - for executable
SET(pyeditorexe_SOURCES ${PyEditor_SOURCES} PyEditor.cxx)

# --- rules ---

ADD_LIBRARY(PyEditor ${PyEditor_SOURCES})
TARGET_LINK_LIBRARIES(PyEditor ${_link_LIBRARIES})
INSTALL(TARGETS PyEditor EXPORT ${TOOLS_EXPORT_NAME}TargetGroup DESTINATION ${PYEDITOR_INSTALL_LIBS})

IF(PYEDITOR_BUILD_EXE)
  ADD_EXECUTABLE(pyeditorexe ${pyeditorexe_SOURCES})
  SET_TARGET_PROPERTIES(pyeditorexe PROPERTIES OUTPUT_NAME "pyeditor")
  TARGET_LINK_LIBRARIES(pyeditorexe ${_link_LIBRARIES})
  TARGET_COMPILE_DEFINITIONS(pyeditorexe PUBLIC "-DPYEDITOREXE")
  INSTALL(TARGETS pyeditorexe EXPORT ${TOOLS_EXPORT_NAME}TargetGroup DESTINATION ${PYEDITOR_INSTALL_BINS})
ENDIF(PYEDITOR_BUILD_EXE)

INSTALL(FILES ${PyEditor_HEADERS} DESTINATION ${PYEDITOR_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${PYEDITOR_INSTALL_RES}")
