# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

INCLUDE(UseQtExt)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${OGL_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/CASCatch
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/ViewerData
  ${PROJECT_SOURCE_DIR}/src/ViewerTools
  ${PROJECT_SOURCE_DIR}/src/OpenGLUtils
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${OGL_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES
  ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${CAS_KERNEL} ${CAS_VIEWER} ${CAS_TKGeomAlgo}
  ${CAS_TKTopAlgo} ${CAS_TKG2d} ${CAS_TKOpenGl}
  CASCatch qtx suit ViewerTools ViewerData OpenGLUtils
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  OCCViewer_AISSelector.h
  OCCViewer_AxialScaleDlg.h
  OCCViewer_ClippingDlg.h
  OCCViewer_RayTracingDlg.h
  OCCViewer_EnvTextureDlg.h
  OCCViewer_LightSourceDlg.h
  OCCViewer_CreateRestoreViewDlg.h
  OCCViewer_CubeAxesDlg.h
  OCCViewer_FontWidget.h
  OCCViewer_SetRotationPointDlg.h
  OCCViewer_ToolTip.h
  OCCViewer_ViewFrame.h
  OCCViewer_ViewManager.h
  OCCViewer_ViewModel.h
  OCCViewer_ViewPort.h
  OCCViewer_ViewPort3d.h
  OCCViewer_ViewSketcher.h
  OCCViewer_ViewWindow.h
  OCCViewer_ViewportInputFilter.h
  OCCViewer_ClipPlaneInteractor.h
)

# header files / no moc processing
SET(_other_HEADERS
  OCCViewer.h
  OCCViewer_ClipPlane.h
  OCCViewer_VService.h
  OCCViewer_Utilities.h
)

# header files / no moc processing / internal
# no processing currently: TODO: check for MSVS project
SET(_internal_HEADERS
  OCCViewer_VService.h
)

# header files / to install
SET(OCCViewer_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by rcc
SET(_rcc_RESOURCES OCCViewer.qrc)

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/OCCViewer_images.ts
  resources/OCCViewer_msg_en.ts
  resources/OCCViewer_msg_fr.ts
  resources/OCCViewer_msg_ja.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/occ_view_ambient.png
  resources/occ_view_anticlockwise.png
  resources/occ_view_back.png
  resources/occ_view_bottom.png
  resources/occ_view_camera_dump.png
  resources/occ_view_clipping.png
  resources/occ_view_clipping_pressed.png
  resources/occ_view_clockwise.png
  resources/occ_view_clone.png
  resources/occ_view_fitall.png
  resources/occ_view_fitarea.png
  resources/occ_view_fitselection.png
  resources/occ_view_front.png
  resources/occ_view_glpan.png
  resources/occ_view_graduated_axes.png
  resources/occ_view_left.png
  resources/occ_view_maximized.png
  resources/occ_view_minimized.png
  resources/occ_view_orthographic.png
  resources/occ_view_pan.png
  resources/occ_view_perspective.png
  resources/occ_view_preselection.png
  resources/occ_view_presets.png
  resources/occ_view_reset.png
  resources/occ_view_return_3d_view.png
  resources/occ_view_right.png
  resources/occ_view_rotate.png
  resources/occ_view_rotation_point.png
  resources/occ_view_scaling.png
  resources/occ_view_selection.png
  resources/occ_view_shoot.png
  resources/occ_view_stereo.png
  resources/occ_view_style_switch.png
  resources/occ_view_top.png
  resources/occ_view_triedre.png
  resources/occ_view_zoom.png
  resources/occ_view_zooming_style_switch.png
  resources/occ_view_ray_tracing.png
  resources/occ_view_env_texture.png
  resources/occ_view_light_source.png
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / rcc wrappings
QT_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

# sources / static
SET(_other_SOURCES
  OCCViewer.cxx
  OCCViewer_AISSelector.cxx
  OCCViewer_AxialScaleDlg.cxx
  OCCViewer_ClippingDlg.cxx
  OCCViewer_RayTracingDlg.cxx
  OCCViewer_EnvTextureDlg.cxx
  OCCViewer_LightSourceDlg.cxx
  OCCViewer_ClipPlane.cxx
  OCCViewer_CreateRestoreViewDlg.cxx
  OCCViewer_CubeAxesDlg.cxx
  OCCViewer_FontWidget.cxx
  OCCViewer_SetRotationPointDlg.cxx
  OCCViewer_ToolTip.cxx
  OCCViewer_VService.cxx
  OCCViewer_ViewFrame.cxx
  OCCViewer_ViewManager.cxx
  OCCViewer_ViewModel.cxx
  OCCViewer_ViewPort.cxx
  OCCViewer_ViewPort3d.cxx
  OCCViewer_ViewSketcher.cxx
  OCCViewer_ViewWindow.cxx
  OCCViewer_ViewportInputFilter.cxx
  OCCViewer_ClipPlaneInteractor.cxx
  OCCViewer_Utilities.cxx
)

# sources / to compile
SET(OCCViewer_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_rcc_SOURCES})

# --- rules ---

ADD_LIBRARY(OCCViewer ${OCCViewer_SOURCES})
TARGET_LINK_LIBRARIES(OCCViewer ${_link_LIBRARIES})
INSTALL(TARGETS OCCViewer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${OCCViewer_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
