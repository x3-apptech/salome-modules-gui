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

# -* Makefile *-
# Author : Patrick GOLDBRONN (CEA)
# Date : 28/06/2001
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

ACLOCAL_AMFLAGS = -I adm_local/unix/config_files \
                  -I ${KERNEL_ROOT_DIR}/salome_adm/unix/config_files

if GUI_ENABLE_CORBA
    IDLDIR = idl
else !GUI_ENABLE_CORBA
    IDLDIR =
endif

SUBDIRS = $(IDLDIR) adm_local resources src doc bin tools

DIST_SUBDIRS = idl adm_local resources src doc bin tools

DISTCLEANFILES = a.out aclocal.m4 configure local-install.sh hack_libtool adm_local/unix/config_files/config.guess adm_local/unix/config_files/config.sub adm_local/unix/config_files/depcomp adm_local/unix/config_files/install-sh adm_local/unix/config_files/libtool.m4 adm_local/unix/config_files/ltmain.sh adm_local/unix/config_files/ltoptions.m4 adm_local/unix/config_files/ltsugar.m4 adm_local/unix/config_files/ltversion.m4 adm_local/unix/config_files/lt~obsolete.m4 adm_local/unix/config_files/missing adm_local/unix/config_files/py-compile

salomeinclude_DATA = GUI_version.h

EXTRA_DIST +=		\
	build_cmake	\
	build_cmake.bat	\
	build_configure	\
	clean_configure	\
	LICENCE

dist-hook:
	rm -rf `find $(distdir) -name CVS`

usr_docs:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) usr_docs)

docs: usr_docs

dev_docs:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) dev_docs)
