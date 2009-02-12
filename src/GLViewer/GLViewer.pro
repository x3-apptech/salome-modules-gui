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
TARGET = GLViewer
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

GL_LIBS = -L/usr/X11R6/lib$$(LIB_LOCATION_SUFFIX) -lGLU

INCLUDEPATH += $$(QTDIR)/include/QtOpenGL ../../include $${CAS_CPPFLAGS} ../Qtx ../SUIT
LIBS += -L../../lib -lqtx -lsuit $${GL_LIBS} $${CAS_KERNEL}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += GLVIEWER_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = GLViewer.h
HEADERS += GLViewer_AspectLine.h
HEADERS += GLViewer_BaseDrawers.h
HEADERS += GLViewer_BaseObjects.h
HEADERS += GLViewer_Compass.h
HEADERS += GLViewer_Context.h
HEADERS += GLViewer_CoordSystem.h
HEADERS += GLViewer_Defs.h
HEADERS += GLViewer_Drawer.h
HEADERS += GLViewer_Geom.h
HEADERS += GLViewer_Grid.h
HEADERS += GLViewer_Group.h
HEADERS += GLViewer_MimeData.h
HEADERS += GLViewer_Object.h
HEADERS += GLViewer_Selector.h
HEADERS += GLViewer_Selector2d.h
HEADERS += GLViewer_Text.h
HEADERS += GLViewer_Tools.h
HEADERS += GLViewer_ToolTip.h
HEADERS += GLViewer_ViewFrame.h
HEADERS += GLViewer_ViewManager.h
HEADERS += GLViewer_ViewPort.h
HEADERS += GLViewer_ViewPort2d.h
HEADERS += GLViewer_Viewer.h
HEADERS += GLViewer_Viewer2d.h
HEADERS += GLViewer_Widget.h

SOURCES  = GLViewer_AspectLine.cxx
SOURCES += GLViewer_BaseDrawers.cxx
SOURCES += GLViewer_BaseObjects.cxx
SOURCES += GLViewer_Compass.cxx
SOURCES += GLViewer_Context.cxx
SOURCES += GLViewer_CoordSystem.cxx
SOURCES += GLViewer_Drawer.cxx
SOURCES += GLViewer_Geom.cxx
SOURCES += GLViewer_Grid.cxx
SOURCES += GLViewer_Group.cxx
SOURCES += GLViewer_MimeData.cxx
SOURCES += GLViewer_Object.cxx
SOURCES += GLViewer_Selector.cxx
SOURCES += GLViewer_Selector2d.cxx
SOURCES += GLViewer_Text.cxx
SOURCES += GLViewer_Tools.cxx
SOURCES += GLViewer_ToolTip.cxx
SOURCES += GLViewer_ViewFrame.cxx
SOURCES += GLViewer_ViewManager.cxx
SOURCES += GLViewer_ViewPort.cxx
SOURCES += GLViewer_ViewPort2d.cxx
SOURCES += GLViewer_Viewer.cxx
SOURCES += GLViewer_Viewer2d.cxx
SOURCES += GLViewer_Widget.cxx

TRANSLATIONS = resources/GLViewer_images.ts \
               resources/GLViewer_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
