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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libOCCViewer.la

salomeinclude_HEADERS =				\
	OCCViewer_AISSelector.h			\
	OCCViewer_ViewManager.h			\
	OCCViewer_ViewModel.h			\
	OCCViewer_ViewPort3d.h			\
	OCCViewer_ViewPort.h			\
	OCCViewer_ViewSketcher.h		\
	OCCViewer_ViewWindow.h			\
	OCCViewer_VService.h			\
	OCCViewer_CreateRestoreViewDlg.h	\
	OCCViewer.h				\
	OCCViewer_ClippingDlg.h			\
	OCCViewer_AxialScaleDlg.h		\
	OCCViewer_SetRotationPointDlg.h		\
	OCCViewer_Trihedron.h                   \
	OCCViewer_FontWidget.h                  \
	OCCViewer_CubeAxesDlg.h                 \
	OCCViewer_ToolTip.h			\
	OCCViewer_ViewFrame.h

dist_libOCCViewer_la_SOURCES =			\
	OCCViewer_AISSelector.cxx		\
	OCCViewer_ViewManager.cxx		\
	OCCViewer_ViewModel.cxx			\
	OCCViewer_ViewPort3d.cxx		\
	OCCViewer_ViewPort.cxx			\
	OCCViewer_ViewSketcher.cxx		\
	OCCViewer_ViewWindow.cxx		\
	OCCViewer_VService.cxx			\
	OCCViewer_CreateRestoreViewDlg.cxx	\
	OCCViewer_SetRotationPointDlg.cxx	\
	OCCViewer_ClippingDlg.cxx		\
	OCCViewer_AxialScaleDlg.cxx		\
	OCCViewer_Trihedron.cxx			\
	OCCViewer_FontWidget.cxx                \
	OCCViewer_CubeAxesDlg.cxx               \
	OCCViewer_ToolTip.cxx			\
	OCCViewer_ViewFrame.cxx

MOC_FILES =					\
	OCCViewer_AISSelector_moc.cxx		\
	OCCViewer_ViewModel_moc.cxx		\
	OCCViewer_ViewPort3d_moc.cxx		\
	OCCViewer_ViewPort_moc.cxx		\
	OCCViewer_ViewSketcher_moc.cxx		\
	OCCViewer_ViewWindow_moc.cxx		\
	OCCViewer_ViewManager_moc.cxx		\
	OCCViewer_CreateRestoreViewDlg_moc.cxx	\
	OCCViewer_SetRotationPointDlg_moc.cxx	\
	OCCViewer_ClippingDlg_moc.cxx		\
	OCCViewer_AxialScaleDlg_moc.cxx		\
	OCCViewer_FontWidget_moc.cxx            \
	OCCViewer_CubeAxesDlg_moc.cxx           \
	OCCViewer_ToolTip_moc.cxx		\
	OCCViewer_ViewFrame_moc.cxx

nodist_libOCCViewer_la_SOURCES = $(MOC_FILES)

dist_salomeres_DATA =				\
	resources/occ_view_anticlockwise.png	\
	resources/occ_view_clockwise.png	\
	resources/occ_view_back.png		\
	resources/occ_view_bottom.png		\
	resources/occ_view_camera_dump.png	\
	resources/occ_view_clone.png		\
	resources/occ_view_clipping.png		\
	resources/occ_view_clipping_pressed.png	\
	resources/occ_view_scaling.png		\
	resources/occ_view_graduated_axes.png	\
	resources/occ_view_ambient.png		\
	resources/occ_view_fitall.png		\
	resources/occ_view_fitarea.png		\
	resources/occ_view_front.png		\
	resources/occ_view_glpan.png		\
	resources/occ_view_left.png		\
	resources/occ_view_pan.png		\
	resources/occ_view_presets.png		\
	resources/occ_view_reset.png		\
	resources/occ_view_right.png		\
	resources/occ_view_rotate.png		\
	resources/occ_view_shoot.png		\
	resources/occ_view_top.png		\
	resources/occ_view_triedre.png		\
	resources/occ_view_zoom.png		\
	resources/occ_view_rotation_point.png	\
	resources/occ_view_style_switch.png	\
	resources/occ_view_zooming_style_switch.png \
	resources/occ_view_maximized.png	\
	resources/occ_view_minimized.png

nodist_salomeres_DATA =		\
	OCCViewer_images.qm	\
	OCCViewer_msg_en.qm 	\
	OCCViewer_msg_fr.qm 

libOCCViewer_la_CPPFLAGS = $(QT_INCLUDES) $(OGL_INCLUDES) $(CAS_CPPFLAGS) @KERNEL_CXXFLAGS@ \
	-I$(srcdir)/../SUIT -I$(srcdir)/../ViewerTools -I$(srcdir)/../Qtx \
	-I$(srcdir)/../OpenGLUtils -I$(srcdir)/../CASCatch

libOCCViewer_la_LDFLAGS  = $(OGL_LIBS) $(QT_MT_LIBS) $(CAS_KERNEL) $(CAS_VIEWER)

libOCCViewer_la_LIBADD   = ../Qtx/libqtx.la ../SUIT/libsuit.la \
	../ViewerTools/libViewerTools.la ../OpenGLUtils/libOpenGLUtils.la
