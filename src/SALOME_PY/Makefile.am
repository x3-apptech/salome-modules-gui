#  SALOME SALOME_PY : binding of VTK graphics and Python
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

lib_LTLIBRARIES= libSalomePy.la
dist_libSalomePy_la_SOURCES= SalomePy.cxx

libSalomePy_la_CPPFLAGS=$(QT_INCLUDES) $(PYTHON_INCLUDES) $(VTK_INCLUDES) \
	$(OGL_INCLUDES) $(CAS_CXXFLAGS) $(BOOST_CPPFLAGS) @KERNEL_CXXFLAGS@ \
	-DHAVE_CONFIG_H -I$(srcdir)/../SalomeApp -I$(srcdir)/../LightApp \
	-I$(srcdir)/../Event -I$(srcdir)/../Session -I$(srcdir)/../SVTK \
	-I$(srcdir)/../Qtx -I$(srcdir)/../SUIT -I$(srcdir)/../CAM \
	-I$(srcdir)/../STD -I$(srcdir)/../VTKViewer -I$(srcdir)/../OBJECT \
	@CAS_CPPFLAGS@ \
	-I$(top_builddir)/salome_adm/unix @CORBA_CXXFLAGS@ @CORBA_INCLUDES@
libSalomePy_la_LDFLAGS=$(PYTHON_LIBS) $(QT_MT_LIBS) $(VTK_LIBS) $(OGL_LIBS) \
	../SalomeApp/libSalomeApp.la \
	-lvtkCommonPythonD -lvtkGraphicsPythonD -lvtkImagingPythonD
