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
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../STD
)

SET(COMMON_LIBS
  qtx 
  suit 
  std
  ${QT_MT_LIBS}
)

SET(CAM_HEADERS CAM_Application.h CAM_Module.h CAM_Study.h CAM_DataModel.h)
QT4_WRAP_CPP(CAM_HEADERS_MOC ${CAM_HEADERS})

SET(CAM_SOURCES
  CAM_Application.cxx
  CAM_DataModel.cxx
  CAM_DataObject.cxx
  CAM_Module.cxx
  CAM_Study.cxx
)

SET(CAMTS_SOURCES
  resources/CAM_msg_en.ts
  resources/CAM_msg_fr.ts
)

ADD_LIBRARY(CAM SHARED ${CAM_SOURCES} ${CAM_HEADERS_MOC})
SET_TARGET_PROPERTIES(CAM PROPERTIES COMPILE_FLAGS "${QT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(CAM ${COMMON_LIBS})
INSTALL(TARGETS CAM DESTINATION ${GUI_salomelib_LIBS})

FILE(GLOB COMMON_HEADERS_H "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${CAMTS_SOURCES}" "${GUI_salomeres_DATA}")
