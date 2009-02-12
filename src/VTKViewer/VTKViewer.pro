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
TARGET = VTKViewer
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

VTKHOME = $$(VTKHOME)
VTK_INCLUDES = $${VTKHOME}/include/vtk

VTK_LIBS = -L$${VTKHOME}/lib/vtk -L$${VTKHOME}/lib/vtk/python -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets   -lGL -L/usr/X11R6/lib -lGLU -L/usr/X11R6/lib -lX11 -lXt

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

INCLUDEPATH += ../../include $${VTK_INCLUDES} $${CAS_CPPFLAGS} ../Qtx ../SUIT
LIBS += -L../../lib -lqtx -lsuit $${VTK_LIBS} $${CAS_KERNEL}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += VTKVIEWER_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = VTKViewer.h
HEADERS += VTKViewer_CellLocationsArray.h
HEADERS += VTKViewer_Actor.h
HEADERS += VTKViewer_ExtractUnstructuredGrid.h
HEADERS += VTKViewer_ConvexTool.h
HEADERS += VTKViewer_Filter.h
HEADERS += VTKViewer_GeometryFilter.h
HEADERS += VTKViewer_AppendFilter.h
HEADERS += VTKViewer_Algorithm.h
HEADERS += VTKViewer_InteractorStyle.h
HEADERS += VTKViewer_RenderWindow.h
HEADERS += VTKViewer_RenderWindowInteractor.h
HEADERS += VTKViewer_ShrinkFilter.h
HEADERS += VTKViewer_TransformFilter.h
HEADERS += VTKViewer_Transform.h
HEADERS += VTKViewer_Trihedron.h
HEADERS += VTKViewer_Utilities.h
HEADERS += VTKViewer_ViewManager.h
HEADERS += VTKViewer_ViewModel.h
HEADERS += VTKViewer_ViewWindow.h
HEADERS += VTKViewer_Functor.h

SOURCES  = VTKViewer_CellLocationsArray.cxx
SOURCES += VTKViewer_Actor.cxx
SOURCES += VTKViewer_ExtractUnstructuredGrid.cxx
SOURCES += VTKViewer_Filter.cxx
SOURCES += VTKViewer_GeometryFilter.cxx
SOURCES += VTKViewer_AppendFilter.cxx
SOURCES += VTKViewer_InteractorStyle.cxx
SOURCES += VTKViewer_RenderWindow.cxx
SOURCES += VTKViewer_RenderWindowInteractor.cxx
SOURCES += VTKViewer_ShrinkFilter.cxx
SOURCES += VTKViewer_Transform.cxx
SOURCES += VTKViewer_TransformFilter.cxx
SOURCES += VTKViewer_Trihedron.cxx
SOURCES += VTKViewer_Utilities.cxx
SOURCES += VTKViewer_ViewManager.cxx
SOURCES += VTKViewer_ViewModel.cxx
SOURCES += VTKViewer_ConvexTool.cxx
SOURCES += VTKViewer_ViewWindow.cxx

TRANSLATIONS = resources/VTKViewer_images.ts \
               resources/VTKViewer_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
