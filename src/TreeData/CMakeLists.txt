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

ADD_SUBDIRECTORY(Test)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${OMNIORB_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/GuiHelpers
  ${PROJECT_SOURCE_DIR}/src/LightApp
  ${PROJECT_SOURCE_DIR}/src/ObjBrowser
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/SalomeApp
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
  ${OMNIORB_LIBRARIES}
  ${QT_LIBRARIES}
  ${KERNEL_SalomeLifeCycleCORBA} ${KERNEL_SalomeKernelHelpers}
  qtx suit std CAM ObjBrowser LightApp SalomeApp 
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  TreeModel.hxx
  TreeObserver.hxx
  TreeView.hxx
)

# header files / no moc processing
SET(_other_HEADERS
  DataModel.hxx
  DataObject.hxx
  DataProcessor.hxx
  DockWidgets.hxx
  TreeData.hxx
  TreeGuiManager.hxx
  TreeItem.hxx
)

# header files / to install
SET(SalomeTreeData_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  DataModel.cxx
  DataObject.cxx
  DataProcessor.cxx
  DockWidgets.cxx
  TreeGuiManager.cxx
  TreeItem.cxx
  TreeModel.cxx
  TreeObserver.cxx
  TreeView.cxx
)

# sources / to compile
SET(SalomeTreeData_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(SalomeTreeData ${SalomeTreeData_SOURCES})
TARGET_LINK_LIBRARIES(SalomeTreeData ${_link_LIBRARIES})
INSTALL(TARGETS SalomeTreeData EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomeTreeData_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
