// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#include "SUIT_DataOwner.h"

#ifndef WNT
#include <typeinfo>
#define _typeinfo std::type_info
#else
#include <typeinfo.h>
#define _typeinfo type_info
#endif

/*! Constructor*/
SUIT_DataOwner::SUIT_DataOwner()
{
}

/*! Destructor*/
SUIT_DataOwner::~SUIT_DataOwner()
{
}

/*! Operator == compares two owners*/
bool operator==( const SUIT_DataOwnerPtr& p1, const SUIT_DataOwnerPtr& p2 )
{
  if ( !p1.isNull() && !p2.isNull() )
    return (p1->keyString() == p2->keyString());
  return p1.isNull() && p2.isNull();
}

// *** jfa: The below function has been put here 14.02.2007 from branch BR_Dev_For_4_0
// *** and also improved for better comparison of DataOwners with DataSubOwners.
// *** This comment is to be removed after merging with BR_Dev_For_4_0.
/*! Operator < allows to order suit data owners for map */
bool operator<( const SUIT_DataOwnerPtr& p1, const SUIT_DataOwnerPtr& p2 )
{
  if ( p1.isNull() && p2.isNull() )
    return false;
  else if ( p1.isNull() )
    return true;
  else if ( p2.isNull() )
    return false;

  return (p1->keyString() < p2->keyString());
}
// *** end

/*!
  \class SUIT_DataOwnerPtrList 
  implements value list with unique items (uniqueness is 
  provided by operator==() and operator<())
*/

/*!
  Constructor (default)
*/
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList()
  : QValueList<SUIT_DataOwnerPtr>(),
    mySkipEqual( true )
{
}

/*!
  Constructor (default)
*/
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList( const bool skipAllEqual )
  : QValueList<SUIT_DataOwnerPtr>(),
    mySkipEqual( skipAllEqual )
{
}

/*!
  Constructor (copy)
*/
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList( const SUIT_DataOwnerPtrList& l )
  : QValueList<SUIT_DataOwnerPtr>( l ),
    mySkipEqual( true )
{
}

/*!
  Constructor (copy)
*/
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList( const SUIT_DataOwnerPtrList& l, const bool skipAllEqual )
  : QValueList<SUIT_DataOwnerPtr>(),
    mySkipEqual( skipAllEqual )
{
  if ( skipAllEqual == l.mySkipEqual )
    operator =( l );
  else
  {
    SUIT_DataOwnerPtrList::const_iterator beginIt = l.begin();
    SUIT_DataOwnerPtrList::const_iterator endIt = l.end();
    for( ; beginIt != endIt; ++beginIt )
      append( *beginIt );
  }
}

#ifndef QT_NO_STL
/*!
  Constructor (from stl)
*/
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList( const std::list<SUIT_DataOwnerPtr>& l )
  : QValueList<SUIT_DataOwnerPtr>( l ),
    mySkipEqual( true )
{
}
#endif

#ifndef QT_NO_STL
/*!
  Constructor (from stl)
*/
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList( const std::list<SUIT_DataOwnerPtr>& l, const bool skipAllEqual )
  : QValueList<SUIT_DataOwnerPtr>(),
    mySkipEqual( skipAllEqual )
{
  std::list<SUIT_DataOwnerPtr>::const_iterator beginIt = l.begin();
  std::list<SUIT_DataOwnerPtr>::const_iterator endIt = l.begin();
  for( ; beginIt != endIt; ++beginIt )
    append( *beginIt );
}
#endif

/*!
  Appends an item to the list
*/
SUIT_DataOwnerPtrList::iterator SUIT_DataOwnerPtrList::append( const SUIT_DataOwnerPtr& x )
{
  if ( mySkipEqual && myMap.contains( x ) ) //contains uses SUIT_DataOwnerPtr::operator==
    return myMap[ x ];

  iterator it = QValueList<SUIT_DataOwnerPtr>::append( x );

   if ( mySkipEqual )
    myMap.insert( x, it );

  return it;
}

/*!
  Clear list
*/
void SUIT_DataOwnerPtrList::clear()
{
  if( mySkipEqual )
    myMap.clear();
  QValueList<SUIT_DataOwnerPtr>::clear();
}

/*!
  Remove an item from the list
*/
uint SUIT_DataOwnerPtrList::remove(const SUIT_DataOwnerPtr& x )
{
  if( mySkipEqual && myMap.contains(x) )
    myMap.remove(x);
  return QValueList<SUIT_DataOwnerPtr>::remove( x );
}
