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
#  Author : Vladimir Klyachin (OCN)
#  Module : SalomeApp
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

SUBDIRS=pluginsdemo

if CPPUNIT_IS_OK
if GUI_ENABLE_CORBA
  SUBDIRS += Test
endif
endif

lib_LTLIBRARIES = libSalomeApp.la

salomeinclude_HEADERS =			\
	SalomeApp.h			\
	SalomeApp_Application.h		\
	SalomeApp_DataModel.h		\
	SalomeApp_DataObject.h		\
	SalomeApp_LoadStudiesDlg.h	\
	SalomeApp_Module.h		\
	SalomeApp_Study.h		\
	SalomeApp_ExceptionHandler.h	\
	SalomeApp_PyInterp.h		\
	SalomeApp_Tools.h		\
	SalomeApp_ImportOperation.h	\
	SalomeApp_Filter.h		\
	SalomeApp_TypeFilter.h		\
	SalomeApp_StudyPropertiesDlg.h	\
	SalomeApp_CheckFileDlg.h	\
	SalomeApp_VisualState.h		\
	SalomeApp_ExitDlg.h             \
	SalomeApp_NoteBookDlg.h         \
	SalomeApp_DoubleSpinBox.h       \
	SalomeApp_IntSpinBox.h

dist_libSalomeApp_la_SOURCES =			\
	SalomeApp_Module.cxx			\
	SalomeApp_Application.cxx		\
	SalomeApp_DataModel.cxx			\
	SalomeApp_DataObject.cxx		\
	SalomeApp_LoadStudiesDlg.cxx		\
	SalomeApp_Study.cxx			\
	SalomeApp_ExceptionHandler.cxx		\
	SalomeApp_PyInterp.cxx			\
	SalomeApp_Tools.cxx			\
	SalomeApp_ImportOperation.cxx		\
	SalomeApp_Filter.cxx			\
	SalomeApp_TypeFilter.cxx		\
	SalomeApp_StudyPropertiesDlg.cxx	\
	SalomeApp_ListView.cxx			\
	SalomeApp_CheckFileDlg.cxx		\
	SalomeApp_VisualState.cxx		\
	SalomeApp_ExitDlg.cxx                   \
	SalomeApp_NoteBookDlg.cxx               \
	SalomeApp_DoubleSpinBox.cxx             \
	SalomeApp_IntSpinBox.cxx

MOC_FILES =					\
	SalomeApp_Application_moc.cxx		\
	SalomeApp_DataModel_moc.cxx		\
	SalomeApp_Module_moc.cxx		\
	SalomeApp_LoadStudiesDlg_moc.cxx	\
	SalomeApp_Study_moc.cxx			\
	SalomeApp_StudyPropertiesDlg_moc.cxx	\
	SalomeApp_ListView_moc.cxx		\
	SalomeApp_CheckFileDlg_moc.cxx		\
	SalomeApp_ExitDlg_moc.cxx               \
	SalomeApp_NoteBookDlg_moc.cxx           \
	SalomeApp_DoubleSpinBox_moc.cxx         \
	SalomeApp_IntSpinBox_moc.cxx

nodist_libSalomeApp_la_SOURCES = $(MOC_FILES)

# python modules
salomepython_PYTHON = salome_pluginsmanager.py

dist_salomescript_DATA = addvars2notebook.py

dist_salomeres_DATA =		\
	resources/SalomeApp.ini	\
	resources/SalomeApp.xml

nodist_salomeres_DATA =		\
	SalomeApp_images.qm	\
	SalomeApp_msg_en.qm	\
	SalomeApp_msg_fr.qm

libSalomeApp_la_CPPFLAGS = $(PYTHON_INCLUDES) $(QT_INCLUDES) $(QWT_INCLUDES)	\
	$(CAS_CPPFLAGS) $(VTK_INCLUDES) $(BOOST_CPPFLAGS) @KERNEL_CXXFLAGS@	\
	-I$(srcdir)/../LightApp -I$(srcdir)/../CAM -I$(srcdir)/../Qtx		\
	-I$(srcdir)/../SUIT -I$(srcdir)/../OBJECT -I$(srcdir)/../SVTK		\
	-I$(srcdir)/../STD -I$(srcdir)/../VTKViewer -I$(srcdir)/../ObjBrowser	\
	-I$(srcdir)/../PyConsole -I$(srcdir)/../TOOLSGUI			\
	-I$(srcdir)/../PyInterp -I$(srcdir)/../Session -I$(top_builddir)/idl	\
	-I$(srcdir)/../Event -I$(srcdir)/../CASCatch  -I$(srcdir)/../Prs        \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@ @LIBXML_INCLUDES@
libSalomeApp_la_LDFLAGS = $(PYTHON_LIBS) $(QT_MT_LIBS) 
libSalomeApp_la_LIBADD  = $(KERNEL_LDFLAGS) -lOpUtil -lSALOMELocalTrace -lSalomeDSClient		\
	../SUIT/libsuit.la ../STD/libstd.la ../CAM/libCAM.la ../ObjBrowser/libObjBrowser.la	\
	../Prs/libSalomePrs.la ../SPlot2d/libSPlot2d.la ../GLViewer/libGLViewer.la		\
	../OCCViewer/libOCCViewer.la ../VTKViewer/libVTKViewer.la ../OBJECT/libSalomeObject.la	\
	../SVTK/libSVTK.la ../SOCC/libSOCC.la ../PyInterp/libPyInterp.la			\
	../PyConsole/libPyConsole.la ../LogWindow/libLogWindow.la				\
	../LightApp/libLightApp.la ../TOOLSGUI/libToolsGUI.la ../Session/libSalomeSession.la	\
	../Event/libEvent.la ../CASCatch/libCASCatch.la ../Prs/libSalomePrs.la $(CAS_KERNEL)
