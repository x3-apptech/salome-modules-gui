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
)

SET(CASCatch_SOURCES
  CASCatch_CatchSignals.cxx
  CASCatch_ErrorHandler.cxx
  CASCatch_Failure.cxx
)

ADD_LIBRARY(CASCatch SHARED ${CASCatch_SOURCES})
SET_TARGET_PROPERTIES(CASCatch PROPERTIES COMPILE_FLAGS "${CAS_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(CASCatch ${CAS_KERNEL})
INSTALL(TARGETS CASCatch DESTINATION ${GUI_salomelib_LIBS})

FILE(GLOB COMMON_HEADERS_HXX "${CMAKE_CURRENT_SOURCE_DIR}/*.hxx")
INSTALL(FILES ${COMMON_HEADERS_HXX} DESTINATION ${GUI_salomeinclude_HEADERS})

