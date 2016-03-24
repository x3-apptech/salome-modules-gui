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

#include "SUIT_Selector.h"

#include "SUIT_SelectionMgr.h"

#include <QObject>

/*!\class SUIT_Selector::Destroyer
  Class provide the watching for qobject parent class of the selector.
*/

class SUIT_Selector::Destroyer : public QObject
{
public:
  Destroyer( SUIT_Selector*, QObject* = 0 );
  virtual ~Destroyer();

  SUIT_Selector* selector() const;
  void           setSelector( SUIT_Selector* );

private:
  SUIT_Selector* mySelector;
};

SUIT_Selector::Destroyer::Destroyer( SUIT_Selector* s, QObject* p )
: QObject( p ),
  mySelector( s )
{
}

SUIT_Selector::Destroyer::~Destroyer()
{
  SUIT_Selector* s = mySelector;
  mySelector = 0;
  if ( s )
    delete s;
}

SUIT_Selector* SUIT_Selector::Destroyer::selector() const
{
  return mySelector;
}

void SUIT_Selector::Destroyer::setSelector( SUIT_Selector* s )
{
  mySelector = s;
}

/*!\class SUIT_Selector
 * Class provide selector for data owners.
 */

/*!
  Constructor.
*/
SUIT_Selector::SUIT_Selector( SUIT_SelectionMgr* selMgr, QObject* parent )
: mySelMgr( selMgr ),
  myBlock( false ),
  myEnabled( true ),
  myAutoBlock( true ),
  myDestroyer( 0 )
{
  if ( selMgr )
    selMgr->installSelector( this );

  if ( parent )
    myDestroyer = new Destroyer( this, parent );
}

/*!
  Destructor.
*/
SUIT_Selector::~SUIT_Selector()
{
  if ( selectionMgr() )
    selectionMgr()->removeSelector( this );

  if ( myDestroyer && myDestroyer->selector() == this )
  {
    myDestroyer->setSelector( 0 );
    delete myDestroyer;
    myDestroyer = 0;
  }
}

/*!
  Gets selection manager.
*/
SUIT_SelectionMgr* SUIT_Selector::selectionMgr() const
{
  return mySelMgr;
}

/*!
  Checks: Is selctor enabled?
*/
bool SUIT_Selector::isEnabled() const
{
  return myEnabled;
}

/*!
  Sets selctor anbled to \a on.
*/
void SUIT_Selector::setEnabled( const bool on )
{
  myEnabled = on;
}

/*!
  Checks: Is selector auto block?
*/
bool SUIT_Selector::autoBlock() const
{
  return myAutoBlock;
}

/*!
  Sets selctor autoblock to \a on.
*/
void SUIT_Selector::setAutoBlock( const bool on )
{
  myAutoBlock = on;
}

/*!
  Puts to \a lst selection list of data owners.
*/
void SUIT_Selector::selected( SUIT_DataOwnerPtrList& lst ) const
{
  lst.clear();
  getSelection( lst );
}

/*!
  Puts to selection list of data owners \a lst..
*/
void SUIT_Selector::setSelected( const SUIT_DataOwnerPtrList& lst )
{
  if ( !isEnabled() )
    return;

  bool block = myBlock;
  myBlock = true;

  setSelection( lst );

  myBlock = block;
}

/*!
  On selection changed.
*/
void SUIT_Selector::selectionChanged()
{
  if ( selectionMgr() && isEnabled() && ( !autoBlock() || !myBlock ) )
    selectionMgr()->selectionChanged( this );
}

/*!
  Checks: Is selection manager has selection mode \a mode?
*/
bool SUIT_Selector::hasSelectionMode( const int mode ) const
{
  if ( !selectionMgr() )
    return false;

  return selectionMgr()->hasSelectionMode( mode );
}

/*!
  Puts to list \a lst selection modes from selection manager.
*/
void SUIT_Selector::selectionModes( QList<int>& lst ) const
{
  if ( selectionMgr() )
    selectionMgr()->selectionModes( lst );
}

/*!
  Get owner of this selector.
 */
QObject* SUIT_Selector::owner() const
{
  return myDestroyer ? myDestroyer->parent() : 0;
}
