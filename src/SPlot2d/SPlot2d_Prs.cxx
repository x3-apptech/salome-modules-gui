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
//  File   : SPlot2d_Prs.cxx
//  Author : Sergey RUIN, Open CASCADE S.A.S. (sergey.ruin@opencascade.com)

#include "SPlot2d_Prs.h"

/*!
 Default constructor
*/
SPlot2d_Prs::SPlot2d_Prs( const char* entry )
  : SALOME_Prs2d( entry ), Plot2d_Prs()  
{
}

/*!
 Standard constructor
*/
SPlot2d_Prs::SPlot2d_Prs( const char* entry, const Plot2d_Object* obj )
  : SALOME_Prs2d( entry ), Plot2d_Prs( obj )
{ 
}

/*!
 Standard constructor
*/
SPlot2d_Prs::SPlot2d_Prs( const Plot2d_Prs* prs )
  : SALOME_Prs2d( 0 )
{
  mySecondY = prs->isSecondY();
  myObjects = prs->getObjects();
  const SPlot2d_Prs* sp = dynamic_cast<const SPlot2d_Prs*>( prs );
  if ( sp )
    myEntry = sp->myEntry;
  myIsAutoDel = false;          // VSR: error? should auto-delete flag be removed
}

/*!
 Destructor
*/
SPlot2d_Prs::~SPlot2d_Prs()
{ 
}


/*!
  Check if the presentation is empty
*/
bool SPlot2d_Prs::IsNull() const
{
  return Plot2d_Prs::IsNull();
}

/*!
  Get contained objects
*/
objectList SPlot2d_Prs::GetObjects() const
{
  return getObjects();
}
