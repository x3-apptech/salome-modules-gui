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
TARGET = SalomeApp
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

PYTHON_INCLUDES = $$(PYTHONHOME)/include/python2.4

QT_INCLUDES = $$(QTDIR)/include $$(QTDIR)/include/QtCore $$(QTDIR)/include/QtGui $$(QTDIR)/include/QtOpenGL $$(QTDIR)/include/QtXml

QWT_INCLUDES = $$(QWTHOME)/include

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

VTK_INCLUDES = $$(VTKHOME)/include/vtk

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

CORBA_INCLUDES = $$(OMNIORBDIR)/include $$(OMNIORBDIR)/include/omniORB4 $$(OMNIORBDIR)/include/COS

HDF5_INCLUDES = $$(HDF5HOME)/include

PYTHON_LIBS = -L$$(PYTHONHOME)/lib/python2.4/config -lpython2.4 -ldl -lutil

QT_MT_LIBS = -L$$(QTDIR)/lib -lQtCore -lQtXml -lQtGui -lQtOpenGL

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel


INCLUDEPATH += $${PYTHON_INCLUDES} $${QT_INCLUDES} $${QWT_INCLUDES} $${CAS_CPPFLAGS} $${VTK_INCLUDES} $${BOOST_CPPFLAGS} $${KERNEL_CXXFLAGS} ../LightApp ../CAM ../Qtx ../SUIT ../OBJECT ../SVTK ../STD ../VTKViewer ../PyConsole ../TOOLSGUI ../PyInterp ../Session ../../idl ../Event ../../salome_adm/unix $${CORBA_INCLUDES} #../ObjBrowser

LIBS += $${QT_MT_LIBS} $${PYTHON_LIBS} $${KERNEL_LDFLAGS} -lOpUtil -lSALOMELocalTrace -lSalomeDSClient -L../../lib -lsuit -lstd -lCAM -lSalomePrs -lSPlot2d -lGLViewer -lOCCViewer -lVTKViewer -lSalomeObject -lSVTK -lSOCC -lPyInterp -lPyConsole -lLogWindow -lLightApp -lToolsGUI $${CAS_KERNEL} #-lObjBrowser

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += SALOMEAPP_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS  = SalomeApp.h
HEADERS += SalomeApp_Application.h
HEADERS += SalomeApp_DataModel.h
HEADERS += SalomeApp_DataObject.h
HEADERS += SalomeApp_LoadStudiesDlg.h
HEADERS += SalomeApp_Module.h
HEADERS += SalomeApp_Study.h
HEADERS += SalomeApp_ExceptionHandler.h
HEADERS += SalomeApp_Tools.h
HEADERS += SalomeApp_ImportOperation.h
HEADERS += SalomeApp_Filter.h
HEADERS += SalomeApp_TypeFilter.h
HEADERS += SalomeApp_StudyPropertiesDlg.h
HEADERS += SalomeApp_CheckFileDlg.h
HEADERS += SalomeApp_VisualState.h

SOURCES  = SalomeApp_Module.cxx
SOURCES += SalomeApp_Application.cxx
SOURCES += SalomeApp_DataModel.cxx
SOURCES += SalomeApp_DataObject.cxx
SOURCES += SalomeApp_LoadStudiesDlg.cxx
SOURCES += SalomeApp_Study.cxx
SOURCES += SalomeApp_ExceptionHandler.cxx
SOURCES += SalomeApp_PyInterp.cxx
SOURCES += SalomeApp_Tools.cxx
SOURCES += SalomeApp_ImportOperation.cxx
SOURCES += SalomeApp_Filter.cxx
SOURCES += SalomeApp_TypeFilter.cxx
SOURCES += SalomeApp_StudyPropertiesDlg.cxx
SOURCES += SalomeApp_ListView.cxx
SOURCES += SalomeApp_CheckFileDlg.cxx
SOURCES += SalomeApp_VisualState.cxx

TRANSLATIONS = resources/SalomeApp_images.ts \
               resources/SalomeApp_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
