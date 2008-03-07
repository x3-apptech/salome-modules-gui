#  SALOME TOOLSGUI : implementation of desktop "Tools" optioins
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
#
#
#  File   : Makefile.in
#  Author : Marc Tajchman (CEA)
#  Module : SALOME
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libToolsGUI.la

salomeinclude_HEADERS= \
	ToolsGUI.h \
	ToolsGUI_RegWidget.h \
	ToolsGUI_CatalogGeneratorDlg.h

dist_libToolsGUI_la_SOURCES= \
	ToolsGUI_CatalogGeneratorDlg.cxx \
	ToolsGUI_HelpWindow.cxx \
	ToolsGUI_RegWidget.cxx \
	ToolsGUI_IntervalWindow.cxx \
	ToolsGUI.cxx

EXTRA_DIST+=

MOC_FILES= \
	ToolsGUI_CatalogGeneratorDlg_moc.cxx \
	ToolsGUI_RegWidget_moc.cxx \
	ToolsGUI_HelpWindow_moc.cxx \
	ToolsGUI_IntervalWindow_moc.cxx

nodist_libToolsGUI_la_SOURCES= $(MOC_FILES)

# .po files to transform in .qm
nodist_salomeres_DATA= \
	ToolsGUI_icons.qm \
	ToolsGUI_msg_en.qm

libToolsGUI_la_CPPFLAGS = $(QT_INCLUDES) $(CAS_CPPFLAGS) $(BOOST_CPPFLAGS) \
	@KERNEL_CXXFLAGS@ @CORBA_CXXFLAGS@ @CORBA_INCLUDES@ \
	-I$(top_builddir)/salome_adm/unix -I$(top_builddir)/idl \
	-I$(srcdir)/../Qtx -I$(srcdir)/../SUIT
libToolsGUI_la_LDFLAGS = $(QT_MT_LIBS) ../SUIT/libsuit.la $(CAS_KERNEL) $(KERNEL_LDFLAGS) \
	-lSalomeNS -lOpUtil $(top_builddir)/idl/libSalomeIDLGUI.so
