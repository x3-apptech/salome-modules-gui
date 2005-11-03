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
