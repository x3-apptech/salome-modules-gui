#  SALOME PyInterp : implementation of base thread-safe Python services
#
#  Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D
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
#
#
#  File   : Makefile.in
#  Module : SALOME

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libPyInterp.la

salomeinclude_HEADERS= \
	PyInterp.h \
	PyInterp_base.h \
	PyInterp_Dispatcher.h

dist_libPyInterp_la_SOURCES= \
	PyInterp_base.cxx \
	PyInterp_Dispatcher.cxx

MOC_FILES= PyInterp_Watcher_moc.cxx
nodist_libPyInterp_la_SOURCES= $(MOC_FILES)

libPyInterp_la_CPPFLAGS= $(PYTHON_INCLUDES) $(QT_INCLUDES)
libPyInterp_la_LDFLAGS= $(PYTHON_LIBS) $(QT_MT_LIBS)
