#include "SUIT_SelectionMgr.h"

SUIT_SelectionMgr::SUIT_SelectionMgr( const bool Feedback )
: myIterations( Feedback ? 1 : 0 ),
myIsSelChangeEnabled( true )
{
}

SUIT_SelectionMgr::~SUIT_SelectionMgr()
{
}

void SUIT_SelectionMgr::installSelector( SUIT_Selector* sel )
{
  if ( sel && !mySelectors.contains( sel ) )
    mySelectors.append( sel );
}

void SUIT_SelectionMgr::removeSelector( SUIT_Selector* sel )
{
  mySelectors.remove( sel );
}

void SUIT_SelectionMgr::selectors( QPtrList<SUIT_Selector>& lst ) const
{
  lst.clear();
  for ( SelectorListIterator it( mySelectors ); it.current(); ++it )
    lst.append( it.current() );
}

void SUIT_SelectionMgr::selectors( const QString& typ, QPtrList<SUIT_Selector>& lst ) const
{
  lst.clear();
  for ( SelectorListIterator it( mySelectors ); it.current(); ++it )
  {
    if ( it.current()->type() == typ )
      lst.append( it.current() );
  }
}

void SUIT_SelectionMgr::setEnabled( const bool on, const QString& typ )
{
  for ( SelectorListIterator it( mySelectors ); it.current(); ++it )
  {
    if ( typ.isEmpty() || it.current()->type() == typ )
      it.current()->setEnabled( on );
  }
}

void SUIT_SelectionMgr::selected( SUIT_DataOwnerPtrList& lst ) const
{
  lst.clear();

  QMap<const SUIT_DataOwner*, int> map;
  for ( SelectorListIterator it( mySelectors ); it.current(); ++it )
  {
    SUIT_DataOwnerPtrList curList;
    it.current()->selected( curList );
    for ( SUIT_DataOwnerPtrList::const_iterator itr = curList.begin(); itr != curList.end(); ++itr )
    {
      const SUIT_DataOwnerPtr& ptr = *itr;
      if ( !map.contains( ptr.operator->() ) )
        lst.append( ptr );
      map.insert( ptr.operator->(), 0 );
    }
  }
}

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

void SUIT_SelectionMgr::clearSelected()
{
  setSelected( SUIT_DataOwnerPtrList() );
}

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
      if ( aSel != sel )
  	    aSel->setSelected( newOwners );
    }
  }
  myIsSelChangeEnabled = true;

  emit selectionChanged();
}

bool SUIT_SelectionMgr::hasSelectionMode( const int mode ) const
{
  return mySelModes.contains( mode );
}

void SUIT_SelectionMgr::selectionModes( QValueList<int>& vals ) const
{
  vals = mySelModes;
}

void SUIT_SelectionMgr::setSelectionModes( const int mode )
{
  QValueList<int> lst;
  lst.append( mode );
  setSelectionModes( lst );
}

void SUIT_SelectionMgr::setSelectionModes( const QValueList<int>& lst )
{
  mySelModes = lst;
}

void SUIT_SelectionMgr::appendSelectionModes( const int mode )
{
  QValueList<int> lst;
  lst.append( mode );
  appendSelectionModes( lst );
}

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

void SUIT_SelectionMgr::removeSelectionModes( const int mode )
{
  QValueList<int> lst;
  lst.append( mode );
  removeSelectionModes( lst );
}

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

bool SUIT_SelectionMgr::isOk( const SUIT_DataOwner* owner ) const
{
  if ( !owner )
    return false;

  bool ok = true;
  for ( SelFilterListIterator it( myFilters ); it.current() && ok; ++it )
    ok = it.current()->isOk( owner );

  return ok;
}

bool SUIT_SelectionMgr::isOk( const SUIT_DataOwnerPtr& ptr ) const
{
  if ( ptr.isNull() )
    return false;

  return isOk( ptr.operator->() );
}

bool SUIT_SelectionMgr::hasFilter( SUIT_SelectionFilter* f ) const
{
  return myFilters.contains( f );
}

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

void SUIT_SelectionMgr::removeFilter( SUIT_SelectionFilter* f )
{
  myFilters.remove( f );
}

void SUIT_SelectionMgr::clearFilters()
{
  myFilters.clear();
}

bool SUIT_SelectionMgr::autoDeleteFilter() const
{
  return myFilters.autoDelete();
}

void SUIT_SelectionMgr::setAutoDeleteFilter( const bool on )
{
  myFilters.setAutoDelete( on );
}

void SUIT_SelectionMgr::filterOwners( const SUIT_DataOwnerPtrList& in, SUIT_DataOwnerPtrList& out ) const
{
  out.clear();
  for ( SUIT_DataOwnerPtrList::const_iterator it = in.begin(); it != in.end(); ++it )
  {
    if ( isOk( *it ) )
      out.append( *it );
  }
}
