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
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
)

SET(COMMON_LIBS
  suit 
  ${QT_MT_LIBS} 
  ${CAS_KERNEL} 
  ${OGL_LIBS}
)

SET(GUI_HEADERS GLViewer_Selector.h GLViewer_Selector2d.h GLViewer_ToolTip.h GLViewer_ViewFrame.h GLViewer_ViewManager.h GLViewer_ViewPort.h GLViewer_ViewPort2d.h GLViewer_Viewer.h GLViewer_Viewer2d.h GLViewer_Widget.h)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(GLViewer_SOURCES
  GLViewer_AspectLine.cxx
  GLViewer_BaseDrawers.cxx
  GLViewer_BaseObjects.cxx
  GLViewer_Compass.cxx
  GLViewer_Context.cxx
  GLViewer_CoordSystem.cxx
  GLViewer_Drawer.cxx
  GLViewer_Geom.cxx
  GLViewer_Grid.cxx
  GLViewer_Group.cxx
  GLViewer_MimeData.cxx
  GLViewer_Object.cxx
  GLViewer_Selector.cxx
  GLViewer_Selector2d.cxx
  GLViewer_Text.cxx
  GLViewer_Tools.cxx
  GLViewer_ToolTip.cxx
  GLViewer_ViewFrame.cxx
  GLViewer_ViewManager.cxx
  GLViewer_ViewPort.cxx
  GLViewer_ViewPort2d.cxx
  GLViewer_Viewer.cxx
  GLViewer_Viewer2d.cxx
  GLViewer_Widget.cxx
)

SET(GUITS_SOURCES
  resources/GLViewer_images.ts
  resources/GLViewer_msg_en.ts
  resources/GLViewer_msg_fr.ts
)

ADD_LIBRARY(GLViewer SHARED ${GLViewer_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(GLViewer PROPERTIES COMPILE_FLAGS "${CAS_DEFINITIONS} ${QT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(GLViewer ${COMMON_LIBS})
INSTALL(TARGETS GLViewer DESTINATION ${GUI_salomelib_LIBS})

FILE(GLOB COMMON_HEADERS_H "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${GUI_salomeres_DATA})
