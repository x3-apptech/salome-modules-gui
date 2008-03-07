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
#  Module : LogWindow
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libLogWindow.la

salomeinclude_HEADERS= LogWindow.h

dist_libLogWindow_la_SOURCES= LogWindow.cxx		

MOC_FILES= LogWindow_moc.cxx
nodist_libLogWindow_la_SOURCES = $(MOC_FILES)

libLogWindow_la_CPPFLAGS=$(QT_INCLUDES) -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx

libLogWindow_la_LDFLAGS=$(QT_MT_LIBS) ../SUIT/libsuit.la ../Qtx/libqtx.la 


