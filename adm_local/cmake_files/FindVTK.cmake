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

INCLUDE(FindVTK)

SET(VTK_INCLUDES)
FOREACH(dir ${VTK_INCLUDE_DIRS})
  SET(VTK_INCLUDES ${VTK_INCLUDES} -I${dir})
ENDFOREACH(dir ${VTK_INCLUDE_DIRS})

SET(VTK_LIBS)
FIND_LIBRARY(VTK_COMMON vtkCommon PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_COMMON})
FIND_LIBRARY(VTK_GRAPHICS vtkGraphics PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_GRAPHICS})
FIND_LIBRARY(VTK_IMAGING vtkImaging PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_IMAGING})
FIND_LIBRARY(VTK_FILTERING vtkFiltering PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_FILTERING})
FIND_LIBRARY(VTK_IO vtkIO PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_IO})
FIND_LIBRARY(VTK_RENDERING vtkRendering PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_RENDERING})
FIND_LIBRARY(VTK_HYBRID vtkHybrid PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_HYBRID})
FIND_LIBRARY(VTK_PARALLEL vtkParallel PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_PARALLEL})
FIND_LIBRARY(VTK_WIDGETS vtkWidgets PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_WIDGETS})
FIND_LIBRARY(VTK_GENERIC_FILTERING vtkGenericFiltering PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_GENERIC_FILTERING})
FIND_LIBRARY(VTK_INFOVIS vtkInfovis PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_INFOVIS})
FIND_LIBRARY(VTK_VOLUME_RENDERING vtkVolumeRendering PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
SET(VTK_LIBS ${VTK_LIBS} ${VTK_VOLUME_RENDERING})

FIND_LIBRARY(vtkCommonPythonD vtkCommonPythonD PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkGraphicsPythonD vtkGraphicsPythonD PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkImagingPythonD vtkImagingPythonD PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)
FIND_LIBRARY(vtkPythonCore vtkPythonCore PATHS ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH)


#IF(NOT WINDOWS)
#  IF(VTK_MAJOR_VERSION STREQUAL 5)
#    IF(VTK_MINOR_VERSION STREQUAL 0)
#      SET(VTK_INCLUDES ${VTK_INCLUDES} -Wno-deprecated)
#    ENDIF(VTK_MINOR_VERSION STREQUAL 0)
#  ENDIF(VTK_MAJOR_VERSION STREQUAL 5)
#ENDIF(NOT WINDOWS)

SET(VTK_INCLUDES ${VTK_INCLUDES} -DVTK_EXCLUDE_STRSTREAM_HEADERS)
SET(VTK_DEFINITIONS -DVTK_EXCLUDE_STRSTREAM_HEADERS)