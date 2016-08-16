# Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

INCLUDE(${VTK_USE_FILE})
INCLUDE(UseQtExt)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${CAS_DEFINITIONS} ${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${VTK_LIBRARIES} ${CAS_KERNEL} qtx suit ${KERNEL_SALOMELocalTrace})

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  VTKViewer_MarkerDlg.h
  VTKViewer_MarkerWidget.h
  VTKViewer_RenderWindow.h
  VTKViewer_RenderWindowInteractor.h
  VTKViewer_ViewManager.h
  VTKViewer_ViewModel.h
  VTKViewer_ViewWindow.h
)

# header files / no moc processing
SET(_other_HEADERS
  VTKViewer.h
  VTKViewer_Actor.h
  VTKViewer_AppendFilter.h
  VTKViewer_Algorithm.h
  VTKViewer_ArcBuilder.h
  VTKViewer_CellCenters.h
  VTKViewer_CellLocationsArray.h
  VTKViewer_ConvexTool.h
  VTKViewer_DataSetMapper.h
  VTKViewer_ExtractUnstructuredGrid.h
  VTKViewer_Filter.h
  VTKViewer_FramedTextActor.h
  VTKViewer_Functor.h
  VTKViewer_GeometryFilter.h
  VTKViewer_InteractorStyle.h
  VTKViewer_MarkerDef.h
  VTKViewer_MarkerUtils.h
  VTKViewer_OpenGLHelper.h
  VTKViewer_OpenGLRenderer.h
  VTKViewer_PolyDataMapper.h
  VTKViewer_ShrinkFilter.h
  VTKViewer_Texture.h
  VTKViewer_Transform.h
  VTKViewer_TransformFilter.h
  VTKViewer_Trihedron.h
  VTKViewer_Utilities.h
)

# header files / to install
SET(VTKViewer_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by rcc
SET(_rcc_RESOURCES VTKViewer.qrc)

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/VTKViewer_images.ts
  resources/VTKViewer_msg_en.ts
  resources/VTKViewer_msg_fr.ts
  resources/VTKViewer_msg_ja.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/vtk_view_anticlockwise.png
  resources/vtk_view_clockwise.png
  resources/vtk_view_back.png
  resources/vtk_view_bottom.png
  resources/vtk_view_camera_dump.png
  resources/vtk_view_fitall.png
  resources/vtk_view_fitarea.png
  resources/vtk_view_fitselection.png
  resources/vtk_view_front.png
  resources/vtk_view_glpan.png
  resources/vtk_view_left.png
  resources/vtk_view_pan.png
  resources/vtk_view_reset.png
  resources/vtk_view_right.png
  resources/vtk_view_rotate.png
  resources/vtk_view_top.png
  resources/vtk_view_triedre.png
  resources/vtk_view_zoom.png
  resources/marker_1.png
  resources/marker_2.png
  resources/marker_3.png
  resources/marker_4.png
  resources/marker_5.png
  resources/marker_6.png
  resources/marker_7.png
  resources/marker_8.png
  resources/marker_9.png
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / rcc wrappings
QT_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

# sources / static
SET(_other_SOURCES
  VTKViewer_Actor.cxx
  VTKViewer_Algorithm.cxx
  VTKViewer_AppendFilter.cxx
  VTKViewer_ArcBuilder.cxx
  VTKViewer_CellCenters.cxx
  VTKViewer_CellLocationsArray.cxx
  VTKViewer_ConvexTool.cxx
  VTKViewer_DataSetMapper.cxx
  VTKViewer_ExtractUnstructuredGrid.cxx
  VTKViewer_Filter.cxx
  VTKViewer_FramedTextActor.cxx
  VTKViewer_GeometryFilter.cxx
  VTKViewer_InteractorStyle.cxx
  VTKViewer_MarkerDlg.cxx
  VTKViewer_MarkerUtils.cxx
  VTKViewer_MarkerWidget.cxx
  VTKViewer_OpenGLHelper.cxx
  VTKViewer_OpenGLRenderer.cxx
  VTKViewer_PolyDataMapper.cxx
  VTKViewer_RenderWindow.cxx
  VTKViewer_RenderWindowInteractor.cxx
  VTKViewer_ShrinkFilter.cxx
  VTKViewer_Texture.cxx
  VTKViewer_Transform.cxx
  VTKViewer_TransformFilter.cxx
  VTKViewer_Trihedron.cxx
  VTKViewer_Utilities.cxx
  VTKViewer_ViewManager.cxx
  VTKViewer_ViewModel.cxx
  VTKViewer_ViewWindow.cxx
)

# sources / to compile
SET(VTKViewer_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_rcc_SOURCES})

# --- rules ---

ADD_LIBRARY(VTKViewer ${VTKViewer_SOURCES})
TARGET_LINK_LIBRARIES(VTKViewer ${_link_LIBRARIES})
INSTALL(TARGETS VTKViewer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${VTKViewer_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})

FILE(GLOB GUIVTI_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.vti")
INSTALL(FILES ${GUIVTI_DATA} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})

FILE(GLOB GUITXT_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.txt")
INSTALL(FILES ${GUITXT_DATA} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})

FILE(GLOB GUIGLSL_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.glsl")
INSTALL(FILES ${GUIGLSL_DATA} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
