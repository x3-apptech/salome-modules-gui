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
TARGET = PyInterp
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

PYTHONVER=2.4
PYTHONHOME=$$(PYTHONHOME)
PYTHONINC=$${PYTHONHOME}/include/python$${PYTHONVER}
PYTHONLIB=$${PYTHONHOME}/lib

INCLUDEPATH += ../../include $${PYTHONINC}
unix:LIBS  += -L$${PYTHONLIB} -lpython$${PYTHONVER}
win32:LIBS += /LIBPATH:$${PYTHONLIB}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += PYINTERP_EXPORTS

HEADERS  = PyInterp.h
HEADERS += PyInterp_Dispatcher.h
HEADERS += PyInterp_Watcher.h
HEADERS += PyInterp_Interp.h

SOURCES  = PyInterp_Dispatcher.cxx
SOURCES += PyInterp_Interp.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
