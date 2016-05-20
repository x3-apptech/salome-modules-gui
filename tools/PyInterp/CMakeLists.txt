# Copyright (C) 2015-2016  CEA/DEN, EDF R&D
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12 FATAL_ERROR)
PROJECT(PyInterp C CXX)

# Versioning
# ===========
# Project name, upper case
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

# To be changed once externalized CMake procedure:
###################
SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "Path to the Salome KERNEL")
IF(EXISTS ${KERNEL_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${KERNEL_ROOT_DIR}/salome_adm/cmake_files")
  INCLUDE(SalomeMacros)
ELSE(EXISTS ${KERNEL_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome KERNEL, please define KERNEL_ROOT_DIR")
ENDIF(EXISTS ${KERNEL_ROOT_DIR})
# From GUI - again to be changed once externalized:
LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../adm_local/cmake_files")
###################

# Platform setup
# ==============
INCLUDE(SalomeSetupPlatform)

# Options
# =======
OPTION(PYINTERP_BUILD_WITH_QT5 "Build PYINTERP with Qt 5" ON)

#
# Set list of prerequisites
# =========================

FIND_PACKAGE(SalomePythonInterp REQUIRED)
FIND_PACKAGE(SalomePythonLibs REQUIRED)
# Qt
IF(NOT PYINTERP_BUILD_WITH_QT5)
  FIND_PACKAGE(SalomeQt4 REQUIRED)
ELSE()
  FIND_PACKAGE(SalomeQt5 REQUIRED)
ENDIF()

# Detection report
SALOME_PACKAGE_REPORT_AND_CHECK()

# Directories
#
# Directories have to be given after prerequisites (to be able to use
# Python version string for example).
# ===========
SET(PYINTERP_INSTALL_LIBS lib CACHE PATH "Install path: PyInterp libs")
SET(PYINTERP_INSTALL_HEADERS include CACHE PATH "Install path: PyInterp headers")

SET(PYINTERP_INSTALL_RES share/resources CACHE PATH "Install path: PyInterp resources")

# Sources
# ========
ADD_SUBDIRECTORY(src)

