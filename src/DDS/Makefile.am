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
#  Author : Alexander SOLOVYOV (OCN)
#  Module : DDS
#  $Header: /home/server/cvs/GUI/GUI_SRC/src/DDS/Makefile.in

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libDDS.la

salomeinclude_HEADERS = DDS.h \
	DDS_DicGroup.h \
	DDS_DicItem.h \
	DDS_Dictionary.h \
	DDS_KeyWords.h

dist_libDDS_la_SOURCES = DDS_DicGroup.cxx \
	DDS_DicItem.cxx \
	DDS_Dictionary.cxx \
	DDS_KeyWords.cxx

libDDS_la_CPPFLAGS=$(CAS_CPPFLAGS)

libDDS_la_LDFLAGS= $(CAS_KERNEL) $(CAS_OCAF)



