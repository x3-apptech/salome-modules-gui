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

INCLUDE(UseQt4Ext)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${PROJECT_SOURCE_DIR}/src/Qtx
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${PLATFORM_LIBS} ${QT_LIBRARIES} qtx)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  PyEditor_Editor.h
  PyEditor_LineNumberArea.h
  PyEditor_PyHighlighter.h
  PyEditor_SettingsDlg.h
)

# header files / no moc processing
SET(_other_HEADERS
  PyEditor.h
  PyEditor_Settings.h
)

# header files / to install
SET(PyEditor_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(RESOURCES_PATH resources)

SET(_ts_RESOURCES
  ${RESOURCES_PATH}/translations/PyEditor_msg_en.ts
  ${RESOURCES_PATH}/translations/PyEditor_msg_fr.ts
  ${RESOURCES_PATH}/translations/PyEditor_msg_ja.ts
)

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  PyEditor_Editor.cxx
  PyEditor_LineNumberArea.cxx
  PyEditor_PyHighlighter.cxx
  PyEditor_Settings.cxx
  PyEditor_SettingsDlg.cxx
)

# sources / to compile
SET(PyEditor_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---
ADD_LIBRARY(PyEditor ${PyEditor_SOURCES})
TARGET_LINK_LIBRARIES(PyEditor ${_link_LIBRARIES})
INSTALL(TARGETS PyEditor EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${PyEditor_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT4_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
