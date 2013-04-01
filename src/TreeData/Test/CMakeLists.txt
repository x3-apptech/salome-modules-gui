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

INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIR}
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_SOURCE_DIR}/src/GuiHelpers
  ${CMAKE_SOURCE_DIR}/src/TreeData
)

SET(COMMON_LIBS
  ${CORBA_LIBS}
  ${QT_LIBRARIES}
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
  testhelper.cxx
  guitester.cxx
  mainwindow.cxx
  MyDataModel.cxx
)

ADD_DEFINITIONS(${OMNIORB_DEFINITIONS} ${QT_DEFINITIONS})

ADD_EXECUTABLE(TreeData_guitester ${TreeData_guitester_SOURCES} ${GUI_HEADERS} ${GUI_FORMS_HEADERS})
TARGET_LINK_LIBRARIES(TreeData_guitester ${COMMON_LIBS})

SET(TreeData_tester_SOURCES
  tester.cxx
  MyDataModel.cxx
)

ADD_EXECUTABLE(TreeData_tester ${TreeData_tester_SOURCES})
TARGET_LINK_LIBRARIES(TreeData_tester ${COMMON_LIBS})

INSTALL(TARGETS TreeData_guitester TreeData_tester DESTINATION ${GUI_salomebin_BINS})
