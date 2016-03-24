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
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} qtx suit)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  QxScene_ViewManager.h
  QxScene_ViewModel.h
  QxScene_ViewWindow.h
)

# header files / no moc processing
SET(_other_HEADERS
  QxScene.h
  QxScene_Def.h
)

# header files / to install
SET(QxScene_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/QxSceneViewer_images.ts
  resources/QxSceneViewer_msg_en.ts
  resources/QxSceneViewer_msg_fr.ts
  resources/QxSceneViewer_msg_ja.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/qx_view_fitall.png
  resources/qx_view_fitarea.png
  resources/qx_view_glpan.png
  resources/qx_view_pan.png
  resources/qx_view_reset.png
  resources/qx_view_zoom.png
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  QxScene_ViewManager.cxx
  QxScene_ViewModel.cxx
  QxScene_ViewWindow.cxx
)

# sources / to compile
SET(QxScene_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(QxScene ${QxScene_SOURCES})
TARGET_LINK_LIBRARIES(QxScene ${_link_LIBRARIES})
INSTALL(TARGETS QxScene EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${QxScene_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
