# Copyright (C) 2015-2016  OPEN CASCADE
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
# Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
#

INCLUDE(UseQt4Ext)

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/PyEditor
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${PLATFORM_LIBS} ${QT_LIBRARIES} qtx suit PyEditor)

# header files / to be processed by moc
SET(_moc_HEADERS
  PyViewer_ViewManager.h
  PyViewer_ViewModel.h
  PyViewer_ViewWindow.h
)

# header files / no moc processing
SET(_other_HEADERS
  PyViewer.h
)

# header files / to install
SET(PyViewer_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# resource files / to be processed by lrelease
SET(RESOURCES_PATH resources)

SET(_ts_RESOURCES
  ${RESOURCES_PATH}/translations/PyViewer_msg_en.ts
  ${RESOURCES_PATH}/translations/PyViewer_msg_fr.ts
  ${RESOURCES_PATH}/translations/PyViewer_msg_ja.ts
)

# resource files / to be processed by rcc
SET(_rcc_RESOURCES ${RESOURCES_PATH}/PyEditor.qrc)

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

# sources / rcc wrappings
QT4_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

# sources / static
SET(_other_SOURCES
  PyViewer_ViewManager.cxx
  PyViewer_ViewModel.cxx
  PyViewer_ViewWindow.cxx
)

# sources / to compile
SET(PyViewer_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---
ADD_LIBRARY(PyViewer ${PyViewer_SOURCES} ${_rcc_SOURCES})
TARGET_LINK_LIBRARIES(PyViewer ${_link_LIBRARIES})
INSTALL(TARGETS PyViewer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

ADD_EXECUTABLE(DummyPyEditor PyViewer.cxx)
SET_TARGET_PROPERTIES(DummyPyEditor PROPERTIES OUTPUT_NAME "PyEditor")
TARGET_LINK_LIBRARIES(DummyPyEditor ${_link_LIBRARIES} PyEditor PyViewer)
INSTALL(TARGETS DummyPyEditor EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

INSTALL(FILES ${PyViewer_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT4_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
