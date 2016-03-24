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

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${CAS_DEFINITIONS} ${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${CAS_KERNEL} ${OPENGL_LIBRARIES} qtx suit)
IF(NOT WIN32)
  LIST(APPEND _link_LIBRARIES Xmu)
ENDIF()

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  GLViewer_Selector.h
  GLViewer_Selector2d.h
  GLViewer_ToolTip.h
  GLViewer_ViewFrame.h
  GLViewer_ViewManager.h
  GLViewer_ViewPort.h
  GLViewer_ViewPort2d.h
  GLViewer_Viewer.h
  GLViewer_Viewer2d.h
  GLViewer_Widget.h
)

# header files / no moc processing
SET(_other_HEADERS
  GLViewer.h
  GLViewer_AspectLine.h
  GLViewer_BaseDrawers.h
  GLViewer_BaseObjects.h
  GLViewer_Compass.h
  GLViewer_Context.h
  GLViewer_CoordSystem.h
  GLViewer_Defs.h
  GLViewer_Drawer.h
  GLViewer_Geom.h
  GLViewer_Grid.h
  GLViewer_Group.h
  GLViewer_MimeData.h
  GLViewer_Object.h
  GLViewer_Text.h
  GLViewer_Tools.h
)

# header files / to install
SET(GLViewer_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/GLViewer_images.ts
  resources/GLViewer_msg_en.ts
  resources/GLViewer_msg_fr.ts
  resources/GLViewer_msg_ja.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/gl_cursor_rotate.png
  resources/gl_cursor_sketch.png
  resources/gl_cursor_zoom.png
  resources/gl_view_dump.png
  resources/gl_view_fitall.png
  resources/gl_view_fitarea.png
  resources/gl_view_fitselect.png
  resources/gl_view_glpan.png
  resources/gl_view_pan.png
  resources/gl_view_reset.png
  resources/gl_view_zoom.png
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
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
  GLViewer_ToolTip.cxx
  GLViewer_Tools.cxx
  GLViewer_ViewFrame.cxx
  GLViewer_ViewManager.cxx
  GLViewer_ViewPort.cxx
  GLViewer_ViewPort2d.cxx
  GLViewer_Viewer.cxx
  GLViewer_Viewer2d.cxx
  GLViewer_Widget.cxx
)

# sources / to compile
SET(GLViewer_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(GLViewer ${GLViewer_SOURCES})
TARGET_LINK_LIBRARIES(GLViewer ${_link_LIBRARIES})
INSTALL(TARGETS GLViewer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${GLViewer_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})

QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
