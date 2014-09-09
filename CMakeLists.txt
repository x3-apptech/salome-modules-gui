# Copyright (C) 2012-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)
INCLUDE(CMakeDependentOption)

PROJECT(SalomeGUI C CXX)

# Ensure a proper linker behavior:
CMAKE_POLICY(SET CMP0003 NEW)

# Versioning
# ===========
# Project name, upper case
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

SET(${PROJECT_NAME_UC}_MAJOR_VERSION 7)
SET(${PROJECT_NAME_UC}_MINOR_VERSION 4)
SET(${PROJECT_NAME_UC}_PATCH_VERSION 1)
SET(${PROJECT_NAME_UC}_VERSION
  ${${PROJECT_NAME_UC}_MAJOR_VERSION}.${${PROJECT_NAME_UC}_MINOR_VERSION}.${${PROJECT_NAME_UC}_PATCH_VERSION})
SET(${PROJECT_NAME_UC}_VERSION_DEV 1)

# Find KERNEL
# ===========
SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "Path to the Salome KERNEL")
IF(EXISTS ${KERNEL_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${KERNEL_ROOT_DIR}/salome_adm/cmake_files")
  INCLUDE(SalomeMacros)
  FIND_PACKAGE(SalomeKERNEL REQUIRED)
  ADD_DEFINITIONS(${KERNEL_DEFINITIONS})
  INCLUDE_DIRECTORIES(${KERNEL_INCLUDE_DIRS})
ELSE(EXISTS ${KERNEL_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome KERNEL, please define KERNEL_ROOT_DIR")
ENDIF(EXISTS ${KERNEL_ROOT_DIR})

# Platform setup
# ==============
INCLUDE(SalomeSetupPlatform)   # From KERNEL
# Always build libraries as shared objects:
SET(BUILD_SHARED_LIBS TRUE)
# Local macros:
LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/adm_local/cmake_files")

# User options
# ============
OPTION(SALOME_BUILD_DOC "Generate SALOME GUI documentation" ON)
OPTION(SALOME_BUILD_TESTS "Build SALOME tests" ON)
OPTION(SALOME_GUI_USE_OBSERVERS "Use study observers in GUI (advanced)" ON)

# Advanced options:
CMAKE_DEPENDENT_OPTION(SALOME_LIGHT_ONLY "Build SALOME Light only (no CORBA)" OFF
                       "NOT SALOME_KERNEL_LIGHT_ONLY" ON)
OPTION(SALOME_USE_VTKVIEWER "Enable VTK visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_OCCVIEWER "Enable OCC visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_GLVIEWER "Enable OpenGL visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_GRAPHICSVIEW "Enable GraphicsView visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_PLOT2DVIEWER "Enable Plot2D visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_PYCONSOLE "Enable Python GUI interface (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_QXGRAPHVIEWER "Enable QX graph visualization (Mandatory in classic configurations)" ON)
CMAKE_DEPENDENT_OPTION(SALOME_USE_SALOMEOBJECT "Enable Salome Object (Mandatory in classic configurations)" ON
                       "SALOME_LIGHT_ONLY" ON)
OPTION(SALOME_USE_SINGLE_DESKTOP "Enable multiple document interface" ON)

MARK_AS_ADVANCED(SALOME_LIGHT_ONLY SALOME_USE_VTKVIEWER SALOME_USE_GRAPHICSVIEW)
MARK_AS_ADVANCED(SALOME_USE_SALOMEOBJECT SALOME_USE_OCCVIEWER SALOME_USE_GLVIEWER SALOME_USE_PLOT2DVIEWER)
MARK_AS_ADVANCED(SALOME_USE_PYCONSOLE SALOME_USE_QXGRAPHVIEWER)
MARK_AS_ADVANCED(SALOME_USE_SINGLE_DESKTOP)

# Prerequisites
# =============
# Find "big" prerequisites first - they reference themselves many others
#   -> this can help finding the smaller prerequisites and detect conficts.
# In our case KERNEL has already loaded many prereq which are also used in GUI:

##
## From KERNEL:
##

# Various
FIND_PACKAGE(SalomePython REQUIRED)
FIND_PACKAGE(SalomePThread REQUIRED)
FIND_PACKAGE(SalomeSWIG REQUIRED)
FIND_PACKAGE(SalomeBoost REQUIRED)
FIND_PACKAGE(SalomeHDF5 REQUIRED COMPONENTS C)

# Other KERNEL optionals:
IF(NOT SALOME_LIGHT_ONLY)
  FIND_PACKAGE(SalomeOmniORB REQUIRED)
  FIND_PACKAGE(SalomeOmniORBPy REQUIRED)
ELSE()
  ADD_DEFINITIONS("-DGUI_DISABLE_CORBA")
ENDIF() 
IF(SALOME_BUILD_TESTS)
  ENABLE_TESTING()
  FIND_PACKAGE(SalomeCppUnit)
  SALOME_LOG_OPTIONAL_PACKAGE(CppUnit SALOME_BUILD_TESTS)  
ENDIF()
IF(SALOME_BUILD_DOC)
  FIND_PACKAGE(SalomeDoxygen)
  FIND_PACKAGE(SalomeGraphviz)
  FIND_PACKAGE(SalomeSphinx)
  SALOME_LOG_OPTIONAL_PACKAGE(Doxygen SALOME_BUILD_DOC)
  SALOME_LOG_OPTIONAL_PACKAGE(Graphviz SALOME_BUILD_DOC)
  SALOME_LOG_OPTIONAL_PACKAGE(Sphinx SALOME_BUILD_DOC)
ENDIF()

##
## GUI specifics
##

FIND_PACKAGE(SalomeSIP REQUIRED)  # should come after Python and before PyQt4

# Build with GUI observers feature
IF(SALOME_GUI_USE_OBSERVERS)
  ADD_DEFINITIONS(-DWITH_SALOMEDS_OBSERVER)
ENDIF()

IF(SALOME_USE_SINGLE_DESKTOP)
  ADD_DEFINITIONS(-DSINGLE_DESKTOP)
ENDIF()

# OCCT
FIND_PACKAGE(SalomeCAS REQUIRED)
IF(NOT SALOME_USE_OCCVIEWER)
  ADD_DEFINITIONS("-DDISABLE_OCCVIEWER")
ENDIF()

# Qt4
FIND_PACKAGE(SalomeQt4 REQUIRED COMPONENTS QtCore QtGui QtXml QtWebKit QtOpenGL)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(SalomePyQt4 REQUIRED)

# Optional prerequisites for GUI
IF(SALOME_USE_GLVIEWER)
  FIND_PACKAGE(SalomeOpenGL)
  SALOME_LOG_OPTIONAL_PACKAGE(OpenGL SALOME_USE_GLVIEWER) 
ELSE()
  ADD_DEFINITIONS("-DDISABLE_GLVIEWER")
ENDIF()
IF(SALOME_USE_VTKVIEWER)
  # Required components are listed in the FindSalomeVTK.cmake file: 
  FIND_PACKAGE(SalomeVTK) 
  SALOME_LOG_OPTIONAL_PACKAGE(VTK SALOME_USE_VTKVIEWER)
ELSE()
  ADD_DEFINITIONS("-DDISABLE_VTKVIEWER")
ENDIF()
IF(SALOME_USE_PLOT2DVIEWER)
  FIND_PACKAGE(SalomeQwt)  
  SALOME_LOG_OPTIONAL_PACKAGE(Qwt SALOME_USE_PLOT2DVIEWER)
ELSE()
  ADD_DEFINITIONS("-DDISABLE_PLOT2DVIEWER")
ENDIF()
IF (NOT SALOME_USE_GRAPHICSVIEW)
  ADD_DEFINITIONS("-DDISABLE_GRAPHICSVIEW")
ENDIF()
IF(SALOME_USE_PYCONSOLE)
  # Build with obsolete Python module's methods
  ADD_DEFINITIONS(-DCALL_OLD_METHODS)
ELSE()
  ADD_DEFINITIONS("-DDISABLE_PYCONSOLE")
ENDIF()
IF(NOT SALOME_USE_QXGRAPHVIEWER)
  ADD_DEFINITIONS("-DDISABLE_QXGRAPHVIEWER")
ENDIF()
IF(NOT SALOME_USE_SALOMEOBJECT)
  ADD_DEFINITIONS("-DDISABLE_SALOMEOBJECT")
ENDIF()

# Detection summary:
SALOME_PACKAGE_REPORT_AND_CHECK()

# Directories
# (default values taken from KERNEL)
# ===========
SET(SALOME_INSTALL_BINS "${SALOME_INSTALL_BINS}" CACHE PATH "Install path: SALOME binaries")
SET(SALOME_INSTALL_LIBS "${SALOME_INSTALL_LIBS}" CACHE PATH "Install path: SALOME libs")
SET(SALOME_INSTALL_IDLS "${SALOME_INSTALL_IDLS}" CACHE PATH "Install path: SALOME IDL files")
SET(SALOME_INSTALL_HEADERS "${SALOME_INSTALL_HEADERS}" CACHE PATH "Install path: SALOME headers")
SET(SALOME_INSTALL_SCRIPT_SCRIPTS "${SALOME_INSTALL_SCRIPT_SCRIPTS}" CACHE PATH 
   "Install path: SALOME scripts")
SET(SALOME_INSTALL_SCRIPT_DATA "${SALOME_INSTALL_SCRIPT_DATA}" CACHE PATH 
   "Install path: SALOME script data")
SET(SALOME_INSTALL_SCRIPT_PYTHON "${SALOME_INSTALL_SCRIPT_PYTHON}" CACHE PATH 
   "Install path: SALOME Python scripts")
SET(SALOME_INSTALL_PYTHON "${SALOME_INSTALL_PYTHON}" CACHE PATH "Install path: SALOME Python stuff")
SET(SALOME_INSTALL_PYTHON_SHARED "${SALOME_INSTALL_PYTHON_SHARED}" CACHE PATH 
   "Install path: SALOME Python shared modules")
SET(SALOME_INSTALL_CMAKE "${SALOME_INSTALL_CMAKE}" CACHE PATH "Install path: SALOME CMake files")
SET(SALOME_INSTALL_CMAKE_LOCAL "${SALOME_INSTALL_CMAKE_LOCAL}" CACHE PATH 
    "Install path: local SALOME CMake files") 
SET(SALOME_INSTALL_AMCONFIG_LOCAL "${SALOME_INSTALL_AMCONFIG_LOCAL}" CACHE PATH
  "Install path: local SALOME config files (obsolete, to be removed)")

SET(SALOME_INSTALL_RES "${SALOME_INSTALL_RES}" CACHE PATH "Install path: SALOME resources")
SET(SALOME_INSTALL_DOC "${SALOME_INSTALL_DOC}" CACHE PATH "Install path: SALOME documentation")

# Specific to GUI:
SET(SALOME_GUI_INSTALL_PARAVIEW_LIBS lib/paraview CACHE PATH 
    "Install path: SALOME GUI ParaView libraries")
SET(SALOME_GUI_INSTALL_RES_DATA "${SALOME_INSTALL_RES}/gui" CACHE PATH 
    "Install path: SALOME GUI specific data")
SET(SALOME_GUI_INSTALL_RES_SCRIPTS "${SALOME_INSTALL_RES}/gui" CACHE PATH 
    "Install path: SALOME GUI scripts")
SET(SALOME_GUI_INSTALL_PLUGINS share/salome/plugins/gui CACHE PATH 
    "Install path: SALOME GUI plugins")

MARK_AS_ADVANCED(SALOME_INSTALL_BINS SALOME_INSTALL_LIBS SALOME_INSTALL_IDLS SALOME_INSTALL_HEADERS)
MARK_AS_ADVANCED(SALOME_INSTALL_SCRIPT_SCRIPTS SALOME_INSTALL_SCRIPT_DATA SALOME_INSTALL_SCRIPT_PYTHON)
MARK_AS_ADVANCED(SALOME_INSTALL_APPLISKEL_SCRIPTS  SALOME_INSTALL_APPLISKEL_PYTHON SALOME_INSTALL_CMAKE SALOME_INSTALL_CMAKE_LOCAL SALOME_INSTALL_RES)
MARK_AS_ADVANCED(SALOME_INSTALL_PYTHON SALOME_INSTALL_PYTHON_SHARED)
MARK_AS_ADVANCED(SALOME_GUI_INSTALL_PARAVIEW_LIBS SALOME_GUI_INSTALL_RES_DATA SALOME_GUI_INSTALL_RES_SCRIPTS SALOME_GUI_INSTALL_PLUGINS)
MARK_AS_ADVANCED(SALOME_INSTALL_AMCONFIG_LOCAL SALOME_INSTALL_DOC)

# Accumulate environment variables for GUI module
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_BINS}
                                                 ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_PYTHON}
                                                 ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_PYTHON_SHARED})
SALOME_ACCUMULATE_ENVIRONMENT(LD_LIBRARY_PATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_LIBS}
                                                      ${CMAKE_INSTALL_PREFIX}/${SALOME_GUI_INSTALL_PARAVIEW_LIBS}) 
                                              
# Sources 
# ========
IF(NOT SALOME_LIGHT_ONLY)
  ADD_SUBDIRECTORY(idl)
ENDIF()

ADD_SUBDIRECTORY(adm_local)
ADD_SUBDIRECTORY(resources)
ADD_SUBDIRECTORY(src)
IF(SALOME_BUILD_DOC)
  ADD_SUBDIRECTORY(doc)
ENDIF()
ADD_SUBDIRECTORY(bin)
ADD_SUBDIRECTORY(tools)

# Header configuration
# ====================
SALOME_XVERSION(${PROJECT_NAME})
SALOME_CONFIGURE_FILE(GUI_version.h.in GUI_version.h INSTALL ${SALOME_INSTALL_HEADERS})

# Configuration export
# (here only the level 1 prerequisites are exposed)
# ====================
INCLUDE(CMakePackageConfigHelpers)

# List of targets in this project we want to make visible to the rest of the world.
# They all have to be INSTALL'd with the option "EXPORT ${PROJECT_NAME}TargetGroup"
SET(_${PROJECT_NAME}_exposed_targets 
    caf CAM CASCatch DDS Event LightApp LogWindow ObjBrowser OpenGLUtils
    QDS qtx SalomePrs SalomeStyle std SUITApp suit ViewerTools ViewerData
    ImageComposer
)

# SALOME object specific targets:
IF(SALOME_USE_SALOMEOBJECT)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
       SalomeObject)
ENDIF(SALOME_USE_SALOMEOBJECT)

# GL specific targets:
IF(SALOME_USE_GLVIEWER)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
       GLViewer)
ENDIF(SALOME_USE_GLVIEWER)

# VTK specific targets:
IF(SALOME_USE_VTKVIEWER)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
       VTKViewer vtkTools vtkEDFOverloads)
  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND _${PROJECT_NAME}_exposed_targets 
         SVTK)
  ENDIF(SALOME_USE_SALOMEOBJECT)
ENDIF(SALOME_USE_VTKVIEWER)

# OCC specific targets:
IF(SALOME_USE_OCCVIEWER)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
       OCCViewer)
  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND _${PROJECT_NAME}_exposed_targets 
         SOCC)
  ENDIF(SALOME_USE_SALOMEOBJECT)
ENDIF(SALOME_USE_OCCVIEWER)

# Plot2d specific targets:
IF(SALOME_USE_PLOT2DVIEWER)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
       Plot2d)
  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND _${PROJECT_NAME}_exposed_targets 
         SPlot2d)
  ENDIF(SALOME_USE_SALOMEOBJECT)
ENDIF(SALOME_USE_PLOT2DVIEWER)

# Qx specific targets:
IF(SALOME_USE_QXGRAPHVIEWER)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
       QxScene)
ENDIF(SALOME_USE_QXGRAPHVIEWER)

# GraphicsView specific targets:
IF(SALOME_USE_GRAPHICSVIEW)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
       GraphicsView)
ENDIF(SALOME_USE_GRAPHICSVIEW)

# Python-based packages specific targets:
IF(SALOME_USE_PYCONSOLE)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets 
       PyInterp PyConsole  SalomePyQtGUILight)
  IF(SALOME_USE_PLOT2DVIEWER)
    LIST(APPEND _${PROJECT_NAME}_exposed_targets SalomePyQt)
  ENDIF()
  IF(NOT SALOME_LIGHT_ONLY)
    LIST(APPEND _${PROJECT_NAME}_exposed_targets SalomePyQtGUI)
  ENDIF()
  IF(SALOME_USE_VTKVIEWER)
    LIST(APPEND _${PROJECT_NAME}_exposed_targets SalomePy)
  ENDIF(SALOME_USE_VTKVIEWER)
ENDIF(SALOME_USE_PYCONSOLE)

# CORBA specific targets:
IF(NOT SALOME_LIGHT_ONLY)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets
       SalomeIDLGUI SalomeSession SalomeApp SalomeGuiHelpers SalomeTreeData ToolsGUI)
ENDIF(NOT SALOME_LIGHT_ONLY)

# Add all targets to the build-tree export set
EXPORT(TARGETS ${_${PROJECT_NAME}_exposed_targets}
  FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)

# Create the configuration files:
#   - in the build tree:

#      Ensure the variables are always defined for the configure:
SET(KERNEL_ROOT_DIR "${KERNEL_ROOT_DIR}")
SET(SIP_ROOT_DIR "${SIP_ROOT_DIR}")
SET(QT4_ROOT_DIR "${QT4_ROOT_DIR}")
SET(PYQT4_ROOT_DIR "${PYQT4_ROOT_DIR}")
SET(CAS_ROOT_DIR "${CAS_ROOT_DIR}")
SET(OPENGL_ROOT_DIR "${OPENGL_ROOT_DIR}")
SET(VTK_ROOT_DIR "${VTK_ROOT_DIR}")
SET(QWT_ROOT_DIR "${QWT_ROOT_DIR}")
 
SET(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include")

# Build variables that will be expanded when configuring Salome<MODULE>Config.cmake:
SALOME_CONFIGURE_PREPARE(CAS OpenGL PyQt4 Qt4 Qwt SIP VTK)

CONFIGURE_PACKAGE_CONFIG_FILE(${PROJECT_NAME}Config.cmake.in 
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}"
    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE_LOCAL CMAKE_INSTALL_PREFIX
       KERNEL_ROOT_DIR SIP_ROOT_DIR QT4_ROOT_DIR PYQT4_ROOT_DIR CAS_ROOT_DIR 
       OPENGL_ROOT_DIR VTK_ROOT_DIR QWT_ROOT_DIR)

#   - in the install tree (VSR 16/08/2013: TEMPORARILY COMMENT THIS - TO REMOVE?):
#       Get the relative path of the include directory so 
#       we can register it in the generated configuration files:
#SET(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")
#CONFIGURE_PACKAGE_CONFIG_FILE(adm_local/cmake_files/${PROJECT_NAME}Config.cmake.in 
#    ${PROJECT_BINARY_DIR}/to_install/${PROJECT_NAME}Config.cmake
#    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}"
#    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE_LOCAL CMAKE_INSTALL_PREFIX 
#       KERNEL_ROOT_DIR SIP_ROOT_DIR QT4_ROOT_DIR PYQT4_ROOT_DIR CAS_ROOT_DIR 
#       OPENGL_ROOT_DIR VTK_ROOT_DIR QWT_ROOT_DIR)

WRITE_BASIC_PACKAGE_VERSION_FILE(${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${${PROJECT_NAME_UC}_VERSION}
    COMPATIBILITY AnyNewerVersion)
  
# Install the CMake configuration files:
INSTALL(FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}")

# Install the export set for use with the install-tree
INSTALL(EXPORT ${PROJECT_NAME}TargetGroup DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}" 
        FILE ${PROJECT_NAME}Targets.cmake)
