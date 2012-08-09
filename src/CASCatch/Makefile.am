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

# File   : Makefile.am
# Author : Sergey RUIN, Open CASCADE S.A.S (sergey.ruin@opencascade.com)

include $(top_srcdir)/adm_local/unix/make_common_starter.am


salomeinclude_HEADERS = CASCatch.hxx			\
			CASCatch_CatchSignals.hxx	\
			CASCatch_ErrorHandler.hxx	\
			CASCatch_Failure.hxx

lib_LTLIBRARIES         = libCASCatch.la
libCASCatch_la_SOURCES  =				\
			CASCatch_CatchSignals.cxx	\
			CASCatch_ErrorHandler.cxx	\
			CASCatch_Failure.cxx

COMMON_CPPFLAGS = \
	@CAS_CPPFLAGS@ @CAS_CXXFLAGS@

libCASCatch_la_CPPFLAGS = $(COMMON_CPPFLAGS)
libCASCatch_la_LIBADD   = $(CAS_KERNEL)
