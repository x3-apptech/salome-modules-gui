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

INCLUDE(UseQtExt)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${PLATFORM_LIBS} ${QT_LIBRARIES} qtx suit PyEditor)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  PyViewer_ViewManager.h
  PyViewer_ViewModel.h
  PyViewer_ViewWindow.h
)

# header files / no moc processing
SET(_other_HEADERS
  PyViewer.h
  PyViewer_Settings.h
)

# header files / to install
SET(PyViewer_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

SET(_ts_RESOURCES
  resources/PyViewer_msg_en.ts
  resources/PyViewer_msg_fr.ts
  resources/PyViewer_msg_ja.ts
  resources/PyViewer_images.ts
)

# resource files / static
SET(_other_RESOURCES
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_copy.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_cut.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_delete.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_help.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_new.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_open.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_paste.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_preferences.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_redo.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_save.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_save_as.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_select_all.png
  ${PROJECT_SOURCE_DIR}/tools/PyEditor/src/resources/images/py_undo.png
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  PyViewer_Settings.cxx
  PyViewer_ViewManager.cxx
  PyViewer_ViewModel.cxx
  PyViewer_ViewWindow.cxx
)

# sources / to compile
SET(PyViewer_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(PyViewer ${PyViewer_SOURCES})
TARGET_LINK_LIBRARIES(PyViewer ${_link_LIBRARIES})
INSTALL(TARGETS PyViewer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${PyViewer_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
