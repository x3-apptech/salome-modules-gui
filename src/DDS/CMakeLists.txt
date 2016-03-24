# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${CAS_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${CAS_KERNEL} ${CAS_OCAF})

# --- headers ---

# header files / to install
SET(DDS_HEADERS
  DDS.h
  DDS_DicGroup.h
  DDS_DicItem.h
  DDS_Dictionary.h
  DDS_KeyWords.h
)

# --- sources ---

# sources / static
SET(DDS_SOURCES
  DDS_DicGroup.cxx
  DDS_DicItem.cxx
  DDS_Dictionary.cxx
  DDS_KeyWords.cxx
)

# --- rules ---

ADD_LIBRARY(DDS ${DDS_SOURCES})
TARGET_LINK_LIBRARIES(DDS ${_link_LIBRARIES})
INSTALL(TARGETS DDS EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${DDS_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
