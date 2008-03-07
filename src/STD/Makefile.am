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
#  Module : STD
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libstd.la

salomeinclude_HEADERS= \
	STD_Application.h \
	STD.h \
	STD_MDIDesktop.h \
	STD_SDIDesktop.h \
	STD_TabDesktop.h \
	STD_LoadStudiesDlg.h

dist_libstd_la_SOURCES=\
	STD_Application.cxx \
	STD_MDIDesktop.cxx \
	STD_SDIDesktop.cxx \
	STD_TabDesktop.cxx \
	STD_LoadStudiesDlg.cxx

MOC_FILES= \
	STD_Application_moc.cxx \
	STD_MDIDesktop_moc.cxx \
	STD_SDIDesktop_moc.cxx \
	STD_TabDesktop_moc.cxx \
	STD_LoadStudiesDlg_moc.cxx

nodist_libstd_la_SOURCES= $(MOC_FILES)

dist_salomeres_DATA=\
	resources/config \
	resources/cut.png \
	resources/copy.png \
	resources/close.png \
	resources/cursor_rotate.png \
	resources/cursor_zoom.png \
	resources/help.png \
	resources/new.png \
	resources/open.png \
	resources/print.png \
	resources/paste.png \
	resources/redo.png \
	resources/reset.png \
	resources/save.png \
	resources/undo_arrow.png \
	resources/undo.png \
	resources/std.ini

nodist_salomeres_DATA=\
	STD_images.qm \
	STD_msg_en.qm

libstd_la_CPPFLAGS=$(QT_INCLUDES) -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx
libstd_la_LDFLAGS=$(QT_MT_LIBS)
libstd_la_LIBADD=../SUIT/libsuit.la
