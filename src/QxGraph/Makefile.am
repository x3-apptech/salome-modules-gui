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
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libQxGraph.la

salomeinclude_HEADERS =		\
	QxGraph_ActiveItem.h	\
	QxGraph_Prs.h		\
	QxGraph_Canvas.h	\
	QxGraph_CanvasView.h	\
	QxGraph_ViewWindow.h	\
	QxGraph_ViewManager.h	\
	QxGraph_ViewModel.h	\
	QxGraph_Def.h		\
	QxGraph.h

dist_libQxGraph_la_SOURCES =	\
	QxGraph_Prs.cxx		\
	QxGraph_Canvas.cxx	\
	QxGraph_CanvasView.cxx	\
	QxGraph_ViewWindow.cxx	\
	QxGraph_ViewManager.cxx	\
	QxGraph_ViewModel.cxx

MOC_FILES =				\
	QxGraph_Canvas_moc.cxx		\
	QxGraph_CanvasView_moc.cxx	\
	QxGraph_ViewWindow_moc.cxx	\
	QxGraph_ViewManager_moc.cxx	\
	QxGraph_ViewModel_moc.cxx

nodist_libQxGraph_la_SOURCES = $(MOC_FILES)

dist_salomeres_DATA =			\
	resources/qx_view_fitall.png	\
	resources/qx_view_fitarea.png	\
	resources/qx_view_glpan.png	\
	resources/qx_view_pan.png	\
	resources/qx_view_reset.png	\
	resources/qx_view_zoom.png

nodist_salomeres_DATA =		\
	QxGraph_images.qm	\
	QxGraph_msg_en.qm  

libQxGraph_la_CPPFLAGS = $(QT_INCLUDES) $(CAS_CPPFLAGS) $(PYTHON_INCLUDES) $(BOOST_CPPFLAGS) \
	                 -I$(srcdir)/../Qtx -I$(srcdir)/../SUIT
libQxGraph_la_LDFLAGS  = $(QT_MT_LIBS) ../SUIT/libsuit.la
