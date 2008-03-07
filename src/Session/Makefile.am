#  SALOME Session : implementation of Session.idl
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

lib_LTLIBRARIES = libSalomeSession.la

salomeinclude_HEADERS= \
	Session_Session_i.hxx \
	Session_ServerLauncher.hxx \
	Session_ServerThread.hxx \
	Session_ServerCheck.hxx \
	SalomeApp_Engine_i.hxx

# Libraries targets

dist_libSalomeSession_la_SOURCES= \
	Session_Session_i.cxx \
	Session_ServerThread.cxx \
	Session_ServerLauncher.cxx \
	Session_ServerCheck.cxx \
	SalomeApp_Engine_i.cxx

libSalomeSession_la_CPPFLAGS= $(QT_MT_INCLUDES) $(PYTHON_INCLUDES) \
	$(HDF5_INCLUDES) $(BOOST_CPPFLAGS) @KERNEL_CXXFLAGS@ $(CAS_CPPFLAGS) \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@ \
	-I$(top_builddir)/salome_adm/unix -I$(top_builddir)/idl \
	-I$(srcdir)/../Qtx -I$(srcdir)/../SUIT -I$(srcdir)/../Event \
	@LIBXML_INCLUDES@

libSalomeSession_la_LDFLAGS= $(QT_MT_LIBS) $(KERNEL_LDFLAGS) -lSalomeNS \
	-lSalomeLifeCycleCORBA -lOpUtil -lSALOMELocalTrace -lSalomeCatalog \
	-lSalomeDSClient $(CAS_KERNEL) -lwith_loggerTraceCollector \
	../SUIT/libsuit.la ../Event/libEvent.la ../../idl/libSalomeIDLGUI.la

# Executable
bin_PROGRAMS= SALOME_Session_Server

dist_SALOME_Session_Server_SOURCES= SALOME_Session_Server.cxx

SALOME_Session_Server_CPPFLAGS= $(libSalomeSession_la_CPPFLAGS)
SALOME_Session_Server_LDADD= $(libSalomeSession_la_LDFLAGS) $(OMNIORB_LIBS) \
	$(HDF5_LIBS) -lSalomeContainer -lSalomeResourcesManager -lTOOLSDS \
	-lSalomeHDFPersist -lSalomeDSImpl -lSalomeGenericObj -lRegistry \
	-lSalomeNotification -lSALOMEBasics -lSalomeLauncher ../Qtx/libqtx.la \
	$(top_builddir)/idl/libSalomeIDLGUI.la \
	libSalomeSession.la

lib-hook:
	ln -sf $(top_builddir)/bin/salome/styles styles

# ../SalomeApp/libSalomeApp.la
