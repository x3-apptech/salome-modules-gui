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
  ${CAS_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/STD
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${CAS_DEFINITIONS} ${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${CAS_OCAF} ${CAS_OCAFVIS} qtx suit std)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
  CAF_Application.h
  CAF_Operation.h
  CAF_Study.h
)

# header files / no moc processing
SET(_other_HEADERS
  CAF.h
  CAF_Tools.h
)

# header files / to install
SET(caf_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/CAF_msg_en.ts
  resources/CAF_msg_fr.ts
  resources/CAF_msg_ja.ts
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  CAF_Application.cxx
  CAF_Operation.cxx
  CAF_Study.cxx
  CAF_Tools.cxx
)

# sources / to compile
SET(caf_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(caf ${caf_SOURCES})
TARGET_LINK_LIBRARIES(caf ${QT_LIBRARIES} ${_link_LIBRARIES})
INSTALL(TARGETS caf EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${caf_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
