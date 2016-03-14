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

#ifndef SUIT_DATAOWNER_H
#define SUIT_DATAOWNER_H

#include "SUIT_SmartPtr.h"

#include <QMap>
#include <QList>
#include <QString>

#ifdef WIN32
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )
#endif

/*!
  \class SUIT_DataOwner
  Represents abstract selected objects.
  Used for selection synchronizing between windows(e.g. ObjectBrowser) and viewers
*/

class SUIT_EXPORT SUIT_DataOwner : public RefCount 
{
public:
  SUIT_DataOwner();//!< constructor
  virtual ~SUIT_DataOwner();//!< destructor
  virtual QString keyString() const = 0;//!< used for comparison
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
class SUIT_EXPORT SUIT_DataOwnerPtrList : public QList<SUIT_DataOwnerPtr>
{
public:
  SUIT_DataOwnerPtrList();                         //!< constructor
  SUIT_DataOwnerPtrList( const bool skipAllEqual );//!< constructor
  SUIT_DataOwnerPtrList( const SUIT_DataOwnerPtrList& l );                         //!< copy constructor
  SUIT_DataOwnerPtrList( const SUIT_DataOwnerPtrList& l, const bool skipAllEqual );//!< copy constructor

  void     append      ( const SUIT_DataOwnerPtr& x );//!< append function
  void     clear       ();
  uint     remove      ( const SUIT_DataOwnerPtr& x );

private:
  // hide this methods: only append() should be used to add items to the list
  iterator prepend( const SUIT_DataOwnerPtr& x );//!< hide method
  iterator insert ( iterator it, const SUIT_DataOwnerPtr& x );//!< hide method
  void push_front ( const SUIT_DataOwnerPtr& x );//!< hide method
  void push_back  ( const SUIT_DataOwnerPtr& x );//!< hide method

private:
  QMap<SUIT_DataOwnerPtr, int> myMap;
  bool                         mySkipEqual;
};

#ifdef WIN32
#pragma warning( default:4251 )
#pragma warning( default:4275 )
#endif

#endif
