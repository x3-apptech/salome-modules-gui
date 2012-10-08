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
  ${QWT_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
)

SET(COMMON_FLAGS "${QT_DEFINITIONS} ${QWT_DEFINITIONS} ${PYTHON_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS ${QWT_LIBS} ${QT_MT_LIBS} ${PYTHON_LIBS} suit)

SET(GUI_HEADERS   
  Plot2d_FitDataDlg.h
  Plot2d_SetupViewDlg.h
  Plot2d_ViewFrame.h
  Plot2d_ViewManager.h
  Plot2d_ViewModel.h
  Plot2d_ViewWindow.h
  Plot2d_SetupCurveDlg.h
  Plot2d_Algorithm.h
  Plot2d_NormalizeAlgorithm.h
  Plot2d_SetupCurveScaleDlg.h
  Plot2d_ToolTip.h
)

SET(Plot2d_SOURCES
  Plot2d.cxx
  Plot2d_PlotItems.cxx
  Plot2d_Object.cxx
  Plot2d_Curve.cxx
  Plot2d_Histogram.cxx
  Plot2d_FitDataDlg.cxx
  Plot2d_Prs.cxx
  Plot2d_SetupViewDlg.cxx
  Plot2d_ViewFrame.cxx
  Plot2d_ViewManager.cxx
  Plot2d_ViewModel.cxx
  Plot2d_ViewWindow.cxx
  Plot2d_Algorithm.cxx
  Plot2d_NormalizeAlgorithm.cxx
  Plot2d_SetupCurveDlg.cxx
  Plot2d_SetupCurveScaleDlg.cxx
  Plot2d_ToolTip.cxx
)

SET(GUITS_SOURCES
  resources/Plot2d_images.ts
  resources/Plot2d_msg_en.ts
  resources/Plot2d_msg_fr.ts
)

# header files 
SET(COMMON_HEADERS_H 
  Plot2d.h
  Plot2d_PlotItems.h
  Plot2d_Object.h
  Plot2d_Curve.h
  Plot2d_Histogram.h
  Plot2d_FitDataDlg.h
  Plot2d_Prs.h
  Plot2d_SetupViewDlg.h
  Plot2d_ViewFrame.h
  Plot2d_ViewManager.h
  Plot2d_ViewModel.h
  Plot2d_ViewWindow.h
  Plot2d_Algorithm.h
  Plot2d_NormalizeAlgorithm.h
  Plot2d_SetupCurveDlg.h
  Plot2d_ToolTip.h
  Plot2d_SetupCurveScaleDlg.h
)

IF(ENABLE_PYCONSOLE)
  SET(COMMON_HEADERS_H
      ${COMMON_HEADERS_H}
      Plot2d_AnalyticalCurve.h
      Plot2d_AnalyticalCurveDlg.h
      Plot2d_AnalyticalParser.h
  )
  SET(Plot2d_SOURCES
      ${Plot2d_SOURCES}
      Plot2d_AnalyticalCurve.cxx
      Plot2d_AnalyticalCurveDlg.cxx
      Plot2d_AnalyticalParser.cxx
  )
  SET(GUI_HEADERS
      ${GUI_HEADERS}
      Plot2d_AnalyticalCurveDlg.h
  )
ENDIF(ENABLE_PYCONSOLE)

QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

ADD_LIBRARY(Plot2d SHARED ${Plot2d_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(Plot2d PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(Plot2d ${COMMON_LIBS})
INSTALL(TARGETS Plot2d DESTINATION ${GUI_salomelib_LIBS})

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${GUI_salomeres_DATA})
