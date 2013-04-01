# Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

# VTK detection for Salome (see http://www.vtk.org/Wiki/VTK/Build_System_Migration)

set(PARAVIEW_ROOT_DIR $ENV{PARAVIEW_ROOT_DIR} CACHE PATH "Path to ParaView directory")
set(PARAVIEW_VERSION $ENV{PARAVIEW_VERSION} CACHE STRING "Version of Paraview")
if(EXISTS ${PARAVIEW_ROOT_DIR})
  set(VTK_DIR ${PARAVIEW_ROOT_DIR}/lib/cmake/paraview-${PARAVIEW_VERSION} CACHE PATH "Path to directory including VTKConfig.cmake")
endif(EXISTS ${PARAVIEW_ROOT_DIR})
set(VTK_COMPONENTS vtkRenderingFreeTypeOpenGL vtkRenderingLOD vtkRenderingAnnotation vtkFiltersParallel vtkIOExport)
find_package(VTK 6.0 REQUIRED COMPONENTS ${VTK_COMPONENTS} NO_MODULE)
if (VTK_FOUND)
  message(STATUS "VTK version is ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")
  message(STATUS "VTK libraries are: ${VTK_LIBRARIES}")
endif(VTK_FOUND)
