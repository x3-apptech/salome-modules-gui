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
  ${QT_INCLUDES}
  ${LIBXML2_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
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
  ${KERNEL_SalomeNS}
  ${KERNEL_SalomeLifeCycleCORBA}
  ${KERNEL_OpUtil}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SalomeCatalog}
  ${KERNEL_SalomeDSClient}
  ${CAS_KERNEL}
  ${KERNEL_with_loggerTraceCollector}
  ${KERNEL_SalomeIDLKernel}
  ${KERNEL_SalomeContainer}
  ${KERNEL_TOOLSDS}
  ${KERNEL_SalomeLauncher}
  ${KERNEL_Registry}
  ${KERNEL_SALOMEBasics}
  suit
  Event
  SalomeIDLGUI
  SalomeStyle
  ${QT_LIBRARIES}
)

ADD_DEFINITIONS(${QT_DEFINITIONS} ${LIBXML2_DEFINITIONS} ${PYTHON_DEFINITIONS} ${HDF5_DEFINITIONS} ${BOOST_DEFINITIONS} ${CAS_DEFINITIONS} ${OMNIORB_DEFINITIONS})

IF(ENABLE_TESTRECORDER)
  ADD_DEFINITIONS("-DENABLE_TESTRECORDER ${TESTRECORDER_DEFINITIONS}")
  SET(COMMON_LIBS ${COMMON_LIBS} ${TESTRECORDER_LIBS})
ENDIF(ENABLE_TESTRECORDER)

SET(SalomeSession_SOURCES
  Session_Session_i.cxx
  Session_ServerThread.cxx
  Session_ServerLauncher.cxx
  Session_ServerCheck.cxx
)
ADD_LIBRARY(SalomeSession ${SalomeSession_SOURCES})
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
  ${HDF5_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${OMNIORB_LIBRARIES}
)
ADD_EXECUTABLE(SALOME_Session_Server SALOME_Session_Server.cxx)
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
