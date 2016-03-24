# Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

# - Find PyQt4 installation
# Sets the following variables:
#    PYQT_PYUIC_EXECUTABLE  - path to the pyuic executable
#    PYQT_PYRCC_EXECUTABLE  - path to the pyrcc executable
#    PYQT_PYUIC_PATH        - command to launch pyuic with the correct PYTHONPATH
#    PYQT_PYRCC_PATH        - command to launch pyrcc with the correct PYTHONPATH
#    PYQT_PYTHONPATH        - path to the PyQt Python modules
#    PYQT_SIPS_DIR          - path to main include directory (which contains several sub folders)
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

IF(NOT PyQt4_FIND_QUIETLY)
  MESSAGE(STATUS "Looking for PyQt4 ...")
ENDIF()

IF(NOT SIP_FOUND AND NOT PyQt4_FIND_QUIETLY)
   MESSAGE(WARNING "PyQt4 needs SIP to be detected correctly!")
ENDIF()

FIND_PROGRAM(PYQT_PYUIC_EXECUTABLE NAMES pyuic4 pyuic4.bat)
FIND_PROGRAM(PYQT_PYRCC_EXECUTABLE NAMES pyrcc4 pyrcc4.bat)

# Get root dir locally, going up two levels from the exec:
GET_FILENAME_COMPONENT(_tmp_ROOT_DIR "${PYQT_PYUIC_EXECUTABLE}" PATH)
GET_FILENAME_COMPONENT(_tmp_ROOT_DIR "${_tmp_ROOT_DIR}" PATH)


# Typical locations of qobject.sip are: 
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
SET(PYQT_PYTHONPATH ${_tmp_ROOT_DIR}/PyQt4)
SET(PYQT_SIPFLAGS)

EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; 
sys.path[:0] = '${PYQT_PYTHONPATH}'.split(';');
sys.path[:0] = '${SIP_PYTHONPATH}'.split(';');
from PyQt4 import pyqtconfig; 
sys.stdout.write(pyqtconfig.Configuration().pyqt_sip_flags)"
  OUTPUT_VARIABLE PYQT_SIPFLAGS)
SEPARATE_ARGUMENTS(PYQT_SIPFLAGS)

SET(PYQT_SIPFLAGS ${PYQT_SIPFLAGS} -I "${PYQT_SIPS_DIR}")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PyQt4 REQUIRED_VARS PYQT_PYUIC_EXECUTABLE PYQT_PYRCC_EXECUTABLE PYQT_SIPS_DIR PYQT_SIPFLAGS )

# Wrap the final executables so that they always use the proper environment (PYTHONPATH):
# The results are put to variables:
#   PYQT_PYUIC_PATH - command to launch pyuic with the correct PYTHONPATH
#   PYQT_PYRCC_PATH - command to launch pyrcc with the correct PYTHONPATH
# TODO: should be done like Sphinx in KERNEL (i.e. generating a shell script)?

IF(WIN32 AND NOT CYGWIN)
  SET(PYQT_PYUIC_PATH set PYTHONPATH=${PYQT_PYTHONPATH};${SIP_PYTHONPATH};%PYTHONPATH% && ${PYQT_PYUIC_EXECUTABLE})
  SET(PYQT_PYRCC_PATH set PYTHONPATH=${PYQT_PYTHONPATH};${SIP_PYTHONPATH};%PYTHONPATH% && ${PYQT_PYRCC_EXECUTABLE})
ELSE()
  STRING(REPLACE ";" ":" PYQT_PYTHONPATH "${PYQT_PYTHONPATH}")
  SET(PYQT_PYUIC_PATH /usr/bin/env PYTHONPATH="${PYQT_PYTHONPATH}:${SIP_PYTHONPATH}:$$PYTHONPATH" ${PYQT_PYUIC_EXECUTABLE})
  SET(PYQT_PYRCC_PATH /usr/bin/env PYTHONPATH="${PYQT_PYTHONPATH}:${SIP_PYTHONPATH}:$$PYTHONPATH" ${PYQT_PYRCC_EXECUTABLE})
ENDIF()
