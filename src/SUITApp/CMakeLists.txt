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

INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/UseQT4EXT.cmake)

INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../Style
)

SET(COMMON_FLAGS "${QT_DEFINITIONS} ${PYTHON_DEFINITIONS} ${PLATFORM_DEFINITIONS}")

SET(COMMON_LIBS
  qtx 
  suit 
  SalomeStyle
  ${QT_MT_LIBS}
)

SET(GUI_HEADERS SUITApp_Application.h)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(SUITApp_SOURCES
  SUITApp.cxx
  SUITApp_Application.cxx
)
SET(COMMON_HEADERS_HXX)
IF(ENABLE_PYCONSOLE)
  SET(COMMON_HEADERS_HXX
    ${COMMON_HEADERS_HXX}
    SUITApp_init_python.hxx
  )
  SET(SUITApp_SOURCES
    ${SUITApp_SOURCES}
    SUITApp_init_python.cxx
  )
  SET(COMMON_LIBS
    ${COMMON_LIBS}
    ${PYTHON_LIBS}
   )
ENDIF(ENABLE_PYCONSOLE)

IF(ENABLE_TESTRECORDER)
  SET(COMMON_FLAGS
    ${COMMON_FLAGS}
    -DENABLE_TESTRECORDER
    ${TESTRECORDER_DEFINITIONS}
  )
    SET(COMMON_LIBS
    ${COMMON_LIBS}
    ${TESTRECORDER_LIBS}
  )
ENDIF(ENABLE_TESTRECORDER)

SET(GUITS_SOURCES
  resources/SUITApp_msg_en.ts
  resources/SUITApp_msg_fr.ts
)

ADD_LIBRARY(SUITApp SHARED ${SUITApp_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(SUITApp PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(SUITApp ${COMMON_LIBS})
INSTALL(TARGETS SUITApp DESTINATION ${GUI_salomelib_LIBS})

ADD_EXECUTABLE(TestSUITApp SUITApp.cxx)
SET_TARGET_PROPERTIES(TestSUITApp PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}" OUTPUT_NAME "SUITApp")
TARGET_LINK_LIBRARIES(TestSUITApp SUITApp ${COMMON_LIBS})
INSTALL(TARGETS TestSUITApp DESTINATION ${GUI_salomebin_BINS})

FILE(GLOB COMMON_HEADERS_H "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
SET(COMMON_HEADERS ${COMMON_HEADERS_H} ${COMMON_HEADERS_HXX})
INSTALL(FILES ${COMMON_HEADERS} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")
