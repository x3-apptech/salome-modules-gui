// Copyright (C) 2010-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "vtkEDFFactory.h"
#include "vtkVersion.h"
#include "vtkEDFCutter.h"

vtkStandardNewMacro(vtkEDFFactory);

VTK_CREATE_CREATE_FUNCTION(vtkEDFCutter);

vtkEDFFactory::vtkEDFFactory()
{
  this->RegisterOverride("vtkCutter",
                         "vtkEDFCutter",
                         "MergeTriangles",
                         1,
                         vtkObjectFactoryCreatevtkEDFCutter);
}

const char* vtkEDFFactory::GetVTKSourceVersion()
{
  return VTK_SOURCE_VERSION;
}

const char* vtkEDFFactory::GetDescription()
{
  return "VTK EDF Factory";
}

#ifdef WIN32
  //RNV: workaround to avoid definition __declspec( dllimport )
  //for the vtkGetFactoryCompilerUsed(), vtkGetFactoryVersion()
  //and vtkLoad(). This happens because of a bug in the 
  //vtkObjectFactory.h file. 
  //This workaround will be removed in the future as soon as
  //mentioned bug is fixed. 
  #ifdef VTK_FACTORY_INTERFACE_IMPLEMENT
    #undef VTK_FACTORY_INTERFACE_IMPLEMENT

    #define VTK_FACTORY_INTERFACE_IMPLEMENT(factoryName)  \
    extern "C"                                      \
    VTKEDF_OVERLOADS_EXPORT                         \
    const char* vtkGetFactoryCompilerUsed()         \
    {                                               \
      return VTK_CXX_COMPILER;                      \
    }                                               \
    extern "C"                                      \
    VTKEDF_OVERLOADS_EXPORT                         \
    const char* vtkGetFactoryVersion()              \
    {                                               \
      return VTK_SOURCE_VERSION;                    \
    }                                               \
    extern "C"                                      \
    VTKEDF_OVERLOADS_EXPORT                         \
    vtkObjectFactory* vtkLoad()                     \
    {                                               \
      return factoryName ::New();                   \
    }
  #endif
#endif


VTK_FACTORY_INTERFACE_IMPLEMENT(vtkEDFFactory);
