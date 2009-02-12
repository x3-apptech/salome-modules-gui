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
#  Module : CAM
#  $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libCAM.la

salomeinclude_HEADERS =		\
	CAM.h			\
	CAM_Application.h	\
	CAM_DataModel.h		\
	CAM_DataObject.h	\
	CAM_Module.h		\
	CAM_Study.h

dist_libCAM_la_SOURCES =	\
	CAM_Application.cxx	\
	CAM_DataModel.cxx	\
	CAM_DataObject.cxx	\
	CAM_Module.cxx		\
	CAM_Study.cxx

MOC_FILES =			\
	CAM_Application_moc.cxx	\
	CAM_Module_moc.cxx	\
	CAM_Study_moc.cxx	\
	CAM_DataModel_moc.cxx
nodist_libCAM_la_SOURCES = $(MOC_FILES)

nodist_salomeres_DATA = CAM_msg_en.qm

libCAM_la_CPPFLAGS = $(QT_INCLUDES) -I$(srcdir)/../SUIT -I$(srcdir)/../STD -I$(srcdir)/../Qtx
libCAM_la_LDFLAGS  = $(QT_MT_LIBS)
libCAM_la_LIBADD   = ../Qtx/libqtx.la ../SUIT/libsuit.la ../STD/libstd.la
