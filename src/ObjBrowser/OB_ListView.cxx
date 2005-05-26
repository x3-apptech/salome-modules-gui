#include "OB_ListView.h"

#include "OB_Filter.h"
#include "OB_ListItem.h"

#include <SUIT_DataObject.h>

#include <qdragobject.h>

OB_ListView::OB_ListView( QWidget* parent, const char* name, WFlags f )
: QtxListView( parent, name, f ),
myFilter( 0 )
{
}

OB_ListView::OB_ListView( const int state, QWidget* parent, const char* name, WFlags f )
: QtxListView( state, parent, name, f ),
myFilter( 0 )
{
}

OB_ListView::~OB_ListView()
{
  delete myFilter;
}

OB_Filter* OB_ListView::filter() const
{
  return myFilter;
}

void OB_ListView::setFilter( OB_Filter* f )
{
  if ( myFilter == f )
    return;

  delete myFilter;
  myFilter = f;
}

void OB_ListView::setSelected( QListViewItem* item, bool sel )
{
  bool on = sel;
  if ( filter() )
  {
    SUIT_DataObject* obj = dataObject( item );
    if ( obj )
      on = on && filter()->isOk( obj );
  }

  QListView::setSelected( item, on );
}

int OB_ListView::addColumn( const QString& label, int width )
{
  int res = QListView::addColumn( label, width );

  updateHeader();

  return res;
}

int OB_ListView::addColumn( const QIconSet& iconset, const QString& label, int width )
{
  int res = QListView::addColumn( iconset, label, width );

  updateHeader();

  return res;
}

QDragObject* OB_ListView::dragObject()
{
  myItems.clear();

  for ( QListViewItemIterator it( this ); it.current(); ++it )
    if ( it.current()->isSelected() )
      myItems.append( it.current() );

  return new QTextDrag( "", this );
}

void OB_ListView::dragEnterEvent( QDragEnterEvent* e )
{
  e->accept();
}

void OB_ListView::dragMoveEvent( QDragMoveEvent* e )
{
  QListViewItem* item = dropItem( e );

  if ( isDropAccepted( item ) )
  {
    setCurrentItem( item );
    e->accept( true );
  }
  else
    e->accept( false );
}

void OB_ListView::dropEvent( QDropEvent* e )
{
  QListViewItem* item = dropItem( e );
  if ( isDropAccepted( item ) )
  {
    e->accept();
    emit dropped( myItems, item, e->action() );
  }
  myItems.clear();
}

QListViewItem* OB_ListView::dropItem( QDropEvent* e ) const
{
  QListViewItem* item = 0;
  if ( e )
    item = itemAt( QPoint( e->pos().x() - viewport()->x(), e->pos().y() - viewport()->y() ) );

  return item;
}

SUIT_DataObject* OB_ListView::dataObject( QListViewItem* item ) const
{
  if ( !item )
    return 0;

  SUIT_DataObject* obj = 0;

  if ( dynamic_cast<OB_ListItem*>( item ) )
    obj = dynamic_cast<OB_ListItem*>( item )->dataObject();
  else if ( dynamic_cast<OB_CheckListItem*>( item ) )
    obj = dynamic_cast<OB_CheckListItem*>( item )->dataObject();

  return obj;
}

bool OB_ListView::isDropAccepted( QListViewItem* item ) const
{
  bool res = true;

  for ( QPtrListIterator<QListViewItem> it( myItems ); it.current() && res; ++it )
    res = res && isDropAccepted( it.current(), item );

  return res;
}

bool OB_ListView::isDropAccepted( QListViewItem* drag, QListViewItem* drop ) const
{
  SUIT_DataObject* dragObj = dataObject( drag );
  SUIT_DataObject* dropObj = dataObject( drop );

  if ( !dragObj || !dropObj )
    return false;

  return dropObj->isDropAccepted( dragObj );
}

void OB_ListView::updateHeader()
{

}
