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
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} qtx)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  Style_PrefDlg.h
  Style_Salome.h
)

# header files / internal
# no processing currently: TODO: check for MSVS project
SET(_internal_HEADERS
  Style_Model.h
  Style_ResourceMgr.h
  Style_Tools.h
)

# header files / no moc processing
SET(_other_HEADERS
  Style.h
)

# header files / to install (internal ones are excluded)
SET(SalomeStyle_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by rcc
SET(_rcc_RESOURCES Style.qrc)

# resource files / to be processed by lrelease
SET(_ts_RESOURCES resources/Style_msg_fr.ts
                  resources/Style_msg_ja.ts)

# resource files / other (static)
SET(_other_RESOURCES resources/SalomeStyle.xml)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / rcc wrappings
QT_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

# sources / static
SET(_other_SOURCES
  Style_Model.cxx
  Style_PrefDlg.cxx
  Style_ResourceMgr.cxx
  Style_Salome.cxx
  Style_Tools.cxx
)

# sources / to compile
SET(SalomeStyle_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_rcc_SOURCES})

# --- rules ---

ADD_LIBRARY(SalomeStyle ${SalomeStyle_SOURCES})
TARGET_LINK_LIBRARIES(SalomeStyle ${_link_LIBRARIES})
INSTALL(TARGETS SalomeStyle EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomeStyle_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES "${_other_RESOURCES}" DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
