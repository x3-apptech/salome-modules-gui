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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  File   : Makefile.in
#  Author : Vladimir Klyachin (OCN)
#  Module : SalomeApp

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES = libSalomeStyle.la

libSalomeStyle_la_SOURCES = SalomeStyle.cxx

libSalomeStyle_la_CPPFLAGS=$(QT_INCLUDES)

libSalomeStyle_la_LDFLAGS=$(QT_MT_LIBS)

EXTRA_DIST+=SalomeStyle.h

install-exec-hook:
	mkdir -p $(bindir)/styles
	(cd $(bindir)/styles; ln -sf ../../../lib@LIB_LOCATION_SUFFIX@/salome/libSalomeStyle.so libSalomeStyle.so;)

uninstall-hook:
	rm -f $(bindir)/styles/libSalomeStyle.so
