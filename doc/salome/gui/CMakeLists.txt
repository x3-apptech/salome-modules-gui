# Copyright (C) 2007-2019  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

INCLUDE(UseSphinx)

SALOME_CONFIGURE_FILE(conf.py.in conf.py)

IF(SALOME_GUI_BUILD_FRENCH_DOC)
  SET(BUILD_LANGUAGE_SWITCHER "true")
  SALOME_CONFIGURE_FILE(static/switchers.js.in static/switchers.js)
  ADD_MULTI_LANG_DOCUMENTATION(TARGET_NAME usr_docs MODULE GUI LANGUAGES fr)
ELSE()
  SET(BUILD_LANGUAGE_SWITCHER "false")
  SALOME_CONFIGURE_FILE(static/switchers.js.in static/switchers.js)
  ADD_MULTI_LANG_DOCUMENTATION(TARGET_NAME usr_docs MODULE GUI)
ENDIF()

INSTALL(CODE "EXECUTE_PROCESS(COMMAND \"${CMAKE_COMMAND}\" --build ${PROJECT_BINARY_DIR} --target usr_docs)")
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/GUI DESTINATION ${SALOME_INSTALL_DOC}/gui)
IF(SALOME_GUI_BUILD_FRENCH_DOC)
  INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/GUI_fr DESTINATION ${SALOME_INSTALL_DOC}/gui)
ENDIF()
