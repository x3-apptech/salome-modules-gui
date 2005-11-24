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
  SUIT_DataOwner();//!< constructor
  virtual ~SUIT_DataOwner();//!< destructor
  //! compare function
  virtual bool isEqual( const SUIT_DataOwner& ) const = 0;
};

/*! \typedef SUIT_DataOwnerPtr
 * Define smart pointer for SUIT_DataOwner object
 */
typedef SMART(SUIT_DataOwner) SUIT_DataOwnerPtr;

/*! Comparing two SUIT_DataOwnerPtr objects.*/
bool operator==( const SUIT_DataOwnerPtr&, const SUIT_DataOwnerPtr& );

/*! \class QValueList
 *  \brief For more documentation see <a href="http://doc.trolltech.com">QT documentation</a>.
 * QT class
 */
/*! \class SUIT_DataOwnerPtrList
 * \brief Manage list of SUIT_DataOwnerPtr.
 */
class SUIT_EXPORT SUIT_DataOwnerPtrList : public QValueList<SUIT_DataOwnerPtr> 
{
public:
  SUIT_DataOwnerPtrList( const bool skipAllEqal = true );//!< constructor
  SUIT_DataOwnerPtrList( const SUIT_DataOwnerPtrList& l );//!< copy constructor
  SUIT_DataOwnerPtrList( const SUIT_DataOwnerPtrList& l, const bool skipAllEqal );//!< copy constructor
#ifndef QT_NO_STL
  SUIT_DataOwnerPtrList( const std::list<SUIT_DataOwnerPtr>& l );//!< copy constructor for STL list
  SUIT_DataOwnerPtrList( const std::list<SUIT_DataOwnerPtr>& l, const bool skipAllEqal );//!< copy constructor for STL list
#endif

  iterator append      ( const SUIT_DataOwnerPtr& x );//!< append function

private:
  // hide this methods: only append() should be used to add items to the list
  iterator prepend( const SUIT_DataOwnerPtr& x );//!< hide method
  iterator insert ( iterator it, const SUIT_DataOwnerPtr& x );//!< hide method
  void push_front ( const SUIT_DataOwnerPtr& x );//!< hide method
  void push_back  ( const SUIT_DataOwnerPtr& x );//!< hide method
private:
  bool mySkipEqual;
};

#ifdef WIN32
#pragma warning( default:4275 )
#endif

#endif
