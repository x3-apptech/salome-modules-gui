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
  ${PROJECT_SOURCE_DIR}/src/Qtx
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${QT_LIBRARIES}
  qtx
)

# --- headers ---

SET(_moc_HEADERS
  qtlocalpeer.h
  qtsingleapplication.h
  HelpBrowser_Application.h
)

# --- resources ---

# resource files / to be processed by rcc
SET(_rcc_RESOURCES HelpBrowser.qrc)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / rcc wrappings
QT_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

# sources / to compile
SET(_other_SOURCES
  qtlocalpeer.cpp
  qtsingleapplication.cpp
  HelpBrowser_Application.cxx
  HelpBrowser.cxx
)

# sources / to compile
SET(HelpBrowser_SOURCES
  ${_other_SOURCES}
  ${_moc_SOURCES}
  ${_rcc_SOURCES}
)

# --- rules ---

ADD_EXECUTABLE(HelpBrowser ${HelpBrowser_SOURCES})
TARGET_LINK_LIBRARIES(HelpBrowser ${_link_LIBRARIES})
INSTALL(TARGETS HelpBrowser EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})
