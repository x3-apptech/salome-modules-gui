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
#  Module : ObjectBrowser
# $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libObjBrowser.la

salomeinclude_HEADERS=\
	OB.h \
	OB_Browser.h \
	OB_ListItem.h \
	OB_ListView.h \
	OB_Filter.h \
	OB_FindDlg.h \
	OB_ObjSearch.h

dist_libObjBrowser_la_SOURCES= \
	OB_Browser.cxx \
	OB_ListItem.cxx \
	OB_ListView.cxx \
	OB_Filter.cxx \
	OB_FindDlg.cxx \
	OB_ObjSearch.cxx

MOC_FILES= \
	OB_Browser_moc.cxx \
	OB_ListView_moc.cxx \
	OB_FindDlg_moc.cxx
nodist_libObjBrowser_la_SOURCES= $(MOC_FILES)

nodist_salomeres_DATA = OB_msg_en.qm

libObjBrowser_la_CPPFLAGS=$(QT_INCLUDES) -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx

libObjBrowser_la_LDFLAGS=$(QT_MT_LIBS)
libObjBrowser_la_LIBADD= ../SUIT/libsuit.la ../Qtx/libqtx.la
