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

#ifndef OCCVIEWER_VSERVICE_H
#define OCCVIEWER_VSERVICE_H

#include <Aspect_Window.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_ExtString.hxx>
#include <Quantity_Length.hxx>
#include <V3d_TypeOfOrientation.hxx>

#include <QWidget>

class Standard_EXPORT OCCViewer_VService
{
public:

  static Handle(Aspect_Window) CreateWindow( const Handle(V3d_View)&, WId );

  static Handle(V3d_Viewer)    CreateViewer( const Standard_ExtString,
					     const Standard_CString = "",
					     const Standard_CString = "",
					     const Quantity_Length = 1000.0,
					     const V3d_TypeOfOrientation = V3d_XposYnegZpos,
					     const Standard_Boolean = Standard_True,
					     const Standard_Boolean = Standard_True );
};

#endif
