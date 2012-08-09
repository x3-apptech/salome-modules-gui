# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#  File   : Makefile.in
#  Author : Vladimir Klyachin (OCN)
#  Module : suitApp
#  $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libSUITApp.la

salomeinclude_HEADERS =		\
	SUITApp_Application.h

dist_libSUITApp_la_SOURCES =	\
	SUITApp.cxx		\
	SUITApp_Application.cxx

if ENABLE_PYCONSOLE
  salomeinclude_HEADERS += SUITApp_init_python.hxx
  dist_libSUITApp_la_SOURCES += SUITApp_init_python.cxx 
endif

MOC_FILES =				\
	SUITApp_Application_moc.cxx

nodist_libSUITApp_la_SOURCES = $(MOC_FILES)

nodist_salomeres_DATA = SUITApp_msg_en.qm SUITApp_msg_fr.qm

libSUITApp_la_CPPFLAGS = $(QT_INCLUDES) $(PYTHON_INCLUDES) \
                         -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx -I$(srcdir)/../Style

if ENABLE_TESTRECORDER
  libSUITApp_la_CPPFLAGS+= -DENABLE_TESTRECORDER $(TESTRECORDER_INCLUDES)
  MOC_FLAGS = -DENABLE_TESTRECORDER
endif

libSUITApp_la_LDFLAGS  = $(QT_MT_LIBS) 

if ENABLE_TESTRECORDER
  libSUITApp_la_LDFLAGS+= $(TESTRECORDER_LIBS)
endif

libSUITApp_la_LIBADD   = ../Qtx/libqtx.la ../SUIT/libsuit.la ../Style/libSalomeStyle.la

if ENABLE_PYCONSOLE
  libSUITApp_la_LDFLAGS += -Xlinker -export-dynamic $(PYTHON_LIBS)
endif

# Executable
bin_PROGRAMS = SUITApp

dist_SUITApp_SOURCES = SUITApp.cxx

SUITApp_CPPFLAGS = $(QT_INCLUDES) $(PYTHON_INCLUDES) \
                   -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx -I$(srcdir)/../Style

if ENABLE_TESTRECORDER
  SUITApp_CPPFLAGS+= -DENABLE_TESTRECORDER $(TESTRECORDER_INCLUDES)
endif

SUITApp_LDADD    = libSUITApp.la  ../Qtx/libqtx.la ../SUIT/libsuit.la ../Style/libSalomeStyle.la ${QT_LIBS}
if ENABLE_PYCONSOLE
  SUITApp_LDADD += $(PYTHON_LIBS)
endif

if ENABLE_TESTRECORDER
  SUITApp_LDADD+= $(TESTRECORDER_LIBS)
endif
