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
// See http://www.salome-platform.org/
//

#ifndef LIGHTAPP_DATAOWNER_H
#define LIGHTAPP_DATAOWNER_H

#include "LightApp.h"
#include "SUIT_DataOwner.h"
#include "SALOME_InteractiveObject.hxx"

/*!
  This class provide data owner objects.
*/
class LIGHTAPP_EXPORT LightApp_DataOwner : public SUIT_DataOwner
{
public:
    LightApp_DataOwner( const Handle(SALOME_InteractiveObject)& theIO );
    LightApp_DataOwner( const QString& );
    virtual ~LightApp_DataOwner();

    virtual bool isEqual( const SUIT_DataOwner& ) const;
    const Handle(SALOME_InteractiveObject)& IO() const;
    QString entry() const;

private:
    QString  myEntry;
    Handle(SALOME_InteractiveObject) myIO;
};

typedef SMART(LightApp_DataOwner) LightApp_DataOwnerPtr;

#endif
