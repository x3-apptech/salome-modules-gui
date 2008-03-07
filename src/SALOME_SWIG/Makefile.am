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

# ===============================================================
# Swig targets
# ===============================================================
# (cf. http://www.geocities.com/foetsch/python/swig_linux.htm)
#
# Step 1: build the wrapping source files with swig
#
# libSALOME_LifeCycleCORBA.i -- swig --> swig_wrap.cpp
#                                        libSALOME_Swig.py
#
# Step 2: build the dynamic library from cpp built source files and
#         dependant libraries.
#
# swig_wrap.cpp -- gcc --> swig_wrap.o    |-- link --> _libSALOME_Swig.la
#                          +              |
#                          dependant libs |
#
# The file libSALOME_Swigcmodule.py will be installed in
# <prefix>/lib/python<version>/site-package/salome.
# The library will be installed in the same place.
#

# this option puts it to dist
#BUILT_SOURCES = swig_wrap.cpp

SWIG_FLAGS    = @SWIG_FLAGS@ -I$(srcdir)
SWIG_SOURCES  = libSALOME_Swig.i

nodist_pkgpython_DATA = libSALOME_Swig.py
libSALOME_Swig.py: swig_wrap.cpp

# library
lib_LTLIBRARIES = libSALOME_Swigcmodule.la
libSALOME_Swigcmodule_la_SOURCES  = $(BUILT_SOURCES) $(SWIG_SOURCES) SALOMEGUI_Swig.cxx
nodist_libSALOME_Swigcmodule_la_SOURCES = swig_wrap.cpp
salomeinclude_HEADERS= SALOMEGUI_Swig.hxx SALOMEGUI_Swig.i

libSALOME_Swigcmodule_la_CPPFLAGS =\
	$(QT_INCLUDES) $(PYTHON_INCLUDES) $(CAS_CPPFLAGS) $(VTK_INCLUDES) \
	$(OGL_INCLUDES) $(BOOST_CPPFLAGS) -DHAVE_CONFIG_H @KERNEL_CXXFLAGS@ \
	-I$(top_builddir)/salome_adm/unix -I$(top_builddir)/idl \
	-I$(srcdir)/../SalomeApp -I$(srcdir)/../Qtx -I$(srcdir)/../SUIT \
	-I$(srcdir)/../ObjBrowser -I$(srcdir)/../LightApp -I$(srcdir)/../SOCC \
	-I$(srcdir)/../SVTK -I$(srcdir)/../Event -I$(srcdir)/../OBJECT \
	-I$(srcdir)/../CAM -I$(srcdir)/../STD -I$(srcdir)/../OCCViewer \
	-I$(srcdir)/../Prs -I$(srcdir)/../VTKViewer -I$(srcdir)/../SPlot2d \
	-I$(srcdir)/../Plot2d \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@

libSALOME_Swigcmodule_la_LDFLAGS  = -module
libSALOME_Swigcmodule_la_LIBADD   = \
	../SalomeApp/libSalomeApp.la \
	@PYTHON_LIBS@

swig_wrap.cpp : $(SWIG_SOURCES)
	$(SWIG) $(SWIG_FLAGS) -o $@ $<

CLEANFILES = swig_wrap.cpp

#
# ===============================================================
# Files to be installed
# ===============================================================
#

# Scripts to be installed.
dist_salomescript_DATA= \
	examplevtk1.py \
	test_table.py \
	test_big_table.py \
	test_many_objects.py \
	test_remove_ref.py \
	libSALOME_Swig.py

nodist_salomescript_DATA= \
	supervisionexample.py \
	supervisiongeomexample.py

EXTRA_DIST+= \
	supervisionexample.py.in \
	supervisiongeomexample.py.in

sharedpkgpython_PYTHON= SALOMEGUI_shared_modules.py

install-exec-hook: $(libdir)/_libSALOME_Swig.so

$(libdir)/_libSALOME_Swig.so:
	(cd $(libdir); ln -sf libSALOME_Swigcmodule.so _libSALOME_Swig.so;)
