#ifndef LIGHTAPP_OBFILTER_H
#define LIGHTAPP_OBFILTER_H

#include "LightApp.h"
#include "OB_Filter.h"

class LightApp_SelectionMgr;

class LIGHTAPP_EXPORT LightApp_OBFilter: public OB_Filter
{
public:
  LightApp_OBFilter( LightApp_SelectionMgr* theSelMgr );
  ~LightApp_OBFilter();

  virtual bool isOk(  const SUIT_DataObject* ) const;

private:
  LightApp_SelectionMgr* mySelMgr;

};

#endif
