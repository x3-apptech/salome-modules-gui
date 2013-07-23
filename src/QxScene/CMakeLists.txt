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

INCLUDE(UseQt4Ext)

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${Boost_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
)

SET(COMMON_LIBS ${QT_LIBRARIES} suit)

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
  resources/QxSceneViewer_msg_ja.ts
)

ADD_DEFINITIONS(${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS} ${PYTHON_DEFINITIONS})

ADD_LIBRARY(QxScene ${QxScene_SOURCES} ${GUI_HEADERS_MOC})
TARGET_LINK_LIBRARIES(QxScene ${COMMON_LIBS})
INSTALL(TARGETS QxScene EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

SET(COMMON_HEADERS_H 
  QxScene_ViewWindow.h
  QxScene_ViewManager.h
  QxScene_ViewModel.h
  QxScene_Def.h
  QxScene.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${SALOME_INSTALL_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
