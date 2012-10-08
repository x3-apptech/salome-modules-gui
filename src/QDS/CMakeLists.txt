# Copyright (C) 2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/UseQT4EXT.cmake)

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../DDS
)

SET(COMMON_FLAGS "${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS ${QT_MT_LIBS} ${CAS_KERNEL} DDS qtx)

SET(GUI_HEADERS   
  QDS_CheckBox.h
  QDS_ComboBox.h
  QDS_Datum.h
  QDS_LineEdit.h
  QDS_SpinBox.h
  QDS_SpinBoxDbl.h
  QDS_TextEdit.h
  QDS_RadioBox.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(QDS_SOURCES
  QDS.cxx
  QDS_CheckBox.cxx
  QDS_ComboBox.cxx
  QDS_Datum.cxx
  QDS_LineEdit.cxx
  QDS_SpinBox.cxx
  QDS_SpinBoxDbl.cxx
  QDS_TextEdit.cxx
  QDS_Validator.cxx
  QDS_RadioBox.cxx
)

SET(GUITS_SOURCES
  resources/QDS_msg_en.ts
  resources/QDS_msg_fr.ts
)

ADD_LIBRARY(QDS SHARED ${QDS_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(QDS PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(QDS ${COMMON_LIBS})
INSTALL(TARGETS QDS DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  QDS.h
  QDS_CheckBox.h
  QDS_ComboBox.h
  QDS_Datum.h
  QDS_LineEdit.h
  QDS_SpinBox.h
  QDS_SpinBoxDbl.h
  QDS_TextEdit.h
  QDS_Validator.h
  QDS_RadioBox.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

