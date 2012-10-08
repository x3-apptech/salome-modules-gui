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
  ${QWT_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../Plot2d
  ${CMAKE_CURRENT_SOURCE_DIR}/../Prs
  ${CMAKE_CURRENT_SOURCE_DIR}/../OBJECT
)

SET(COMMON_FLAGS "${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS} ${QWT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS suit Plot2d SalomePrs SalomeObject)

SET(GUI_HEADERS   
  SPlot2d_ViewModel.h
  SPlot2d_ViewWindow.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(SPlot2d_SOURCES
  SPlot2d_Curve.cxx
  SPlot2d_Histogram.cxx
  SPlot2d_Prs.cxx
  SPlot2d_ViewModel.cxx
  SPlot2d_ViewWindow.cxx
)

ADD_LIBRARY(SPlot2d SHARED ${SPlot2d_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(SPlot2d PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(SPlot2d ${COMMON_LIBS})
INSTALL(TARGETS SPlot2d DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  SPlot2d.h
  SPlot2d_Curve.h
  SPlot2d_Histogram.h
  SPlot2d_Prs.h
  SPlot2d_ViewModel.h
  SPlot2d_ViewWindow.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
