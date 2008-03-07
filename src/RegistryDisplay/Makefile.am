#  SALOME RegistryDisplay : GUI for Registry server implementation
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
#  Author : Paul RASCLE, EDF
#  Module : SALOME
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libRegistryDisplay.la 

salomeinclude_HEADERS= \
	HelpWindow.hxx \
	IntervalWindow.hxx \
	RegWidget.h

dist_libRegistryDisplay_la_SOURCES= \
	HelpWindow.cxx \
	RegWidget.cxx \
	RegWidgetFactory.cxx \
	IntervalWindow.cxx

MOC_FILES = RegWidget_moc.cxx
nodist_libRegistryDisplay_la_SOURCES= $(MOC_FILES)

libRegistryDisplay_la_CPPFLAGS=$(QT_INCLUDES) @KERNEL_CXXFLAGS@ @CORBA_CXXFLAGS@ @CORBA_INCLUDES@\
	-I$(top_builddir)/salome_adm/unix -I$(top_builddir)/idl
libRegistryDisplay_la_LDFLAGS=$(QT_MT_LIBS) $(KERNEL_LDFLAGS) -lSalomeNS -lOpUtil -lSalomeIDLKernel
