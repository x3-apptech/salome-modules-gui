# - Find Qwt installation
# Sets the following variables:
#   QWT_LIBRARY         - path to the Qwt library
#   QWT_INCLUDE_DIR     - path to the Qwt headers
#
#  The header qwt_plot.h is looked for.
#  The libraries 
#      qwt-qt4, qwt
#  are looked for.
#

#########################################################################
# Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

IF(NOT Qwt_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for Qwt ...")
ENDIF()

FIND_PATH(QWT_INCLUDE_DIR qwt_plot.h PATH_SUFFIXES qwt)
FIND_PATH(QWT_INCLUDE_DIR qwt_plot.h PATH_SUFFIXES qwt-qt4)

IF(WIN32)
  SET(QWT_DEFINITIONS "-DQWT_DLL")
  
  IF(CMAKE_BUILD_TYPE STREQUAL Debug)
    FIND_LIBRARY(QWT_LIBRARY qwtd5)
  ELSE()
    FIND_LIBRARY(QWT_LIBRARY qwt5)
  ENDIF()
ELSE(WIN32)
  # Give precedence to qwt-qt4 library:
  FIND_LIBRARY(QWT_LIBRARY qwt-qt4 PATH_SUFFIXES lib lib64)
  FIND_LIBRARY(QWT_LIBRARY qwt-qt4)
  FIND_LIBRARY(QWT_LIBRARY qwt PATH_SUFFIXES lib lib64)
  FIND_LIBRARY(QWT_LIBRARY qwt)
ENDIF(WIN32)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Qwt REQUIRED_VARS QWT_INCLUDE_DIR QWT_LIBRARY)

