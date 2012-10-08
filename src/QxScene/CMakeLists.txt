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
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
)

SET(COMMON_FLAGS "${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS} ${PYTHON_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS ${QT_MT_LIBS} suit)

SET(GUI_HEADERS   
  QxScene_ViewWindow.h
  QxScene_ViewManager.h
  QxScene_ViewModel.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(QxScene_SOURCES
  QxScene_ViewWindow.cxx
  QxScene_ViewManager.cxx
  QxScene_ViewModel.cxx
)

SET(GUITS_SOURCES
  resources/QxSceneViewer_images.ts
  resources/QxSceneViewer_msg_en.ts
  resources/QxSceneViewer_msg_fr.ts
)

ADD_LIBRARY(QxScene SHARED ${QxScene_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(QxScene PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(QxScene ${COMMON_LIBS})
INSTALL(TARGETS QxScene DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  QxScene_ViewWindow.h
  QxScene_ViewManager.h
  QxScene_ViewModel.h
  QxScene_Def.h
  QxScene.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${GUI_salomeres_DATA})
