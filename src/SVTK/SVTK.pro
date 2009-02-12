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
TARGET = SVTK
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

VTKHOME = $$(VTKHOME)
VTK_INCLUDES = $${VTKHOME}/include/vtk

VTK_LIBS = -L$${VTKHOME}/lib/vtk -L$${VTKHOME}/lib/vtk/python -lvtkCommon -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkIO -lvtkRendering -lvtkHybrid -lvtkParallel -lvtkWidgets   -lGL -L/usr/X11R6/lib -lGLU -L/usr/X11R6/lib -lX11 -lXt

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

OCC_LIBS = 

INCLUDEPATH += ../../include $${CAS_CPPFLAGS} $${VTK_INCLUDES} $${BOOST_CPPFLAGS} ../Qtx ../SUIT ../OBJECT ../Prs ../VTKViewer
LIBS += -L../../lib -lqtx -lsuit -lSalomeObject -lSalomePrs -lVTKViewer $${OCC_LIBS} $${VTK_LIBS}

CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += SVTK_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS

HEADERS  = SVTK.h
HEADERS += SVTK_Prs.h
HEADERS += SVTK_Actor.h
HEADERS += SALOME_Actor.h
HEADERS += SVTK_RectPicker.h
HEADERS += SVTK_DeviceActor.h
HEADERS += SVTK_DialogBase.h
HEADERS += SVTK_FontWidget.h
HEADERS += SVTK_CubeAxesActor2D.h
HEADERS += SVTK_Functor.h
HEADERS += SVTK_View.h
HEADERS += SVTK_ViewManager.h
HEADERS += SVTK_ViewModel.h
HEADERS += SVTK_ViewWindow.h
HEADERS += SVTK_Renderer.h
HEADERS += SVTK_InteractorStyle.h
HEADERS += SVTK_KeyFreeInteractorStyle.h
HEADERS += SVTK_RenderWindowInteractor.h
HEADERS += SVTK_GenericRenderWindowInteractor.h
HEADERS += SVTK_Selector.h
HEADERS += SVTK_Selection.h
HEADERS += SVTK_SelectionEvent.h
HEADERS += SVTK_SpaceMouse.h
HEADERS += SVTK_Event.h
HEADERS += SVTK_ViewModelBase.h
HEADERS += SVTK_SetRotationPointDlg.h
HEADERS += SVTK_ViewParametersDlg.h
HEADERS += SVTK_ComboAction.h
HEADERS += SVTK_Extension.h

SOURCES  = SVTK_Prs.cxx
SOURCES += SVTK_Actor.cxx
SOURCES += SALOME_Actor.cxx
SOURCES += SVTK_RectPicker.cxx
SOURCES += SVTK_DeviceActor.cxx
SOURCES += SVTK_CubeAxesActor2D.cxx
SOURCES += SVTK_NonIsometricDlg.cxx
SOURCES += SVTK_UpdateRateDlg.cxx
SOURCES += SVTK_CubeAxesDlg.cxx
SOURCES += SVTK_DialogBase.cxx
SOURCES += SVTK_FontWidget.cxx
SOURCES += SVTK_Trihedron.cxx
SOURCES += SVTK_View.cxx
SOURCES += SVTK_ViewManager.cxx
SOURCES += SVTK_ViewModel.cxx
SOURCES += SVTK_Renderer.cxx
SOURCES += SVTK_ViewWindow.cxx
SOURCES += SVTK_InteractorStyle.cxx
SOURCES += SVTK_KeyFreeInteractorStyle.cxx
SOURCES += SVTK_RenderWindowInteractor.cxx
SOURCES += SVTK_GenericRenderWindowInteractor.cxx
SOURCES += SVTK_SpaceMouse.cxx
SOURCES += SVTK_Selector.cxx
SOURCES += SVTK_SetRotationPointDlg.cxx
SOURCES += SVTK_ViewParametersDlg.cxx
SOURCES += SVTK_ComboAction.cxx
SOURCES += SVTK_Extension.cxx

TRANSLATIONS = resources/SVTK_images.ts \
               resources/SVTK_msg_en.ts

ICONS   = resources/*.png

includes.files = $$HEADERS
includes.path = ../../include

resources.files = $$ICONS resources/*.qm
resources.path = ../../resources

INSTALLS += includes resources
