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

INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/UseQT4EXT.cmake)

INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../Event
)

SET(COMMON_FLAGS "${QT_DEFINITIONS} ${PYTHON_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS ${PYTHON_LIBS} ${QT_LIBS} Event)

SET(GUI_HEADERS PyInterp_Watcher.h)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(PyInterp_SOURCES
  PyInterp_Interp.cxx
  PyInterp_Dispatcher.cxx
)

ADD_LIBRARY(PyInterp SHARED ${PyInterp_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(PyInterp PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(PyInterp ${COMMON_LIBS})
INSTALL(TARGETS PyInterp DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  PyInterp.h
  PyInterp_Interp.h
  PyInterp_Dispatcher.h
)
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
