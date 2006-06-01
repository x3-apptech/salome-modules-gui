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
#ifndef CAM_DATAOBJECT_H
#define CAM_DATAOBJECT_H

#include "CAM.h"

#include <SUIT_DataObject.h>

class CAM_Module;
class CAM_DataModel;

/*!
  \class CAM_DataObject
  Provides only additional link to CAM_DataModel
*/
class CAM_EXPORT CAM_DataObject : public SUIT_DataObject
{
public:
  CAM_DataObject( SUIT_DataObject* = 0 );
  virtual ~CAM_DataObject();

  CAM_Module*            module() const;
  virtual CAM_DataModel* dataModel() const;
};

#endif

#if _MSC_VER > 1000
#pragma once
#endif
