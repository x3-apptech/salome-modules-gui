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

lib_LTLIBRARIES = libSPlot2d.la

salomeinclude_HEADERS =		\
	SPlot2d.h		\
	SPlot2d_Curve.h		\
	SPlot2d_Prs.h		\
	SPlot2d_ViewModel.h	\
	SPlot2d_ViewWindow.h

dist_libSPlot2d_la_SOURCES =	\
	SPlot2d_Curve.cxx	\
	SPlot2d_Prs.cxx		\
	SPlot2d_ViewModel.cxx	\
	SPlot2d_ViewWindow.cxx

MOC_FILES =				\
	SPlot2d_ViewModel_moc.cxx	\
	SPlot2d_ViewWindow_moc.cxx
nodist_libSPlot2d_la_SOURCES = $(MOC_FILES)

libSPlot2d_la_CPPFLAGS = $(QT_INCLUDES) $(CAS_CPPFLAGS) $(QWT_INCLUDES) $(BOOST_CPPFLAGS)	\
	                 -I$(srcdir)/../Qtx -I$(srcdir)/../SUIT -I$(srcdir)/../Plot2d		\
	                 -I$(srcdir)/../Prs -I$(srcdir)/../OBJECT
libSPlot2d_la_LDFLAGS  = $(QT_MT_LIBS) $(CAS_KERNEL) $(QWT_LIBS) ../SUIT/libsuit.la ../Plot2d/libPlot2d.la ../Prs/libSalomePrs.la ../OBJECT/libSalomeObject.la





