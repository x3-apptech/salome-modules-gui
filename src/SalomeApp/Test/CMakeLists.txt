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
  ${CPPUNIT_INCLUDE_DIRS}
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${CPPUNIT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${CPPUNIT_LIBRARIES} ${PLATFORM_LIBS})

# --- rules ---

ADD_EXECUTABLE(TestSalomeApp TestSalomeApp.cxx)
TARGET_LINK_LIBRARIES(TestSalomeApp ${_link_LIBRARIES})
INSTALL(TARGETS TestSalomeApp DESTINATION ${SALOME_INSTALL_BINS})  # TODO: should it be exported?

SALOME_INSTALL_SCRIPTS("TestSalomeApp.py" ${SALOME_INSTALL_SCRIPT_PYTHON})
