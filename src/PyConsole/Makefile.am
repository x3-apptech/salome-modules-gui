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
#  Module : PyConsole
#  $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libPyConsole.la

salomeinclude_HEADERS =		\
	PyConsole.h		\
	PyConsole_Console.h	\
	PyConsole_Editor.h	\
	PyConsole_Interp.h

dist_libPyConsole_la_SOURCES =	\
	PyConsole_Console.cxx	\
	PyConsole_Editor.cxx	\
	PyConsole_Interp.cxx		

MOC_FILES =				\
	PyConsole_Editor_moc.cxx	\
	PyConsole_Console_moc.cxx

nodist_libPyConsole_la_SOURCES = $(MOC_FILES)

nodist_salomeres_DATA = \
	PyConsole_msg_en.qm

libPyConsole_la_CPPFLAGS = $(PYTHON_INCLUDES) $(QT_INCLUDES) \
	-I$(srcdir)/../PyInterp -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx

libPyConsole_la_LDFLAGS  = $(PYTHON_LIBS) $(QT_MT_LIBS)
libPyConsole_la_LIBADD   = ../Qtx/libqtx.la ../SUIT/libsuit.la ../PyInterp/libPyInterp.la
