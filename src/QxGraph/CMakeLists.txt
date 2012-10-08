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
  ${BOOST_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
)

SET(COMMON_FLAGS "${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS} ${PYTHON_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS ${QT_MT_LIBS} suit)

SET(GUI_HEADERS   
  QxGraph_Canvas.h
  QxGraph_CanvasView.h
  QxGraph_ViewWindow.h
  QxGraph_ViewManager.h
  QxGraph_ViewModel.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(QxGraph_SOURCES
  QxGraph_Prs.cxx
  QxGraph_Canvas.cxx
  QxGraph_CanvasView.cxx
  QxGraph_ViewWindow.cxx
  QxGraph_ViewManager.cxx
  QxGraph_ViewModel.cxx
)

SET(GUITS_SOURCES
  resources/QxGraph_images.ts
  resources/QxGraph_msg_en.ts
  resources/QxGraph_msg_fr.ts
)

ADD_LIBRARY(QxGraph SHARED ${QxGraph_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(QxGraph PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(QxGraph ${COMMON_LIBS})
INSTALL(TARGETS QxGraph DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  QxGraph_ActiveItem.h
  QxGraph_Prs.h
  QxGraph_Canvas.h
  QxGraph_CanvasView.h
  QxGraph_ViewWindow.h
  QxGraph_ViewManager.h
  QxGraph_ViewModel.h
  QxGraph_Def.h
  QxGraph.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${GUI_salomeres_DATA})
