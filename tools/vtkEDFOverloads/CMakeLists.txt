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
  ${VTK_INCLUDE_DIRS}
)

ADD_LIBRARY(vtkTools SHARED vtkEDFCutter.cxx)
SET_TARGET_PROPERTIES(vtkTools PROPERTIES COMPILE_FLAGS "${VTK_DEFINITIONS}")
TARGET_LINK_LIBRARIES(vtkTools ${VTK_LIBS})
INSTALL(TARGETS vtkTools DESTINATION ${GUI_salomelib_LIBS})

ADD_LIBRARY(vtkEDFOverloads SHARED vtkEDFFactory.cxx)
SET_TARGET_PROPERTIES(vtkEDFOverloads PROPERTIES COMPILE_FLAGS "${VTK_DEFINITIONS}")
TARGET_LINK_LIBRARIES(vtkEDFOverloads ${VTK_LIBS} vtkTools)
INSTALL(TARGETS vtkEDFOverloads DESTINATION ${GUI_paraviewlib_LIBS})

FILE(GLOB COMMON_HEADERS_H "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
