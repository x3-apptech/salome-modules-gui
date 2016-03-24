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

#include "SUIT_SelectionMgr.h"

#include "SUIT_Selector.h"
#include "SUIT_SelectionFilter.h"

/*!\class SUIT_SelectionMgr
 * Provide selection manager. Manipulate by selection filters, modes, data owners.
 */

/*!constructor. initialize myIterations and myIsSelChangeEnabled.*/
SUIT_SelectionMgr::SUIT_SelectionMgr( const bool Feedback, QObject* p )
: QObject( p ),
myIterations( Feedback ? 1 : 0 ),
myAutoDelFilter( false ),
myIsSelChangeEnabled( true )
{
}

/*!destructor. mySelectors auto delete.*/
SUIT_SelectionMgr::~SUIT_SelectionMgr()
{
  while( !mySelectors.empty() ) {
    SelectorList::iterator it = mySelectors.begin();
    delete *it;
  }
}

/*!Add selector \a sel to selectors list,if it's not exists in list.*/
void SUIT_SelectionMgr::installSelector( SUIT_Selector* sel )
{
  if ( sel && !mySelectors.contains( sel ) )
    mySelectors.append( sel );
}

/*!Remove selector \a sel from list.*/
void SUIT_SelectionMgr::removeSelector( SUIT_Selector* sel )
{
  mySelectors.removeAll( sel );
}

/*!Gets selectors list to \a lst.*/
void SUIT_SelectionMgr::selectors( QList<SUIT_Selector*>& lst ) const
{
  lst.clear();
  for ( SelectorList::const_iterator it = mySelectors.begin(); it != mySelectors.end(); ++it )
    lst.append( *it );
}

/*!Gets selectors list to \a lst with type \a typ.*/
void SUIT_SelectionMgr::selectors( const QString& typ, QList<SUIT_Selector*>& lst ) const
{
  lst.clear();
  for ( SelectorList::const_iterator it = mySelectors.begin(); it != mySelectors.end(); ++it )
  {
    if ( (*it)->type() == typ )
      lst.append( *it );
  }
}

/*! Sets ebabled to \a on for all data owners with type \a typ.
*/
void SUIT_SelectionMgr::setEnabled( const bool on, const QString& typ )
{
  for ( SelectorList::const_iterator it = mySelectors.begin(); it != mySelectors.end(); ++it )
  {
    if ( typ.isEmpty() || (*it)->type() == typ )
      (*it)->setEnabled( on );
  }
}

/*! Gets selected data owners from list with type \a type to list \a lst.
*/
void SUIT_SelectionMgr::selected( SUIT_DataOwnerPtrList& lst, const QString& type ) const
{
  lst.clear();

  for ( SelectorList::const_iterator it = mySelectors.begin(); it != mySelectors.end(); ++it )
  {
    if ( !(*it)->isEnabled() )
      continue;
    if ( !type.isEmpty() && (*it)->type() != type )
      continue;

    SUIT_DataOwnerPtrList curList;
    (*it)->selected( curList );
    for ( SUIT_DataOwnerPtrList::const_iterator itr = curList.begin(); itr != curList.end(); ++itr )
      lst.append( *itr );
  }
}

/*! Sets selected data owners from \a lst and append to list, if \a append - true.
*/
void SUIT_SelectionMgr::setSelected( const SUIT_DataOwnerPtrList& lst, const bool append )
{
  SUIT_DataOwnerPtrList owners;
  filterOwners( lst, owners );

  for ( SelectorList::const_iterator it = mySelectors.begin(); it != mySelectors.end(); ++it )
  {
    if ( append )
    {
      SUIT_DataOwnerPtrList current;
      (*it)->selected( current );
      for ( SUIT_DataOwnerPtrList::const_iterator it = current.begin(); it != current.end(); ++it )
        owners.append( *it );
    }
    (*it)->setSelected( owners );
  }
}

/*! Clear selected data owners.
*/
void SUIT_SelectionMgr::clearSelected()
{
  setSelected( SUIT_DataOwnerPtrList() );
}

/*! On selection \a sel changed.
*/
void SUIT_SelectionMgr::selectionChanged( SUIT_Selector* sel )
{
  if ( !sel || !myIsSelChangeEnabled || !sel->isEnabled() )
    return;

  SUIT_DataOwnerPtrList owners;

  myIsSelChangeEnabled = false;
  sel->selected( owners );

  SUIT_DataOwnerPtrList newOwners;
  filterOwners( owners, newOwners );

  for ( int i = 0; i < myIterations; i++ )
  {
    for ( SelectorList::iterator it = mySelectors.begin(); it != mySelectors.end(); ++it )
    {
      // Temporary action(to avoid selection of the objects which don't pass the filters):
      //if ( *it != sel )
        (*it)->setSelected( newOwners );
    }
  }
  myIsSelChangeEnabled = true;

  emit selectionChanged();
}

/*!
  Returns true if selection manger is in synchronising mode
  (during synchonisation of the selectors selection).
*/
bool SUIT_SelectionMgr::isSynchronizing() const
{
  return !myIsSelChangeEnabled;
}

/*! Checks: Is selection manager has selection mode \a mode?
*/
bool SUIT_SelectionMgr::hasSelectionMode( const int mode ) const
{
  return mySelModes.contains( mode );
}

/*! Gets selection modes to list \a vals.
*/
void SUIT_SelectionMgr::selectionModes( QList<int>& vals ) const
{
  vals = mySelModes;
}

/*! Set selection mode \a mode to list of selection modes.
*/
void SUIT_SelectionMgr::setSelectionModes( const int mode )
{
  QList<int> lst;
  lst.append( mode );
  setSelectionModes( lst );
}

/*! Sets selection modes list from \a lst.
*/
void SUIT_SelectionMgr::setSelectionModes( const QList<int>& lst )
{
  mySelModes = lst;
}

/*! Append selection mode \a mode to list of selection modes.
*/
void SUIT_SelectionMgr::appendSelectionModes( const int mode )
{
  QList<int> lst;
  lst.append( mode );
  appendSelectionModes( lst );
}

/*! Append selection modes \a lst list.
*/
void SUIT_SelectionMgr::appendSelectionModes( const QList<int>& lst )
{
  QMap<int, int> map;
  for ( QList<int>::const_iterator it = mySelModes.begin(); it != mySelModes.end(); ++it )
    map.insert( *it, 0 );

  for ( QList<int>::const_iterator itr = lst.begin(); itr != lst.end(); ++itr )
  {
    if ( !map.contains( *itr ) )
      mySelModes.append( *itr );
  }
}

/*! Remove selection mode \a mode from list.
*/
void SUIT_SelectionMgr::removeSelectionModes( const int mode )
{
  QList<int> lst;
  lst.append( mode );
  removeSelectionModes( lst );
}

/*! Remove selection modea \a lst from list.
*/
void SUIT_SelectionMgr::removeSelectionModes( const QList<int>& lst )
{
  QMap<int, int> map;
  for ( QList<int>::const_iterator it = mySelModes.begin(); it != mySelModes.end(); ++it )
    map.insert( *it, 0 );

  for ( QList<int>::const_iterator itr = lst.begin(); itr != lst.end(); ++itr )
    map.remove( *itr );

  mySelModes.clear();
  for ( QMap<int, int>::ConstIterator iter = map.begin(); iter != map.end(); ++iter )
    mySelModes.append( iter.key() );
}

/*! Checks data owner is ok?
*/
bool SUIT_SelectionMgr::isOk( const SUIT_DataOwner* owner ) const
{
  if ( !owner )
    return false;

  bool ok = true;
  for ( SelFilterList::const_iterator it = myFilters.begin(); it != myFilters.end() && ok; ++it )
    ok = (*it)->isOk( owner );

  return ok;
}

/*! Checks data owner pointer is ok?
*/
bool SUIT_SelectionMgr::isOk( const SUIT_DataOwnerPtr& ptr ) const
{
  if ( ptr.isNull() )
    return false;

  return isOk( ptr.operator->() );
}

/*! Checks selection manager has filter \a f?
*/
bool SUIT_SelectionMgr::hasFilter( SUIT_SelectionFilter* f ) const
{
  return myFilters.contains( f );
}

/*! Install filter \a f and set selected, if \a update = true.
*/
void SUIT_SelectionMgr::installFilter( SUIT_SelectionFilter* f, const bool updateSelection )
{
  if ( !hasFilter( f ) )
  {
    SUIT_DataOwnerPtrList selOwners;
    if( updateSelection )
      selected( selOwners );
      
    myFilters.append( f );
    
    if( updateSelection )
      setSelected( selOwners );
  }
}

/*! Remove filter \a f from filters list.
*/
void SUIT_SelectionMgr::removeFilter( SUIT_SelectionFilter* f )
{
  if ( !myFilters.contains( f ) )
    return;

  myFilters.removeAll( f );

  if ( autoDeleteFilter() )
    delete f;
}

/*! Clear filters list.
*/
void SUIT_SelectionMgr::clearFilters()
{
  if ( autoDeleteFilter() )
  {
    for ( SelFilterList::const_iterator it = myFilters.begin(); it != myFilters.end(); ++it )
      delete *it;
  }

  myFilters.clear();
}

/*! Sets auto delete filter.
*/
bool SUIT_SelectionMgr::autoDeleteFilter() const
{
  return myAutoDelFilter;
}

/*! Sets auto delete filter to \a on.
*/
void SUIT_SelectionMgr::setAutoDeleteFilter( const bool on )
{
  myAutoDelFilter = on;
}

/*! Gets good data owners list to \a out from \a in.
*/
void SUIT_SelectionMgr::filterOwners( const SUIT_DataOwnerPtrList& in, SUIT_DataOwnerPtrList& out ) const
{
  out.clear();
  for ( SUIT_DataOwnerPtrList::const_iterator it = in.begin(); it != in.end(); ++it )
  {
    if ( isOk( *it ) )
      out.append( *it );
  }
}
