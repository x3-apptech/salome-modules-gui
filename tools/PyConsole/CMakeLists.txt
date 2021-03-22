# Copyright (C) 2015-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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
# Project name, uppercase / lowercase
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)
STRING(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LC)

# Include common macros
# =====================
SET(CONFIGURATION_ROOT_DIR $ENV{CONFIGURATION_ROOT_DIR} CACHE PATH "Path to the Salome CMake configuration files")
IF(EXISTS ${CONFIGURATION_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${CONFIGURATION_ROOT_DIR}/cmake")
  INCLUDE(SalomeMacros NO_POLICY_SCOPE)
ELSE()
  MESSAGE(FATAL_ERROR "We absolutely need the Salome CMake configuration files, please define CONFIGURATION_ROOT_DIR !")
ENDIF()

# Platform setup
# ==============
INCLUDE(SalomeSetupPlatform)

# Options
# =======
OPTION(BUILD_SHARED_LIBS "Build shared libraries" ON)
OPTION(BUILD_PYMODULE "Build Python module" ON)

# Detect prerequisites
# ====================

# Python
FIND_PACKAGE(SalomePythonInterp REQUIRED)
FIND_PACKAGE(SalomePythonLibs REQUIRED)
# Qt
FIND_PACKAGE(SalomeQt5 REQUIRED)
IF(BUILD_PYMODULE)
  # Sip (should come after Python and before PyQt)
  FIND_PACKAGE(SalomeSIP REQUIRED)
  SALOME_LOG_OPTIONAL_PACKAGE(SalomeSIP BUILD_PYMODULE)
  # PyQt
  FIND_PACKAGE(SalomePyQt5 REQUIRED)
  SALOME_LOG_OPTIONAL_PACKAGE(SalomePyQt5 BUILD_PYMODULE)
ENDIF(BUILD_PYMODULE)

# Detection summary report
SALOME_PACKAGE_REPORT_AND_CHECK()

# Directories
#
# Directories have to be given after prerequisites (to be able to use
# Python version string for example).
# ===================================================================
SET(PYCONSOLE_INSTALL_LIBS lib CACHE PATH "Install path: libraries")
SET(PYCONSOLE_INSTALL_PYTHON lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages CACHE PATH "Install path: Python module")
SET(PYCONSOLE_INSTALL_HEADERS include CACHE PATH "Install path: headers")
SET(PYCONSOLE_INSTALL_RES share/resources CACHE PATH "Install path: resources")
SET(PYCONSOLE_INSTALL_CMAKE lib/cmake/${PROJECT_NAME_LC} CACHE PATH "Install path: cmake files")

MARK_AS_ADVANCED(PYCONSOLE_INSTALL_LIBS PYCONSOLE_INSTALL_HEADERS
                 PYCONSOLE_INSTALL_RES PYCONSOLE_INSTALL_CMAKE)

SET(PYCONSOLE_EXPORT_NAME ${PROJECT_NAME} CACHE STRING "Export name")
MARK_AS_ADVANCED(PYCONSOLE_EXPORT_NAME)

# Dependencies
# ============
# Force PyInterp to build as a sub-project
SET(PYINTERP_SUBPROJECT YES CACHE INTERNAL "")
SET(PYINTERP_EXPORT_NAME "${PROJECT_NAME}" CACHE STRING "")
SET(PYINTERP_INSTALL_LIBS "${PYCONSOLE_INSTALL_LIBS}" CACHE PATH "")
SET(PYINTERP_INSTALL_HEADERS "${PYCONSOLE_INSTALL_HEADERS}" CACHE PATH "")
SET(PYINTERP_INSTALL_RES "${PYCONSOLE_INSTALL_RES}" CACHE PATH "")
SET(PYINTERP_INSTALL_CMAKE "${PYCONSOLE_INSTALL_CMAK}" CACHE PATH "")

ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/../PyInterp ${PROJECT_BINARY_DIR}/PyInterp)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../PyInterp/src)

# Subdirs
# =======
ADD_SUBDIRECTORY(src)

# Export configuration and targets
# ================================
IF(NOT PYCONSOLE_SUBPROJECT)
  # Install the export set for use with the install-tree
  INSTALL(EXPORT ${PYCONSOLE_EXPORT_NAME}TargetGroup DESTINATION ${PYCONSOLE_INSTALL_CMAKE}
          FILE ${PROJECT_NAME}Targets.cmake)
ENDIF()
