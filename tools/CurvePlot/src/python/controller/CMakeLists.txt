# Copyright (C) 2012-2019  CEA/DEN, EDF R&D
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

SET(SALOME_CURVEPLOT_TEST_MODE "0")
SALOME_CONFIGURE_FILE(utils.py.in ${CMAKE_CURRENT_BINARY_DIR}/utils.py)

# For test purposes:
SET(SALOME_CURVEPLOT_TEST_MODE "1")
SALOME_CONFIGURE_FILE(utils.py.in ${CMAKE_CURRENT_BINARY_DIR}/utils_test.py)
SET(SALOME_CURVEPLOT_TEST_MODE "0")

SET(_all_lib_SCRIPTS
    PlotController.py
    __init__.py
)

SET(_util_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/utils.py)

SALOME_INSTALL_SCRIPTS("${_all_lib_SCRIPTS}" ${SALOME_CURVEPLOT_INSTALL_PYTHON})
SALOME_INSTALL_SCRIPTS("${_util_SCRIPT}" ${SALOME_CURVEPLOT_INSTALL_PYTHON})

# For test purposes
FILE(COPY ${_all_lib_SCRIPTS} DESTINATION ${CRVPLOT_TEST_INSTALL})
FILE(COPY ${CMAKE_CURRENT_BINARY_DIR}/utils_test.py DESTINATION ${CRVPLOT_TEST_INSTALL})
FILE(RENAME ${CRVPLOT_TEST_INSTALL}/utils_test.py ${CRVPLOT_TEST_INSTALL}/utils.py)
