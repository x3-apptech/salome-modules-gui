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

#  File   : Makefile.in
#  Author : Nicolas REJNERI
#  Module : SALOME
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

######################

# library target
lib_LTLIBRARIES = libSalomePyQtGUI.la

# moc files (generated my moc)
MOC_FILES = SALOME_PYQT_Module_moc.cxx

# exported header files
salomeinclude_HEADERS =	       \
	SALOME_PYQT_GUI.h      \
	SALOME_PYQT_Module.h   

# library sources
dist_libSalomePyQtGUI_la_SOURCES =	\
	SALOME_PYQT_Module.cxx          

nodist_libSalomePyQtGUI_la_SOURCES = $(MOC_FILES)

# compilation flags
libSalomePyQtGUI_la_CPPFLAGS = $(QT_INCLUDES) $(SIP_INCLUDES) $(PYTHON_INCLUDES)	\
	$(CAS_CPPFLAGS) $(VTK_INCLUDES) $(OGL_INCLUDES) $(BOOST_CPPFLAGS)		\
	-DHAVE_CONFIG_H @KERNEL_CXXFLAGS@ -DCALL_OLD_METHODS				\
	-I. -I$(srcdir)/../../PyInterp -I$(srcdir)/../../SalomeApp		\
	-I$(srcdir)/../../SUIT -I$(srcdir)/../../Qtx -I$(srcdir)/../../LightApp		\
	-I$(srcdir)/../SALOME_PYQT_GUILight                                             \
	-I$(srcdir)/../../Plot2d -I$(srcdir)/../../OCCViewer				\
	-I$(srcdir)/../../SalomeApp -I$(srcdir)/../../CAM -I$(srcdir)/../../STD		\
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@

# linkage flags
libSalomePyQtGUI_la_LIBADD = $(PYTHON_LIBS) $(SIP_LIBS) $(PYQT_LIBS) $(VTK_LIBS)	\
	$(OGL_LIBS) ../../PyInterp/libPyInterp.la ../../SalomeApp/libSalomeApp.la	\
	../../OCCViewer/libOCCViewer.la ../../Plot2d/libPlot2d.la                       \
	../SALOME_PYQT_GUILight/libSalomePyQtGUILight.la
