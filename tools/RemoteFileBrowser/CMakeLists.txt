# Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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
# Author : Anthony GEAY (EDF R&D)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)
PROJECT(RemoteFileBrowser CXX)

SET(CONFIGURATION_ROOT_DIR $ENV{CONFIGURATION_ROOT_DIR} CACHE PATH "Path to the Salome CMake files")
IF(EXISTS ${CONFIGURATION_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${CONFIGURATION_ROOT_DIR}/cmake")
  INCLUDE(SalomeMacros NO_POLICY_SCOPE)
ELSE()
  MESSAGE(FATAL_ERROR "We absolutely need the Salome CMake configuration files, please define CONFIGURATION_ROOT_DIR !")
ENDIF()

SET(REMOTEFILEBROWSER_INSTALL_BINS bin CACHE PATH "Install path: RemoteFileBrowser binaries")
SET(REMOTEFILEBROWSER_INSTALL_LIBS lib CACHE PATH "Install path: RemoteFileBrowser libraries")
SET(REMOTEFILEBROWSER_INSTALL_HEADERS include CACHE PATH "Install path: RemoteFileBrowser headers")

INCLUDE(SalomeSetupPlatform)
SET(BUILD_SHARED_LIBS TRUE)
FIND_PACKAGE(SalomeQt5 REQUIRED)
INCLUDE(UseQtExt)
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  )

ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  )

SET(qremotefilebrowser_SOURCES
  QRemoteFileBrowser.cxx
  QMachineBrowser.cxx
  QRemoteCopyWidget.cxx
  )

SET(qremotefilebrowser_HEADERS
  QRemoteFileBrowser.h
  QRemoteCopyWidget.h
  QMachineBrowser.h
  RemoteFileBrowser.h 
  )

SET(qremotefilebrowser_LIBRARIES
  "Qt5::Core;Qt5::Widgets"
  )

SET(_moc_HEADERS
  QRemoteFileBrowser.h
  QMachineBrowser.h
  QRemoteCopyWidget.h
  )

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

ADD_LIBRARY(qremotefilebrowser ${qremotefilebrowser_SOURCES} ${_moc_SOURCES})
TARGET_LINK_LIBRARIES(qremotefilebrowser ${qremotefilebrowser_LIBRARIES})
ADD_EXECUTABLE(remotefilebrowser remotefilebrowser.cxx)
TARGET_LINK_LIBRARIES(remotefilebrowser qremotefilebrowser)
INSTALL(TARGETS remotefilebrowser DESTINATION ${REMOTEFILEBROWSER_INSTALL_BINS})
INSTALL(TARGETS qremotefilebrowser DESTINATION ${REMOTEFILEBROWSER_INSTALL_LIBS})
INSTALL(FILES ${_moc_HEADERS} DESTINATION ${REMOTEFILEBROWSER_INSTALL_HEADERS})
