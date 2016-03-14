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
# Author: Adrien Bruneton
#

# GUI detection for Salome - this is typically called by dependent modules
# (PARAVIS, etc ...)
#
# The detection is simpler than for other prerequisites.
# See explanation in FindSalomeKERNEL.cmake.
#

IF(NOT SalomeGUI_FIND_QUIETLY)
  MESSAGE(STATUS "Looking for Salome GUI ...")
ENDIF()

SET(CMAKE_PREFIX_PATH "${GUI_ROOT_DIR}")
SALOME_FIND_PACKAGE(SalomeGUI SalomeGUI CONFIG)

IF(NOT SalomeGUI_FIND_QUIETLY)
  MESSAGE(STATUS "Found Salome GUI: ${GUI_ROOT_DIR}")
ENDIF()

FOREACH(_res ${SalomeGUI_EXTRA_ENV})
  SALOME_ACCUMULATE_ENVIRONMENT(${_res} "${SalomeGUI_EXTRA_ENV_${_res}}")
ENDFOREACH()

#----------------------------------------------------------------------------
# SALOME_GUI_WITH_CORBA is a macro useful for determining whether a SALOME GUI module 
# is built in not light mode (with CORBA)
#----------------------------------------------------------------------------
MACRO(SALOME_GUI_WITH_CORBA)
  IF(SALOME_GUI_LIGHT_ONLY)
    MESSAGE(FATAL_ERROR "\nWe absolutely need a Salome GUI module with CORBA.\nPlease set option SALOME_LIGHT_ONLY to OFF when building GUI module.")
  ENDIF()
ENDMACRO(SALOME_GUI_WITH_CORBA)


#----------------------------------------------------------------------------
# SALOME_GUI_MODE is a macro useful for determining whether a GUI module
# builded in particular mode 
#----------------------------------------------------------------------------
#########################################################################
# SALOME_GUI_MODE()
# 
# USAGE: SALOME_GUI_MODE(_options)
#
# ARGUMENTS:
#   _options [input] List - The list of CMake options given to SALOME GUI
#
# Notes:
# - The arguments list can include "OPTIONAL" keywords:
#   * All arguments preceded by "OPTIONAL" keyword are mandatory for current module.
#     If GUI module was compiled without this functionality, further installation 
#     of the current module becomes impossible.
#   * All arguments following "OPTIONAL" keyword are optional for current module, which
#     uses this functionality, but can be installed without it.
#
# - At present there are next options in the GUI module:
#   SALOME_USE_OCCVIEWER, SALOME_USE_VTKVIEWER, SALOME_USE_GLVIEWER, SALOME_USE_PLOT2DVIEWER,
#   SALOME_USE_GRAPHICSVIEW, SALOME_USE_QXGRAPHVIEWER, SALOME_USE_PVVIEWER, SALOME_USE_PYVIEWER,
#   SALOME_USE_PYCONSOLE, SALOME_USE_SALOMEOBJECT
#
# Example of usage:
#   SALOME_GUI_MODE(SALOME_USE_OCCVIEWER SALOME_USE_SALOMEOBJECT OPTIONAL SALOME_USE_PYCONSOLE)
#
MACRO(SALOME_GUI_MODE)
  MESSAGE(STATUS "")
  MESSAGE(STATUS "  Checking status of GUI options")
  MESSAGE(STATUS "  ==================================== ")
  MESSAGE(STATUS "")
  SET(_message)
  SET(_length 25)   
  SET(_is_optional OFF)
  FOREACH(_option ${ARGN})
    IF(${_option} STREQUAL "OPTIONAL")
      SET(_is_optional ON)
      MESSAGE(STATUS "  Optional:")
    ELSE()
      SALOME_JUSTIFY_STRING(${_option} ${_length} _option_name)
      MESSAGE(STATUS "  * ${_option_name}  ->  ${${_option}}")
      IF(NOT ${_option} AND NOT ${_is_optional})
        LIST(APPEND _message ${_option})
      ENDIF()
    ENDIF()
  ENDFOREACH()
  MESSAGE(STATUS "")
  
  IF(_message)
    SET(_message "We absolutely need a Salome GUI module in special mode. The following options should be set to ON when building GUI module: \n${_message}\n")
    MESSAGE(FATAL_ERROR "${_message}")
  ENDIF()
ENDMACRO(SALOME_GUI_MODE)
