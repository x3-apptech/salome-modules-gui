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
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/ImageComposer
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} qtx suit ImageComposer)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  GraphicsView_Scene.h
  GraphicsView_Selector.h
  GraphicsView_ViewFrame.h
  GraphicsView_ViewManager.h
  GraphicsView_ViewPort.h
  GraphicsView_Viewer.h
)

# header files / no moc processing
SET(_other_HEADERS
  GraphicsView.h
  GraphicsView_Defs.h
  GraphicsView_Object.h
  GraphicsView_ViewTransformer.h
)

# header files / to install
SET(GraphicsView_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/GraphicsView_images.ts
  resources/GraphicsView_msg_en.ts
  resources/GraphicsView_msg_fr.ts
  resources/GraphicsView_msg_ja.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/graphics_view_cursor_zoom.png
  resources/graphics_view_dump.png
  resources/graphics_view_fitall.png
  resources/graphics_view_fitarea.png
  resources/graphics_view_fitselect.png
  resources/graphics_view_glpan.png
  resources/graphics_view_pan.png
  resources/graphics_view_reset.png
  resources/graphics_view_rotate.png
  resources/graphics_view_zoom.png
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  GraphicsView_Object.cxx
  GraphicsView_Scene.cxx
  GraphicsView_Selector.cxx
  GraphicsView_ViewFrame.cxx
  GraphicsView_ViewManager.cxx
  GraphicsView_ViewPort.cxx
  GraphicsView_ViewTransformer.cxx
  GraphicsView_Viewer.cxx
)

# sources / to compile
SET(GraphicsView_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(GraphicsView ${GraphicsView_SOURCES})
TARGET_LINK_LIBRARIES(GraphicsView ${_link_LIBRARIES})
INSTALL(TARGETS GraphicsView EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${GraphicsView_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
