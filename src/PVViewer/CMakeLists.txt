# Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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

INCLUDE(UseQt4Ext)

# Install ParaView filters, etc ...
ADD_SUBDIRECTORY(resources)

# --- options ---

INCLUDE_DIRECTORIES(${GUI_INCLUDE_DIRS} 
    ${QT_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/SUIT
    ${CMAKE_SOURCE_DIR}/src/CAM
    ${CMAKE_SOURCE_DIR}/src/STD
    ${CMAKE_SOURCE_DIR}/src/Qtx
    ${CMAKE_SOURCE_DIR}/src/Event
    ${CMAKE_SOURCE_DIR}/src/PyInterp
    ${CMAKE_SOURCE_DIR}/src/PyConsole
    ${CMAKE_SOURCE_DIR}/src/LogWindow
    )

SET(_PARAVIEW_APP_COMPO_LIB
  pqApplicationComponents
  )

ADD_DEFINITIONS(${KERNEL_DEFINITIONS})
ADD_DEFINITIONS(${OMNIORB_DEFINITIONS} ${PYTHON_DEFINITIONS})

SET(_link_LIBRARIES 
  ${KERNEL_LDFLAGS} ${KERNEL_SALOMELocalTrace} ${KERNEL_OpUtil} 
  ${GUI_LDFLAGS} suit Event PyInterp 
  ${_PARAVIEW_APP_COMPO_LIB}
  vtkRenderingFreeTypeOpenGL
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  PVViewer_ViewManager.h
  PVViewer_ViewModel.h
  PVViewer_ViewWindow.h
  PVViewer_Behaviors.h
  PVViewer_GUIElements.h
)

# header files / no moc processing
SET(_other_HEADERS
  PVViewer.h
  PVViewer_LogWindowAdapter.h
  PVViewer_EngineWrapper.h
)
# header files / to install
SET(PVViewer_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/PVViewer_msg_en.ts
)

# --- sources ---

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

# sources / rcc wrappings
QT4_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

# sources / static
SET(_other_SOURCES
  PVViewer_ViewManager.cxx
  PVViewer_ViewModel.cxx
  PVViewer_ViewWindow.cxx
  PVViewer_LogWindowAdapter.cxx
  PVViewer_Behaviors.cxx
  PVViewer_GUIElements.cxx
  PVViewer_EngineWrapper.cxx
  )
  
# sources / to compile
SET(PVViewer_SOURCES 
  ${_other_SOURCES} 
  ${_moc_SOURCES} 
  ${_rcc_SOURCES}
  )

# --- rules ---
ADD_LIBRARY(PVViewer ${PVViewer_SOURCES})

TARGET_LINK_LIBRARIES(PVViewer ${_link_LIBRARIES})
INSTALL(TARGETS PVViewer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

QT4_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

# --- header and resources installation ---
INSTALL(FILES ${PVViewer_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
