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
  ${QT_INCLUDES}
  ${PYTHON_INCLUDE_DIRS}
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${PYTHON_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${PYTHON_LIBRARIES})

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  PyInterp_Dispatcher.h
)

# header files / no moc processing
SET(_other_HEADERS
  PyInterp.h
  PyInterp_Event.h
  PyInterp_Interp.h
  PyInterp_Request.h
  PyInterp_Utils.h
)

# header files / to install
SET(PyInterp_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  PyInterp_Dispatcher.cxx
  PyInterp_Event.cxx
  PyInterp_Interp.cxx
  PyInterp_Request.cxx
)

# sources / to compile
SET(PyInterp_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(PyInterp ${PyInterp_SOURCES})
TARGET_LINK_LIBRARIES(PyInterp ${_link_LIBRARIES})
INSTALL(TARGETS PyInterp EXPORT ${TOOLS_EXPORT_NAME}TargetGroup DESTINATION ${PYINTERP_INSTALL_LIBS})

INSTALL(FILES ${PyInterp_HEADERS} DESTINATION ${PYINTERP_INSTALL_HEADERS})
