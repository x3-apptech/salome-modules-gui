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

#include "ViewerData_AISShape.hxx"

#ifdef USE_TEXTURED_SHAPE
  OCCT_IMPLEMENT_STANDARD_RTTIEXT(ViewerData_AISShape, AIS_TexturedShape)
#else
  OCCT_IMPLEMENT_STANDARD_RTTIEXT(ViewerData_AISShape, AIS_Shape)
#endif

/*!
  Constructor
  \param shape - TopoDS shape 
*/
ViewerData_AISShape::ViewerData_AISShape (const TopoDS_Shape& theShape)
#ifdef USE_TEXTURED_SHAPE
: AIS_TexturedShape (theShape),
#else
: AIS_Shape (theShape),
#endif
  myIsClippable (true)
{
}

/*!
  Destructor
*/
ViewerData_AISShape::~ViewerData_AISShape()
{
}

