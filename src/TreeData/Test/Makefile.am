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

# moc-files generation
%_moc.cxx: %.hxx
	$(MOC) $< -o $@

mostlyclean-local:
	rm -f @builddir@/*_moc.cxx
	rm -f @builddir@/*.qm
	rm -f @builddir@/*_moc.cxx @builddir@/ui_*.h

# qt forms files generation (uic)
ui_%.h: %.ui
	$(UIC) -o $@ $<


CORBA_CXXFLAGS=@OMNIORB_CXXFLAGS@ @OMNIORB_INCLUDES@
CORBA_LIBS=@OMNIORB_LIBS@
QT_CXXFLAGS=@QT_INCLUDES@ @QT_MT_INCLUDES@

COMMON_CPP_FLAGS = \
	$(QT_CXXFLAGS) \
	$(CORBA_CXXFLAGS) \
	$(KERNEL_CXXFLAGS) \
	-I$(top_srcdir)/src/GuiHelpers \
	-I$(top_srcdir)/src/TreeData \
	-I.

COMMON_LIBS  = \
	$(CORBA_LIBS) $(QT_LIBS) \
	$(top_builddir)/src/TreeData/libSalomeTreeData.la \
	$(top_builddir)/src/GuiHelpers/libSalomeGuiHelpers.la \
	$(top_builddir)/src/SUIT/libsuit.la \
	$(top_builddir)/src/CAM/libCAM.la \
	$(top_builddir)/src/Plot2d/libPlot2d.la \
	$(top_builddir)/src/LightApp/libLightApp.la \
	$(top_builddir)/src/SalomeApp/libSalomeApp.la \
	$(KERNEL_LDFLAGS) -lSalomeLifeCycleCORBA -lSalomeKernelHelpers @LIBXML_LIBS@

# Program targets
bin_PROGRAMS = TreeData_guitester TreeData_tester

MOC_FILES_HXX = \
	mainwindow_moc.cxx

UIC_FILES = \
	ui_mainwindow.h

BUILT_SOURCES = $(UIC_FILES)

nodist_TreeData_guitester_SOURCES = $(MOC_FILES_HXX) $(UIC_FILES)

TreeData_guitester_SOURCES = \
	testhelper.hxx \
	testhelper.cxx \
	guitester.cxx \
	mainwindow.hxx \
	mainwindow.cxx \
	MyDataModel.hxx \
	MyDataModel.cxx

TreeData_guitester_CPPFLAGS = \
	$(COMMON_CPP_FLAGS)

TreeData_guitester_LDFLAGS = \
	$(COMMON_LIBS)

TreeData_tester_SOURCES = \
	tester.cxx \
	MyDataModel.cxx

TreeData_tester_CPPFLAGS = \
	$(COMMON_CPP_FLAGS)

TreeData_tester_LDFLAGS = \
	$(COMMON_LIBS)

# test data files
testdir = $(salomeresdir)/testdata
test_DATA = \
	data.txt

EXTRA_DIST+=$(test_DATA)
