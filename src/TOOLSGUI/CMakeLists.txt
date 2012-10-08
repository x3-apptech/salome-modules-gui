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
  ${QT_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT)

SET(COMMON_LIBS
  ${QT_MT_LIBS}
  suit
  ${CAS_KERNEL}
  ${SalomeIDLKernel}
  ${SALOMELocalTrace}
  ${SalomeNS}
  ${OpUtil}
  SalomeIDLGUI
)

SET(GUI_HEADERS ToolsGUI_CatalogGeneratorDlg.h ToolsGUI_RegWidget.h)
QT4_WRAP_CPP(GUI_HEADERS ${GUI_HEADERS})
SET(ToolsGUI_SOURCES
  ToolsGUI_CatalogGeneratorDlg.cxx
  ToolsGUI_RegWidget.cxx
  ToolsGUI.cxx
)

SET(GUITS_SOURCES
  resources/ToolsGUI_msg_en.ts
  resources/ToolsGUI_msg_fr.ts
)

ADD_LIBRARY(ToolsGUI SHARED ${ToolsGUI_SOURCES} ${GUI_HEADERS})
SET_TARGET_PROPERTIES(ToolsGUI PROPERTIES COMPILE_FLAGS "${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS} ${OMNIORB_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(ToolsGUI ${COMMON_LIBS})
INSTALL(TARGETS ToolsGUI DESTINATION ${GUI_salomelib_LIBS})

FILE(GLOB COMMON_HEADERS_H "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

