# Copyright (C) 2012  CEA/DEN, EDF R&D
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

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/SalomeMacros.cmake)

SET_SOURCE_FILES_PROPERTIES(libSALOME_Swig.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(libSALOME_Swig.i PROPERTIES SWIG_DEFINITIONS "-shadow")
SET_SOURCE_FILES_PROPERTIES(libSALOME_SwigPYTHON_wrap.cxx PROPERTIES COMPILE_FLAGS "-DHAVE_CONFIG_H")

INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${QWT_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/salome_adm
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../LightApp
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../ObjBrowser
  ${CMAKE_CURRENT_SOURCE_DIR}/../LightApp
  ${CMAKE_CURRENT_SOURCE_DIR}/../SOCC
  ${CMAKE_CURRENT_SOURCE_DIR}/../SVTK
  ${CMAKE_CURRENT_SOURCE_DIR}/../Event
  ${CMAKE_CURRENT_SOURCE_DIR}/../OBJECT
  ${CMAKE_CURRENT_SOURCE_DIR}/../CAM
  ${CMAKE_CURRENT_SOURCE_DIR}/../STD
  ${CMAKE_CURRENT_SOURCE_DIR}/../OCCViewer
  ${CMAKE_CURRENT_SOURCE_DIR}/../Prs
  ${CMAKE_CURRENT_SOURCE_DIR}/../VTKViewer
  ${CMAKE_CURRENT_SOURCE_DIR}/../SPlot2d
  ${CMAKE_CURRENT_SOURCE_DIR}/../Plot2d
)

SET(COMMON_FLAGS "${PYTHON_DEFINITIONS} ${CAS_DEFINITIONS} ${QT_DEFINITIONS} ${QWT_DEFINITIONS} ${VTK_DEFINITIONS} ${BOOST_DEFINITIONS} ${PLATFORM_DEFINITIONS}")

SWIG_ADD_MODULE(libSALOME_Swig python libSALOME_Swig.i SALOMEGUI_Swig.cxx)
SWIG_LINK_LIBRARIES(libSALOME_Swig LightApp ${PYTHON_LIBS})
SET_TARGET_PROPERTIES(_libSALOME_Swig PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")

IF(WINDOWS)
  SET_TARGET_PROPERTIES(_libSALOME_Swig PROPERTIES DEBUG_OUTPUT_NAME _libSALOME_Swig_d)
ENDIF(WINDOWS)

INSTALL(TARGETS _libSALOME_Swig DESTINATION ${GUI_salomepythondir})

SET(COMMON_HEADERS
  libSALOME_Swig.i
  SALOMEGUI_Swig.hxx
  SALOMEGUI_Swig.i
)
INSTALL(FILES ${COMMON_HEADERS} DESTINATION ${GUI_salomeinclude_HEADERS})
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libSALOME_Swig.py DESTINATION ${GUI_salomepythondir})

#
# ===============================================================
# Files to be installed
# ===============================================================
#
SET(PYTHON_FILES
  examplevtk1.py 
  test_table.py 
  test_big_table.py 
  test_many_objects.py 
  test_remove_ref.py
)
SALOME_INSTALL_SCRIPTS("${PYTHON_FILES}" ${GUI_salomescript_DATA})
SALOME_INSTALL_SCRIPTS("SALOMEGUI_shared_modules.py" ${GUI_sharedpkgpython_PYTHON})

CONFIGURE_FILE(supervisionexample.py.in supervisionexample.py @ONLY)
CONFIGURE_FILE(supervisiongeomexample.py.in supervisiongeomexample.py @ONLY)
SET(script_DATA 
  ${CMAKE_CURRENT_BINARY_DIR}/libSALOME_Swig.py 
  ${CMAKE_CURRENT_BINARY_DIR}/supervisionexample.py 
  ${CMAKE_CURRENT_BINARY_DIR}/supervisiongeomexample.py
)
INSTALL(FILES ${script_DATA} DESTINATION ${GUI_salomescript_DATA}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
