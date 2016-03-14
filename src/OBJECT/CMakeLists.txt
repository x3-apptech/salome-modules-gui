# Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/ViewerData
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${CAS_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${CAS_LDPATH} ${CAS_KERNEL} ${CAS_TKV3d} ViewerData)

# --- headers ---

# header files / to install
SET(SalomeObject_HEADERS
  SALOME_AISObject.hxx
  SALOME_AISShape.hxx
  SALOME_DataMapOfIOMapOfInteger.hxx
  SALOME_Filter.hxx
  SALOME_InteractiveObject.hxx
  SALOME_ListIO.hxx
  SALOME_Selection.h
  SALOME_TypeFilter.hxx
)

# --- sources ---

# sources / to compile

SET(SalomeObject_SOURCES
  SALOME_InteractiveObject.cxx
  SALOME_AISShape.cxx
  SALOME_AISObject.cxx
  SALOME_Filter.cxx
  SALOME_TypeFilter.cxx
)

# --- rules ---

ADD_LIBRARY(SalomeObject ${SalomeObject_SOURCES})
TARGET_LINK_LIBRARIES(SalomeObject ${_link_LIBRARIES})
INSTALL(TARGETS SalomeObject EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomeObject_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
