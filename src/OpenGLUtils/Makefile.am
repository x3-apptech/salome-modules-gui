# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#  File   : Makefile.am
#  Author : Oleg UVAROV
#  Module : SALOME
#  $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libOpenGLUtils.la

salomeinclude_HEADERS =	\
	OpenGLUtils.h \
	OpenGLUtils_FrameBuffer.h

dist_libOpenGLUtils_la_SOURCES = \
	OpenGLUtils_FrameBuffer.cxx

libOpenGLUtils_la_CPPFLAGS = $(OGL_INCLUDES)
libOpenGLUtils_la_LDFLAGS  = $(KERNEL_LDFLAGS)

libOpenGLUtils_la_LIBADD = $(OGL_LIBS) -lSALOMELocalTrace