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

lib_LTLIBRARIES = libSUPERVGraph.la

salomeinclude_HEADERS =			\
	SUPERVGraph_ViewFrame.h		\
	SUPERVGraph_ViewManager.h	\
	SUPERVGraph_ViewModel.h		\
	SUPERVGraph.h

dist_libSUPERVGraph_la_SOURCES = 	\
	SUPERVGraph.cxx			\
	SUPERVGraph_ViewFrame.cxx	\
	SUPERVGraph_ViewManager.cxx	\
	SUPERVGraph_ViewModel.cxx 

MOC_FILES = 				\
	SUPERVGraph_moc.cxx		\
	SUPERVGraph_ViewFrame_moc.cxx	\
	SUPERVGraph_ViewManager_moc.cxx	\
	SUPERVGraph_ViewModel_moc.cxx 
nodist_libSUPERVGraph_la_SOURCES = $(MOC_FILES)

dist_salomeres_DATA =			\
	resources/superv_view_pan.png	\
	resources/superv_view_reset.png

nodist_salomeres_DATA =		\
	SUPERVGraph_images.qm	\
	SUPERVGraph_msg_en.qm  

libSUPERVGraph_la_CPPFLAGS = $(QT_INCLUDES) $(CAS_CPPFLAGS) $(PYTHON_INCLUDES) $(BOOST_CPPFLAGS) \
	                     -I$(srcdir)/../Qtx -I$(srcdir)/../SUIT -I$(srcdir)/../OBJECT
libSUPERVGraph_la_LDFLAGS  = $(QT_MT_LIBS) ../SUIT/libsuit.la ../OBJECT/libSalomeObject.la
