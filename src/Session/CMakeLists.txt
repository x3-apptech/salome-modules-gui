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
  ${QT_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../Event
  ${CMAKE_CURRENT_SOURCE_DIR}/../Style
  ${CMAKE_CURRENT_SOURCE_DIR}/../CASCatch
)

SET(COMMON_LIBS 
  ${SalomeNS}
  ${SalomeLifeCycleCORBA}
  ${OpUtil}
  ${SALOMELocalTrace}
  ${SalomeCatalog}
  ${SalomeDSClient}
  ${CAS_KERNEL}
  ${with_loggerTraceCollector}
  ${SalomeIDLKernel}
  ${SalomeContainer}
  ${TOOLSDS}
  ${SalomeLauncher}
  ${Registry}
  ${SALOMEBasics}
  suit
  Event
  SalomeIDLGUI
  SalomeStyle
  ${QT_LIBS}
)
SET(COMMON_FLAGS "${QT_DEFINITIONS} ${LIBXML_DEFINITIONS} ${PYTHON_DEFINITIONS} ${HDF5_DEFINITIONS} ${BOOST_DEFINITIONS} ${CAS_DEFINITIONS} ${CORBA_DEFINITIONS} ${PLATFORM_DEFINITIONS}")

IF(ENABLE_TESTRECORDER)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DENABLE_TESTRECORDER ${TESTRECORDER_DEFINITIONS}")
  SET(COMMON_LIBS ${COMMON_LIBS} ${TESTRECORDER_LIBS})
ENDIF(ENABLE_TESTRECORDER)

SET(SalomeSession_SOURCES
  Session_Session_i.cxx
  Session_ServerThread.cxx
  Session_ServerLauncher.cxx
  Session_ServerCheck.cxx
)
ADD_LIBRARY(SalomeSession SHARED ${SalomeSession_SOURCES})
SET_TARGET_PROPERTIES(SalomeSession PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(SalomeSession ${COMMON_LIBS})
INSTALL(TARGETS SalomeSession DESTINATION ${GUI_salomelib_LIBS}) 

SET(SALOME_Session_Server_LIBS
  ${COMMON_LIBS}
  ${SalomeContainer}
  ${SalomeResourcesManager}
  ${TOOLSDS}
  ${SalomeHDFPersist}
  ${SalomeDSImpl}
  ${SalomeGenericObj}
  ${Registry}
  ${SalomeNotification}
  ${SALOMEBasics}
  ${SalomeLauncher}
  qtx
  SalomeIDLGUI
  SalomeSession
  ${HDF5_LIBS}
  ${PYTHON_LIBS}
  ${OMNIORB_LIBS}
)
ADD_EXECUTABLE(SALOME_Session_Server SALOME_Session_Server.cxx)
SET_TARGET_PROPERTIES(SALOME_Session_Server PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(SALOME_Session_Server ${SALOME_Session_Server_LIBS})
INSTALL(TARGETS SALOME_Session_Server DESTINATION ${GUI_salomebin_BINS})

SET(COMMON_HEADERS_H
  Session_Session_i.hxx
  SALOME_Session.hxx
  Session_ServerLauncher.hxx
  Session_ServerThread.hxx
  Session_ServerCheck.hxx
)
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
