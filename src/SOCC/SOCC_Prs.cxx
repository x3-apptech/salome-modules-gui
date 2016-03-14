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

//  SALOME OCCViewer : build OCC Viewer into Salome desktop
//  File   : SOCC_Prs.cxx
//  Author : Sergey ANIKIN

#include "SOCC_Prs.h"

#include <AIS_InteractiveObject.hxx>

/*!
  Default constructor
*/
SOCC_Prs::SOCC_Prs( const char* entry )
  : SALOME_OCCPrs( entry )
{
  myToActivate = true;
}

/*!
  Standard constructor
*/
SOCC_Prs::SOCC_Prs( const char* entry, const Handle(AIS_InteractiveObject)& obj ) 
  : SALOME_OCCPrs( entry )
{  
  AddObject( obj ); 
}

/*!
  Destructor
*/
SOCC_Prs::~SOCC_Prs()
{ 
  myObjects.Clear(); 
}

/*!
  Get interactive objects list
*/
void SOCC_Prs::GetObjects( AIS_ListOfInteractive& list ) const 
{ 
  list = myObjects; 
}

/*!
  Add interactive object
*/
void SOCC_Prs::AddObject( const Handle(AIS_InteractiveObject)& obj ) 
{ 
  myObjects.Append( obj ); 
}

/*!
  Prepend interactive object
*/
void SOCC_Prs::PrependObject( const Handle(AIS_InteractiveObject)& obj ) 
{ 
  myObjects.Prepend( obj ); 
}

/*!
  Remove first interactive object
*/
void SOCC_Prs::RemoveFirst()
{
  myObjects.RemoveFirst();
}

/*!
  Remove all interactive objects
*/
void SOCC_Prs::Clear()
{
  myObjects.Clear();
}

/*!
  \return 0 if list of the interactive objects is empty [ Reimplemented from SALOME_Prs ]
*/
bool SOCC_Prs::IsNull() const 
{ 
  return myObjects.IsEmpty() == Standard_True;
}

/*!
  This method is used for activisation/deactivisation of
  objects in the moment of displaying
*/   
void SOCC_Prs::SetToActivate( const bool toActivate )
{
  myToActivate = toActivate;
}

bool SOCC_Prs::ToActivate() const
{
  return myToActivate;
}
