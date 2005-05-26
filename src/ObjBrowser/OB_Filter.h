#ifndef OB_FILTER_H
#define OB_FILTER_H

#include "OB.h"

class SUIT_DataObject;

class OB_EXPORT OB_Filter
{
public:
  OB_Filter();
  virtual ~OB_Filter();

  virtual bool isOk( const SUIT_DataObject* ) const = 0;
};

#endif
