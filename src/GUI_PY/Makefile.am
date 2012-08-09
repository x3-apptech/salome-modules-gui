# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

mypkgpythondir = $(salomepythondir)/salome/gui

# Python modules to be installed
mypkgpython_PYTHON = \
	__init__.py            \
	selectvars.py          \
	genericdialog.py       \
	mytestdialog.py        \
	helper.py

PYUIC_FILES = \
	SelectVarsDialog_ui.py \
	genericdialog_ui.py    \
	mytestdialog_ui.py

nodist_mypkgpython_PYTHON = $(PYUIC_FILES)
CLEANFILES = $(PYUIC_FILES)

EXTRA_DIST += \
	SelectVarsDialog.ui \
	genericdialog.ui    \
	mytestdialog.ui

# This rule indicates that some python files are generated from ui
# files using the PYUIC utility. The convention is to create a file
# name <basename>_ui.py when the ui file name is <basename>.ui. When
# implementing the dialog, you generally create a third file named
# <basename>.py that uses the UI_<basename> classe defined in the
# generated file <basename>_ui.py.
%_ui.py:%.ui
	$(PYUIC) -x $< -o $@
