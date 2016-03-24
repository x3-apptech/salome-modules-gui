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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/DDS
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${CAS_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${CAS_KERNEL} qtx DDS)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  QDS_CheckBox.h
  QDS_ComboBox.h
  QDS_Datum.h
  QDS_LineEdit.h
  QDS_RadioBox.h
  QDS_SpinBox.h
  QDS_SpinBoxDbl.h
  QDS_TextEdit.h
)

# obsolete files: not migrated to Qt4
# _moc_HEADERS << QDS_Table.h
# TODO: migrate or remove?

# header files / no moc processing
SET(_other_HEADERS
  QDS.h
  QDS_Validator.h
)

# header files / to install
SET(QDS_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/QDS_msg_en.ts
  resources/QDS_msg_fr.ts
  resources/QDS_msg_ja.ts
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  QDS.cxx
  QDS_CheckBox.cxx
  QDS_ComboBox.cxx
  QDS_Datum.cxx
  QDS_LineEdit.cxx
  QDS_RadioBox.cxx
  QDS_SpinBox.cxx
  QDS_SpinBoxDbl.cxx
  QDS_TextEdit.cxx
  QDS_Validator.cxx
)

# obsolete files: not migrated to Qt4
# _other_SOURCES << QDS_Table.cxx
# TODO: migrate or remove?

# sources / to compile
SET(QDS_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(QDS ${QDS_SOURCES})
TARGET_LINK_LIBRARIES(QDS ${_link_LIBRARIES})
INSTALL(TARGETS QDS EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${QDS_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
