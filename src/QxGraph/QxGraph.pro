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
TARGET = QxGraph
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

QT_INCLUDES = $$(QTDIR)/include $$(QTDIR)/include/QtCore $$(QTDIR)/include/QtGui $$(QTDIR)/include/QtOpenGL $$(QTDIR)/include/QtXml

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

PYTHON_INCLUDES = $$(PYTHONHOME)/include/python2.4

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

QT_MT_LIBS = -L$$(QTDIR)/lib -lQtCore -lQtXml -lQtGui -lQtOpenGL

INCLUDEPATH += $${QT_INCLUDES} $${CAS_CPPFLAGS} $${PYTHON_INCLUDES} $${BOOST_CPPFLAGS} ../Qtx ../SUIT

LIBS += $${QT_MT_LIBS} -L../../lib -lsuit

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += QXGRAPH_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = QxGraph_ActiveItem.h
HEADERS += QxGraph_Prs.h
HEADERS += QxGraph_Canvas.h
HEADERS += QxGraph_CanvasView.h
HEADERS += QxGraph_ViewWindow.h
HEADERS += QxGraph_ViewManager.h
HEADERS += QxGraph_ViewModel.h
HEADERS += QxGraph_Def.h
HEADERS += QxGraph.h

SOURCES  = QxGraph_Prs.cxx
SOURCES += QxGraph_Canvas.cxx
SOURCES += QxGraph_CanvasView.cxx
SOURCES += QxGraph_ViewWindow.cxx
SOURCES += QxGraph_ViewManager.cxx
SOURCES += QxGraph_ViewModel.cxx

TRANSLATIONS = resources/QxGraph_images.ts \
               resources/QxGraph_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
