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
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)
PROJECT(SalomeGUI C CXX)

# Ensure a proper linker behavior:
CMAKE_POLICY(SET CMP0003 NEW)

#set(CMAKE_PREFIX_PATH "")
#set(_compo QtCore QtGui)
#find_package(Qt4 REQUIRED COMPONENTS _compo)

# Versioning
# ===========
# Project name, upper case
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

SET(${PROJECT_NAME_UC}_MAJOR_VERSION 7)
SET(${PROJECT_NAME_UC}_MINOR_VERSION 2)
SET(${PROJECT_NAME_UC}_PATCH_VERSION 0)
SET(${PROJECT_NAME_UC}_VERSION
  ${${PROJECT_NAME_UC}_MAJOR_VERSION}.${${PROJECT_NAME_UC}_MINOR_VERSION}.${${PROJECT_NAME_UC}_PATCH_VERSION})
# Temporary backward compatibility:
SET(VERSION ${${PROJECT_NAME_UC}_VERSION})
SET(VERSION_DEV "0")

# Find KERNEL
# ===========
SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "Path to the Salome KERNEL")
IF(EXISTS ${KERNEL_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${KERNEL_ROOT_DIR}/salome_adm/cmake_files")
  INCLUDE(SalomeMacros)
  FIND_PACKAGE(SalomeKERNEL REQUIRED)
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
# (some options have already been defined in KERNEL) 
# ============
OPTION(SALOME_USE_MPI "Use MPI containers" ${SALOME_USE_MPI})
OPTION(SALOME_BUILD_DOC "Generate SALOME GUI documentation" ${SALOME_BUILD_DOC})
OPTION(SALOME_BUILD_TESTS "Build SALOME tests" ${SALOME_BUILD_TESTS})

# Advanced options:
OPTION(SALOME_LIGHT_ONLY "Build SALOME Light only (no CORBA)" ${SALOME_LIGHT_ONLY})

OPTION(SALOME_USE_VTKVIEWER "Enable VTK visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_SALOMEOBJECT "Enable Salome Object (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_OCCVIEWER "Enable OCC visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_GLVIEWER "Enable OpenGL visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_PLOT2DVIEWER "Enable Plot2D visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_PYCONSOLE "Enable Python GUI interface (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_SUPERVGRAPHVIEWER "Enable Supervision visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_QXGRAPHVIEWER "Enable QX graph visualization (Mandatory in classic configurations)" ON)

MARK_AS_ADVANCED(SALOME_LIGHT_ONLY SALOME_USE_LIBBATCH SALOME_USE_VTKVIEWER SALOME_USE_VTKVIEWER)
MARK_AS_ADVANCED(SALOME_USE_SALOMEOBJECT SALOME_USE_OCCVIEWER SALOME_USE_GLVIEWER SALOME_USE_PLOT2DVIEWER)
MARK_AS_ADVANCED(SALOME_USE_PYCONSOLE SALOME_USE_SUPERVGRAPHVIEWER SALOME_USE_QXGRAPHVIEWER)

# Prerequisites
# =============
# Find "big" prerequisites first - they reference themselves many others
#   -> this can help finding the smaller prerequisites and detect conficts.
# In our case KERNEL has already loaded many prereq:

##
## From KERNEL:
##

# Various
FIND_PACKAGE(SalomePython REQUIRED)
FIND_PACKAGE(SalomePThread REQUIRED)
FIND_PACKAGE(SalomeSWIG REQUIRED)
FIND_PACKAGE(SalomeLibXml2 REQUIRED)
FIND_PACKAGE(SalomeBoost REQUIRED)
FIND_PACKAGE(SalomeHDF5 REQUIRED COMPONENTS C)

# Other KERNEL optionals:
IF(SALOME_USE_MPI)
  FIND_PACKAGE(SalomeMPI REQUIRED)
ENDIF()
IF(NOT SALOME_LIGHT_ONLY)
  FIND_PACKAGE(SalomeOmniORB REQUIRED)
  FIND_PACKAGE(SalomeOmniORBPy REQUIRED)
ENDIF() 
IF(SALOME_BUILD_TESTS)
  ENABLE_TESTING()
  FIND_PACKAGE(SalomeCppUnit REQUIRED)  
ENDIF()
IF(SALOME_BUILD_DOC)
  FIND_PACKAGE(SalomeDoxygen REQUIRED)
  FIND_PACKAGE(SalomeGraphviz)          # Remains optional even here.
  FIND_PACKAGE(SalomeSphinx REQUIRED)
ENDIF()

##
## GUI specifics
##

FIND_PACKAGE(SalomeSIP REQUIRED)  # should come after Python and before PyQt4

# Qt4
FIND_PACKAGE(SalomeQt4 REQUIRED COMPONENTS QtCore QtGui QtXml QtWebKit QtOpenGL)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(SalomePyQt4 REQUIRED) 

# Optional prerequisites for GUI
IF(SALOME_USE_OCCVIEWER)
  FIND_PACKAGE(SalomeCAS REQUIRED)
ENDIF()
IF(SALOME_USE_GLVIEWER)
  FIND_PACKAGE(SalomeOpenGL REQUIRED) 
ENDIF()
IF(SALOME_USE_VTKVIEWER)
  # Required components are listed in the FindSalomeVTK.cmake file: 
  FIND_PACKAGE(SalomeVTK 6.0 REQUIRED) 
ENDIF()
IF(SALOME_USE_PLOT2DVIEWER)
  FIND_PACKAGE(SalomeQwt REQUIRED)  
ENDIF()

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
SET(SALOME_INSTALL_APPLISKEL_SCRIPTS "${SALOME_INSTALL_APPLISKEL_SCRIPTS}" CACHE PATH 
   "Install path: SALOME application skeleton - scripts")
SET(SALOME_INSTALL_APPLISKEL_PYTHON "${SALOME_INSTALL_APPLISKEL_PYTHON}" CACHE PATH 
   "Install path: SALOME application skeleton - Python")
SET(SALOME_INSTALL_PYTHON "${SALOME_INSTALL_PYTHON}" CACHE PATH "Install path: SALOME Python stuff")
SET(SALOME_INSTALL_PYTHON_SHARED "${SALOME_INSTALL_PYTHON_SHARED}" CACHE PATH 
   "Install path: SALOME Python shared modules")
SET(SALOME_INSTALL_CMAKE "${SALOME_INSTALL_CMAKE}" CACHE PATH "Install path: SALOME CMake files")
SET(SALOME_INSTALL_CMAKE_LOCAL "${SALOME_INSTALL_CMAKE_LOCAL}" CACHE PATH 
    "Install path: local SALOME CMake files") 

SET(SALOME_INSTALL_RES "${SALOME_INSTALL_RES}" CACHE PATH "Install path: SALOME resources")

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

# Sources 
# ========
IF(NOT SALOME_LIGHT_ONLY)
  SET(IDLDIR idl)
ENDIF()

SET(SUBDIRS
  ${IDLDIR}
  adm_local
  resources
  src
  doc
  bin
  tools)
  
FOREACH(dir ${SUBDIRS})
  ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})

# Header configuration
# ====================
EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; t=sys.argv[-1].split(\".\") ; t[:]=(int(elt) for elt in t) ; sys.stdout.write(\"0x%02x%02x%02x\"%tuple(t))" ${${PROJECT_NAME_UC}_VERSION}
  OUTPUT_VARIABLE XVERSION)

SET(input ${CMAKE_CURRENT_SOURCE_DIR}/GUI_version.h.in)
SET(output ${CMAKE_CURRENT_BINARY_DIR}/GUI_version.h)
MESSAGE(STATUS "Creation of ${output}")
CONFIGURE_FILE(${input} ${output} @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/GUI_version.h DESTINATION ${SALOME_INSTALL_HEADERS})

# Configuration export
# (here only the level 1 prerequisites are exposed)
# ====================
INCLUDE(CMakePackageConfigHelpers)

# List of targets in this project we want to make visible to the rest of the world.
# They all have to be INSTALL'd with the option "EXPORT ${PROJECT_NAME}TargetGroup"
SET(_${PROJECT_NAME}_exposed_targets 
    caf CAM CASCatch  DDS Event GLViewer  LightApp  LogWindow ObjBrowser  
    OCCViewer OpenGLUtils Plot2d  PyConsole PyInterp  QDS qtx QxScene SalomeApp 
    SalomeIDLGUI  SalomeObject  SalomePrs SalomePyQtGUILight  SalomePyQtGUI 
    SalomePyQt  SalomePy  SalomeSession SalomeStyle SOCC  SPlot2d std SUITApp suit  
    SUPERVGraph SVTK  ToolsGUI  ViewerTools VTKViewer  vtkTools vtkEDFOverloads SalomeGuiHelpers  
    SalomeTreeData 
)

IF(SALOME_BUILD_TESTS)
  LIST(APPEND _${PROJECT_NAME}_exposed_targets SalomeAppTest) 
ENDIF()

# Add all targets to the build-tree export set
EXPORT(TARGETS ${_${PROJECT_NAME}_exposed_targets}
  FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)

# Create the configuration files:
#   - in the build tree:

#      Ensure the variables are always defined for the configure:
SET(SIP_ROOT_DIR "${SIP_ROOT_DIR}")
SET(QT4_ROOT_DIR "${QT4_ROOT_DIR}")
SET(PYQT4_ROOT_DIR "${PYQT4_ROOT_DIR}")
SET(CAS_ROOT_DIR "${CAS_ROOT_DIR}")
SET(OPENGL_ROOT_DIR "${OPENGL_ROOT_DIR}")
SET(VTK_ROOT_DIR "${VTK_ROOT_DIR}")
SET(QWT_ROOT_DIR "${QWT_ROOT_DIR}")
 
SET(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include")
CONFIGURE_PACKAGE_CONFIG_FILE(adm_local/cmake_files/${PROJECT_NAME}Config.cmake.in 
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE}"
    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE CMAKE_INSTALL_PREFIX
       KERNEL_ROOT_DIR SIP_ROOT_DIR QT4_ROOT_DIR PYQT4_ROOT_DIR CAS_ROOT_DIR 
       OPENGL_ROOT_DIR VTK_ROOT_DIR QWT_ROOT_DIR)

#   - in the install tree:
#       Get the relative path of the include directory so 
#       we can register it in the generated configuration files:
SET(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")
CONFIGURE_PACKAGE_CONFIG_FILE(adm_local/cmake_files/${PROJECT_NAME}Config.cmake.in 
    ${PROJECT_BINARY_DIR}/to_install/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE}"
    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE CMAKE_INSTALL_PREFIX 
       KERNEL_ROOT_DIR SIP_ROOT_DIR QT4_ROOT_DIR PYQT4_ROOT_DIR CAS_ROOT_DIR 
       OPENGL_ROOT_DIR VTK_ROOT_DIR QWT_ROOT_DIR)

WRITE_BASIC_PACKAGE_VERSION_FILE(${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${${PROJECT_NAME_UC}_VERSION}
    COMPATIBILITY AnyNewerVersion)
  
# Install the CMake configuration files:
INSTALL(FILES
  "${PROJECT_BINARY_DIR}/to_install/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${SALOME_INSTALL_CMAKE}")

# Install the export set for use with the install-tree
INSTALL(EXPORT ${PROJECT_NAME}TargetGroup DESTINATION "${SALOME_INSTALL_CMAKE}" 
        FILE ${PROJECT_NAME}Targets.cmake)
