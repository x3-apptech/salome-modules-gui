#include "SalomeApp_OBSelector.h"

#include "SalomeApp_DataOwner.h"
#include "SalomeApp_DataObject.h"

#include <OB_Browser.h>

#include <SUIT_DataObjectIterator.h>

SalomeApp_OBSelector::SalomeApp_OBSelector( OB_Browser* ob, SUIT_SelectionMgr* mgr )
: QObject( 0 ),
SUIT_Selector( mgr ),
myBrowser( ob )
{
  if ( myBrowser )
    connect( myBrowser, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
}

SalomeApp_OBSelector::~SalomeApp_OBSelector()
{
}

OB_Browser* SalomeApp_OBSelector::browser() const
{
  return myBrowser;
}

void SalomeApp_OBSelector::getSelection( SUIT_DataOwnerPtrList& list ) const
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
      list.append( SUIT_DataOwnerPtr( new SalomeApp_DataOwner( aSObj  ) ) );
    }
  }
}

void SalomeApp_OBSelector::setSelection( const SUIT_DataOwnerPtrList& list )
{
  if ( !myBrowser )
    return;

  QMap<QString, SalomeApp_DataObject*> map;
  fillEntries( map );

  DataObjectList objList;
  for ( SUIT_DataOwnerPtrList::const_iterator it = list.begin(); it != list.end(); ++it )
  {
    const SalomeApp_DataOwner* owner = dynamic_cast<const SalomeApp_DataOwner*>( (*it).operator->() );
    if ( owner && map.contains( owner->entry() ) )
      objList.append( map[owner->entry()] );
  }

  myBrowser->setSelected( objList );
}

void SalomeApp_OBSelector::onSelectionChanged()
{
  selectionChanged();
}

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
