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
#  Module : PythonConsole
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libPythonConsole.la

salomeinclude_HEADERS= \
	PythonConsole.h \
	PythonConsole_PyConsole.h \
	PythonConsole_PyEditor.h \
	PythonConsole_PyInterp.h

dist_libPythonConsole_la_SOURCES= \
	PythonConsole_PyConsole.cxx \
	PythonConsole_PyEditor.cxx \
	PythonConsole_PyInterp.cxx		

MOC_FILES= \
	PythonConsole_PyEditor_moc.cxx \
	PythonConsole_PyConsole_moc.cxx
nodist_libPythonConsole_la_SOURCES= $(MOC_FILES)


libPythonConsole_la_CPPFLAGS= $(PYTHON_INCLUDES) $(QT_INCLUDES) \
	-I$(srcdir)/../PyInterp -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx

libPythonConsole_la_LDFLAGS= $(PYTHON_LIBS) $(QT_MT_LIBS)


