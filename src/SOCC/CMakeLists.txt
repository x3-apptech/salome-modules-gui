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

INCLUDE(UseQtExt)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/Prs
  ${PROJECT_SOURCE_DIR}/src/OCCViewer
  ${PROJECT_SOURCE_DIR}/src/ViewerData
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${CAS_DEFINITIONS} ${OGL_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES
  ${QT_LIBRARIES} ${CAS_KERNEL} ${CAS_VIEWER}
  qtx suit SalomeObject SalomePrs OCCViewer
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  SOCC_ViewModel.h
  SOCC_ViewWindow.h
)

# header files / no moc processing
SET(_other_HEADERS
  SOCC.h
  SOCC_Prs.h
)

# header files / to install
SET(SOCC_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  SOCC_Prs.cxx
  SOCC_ViewModel.cxx
  SOCC_ViewWindow.cxx
)

# sources / to compile
SET(SOCC_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(SOCC ${SOCC_SOURCES})
TARGET_LINK_LIBRARIES(SOCC ${_link_LIBRARIES})
INSTALL(TARGETS SOCC EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SOCC_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
