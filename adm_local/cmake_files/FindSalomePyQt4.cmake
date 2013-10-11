# Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
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
# Author: Adrien Bruneton
#

# PyQt4 detection dor Salome
#
#  !! Please read the generic detection procedure in SalomeMacros.cmake !!
#

IF(NOT SalomeSIP_FOUND AND NOT SalomePyQt4_FIND_QUIETLY)
   MESSAGE(WARNING "PyQt4 needs SIP to be detected correctly!")
ENDIF()   

SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS(PyQt4 PYQT_PYUIC_EXECUTABLE 2)
MARK_AS_ADVANCED(PYQT_PYUIC_EXECUTABLE PYQT_PYRCC_EXECUTABLE PYQT_SIPS_DIR PYQT_PYUIC_PATH PYQT_PYRCC_PATH)

# Wrap the final executables so that they always use the proper environment (PYTHONPATH):
# The results are put to variables:
#   PYQT_PYUIC_PATH - command to launch pyuic with the correct PYTHONPATH
#   PYQT_PYRCC_PATH - command to launch pyrcc with the correct PYTHONPATH
# TODO: should be done like Sphinx in KERNEL (i.e. generating a shell script)?
IF(WIN32 AND NOT CYGWIN)
  MESSAGE(WARNING "PyQt4 command was not tested under Win32")
  SET(PYQT_PYUIC_PATH set PYTHONPATH=${PYQT_PYTHONPATH};${SIP_PYTHONPATH};%PYTHONPATH% && ${PYQT_PYUIC_EXECUTABLE})
  SET(PYQT_PYRCC_PATH set PYTHONPATH=${PYQT_PYTHONPATH};${SIP_PYTHONPATH};%PYTHONPATH% && ${PYQT_PYRCC_EXECUTABLE})
ELSE()
  SET(PYQT_PYUIC_PATH /usr/bin/env PYTHONPATH="${PYQT_PYTHONPATH}:${SIP_PYTHONPATH}:$$PYTHONPATH" ${PYQT_PYUIC_EXECUTABLE})
  SET(PYQT_PYRCC_PATH /usr/bin/env PYTHONPATH="${PYQT_PYTHONPATH}:${SIP_PYTHONPATH}:$$PYTHONPATH" ${PYQT_PYRCC_EXECUTABLE})
ENDIF()
