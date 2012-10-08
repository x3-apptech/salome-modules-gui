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
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
)

SET(COMMON_FLAGS "${QT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS ${QT_MT_LIBS} suit qtx)

SET(GUI_HEADERS   
  STD_Application.h
  STD_MDIDesktop.h
  STD_SDIDesktop.h
  STD_TabDesktop.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(std_SOURCES
  STD_Application.cxx
  STD_MDIDesktop.cxx
  STD_SDIDesktop.cxx
  STD_TabDesktop.cxx
)

SET(GUITS_SOURCES
  resources/STD_images.ts
  resources/STD_msg_en.ts
  resources/STD_msg_fr.ts
)

ADD_LIBRARY(std SHARED ${std_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(std PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(std ${COMMON_LIBS})
INSTALL(TARGETS std DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  STD.h
  STD_Application.h
  STD_MDIDesktop.h
  STD_SDIDesktop.h
  STD_TabDesktop.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
SET(GUIPNG_DATA ${GUIPNG_DATA} ${CMAKE_CURRENT_SOURCE_DIR}/resources/std.ini)
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${GUI_salomeres_DATA})
