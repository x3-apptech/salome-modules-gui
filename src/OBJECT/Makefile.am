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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libSalomeObject.la

salomeinclude_HEADERS =						\
	SALOME_InteractiveObject.hxx				\
	Handle_SALOME_InteractiveObject.hxx			\
	SALOME_AISShape.hxx					\
	Handle_SALOME_AISShape.hxx				\
	SALOME_AISObject.hxx					\
	Handle_SALOME_AISObject.hxx				\
	SALOME_ListIO.hxx					\
	SALOME_ListIteratorOfListIO.hxx				\
	Handle_SALOME_ListNodeOfListIO.hxx			\
	SALOME_ListNodeOfListIO.hxx				\
	Handle_SALOME_Filter.hxx				\
	SALOME_Filter.hxx					\
	Handle_SALOME_TypeFilter.hxx				\
	SALOME_TypeFilter.hxx					\
	SALOME_DataMapOfIOMapOfInteger.hxx			\
	SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger.hxx	\
	SALOME_DataMapNodeOfDataMapOfIOMapOfInteger.hxx		\
	Handle_SALOME_DataMapNodeOfDataMapOfIOMapOfInteger.hxx	\
	SALOME_Selection.h

dist_libSalomeObject_la_SOURCES =				\
	SALOME_InteractiveObject.cxx				\
	SALOME_AISShape.cxx					\
	SALOME_AISObject.cxx					\
	SALOME_ListIO_0.cxx					\
	SALOME_ListIteratorOfListIO_0.cxx			\
	SALOME_ListNodeOfListIO_0.cxx				\
	SALOME_Filter.cxx					\
	SALOME_TypeFilter.cxx					\
	SALOME_DataMapOfIOMapOfInteger_0.cxx			\
	SALOME_DataMapNodeOfDataMapOfIOMapOfInteger_0.cxx	\
	SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger_0.cxx

libSalomeObject_la_CPPFLAGS = $(QT_INCLUDES) $(CAS_CPPFLAGS) @KERNEL_CXXFLAGS@
libSalomeObject_la_LDFLAGS  = $(QT_LIBS) $(CAS_LDPATH) -lTKV3d

# internal (non-distributed) headers
EXTRA_DIST +=				\
	SALOME_AISObject.ixx		\
	SALOME_AISObject.jxx		\
	SALOME_AISShape.ixx		\
	SALOME_AISShape.jxx		\
	SALOME_Filter.ixx		\
	SALOME_Filter.jxx		\
	SALOME_InteractiveObject.ixx	\
	SALOME_InteractiveObject.jxx	\
	SALOME_TypeFilter.ixx		\
	SALOME_TypeFilter.jxx
