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
#ifndef CAM_ROOTOBJECT_H
#define CAM_ROOTOBJECT_H

#include "CAM_DataObject.h"

/*!
  CAM_RootObject - class intended for optimized access to CAM_DataModel instance
  from CAM_DataObject instances.

  To take advantage of this class in a specific application, 
  custom data model root object class should be derived from both CAM_RootObject
  and application-specific DataObject implementation using virtual inheritance.
 */
class CAM_EXPORT CAM_RootObject : public virtual CAM_DataObject
{
public:
  CAM_RootObject( SUIT_DataObject* = 0 );
  CAM_RootObject( CAM_DataModel*, SUIT_DataObject* = 0 );
  virtual ~CAM_RootObject();

  virtual QString        name() const;

  virtual CAM_DataModel* dataModel() const;
  virtual void           setDataModel( CAM_DataModel* );

private:
  CAM_DataModel*         myDataModel; 
};

#endif

#if _MSC_VER > 1000
#pragma once
#endif
