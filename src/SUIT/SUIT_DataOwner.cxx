#include "SUIT_DataOwner.h"

#ifndef WNT
#include <typeinfo>
#define _typeinfo std::type_info
#else
#include <typeinfo.h>
#define _typeinfo type_info
#endif

//********************************************************************
// SUIT_DataOwner class
//********************************************************************


/*! Constructor*/
SUIT_DataOwner::SUIT_DataOwner()
{
}

/*! Destructor*/
SUIT_DataOwner::~SUIT_DataOwner()
{
}

/*! operator== : compares two owners*/
bool operator==( const SUIT_DataOwnerPtr& p1, const SUIT_DataOwnerPtr& p2 )
{
  if ( !p1.isNull() && !p2.isNull() )
    return p1->isEqual( *p2 );
  return p1.isNull() && p2.isNull();
}

//********************************************************************
/*! \class SUIT_DataOwnerPtrList 
 * implements value list with unique items (uniqueness is 
 * provided by operator==())
 */
//********************************************************************

//====================================================================
//! Constructor (default)
//====================================================================
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList( const bool skipAllEqal )
: QValueList<SUIT_DataOwnerPtr>(),
mySkipEqual( skipAllEqal )
{
}

//====================================================================
//! Constructor (copy)
//====================================================================
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList( const SUIT_DataOwnerPtrList& l )
: QValueList<SUIT_DataOwnerPtr>( l )
{
}

//====================================================================
//! Constructor (copy)
//====================================================================
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList( const SUIT_DataOwnerPtrList& l, const bool skipAllEqal )
: QValueList<SUIT_DataOwnerPtr>(),
mySkipEqual( skipAllEqal )
{
  if ( skipAllEqal == l.mySkipEqual )
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
//====================================================================
//! Constructor (from stl)
//====================================================================
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList( const std::list<SUIT_DataOwnerPtr>& l )
: QValueList<SUIT_DataOwnerPtr>( l )
{
}
#endif

#ifndef QT_NO_STL
//====================================================================
//! Constructor (from stl)
//====================================================================
SUIT_DataOwnerPtrList::SUIT_DataOwnerPtrList( const std::list<SUIT_DataOwnerPtr>& l, const bool skipAllEqal )
: QValueList<SUIT_DataOwnerPtr>(),
mySkipEqual( skipAllEqal )
{
  std::list<SUIT_DataOwnerPtr>::const_iterator beginIt = l.begin();
  std::list<SUIT_DataOwnerPtr>::const_iterator endIt = l.begin();
  for( ; beginIt != endIt; ++beginIt )
    append( *beginIt );
}
#endif

//====================================================================
//! Appends an item to the list
//====================================================================
SUIT_DataOwnerPtrList::iterator SUIT_DataOwnerPtrList::append( const SUIT_DataOwnerPtr& x )
{
  SUIT_DataOwnerPtrList::iterator it = find( x );
  if ( it != end() )
  {
    if ( mySkipEqual )
      return it;

    const _typeinfo& ti1 = typeid( *((*it).operator->()) );
    const _typeinfo& ti2 = typeid( *(x.operator->()) );

    if (ti1 == ti2)
      return it;
  }
  return QValueList<SUIT_DataOwnerPtr>::append( x );
}
