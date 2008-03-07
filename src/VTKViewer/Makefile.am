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
#  Author : Vladimir Klyachin (OCN)
#  Module : VTKViewer
# $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libVTKViewer.la

salomeinclude_HEADERS= \
	VTKViewer.h \
	VTKViewer_CellLocationsArray.h \
	VTKViewer_Actor.h \
	VTKViewer_ExtractUnstructuredGrid.h \
	VTKViewer_ConvexTool.h \
	VTKViewer_Filter.h \
	VTKViewer_GeometryFilter.h \
	VTKViewer_AppendFilter.h \
	VTKViewer_Algorithm.h \
	VTKViewer_InteractorStyle.h \
	VTKViewer_RenderWindow.h \
	VTKViewer_RenderWindowInteractor.h \
	VTKViewer_ShrinkFilter.h \
	VTKViewer_TransformFilter.h \
	VTKViewer_Transform.h \
	VTKViewer_Trihedron.h \
	VTKViewer_Utilities.h \
	VTKViewer_ViewManager.h \
	VTKViewer_ViewModel.h \
	VTKViewer_ViewWindow.h \
	VTKViewer_Functor.h

dist_libVTKViewer_la_SOURCES= \
	VTKViewer_CellLocationsArray.cxx \
	VTKViewer_Actor.cxx \
	VTKViewer_ExtractUnstructuredGrid.cxx \
	VTKViewer_Filter.cxx \
	VTKViewer_GeometryFilter.cxx \
	VTKViewer_AppendFilter.cxx \
	VTKViewer_InteractorStyle.cxx \
	VTKViewer_RenderWindow.cxx \
	VTKViewer_RenderWindowInteractor.cxx \
	VTKViewer_ShrinkFilter.cxx \
	VTKViewer_Transform.cxx \
	VTKViewer_TransformFilter.cxx \
	VTKViewer_Trihedron.cxx \
	VTKViewer_Utilities.cxx \
	VTKViewer_ViewManager.cxx \
	VTKViewer_ViewModel.cxx \
	VTKViewer_ConvexTool.cxx \
	VTKViewer_ViewWindow.cxx 		

MOC_FILES= \
	VTKViewer_RenderWindow_moc.cxx \
	VTKViewer_RenderWindowInteractor_moc.cxx \
	VTKViewer_ViewManager_moc.cxx \
	VTKViewer_ViewModel_moc.cxx \
	VTKViewer_ViewWindow_moc.cxx 
nodist_libVTKViewer_la_SOURCES= $(MOC_FILES)

dist_salomeres_DATA=\
	resources/view_back.png \
	resources/view_bottom.png \
	resources/view_camera_dump.png \
	resources/view_fitall.png \
	resources/view_fitarea.png \
	resources/view_front.png \
	resources/view_glpan.png \
	resources/view_graduated_axes.png \
	resources/view_left.png \
	resources/view_pan.png \
	resources/view_reset.png \
	resources/view_right.png \
	resources/view_rotate.png \
	resources/view_scaling.png \
	resources/view_top.png \
	resources/view_triedre.png \
	resources/view_update_rate.png \
	resources/view_zoom.png \
	resources/view_rotation_point.png

nodist_salomeres_DATA= \
	VTKViewer_images.qm \
	VTKViewer_msg_en.qm

libVTKViewer_la_CPPFLAGS=$(QT_INCLUDES) $(VTK_INCLUDES) $(CAS_CPPFLAGS) \
	-I$(srcdir)/../Qtx -I$(srcdir)/../SUIT
libVTKViewer_la_LDFLAGS=$(VTK_LIBS) $(QT_MT_LIBS) $(CAS_KERNEL) \
	../Qtx/libqtx.la ../SUIT/libsuit.la

# Executable

bin_PROGRAMS = VTKViewer

dist_VTKViewer_SOURCES=VTKViewer.cxx

VTKViewer_CPPFLAGS=$(libVTKViewer_la_CPPFLAGS)
VTKViewer_LDADD=libVTKViewer.la $(libVTKViewer_la_LDFLAGS)

