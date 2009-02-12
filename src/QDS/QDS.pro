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
TARGET = QDS
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

INCLUDEPATH += ../../include $${CAS_CPPFLAGS} ../Qtx ../DDS
LIBS += -L../../lib -lqtx -lDDS $${CAS_KERNEL}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += QDS_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = QDS.h
HEADERS += QDS_CheckBox.h
HEADERS += QDS_ComboBox.h
HEADERS += QDS_Datum.h
HEADERS += QDS_LineEdit.h
HEADERS += QDS_SpinBox.h
HEADERS += QDS_SpinBoxDbl.h
HEADERS += QDS_TextEdit.h
HEADERS += QDS_Validator.h
HEADERS += QDS_RadioBox.h
#HEADERS += QDS_Table.h

SOURCES  = QDS.cxx
SOURCES += QDS_CheckBox.cxx
SOURCES += QDS_ComboBox.cxx
SOURCES += QDS_Datum.cxx
SOURCES += QDS_LineEdit.cxx
SOURCES += QDS_SpinBox.cxx
SOURCES += QDS_SpinBoxDbl.cxx
SOURCES += QDS_TextEdit.cxx
SOURCES += QDS_Validator.cxx
SOURCES += QDS_RadioBox.cxx
#SOURCES += QDS_Table.cxx

TRANSLATIONS = resources/QDS_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
