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
TARGET = Plot2d
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

PYTHONVER=2.4
PYTHONHOME=$$(PYTHONHOME)
PYTHONINC=$${PYTHONHOME}/include/python$${PYTHONVER}

QWTHOME=$$(QWTHOME)
QWTINC=$${QWTHOME}/include
QWTLIB=$${QWTHOME}/lib

INCLUDEPATH += ../../include $${QWTINC} $${PYTHONINC}
INCLUDEPATH += ../Qtx ../SUIT
unix:LIBS  += -L$${QWTLIB} -lqwt
win32:LIBS += /LIBPATH:$$(QWTLIB)
LIBS += -L../../lib -lqtx -lsuit

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += PLOT2D_EXPORTS

HEADERS  = Plot2d.h
HEADERS += Plot2d_Curve.h
HEADERS += Plot2d_FitDataDlg.h
HEADERS += Plot2d_Prs.h
HEADERS += Plot2d_SetupViewDlg.h
HEADERS += Plot2d_ViewFrame.h
HEADERS += Plot2d_ViewManager.h
HEADERS += Plot2d_ViewModel.h
HEADERS += Plot2d_ViewWindow.h
HEADERS += Plot2d_SetupCurveDlg.h
HEADERS += Plot2d_ToolTip.h

SOURCES  = Plot2d_Curve.cxx
SOURCES += Plot2d_FitDataDlg.cxx
SOURCES += Plot2d_Prs.cxx
SOURCES += Plot2d_SetupViewDlg.cxx
SOURCES += Plot2d_ViewFrame.cxx
SOURCES += Plot2d_ViewManager.cxx
SOURCES += Plot2d_ViewModel.cxx
SOURCES += Plot2d_ViewWindow.cxx
SOURCES += Plot2d_SetupCurveDlg.cxx
SOURCES += Plot2d_ToolTip.cxx

TRANSLATIONS = resources/Plot2d_msg_en.ts \
               resources/Plot2d_images.ts

ICONS = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
