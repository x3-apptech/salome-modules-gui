
# VTK detection for Salome (see http://www.vtk.org/Wiki/VTK/Build_System_Migration)

set(PARAVIEW_ROOT_DIR $ENV{PARAVIEW_ROOT_DIR} CACHE PATH "Path to ParaView directory")
set(PARAVIEW_VERSION $ENV{PARAVIEW_VERSION} CACHE STRING "Version of Paraview")
if(EXISTS ${PARAVIEW_ROOT_DIR})
  set(VTK_DIR ${PARAVIEW_ROOT_DIR}/lib/cmake/paraview-${PARAVIEW_VERSION} CACHE PATH "Path to directory including VTKConfig.cmake")
endif(EXISTS ${PARAVIEW_ROOT_DIR})
set(VTK_COMPONENTS vtkRenderingOpenGL vtkRenderingLOD vtkRenderingAnnotation vtkFiltersParallel vtkIOExport)
find_package(VTK 6.0 REQUIRED COMPONENTS ${VTK_COMPONENTS} NO_MODULE)
if (VTK_FOUND)
  message(STATUS "VTK version is ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")
  message(STATUS "VTK libraries are: ${VTK_LIBRARIES}")
endif(VTK_FOUND)
