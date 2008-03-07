#  Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  File   : Makefile.in
#  Author : Vasily Rusyaev (Open Cascade NN)
#  Module : doc

include $(top_srcdir)/adm_local/unix/make_common_starter.am

EXTRA_DIST+=GUI

doxygen=@DOXYGEN@

usr_docs:
	cd ./GUI; \
	echo "Running doxygen in directory: "`pwd`; \
	$(doxygen) ./doxyfile;

docs: usr_docs

clean-local:
	cd ./GUI; \
	rm -fr `ls | grep -v "doxyfile"`
	rm -fr `ls | grep -vE "Makefile|GUI"`

install-data-local: usr_docs
	mkdir -p $(docdir)/gui
	cp -rf GUI $(docdir)/gui
	-find $(PWD) -name CVS -prune -exec rm -rf {} \;

uninstall-local:
	rm -rf $(docdir)/gui/GUI

docguidir=$(docdir)/gui/GUI

nodist_docgui_DATA= GUI/doxyfile
GUI/doxyfile: GUI/doxyfile.in
EXTRA_DIST+= GUI/doxyfile.in