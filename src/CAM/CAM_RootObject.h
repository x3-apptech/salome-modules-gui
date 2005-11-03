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
