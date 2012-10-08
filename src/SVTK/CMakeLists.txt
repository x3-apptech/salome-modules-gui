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
  ${VTK_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../ViewerTools
  ${CMAKE_CURRENT_SOURCE_DIR}/../OBJECT
  ${CMAKE_CURRENT_SOURCE_DIR}/../Prs
  ${CMAKE_CURRENT_SOURCE_DIR}/../VTKViewer
  ${CMAKE_CURRENT_SOURCE_DIR}/../OpenGLUtils
)
SET(COMMON_FLAGS "${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${VTK_DEFINITIONS} ${BOOST_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS
  qtx
  suit
  ViewerTools
  SalomeObject
  SalomePrs
  VTKViewer
  OpenGLUtils
  ${SALOMELocalTrace}
  ${OpUtil}
  ${VTK_LIBS}
  ${OGL_LIBS}
  ${QT_MT_LIBS}
  ${CAS_KERNEL}
  ${CAS_VIEWER}
)

SET(GUI_HEADERS SVTK_GenericRenderWindowInteractor.h SVTK_RenderWindowInteractor.h SVTK_NonIsometricDlg.h 
  SVTK_UpdateRateDlg.h SVTK_CubeAxesDlg.h SVTK_FontWidget.h SVTK_ViewModelBase.h SVTK_ViewManager.h 
  SVTK_ViewWindow.h SVTK_ViewModel.h SVTK_View.h SVTK_SetRotationPointDlg.h SVTK_ViewParameterDlg.h
  SVTK_ComboAction.h SVTK_RecorderDlg.h)
QT4_WRAP_CPP(GUI_HEADERS ${GUI_HEADERS})

SET(SVTK_SOURCES
  SVTK_Prs.cxx
  SVTK_Actor.cxx
  SALOME_Actor.cxx
  SVTK_RectPicker.cxx
  SVTK_DeviceActor.cxx
  SVTK_CubeAxesActor2D.cxx
  SVTK_NonIsometricDlg.cxx
  SVTK_UpdateRateDlg.cxx
  SVTK_CubeAxesDlg.cxx
  SVTK_FontWidget.cxx
  SVTK_Trihedron.cxx
  SVTK_View.cxx
  SVTK_ViewManager.cxx
  SVTK_ViewModel.cxx
  SVTK_Renderer.cxx
  SVTK_ViewWindow.cxx
  SVTK_InteractorStyle.cxx
  SVTK_KeyFreeInteractorStyle.cxx
  SVTK_RenderWindowInteractor.cxx
  SVTK_GenericRenderWindowInteractor.cxx
  SVTK_SpaceMouse.cxx
  SVTK_Selector.cxx
  SVTK_SetRotationPointDlg.cxx
  SVTK_ViewParameterDlg.cxx
  SVTK_ComboAction.cxx
  SVTK_Recorder.cxx
  SVTK_RecorderDlg.cxx
  SVTK_ImageWriter.cxx
  SVTK_ImageWriterMgr.cxx
  SVTK_Utils.cxx
  vtkPVAxesActor.h
  vtkPVAxesActor.cxx
  vtkPVAxesWidget.h
  vtkPVAxesWidget.cxx
)

SET(GUITS_SOURCES
  resources/SVTK_msg_en.ts
  resources/SVTK_msg_fr.ts
  resources/SVTK_images.ts
)

ADD_LIBRARY(SVTK SHARED ${SVTK_SOURCES} ${GUI_HEADERS})
SET_TARGET_PROPERTIES(SVTK PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(SVTK ${COMMON_LIBS})
INSTALL(TARGETS SVTK DESTINATION ${GUI_salomelib_LIBS})

#ADD_EXECUTABLE(TestSVTK SVTK.cxx)
#SET_TARGET_PROPERTIES(TestSVTK PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}" OUTPUT_NAME "SVTK")
#TARGET_LINK_LIBRARIES(TestSVTK SVTK ${COMMON_LIBS})
#INSTALL(TARGETS TestSVTK DESTINATION ${GUI_salomebin_BINS})

SET(COMMON_HEADERS_H 
  SVTK.h
  SVTK_Prs.h
  SVTK_Actor.h
  SALOME_Actor.h
  SVTK_RectPicker.h
  SVTK_DeviceActor.h
  SVTK_FontWidget.h
  SVTK_CubeAxesActor2D.h
  SVTK_Functor.h
  SVTK_View.h
  SVTK_ViewManager.h
  SVTK_ViewModel.h
  SVTK_ViewWindow.h
  SVTK_Renderer.h
  SVTK_InteractorStyle.h
  SVTK_KeyFreeInteractorStyle.h
  SVTK_RenderWindowInteractor.h
  SVTK_GenericRenderWindowInteractor.h
  SVTK_Selector.h
  SVTK_Selection.h
  SVTK_SelectionEvent.h
  SVTK_SpaceMouse.h
  SVTK_Event.h
  SVTK_ViewModelBase.h
  SVTK_SetRotationPointDlg.h
  SVTK_ViewParameterDlg.h
  SVTK_ComboAction.h
  SVTK_Recorder.h
  SVTK_RecorderDlg.h
  SVTK_ImageWriter.h
  SVTK_ImageWriterMgr.h
  SVTK_Utils.h
)
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${GUI_salomeres_DATA})

