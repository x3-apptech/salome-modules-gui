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
PROJECT(PyConsole C CXX)

# Versioning
# ===========
# Project name, upper case
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

###################
# To be changed once externalized CMake procedure:
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
OPTION(PYCONSOLE_BUILD_WITH_QT5 "Build PYCONSOLE with Qt 5" ON)
OPTION(PYCONSOLE_BUILD_PYTHON "Build Python wrapping for PyConsole" ON)

#
# Set list of prerequisites
# =========================

FIND_PACKAGE(SalomePythonInterp REQUIRED)
FIND_PACKAGE(SalomePythonLibs REQUIRED)

# Qt
IF(NOT PYCONSOLE_BUILD_WITH_QT5)
  FIND_PACKAGE(SalomeQt4 REQUIRED)
ELSE()
  FIND_PACKAGE(SalomeQt5 REQUIRED)
ENDIF()

IF(PYCONSOLE_BUILD_PYTHON)
  # Sip
  FIND_PACKAGE(SalomeSIP REQUIRED)  # should come after Python and before PyQt
  SALOME_LOG_OPTIONAL_PACKAGE(SalomeSIP PYCONSOLE_BUILD_PYTHON)
  # PyQt
  IF (NOT PYCONSOLE_BUILD_WITH_QT5)
    FIND_PACKAGE(SalomePyQt4 REQUIRED)
    SALOME_LOG_OPTIONAL_PACKAGE(SalomePyQt4 PYCONSOLE_BUILD_PYTHON)
  ELSE()
    FIND_PACKAGE(SalomePyQt5 REQUIRED)
    SALOME_LOG_OPTIONAL_PACKAGE(SalomePyQt5 PYCONSOLE_BUILD_PYTHON)
  ENDIF()
ENDIF(PYCONSOLE_BUILD_PYTHON)

# Detection report
SALOME_PACKAGE_REPORT_AND_CHECK()

# Directories
#
# Directories have to be given after prerequisites (to be able to use
# Python version string for example).
# ===========
SET(PYCONSOLE_INSTALL_LIBS lib CACHE PATH "Install path: PyConsole libs")
SET(PYCONSOLE_INSTALL_HEADERS include CACHE PATH "Install path: PyConsole headers")

SET(PYCONSOLE_INSTALL_RES share/resources CACHE PATH "Install path: PyConsole resources")

# Tool dependencies
# =================
# PyConsole depends on PyInterp:
SET(PYINTERP_INSTALL_LIBS "${PYCONSOLE_INSTALL_LIBS}"  CACHE PATH "")
SET(PYINTERP_INSTALL_HEADERS "${PYCONSOLE_INSTALL_HEADERS}" CACHE PATH "")
SET(PYINTERP_INSTALL_RES "${PYCONSOLE_INSTALL_RES}" CACHE PATH "")
OPTION(PYINTERP_BUILD_WITH_QT5 "Build PYCONSOLE with Qt 5" ${PYCONSOLE_BUILD_WITH_QT5})

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/../PyInterp ${PROJECT_BINARY_DIR}/PyInterp)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../PyInterp/src)

# Sources
# ========
ADD_SUBDIRECTORY(src)

