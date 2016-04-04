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
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/Style
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS} ${PYTHON_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${PLATFORM_LIBS} ${QT_LIBRARIES} qtx suit SalomeStyle)
IF(SALOME_USE_PYCONSOLE)
  LIST(APPEND _link_LIBRARIES ${PYTHON_LIBRARIES})
ENDIF()

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  SUITApp_Application.h
)

# header files / internal
# no processing currently: TODO: check for MSVS project
SET(_internal_HEADERS
  SUITApp_init_python.hxx
)

# header files / no moc processing
IF(SALOME_USE_PYCONSOLE)
  SET(_other_HEADERS SUITApp_init_python.hxx)
ENDIF()

# header files / to install
SET(SUITApp_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/SUITApp_msg_en.ts
  resources/SUITApp_msg_fr.ts
  resources/SUITApp_msg_ja.ts
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  SUITApp_Application.cxx
)
IF(SALOME_USE_PYCONSOLE)
  LIST(APPEND _other_SOURCES SUITApp_init_python.cxx)
ENDIF()

# sources / to compile
SET(SUITApp_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(SUITApp ${SUITApp_SOURCES})
TARGET_LINK_LIBRARIES(SUITApp ${_link_LIBRARIES})
INSTALL(TARGETS SUITApp EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

ADD_EXECUTABLE(suitexe SUITApp.cxx)
TARGET_LINK_LIBRARIES(suitexe ${_link_LIBRARIES} SUITApp)
INSTALL(TARGETS suitexe EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

INSTALL(FILES ${SUITApp_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
