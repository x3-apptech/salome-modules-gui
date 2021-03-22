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
PROJECT(PyInterp C CXX)

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

# Detect prerequisites
# ====================

# Python
FIND_PACKAGE(SalomePythonInterp REQUIRED)
FIND_PACKAGE(SalomePythonLibs REQUIRED)
# Qt
FIND_PACKAGE(SalomeQt5 REQUIRED)

# Detection summary report
SALOME_PACKAGE_REPORT_AND_CHECK()

# Directories
#
# Directories have to be given after prerequisites (to be able to use
# Python version string for example).
# ===================================================================
SET(PYINTERP_INSTALL_LIBS lib CACHE PATH "Install path: libraries")
SET(PYINTERP_INSTALL_HEADERS include CACHE PATH "Install path: headers")
SET(PYINTERP_INSTALL_RES share/resources CACHE PATH "Install path: resources")
SET(PYINTERP_INSTALL_CMAKE lib/cmake/${PROJECT_NAME_LC} CACHE PATH "Install path: cmake files")

MARK_AS_ADVANCED(PYINTERP_INSTALL_LIBS PYINTERP_INSTALL_HEADERS
                 PYINTERP_INSTALL_RES PYINTERP_INSTALL_CMAKE)

SET(PYINTERP_EXPORT_NAME ${PROJECT_NAME} CACHE STRING "Export name")
MARK_AS_ADVANCED(PYINTERP_EXPORT_NAME)

# Subdirs
# =======
ADD_SUBDIRECTORY(src)

# Export configuration and targets
# ================================
IF(NOT PYINTERP_SUBPROJECT)
  # Install the export set for use with the install-tree
  INSTALL(EXPORT ${PYINTERP_EXPORT_NAME}TargetGroup DESTINATION ${PYINTERP_INSTALL_CMAKE}
          FILE ${PROJECT_NAME}Targets.cmake)
ENDIF()
