#  Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  File   : Makefile.in
#  Author : Alexander Solovyov(OCN)
#  Module : SVTK
# $Header: 

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libSVTK.la

salomeinclude_HEADERS= \
	SVTK.h \
	SVTK_Prs.h \
	SVTK_Actor.h \
	SALOME_Actor.h \
	SVTK_RectPicker.h \
	SVTK_DeviceActor.h \
	SVTK_DialogBase.h \
	SVTK_FontWidget.h \
	SVTK_CubeAxesActor2D.h \
	SVTK_Functor.h \
	SVTK_MainWindow.h \
	SVTK_View.h \
	SVTK_ViewManager.h \
	SVTK_ViewModel.h \
	SVTK_ViewWindow.h \
	SVTK_Renderer.h \
	SVTK_InteractorStyle.h \
	SVTK_RenderWindowInteractor.h \
	SVTK_GenericRenderWindowInteractor.h \
	SVTK_Selector.h \
	SVTK_Selection.h \
	SVTK_SelectionEvent.h \
	SVTK_SpaceMouse.h \
	SVTK_Event.h \
	SVTK_ViewModelBase.h \
	SVTK_SetRotationPointDlg.h \
	SVTK_Extension.h

dist_libSVTK_la_SOURCES= \
	SVTK_Prs.cxx \
	SVTK_Actor.cxx \
	SALOME_Actor.cxx \
	SVTK_RectPicker.cxx \
	SVTK_DeviceActor.cxx \
	SVTK_CubeAxesActor2D.cxx \
	SVTK_NonIsometricDlg.cxx \
	SVTK_UpdateRateDlg.cxx \
	SVTK_CubeAxesDlg.cxx \
	SVTK_DialogBase.cxx \
	SVTK_FontWidget.cxx \
	SVTK_Trihedron.cxx \
	SVTK_MainWindow.cxx \
	SVTK_View.cxx \
	SVTK_ViewManager.cxx \
	SVTK_ViewModel.cxx \
	SVTK_Renderer.cxx \
	SVTK_ViewWindow.cxx \
	SVTK_InteractorStyle.cxx \
	SVTK_RenderWindowInteractor.cxx \
	SVTK_GenericRenderWindowInteractor.cxx \
	SVTK_SpaceMouse.cxx \
	SVTK_Selector.cxx \
	SVTK_SetRotationPointDlg.cxx \
	SVTK_Extension.cxx

EXTRA_DIST+= SVTK_SelectorDef.h SVTK_Trihedron.h

MOC_FILES= \
	SVTK_GenericRenderWindowInteractor_moc.cxx \
	SVTK_RenderWindowInteractor_moc.cxx \
	SVTK_NonIsometricDlg_moc.cxx \
	SVTK_UpdateRateDlg_moc.cxx \
	SVTK_CubeAxesDlg_moc.cxx \
	SVTK_FontWidget_moc.cxx \
	SVTK_DialogBase_moc.cxx \
	SVTK_ViewModelBase_moc.cxx \
	SVTK_ViewManager_moc.cxx \
	SVTK_ViewWindow_moc.cxx \
	SVTK_MainWindow_moc.cxx \
	SVTK_ViewModel_moc.cxx \
	SVTK_View_moc.cxx \
	SVTK_SetRotationPointDlg_moc.cxx
nodist_libSVTK_la_SOURCES= $(MOC_FILES)

nodist_salomeres_DATA= \
	SVTK_msg_en.qm \
	SVTK_images.qm

libSVTK_la_CPPFLAGS= \
	$(QT_INCLUDES) \
	$(CAS_CPPFLAGS) \
	$(VTK_INCLUDES) \
	$(QWT_INCLUDES) \
	$(BOOST_CPPFLAGS) \
	-I$(srcdir)/../Qtx -I$(srcdir)/../SUIT -I$(srcdir)/../STD \
	-I$(srcdir)/../CAM -I$(srcdir)/../OBJECT -I$(srcdir)/../Prs \
	-I$(srcdir)/../VTKViewer

libSVTK_la_LDFLAGS= \
	$(VTK_LIBS) \
	$(QT_MT_LIBS) \
	$(CAS_KERNEL) $(CAS_VIEWER) \
	$(QWT_LIBS)

libSVTK_la_LIBADD= ../Qtx/libqtx.la ../SUIT/libsuit.la ../STD/libstd.la \
		   ../CAM/libCAM.la ../OBJECT/libSalomeObject.la \
		   ../Prs/libSalomePrs.la ../VTKViewer/libVTKViewer.la

# Executable
bin_PROGRAMS = SVTK

dist_SVTK_SOURCES= SVTK.cxx

SVTK_CPPFLAGS=$(libSVTK_la_CPPFLAGS)
SVTK_LDFLAGS=$(libSVTK_la_LDFLAGS)
SVTK_LDADD=libSVTK.la $(libSVTK_la_LIBADD)
