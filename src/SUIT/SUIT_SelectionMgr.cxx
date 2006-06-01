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
#include "SUIT_SelectionMgr.h"

/*!\class SUIT_SelectionMgr
 * Provide selection manager. Manipulate by selection filters, modes, data owners.
 */

/*!constructor. initialize myIterations and myIsSelChangeEnabled.*/
SUIT_SelectionMgr::SUIT_SelectionMgr( const bool Feedback, QObject* p )
: QObject( p ),
myIterations( Feedback ? 1 : 0 ),
myIsSelChangeEnabled( true )
{
}

/*!destructor. mySelectors auto delete.*/
SUIT_SelectionMgr::~SUIT_SelectionMgr()
{
  mySelectors.setAutoDelete( true );
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
  mySelectors.remove( sel );
}

/*!Gets selectors list to \a lst.*/
void SUIT_SelectionMgr::selectors( QPtrList<SUIT_Selector>& lst ) const
{
  lst.clear();
  for ( SelectorListIterator it( mySelectors ); it.current(); ++it )
    lst.append( it.current() );
}

/*!Gets selectors list to \a lst with type \a typ.*/
void SUIT_SelectionMgr::selectors( const QString& typ, QPtrList<SUIT_Selector>& lst ) const
{
  lst.clear();
  for ( SelectorListIterator it( mySelectors ); it.current(); ++it )
  {
    if ( it.current()->type() == typ )
      lst.append( it.current() );
  }
}

/*! Sets ebabled to \a on for all data owners with type \a typ.
*/
void SUIT_SelectionMgr::setEnabled( const bool on, const QString& typ )
{
  for ( SelectorListIterator it( mySelectors ); it.current(); ++it )
  {
    if ( typ.isEmpty() || it.current()->type() == typ )
      it.current()->setEnabled( on );
  }
}

/*! Gets selected data owners from list with type \a type to list \a lst.
*/
void SUIT_SelectionMgr::selected( SUIT_DataOwnerPtrList& lst, const QString& type ) const
{
  lst.clear();

  for ( SelectorListIterator it( mySelectors ); it.current(); ++it )
  {
    if ( !type.isEmpty() && it.current()->type() != type )
      continue;
    SUIT_DataOwnerPtrList curList;
    it.current()->selected( curList );
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

  for ( SelectorListIterator it( mySelectors ); it.current(); ++it )
  {
    if ( append )
    {
      SUIT_DataOwnerPtrList current;
      it.current()->selected( current );
      for ( SUIT_DataOwnerPtrList::const_iterator it = current.begin(); it != current.end(); ++it )
        owners.append( *it );
    }
    it.current()->setSelected( owners );
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
    for ( SUIT_Selector* aSel = mySelectors.first(); aSel; aSel = mySelectors.next() )
    {
      // Temporary action(to avoid selection of the objects which don't pass the filters):
      //if ( aSel != sel )
	    aSel->setSelected( newOwners );
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
void SUIT_SelectionMgr::selectionModes( QValueList<int>& vals ) const
{
  vals = mySelModes;
}

/*! Set selection mode \a mode to list of selection modes.
*/
void SUIT_SelectionMgr::setSelectionModes( const int mode )
{
  QValueList<int> lst;
  lst.append( mode );
  setSelectionModes( lst );
}

/*! Sets selection modes list from \a lst.
*/
void SUIT_SelectionMgr::setSelectionModes( const QValueList<int>& lst )
{
  mySelModes = lst;
}

/*! Append selection mode \a mode to list of selection modes.
*/
void SUIT_SelectionMgr::appendSelectionModes( const int mode )
{
  QValueList<int> lst;
  lst.append( mode );
  appendSelectionModes( lst );
}

/*! Append selection modes \a lst list.
*/
void SUIT_SelectionMgr::appendSelectionModes( const QValueList<int>& lst )
{
  QMap<int, int> map;
  for ( QValueList<int>::const_iterator it = mySelModes.begin(); it != mySelModes.end(); ++it )
    map.insert( *it, 0 );

  for ( QValueList<int>::const_iterator itr = lst.begin(); itr != lst.end(); ++itr )
  {
    if ( !map.contains( *itr ) )
      mySelModes.append( *itr );
  }
}

/*! Remove selection mode \a mode from list.
*/
void SUIT_SelectionMgr::removeSelectionModes( const int mode )
{
  QValueList<int> lst;
  lst.append( mode );
  removeSelectionModes( lst );
}

/*! Remove selection modea \a lst from list.
*/
void SUIT_SelectionMgr::removeSelectionModes( const QValueList<int>& lst )
{
  QMap<int, int> map;
  for ( QValueList<int>::const_iterator it = mySelModes.begin(); it != mySelModes.end(); ++it )
    map.insert( *it, 0 );

  for ( QValueList<int>::const_iterator itr = lst.begin(); itr != lst.end(); ++itr )
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
  for ( SelFilterListIterator it( myFilters ); it.current() && ok; ++it )
    ok = it.current()->isOk( owner );

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
  myFilters.remove( f );
}

/*! Clear filters list.
*/
void SUIT_SelectionMgr::clearFilters()
{
  myFilters.clear();
}

/*! Sets auto delete filter.
*/
bool SUIT_SelectionMgr::autoDeleteFilter() const
{
  return myFilters.autoDelete();
}

/*! Sets auto delete filter to \a on.
*/
void SUIT_SelectionMgr::setAutoDeleteFilter( const bool on )
{
  myFilters.setAutoDelete( on );
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
