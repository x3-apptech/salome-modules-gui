// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  SMESHGUI_Filter : Filter for VTK viewer
//  File   : VTKViewer_Filter.h
//  Author : Sergey LITONIN
//  Module : SALOME
//
#ifndef VTKViewer_Filter_HeaderFile
#define VTKViewer_Filter_HeaderFile

#include "VTKViewer.h"

#include <MMgt_TShared.hxx>
#include <Standard_DefineHandle.hxx>

#include <Basics_OCCTVersion.hxx>

class VTKViewer_Actor;

DEFINE_STANDARD_HANDLE(VTKViewer_Filter, MMgt_TShared);

/*
  Class       : VTKViewer_Filter
  Description : Base class of filters of for <a href="http://www.vtk.org/">VTK</a> viewer. Method IsValid 
                should be redefined in derived classes
*/

class VTKViewer_Filter : public MMgt_TShared
{

public:
  VTKVIEWER_EXPORT VTKViewer_Filter();
  VTKVIEWER_EXPORT virtual ~VTKViewer_Filter();

  VTKVIEWER_EXPORT bool                        IsValid( VTKViewer_Actor*, const int theId );
  VTKVIEWER_EXPORT virtual bool                IsValid( const int theId ) const = 0;
  VTKVIEWER_EXPORT virtual int                 GetId() const = 0;
  VTKVIEWER_EXPORT virtual bool                IsNodeFilter() const = 0;

  VTKVIEWER_EXPORT virtual void                SetActor( VTKViewer_Actor* );
  
protected:
  VTKViewer_Actor*                             myActor;

public:  
  OCCT_DEFINE_STANDARD_RTTIEXT(VTKViewer_Filter,MMgt_TShared)
};

#endif
