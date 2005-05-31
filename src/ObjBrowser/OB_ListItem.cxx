#include "OB_ListItem.h"

#include "OB_ListView.h"

#include <SUIT_DataObject.h>

#include <qpainter.h>
#include <qwmatrix.h>

/*!
    Class: ListItem
    Descr: base template class
*/
template<class T>
ListItem<T>::ListItem( SUIT_DataObject* obj, QListView* parent )
: T( parent ),
myObject( obj )
{
  update();
}

template<class T>
ListItem<T>::ListItem( SUIT_DataObject* obj, QListViewItem* parent )
: T( parent ),
myObject( obj )
{
  update();
}

template<class T>
ListItem<T>::ListItem( SUIT_DataObject* obj, QListView* parent, QListViewItem* after )
: T( parent, after ),
myObject( obj )
{
  update();
}

template<class T>
ListItem<T>::ListItem( SUIT_DataObject* obj, QListViewItem* parent, QListViewItem* after )
: T( parent, after ),
myObject( obj )
{
  update();
}

template<class T>
ListItem<T>::ListItem( SUIT_DataObject* obj, QListView* parent, int type )
: T( parent, "", (typename T::Type)type ),
myObject( obj )
{
  update();
}

template<class T>
ListItem<T>::ListItem( SUIT_DataObject* obj, QListViewItem* parent, int type )
: T( parent, "", (typename T::Type)type ),
myObject( obj )
{
  update();
}

template<class T>
ListItem<T>::ListItem( SUIT_DataObject* obj, QListView* parent, QListViewItem* after, int type )
#if defined(QT_VERSION) && QT_VERSION >= 0x030101
: T( parent, after, "", (typename T::Type)type ),
#else
: T( parent, "", (typename T::Type)type ),
#endif
myObject( obj )
{
  update();
}

template<class T>
ListItem<T>::ListItem( SUIT_DataObject* obj, QListViewItem* parent, QListViewItem* after, int type )
#if defined(QT_VERSION) && QT_VERSION >= 0x030101
: T( parent, after, "", (typename T::Type)type ),
#else
: T( parent, "", (typename T::Type)type ),
#endif
myObject( obj )
{
  update();
}

template<class T>
void ListItem<T>::paintCell( QPainter* p, const QColorGroup& cg, int c, int w, int align )
{ 
  QColorGroup colorGrp( cg );
  if ( myObject )
  {
    if ( myObject->color( SUIT_DataObject::Text ).isValid() )
      colorGrp.setColor( QColorGroup::Text, myObject->color( SUIT_DataObject::Text ) );
    if ( myObject->color( SUIT_DataObject::Base ).isValid() )
      colorGrp.setColor( QColorGroup::Base, myObject->color( SUIT_DataObject::Base ) );
    if ( myObject->color( SUIT_DataObject::Foreground ).isValid() )
      colorGrp.setColor( QColorGroup::Foreground, myObject->color( SUIT_DataObject::Foreground ) );
    if ( myObject->color( SUIT_DataObject::Background ).isValid() )
      colorGrp.setColor( QColorGroup::Background, myObject->color( SUIT_DataObject::Background ) );
    if ( myObject->color( SUIT_DataObject::Highlight ).isValid() )
      colorGrp.setColor( QColorGroup::Highlight, myObject->color( SUIT_DataObject::Highlight ) );
  }

  
  p->fillRect( 0, 0, w, this->height(), colorGrp.brush( QColorGroup::Base ) );
  int itemW = width( p->fontMetrics(), this->listView(), c );
    
  T::paintCell( p, colorGrp, c, itemW,  align );
}

template<class T>
void ListItem<T>::paintFocus( QPainter* p, const QColorGroup& cg, const QRect& r )
{
  QRect rect = r;
  rect.setWidth( width( p->fontMetrics(), this->listView(), 0 ) );
  T::paintFocus( p, cg, rect );
}

template<class T>
void ListItem<T>::setSelected( bool s )
{
  if ( s && listView() && listView()->inherits( "OB_ListView" ) )
  {
    OB_ListView* oblv = (OB_ListView*)listView();
    s = s && oblv->isOk( this );
  }

  QListViewItem::setSelected( s );
}

template<class T>
void ListItem<T>::update()
{
  SUIT_DataObject* obj = dataObject();
  if ( !obj )
    return;

  setText( 0, obj->name() );

  int aIconW = obj->icon().width();
  if ( aIconW > 0 )
  {
    if ( aIconW > 20 )
    {
      QWMatrix aM;
      double aScale = 20.0 / aIconW;
      aM.scale( aScale, aScale );
      setPixmap( 0, obj->icon().xForm( aM ) );
    }
    else
      setPixmap( 0, obj->icon() );
  }

  this->setDragEnabled( obj->isDragable() );
  this->setDropEnabled( true );
}

/*!
    Class: OB_ListItem
    Descr: List view item for OB_Browser.
*/

OB_ListItem::OB_ListItem( SUIT_DataObject* obj, QListView* parent )
: ListItem<QListViewItem>( obj, parent )
{
}

OB_ListItem::OB_ListItem( SUIT_DataObject* obj, QListViewItem* parent )
: ListItem<QListViewItem>( obj, parent )
{
}

OB_ListItem::OB_ListItem( SUIT_DataObject* obj, QListView* parent, QListViewItem* after )
: ListItem<QListViewItem>( obj, parent, after )
{
}

OB_ListItem::OB_ListItem( SUIT_DataObject* obj, QListViewItem* parent, QListViewItem* after )
: ListItem<QListViewItem>( obj, parent, after )
{
}

OB_ListItem::~OB_ListItem()
{
}

int OB_ListItem::RTTI()
{
  return 1000;
}

int OB_ListItem::rtti() const
{
  return RTTI();
}

/*!
    Class: OB_CheckListItem
    Descr: Check list view item for OB_Browser.
*/

OB_CheckListItem::OB_CheckListItem( SUIT_DataObject* obj, QListView* parent, Type type )
: ListItem<QCheckListItem>( obj, parent, type )
{
}

OB_CheckListItem::OB_CheckListItem( SUIT_DataObject* obj, QListViewItem* parent, Type type )
: ListItem<QCheckListItem>( obj, parent, type )
{
}

OB_CheckListItem::OB_CheckListItem( SUIT_DataObject* obj, QListView* parent, QListViewItem* after, Type type )
: ListItem<QCheckListItem>( obj, parent, after, type )
{
}

OB_CheckListItem::OB_CheckListItem( SUIT_DataObject* obj, QListViewItem* parent, QListViewItem* after, Type type )
: ListItem<QCheckListItem>( obj, parent, after, type )
{
}

OB_CheckListItem::~OB_CheckListItem()
{
}

int OB_CheckListItem::RTTI()
{
  return OB_ListItem::RTTI() + 1;
}

int OB_CheckListItem::rtti() const
{
  return RTTI();
}

void OB_CheckListItem::stateChange( bool on )
{
  QCheckListItem::stateChange( on );

  if ( dataObject() )
    dataObject()->setOn( on );
}
