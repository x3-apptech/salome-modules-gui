#include "SUIT_Selector.h"

#include "SUIT_SelectionMgr.h"

SUIT_Selector::SUIT_Selector( SUIT_SelectionMgr* selMgr )
: mySelMgr( selMgr ),
myBlock( false ),
myEnabled( true ),
myAutoBlock( true )
{
  if ( selMgr )
    selMgr->installSelector( this );
}

SUIT_Selector::~SUIT_Selector()
{
  if ( selectionMgr() )
    selectionMgr()->removeSelector( this );
}

SUIT_SelectionMgr* SUIT_Selector::selectionMgr() const
{
  return mySelMgr;
}

bool SUIT_Selector::isEnabled() const
{
  return myEnabled;
}

void SUIT_Selector::setEnabled( const bool on )
{
  myEnabled = on;
}

bool SUIT_Selector::autoBlock() const
{
  return myAutoBlock;
}

void SUIT_Selector::setAutoBlock( const bool on )
{
  myAutoBlock = on;
}

void SUIT_Selector::selected( SUIT_DataOwnerPtrList& lst ) const
{
  lst.clear();
  getSelection( lst );
}

void SUIT_Selector::setSelected( const SUIT_DataOwnerPtrList& lst )
{
  if ( !isEnabled() )
    return;

  bool block = myBlock;
  myBlock = true;

  setSelection( lst );

  myBlock = block;
}

void SUIT_Selector::selectionChanged()
{
  if ( selectionMgr() && isEnabled() && ( !autoBlock() || !myBlock ) )
    selectionMgr()->selectionChanged( this );
}

bool SUIT_Selector::hasSelectionMode( const int mode ) const
{
  if ( !selectionMgr() )
    return false;

  return selectionMgr()->hasSelectionMode( mode );
}

void SUIT_Selector::selectionModes( QValueList<int>& lst ) const
{
  if ( selectionMgr() )
    selectionMgr()->selectionModes( lst );
}
