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

# PyQt4 detection for Salome

set(PYQT_ROOT_DIR $ENV{PYQT_ROOT_DIR} CACHE PATH "Path to PyQt4 directory")
if(EXISTS ${PYQT_ROOT_DIR})
  set(CMAKE_INCLUDE_PATH ${PYQT_ROOT_DIR}/share/sip ${PYQT_ROOT_DIR}/sip)
  set(CMAKE_PROGRAM_PATH ${PYQT_ROOT_DIR}/bin)
endif(EXISTS ${PYQT_ROOT_DIR})
FIND_PROGRAM(PYQT_PYUIC_EXECUTABLE NAMES pyuic4 pyuic4.bat)
FIND_PATH(PYQT_SIPS_DIR NAMES QtCore QtGui QtXml QtOpenGL QtAssistant QtDesigner QtNetwork QtSql QtSvg QtTest HINTS /usr/share/sip/PyQt4)
SET(PYQT_PYTHONPATH "")
if(EXISTS ${PYQT_ROOT_DIR})
  set(PYQT_PYTHONPATH ${PYQT_ROOT_DIR}/lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)
endif(EXISTS ${PYQT_ROOT_DIR})
EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; sys.path[:0] = ['${PYQT_PYTHONPATH}']; from PyQt4 import pyqtconfig; sys.stdout.write(pyqtconfig.Configuration().pyqt_sip_flags)"
  OUTPUT_VARIABLE PYQT_SIPFLAGS)
SEPARATE_ARGUMENTS(PYQT_SIPFLAGS)
SET(PYQT_SIPFLAGS ${PYQT_SIPFLAGS} -s .cc -c .
  -I ${PYQT_SIPS_DIR}
  -I ${PYQT_SIPS_DIR}/QtCore -I ${PYQT_SIPS_DIR}/QtGui  -I ${PYQT_SIPS_DIR}/QtXml -I ${PYQT_SIPS_DIR}/QtOpenGL
  -I ${PYQT_SIPS_DIR}/QtAssistant  -I ${PYQT_SIPS_DIR}/QtDesigner -I ${PYQT_SIPS_DIR}/QtNetwork -I ${PYQT_SIPS_DIR}/QtSql
  -I ${PYQT_SIPS_DIR}/QtSvg -I ${PYQT_SIPS_DIR}/QtTest )
