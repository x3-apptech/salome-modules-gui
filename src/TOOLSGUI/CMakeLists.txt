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
  ${QT_INCLUDES}
  ${CAS_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${CAS_DEFINITIONS}
  ${BOOST_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${QT_LIBRARIES}
  ${CAS_KERNEL}
  ${KERNEL_SalomeIDLKernel}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SalomeNS}
  ${KERNEL_OpUtil}
  ${OMNIORB_LIBRARIES}
  qtx suit
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  ToolsGUI_CatalogGeneratorDlg.h
  ToolsGUI_RegWidget.h
)

# header files / no moc processing
SET(_other_HEADERS
  ToolsGUI.h
)

# header files / to install
SET(ToolsGUI_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/ToolsGUI_msg_en.ts
  resources/ToolsGUI_msg_fr.ts
  resources/ToolsGUI_msg_ja.ts
) 

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  ToolsGUI.cxx
  ToolsGUI_CatalogGeneratorDlg.cxx
  ToolsGUI_RegWidget.cxx
)

# sources / to compile
SET(ToolsGUI_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(ToolsGUI ${ToolsGUI_SOURCES})
TARGET_LINK_LIBRARIES(ToolsGUI ${_link_LIBRARIES})
INSTALL(TARGETS ToolsGUI EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${ToolsGUI_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
