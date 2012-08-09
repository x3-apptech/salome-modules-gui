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

IF(${WINDOWS})
  FIND_LIBRARY(OpenGL_LIB OpenGL32 "C:/Program Files/Microsoft SDKs/Windows/v6.0A/lib")
  FIND_LIBRARY(GlU_LIB GlU32 "C:/Program Files/Microsoft SDKs/Windows/v6.0A/lib")
ELSE(${WINDOWS})
  FIND_LIBRARY(OpenGL_LIB GL)
  FIND_LIBRARY(GlU_LIB GLU)
ENDIF(${WINDOWS})
SET(OGL_LIBS ${OpenGL_LIB} ${GlU_LIB})
