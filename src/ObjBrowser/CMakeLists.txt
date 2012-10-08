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
)

SET(GUI_HEADERS OB_Browser.h)
QT4_WRAP_CPP(GUI_HEADERS ${GUI_HEADERS})
SET(ObjBrowser_SOURCES
  OB_Browser.cxx
)

SET(GUITS_SOURCES
  resources/OB_msg_en.ts
  resources/OB_msg_fr.ts
)

ADD_LIBRARY(ObjBrowser SHARED ${ObjBrowser_SOURCES} ${GUI_HEADERS})
SET_TARGET_PROPERTIES(ObjBrowser PROPERTIES COMPILE_FLAGS "${QT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(ObjBrowser qtx ${QT_MT_LIBS})
INSTALL(TARGETS ObjBrowser DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H
  OB.h
  OB_Browser.h
)
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")
