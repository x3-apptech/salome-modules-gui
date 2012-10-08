# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

#  File   : Makefile.in
#  Author : Alexander Solovyov(OCN)
#  Module : SVTK
# $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libSVTK.la

salomeinclude_HEADERS= \
	SVTK.h \
	SVTK_Prs.h \
	SVTK_Actor.h \
	SALOME_Actor.h \
	SVTK_RectPicker.h \
	SVTK_DeviceActor.h \
	SVTK_FontWidget.h \
	SVTK_CubeAxesActor2D.h \
	SVTK_Functor.h \
	SVTK_View.h \
	SVTK_ViewManager.h \
	SVTK_ViewModel.h \
	SVTK_ViewWindow.h \
	SVTK_Renderer.h \
	SVTK_InteractorStyle.h \
	SVTK_KeyFreeInteractorStyle.h \
	SVTK_RenderWindowInteractor.h \
	SVTK_GenericRenderWindowInteractor.h \
	SVTK_Selector.h \
	SVTK_Selection.h \
	SVTK_SelectionEvent.h \
	SVTK_SpaceMouse.h \
	SVTK_Event.h \
	SVTK_ViewModelBase.h \
	SVTK_SetRotationPointDlg.h \
	SVTK_ViewParameterDlg.h \
	SVTK_ComboAction.h \
	SVTK_Recorder.h \
	SVTK_RecorderDlg.h \
	SVTK_ImageWriter.h \
	SVTK_ImageWriterMgr.h \
	SVTK_Utils.h

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
	SVTK_FontWidget.cxx \
	SVTK_Trihedron.cxx \
	SVTK_View.cxx \
	SVTK_ViewManager.cxx \
	SVTK_ViewModel.cxx \
	SVTK_Renderer.cxx \
	SVTK_ViewWindow.cxx \
	SVTK_InteractorStyle.cxx \
	SVTK_KeyFreeInteractorStyle.cxx \
	SVTK_RenderWindowInteractor.cxx \
	SVTK_GenericRenderWindowInteractor.cxx \
	SVTK_SpaceMouse.cxx \
	SVTK_Selector.cxx \
	SVTK_SetRotationPointDlg.cxx \
	SVTK_ViewParameterDlg.cxx \
	SVTK_ComboAction.cxx \
	SVTK_Recorder.cxx \
	SVTK_RecorderDlg.cxx \
	SVTK_ImageWriter.cxx \
	SVTK_ImageWriterMgr.cxx \
	SVTK_Utils.cxx \
	vtkPVAxesActor.h \
	vtkPVAxesActor.cxx \
	vtkPVAxesWidget.h \
	vtkPVAxesWidget.cxx


# internal headers
EXTRA_DIST += SVTK_SelectorDef.h SVTK_Trihedron.h

MOC_FILES= \
	SVTK_GenericRenderWindowInteractor_moc.cxx \
	SVTK_RenderWindowInteractor_moc.cxx \
	SVTK_NonIsometricDlg_moc.cxx \
	SVTK_UpdateRateDlg_moc.cxx \
	SVTK_CubeAxesDlg_moc.cxx \
	SVTK_FontWidget_moc.cxx \
	SVTK_ViewModelBase_moc.cxx \
	SVTK_ViewManager_moc.cxx \
	SVTK_ViewWindow_moc.cxx \
	SVTK_ViewModel_moc.cxx \
	SVTK_View_moc.cxx \
	SVTK_SetRotationPointDlg_moc.cxx \
	SVTK_ViewParameterDlg_moc.cxx \
	SVTK_ComboAction_moc.cxx \
	SVTK_RecorderDlg_moc.cxx
nodist_libSVTK_la_SOURCES= $(MOC_FILES)

dist_salomeres_DATA=\
	resources/vtk_view_graduated_axes.png \
	resources/vtk_view_scaling.png \
	resources/vtk_view_update_rate.png \
	resources/vtk_view_rotation_point.png \
	resources/vtk_view_parallel.png \
	resources/vtk_view_perspective.png \
	resources/vtk_view_parameters.png \
	resources/vtk_view_style_switch.png \
	resources/vtk_view_zooming_style_switch.png \
	resources/vtk_view_recording_start.png \
	resources/vtk_view_recording_play.png \
	resources/vtk_view_recording_pause.png \
	resources/vtk_view_recording_stop.png \
	resources/vtk_view_sync.png \
	resources/vtk_view_highlight.png

nodist_salomeres_DATA =	\
	SVTK_msg_en.qm	\
	SVTK_msg_fr.qm	\
	SVTK_images.qm

libSVTK_la_CPPFLAGS =		\
	$(QT_INCLUDES)		\
	$(CAS_CPPFLAGS)		\
	$(VTK_INCLUDES)		\
	$(BOOST_CPPFLAGS)	\
	-I$(srcdir)/../Qtx	\
	-I$(srcdir)/../SUIT	\
	-I$(srcdir)/../ViewerTools \
	-I$(srcdir)/../OBJECT	\
	-I$(srcdir)/../Prs 	\
	-I$(srcdir)/../VTKViewer \
	-I$(srcdir)/../OpenGLUtils

libSVTK_la_LDFLAGS =			\
	$(VTK_LIBS) $(OGL_LIBS)		\
	$(QT_MT_LIBS)			\
	$(CAS_KERNEL) $(CAS_VIEWER)     \
	$(KERNEL_LDFLAGS)


libSVTK_la_LIBADD = ../Qtx/libqtx.la ../SUIT/libsuit.la ../ViewerTools/libViewerTools.la \
		    ../OBJECT/libSalomeObject.la ../Prs/libSalomePrs.la \
		    ../VTKViewer/libVTKViewer.la ../OpenGLUtils/libOpenGLUtils.la \
		    -lSALOMELocalTrace -lOpUtil

# Executable
bin_PROGRAMS = SVTK

dist_SVTK_SOURCES = SVTK.cxx

SVTK_CPPFLAGS = $(libSVTK_la_CPPFLAGS)
SVTK_LDFLAGS  = $(libSVTK_la_LDFLAGS)
SVTK_LDADD    = libSVTK.la $(libSVTK_la_LIBADD)
