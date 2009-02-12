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
TARGET = SPlot2d
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

QWTHOME=$$(QWTHOME)
QWTINC=$${QWTHOME}/include
QWTLIB=$${QWTHOME}/lib

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

INCLUDEPATH += ../../include $${CAS_CPPFLAGS} ../../include $${QWTINC} $${BOOST_CPPFLAGS}
INCLUDEPATH += ../Qtx ../SUIT ../Plot2d ../Prs ../OBJECT
unix:LIBS  += -L$${QWTLIB} -lqwt
win32:LIBS += /LIBPATH:$$(QWTLIB)
LIBS += -L../../lib -lsuit -lPlot2d -lSalomePrs

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += SPLOT2D_EXPORTS

HEADERS  = SPlot2d.h
HEADERS  = SPlot2d_Curve.h
HEADERS  = SPlot2d_Prs.h
HEADERS  = SPlot2d_ViewModel.h
HEADERS  = SPlot2d_ViewWindow.h

SOURCES   = SPlot2d_Curve.cxx
SOURCES  += SPlot2d_Curve.cxx
SOURCES  += SPlot2d_Prs.cxx
SOURCES  += SPlot2d_ViewModel.cxx
SOURCES  += SPlot2d_ViewWindow.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
