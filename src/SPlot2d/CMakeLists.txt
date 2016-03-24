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
  ${QWT_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/Plot2d
  ${PROJECT_SOURCE_DIR}/src/Prs
  ${PROJECT_SOURCE_DIR}/src/OBJECT
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${QWT_DEFINITIONS} ${CAS_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${QWT_LIBRARY} qtx suit Plot2d SalomePrs SalomeObject)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  SPlot2d_ViewModel.h
  SPlot2d_ViewWindow.h
)

# header files / no moc processing
SET(_other_HEADERS
  SPlot2d.h
  SPlot2d_Curve.h
  SPlot2d_Histogram.h
  SPlot2d_Prs.h
)

# header files / to install
SET(SPlot2d_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  SPlot2d_Curve.cxx
  SPlot2d_Histogram.cxx
  SPlot2d_Prs.cxx
  SPlot2d_ViewModel.cxx
  SPlot2d_ViewWindow.cxx
)

# sources / to compile
SET(SPlot2d_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(SPlot2d ${SPlot2d_SOURCES})
TARGET_LINK_LIBRARIES(SPlot2d ${_link_LIBRARIES})
INSTALL(TARGETS SPlot2d EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SPlot2d_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
