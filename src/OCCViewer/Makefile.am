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
#  Module : OCCViewer
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libOCCViewer.la

salomeinclude_HEADERS= \
	OCCViewer_AISSelector.h \
	OCCViewer_ViewManager.h \
	OCCViewer_ViewModel.h \
	OCCViewer_ViewPort3d.h \
	OCCViewer_ViewPort.h \
	OCCViewer_ViewSketcher.h \
	OCCViewer_ViewWindow.h \
	OCCViewer_VService.h \
	OCCViewer_CreateRestoreViewDlg.h \
	OCCViewer.h \
	OCCViewer_ClippingDlg.h \
	OCCViewer_SetRotationPointDlg.h

dist_libOCCViewer_la_SOURCES= \
	OCCViewer_AISSelector.cxx \
	OCCViewer_ViewManager.cxx \
	OCCViewer_ViewModel.cxx \
	OCCViewer_ViewPort3d.cxx \
	OCCViewer_ViewPort.cxx \
	OCCViewer_ViewSketcher.cxx \
	OCCViewer_ViewWindow.cxx \
	OCCViewer_VService.cxx \
	OCCViewer_CreateRestoreViewDlg.cxx \
	OCCViewer_SetRotationPointDlg.cxx \
	OCCViewer_ClippingDlg.cxx

MOC_FILES= \
	OCCViewer_AISSelector_moc.cxx \
	OCCViewer_ViewModel_moc.cxx \
	OCCViewer_ViewPort3d_moc.cxx \
	OCCViewer_ViewPort_moc.cxx \
	OCCViewer_ViewSketcher_moc.cxx \
	OCCViewer_ViewWindow_moc.cxx \
	OCCViewer_ViewManager_moc.cxx \
	OCCViewer_CreateRestoreViewDlg_moc.cxx \
	OCCViewer_SetRotationPointDlg_moc.cxx \
	OCCViewer_ClippingDlg_moc.cxx
nodist_libOCCViewer_la_SOURCES= $(MOC_FILES)

dist_salomeres_DATA = \
	resources/view_back.png \
	resources/view_bottom.png \
	resources/view_camera_dump.png \
	resources/view_clone.png \
	resources/view_clipping.png \
	resources/view_clipping_pressed.png \
	resources/view_fitall.png \
	resources/view_fitarea.png \
	resources/view_front.png \
	resources/view_glpan.png \
	resources/view_left.png \
	resources/view_pan.png \
	resources/view_presets.png \
	resources/view_reset.png \
	resources/view_right.png \
	resources/view_rotate.png \
	resources/view_shoot.png \
	resources/view_top.png \
	resources/view_triedre.png \
	resources/view_zoom.png \
	resources/view_rotation_point.png

nodist_salomeres_DATA = \
	OCCViewer_images.qm \
	OCCViewer_msg_en.qm 

libOCCViewer_la_CPPFLAGS=$(QT_INCLUDES) $(OGL_INCLUDES) $(CAS_CPPFLAGS) \
	-I$(srcdir)/../SUIT -I$(srcdir)/../Qtx

libOCCViewer_la_LDFLAGS=$(OGL_LIBS) $(QT_MT_LIBS) $(CAS_KERNEL) $(CAS_VIEWER) ../SUIT/libsuit.la
