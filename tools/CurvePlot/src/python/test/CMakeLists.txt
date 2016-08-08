# Copyright (C) 2012-2014  CEA/DEN, EDF R&D
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

SET(SGPYQT_RES_DIR "${SALOME_INSTALL_RES}")
SALOME_CONFIGURE_FILE(SalomePyQt_MockUp.py.in mockup/SalomePyQt_MockUp.py)

SET(_all_SCRIPTS
    PlotCurve_Standalone.py
    plot_test.py
    TestDesktop.py
)

SALOME_INSTALL_SCRIPTS("${_all_SCRIPTS}" ${SALOME_INSTALL_SCRIPT_PYTHON}/tests)
SALOME_INSTALL_SCRIPTS(${CMAKE_CURRENT_BINARY_DIR}/mockup/SalomePyQt_MockUp.py ${SALOME_INSTALL_SCRIPT_PYTHON}/tests)

INSTALL(DIRECTORY baselines DESTINATION ${SALOME_INSTALL_SCRIPT_PYTHON}/tests)

SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH ${CMAKE_CURRENT_SOURCE_DIR}/../model)
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH ${CMAKE_CURRENT_SOURCE_DIR}/../ui)
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH ${CMAKE_CURRENT_SOURCE_DIR}/../views)
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH ${CMAKE_CURRENT_SOURCE_DIR}/../controller)
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH ${PROJECT_BINARY_DIR}/src/python/ui)  # for the generated PY files (from UI files)
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH ${PROJECT_BINARY_DIR}/src/python/controller)  # for utils.py
SALOME_GENERATE_TESTS_ENVIRONMENT(tests_env)

ADD_TEST(CurvePlotUnitTests ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/plot_test.py)
SET_TESTS_PROPERTIES(CurvePlotUnitTests PROPERTIES ENVIRONMENT "${tests_env}")

