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
  ${OMNIORB_INCLUDE_DIR}
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/GuiHelpers
  ${PROJECT_SOURCE_DIR}/src/TreeData
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${OMNIORB_DEFINITIONS} ${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES
  ${CORBA_LIBS}
  ${QT_LIBRARIES}
  SalomeTreeData SalomeGuiHelpers
  ${KERNEL_SalomeLifeCycleCORBA} ${KERNEL_SalomeKernelHelpers}
)

# --- resources ---

# resource files / to be processed by uic
SET(_uic_files mainwindow.ui)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS mainwindow.hxx)

# header files / uic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_uic_files})

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# --- sources ---

# sources / to compile
SET(TreeData_guitester_SOURCES MyDataModel.cxx guitester.cxx mainwindow.cxx testhelper.cxx ${_moc_SOURCES} ${_uic_HEADERS})
SET(TreeData_tester_SOURCES MyDataModel.cxx tester.cxx)

# --- rules ---

ADD_EXECUTABLE(TreeData_guitester ${TreeData_guitester_SOURCES})
TARGET_LINK_LIBRARIES(TreeData_guitester ${_link_LIBRARIES})

ADD_EXECUTABLE(TreeData_tester ${TreeData_tester_SOURCES})
TARGET_LINK_LIBRARIES(TreeData_tester ${_link_LIBRARIES})

INSTALL(TARGETS TreeData_guitester TreeData_tester DESTINATION ${SALOME_INSTALL_BINS})
