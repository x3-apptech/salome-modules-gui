# Copyright (C) 2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#
# Note that the plugins files should be installed in the directory
# <salomepluginsdir> (ROOT_DIR/share/salome/plugins) or one of this
# sub-directories (the search of plugins by the plugin manager is
# recurcive from this folder, in each SALOME module, i.e. each
# variable *_ROOT_DIR).  
#

INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/SalomeMacros.cmake)

SET(plugins_PYTHON
  salome_plugins.py
  smesh_plugins.py
  trihedron.py
  tubedialog_ui.py
  tubedialog.py
  tubebuilder.py
  xalome.py
  minmax_dialog.py
  minmax_plugin.py
)

SALOME_INSTALL_SCRIPTS("${plugins_PYTHON}" ${GUI_salomepluginsdir}/demo DEF_PERMS)
