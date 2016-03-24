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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${QWT_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${QWT_DEFINITIONS} ${PYTHON_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${QWT_LIBRARY} ${PYTHON_LIBRARIES} qtx suit)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  Plot2d_Algorithm.h
  Plot2d_FitDataDlg.h
  Plot2d_NormalizeAlgorithm.h
  Plot2d_SetupCurveDlg.h
  Plot2d_SetupCurveScaleDlg.h
  Plot2d_SetupViewDlg.h
  Plot2d_ToolTip.h
  Plot2d_ViewFrame.h
  Plot2d_ViewManager.h
  Plot2d_ViewModel.h
  Plot2d_ViewWindow.h
  Plot2d_AnalyticalCurveDlg.h
  )

# header files / no moc processing
SET(_other_HEADERS
  Plot2d.h
  Plot2d_Curve.h
  Plot2d_Histogram.h
  Plot2d_Object.h
  Plot2d_PlotItems.h
  Plot2d_Prs.h
  Plot2d_AnalyticalCurve.h
  Plot2d_AnalyticalParser.h
  )

# header files / to install
SET(Plot2d_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/Plot2d_images.ts
  resources/Plot2d_msg_en.ts
  resources/Plot2d_msg_fr.ts
  resources/Plot2d_msg_ja.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/plot2d_analytical_curve.png
  resources/plot2d_camera_dump.png
  resources/plot2d_clone.png
  resources/plot2d_fitall.png
  resources/plot2d_fitarea.png
  resources/plot2d_glpan.png
  resources/plot2d_legend.png
  resources/plot2d_linear.png
  resources/plot2d_linear_y.png
  resources/plot2d_lines.png
  resources/plot2d_lmax_normalization.png
  resources/plot2d_lmin_normalization.png
  resources/plot2d_log.png
  resources/plot2d_log_y.png
  resources/plot2d_pan.png
  resources/plot2d_points.png
  resources/plot2d_print.png
  resources/plot2d_rmax_normalization.png
  resources/plot2d_rmin_normalization.png
  resources/plot2d_settings.png
  resources/plot2d_splines.png
  resources/plot2d_zoom.png
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  Plot2d.cxx
  Plot2d_Algorithm.cxx
  Plot2d_Curve.cxx
  Plot2d_FitDataDlg.cxx
  Plot2d_Histogram.cxx
  Plot2d_NormalizeAlgorithm.cxx
  Plot2d_Object.cxx
  Plot2d_PlotItems.cxx
  Plot2d_Prs.cxx
  Plot2d_SetupCurveDlg.cxx
  Plot2d_SetupCurveScaleDlg.cxx
  Plot2d_SetupViewDlg.cxx
  Plot2d_ToolTip.cxx
  Plot2d_ViewFrame.cxx
  Plot2d_ViewManager.cxx
  Plot2d_ViewModel.cxx
  Plot2d_ViewWindow.cxx
  Plot2d_AnalyticalCurve.cxx
  Plot2d_AnalyticalCurveDlg.cxx
  Plot2d_AnalyticalParser.cxx
  )

# sources / to compile
SET(Plot2d_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(Plot2d ${Plot2d_SOURCES})
TARGET_LINK_LIBRARIES(Plot2d ${_link_LIBRARIES})
INSTALL(TARGETS Plot2d EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${Plot2d_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
