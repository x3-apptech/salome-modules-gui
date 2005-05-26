#ifndef CAM_DATAOBJECT_H
#define CAM_DATAOBJECT_H

#include "CAM.h"

#include <SUIT_DataObject.h>

class CAM_Module;
class CAM_DataModel;

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
