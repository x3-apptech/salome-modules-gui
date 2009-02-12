#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
TEMPLATE = lib
TARGET = LightApp
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

PYTHON_INCLUDES = $$(PYTHONHOME)/include/python2.4

QT_INCLUDES = $$(QTDIR)/include $$(QTDIR)/include/QtCore $$(QTDIR)/include/QtGui $$(QTDIR)/include/QtOpenGL $$(QTDIR)/include/QtXml

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

HDF5_INCLUDES = $$(HDF5HOME)/include

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

VTK_INCLUDES = $$(VTKHOME)/include/vtk

QWT_INCLUDES = $$(QWTHOME)/include

PYTHON_INCLUDES = $$(PYTHONHOME)/include/python2.4

INCLUDEPATH += $${PYTHON_INCLUDES} $${QT_INCLUDES} $${CAS_CPPFLAGS} $${HDF5_INCLUDES} $${KERNEL_CXXFLAGS} ../SUIT ../STD ../CAM ../LogWindow ../Prs ../Qtx ../Event #../ObjBrowser

#if ENABLE_VTKVIEWER
  INCLUDEPATH += $${VTK_INCLUDES} ../VTKViewer
#else
#  DEFINES += DISABLE_VTKVIEWER
#endif

#if ENABLE_PLOT2DVIEWER
  INCLUDEPATH += $${QWT_INCLUDES} ../Plot2d
#else
#  DEFINES += DISABLE_PLOT2DVIEWER
#endif

#if ENABLE_OCCVIEWER
  INCLUDEPATH += ../OCCViewer
#else !ENABLE_OCCVIEWER
#  DEFINES += DISABLE_OCCVIEWER
#endif

#if ENABLE_PYCONSOLE
  INCLUDEPATH += $${PYTHON_INCLUDES} ../PyConsole ../PyInterp
#else !ENABLE_PYCONSOLE
#  DEFINES += DISABLE_PYCONSOLE
#endif

#if ENABLE_GLVIEWER
  INCLUDEPATH += ../GLViewer
#else !ENABLE_GLVIEWER
#  DEFINES += DISABLE_GLVIEWER
#endif

#if ENABLE_SUPERVGRAPHVIEWER
  INCLUDEPATH += ../SUPERVGraph
#else !ENABLE_SUPERVGRAPHVIEWER
#  DEFINES += DISABLE_SUPERVGRAPHVIEWER
#endif

#if ENABLE_SALOMEOBJECT

  INCLUDEPATH += ../OBJECT
#if ENABLE_PLOT2DVIEWER
  INCLUDEPATH += ../SPlot2d
#endif

#if ENABLE_OCCVIEWER
  INCLUDEPATH += ../SOCC
#endif

#if ENABLE_VTKVIEWER
  INCLUDEPATH += ../SVTK
#endif

#else !ENABLE_SALOMEOBJECT
#  DEFINES += DISABLE_SALOMEOBJECT
  
#endif

QT_MT_LIBS = -L$$(QTDIR)/lib -lQtCore -lQtXml -lQtGui -lQtOpenGL

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

HDF5_LIBS = -L$$(HDF5HOME)/lib -lhdf5

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

PYTHON_LIBS = -L$$(PYTHONHOME)/lib/python2.4/config -lpython2.4 -ldl -lutil

LIBS += $${QT_MT_LIBS} -L../../lib -lsuit -lstd -lCAM -lLogWindow $${CAS_KERNEL} -lSalomePrs $${HDF5_LIBS} $${KERNEL_LDFLAGS} -lSalomeHDFPersist #-lObjBrowser

#if ENABLE_PYCONSOLE
  LIBS += $${PYTHON_LIBS}
#endif

#if ENABLE_SALOMEOBJECT
  LIBS += -lSalomeObject
#endif

#if ENABLE_VTKVIEWER
  LIBS += -lVTKViewer
#if ENABLE_SALOMEOBJECT
  LIBS += -lSVTK
#endif
#endif

#if ENABLE_OCCVIEWER
  LIBS += -lOCCViewer
#if ENABLE_SALOMEOBJECT
  LIBS += -lSOCC
#endif
#endif

#if ENABLE_GLVIEWER
  LIBS += -lGLViewer
#endif

#if ENABLE_PLOT2DVIEWER
  LIBS += -lPlot2d
#if ENABLE_SALOMEOBJECT
  LIBS += -lSPlot2d
#endif
#endif

#if ENABLE_PYCONSOLE
  LIBS += -lPyInterp -lPyConsole
#endif

#if ENABLE_SUPERVGRAPHVIEWER
  LIBS += -lSUPERVGraph
#endif

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += LIGHTAPP_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = LightApp.h
HEADERS += LightApp_AboutDlg.h
HEADERS += LightApp_Application.h
HEADERS += LightApp_DataModel.h
HEADERS += LightApp_DataObject.h
HEADERS += LightApp_DataOwner.h
HEADERS += LightApp_DataSubOwner.h
HEADERS += LightApp_Dialog.h
HEADERS += LightApp_Displayer.h
HEADERS += LightApp_Driver.h
HEADERS += LightApp_EventFilter.h
HEADERS += LightApp_HDFDriver.h
HEADERS += LightApp_Module.h
HEADERS += LightApp_ModuleAction.h
HEADERS += LightApp_ModuleDlg.h
HEADERS += LightApp_NameDlg.h
#HEADERS += LightApp_OBFilter.h
HEADERS += LightApp_OBSelector.h
HEADERS += LightApp_Operation.h
HEADERS += LightApp_Selection.h
HEADERS += LightApp_SelectionMgr.h
HEADERS += LightApp_ShowHideOp.h
HEADERS += LightApp_Study.h
HEADERS += LightApp_SwitchOp.h
HEADERS += LightApp_Preferences.h
HEADERS += LightApp_PreferencesDlg.h
HEADERS += LightApp_RootObject.h
HEADERS += LightApp_UpdateFlags.h
HEADERS += LightApp_WidgetContainer.h

#if ENABLE_VTKVIEWER
#if ENABLE_SALOMEOBJECT
  HEADERS += LightApp_VTKSelector.h
#endif
#endif
#if ENABLE_OCCVIEWER
  HEADERS += LightApp_OCCSelector.h
#endif
#if ENABLE_GLVIEWER
  HEADERS += LightApp_GLSelector.h
#endif

SOURCES  = LightApp_AboutDlg.cxx
SOURCES += LightApp_Application.cxx
SOURCES += LightApp_DataModel.cxx
SOURCES += LightApp_DataObject.cxx
SOURCES += LightApp_DataOwner.cxx
SOURCES += LightApp_DataSubOwner.cxx
SOURCES += LightApp_Dialog.cxx
SOURCES += LightApp_Displayer.cxx
SOURCES += LightApp_Driver.cxx
SOURCES += LightApp_EventFilter.cxx
SOURCES += LightApp_HDFDriver.cxx
SOURCES += LightApp_Module.cxx
SOURCES += LightApp_ModuleAction.cxx
SOURCES += LightApp_ModuleDlg.cxx
SOURCES += LightApp_NameDlg.cxx
#SOURCES += LightApp_OBFilter.cxx
SOURCES += LightApp_OBSelector.cxx
SOURCES += LightApp_Operation.cxx
SOURCES += LightApp_Selection.cxx
SOURCES += LightApp_SelectionMgr.cxx
SOURCES += LightApp_ShowHideOp.cxx
SOURCES += LightApp_Study.cxx
SOURCES += LightApp_SwitchOp.cxx
SOURCES += LightApp_Preferences.cxx
SOURCES += LightApp_PreferencesDlg.cxx
SOURCES += LightApp_WidgetContainer.cxx

#if ENABLE_VTKVIEWER
#if ENABLE_SALOMEOBJECT
  SOURCES += LightApp_VTKSelector.cxx
#endif
#endif
#if ENABLE_OCCVIEWER
  SOURCES += LightApp_OCCSelector.cxx
#endif
#if ENABLE_GLVIEWER
  SOURCES += LightApp_GLSelector.cxx
#endif

TRANSLATIONS = resources/LightApp_images.ts \
               resources/LightApp_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
