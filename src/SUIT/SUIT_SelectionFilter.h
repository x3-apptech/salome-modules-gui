#ifndef SUIT_SELECTIONFILTER_H
#define SUIT_SELECTIONFILTER_H

#include "SUIT.h"

class SUIT_DataOwner;

/*!Base class.*/
class SUIT_EXPORT SUIT_SelectionFilter
{
public:
  SUIT_SelectionFilter();
  ~SUIT_SelectionFilter();

  virtual bool isOk( const SUIT_DataOwner* ) const = 0;
};

#endif
