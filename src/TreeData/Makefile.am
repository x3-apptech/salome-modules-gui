# Copyright (C) 2010-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
# Author : Guillaume Boulant (EDF) 
#

include $(top_srcdir)/adm_local/unix/make_common_starter.am

SUBDIRS = . Test

# moc-files generation
%_moc.cxx: %.hxx
	$(MOC) $< -o $@

mostlyclean-local:
	rm -f @builddir@/*_moc.cxx
	rm -f @builddir@/*.qm

# Libraries targets
lib_LTLIBRARIES = libSalomeTreeData.la

#
# Generic source files
#
salomeinclude_HEADERS = \
	TreeData.hxx \
	DockWidgets.hxx \
	TreeModel.hxx \
	TreeItem.hxx \
	TreeView.hxx \
	TreeObserver.hxx \
	DataModel.hxx \
	DataObject.hxx \
	DataProcessor.hxx \
	TreeGuiManager.hxx

dist_libSalomeTreeData_la_SOURCES = \
	DockWidgets.cxx \
	TreeModel.cxx \
	TreeItem.cxx \
	TreeView.cxx \
	TreeObserver.cxx \
	DataModel.cxx \
	DataObject.cxx \
	DataProcessor.cxx \
	TreeGuiManager.cxx

# MOC pre-processing
MOC_FILES_HXX = \
	TreeModel_moc.cxx \
	TreeView_moc.cxx \
	TreeObserver_moc.cxx

nodist_libSalomeTreeData_la_SOURCES = $(MOC_FILES_HXX)

CORBA_CXXFLAGS=@OMNIORB_CXXFLAGS@ @OMNIORB_INCLUDES@
CORBA_LIBS=@OMNIORB_LIBS@

QT_CXXFLAGS=@QT_INCLUDES@ @QT_MT_INCLUDES@
CAS_CXXFLAGS=@CAS_CPPFLAGS@ @CAS_CXXFLAGS@

BOOST_CPPFLAGS=@BOOST_CPPFLAGS@
BOOST_LIBS=@BOOST_LIBS@

libSalomeTreeData_la_CPPFLAGS = \
	$(QT_CXXFLAGS) \
	$(CAS_CXXFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(CORBA_CXXFLAGS) \
	$(KERNEL_CXXFLAGS) \
	-I$(srcdir)/../SalomeApp \
	-I$(srcdir)/../LightApp  \
	-I$(srcdir)/../CAM  \
	-I$(srcdir)/../STD \
	-I$(srcdir)/../ObjBrowser \
	-I$(srcdir)/../SUIT \
	-I$(srcdir)/../Qtx \
	-I$(srcdir)/../GuiHelpers




libSalomeTreeData_la_LDFLAGS  = \
	$(CORBA_LIBS) $(QT_LIBS)\
	$(KERNEL_LDFLAGS) -lSalomeLifeCycleCORBA -lSalomeKernelHelpers \
	$(top_builddir)/src/SalomeApp/libSalomeApp.la \
	$(top_builddir)/src/LightApp/libLightApp.la \
	$(top_builddir)/src/SUIT/libsuit.la \
	$(top_builddir)/src/Qtx/libqtx.la \
	$(top_builddir)/src/CAM/libCAM.la \
	$(top_builddir)/src/STD/libstd.la \
	$(top_builddir)/src/ObjBrowser/libObjBrowser.la
