// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SUIT_DataObjectKey.h"

#include <string.h>

#ifndef WIN32
#include <typeinfo>
#define _typeinfo std::type_info
#else
#include <typeinfo.h>
#define _typeinfo type_info
#endif

/*!\class SUIT_DataObjectKey
 * Key for personal idetfication of SUIT_DataObject.
 */

/*!
  Constructor.
*/
SUIT_DataObjectKey::SUIT_DataObjectKey()
: refCounter( 0 )
{
}

/*!
  Destructor.
*/
SUIT_DataObjectKey::~SUIT_DataObjectKey()
{
}

/*!\class SUIT_DataObjectKeyHandle
 * Wrapper around the pointer of class SUIT_DataObjectKey.
 */

/*!
  Constructor. 
*/
SUIT_DataObjectKeyHandle::SUIT_DataObjectKeyHandle()
: myKey( 0 )
{
}

/*!
  Copy Constructor.
*/
SUIT_DataObjectKeyHandle::SUIT_DataObjectKeyHandle( const SUIT_DataObjectKeyHandle& other )
: myKey( other.myKey )
{
  myKey = other.myKey;

  beginScope();
}

/*!
  Constructor. Initialize by key \a key.
*/
SUIT_DataObjectKeyHandle::SUIT_DataObjectKeyHandle( SUIT_DataObjectKey* key )
: myKey( key )
{
  beginScope();
}

/*!
  Destructor.
*/
SUIT_DataObjectKeyHandle::~SUIT_DataObjectKeyHandle()
{
  nullify();
}

/*!
 * Checks: Is key null?
 *\retval \c true - if null, esle \c false.
*/
bool SUIT_DataObjectKeyHandle::isNull() const
{
  return !myKey;
}

/*!
  Nullify key.
*/
void SUIT_DataObjectKeyHandle::nullify()
{
  endScope();
}

/*!
 *  Operator less.
 *\retval boolean. \c true - If current key less than \a kw.
 */
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

/*!
 * Operator is equal.
 *\retval boolean. \c true - If current key equal \a kw.
 */
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

/*!
 * Copy value of key \a kw to current.
 */
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

/*!
 * Inctrement reference counter for current key.
 */
void SUIT_DataObjectKeyHandle::beginScope()
{
  if ( myKey )
    myKey->refCounter++;
}

/*!
 * Decrement reference counter for current key.
 */
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
