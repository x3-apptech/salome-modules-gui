#ifndef SALOMEAPP_TYPEFILTER_H
#define SALOMEAPP_TYPEFILTER_H

#include <qstring.h>

#include "SalomeApp_Filter.h"

class SALOMEAPP_EXPORT SalomeApp_TypeFilter: public SalomeApp_Filter
{
public:
  SalomeApp_TypeFilter( SalomeApp_Study* study, const QString& kind );
  ~SalomeApp_TypeFilter();

  virtual bool isOk( const SUIT_DataOwner* ) const;
  
private:
  QString myKind;
};

#endif
