// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : SVTK_Prs.cxx
//  Author : Sergey ANIKIN

#include "SVTK_Prs.h"

#include <vtkActorCollection.h>

/*!
  Default constructor
*/
SVTK_Prs::SVTK_Prs() : myObjects( 0 )
{
}
 
/*!
  Standard constructor
*/
SVTK_Prs::SVTK_Prs( const vtkActor* obj ) 
{ 
  AddObject( obj ); 
}

/*!
  Destructor
*/
SVTK_Prs:: ~SVTK_Prs() 
{ 
  if ( myObjects ) myObjects->Delete(); 
}

/*!
  \return actors list
*/
vtkActorCollection* SVTK_Prs::GetObjects() const 
{ 
  return myObjects; 
}

/*!
  Add actor
*/
void SVTK_Prs::AddObject( const vtkActor* obj ) 
{ 
  if ( !myObjects) 
    myObjects = vtkActorCollection::New(); 
  myObjects->AddItem( (vtkActor*)obj ); 
}
  
/*!
  \return 0 if list of the actors is empty [ Reimplemented from SALOME_Prs ]
*/
bool SVTK_Prs::IsNull() const 
{ 
  return !myObjects || myObjects->GetNumberOfItems() <= 0; 
}
