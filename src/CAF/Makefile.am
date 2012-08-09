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
#  Module : caf
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libcaf.la

salomeinclude_HEADERS =		\
	CAF.h			\
	CAF_Application.h	\
	CAF_Operation.h		\
	CAF_Study.h		\
	CAF_Tools.h

dist_libcaf_la_SOURCES =	\
	CAF_Application.cxx	\
	CAF_Operation.cxx	\
	CAF_Study.cxx		\
	CAF_Tools.cxx 		

MOC_FILES =			\
	CAF_Application_moc.cxx	\
	CAF_Operation_moc.cxx	\
	CAF_Study_moc.cxx

nodist_libcaf_la_SOURCES = $(MOC_FILES)

nodist_salomeres_DATA = \
	CAF_msg_en.qm \
	CAF_msg_fr.qm

libcaf_la_CPPFLAGS = $(QT_INCLUDES) $(CAS_CPPFLAGS) @KERNEL_CXXFLAGS@	\
	-I$(srcdir)/../STD -I$(srcdir)/../SUIT -I$(srcdir)/../Qtx
libcaf_la_LDFLAGS  = $(QT_MT_LIBS) $(CAS_OCAF) $(CAS_OCAFVIS)
libcaf_la_LIBADD   = ../Qtx/libqtx.la ../SUIT/libsuit.la ../STD/libstd.la
