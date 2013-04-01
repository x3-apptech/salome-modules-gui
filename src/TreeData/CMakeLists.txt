# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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
INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/UseQT4EXT.cmake)

ADD_SUBDIRECTORY(Test)

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${OMNIORB_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../SalomeApp
  ${CMAKE_CURRENT_SOURCE_DIR}/../LightApp
  ${CMAKE_CURRENT_SOURCE_DIR}/../CAM
  ${CMAKE_CURRENT_SOURCE_DIR}/../STD
  ${CMAKE_CURRENT_SOURCE_DIR}/../ObjBrowser
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../GuiHelpers
)


SET(COMMON_LIBS
  ${OMNIORB_LIBRARIES}
  ${QT_LIBRARIES}
  ${KERNEL_LDFLAGS}
  ${KERNEL_SalomeLifeCycleCORBA}
  ${KERNEL_SalomeKernelHelpers}
  SalomeApp
  LightApp
  suit
  qtx
  CAM
  std
  ObjBrowser
)

SET(GUI_HEADERS TreeModel.hxx TreeView.hxx TreeObserver.hxx)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})
SET(SalomeTreeData_SOURCES
  DockWidgets.cxx
  TreeModel.cxx
  TreeItem.cxx
  TreeView.cxx
  TreeObserver.cxx
  DataModel.cxx
  DataObject.cxx
  DataProcessor.cxx
  TreeGuiManager.cxx
)

ADD_DEFINITIONS(${CAS_DEFINITIONS} ${QT_DEFINITIONS} ${BOOST_DEFINITIONS} ${OMNIORB_DEFINITIONS})

ADD_LIBRARY(SalomeTreeData ${SalomeTreeData_SOURCES} ${GUI_HEADERS_MOC})
TARGET_LINK_LIBRARIES(SalomeTreeData ${COMMON_LIBS})
INSTALL(TARGETS SalomeTreeData DESTINATION ${GUI_salomelib_LIBS})

FILE(GLOB COMMON_HEADERS_HXX "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")
INSTALL(FILES ${COMMON_HEADERS_HXX} DESTINATION ${GUI_salomeinclude_HEADERS})
