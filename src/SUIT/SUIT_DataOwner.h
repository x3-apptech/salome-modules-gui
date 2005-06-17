#ifndef SUIT_DATAOWNER_H
#define SUIT_DATAOWNER_H

#include "SUIT_SmartPtr.h"

#include <qvaluelist.h>

#ifdef WIN32
#pragma warning( disable:4275 )
#endif

class SUIT_EXPORT SUIT_DataOwner : public RefCount 
{
public:
  SUIT_DataOwner();
  virtual ~SUIT_DataOwner();

  virtual bool isEqual( const SUIT_DataOwner& ) const = 0;            
};

typedef SMART(SUIT_DataOwner) SUIT_DataOwnerPtr;

bool operator==( const SUIT_DataOwnerPtr&, const SUIT_DataOwnerPtr& );

class SUIT_EXPORT SUIT_DataOwnerPtrList : public QValueList<SUIT_DataOwnerPtr> 
{
public:
  SUIT_DataOwnerPtrList();
  SUIT_DataOwnerPtrList( const SUIT_DataOwnerPtrList& l );
#ifndef QT_NO_STL
  SUIT_DataOwnerPtrList( const std::list<SUIT_DataOwnerPtr>& l );
#endif

  iterator append      ( const SUIT_DataOwnerPtr& x );

private:
  // hide this methods: only append() should be used to add items to the list
  iterator prepend( const SUIT_DataOwnerPtr& x );
  iterator insert ( iterator it, const SUIT_DataOwnerPtr& x );
  void push_front ( const SUIT_DataOwnerPtr& x );
  void push_back  ( const SUIT_DataOwnerPtr& x );
};

#ifdef WIN32
#pragma warning( default:4275 )
#endif

#endif
