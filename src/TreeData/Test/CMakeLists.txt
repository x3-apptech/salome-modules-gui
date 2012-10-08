# Copyright (C) 2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_SOURCE_DIR}/src/GuiHelpers
  ${CMAKE_SOURCE_DIR}/src/TreeData
)

SET(COMMON_LIBS
  ${CORBA_LIBS}
  ${QT_LIBS}
  SalomeTreeData
  SalomeGuiHelpers
  ${SalomeLifeCycleCORBA}
  ${SalomeKernelHelpers}
)

SET(GUI_HEADERS mainwindow.hxx)
QT4_WRAP_CPP(GUI_HEADERS ${GUI_HEADERS})
SET(GUI_FORMS mainwindow.ui)
QT4_WRAP_UI(GUI_FORMS_HEADERS ${GUI_FORMS})

SET(TreeData_guitester_SOURCES
  testhelper.hxx
  testhelper.cxx
  guitester.cxx
  mainwindow.hxx
  mainwindow.cxx
  MyDataModel.hxx
  MyDataModel.cxx
)

ADD_EXECUTABLE(TreeData_guitester ${TreeData_guitester_SOURCES} ${GUI_HEADERS} ${GUI_FORMS_HEADERS})
SET_TARGET_PROPERTIES(TreeData_guitester PROPERTIES COMPILE_FLAGS "${OMNIORB_DEFINITIONS} ${QT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(TreeData_guitester ${COMMON_LIBS})

SET(TreeData_tester_SOURCES
  tester.cxx
  MyDataModel.cxx
)

ADD_EXECUTABLE(TreeData_tester ${TreeData_tester_SOURCES})
SET_TARGET_PROPERTIES(TreeData_tester PROPERTIES COMPILE_FLAGS "${OMNIORB_DEFINITIONS} ${QT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(TreeData_tester ${COMMON_LIBS})

INSTALL(TARGETS TreeData_guitester TreeData_tester DESTINATION ${GUI_salomebin_BINS})
