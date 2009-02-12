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
TARGET = OCCViewer
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

OGL_INCLUDES = 

OGL_LIBS = -lGL -L/usr/X11R6/lib -lGLU

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

CAS_VIEWER = -L$${CASROOT}/Linux/lib -lTKV3d -lTKService

INCLUDEPATH += ../../include $${OGL_INCLUDES} $${CAS_CPPFLAGS} ../Qtx ../SUIT
LIBS += -L../../lib -lqtx -lsuit $${OGL_LIBS} $${CAS_KERNEL} $${CAS_VIEWER}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCCVIEWER_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = OCCViewer_AISSelector.h
HEADERS += OCCViewer_ViewManager.h
HEADERS += OCCViewer_ViewModel.h
HEADERS += OCCViewer_ViewPort3d.h
HEADERS += OCCViewer_ViewPort.h
HEADERS += OCCViewer_ViewWindow.h
HEADERS += OCCViewer_VService.h
HEADERS += OCCViewer_CreateRestoreViewDlg.h
HEADERS += OCCViewer.h
HEADERS += OCCViewer_ClippingDlg.h
HEADERS += OCCViewer_SetRotationPointDlg.h

SOURCES  = OCCViewer_AISSelector.cxx
SOURCES += OCCViewer_ViewManager.cxx
SOURCES += OCCViewer_ViewModel.cxx
SOURCES += OCCViewer_ViewPort3d.cxx
SOURCES += OCCViewer_ViewPort.cxx
SOURCES += OCCViewer_ViewWindow.cxx
SOURCES += OCCViewer_VService.cxx
SOURCES += OCCViewer_CreateRestoreViewDlg.cxx
SOURCES += OCCViewer_SetRotationPointDlg.cxx
SOURCES += OCCViewer_ClippingDlg.cxx

TRANSLATIONS = resources/OCCViewer_images.ts \
               resources/OCCViewer_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
