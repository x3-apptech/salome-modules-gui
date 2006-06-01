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
#ifndef LIGHTAPP_DATAOBJECT_H
#define LIGHTAPP_DATAOBJECT_H

#include "LightApp.h"

#include "CAM_DataObject.h"
#include "CAM_DataModel.h"
#include "CAM_RootObject.h"

class LightApp_Study;

/*!Description : Data Object has empty entry so it's children must redefine metod entry() and return some unique string*/
// to do : decomment virtual inheritance 
class LIGHTAPP_EXPORT LightApp_DataObject : public virtual CAM_DataObject
{
  class Key;

public:
  enum { CT_Value, CT_Entry, CT_IOR, CT_RefEntry };

public:
  LightApp_DataObject( SUIT_DataObject* = 0 );
  virtual ~LightApp_DataObject();

  virtual SUIT_DataObjectKey*     key() const;
  virtual QString                 entry() const;

  virtual SUIT_DataObject*        componentObject() const;
  virtual QString                 componentDataType() const;

protected:
  QString                         myCompDataType;
  SUIT_DataObject*                myCompObject;
};

/*!
 * LightApp_ModuleObject - class for optimized access to DataModel from
 * CAM_RootObject.h.
 * In modules which will be redefine LightApp_DataObject, LightApp_ModuleObject must be children from rederined DataObject for having necessary properties and children from LightApp_ModuleObject.
 */

class LIGHTAPP_EXPORT LightApp_ModuleObject : public CAM_RootObject
{
public:
  LightApp_ModuleObject( SUIT_DataObject* = 0 );
  LightApp_ModuleObject ( CAM_DataModel*, SUIT_DataObject* = 0 );

  virtual ~LightApp_ModuleObject();

  virtual QString        name() const;
  virtual void           insertChild( SUIT_DataObject*, int thePosition );
};

#endif
