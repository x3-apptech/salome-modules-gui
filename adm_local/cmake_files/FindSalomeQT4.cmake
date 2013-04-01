# Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

# Qt4 detection for Salome

#set(QT_MIN_VERSION 4.8.0)
set(QT_ROOT_DIR $ENV{QT_ROOT_DIR} CACHE PATH "Path to Qt directory")
if(EXISTS ${QT_ROOT_DIR})
  set(CMAKE_INCLUDE_PATH ${QT_ROOT_DIR}/include)
  set(CMAKE_LIBRARY_PATH ${QT_ROOT_DIR}/lib)
  set(CMAKE_PROGRAM_PATH ${QT_ROOT_DIR}/bin)
endif(EXISTS ${QT_ROOT_DIR})
find_package(Qt4 REQUIRED QtCore QtGui QtXml QtWebKit QtOpenGL)
IF(NOT QT4_FOUND)
  MESSAGE(FATAL_ERROR "Qt4 not found, please set QT_ROOT_DIR environment or cmake variable")
ELSE(NOT QT4_FOUND})
  INCLUDE(${QT_USE_FILE})
ENDIF(NOT QT4_FOUND)
