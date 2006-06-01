// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "LightApp_DataSubOwner.h"

#include "LightApp_DataObject.h"

#ifdef WNT
#include <typeinfo.h>
#endif

/*!Constructor.Initialize by \a entry and \a index*/
LightApp_DataSubOwner::LightApp_DataSubOwner( const QString& entry, const int index )
: LightApp_DataOwner( entry ),
myIndex( index )
{
}

/*!Destructor. Do nothing.*/
LightApp_DataSubOwner::~LightApp_DataSubOwner()
{
}

/*!Checks: Is current data sub owner equal \a obj.*/
bool LightApp_DataSubOwner::isEqual( const SUIT_DataOwner& obj ) const
{  
  if (LightApp_DataOwner::isEqual(obj)) {
    const LightApp_DataSubOwner* other = dynamic_cast<const LightApp_DataSubOwner*>( &obj );
    return other && index() == other->index();
  }
  return false;
}

/*!Gets index.*/
int LightApp_DataSubOwner::index() const
{
  return myIndex;
}
