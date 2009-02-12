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
TARGET = PyConsole
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

PYTHONVER=2.4
PYTHONHOME=$$(PYTHONHOME)
PYTHONINC=$${PYTHONHOME}/include/python$${PYTHONVER}
PYTHONLIB=$${PYTHONHOME}/lib

INCLUDEPATH += ../../include $${PYTHONINC}
INCLUDEPATH += ../Qtx ../SUIT ../PyInterp
unix:LIBS  += -L$${PYTHONLIB} -lpython$${PYTHONVER}
win32:LIBS += /LIBPATH:$$(PYTHONLIB)
LIBS += -L../../lib -lqtx -lsuit -lPyInterp

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += PYCONSOLE_EXPORTS

HEADERS  = PyConsole.h
HEADERS += PyConsole_Console.h
HEADERS += PyConsole_Editor.h
HEADERS += PyConsole_Interp.h

SOURCES  = PyConsole_Console.cxx
SOURCES += PyConsole_Editor.cxx
SOURCES += PyConsole_Interp.cxx

TRANSLATIONS = resources/PyConsole_msg_en.ts

includes.files = $$HEADERS
includes.path = ../../include

resources.files = resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
