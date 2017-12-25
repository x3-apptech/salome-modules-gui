# Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

INCLUDE(UsePyQt)

# --- resources ---

# uic files / to be processed by pyuic
SET(_pyuic_FILES
  SelectVarsDialog.ui
  genericdialog.ui
  mytestdialog.ui
)

# --- scripts ---

# scripts / pyuic wrappings
PYQT_WRAP_UIC(_pyuic_SCRIPTS ${_pyuic_FILES} TARGET_NAME _target_name_pyuic)

# scripts / static
SET(_other_SCRIPTS 
  __init__.py
  genericdialog.py
  helper.py
  mytestdialog.py
  selectvars.py
  dockwidgets.py
)

SET(_bin_SCRIPTS
  test_dockwidgets.py
)

SALOME_CONFIGURE_FILE(qtsalome.py.in qtsalome.py)

SET(_gen_SCRIPTS
    ${CMAKE_CURRENT_BINARY_DIR}/qtsalome.py
)

# --- rules ---
SALOME_INSTALL_SCRIPTS("${_gen_SCRIPTS}" ${SALOME_INSTALL_PYTHON})
SALOME_INSTALL_SCRIPTS("${_other_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/gui)
SALOME_INSTALL_SCRIPTS("${_bin_SCRIPTS}" ${SALOME_INSTALL_SCRIPT_DATA})
SALOME_INSTALL_SCRIPTS("${_pyuic_SCRIPTS}" ${SALOME_INSTALL_PYTHON}/salome/gui TARGET_NAME _target_name_pyuic_py)
# add dependency of compiled py files on uic files in order
# to avoid races problems when compiling in parallel
ADD_DEPENDENCIES(${_target_name_pyuic_py} ${_target_name_pyuic})
