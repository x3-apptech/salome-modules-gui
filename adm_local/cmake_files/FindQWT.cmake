# Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

SET(QWT_ROOT $ENV{QWTHOME} CACHE PATH "Path to the Qwt directory")

FIND_PATH(QWT_INCLUDE_DIR qwt_plot.h PATHS ${QWT_ROOT}/include ${QWT_ROOT}/include/qwt NO_DEFAULT_PATH)
FIND_PATH(QWT_INCLUDE_DIR qwt_plot.h PATHS /usr/include/qwt-qt4)
IF(WINDOWS)
  SET(QWT_DEFINITIONS "-DQWT_DLL")
ENDIF(WINDOWS)

IF(WINDOWS)
  IF(CMAKE_BUILD_TYPE STREQUAL Debug)
    FIND_LIBRARY(QWT_LIBRARY qwtd5 ${QWT_ROOT}/lib)
  ELSE(CMAKE_BUILD_TYPE STREQUAL Debug)
    FIND_LIBRARY(QWT_LIBRARY qwt5 ${QWT_ROOT}/lib)
  ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)
ELSE(WINDOWS)
  FIND_LIBRARY(QWT_LIBRARY qwt-qt4 HINTS ${QWT_ROOT} PATH_SUFFIXES lib lib64 NO_DEFAULT_PATH)
  FIND_LIBRARY(QWT_LIBRARY qwt-qt4)
  FIND_LIBRARY(QWT_LIBRARY qwt HINTS ${QWT_ROOT} PATH_SUFFIXES lib lib64 NO_DEFAULT_PATH)
  FIND_LIBRARY(QWT_LIBRARY qwt)
ENDIF(WINDOWS)

SET(QWT_FOUND "FALSE")
IF(QWT_INCLUDE_DIR AND QWT_LIBRARY)
  SET(QWT_FOUND "TRUE")
  MARK_AS_ADVANCED(QWT_ROOT)
  MARK_AS_ADVANCED(QWT_INCLUDE_DIR)
  MARK_AS_ADVANCED(QWT_LIBRARY)
ELSE(QWT_INCLUDE_DIR AND QWT_LIBRARY)
  MESSAGE(FATAL_ERROR "Qwt is required, please define QWT_ROOT CMake variable or set QWTHOME in your environment")
ENDIF(QWT_INCLUDE_DIR AND QWT_LIBRARY)  