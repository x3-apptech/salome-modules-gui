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

#  File   : Makefile.in
#  Author : Vladimir Klyachin (OCN)
#  Module : STD
#  $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libstd.la

salomeinclude_HEADERS =		\
	STD.h			\
	STD_Application.h	\
	STD_MDIDesktop.h	\
	STD_SDIDesktop.h	\
	STD_TabDesktop.h

dist_libstd_la_SOURCES =	\
	STD_Application.cxx	\
	STD_MDIDesktop.cxx	\
	STD_SDIDesktop.cxx	\
	STD_TabDesktop.cxx

MOC_FILES =				\
	STD_Application_moc.cxx		\
	STD_MDIDesktop_moc.cxx		\
	STD_SDIDesktop_moc.cxx		\
	STD_TabDesktop_moc.cxx

nodist_libstd_la_SOURCES = $(MOC_FILES)

dist_salomeres_DATA =			\
	resources/cascade.png		\
	resources/close.png		\
	resources/copy.png		\
	resources/cut.png		\
	resources/delete.png		\
	resources/help.png		\
	resources/htile.png		\
	resources/new.png		\
	resources/open.png		\
	resources/paste.png		\
	resources/print.png		\
	resources/redo.png		\
	resources/reset.png		\
	resources/save.png		\
	resources/undo.png		\
	resources/vtile.png		\
	resources/std.ini

nodist_salomeres_DATA =	\
	STD_images.qm	\
	STD_msg_en.qm	\
	STD_msg_fr.qm

libstd_la_CPPFLAGS = $(QT_INCLUDES) -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx
libstd_la_LDFLAGS  = $(QT_MT_LIBS)
libstd_la_LIBADD   = ../Qtx/libqtx.la ../SUIT/libsuit.la
