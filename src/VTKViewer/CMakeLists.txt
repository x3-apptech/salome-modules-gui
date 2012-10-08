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
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
)

SET(COMMON_FLAGS "${VTK_DEFINITIONS} ${CAS_DEFINITIONS} ${QT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS ${QT_MT_LIBS} ${OGL_LIBS} ${VTK_LIBS} ${CAS_KERNEL} qtx suit ${SALOMELocalTrace})

SET(GUI_HEADERS   
  VTKViewer_RenderWindow.h
  VTKViewer_RenderWindowInteractor.h
  VTKViewer_ViewManager.h
  VTKViewer_ViewModel.h
  VTKViewer_ViewWindow.h
  VTKViewer_MarkerWidget.h
  VTKViewer_MarkerDlg.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(RCCS VTKViewer.qrc)
QT4_ADD_RESOURCES(RCC_SRCS ${RCCS})

SET(VTKViewer_SOURCES
  VTKViewer_CellLocationsArray.cxx
  VTKViewer_Actor.cxx
  VTKViewer_FramedTextActor.cxx
  VTKViewer_ExtractUnstructuredGrid.cxx
  VTKViewer_Filter.cxx
  VTKViewer_GeometryFilter.cxx
  VTKViewer_AppendFilter.cxx
  VTKViewer_Algorithm.cxx
  VTKViewer_InteractorStyle.cxx
  VTKViewer_RenderWindow.cxx
  VTKViewer_RenderWindowInteractor.cxx
  VTKViewer_ShrinkFilter.cxx
  VTKViewer_Transform.cxx
  VTKViewer_TransformFilter.cxx
  VTKViewer_Trihedron.cxx
  VTKViewer_Utilities.cxx
  VTKViewer_ViewManager.cxx
  VTKViewer_ViewModel.cxx
  VTKViewer_ConvexTool.cxx
  VTKViewer_ViewWindow.cxx
  VTKViewer_ArcBuilder.cxx
  VTKViewer_MarkerUtils.cxx
  VTKViewer_MarkerWidget.cxx
  VTKViewer_MarkerDlg.cxx
  VTKViewer_PolyDataMapper.cxx
  VTKViewer_DataSetMapper.cxx
  VTKViewer_Texture.cxx
  VTKViewer_OpenGLRenderer.cxx
  VTKViewer_CellCenters.cxx
)

SET(GUITS_SOURCES
  resources/VTKViewer_images.ts
  resources/VTKViewer_msg_en.ts
  resources/VTKViewer_msg_fr.ts
)

ADD_LIBRARY(VTKViewer SHARED ${VTKViewer_SOURCES} ${GUI_HEADERS_MOC} ${RCC_SRCS})
SET_TARGET_PROPERTIES(VTKViewer PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(VTKViewer ${COMMON_LIBS})
INSTALL(TARGETS VTKViewer DESTINATION ${GUI_salomelib_LIBS})

#ADD_EXECUTABLE(TestVTKViewer VTKViewer.cxx)
#SET_TARGET_PROPERTIES(TestVTKViewer PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}"  OUTPUT_NAME "VTKViewer")
#TARGET_LINK_LIBRARIES(TestVTKViewer VTKViewer ${COMMON_LIBS})
#INSTALL(TARGETS TestVTKViewer DESTINATION ${GUI_salomebin_BINS})

SET(COMMON_HEADERS_H 
  VTKViewer.h
  VTKViewer_CellLocationsArray.h
  VTKViewer_Actor.h
  VTKViewer_FramedTextActor.h
  VTKViewer_ExtractUnstructuredGrid.h
  VTKViewer_ConvexTool.h
  VTKViewer_Filter.h
  VTKViewer_GeometryFilter.h
  VTKViewer_AppendFilter.h
  VTKViewer_Algorithm.h
  VTKViewer_InteractorStyle.h
  VTKViewer_RenderWindow.h
  VTKViewer_RenderWindowInteractor.h
  VTKViewer_ShrinkFilter.h
  VTKViewer_TransformFilter.h
  VTKViewer_Transform.h
  VTKViewer_Trihedron.h
  VTKViewer_Utilities.h
  VTKViewer_ViewManager.h
  VTKViewer_ViewModel.h
  VTKViewer_ViewWindow.h
  VTKViewer_Functor.h
  VTKViewer_ArcBuilder.h
  VTKViewer_MarkerDef.h
  VTKViewer_MarkerUtils.h
  VTKViewer_MarkerWidget.h
  VTKViewer_MarkerDlg.h
  VTKViewer_PolyDataMapper.h
  VTKViewer_DataSetMapper.h
  VTKViewer_Texture.h
  VTKViewer_OpenGLRenderer.h
  VTKViewer_CellCenters.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${GUI_salomeres_DATA})
