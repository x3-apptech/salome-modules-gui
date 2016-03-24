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

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${Boost_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/SalomeApp
  ${PROJECT_SOURCE_DIR}/src/LightApp
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/OBJECT
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${QT_DEFINITIONS}
  ${BOOST_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES 
  ${CORBA_LIBS}
  ${KERNEL_SalomeKernelHelpers}
  qtx suit std CAM SalomeObject LightApp SalomeApp
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  StandardApp_Module.hxx
)

# header files / no moc processing
SET(_other_HEADERS
  QtHelper.hxx
  SALOME_AppStudyEditor.hxx
  SALOME_GuiServices.hxx
  SalomeGuiHelpers.hxx
)

# header files / to install
SET(SalomeGuiHelpers_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  SALOME_AppStudyEditor.cxx
  SALOME_GuiServices.cxx
  StandardApp_Module.cxx
)

# sources / to compile
SET(SalomeGuiHelpers_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(SalomeGuiHelpers ${SalomeGuiHelpers_SOURCES})
TARGET_LINK_LIBRARIES(SalomeGuiHelpers ${_link_LIBRARIES})
INSTALL(TARGETS SalomeGuiHelpers EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomeGuiHelpers_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
