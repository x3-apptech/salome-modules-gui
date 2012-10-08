# Copyright (C) 2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)

IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Temporary definitions
SET(DOXYGEN_IS_OK 0)
IF(WINDOWS)
 SET(CPPUNIT_IS_OK 0)
ENDIF(WINDOWS)
SET(GUI_ENABLE_CORBA ON)
SET(ENABLE_VTKVIEWER ON)
SET(ENABLE_SALOMEOBJECT ON)
SET(ENABLE_OCCVIEWER ON)
SET(ENABLE_GLVIEWER ON)
SET(ENABLE_PLOT2DVIEWER ON)
SET(ENABLE_PYCONSOLE ON)
SET(ENABLE_SUPERVGRAPHVIEWER ON)
SET(ENABLE_QXGRAPHVIEWER ON)

ENABLE_TESTING()
SET(VERSION "6.5.0")
SET(VERSION_DEV "1")

SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR})
IF(KERNEL_ROOT_DIR)
  STRING(REPLACE "\\" "/" KERNEL_ROOT_DIR ${KERNEL_ROOT_DIR})
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindPLATFORM.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindPYTHON.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindOMNIORB.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindPTHREADS.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindMPI.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindHDF5.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindBOOST.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindLIBXML2.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindSWIG.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindCPPUNIT.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindDOXYGEN.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindLIBBATCH.cmake)
  INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/FindKERNEL.cmake)
ENDIF(KERNEL_ROOT_DIR)

INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/FindCAS.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/FindQT4.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/FindOPENGL.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/FindVTK.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/FindQWT.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/FindSIPPYQT.cmake)

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

SET(GUI_pythondir lib/python${PYTHON_VERSION}/site-packages)

SET(GUI_salomepythondir ${GUI_pythondir}/salome)
SET(KERNEL_salomepythondir ${GUI_salomepythondir})

SET(GUI_sharedpkgpython_PYTHON ${GUI_salomepythondir}/shared_modules)

SET(GUI_admlocalcmake adm_local/cmake_files)

SET(IDLDIR)                        
IF(GUI_ENABLE_CORBA)
  SET(IDLDIR idl)
ENDIF(GUI_ENABLE_CORBA)

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
