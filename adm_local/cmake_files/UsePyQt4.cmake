# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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
# Author: Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

####################################################################
#
# PYQT4_WRAP_UIC macro
#
# Create Python modules by processing input *.ui (Qt designer) files with
# PyQt4 pyuic4 tool.
#
# USAGE: PYQT4_WRAP_UIC(output_files pyuic_files)
#
# ARGUMENTS:
#   output_files [out] variable where output file names are listed to
#   pyuic_files  [in]  list of *.ui files
# 
# NOTES:
#   - Input files are considered relative to the current source directory.
#   - Output files are generated in the current build directory.
#   - Macro automatically adds custom build target to generate output files
# 
####################################################################
MACRO(PYQT4_WRAP_UIC outfiles)
  FOREACH(_input ${ARGN})
    GET_FILENAME_COMPONENT(_input_name ${_input} NAME)
    STRING(REPLACE ".ui" "_ui.py" _input_name ${_input_name})
    SET(_output ${CMAKE_CURRENT_BINARY_DIR}/${_input_name})
    ADD_CUSTOM_COMMAND(
      OUTPUT ${_output}
      COMMAND ${PYQT_PYUIC_EXECUTABLE} -o ${_output} ${CMAKE_CURRENT_SOURCE_DIR}/${_input}
      MAIN_DEPENDENCY ${_input}
      )
    SET(${outfiles} ${${outfiles}} ${_output})
  ENDFOREACH()
  ADD_CUSTOM_TARGET(BUILD_UI_PY_FILES ALL DEPENDS ${${outfiles}})
ENDMACRO(PYQT4_WRAP_UIC)

####################################################################
#
# PYQT4_WRAP_SIP macro
#
# Generate C++ wrappings for *.sip files by processing them with sip.
#
# USAGE: PYQT4_WRAP_SIP(output_files sip_files)
#
# ARGUMENTS:
#   output_files [out] variable where output file names are listed to
#   sip_files    [in]  list of *.sip files
# 
# NOTES:
#   - Input files are considered relative to the current source directory.
#   - Output files are generated in the current build directory.
#   - This version of macro requires class(es) definition in the 
#     *.sip file to be started on a new line without any preceeding characters.
#
# TODO:
#   - Check if dependency of static sources on generated headers works properly:
#     if header is changed, dependant sources should be recompiled.
# 
####################################################################
MACRO(PYQT4_WRAP_SIP outfiles)
  FOREACH(_input ${ARGN})
    FILE(STRINGS ${_input} _sip_modules REGEX "%Module")
    FILE(STRINGS ${_input} _sip_classes REGEX "^class ")
    SET(_output)
    FOREACH(_sip_module ${_sip_modules})
      STRING(REGEX MATCH ".*%Module *\\( *name=.*\\).*" _mod_name "${_sip_module}")
      IF (_mod_name)
	STRING(REGEX REPLACE ".*%Module *\\( *name=(.*).*\\).*" "\\1" _mod_name ${_sip_module})
      ELSE()
	STRING(REGEX REPLACE ".*%Module *(.*)" "\\1" _mod_name ${_sip_module})
      ENDIF()
      SET(_mod_header "sipAPI${_mod_name}.h")
      SET(_mod_source "sip${_mod_name}cmodule${PYQT_CXX_EXT}")
      LIST(APPEND _output ${CMAKE_CURRENT_BINARY_DIR}/${_mod_source})
      SET(${outfiles} ${${outfiles}} ${CMAKE_CURRENT_BINARY_DIR}/${_mod_source})
    ENDFOREACH()
    FOREACH(_sip_class ${_sip_classes})
      STRING(REGEX MATCH ".*class +.* *:" _class_name "${_sip_class}")
      IF (_class_name)
	STRING(REGEX REPLACE ".*class +(.*) *:.*" "\\1" _class_name ${_sip_class})
      ELSE()
	STRING(REGEX REPLACE ".*class *(.*)" "\\1" _class_name ${_sip_class})
      ENDIF()
      STRING(STRIP ${_class_name} _class_name)
      SET(_class_source "sip${_mod_name}${_class_name}${PYQT_CXX_EXT}")
      LIST(APPEND _output ${CMAKE_CURRENT_BINARY_DIR}/${_class_source})
      SET(${outfiles} ${${outfiles}} ${CMAKE_CURRENT_BINARY_DIR}/${_class_source})
    ENDFOREACH()
    ADD_CUSTOM_COMMAND(
      OUTPUT ${_output}
      COMMAND ${SIP_EXECUTABLE} ${PYQT_SIPFLAGS} ${CMAKE_CURRENT_SOURCE_DIR}/${_input}
      MAIN_DEPENDENCY ${_input}
      )
  ENDFOREACH()
ENDMACRO(PYQT4_WRAP_SIP)
