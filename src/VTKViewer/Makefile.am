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
#  File   : Makefile.in
#  Author : Vladimir Klyachin (OCN)
#  Module : VTKViewer

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libVTKViewer.la

salomeinclude_HEADERS =				\
	VTKViewer.h				\
	VTKViewer_CellLocationsArray.h		\
	VTKViewer_Actor.h			\
	VTKViewer_ExtractUnstructuredGrid.h	\
	VTKViewer_ConvexTool.h			\
	VTKViewer_Filter.h			\
	VTKViewer_GeometryFilter.h		\
	VTKViewer_AppendFilter.h		\
	VTKViewer_Algorithm.h			\
	VTKViewer_InteractorStyle.h		\
	VTKViewer_RenderWindow.h		\
	VTKViewer_RenderWindowInteractor.h	\
	VTKViewer_ShrinkFilter.h		\
	VTKViewer_TransformFilter.h		\
	VTKViewer_Transform.h 			\
	VTKViewer_Trihedron.h			\
	VTKViewer_Utilities.h			\
	VTKViewer_ViewManager.h			\
	VTKViewer_ViewModel.h			\
	VTKViewer_ViewWindow.h			\
	VTKViewer_Functor.h

dist_libVTKViewer_la_SOURCES =			\
	VTKViewer_CellLocationsArray.cxx	\
	VTKViewer_Actor.cxx			\
	VTKViewer_ExtractUnstructuredGrid.cxx	\
	VTKViewer_Filter.cxx			\
	VTKViewer_GeometryFilter.cxx		\
	VTKViewer_AppendFilter.cxx		\
	VTKViewer_InteractorStyle.cxx		\
	VTKViewer_RenderWindow.cxx		\
	VTKViewer_RenderWindowInteractor.cxx	\
	VTKViewer_ShrinkFilter.cxx		\
	VTKViewer_Transform.cxx			\
	VTKViewer_TransformFilter.cxx		\
	VTKViewer_Trihedron.cxx			\
	VTKViewer_Utilities.cxx			\
	VTKViewer_ViewManager.cxx		\
	VTKViewer_ViewModel.cxx			\
	VTKViewer_ConvexTool.cxx		\
	VTKViewer_ViewWindow.cxx 		

MOC_FILES =						\
	VTKViewer_RenderWindow_moc.cxx			\
	VTKViewer_RenderWindowInteractor_moc.cxx	\
	VTKViewer_ViewManager_moc.cxx			\
	VTKViewer_ViewModel_moc.cxx			\
	VTKViewer_ViewWindow_moc.cxx 
nodist_libVTKViewer_la_SOURCES = $(MOC_FILES)

dist_salomeres_DATA =				\
	resources/vtk_view_back.png		\
	resources/vtk_view_bottom.png		\
	resources/vtk_view_camera_dump.png	\
	resources/vtk_view_fitall.png		\
	resources/vtk_view_fitarea.png		\
	resources/vtk_view_front.png		\
	resources/vtk_view_glpan.png		\
	resources/vtk_view_left.png		\
	resources/vtk_view_pan.png		\
	resources/vtk_view_reset.png		\
	resources/vtk_view_right.png		\
	resources/vtk_view_rotate.png		\
	resources/vtk_view_top.png		\
	resources/vtk_view_triedre.png		\
	resources/vtk_view_zoom.png

nodist_salomeres_DATA =		\
	VTKViewer_images.qm	\
	VTKViewer_msg_en.qm

libVTKViewer_la_CPPFLAGS = $(QT_INCLUDES) $(VTK_INCLUDES) $(CAS_CPPFLAGS)	\
	-I$(srcdir)/../Qtx -I$(srcdir)/../SUIT
libVTKViewer_la_LDFLAGS = $(VTK_LIBS) $(QT_MT_LIBS) $(CAS_KERNEL) $(OGL_LIBS)	\
	../Qtx/libqtx.la ../SUIT/libsuit.la

# Executable

bin_PROGRAMS = VTKViewer

dist_VTKViewer_SOURCES = VTKViewer.cxx

VTKViewer_CPPFLAGS = $(libVTKViewer_la_CPPFLAGS)
VTKViewer_LDADD    = libVTKViewer.la $(libVTKViewer_la_LDFLAGS)

