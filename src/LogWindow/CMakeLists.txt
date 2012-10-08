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
  ${QT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
)

SET(GUI_HEADERS LogWindow.h)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(GUITS_SOURCES
  resources/LogWindow_msg_en.ts
  resources/LogWindow_msg_fr.ts
)

ADD_LIBRARY(LogWindow SHARED LogWindow.cxx ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(LogWindow PROPERTIES COMPILE_FLAGS "${QT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(LogWindow qtx suit ${QT_MT_LIBS})
INSTALL(TARGETS LogWindow DESTINATION ${GUI_salomelib_LIBS})

FILE(GLOB COMMON_HEADERS_H "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")
