# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

IF (QT_VERSION VERSION_LESS "5.0")
  INCLUDE(${QT_USE_FILE})
ENDIF()

INSTALL(CODE "MACRO(QT_COMPILE_TS_ON_INSTALL MYLRELEASE MYTSFILE MYFULLDIR MYOUTQMFILE)
  FILE(MAKE_DIRECTORY \${MYFULLDIR})
  MESSAGE(STATUS \"Compiling \${MYTSFILE}\")
  EXECUTE_PROCESS(COMMAND \${MYLRELEASE} \${MYTSFILE} -qm \${MYFULLDIR}/\${MYOUTQMFILE})
ENDMACRO(QT_COMPILE_TS_ON_INSTALL)")

# This MACRO uses the following vars
# - QT_LRELEASE_EXECUTABLE : (given by default by FindQT.cmake)
#
# MYTSFILES containing all ts files to be compiled.
# WHERETOINSTALL contains directory (relative to install_prefix) where to install files after compilation of ts files too qm.
MACRO(QT_INSTALL_TS_RESOURCES MYTSFILES WHERETOINSTALL)
  INSTALL(CODE "SET(INSTALL_TS_DIR ${WHERETOINSTALL})")
  SET(MYSOURCES)
  FOREACH(input ${MYTSFILES})
    GET_FILENAME_COMPONENT(input2 ${input} NAME)
    STRING(REGEX REPLACE ".ts" "" base ${input2})
    SET(output "${base}.qm")
    INSTALL(CODE "QT_COMPILE_TS_ON_INSTALL( \"${QT_LRELEASE_EXECUTABLE}\" \"${CMAKE_CURRENT_SOURCE_DIR}/${input}\" \"${CMAKE_INSTALL_PREFIX}/\${INSTALL_TS_DIR}\" ${output})")
  ENDFOREACH(input ${MYIDLFILES})
ENDMACRO(QT_INSTALL_TS_RESOURCES)

MACRO(QT_WRAP_MOC)
IF (QT_VERSION VERSION_LESS "5.0")
  QT4_WRAP_CPP(${ARGN})
ELSE()
  QT5_WRAP_CPP(${ARGN})
ENDIF()
ENDMACRO(QT_WRAP_MOC)

MACRO(QT_WRAP_UIC)
IF (QT_VERSION VERSION_LESS "5.0")
  QT4_WRAP_UI(${ARGN})
ELSE()
  QT5_WRAP_UI(${ARGN})
ENDIF()
ENDMACRO(QT_WRAP_UIC)

MACRO(QT_ADD_RESOURCES)
IF (QT_VERSION VERSION_LESS "5.0")
  QT4_ADD_RESOURCES(${ARGN})
ELSE()
  QT5_ADD_RESOURCES(${ARGN})
ENDIF()
ENDMACRO(QT_ADD_RESOURCES)
