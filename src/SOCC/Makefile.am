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
#  Author : Alexander Solovyov(OCN)
#  Module : SOCC
# $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libSOCC.la

salomeinclude_HEADERS =		\
	SOCC.h			\
	SOCC_ViewModel.h	\
	SOCC_Prs.h		\
	SOCC_ViewWindow.h

dist_libSOCC_la_SOURCES =	\
	SOCC_ViewModel.cxx	\
	SOCC_Prs.cxx		\
	SOCC_ViewWindow.cxx

MOC_FILES = 			\
	SOCC_ViewModel_moc.cxx	\
	SOCC_ViewWindow_moc.cxx
nodist_libSOCC_la_SOURCES = $(MOC_FILES)

libSOCC_la_CPPFLAGS = $(QT_INCLUDES) $(CAS_CPPFLAGS) $(BOOST_CPPFLAGS)			\
	              -I$(srcdir)/../SUIT -I$(srcdir)/../OBJECT -I$(srcdir)/../Prs	\
	              -I$(srcdir)/../OCCViewer -I$(srcdir)/../Qtx

libSOCC_la_LDFLAGS  = $(QT_MT_LIBS)

libSOCC_la_LIBADD   = ../Qtx/libqtx.la ../SUIT/libsuit.la ../OBJECT/libSalomeObject.la	\
	              ../Prs/libSalomePrs.la ../OCCViewer/libOCCViewer.la
