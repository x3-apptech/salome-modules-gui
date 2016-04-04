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
  ${QT_INCLUDES}
  ${Boost_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${CAS_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/Event
  ${PROJECT_SOURCE_DIR}/src/Style
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/LightApp
  ${PROJECT_SOURCE_DIR}/src/SalomeApp
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${BOOST_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${QT_LIBRARIES}
  ${CAS_KERNEL}
  ${OMNIORB_LIBRARIES}
  ${KERNEL_SalomeNS}
  ${KERNEL_SalomeLifeCycleCORBA}
  ${KERNEL_OpUtil}
  ${KERNEL_SALOMELocalTrace}
  ${KERNEL_SalomeCatalog}
  ${KERNEL_SalomeDSClient}
  ${KERNEL_with_loggerTraceCollector}
  ${KERNEL_SalomeIDLKernel}
  ${KERNEL_SalomeContainer}
  ${KERNEL_SalomeLauncher}
  ${KERNEL_Registry}
  ${KERNEL_SALOMEBasics}
  qtx suit Event SalomeStyle SalomeApp SalomeIDLGUI
)

# --- headers ---

# header files / to install
SET(SalomeSession_HEADERS
  SALOME_Session.hxx
  Session_ServerCheck.hxx
  Session_ServerLauncher.hxx
  Session_ServerThread.hxx
  Session_Session_i.hxx
)

# --- sources ---

# sources / to compile
SET(SalomeSession_SOURCES
  Session_ServerCheck.cxx
  Session_ServerLauncher.cxx
  Session_ServerThread.cxx
  Session_Session_i.cxx
)

# --- rules ---

ADD_LIBRARY(SalomeSession ${SalomeSession_SOURCES})
TARGET_LINK_LIBRARIES(SalomeSession ${_link_LIBRARIES})
INSTALL(TARGETS SalomeSession EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS}) 

ADD_EXECUTABLE(SALOME_Session_Server SALOME_Session_Server.cxx)
TARGET_LINK_LIBRARIES(SALOME_Session_Server ${_link_LIBRARIES} SalomeSession)
INSTALL(TARGETS SALOME_Session_Server EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

INSTALL(FILES ${SalomeSession_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
