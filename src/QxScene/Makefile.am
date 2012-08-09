# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

lib_LTLIBRARIES = libQxScene.la

salomeinclude_HEADERS= \
	QxScene_ViewWindow.h \
	QxScene_ViewManager.h \
	QxScene_ViewModel.h \
	QxScene_Def.h \
	QxScene.h

dist_libQxScene_la_SOURCES= \
	QxScene_ViewWindow.cxx \
	QxScene_ViewManager.cxx \
	QxScene_ViewModel.cxx

MOC_FILES= \
	QxScene_ViewWindow_moc.cxx \
	QxScene_ViewManager_moc.cxx \
	QxScene_ViewModel_moc.cxx

nodist_libQxScene_la_SOURCES= $(MOC_FILES)

dist_salomeres_DATA= \
	resources/qx_view_fitall.png \
	resources/qx_view_fitarea.png \
	resources/qx_view_glpan.png \
	resources/qx_view_pan.png \
	resources/qx_view_reset.png \
	resources/qx_view_zoom.png

# --- warnings: 
#     QxSceneViewer prefix must be coherent with type defined in QxScene_ViewModel.h
#                          (for translator loading)
#     coherence also needed in LightApp.xml and SalomeApp.xml

nodist_salomeres_DATA= \
	QxSceneViewer_images.qm \
	QxSceneViewer_msg_en.qm \
	QxSceneViewer_msg_fr.qm

libQxScene_la_CPPFLAGS=$(QT_INCLUDES) $(CAS_CPPFLAGS) $(PYTHON_INCLUDES) $(BOOST_CPPFLAGS) \
	-I$(srcdir)/../Qtx -I$(srcdir)/../SUIT
libQxScene_la_LDFLAGS=$(QT_MT_LIBS) ../SUIT/libsuit.la

# --- warning:
#     this file is used for debug purposes only, to be removed later
EXTRA_DIST += DebTrace.hxx
