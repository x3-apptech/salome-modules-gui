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

#ifndef LIGHTAPP_DATAOWNER_H
#define LIGHTAPP_DATAOWNER_H

#include "LightApp.h"
#include "SUIT_DataOwner.h"

#ifndef DISABLE_SALOMEOBJECT
  #include "SALOME_InteractiveObject.hxx"
#endif


/*!
  This class provide data owner objects.
*/
class LIGHTAPP_EXPORT LightApp_DataOwner : public SUIT_DataOwner
{
public:
#ifndef DISABLE_SALOMEOBJECT
    LightApp_DataOwner( const Handle(SALOME_InteractiveObject)& theIO );
#endif
    LightApp_DataOwner( const QString& );
    virtual ~LightApp_DataOwner();

    // *** jfa: The below line has been put here 14.02.2007.
    // *** It cancels modifications from branch BR_Dev_For_4_0
    virtual QString keyString() const;

#ifndef DISABLE_SALOMEOBJECT
    const Handle(SALOME_InteractiveObject)& IO() const;
#endif
    QString entry() const;

private:
    QString  myEntry;
#ifndef DISABLE_SALOMEOBJECT
    Handle(SALOME_InteractiveObject) myIO;
#endif
};

typedef SMART(LightApp_DataOwner) LightApp_DataOwnerPtr;

#endif
