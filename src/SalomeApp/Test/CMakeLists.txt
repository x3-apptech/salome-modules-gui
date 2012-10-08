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
  ${CPPUNIT_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
)

SET(COMMON_LIBS ${CPPUNIT_LIBS} ${PLATFORM_LIBS})
SET(COMMON_FLAGS "${CPPUNIT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")

ADD_LIBRARY(SalomeAppTest SHARED SalomeAppTest.cxx)
SET_TARGET_PROPERTIES(SalomeAppTest PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(SalomeAppTest ${COMMON_LIBS})
INSTALL(TARGETS SalomeAppTest DESTINATION ${GUI_salomelib_LIBS}) 

ADD_EXECUTABLE(TestSalomeApp TestSalomeApp.cxx)
SET_TARGET_PROPERTIES(TestSalomeApp PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(TestSalomeApp SalomeAppTest ${COMMON_LIBS})
INSTALL(TARGETS TestSalomeApp DESTINATION ${GUI_salomebin_BINS})

INSTALL(FILES SalomeAppTest.hxx DESTINATION ${GUI_salomeinclude_HEADERS})
SALOME_INSTALL_SCRIPTS("TestSalomeApp.py" ${GUI_salomescript_PYTHON})
