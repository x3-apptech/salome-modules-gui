#ifndef SALOMEAPP_OBFILTER_H
#define SALOMEAPP_OBFILTER_H

#include "SalomeApp.h"
#include "OB_Filter.h"

class SalomeApp_SelectionMgr;

class SALOMEAPP_EXPORT SalomeApp_OBFilter: public OB_Filter
{
public:
  SalomeApp_OBFilter( SalomeApp_SelectionMgr* theSelMgr );
  ~SalomeApp_OBFilter();

  virtual bool isOk(  const SUIT_DataObject* ) const;

private:
  SalomeApp_SelectionMgr* mySelMgr;

};

#endif
