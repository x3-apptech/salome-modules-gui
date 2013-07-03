# - Find PyQt4 installation
# Sets the following variables:
#    PYQT_PYUIC_EXECUTABLE  - path to the pyuic4 executable
#    PYQT_PYTHONPATH        - path to the PyQt Python modules
#    PYQT_SIPS_DIR          - path to main include directory (which contains several sub folders)
#    PYQT_INCLUDE_DIRS      - list of paths to include when compiling (all rooted on PYQT_SIP_DIRS)
#    PYQT_SIPFLAGS          - compilation flags extracted from PyQt
#
#  The executables
#      pyuic4 (pyuic4.bat)
#  are searched.
#  The Python command 
#      pyqtconfig.Configuration().pyqt_sip_flags
#  is called to get the compilation flags.
#  Headers are located by looking for the header file
#      qobject.sip

#########################################################################
# Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

IF(NOT PyQt4_FIND_QUIETLY)
  MESSAGE(STATUS "Looking for PyQt4 ...")
ENDIF()

FIND_PROGRAM(PYQT_PYUIC_EXECUTABLE NAMES pyuic4 pyuic4.bat)

# Get root dir locally, going up two levels from the exec:
GET_FILENAME_COMPONENT(_tmp_ROOT_DIR "${PYQT_PYUIC_EXECUTABLE}" PATH)
GET_FILENAME_COMPONENT(_tmp_ROOT_DIR "${_tmp_ROOT_DIR}" PATH)

# Typical location of qobject.sip are: 
#   - /usr/share/sip/PyQt4/QtCore, for a system install
#   - or <xyz>/sip/QtCore, for a custom install
#   - or <xyz>/share/sip/QtCore, for a custom install
FIND_FILE(PYQT_SIP_MAIN_FILE qobject.sip PATH_SUFFIXES share/sip/QtCore sip/QtCore share/sip/PyQt4/QtCore)

IF(PYQT_SIP_MAIN_FILE)
  GET_FILENAME_COMPONENT(PYQT_SIPS_DIR "${PYQT_SIP_MAIN_FILE}" PATH)
  GET_FILENAME_COMPONENT(PYQT_SIPS_DIR "${PYQT_SIPS_DIR}" PATH)
ENDIF()
MARK_AS_ADVANCED(PYQT_SIP_MAIN_FILE)

# Get PyQt compilation flags:
SET(PYQT_PYTHONPATH "${PYQT_PYTHONPATH}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages")
SET(PYQT_SIPFLAGS)
EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; sys.path[:0] = ['${PYQT_PYTHONPATH}']; from PyQt4 import pyqtconfig; sys.stdout.write(pyqtconfig.Configuration().pyqt_sip_flags)"
  OUTPUT_VARIABLE PYQT_SIPFLAGS)
SEPARATE_ARGUMENTS(PYQT_SIPFLAGS)

SET(PYQT_INCLUDE_DIRS
  "${PYQT_SIPS_DIR}"
  "${PYQT_SIPS_DIR}/QtCore"
  "${PYQT_SIPS_DIR}/QtGui"
  "${PYQT_SIPS_DIR}/QtXml"
  "${PYQT_SIPS_DIR}/QtOpenGL"
  "${PYQT_SIPS_DIR}/QtAssistant"
  "${PYQT_SIPS_DIR}/QtDesigner"
  "${PYQT_SIPS_DIR}/QtNetwork"
  "${PYQT_SIPS_DIR}/QtSql"
  "${PYQT_SIPS_DIR}/QtSvg"
  "${PYQT_SIPS_DIR}/QtTest"
)

SET(PYQT_SIPFLAGS ${PYQT_SIPFLAGS} -s .cc -c .)
FOREACH(_dir ${PYQT_INCLUDE_DIRS})
  LIST(APPEND PYQT_SIPFLAGS -I ${_dir})
ENDFOREACH()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PyQt4 REQUIRED_VARS PYQT_PYUIC_EXECUTABLE PYQT_SIPS_DIR PYQT_SIPFLAGS)

