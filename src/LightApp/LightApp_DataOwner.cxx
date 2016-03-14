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

#include "LightApp_DataOwner.h"

/*!Constructor. Initialize by \a theEntry.*/
LightApp_DataOwner::LightApp_DataOwner( const QString& theEntry )
: myEntry( theEntry )
{
}

#ifndef DISABLE_SALOMEOBJECT
/*!Constructor. Initialize by \a SALOME_InteractiveObject.*/
LightApp_DataOwner::LightApp_DataOwner( const Handle(SALOME_InteractiveObject)& theIO ):
  myEntry(!theIO.IsNull()? theIO->getEntry(): ""),
  myIO(theIO)
{
}
#endif

/*!Destructor. Do nothing.*/
LightApp_DataOwner::~LightApp_DataOwner()
{
}

/*!Gets key string, used for data owners comparison.*/
QString LightApp_DataOwner::keyString() const
{
  return myEntry;
}

/*!Gets entry.*/
QString LightApp_DataOwner::entry() const
{
  return myEntry;
}

#ifndef DISABLE_SALOMEOBJECT
/*!Gets SALOME_InteractiveObject.*/
const Handle(SALOME_InteractiveObject)& LightApp_DataOwner::IO() const
{
  return myIO;
}
#endif
