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
PROJECT(SALOME_GUI)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)

IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

IF(APPLE)
  SET(CMAKE_C_COMPILER gcc)
  SET(CMAKE_CXX_COMPILER g++)
  # because default is clang(llvm) with mountain lion at least
ENDIF(APPLE)

# Temporary definitions
SET(DOXYGEN_IS_OK 0)
IF(WINDOWS)
 SET(CPPUNIT_IS_OK 0)
ENDIF(WINDOWS)

#Defining all options first
option(SALOME_USE_MPI "Use MPI containers" OFF)
option(SALOME_USE_CORBA "Use distribute application with components" ON)
option(SALOME_USE_DOC "Generate documentation" ON)
option(SALOME_USE_TEST "Generate test" OFF)
OPTION(SALOME_USE_VTKVIEWER "Enable VTK visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_SALOMEOBJECT "Enable Salome Object (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_OCCVIEWER "Enable OCC visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_GLVIEWER "Enable OpenGL visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_PLOT2DVIEWER "Enable Plot2D visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_PYCONSOLE "Enable Python GUI interface (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_SUPERVGRAPHVIEWER "Enable Supervision visualization (Mandatory in classic configurations)" ON)
OPTION(SALOME_USE_QXGRAPHVIEWER "Enable QX graph visualization (Mandatory in classic configurations)" ON)

ENABLE_TESTING()
SET(VERSION "7.1.0")
SET(VERSION_DEV "1")

SET(BUILD_SHARED_LIBS true)

SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "Path to the Salome KERNEL")
IF(EXISTS ${KERNEL_ROOT_DIR})
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindKERNEL.cmake)
ELSE(EXISTS ${KERNEL_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome KERNEL, please define KERNEL_ROOT_DIR")
ENDIF(EXISTS ${KERNEL_ROOT_DIR})

STRING(REPLACE "\\" "/" KERNEL_ROOT_DIR ${KERNEL_ROOT_DIR})

# directories for packages detection
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${KERNEL_ROOT_DIR}/salome_adm/cmake_files ${CMAKE_SOURCE_DIR}/adm_local/cmake_files)

find_package(PLATFORM)


# Threads
SET(CMAKE_THREAD_PREFER_PTHREAD)
find_package(Threads REQUIRED)


# required prerequisites from KERNEL

find_package(SalomePYTHON)
find_package(SalomeSWIG)
find_package(SalomeLIBXML2)
find_package(SalomeHDF5)
find_package(SalomeBOOST)

# required prerequisites for GUI

find_package(SalomeQT4)
find_package(SalomeSIP)
find_package(SalomePYQT4)

# optional prerequisites from KERNEL

if(SALOME_USE_MPI OR HDF5_IS_PARALLEL)
  find_package(SalomeMPI)
endif(SALOME_USE_MPI OR HDF5_IS_PARALLEL)

if(SALOME_USE_CORBA)
  find_package(OMNIORB)
endif(SALOME_USE_CORBA)

if(SALOME_USE_TEST)
  find_package(SalomeCPPUNIT)
endif(SALOME_USE_TEST)

if(SALOME_USE_DOC)
  find_package(SalomeDOXYGEN)
  find_package(SalomeSPHINX)
endif(SALOME_USE_DOC)

if(SALOME_USE_LIBBATCH)
  find_package(LIBBATCH)
endif(SALOME_USE_LIBBATCH)


# optional prerequisites for GUI

if(SALOME_USE_OCCVIEWER)
  find_package(SalomeCAS)
endif(SALOME_USE_OCCVIEWER)

if(SALOME_USE_GLVIEWER)
  find_package(OpenGL REQUIRED)
endif(SALOME_USE_GLVIEWER)

if(SALOME_USE_VTKVIEWER)
  find_package(SalomeVTK)
endif(SALOME_USE_VTKVIEWER)

if(SALOME_USE_PLOT2DVIEWER)
  find_package(SalomeQWT)
endif(SALOME_USE_PLOT2DVIEWER)

EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; t=sys.argv[-1].split(\".\") ; t[:]=(int(elt) for elt in t) ; sys.stdout.write(\"0x%02x%02x%02x\"%tuple(t))" ${VERSION}
  OUTPUT_VARIABLE XVERSION)

SET(GUI_salomebin_BINS bin/salome)

SET(GUI_salomelib_LIBS lib/salome)
SET(GUI_paraviewlib_LIBS lib/paraview)

SET(GUI_salomeinclude_HEADERS include/salome)

SET(GUI_salomeres_DATA share/salome/resources/gui)

SET(GUI_salomeres_SCRIPTS share/salome/resources/gui)

SET(GUI_salomepluginsdir share/salome/plugins/gui)

SET(GUI_salomescript_SCRIPTS bin/salome)

SET(GUI_salomescript_DATA bin/salome)

SET(GUI_salomescript_PYTHON bin/salome)
SET(KERNEL_salomescript_PYTHON ${GUI_salomescript_PYTHON})

SET(GUI_appliskel_SCRIPTS bin/salome/appliskel)

SET(GUI_appliskel_PYTHON bin/salome/appliskel)

SET(GUI_pythondir lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)

SET(GUI_salomepythondir ${GUI_pythondir}/salome)
SET(KERNEL_salomepythondir ${GUI_salomepythondir})

SET(GUI_sharedpkgpython_PYTHON ${GUI_salomepythondir}/shared_modules)

SET(GUI_admlocalcmake adm_local/cmake_files)

SET(IDLDIR)                        
IF(SALOME_USE_CORBA)
  SET(IDLDIR idl)
ENDIF(SALOME_USE_CORBA)

SET(SUBDIRS
  ${IDLDIR}
  adm_local
  resources
  src
  doc
  bin
  tools
)
FOREACH(dir ${SUBDIRS})
  ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})

SET(input ${CMAKE_CURRENT_SOURCE_DIR}/GUI_version.h.in)
SET(output ${CMAKE_CURRENT_BINARY_DIR}/GUI_version.h)
MESSAGE(STATUS "Creation of ${output}")
CONFIGURE_FILE(${input} ${output} @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/GUI_version.h DESTINATION ${GUI_salomeinclude_HEADERS})
