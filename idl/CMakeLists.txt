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

INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/UseOMNIORB.cmake)

INCLUDE_DIRECTORIES(
  ${OMNIORB_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_BINARY_DIR}
)

SET(SalomeIDLGUI_IDLSOURCES
  SalomeApp_Engine.idl
)

SET(IDL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${KERNEL_ROOT_DIR}/idl/salome
)

SET(TYPE SHARED)
IF(WINDOWS)
  SET(TYPE STATIC)
ENDIF(WINDOWS)

OMNIORB_ADD_MODULE(SalomeIDLGUI "${SalomeIDLGUI_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" ${TYPE})
TARGET_LINK_LIBRARIES(SalomeIDLGUI ${CORBA_LIBS})
SET_TARGET_PROPERTIES(SalomeIDLGUI PROPERTIES COMPILE_FLAGS "${OMNIORB_DEFINITIONS}")
            
INSTALL(TARGETS SalomeIDLGUI DESTINATION ${GUI_salomelib_LIBS})
