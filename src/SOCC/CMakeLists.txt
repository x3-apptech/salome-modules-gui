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
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../OBJECT
  ${CMAKE_CURRENT_SOURCE_DIR}/../Prs
  ${CMAKE_CURRENT_SOURCE_DIR}/../OCCViewer
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
)

SET(COMMON_FLAGS "${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS ${QT_LIBS} qtx suit SalomeObject SalomePrs OCCViewer)

SET(GUI_HEADERS   
  SOCC_ViewModel.h
  SOCC_ViewWindow.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(SOCC_SOURCES
  SOCC_ViewModel.cxx
  SOCC_Prs.cxx
  SOCC_ViewWindow.cxx
)

ADD_LIBRARY(SOCC SHARED ${SOCC_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(SOCC PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(SOCC ${COMMON_LIBS})
INSTALL(TARGETS SOCC DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  SOCC.h
  SOCC_ViewModel.h
  SOCC_Prs.h
  SOCC_ViewWindow.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
