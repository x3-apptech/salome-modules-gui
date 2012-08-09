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

#  SALOMELocalTrace : log on local machine
#  File   : Makefile.in
#  Author : Sergey RUIN
#  Module : SALOME
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libSalomeAppTest.la

salomeinclude_HEADERS = SalomeAppTest.hxx

dist_libSalomeAppTest_la_SOURCES = SalomeAppTest.cxx

libSalomeAppTest_la_CXXFLAGS = @CPPUNIT_INCLUDES@ $(KERNEL_CXXFLAGS)
libSalomeAppTest_la_LIBADD   = @CPPUNIT_LIBS@

bin_PROGRAMS = TestSalomeApp

TestSalomeApp_SOURCES = TestSalomeApp.cxx

TestSalomeApp_CXXFLAGS = $(libSalomeAppTest_la_CXXFLAGS)
TestSalomeApp_LDADD    = $(libSalomeAppTest_la_LIBADD) libSalomeAppTest.la

dist_salomescript_PYTHON = TestSalomeApp.py
UNIT_TEST_PROG = TestSalomeApp
