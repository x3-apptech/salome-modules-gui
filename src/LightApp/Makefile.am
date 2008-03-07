#  Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
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
#  File   : Makefile.in
#  Author : OCC team (OCN)
#  Module : LightApp
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libLightApp.la

salomeinclude_HEADERS= \
	LightApp.h \
	LightApp_AboutDlg.h \
	LightApp_Application.h \
	LightApp_DataModel.h \
	LightApp_DataObject.h \
	LightApp_DataOwner.h \
	LightApp_DataSubOwner.h \
	LightApp_Dialog.h \
	LightApp_Displayer.h \
	LightApp_Driver.h \
	LightApp_EventFilter.h \
	LightApp_HDFDriver.h \
	LightApp_Module.h \
	LightApp_ModuleDlg.h \
	LightApp_NameDlg.h \
	LightApp_OBFilter.h \
	LightApp_OBSelector.h \
	LightApp_Operation.h \
	LightApp_Selection.h \
	LightApp_SelectionMgr.h \
	LightApp_ShowHideOp.h \
	LightApp_Study.h \
	LightApp_SwitchOp.h \
	LightApp_Preferences.h \
	LightApp_PreferencesDlg.h \
	LightApp_RootObject.h \
	LightApp_UpdateFlags.h \
	LightApp_WidgetContainer.h

if ENABLE_VTKVIEWER
if ENABLE_SALOMEOBJECT
  salomeinclude_HEADERS+= LightApp_VTKSelector.h
endif
endif
if ENABLE_OCCVIEWER
  salomeinclude_HEADERS+= LightApp_OCCSelector.h
endif
if ENABLE_GLVIEWER
  salomeinclude_HEADERS+= LightApp_GLSelector.h
endif

dist_libLightApp_la_SOURCES= \
	LightApp_AboutDlg.cxx \
	LightApp_Application.cxx \
	LightApp_DataModel.cxx \
	LightApp_DataObject.cxx \
	LightApp_DataOwner.cxx \
	LightApp_DataSubOwner.cxx \
	LightApp_Dialog.cxx \
	LightApp_Displayer.cxx \
	LightApp_Driver.cxx \
	LightApp_EventFilter.cxx \
	LightApp_HDFDriver.cxx \
	LightApp_Module.cxx \
	LightApp_ModuleDlg.cxx \
	LightApp_NameDlg.cxx \
	LightApp_OBFilter.cxx \
	LightApp_OBSelector.cxx \
	LightApp_Operation.cxx \
	LightApp_Selection.cxx \
	LightApp_SelectionMgr.cxx \
	LightApp_ShowHideOp.cxx \
	LightApp_Study.cxx \
	LightApp_SwitchOp.cxx \
	LightApp_Preferences.cxx \
	LightApp_PreferencesDlg.cxx \
	LightApp_WidgetContainer.cxx

if ENABLE_VTKVIEWER
if ENABLE_SALOMEOBJECT
  dist_libLightApp_la_SOURCES+= LightApp_VTKSelector.cxx
endif
endif
if ENABLE_OCCVIEWER
  dist_libLightApp_la_SOURCES+= LightApp_OCCSelector.cxx
endif
if ENABLE_GLVIEWER
  dist_libLightApp_la_SOURCES+= LightApp_GLSelector.cxx
endif

MOC_FILES= \
	LightApp_AboutDlg_moc.cxx \
	LightApp_Application_moc.cxx \
	LightApp_DataModel_moc.cxx \
	LightApp_Dialog_moc.cxx \
	LightApp_OBSelector_moc.cxx \
	LightApp_Operation_moc.cxx \
	LightApp_Module_moc.cxx \
	LightApp_ModuleDlg_moc.cxx \
	LightApp_NameDlg_moc.cxx \
	LightApp_SelectionMgr_moc.cxx \
	LightApp_ShowHideOp_moc.cxx \
	LightApp_Study_moc.cxx \
	LightApp_SwitchOp_moc.cxx \
	LightApp_Preferences_moc.cxx \
	LightApp_PreferencesDlg_moc.cxx \
	LightApp_WidgetContainer_moc.cxx

if ENABLE_VTKVIEWER
if ENABLE_SALOMEOBJECT
  MOC_FILES+= LightApp_VTKSelector_moc.cxx
endif
endif
if ENABLE_OCCVIEWER
  MOC_FILES+= LightApp_OCCSelector_moc.cxx
endif
if ENABLE_GLVIEWER
  MOC_FILES+= LightApp_GLSelector_moc.cxx
endif
nodist_libLightApp_la_SOURCES= $(MOC_FILES)

dist_salomeres_DATA= \
	resources/icon_about.png \
	resources/icon_applogo.png \
	resources/icon_default.png \
	resources/icon_module.png \
	resources/icon_module_big.png \
	resources/icon_select.png \
	resources/LightApp.ini \
	resources/LightApp.xml

nodist_salomeres_DATA= \
	LightApp_images.qm \
	LightApp_msg_en.qm

libLightApp_la_CPPFLAGS=$(PYTHON_INCLUDES) $(QT_INCLUDES) $(CAS_CPPFLAGS) \
	$(HDF5_INCLUDES) @KERNEL_CXXFLAGS@ -I$(srcdir)/../SUIT -I$(srcdir)/../STD \
	-I$(srcdir)/../CAM -I$(srcdir)/../ObjBrowser -I$(srcdir)/../LogWindow \
	-I$(srcdir)/../Prs -I$(srcdir)/../Qtx
libLightApp_la_LDFLAGS=$(QT_MT_LIBS)

if ENABLE_VTKVIEWER
  libLightApp_la_CPPFLAGS+= $(VTK_INCLUDES) -I$(srcdir)/../VTKViewer
else
  libLightApp_la_CPPFLAGS+= -DDISABLE_VTKVIEWER
endif
if ENABLE_PLOT2DVIEWER
  libLightApp_la_CPPFLAGS+= $(QWT_INCLUDES) -I$(srcdir)/../Plot2d
else
  libLightApp_la_CPPFLAGS+= -DDISABLE_PLOT2DVIEWER
endif
if ENABLE_OCCVIEWER
  libLightApp_la_CPPFLAGS+= -I$(srcdir)/../OCCViewer
else !ENABLE_OCCVIEWER
  libLightApp_la_CPPFLAGS+= -DDISABLE_OCCVIEWER
endif
if ENABLE_PYCONSOLE
  libLightApp_la_CPPFLAGS+= $(PYTHON_INCLUDES) -I$(srcdir)/../PythonConsole -I$(srcdir)/../PyInterp
  libLightApp_la_LDFLAGS+= $(PYTHON_LIBS)
else !ENABLE_PYCONSOLE
  libLightApp_la_CPPFLAGS+= -DDISABLE_PYCONSOLE
endif
if ENABLE_GLVIEWER
  libLightApp_la_CPPFLAGS+= -I$(srcdir)/../GLViewer
else !ENABLE_GLVIEWER
  libLightApp_la_CPPFLAGS+= -DDISABLE_GLVIEWER
endif
if ENABLE_SUPERVGRAPHVIEWER
  libLightApp_la_CPPFLAGS+= -I$(srcdir)/../SUPERVGraph
else !ENABLE_SUPERVGRAPHVIEWER
  libLightApp_la_CPPFLAGS+= -DDISABLE_SUPERVGRAPHVIEWER
endif
if ENABLE_QXGRAPHVIEWER
  libLightApp_la_CPPFLAGS+= -I$(srcdir)/../QxGraph
else !ENABLE_QXGRAPHVIEWER
  libLightApp_la_CPPFLAGS+= -DDISABLE_QXGRAPHVIEWER
endif

if ENABLE_SALOMEOBJECT

  libLightApp_la_CPPFLAGS+= -I$(srcdir)/../OBJECT
if ENABLE_PLOT2DVIEWER
  libLightApp_la_CPPFLAGS+= -I$(srcdir)/../SPlot2d
endif
if ENABLE_OCCVIEWER
  libLightApp_la_CPPFLAGS+= -I$(srcdir)/../SOCC
endif
if ENABLE_VTKVIEWER
  libLightApp_la_CPPFLAGS+= -I$(srcdir)/../SVTK
endif
else !ENABLE_SALOMEOBJECT
  libLightApp_la_CPPFLAGS+= -DDISABLE_SALOMEOBJECT
  
endif

libLightApp_la_LIBADD= ../SUIT/libsuit.la ../STD/libstd.la ../CAM/libCAM.la \
	../ObjBrowser/libObjBrowser.la ../LogWindow/libLogWindow.la $(CAS_KERNEL) \
	../Prs/libSalomePrs.la $(HDF5_LIBS) $(KERNEL_LDFLAGS) -lSalomeHDFPersist

if ENABLE_SALOMEOBJECT
  libLightApp_la_LIBADD+= ../OBJECT/libSalomeObject.la
endif
if ENABLE_VTKVIEWER
  libLightApp_la_LIBADD+= ../VTKViewer/libVTKViewer.la
if ENABLE_SALOMEOBJECT
  libLightApp_la_LIBADD+= ../SVTK/libSVTK.la
endif
endif
if ENABLE_OCCVIEWER
  libLightApp_la_LIBADD+= ../OCCViewer/libOCCViewer.la
if ENABLE_SALOMEOBJECT
  libLightApp_la_LIBADD+= ../SOCC/libSOCC.la
endif
endif
if ENABLE_GLVIEWER
  libLightApp_la_LIBADD+= ../GLViewer/libGLViewer.la
endif
if ENABLE_PLOT2DVIEWER
  libLightApp_la_LIBADD+= ../Plot2d/libPlot2d.la
if ENABLE_SALOMEOBJECT
  libLightApp_la_LIBADD+= ../SPlot2d/libSPlot2d.la
endif
endif
if ENABLE_PYCONSOLE
  libLightApp_la_LIBADD+= ../PyInterp/libPyInterp.la ../PythonConsole/libPythonConsole.la
endif
if ENABLE_SUPERVGRAPHVIEWER
  libLightApp_la_LIBADD+= ../SUPERVGraph/libSUPERVGraph.la
endif
if ENABLE_QXGRAPHVIEWER
  libLightApp_la_LIBADD+= ../QxGraph/libQxGraph.la
endif
