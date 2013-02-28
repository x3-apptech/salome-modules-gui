# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

INSTALL(CODE "MACRO(QT4_COMPILE_TS_ON_INSTALL MYLRELEASE MYTSFILE MYFULLDIR MYOUTQMFILE)
  FILE(MAKE_DIRECTORY \${MYFULLDIR})
  MESSAGE(STATUS \"Compiling \${MYTSFILE}\")
  EXECUTE_PROCESS(COMMAND \${MYLRELEASE} \${MYTSFILE} -qm \${MYFULLDIR}/\${MYOUTQMFILE})
ENDMACRO(QT4_COMPILE_TS_ON_INSTALL)")

# This MACRO uses the following vars
# - QT_LRELEASE_EXECUTABLE : (given by default by FindQT.cmake)
#
# MYTSFILES containing all ts files to be compiled.
# WHERETOINSTALL contains directory (relative to install_prefix) where to install files after compilation of ts files too qm.
MACRO(QT4_INSTALL_TS_RESOURCES MYTSFILES WHERETOINSTALL)
  INSTALL(CODE "SET(INSTALL_TS_DIR ${WHERETOINSTALL})")
  SET(MYSOURCES)
  FOREACH(input ${MYTSFILES})
    GET_FILENAME_COMPONENT(input2 ${input} NAME)
    STRING(REGEX REPLACE ".ts" "" base ${input2})
    SET(output "${base}.qm")
    INSTALL(CODE "QT4_COMPILE_TS_ON_INSTALL( \"${QT_LRELEASE_EXECUTABLE}\" \"${CMAKE_CURRENT_SOURCE_DIR}/${input}\" \"${CMAKE_INSTALL_PREFIX}/\${INSTALL_TS_DIR}\" ${output})")
  ENDFOREACH(input ${MYIDLFILES})
ENDMACRO(QT4_INSTALL_TS_RESOURCES)
