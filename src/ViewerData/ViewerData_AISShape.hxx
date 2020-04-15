// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef VIEWERDATA_AISSHAPE
#define VIEWERDATA_AISSHAPE

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <AIS_ColoredShape.hxx>

class ViewerData_AISShape : public AIS_ColoredShape
{
public:
  Standard_EXPORT ViewerData_AISShape(const TopoDS_Shape&);
  Standard_EXPORT ~ViewerData_AISShape();

  //! Checks if shape is clippable.
  Standard_EXPORT inline bool IsClippable() const
  {
    return myIsClippable;
  }

  //! Makes shape clippable/not clippable.
  Standard_EXPORT inline void SetClippable(bool isClippable)
  {
    myIsClippable = isClippable;
  }

protected:
  bool myIsClippable;

public:
  DEFINE_STANDARD_RTTIEXT(ViewerData_AISShape, AIS_ColoredShape)
};

DEFINE_STANDARD_HANDLE(ViewerData_AISShape, AIS_ColoredShape)

#endif // VIEWERDATA_AISSHAPE
