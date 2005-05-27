#ifndef SALOMEAPP_FILTER_H
#define SALOMEAPP_FILTER_H

#include "SalomeApp.h"
#include "SUIT_SelectionFilter.h"

class SalomeApp_Study;

class SALOMEAPP_EXPORT SalomeApp_Filter: public SUIT_SelectionFilter
{
public:
  SalomeApp_Filter(SalomeApp_Study* study);
  ~SalomeApp_Filter();

protected:
  SalomeApp_Study* getStudy() const;

private:
  SalomeApp_Study* myStudy;
};

#endif
