#ifndef SUIT_DATAOBJECTKEY_H
#define SUIT_DATAOBJECTKEY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SUIT.h"

class SUIT_EXPORT SUIT_DataObjectKey
{
public:
  SUIT_DataObjectKey();
  ~SUIT_DataObjectKey();

  virtual bool isLess( const SUIT_DataObjectKey* ) const = 0;
  virtual bool isEqual( const SUIT_DataObjectKey* ) const = 0;

private:
  int     refCounter;

  friend class SUIT_DataObjectKeyHandle;
};

class SUIT_EXPORT SUIT_DataObjectKeyHandle
{
public:
  SUIT_DataObjectKeyHandle();
  SUIT_DataObjectKeyHandle( SUIT_DataObjectKey* );
  SUIT_DataObjectKeyHandle( const SUIT_DataObjectKeyHandle& );
  ~SUIT_DataObjectKeyHandle();

  void                      nullify();
  bool                      isNull() const;
  bool                      operator <( const SUIT_DataObjectKeyHandle& ) const;
  bool                      operator ==( const SUIT_DataObjectKeyHandle& ) const;
  SUIT_DataObjectKeyHandle& operator =( const SUIT_DataObjectKeyHandle& );

private:
  void                      beginScope();
  void                      endScope();

private:
  SUIT_DataObjectKey*       myKey;
};

#endif
