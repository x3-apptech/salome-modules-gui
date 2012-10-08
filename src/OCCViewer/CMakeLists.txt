# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
  ${OGL_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../ViewerTools
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../OpenGLUtils
  ${CMAKE_CURRENT_SOURCE_DIR}/../CASCatch
)
SET(COMMON_FLAGS "${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${OGL_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS
  qtx
  suit
  ViewerTools
  OpenGLUtils
  ${OGL_LIBS}
  ${QT_MT_LIBS}
  ${CAS_KERNEL}
  ${CAS_VIEWER}
)

SET(GUI_HEADERS 
  OCCViewer_AISSelector.h
  OCCViewer_ViewModel.h
  OCCViewer_ViewPort3d.h
  OCCViewer_ViewPort.h
  OCCViewer_ViewSketcher.h
  OCCViewer_ViewWindow.h
  OCCViewer_ViewManager.h
  OCCViewer_CreateRestoreViewDlg.h
  OCCViewer_SetRotationPointDlg.h
  OCCViewer_ClippingDlg.h
  OCCViewer_AxialScaleDlg.h
  OCCViewer_FontWidget.h
  OCCViewer_CubeAxesDlg.h
  OCCViewer_ToolTip.h
  OCCViewer_ViewFrame.h
)
QT4_WRAP_CPP(GUI_HEADERS ${GUI_HEADERS})

SET(OCCViewer_SOURCES
  OCCViewer_AISSelector.cxx
  OCCViewer_ViewManager.cxx
  OCCViewer_ViewModel.cxx
  OCCViewer_ViewPort3d.cxx
  OCCViewer_ViewPort.cxx
  OCCViewer_ViewSketcher.cxx
  OCCViewer_ViewWindow.cxx
  OCCViewer_VService.cxx
  OCCViewer_CreateRestoreViewDlg.cxx
  OCCViewer_SetRotationPointDlg.cxx
  OCCViewer_ClippingDlg.cxx
  OCCViewer_AxialScaleDlg.cxx
  OCCViewer_Trihedron.cxx
  OCCViewer_FontWidget.cxx
  OCCViewer_CubeAxesDlg.cxx
  OCCViewer_ToolTip.cxx
  OCCViewer_ViewFrame.cxx
)

SET(GUITS_SOURCES
  resources/OCCViewer_images.ts
  resources/OCCViewer_msg_en.ts
  resources/OCCViewer_msg_fr.ts
)

ADD_LIBRARY(OCCViewer SHARED ${OCCViewer_SOURCES} ${GUI_HEADERS})
SET_TARGET_PROPERTIES(OCCViewer PROPERTIES COMPILE_FLAGS ${COMMON_FLAGS})
TARGET_LINK_LIBRARIES(OCCViewer ${COMMON_LIBS})
INSTALL(TARGETS OCCViewer DESTINATION ${GUI_salomelib_LIBS})
SET(COMMON_HEADERS_H
  OCCViewer_AISSelector.h
  OCCViewer_ViewManager.h
  OCCViewer_ViewModel.h
  OCCViewer_ViewPort3d.h
  OCCViewer_ViewPort.h
  OCCViewer_ViewSketcher.h
  OCCViewer_ViewWindow.h
  OCCViewer_VService.h
  OCCViewer_CreateRestoreViewDlg.h
  OCCViewer.h
  OCCViewer_ClippingDlg.h	
  OCCViewer_AxialScaleDlg.h
  OCCViewer_SetRotationPointDlg.h
  OCCViewer_Trihedron.h 
  OCCViewer_FontWidget.h
  OCCViewer_CubeAxesDlg.h
  OCCViewer_ToolTip.h
  OCCViewer_ViewFrame.h
)
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${GUI_salomeres_DATA})
