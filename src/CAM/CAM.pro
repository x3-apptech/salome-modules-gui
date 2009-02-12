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
TARGET = CAM
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

INCLUDEPATH += ../../include ../Qtx ../SUIT ../STD
LIBS += -L../../lib -lqtx -lsuit -lstd

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += CAM_EXPORTS

HEADERS  = CAM.h
HEADERS += CAM_Application.h
HEADERS += CAM_DataModel.h
HEADERS += CAM_DataObject.h
HEADERS += CAM_Module.h
HEADERS += CAM_Study.h

SOURCES  = CAM_Application.cxx
SOURCES += CAM_DataModel.cxx
SOURCES += CAM_DataObject.cxx
SOURCES += CAM_Module.cxx
SOURCES += CAM_Study.cxx

TRANSLATIONS = resources/CAM_images.ts \
               resources/CAM_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm resources/*.xml resources/*.ini
resources.path = ../../resources

INSTALLS += includes resources
