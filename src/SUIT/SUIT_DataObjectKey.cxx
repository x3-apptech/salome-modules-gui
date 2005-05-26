#include "SUIT_DataObjectKey.h"

#include <qobject.h>

#ifndef WNT
#include <typeinfo>
#define _typeinfo std::type_info
#else
#include <typeinfo.h>
#define _typeinfo type_info
#endif

/*!
    Class: SUIT_DataObjectKey
    Descr: Key for personal idetfication of SUIT_DataObject.
*/

SUIT_DataObjectKey::SUIT_DataObjectKey()
: refCounter( 0 )
{
}

SUIT_DataObjectKey::~SUIT_DataObjectKey()
{
}

/*!
    Class: SUIT_DataObjectKeyHandle
    Descr: Wrapper around the pointer of class SUIT_DataObjectKey.
*/

SUIT_DataObjectKeyHandle::SUIT_DataObjectKeyHandle()
: myKey( 0 )
{
}

SUIT_DataObjectKeyHandle::SUIT_DataObjectKeyHandle( const SUIT_DataObjectKeyHandle& other )
: myKey( other.myKey )
{
  myKey = other.myKey;

  beginScope();
}

SUIT_DataObjectKeyHandle::SUIT_DataObjectKeyHandle( SUIT_DataObjectKey* key )
: myKey( key )
{
  beginScope();
}

SUIT_DataObjectKeyHandle::~SUIT_DataObjectKeyHandle()
{
  nullify();
}

bool SUIT_DataObjectKeyHandle::isNull() const
{
  return !myKey;
}

void SUIT_DataObjectKeyHandle::nullify()
{
  endScope();
}

bool SUIT_DataObjectKeyHandle::operator<( const SUIT_DataObjectKeyHandle& kw ) const
{
  if ( myKey == kw.myKey )
    return false;

  if ( !myKey || !kw.myKey )
    return myKey < kw.myKey;

  const _typeinfo& i1 = typeid( *myKey );
  const _typeinfo& i2 = typeid( *kw.myKey );

  int cmp = strcmp( i1.name(), i2.name() );
  if ( cmp < 0 )
    return true;
  else if ( cmp > 0 )
    return false;
  else
    return myKey->isLess( kw.myKey );
}

bool SUIT_DataObjectKeyHandle::operator==( const SUIT_DataObjectKeyHandle& kw ) const
{
  if ( myKey == kw.myKey )
    return true;

  if ( !myKey || !kw.myKey )
    return false;

  if ( typeid( *myKey ) != typeid( *kw.myKey ) )
    return false;

  return myKey->isEqual( kw.myKey );
}

SUIT_DataObjectKeyHandle& SUIT_DataObjectKeyHandle::operator=( const SUIT_DataObjectKeyHandle& kw )
{
  if ( myKey != kw.myKey )
  {
    endScope();

    myKey = kw.myKey;

    beginScope();
  }
  return *this;
}

void SUIT_DataObjectKeyHandle::beginScope()
{
  if ( myKey )
    myKey->refCounter++;
}

void SUIT_DataObjectKeyHandle::endScope()
{
  if ( !myKey )
    return;

  myKey->refCounter--;

  if ( !myKey->refCounter )
  {
    delete myKey;
    myKey = 0;
  }
}
