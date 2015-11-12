# Copyright (C) 2013-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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

# PyQt5 detection for Salome
#
#  !! Please read the generic detection procedure in SalomeMacros.cmake !!
#

# PyQt needs SIP, call it automatically
FIND_PACKAGE(SalomeSIP REQUIRED)

SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS(PyQt5 PYQT_PYUIC_EXECUTABLE 2)
MARK_AS_ADVANCED(PYQT_PYUIC_EXECUTABLE PYQT_PYRCC_EXECUTABLE PYQT_SIPS_DIR PYQT_PYUIC_PATH PYQT_PYRCC_PATH)

IF(PYQT5_FOUND) 
  SALOME_ACCUMULATE_ENVIRONMENT(PATH ${PYQT_PYUIC_EXECUTABLE})
  SALOME_ACCUMULATE_ENVIRONMENT(LD_LIBRARY_PATH ${PYQT_PYTHONPATH})
  SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH ${PYQT_PYTHONPATH})
ENDIF()
