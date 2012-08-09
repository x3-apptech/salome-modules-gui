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

lib_LTLIBRARIES = libViewerTools.la

salomeinclude_HEADERS= \
	ViewerTools.h \
	ViewerTools_CubeAxesDlgBase.h \
	ViewerTools_DialogBase.h \
	ViewerTools_FontWidgetBase.h

dist_libViewerTools_la_SOURCES= \
	ViewerTools_CubeAxesDlgBase.cxx \
	ViewerTools_DialogBase.cxx \
	ViewerTools_FontWidgetBase.cxx

MOC_FILES= \
	ViewerTools_CubeAxesDlgBase_moc.cxx \
	ViewerTools_DialogBase_moc.cxx \
	ViewerTools_FontWidgetBase_moc.cxx

nodist_libViewerTools_la_SOURCES= $(MOC_FILES)

nodist_salomeres_DATA =	\
	ViewerTools_msg_en.qm \
	ViewerTools_msg_fr.qm

libViewerTools_la_CPPFLAGS =	\
	$(QT_INCLUDES)		\
	-I$(srcdir)/../Qtx

libViewerTools_la_LDFLAGS =	\
	$(QT_MT_LIBS)

libViewerTools_la_LIBADD = ../Qtx/libqtx.la
