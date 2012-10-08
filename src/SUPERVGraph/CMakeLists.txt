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
  ${PTHREAD_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../OBJECT
)

SET(SUPERVGraph_SOURCES
  SUPERVGraph.cxx
  SUPERVGraph_ViewFrame.cxx
  SUPERVGraph_ViewManager.cxx
  SUPERVGraph_ViewModel.cxx
)

SET(GUI_HEADERS
  SUPERVGraph.h
  SUPERVGraph_ViewFrame.h
  SUPERVGraph_ViewManager.h
  SUPERVGraph_ViewModel.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(dist_salomeres_DATA
  resources/superv_view_pan.png
  resources/superv_view_reset.png
)

SET(GUITS_SOURCES
  resources/SUPERVGraph_images.ts
  resources/SUPERVGraph_msg_en.ts
  resources/SUPERVGraph_msg_fr.ts
)

ADD_LIBRARY(SUPERVGraph SHARED ${SUPERVGraph_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(SUPERVGraph PROPERTIES COMPILE_FLAGS "${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${PYTHON_DEFINITIONS} ${BOOST_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(SUPERVGraph ${QT_MT_LIBS} suit SalomeObject)
INSTALL(TARGETS SUPERVGraph DESTINATION ${GUI_salomelib_LIBS})

FILE(GLOB COMMON_HEADERS_H "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${GUI_salomeres_DATA})


