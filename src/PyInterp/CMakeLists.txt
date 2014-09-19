# Copyright (C) 2012-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE(UseQt4Ext)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
  ${PYTHON_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/Event
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${PYTHON_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${PYTHON_LIBRARIES} Event)

# --- headers ---

# header files / to be processed by moc / internal
# TODO: check for MSVS project
SET(_moc_internal_HEADERS PyInterp_Watcher.h)

# header files / no moc processing
SET(_other_HEADERS
  PyInterp.h
  PyInterp_Dispatcher.h
  PyInterp_Event.h
  PyInterp_Interp.h
  PyInterp_Request.h
  PyInterp_Utils.h
)

# header files / to install
SET(PyInterp_HEADERS ${_other_HEADERS})

# --- sources ---

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_internal_HEADERS})

# sources / static
SET(_other_SOURCES
  PyInterp_Dispatcher.cxx
  PyInterp_Event.cxx
  PyInterp_Interp.cxx
  PyInterp_Request.cxx
  PyInterp_Utils.cxx
)

# sources / to compile
SET(PyInterp_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(PyInterp ${PyInterp_SOURCES})
TARGET_LINK_LIBRARIES(PyInterp ${_link_LIBRARIES})
INSTALL(TARGETS PyInterp EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${PyInterp_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
