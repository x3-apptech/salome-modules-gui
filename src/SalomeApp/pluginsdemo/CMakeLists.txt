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

#
# Note that the plugins files should be installed in the directory
# <salomepluginsdir> (ROOT_DIR/share/salome/plugins) or one of this
# sub-directories (the search of plugins by the plugin manager is
# recurcive from this folder, in each SALOME module, i.e. each
# variable *_ROOT_DIR).  
#

INCLUDE(UseQtExt)
INCLUDE(UsePyQt)

# --- resources ---

# uic files / to be processed by pyuic 
SET(_pyuic_files
  minmax.ui
)

# scripts / pyuic wrappings
PYQT_WRAP_UIC(_pyuic_SCRIPTS ${_pyuic_files})

# --- scripts ---

# scripts / static
# TODO: process UIC files via PyQt pyuic tool, see UsePyQt.cmake
SET(_plugins_SCRIPTS
  ${_pyuic_SCRIPTS}
  minmax_plugin.py
  trihedron.py
  tubedialog_ui.py
  tubedialog.py
  tubebuilder.py
  salome_plugins.py
  smesh_plugins.py
)
SET(_helper_SCRIPTS
  xalome.py
)

# --- rules ---

SALOME_INSTALL_SCRIPTS("${_plugins_SCRIPTS}" ${SALOME_GUI_INSTALL_PLUGINS}/demo DEF_PERMS)
SALOME_INSTALL_SCRIPTS("${_helper_SCRIPTS}" ${SALOME_INSTALL_PYTHON})
