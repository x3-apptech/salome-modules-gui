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

#if !defined(__SUIT_SMARTPTR_H)
#define __SUIT_SMARTPTR_H

#include "SUIT.h"

/*! \brief Base counter class what children using in SmartPtr class how template */
class SUIT_EXPORT RefCount {
public:
  //! constructor
  RefCount() : crefs( 0 ) {}
  //! copy constructor 
  RefCount( const RefCount& ) : crefs( 0 ) {}
  //! destructor
  virtual ~RefCount() {}
  //! operator= (does not change counter)
  RefCount& operator=( const RefCount& ) { return *this; }

  //! increments reference counter
  void upcount() { 
    ++crefs; 
  }

  //! decrements reference counter
  void downcount()
  {
    if ( crefs > 0 && --crefs == 0 )
      delete this;
  }
  
  //! get reference counter value
  int refcount() const { return crefs; }

private:
  unsigned long crefs;   //!< reference counter
};

/*! \brief Template class that provides automatic casting for hold RefCount based objects.*/
template <class T> class SmartPtr {
public:
  //! default constructor
  SmartPtr() : p( 0 ) {}
  //! constructor from any RefCount-based class
  template<class Y> SmartPtr( Y* y_ ) { p = dynamic_cast<T*>( y_ ); if ( p ) p->upcount(); }
  //! copy constructor from any RefCount-based class
  template<class Y> SmartPtr( const SmartPtr<Y>& y_ ) { p = dynamic_cast<T*>( y_.get() ); if ( p ) p->upcount(); }
  //! copy constructor
  SmartPtr( const SmartPtr& t_ ) : p( t_.p ) { if ( p ) p->upcount();   }
  //! destructor
  virtual ~SmartPtr(void)     
  { 
    if ( p )
      p->downcount(); 
  }

  // getting access
  T& operator*() const        { return *p;           }//!< return *pointer
  T* operator->() const       { return  p;           }//!< return pointer
  operator T*() const         { return  p;           }//!< return pointer
  T* get() const              { return  p;           }//!< return pointer

  //! assignment
  template<class Y> SmartPtr& operator=( const SmartPtr<Y>& y_ ) 
  { 
    if ( this == &y_) return *this;
    return operator=( y_.get() );
  }
  //! assignment
  SmartPtr& operator=( const SmartPtr& t_ )
  { 
    if ( this == &t_) return *this;
    return operator=( t_.get() ); 
  }
  //!< assignment
  SmartPtr& operator=( T* p_ )
  {
    if ( p )
      p->downcount(); 
    p = p_; 
    if ( p )
      p->upcount(); 
    return *this;
  }
  
  // comparing
  int operator==( const SmartPtr& t_ ) { return p == t_.p; }//!< comparing
  int operator==( const T* p_ )           { return p == p_; }//!< comparing
  friend int operator==( const T* p_, const SmartPtr& t_ ) { return t_ == p_; }//!< comparing
  int operator!=( SmartPtr& t_ ) { return p != t_.p; }//!< comparing
  int operator!=( T* p_ )           { return p != p_; }//!< comparing
  friend int operator!=( T* p_, SmartPtr& t_ ) { return p_ != t_.p; }//!< comparing

  //! nullify
  void nullify() { if ( p ) p->downcount(); p = 0; }
  //! check for null
  bool isNull() const { return p == 0; }

private:
  T* p;//!< stored pointer
};

/*! \def SMART( C )
 * Define macros SMART( C ) - same as SmartPtr(C), where \a C - class object.
 */
#define SMART( C ) SmartPtr<C>
//! casting class T2 to class T1
template <class T1, class T2> SMART(T1) downcast( SMART(T2)& t ) 
{
  return SMART(T1)(t.get());
}

#endif // __SUIT_SMARTPTR_H
