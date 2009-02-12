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
TARGET = ToolsGUI
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

CORBA_INCLUDES = $$(OMNIORBDIR)/include $$(OMNIORBDIR)/include/omniORB4 $$(OMNIORBDIR)/include/COS

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

INCLUDEPATH += $${CAS_CPPFLAGS} $${BOOST_CPPFLAGS} $${KERNEL_CXXFLAGS} $${CORBA_INCLUDES} ../../salome_adm/unix ../../idl ../Qtx ../SUIT
LIBS += -L../../lib -lsuit -lSalomeNS -lOpUtil $${CAS_KERNEL} $${KERNEL_LDFLAGS}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = ToolsGUI.h
HEADERS += ToolsGUI_RegWidget.h
HEADERS += ToolsGUI_CatalogGeneratorDlg.h

SOURCES  = ToolsGUI_CatalogGeneratorDlg.cxx
SOURCES += ToolsGUI_RegWidget.cxx
SOURCES += ToolsGUI.cxx

TRANSLATIONS = resources/ToolsGUI_icons.ts \
               resources/ToolsGUI_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
