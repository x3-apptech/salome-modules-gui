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

INCLUDE_DIRECTORIES(
  ${PTHREAD_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
)

SET(COMMON_LIBS
  ${QT_LIBS}
  ${CAS_LDPATH}
  ${TKV3d}
)

SET(SalomeObject_SOURCES
  SALOME_InteractiveObject.cxx
  SALOME_AISShape.cxx
  SALOME_AISObject.cxx
  SALOME_ListIO_0.cxx
  SALOME_ListIteratorOfListIO_0.cxx
  SALOME_ListNodeOfListIO_0.cxx
  SALOME_Filter.cxx
  SALOME_TypeFilter.cxx
  SALOME_DataMapOfIOMapOfInteger_0.cxx
  SALOME_DataMapNodeOfDataMapOfIOMapOfInteger_0.cxx
  SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger_0.cxx
)

ADD_LIBRARY(SalomeObject SHARED ${SalomeObject_SOURCES})
SET_TARGET_PROPERTIES(SalomeObject PROPERTIES COMPILE_FLAGS "${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(SalomeObject ${COMMON_LIBS})
INSTALL(TARGETS SalomeObject DESTINATION ${GUI_salomelib_LIBS})

FILE(GLOB COMMON_HEADERS_HXX "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")
INSTALL(FILES ${COMMON_HEADERS_HXX} SALOME_Selection.h DESTINATION ${GUI_salomeinclude_HEADERS})
