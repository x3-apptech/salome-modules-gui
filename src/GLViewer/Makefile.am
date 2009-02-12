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
#  Module : caf
#  $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libGLViewer.la

salomeinclude_HEADERS =		\
	GLViewer.h 		\
	GLViewer_AspectLine.h	\
	GLViewer_BaseDrawers.h	\
	GLViewer_BaseObjects.h	\
	GLViewer_Compass.h	\
	GLViewer_Context.h	\
	GLViewer_CoordSystem.h	\
	GLViewer_Defs.h		\
	GLViewer_Drawer.h	\
	GLViewer_Geom.h		\
	GLViewer_Grid.h		\
	GLViewer_Group.h	\
	GLViewer_MimeData.h	\
	GLViewer_Object.h	\
	GLViewer_Selector.h	\
	GLViewer_Selector2d.h	\
	GLViewer_Text.h		\
	GLViewer_Tools.h	\
	GLViewer_ToolTip.h	\
	GLViewer_ViewFrame.h	\
	GLViewer_ViewManager.h	\
	GLViewer_ViewPort.h	\
	GLViewer_ViewPort2d.h	\
	GLViewer_Viewer.h	\
	GLViewer_Viewer2d.h	\
	GLViewer_Widget.h

dist_libGLViewer_la_SOURCES =		\
	GLViewer_AspectLine.cxx		\
	GLViewer_BaseDrawers.cxx	\
	GLViewer_BaseObjects.cxx	\
	GLViewer_Compass.cxx		\
	GLViewer_Context.cxx		\
	GLViewer_CoordSystem.cxx	\
	GLViewer_Drawer.cxx		\
	GLViewer_Geom.cxx		\
	GLViewer_Grid.cxx		\
	GLViewer_Group.cxx		\
	GLViewer_MimeData.cxx		\
	GLViewer_Object.cxx		\
	GLViewer_Selector.cxx		\
	GLViewer_Selector2d.cxx		\
	GLViewer_Text.cxx		\
	GLViewer_Tools.cxx		\
	GLViewer_ToolTip.cxx		\
	GLViewer_ViewFrame.cxx		\
	GLViewer_ViewManager.cxx	\
	GLViewer_ViewPort.cxx		\
	GLViewer_ViewPort2d.cxx		\
	GLViewer_Viewer.cxx		\
	GLViewer_Viewer2d.cxx		\
	GLViewer_Widget.cxx

MOC_FILES =				\
	GLViewer_Selector_moc.cxx	\
	GLViewer_Selector2d_moc.cxx	\
	GLViewer_ToolTip_moc.cxx	\
	GLViewer_ViewFrame_moc.cxx	\
	GLViewer_ViewManager_moc.cxx	\
	GLViewer_ViewPort_moc.cxx	\
	GLViewer_ViewPort2d_moc.cxx	\
	GLViewer_Viewer_moc.cxx		\
	GLViewer_Viewer2d_moc.cxx	\
	GLViewer_Widget_moc.cxx
nodist_libGLViewer_la_SOURCES = $(MOC_FILES)

dist_salomeres_DATA =			\
	resources/gl_cursor_rotate.png	\
	resources/gl_cursor_sketch.png	\
	resources/gl_cursor_zoom.png	\
	resources/gl_view_dump.png	\
	resources/gl_view_fitall.png	\
	resources/gl_view_fitarea.png	\
	resources/gl_view_fitselect.png	\
	resources/gl_view_glpan.png	\
	resources/gl_view_pan.png	\
	resources/gl_view_reset.png	\
	resources/gl_view_zoom.png

nodist_salomeres_DATA =		\
	GLViewer_images.qm	\
	GLViewer_msg_en.qm

libGLViewer_la_CPPFLAGS = $(QT_INCLUDES) $(CAS_CPPFLAGS) -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx
libGLViewer_la_LDFLAGS  = $(QT_MT_LIBS) $(CAS_KERNEL) $(OGL_LIBS)
libGLViewer_la_LIBADD   = ../SUIT/libsuit.la
