#include "SalomeApp_OBSelector.h"

#include "SalomeApp_DataOwner.h"
#include "SalomeApp_DataObject.h"

#include <OB_Browser.h>

#include <SUIT_DataObjectIterator.h>

/*!
  Constructor
*/
SalomeApp_OBSelector::SalomeApp_OBSelector( OB_Browser* ob, SUIT_SelectionMgr* mgr )
: SUIT_Selector( mgr, ob ),
  myBrowser( ob )
{
  if ( myBrowser ) {
    connect( myBrowser, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
  }    
}

/*!
  Destructor
*/
SalomeApp_OBSelector::~SalomeApp_OBSelector()
{
}

/*!
  Gets browser.
*/
OB_Browser* SalomeApp_OBSelector::browser() const
{
  return myBrowser;
}

/*!
  Gets selection.
*/
void SalomeApp_OBSelector::getSelection( SUIT_DataOwnerPtrList& thelist ) const
{
  if ( !myBrowser )
    return;

  DataObjectList objlist;
  myBrowser->getSelected( objlist );
  for ( DataObjectListIterator it( objlist ); it.current(); ++it )
  {
    SalomeApp_DataObject* obj = dynamic_cast<SalomeApp_DataObject*>( it.current() );
    if ( obj )
    {
      Handle( SALOME_InteractiveObject ) aSObj = new SALOME_InteractiveObject
	( obj->entry(), obj->componentDataType(), obj->name() );
      SalomeApp_DataOwner* owner = new SalomeApp_DataOwner( aSObj  );
      thelist.append( SUIT_DataOwnerPtr( owner ) );
    }
  }
}

/*!Sets selection.*/
void SalomeApp_OBSelector::setSelection( const SUIT_DataOwnerPtrList& thelist )
{
  if ( !myBrowser )
    return;

  QMap<QString, SalomeApp_DataObject*> themap;
  fillEntries( themap );

  DataObjectList objList;
  for ( SUIT_DataOwnerPtrList::const_iterator it = thelist.begin(); it != thelist.end(); ++it )
  {
    const SalomeApp_DataOwner* owner = dynamic_cast<const SalomeApp_DataOwner*>( (*it).operator->() );
    if ( owner && themap.contains( owner->entry() ) )
      objList.append( themap[owner->entry()] );
  }

  myBrowser->setSelected( objList );
}

/*!On selection changed.*/
void SalomeApp_OBSelector::onSelectionChanged()
{
  selectionChanged();
}

/*!Fill entries.*/
void SalomeApp_OBSelector::fillEntries( QMap<QString, SalomeApp_DataObject*>& entires )
{
  entires.clear();

  if ( !myBrowser )
    return;

  for ( SUIT_DataObjectIterator it( myBrowser->getRootObject(),
                                    SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
  {
    SalomeApp_DataObject* obj = dynamic_cast<SalomeApp_DataObject*>( it.current() );
    if ( obj )
      entires.insert( obj->entry(), obj );
  }
}
