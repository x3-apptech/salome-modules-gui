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

INCLUDE(UseQtExt)
INCLUDE(${VTK_USE_FILE})

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${Boost_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/ViewerTools
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/Prs
  ${PROJECT_SOURCE_DIR}/src/VTKViewer
  ${PROJECT_SOURCE_DIR}/src/OpenGLUtils
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES 
  ${QT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${VTK_LIBRARIES}
  ${CAS_KERNEL} ${CAS_VIEWER}
  ${KERNEL_OpUtil}
  qtx suit ViewerTools SalomeObject SalomePrs VTKViewer OpenGLUtils
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
   SVTK_ComboAction.h
   SVTK_FontWidget.h
   SVTK_GenericRenderWindowInteractor.h
   SVTK_RecorderDlg.h
   SVTK_PsOptionsDlg.h	
   SVTK_RenderWindowInteractor.h
   SVTK_SetRotationPointDlg.h
   SVTK_View.h
   SVTK_ViewManager.h
   SVTK_ViewModelBase.h
   SVTK_ViewModel.h
   SVTK_ViewParameterDlg.h
   SVTK_ViewWindow.h
)

# header files / to be processed by moc / internal
# TODO: check for MSVS project
SET(_moc_internal_HEADERS
  SVTK_CubeAxesDlg.h
  SVTK_NonIsometricDlg.h
  SVTK_UpdateRateDlg.h
)

# header files / no moc processing
SET(_other_HEADERS
   SALOME_Actor.h
   SVTK.h
   SVTK_Actor.h
   SVTK_AreaPicker.h
   SVTK_CubeAxesActor2D.h
   SVTK_DeviceActor.h
   SVTK_Event.h
   SVTK_Functor.h
   SVTK_ImageWriter.h
   SVTK_ImageWriterMgr.h
   SVTK_InteractorStyle.h
   SVTK_KeyFreeInteractorStyle.h
   SVTK_Prs.h
   SVTK_Recorder.h
   SVTK_Renderer.h
   SVTK_Selection.h
   SVTK_SelectionEvent.h
   SVTK_Selector.h
   SVTK_SpaceMouse.h
   SVTK_Utils.h
)

# header files / no moc processing / internal
# no processing currently: TODO: check for MSVS project
SET(_internal_HEADERS
  SVTK_SelectorDef.h
  SVTK_Trihedron.h
  salomevtkPVAxesActor.h
  salomevtkPVAxesWidget.h
)

# header files / to install
SET(SVTK_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/SVTK_msg_en.ts
  resources/SVTK_msg_fr.ts
  resources/SVTK_msg_ja.ts
  resources/SVTK_images.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/vtk_view_graduated_axes.png
  resources/vtk_view_highlight.png
  resources/vtk_view_highlight_dyn.png
  resources/vtk_view_highlight_off.png
  resources/vtk_view_highlight_std.png
  resources/vtk_view_parallel.png
  resources/vtk_view_parameters.png
  resources/vtk_view_perspective.png
  resources/vtk_view_recording_pause.png
  resources/vtk_view_recording_play.png
  resources/vtk_view_recording_start.png
  resources/vtk_view_recording_stop.png
  resources/vtk_view_rotation_point.png
  resources/vtk_view_scaling.png
  resources/vtk_view_selection.png
  resources/vtk_view_stereo.png
  resources/vtk_view_style_switch.png
  resources/vtk_view_update_rate.png
  resources/vtk_view_zooming_style_switch.png
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS} ${_moc_internal_HEADERS})

# sources / static
SET(_other_SOURCES
  SALOME_Actor.cxx
  SVTK_Actor.cxx
  SVTK_AreaPicker.cxx
  SVTK_ComboAction.cxx
  SVTK_CubeAxesActor2D.cxx
  SVTK_CubeAxesDlg.cxx
  SVTK_DeviceActor.cxx
  SVTK_FontWidget.cxx
  SVTK_GenericRenderWindowInteractor.cxx
  SVTK_ImageWriter.cxx
  SVTK_ImageWriterMgr.cxx
  SVTK_InteractorStyle.cxx
  SVTK_KeyFreeInteractorStyle.cxx
  SVTK_NonIsometricDlg.cxx
  SVTK_Prs.cxx
  SVTK_Recorder.cxx
  SVTK_RecorderDlg.cxx
  SVTK_PsOptionsDlg.cxx
  SVTK_RenderWindowInteractor.cxx
  SVTK_Renderer.cxx
  SVTK_Selector.cxx
  SVTK_SetRotationPointDlg.cxx
  SVTK_SpaceMouse.cxx
  SVTK_Trihedron.cxx
  SVTK_UpdateRateDlg.cxx
  SVTK_Utils.cxx
  SVTK_View.cxx
  SVTK_ViewManager.cxx
  SVTK_ViewModel.cxx
  SVTK_ViewParameterDlg.cxx
  SVTK_ViewWindow.cxx
  salomevtkPVAxesActor.cxx
  salomevtkPVAxesWidget.cxx
)

# sources / to compile
SET(SVTK_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(SVTK ${SVTK_SOURCES})
TARGET_LINK_LIBRARIES(SVTK ${_link_LIBRARIES})
INSTALL(TARGETS SVTK EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SVTK_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
