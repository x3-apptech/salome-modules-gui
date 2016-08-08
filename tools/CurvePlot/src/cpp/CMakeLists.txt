# Copyright (C) 2012-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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

ADD_SUBDIRECTORY(test)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIRS}
  ${NUMPY_INCLUDE_DIR}
  ${GUI_INCLUDE_DIRS}
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${PYTHON_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${PYTHON_LIBRARIES} ${GUI_PyInterp})

# --- headers ---

# header files / no moc processing
SET(_other_HEADERS
  CurvePlot.hxx
#  ColumnVector.hxx
)

# sources / static
SET(_other_SOURCES
  CurvePlot.cxx
#  ColumnVector.cxx
)
# --- rules ---

ADD_LIBRARY(CurvePlot ${_other_SOURCES})
TARGET_LINK_LIBRARIES(CurvePlot ${_link_LIBRARIES})
INSTALL(TARGETS CurvePlot EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${_other_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
