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
#  Module : suitApp
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libSUITApp.la

salomeinclude_HEADERS= SUITApp_Application.h

dist_libSUITApp_la_SOURCES= \
	SUITApp.cxx \
	SUITApp_Application.cxx 

MOC_FILES=SUITApp_Application_moc.cxx
nodist_libSUITApp_la_SOURCES= $(MOC_FILES)

nodist_salomeres_DATA= SUITApp_msg_en.qm

libSUITApp_la_CPPFLAGS=$(QT_INCLUDES) $(PYTHON_INCLUDES) \
                       -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx

libSUITApp_la_LDFLAGS=$(QT_MT_LIBS) 
libSUITApp_la_LIBADD= ../SUIT/libsuit.la ../Qtx/libqtx.la

if ENABLE_PYCONSOLE
libSUITApp_la_LDFLAGS+= -Xlinker -export-dynamic $(PYTHON_LIBS)
endif

# Executable
bin_PROGRAMS= SUITApp

dist_SUITApp_SOURCES=SUITApp.cxx

SUITApp_CPPFLAGS=$(QT_INCLUDES) $(PYTHON_INCLUDES) \
                 -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx

SUITApp_LDADD= libSUITApp.la ../SUIT/libsuit.la ../Qtx/libqtx.la
