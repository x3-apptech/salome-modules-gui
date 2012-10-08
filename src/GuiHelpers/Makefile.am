# Copyright (C) 2011-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
# Author : Guillaume Boulant (EDF/R&D) 
#

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files 
salomeinclude_HEADERS =        \
	QtHelper.hxx           \
	SALOME_GuiServices.hxx       \
	SALOME_AppStudyEditor.hxx     \
	StandardApp_Module.hxx     \
	SalomeGuiHelpers.hxx

# Libraries targets
lib_LTLIBRARIES = libSalomeGuiHelpers.la

dist_libSalomeGuiHelpers_la_SOURCES = \
	SALOME_GuiServices.cxx         \
	SALOME_AppStudyEditor.cxx       \
	StandardApp_Module.cxx


QT_CXXFLAGS=@QT_INCLUDES@ @QT_MT_INCLUDES@
CAS_CXXFLAGS=@CAS_CPPFLAGS@ @CAS_CXXFLAGS@
BOOST_CPPFLAGS=@BOOST_CPPFLAGS@
BOOST_LIBS=@BOOST_LIBS@
CORBA_CXXFLAGS=@OMNIORB_CXXFLAGS@ @OMNIORB_INCLUDES@
CORBA_LIBS=@OMNIORB_LIBS@

libSalomeGuiHelpers_la_CPPFLAGS =   \
	$(QT_CXXFLAGS)              \
	$(CAS_CXXFLAGS)             \
	$(BOOST_CPPFLAGS)           \
	$(CORBA_CXXFLAGS)           \
	$(KERNEL_CXXFLAGS)          \
	-I$(srcdir)/../SalomeApp    \
	-I$(srcdir)/../LightApp     \
	-I$(srcdir)/../SUIT         \
	-I$(srcdir)/../Qtx          \
	-I$(srcdir)/../CAM          \
	-I$(srcdir)/../STD          \
	-I$(srcdir)/../OBJECT


libSalomeGuiHelpers_la_LDFLAGS  = \
	../SalomeApp/libSalomeApp.la \
	$(KERNEL_LDFLAGS) -lSalomeKernelHelpers \
	$(CORBA_LIBS)

#
# =======================================================
# Specific definitions for Qt MOC files
#

# moc-files generation
%_moc.cxx: %.hxx
	$(MOC) $< -o $@

# MOC pre-processing
MOC_FILES_HXX = \
	StandardApp_Module_moc.cxx

nodist_libSalomeGuiHelpers_la_SOURCES = $(MOC_FILES_HXX)

EXTRA_DIST+=$(MOC_FILES_HXX:%_moc.cxx=%.hxx)
