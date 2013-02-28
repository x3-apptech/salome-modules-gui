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

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${PTHREAD_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${SIP_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../PyInterp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../SalomeApp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../../LightApp
  ${CMAKE_CURRENT_SOURCE_DIR}/../SALOME_PYQT_GUILight
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Plot2d
  ${CMAKE_CURRENT_SOURCE_DIR}/../../OCCViewer
  ${CMAKE_CURRENT_SOURCE_DIR}/../../SalomeApp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../CAM
  ${CMAKE_CURRENT_SOURCE_DIR}/../../STD
)

SET(COMMON_LIBS 
  ${PYTHON_LIBRARIES}
  ${PYQT_LIBRARIES}
  ${VTK_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${PLATFORM_LIBS}
  PyInterp
  SalomeApp
  OCCViewer
  Plot2d
  SalomePyQtGUILight
)

SET(GUI_HEADERS SALOME_PYQT_Module.h)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

ADD_DEFINITIONS("-DCALL_OLD_METHODS -DHAVE_CONFIG_H ${QT_DEFINITIONS} ${PYTHON_DEFINITIONS} ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS} ${VTK_DEFINITIONS} ${OMNIORB_DEFINITIONS}")

ADD_LIBRARY(SalomePyQtGUI SALOME_PYQT_Module.cxx ${GUI_HEADERS_MOC})
TARGET_LINK_LIBRARIES(SalomePyQtGUI ${COMMON_LIBS})
INSTALL(TARGETS SalomePyQtGUI DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  SALOME_PYQT_GUI.h
  SALOME_PYQT_Module.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
