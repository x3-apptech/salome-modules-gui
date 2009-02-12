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

INCLUDE(FindVTK)

SET(VTK_INCLUDES)
FOREACH(dir ${VTK_INCLUDE_DIRS})
  SET(VTK_INCLUDES ${VTK_INCLUDES} -I${dir})
ENDFOREACH(dir ${VTK_INCLUDE_DIRS})

SET(VTK_LIBS)
FIND_LIBRARY(VTK_COMMON vtkCommon ${VTK_LIBRARY_DIRS})
SET(VTK_LIBS ${VTK_LIBS} ${VTK_COMMON})
FIND_LIBRARY(VTK_GRAPHICS vtkGraphics ${VTK_LIBRARY_DIRS})
SET(VTK_LIBS ${VTK_LIBS} ${VTK_GRAPHICS})
FIND_LIBRARY(VTK_IMAGING vtkImaging ${VTK_LIBRARY_DIRS})
SET(VTK_LIBS ${VTK_LIBS} ${VTK_IMAGING})
FIND_LIBRARY(VTK_FILTERING vtkFiltering ${VTK_LIBRARY_DIRS})
SET(VTK_LIBS ${VTK_LIBS} ${VTK_FILTERING})
FIND_LIBRARY(VTK_IO vtkIO ${VTK_LIBRARY_DIRS})
SET(VTK_LIBS ${VTK_LIBS} ${VTK_IO})
FIND_LIBRARY(VTK_RENDERING vtkRendering ${VTK_LIBRARY_DIRS})
SET(VTK_LIBS ${VTK_LIBS} ${VTK_RENDERING})
FIND_LIBRARY(VTK_HYBRID vtkHybrid ${VTK_LIBRARY_DIRS})
SET(VTK_LIBS ${VTK_LIBS} ${VTK_HYBRID})
FIND_LIBRARY(VTK_PARALLEL vtkParallel ${VTK_LIBRARY_DIRS})
SET(VTK_LIBS ${VTK_LIBS} ${VTK_PARALLEL})
FIND_LIBRARY(VTK_WIDGETS vtkWidgets ${VTK_LIBRARY_DIRS})
SET(VTK_LIBS ${VTK_LIBS} ${VTK_WIDGETS})
