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
  STD_Application.h
  STD_MDIDesktop.h
  STD_SDIDesktop.h
  STD_TabDesktop.h
)

# header files / no moc processing
SET(_other_HEADERS STD.h)

# header files / to install
SET(std_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/STD_images.ts
  resources/STD_msg_en.ts
  resources/STD_msg_fr.ts
  resources/STD_msg_ja.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/cascade.png
  resources/close.png
  resources/copy.png
  resources/cut.png
  resources/delete.png
  resources/help.png
  resources/htile.png
  resources/new.png
  resources/open.png
  resources/paste.png
  resources/print.png
  resources/redo.png
  resources/reset.png
  resources/save.png
  resources/std.ini
  resources/undo.png
  resources/vtile.png
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  STD_Application.cxx
  STD_MDIDesktop.cxx
  STD_SDIDesktop.cxx
  STD_TabDesktop.cxx
)

# sources / to compile
SET(std_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(std ${std_SOURCES})
TARGET_LINK_LIBRARIES(std ${_link_LIBRARIES})
INSTALL(TARGETS std EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${std_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
