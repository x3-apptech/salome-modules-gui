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
// File:      QtxResourceEdit.cxx
// Author:    Sergey TELKOV

#include "QtxResourceEdit.h"

#include "QtxResourceMgr.h"


/*!
  Constructor
*/
QtxResourceEdit::QtxResourceEdit( QtxResourceMgr* mgr )
: myResMgr( mgr )
{
}

/*!
  Destructor
*/
QtxResourceEdit::~QtxResourceEdit()
{
  ItemMap items;
  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end(); ++it )
    items.insert( it.key(), it.data() );

  for ( ItemMap::ConstIterator itr = items.begin(); itr != items.end(); ++itr )
    if ( myItems.contains( itr.key() ) )
      delete itr.data();
}

/*!
  \return assigned resource manager
*/
QtxResourceMgr* QtxResourceEdit::resourceMgr() const
{
  return myResMgr;
}

/*!
  Adds new item
  \param label - label of widget to edit preference
  \param pId - parent item id
  \param type - type of item
  \param section - section of resource assigned with item
  \param param - name of resource assigned with item
*/
int QtxResourceEdit::addItem( const QString& label, const int pId, const int type,
                              const QString& section, const QString& param )
{
  Item* i = createItem( label, type, pId );
  if ( !i )
    return -1;

  if ( !myItems.contains( i->id() ) )
  {
    myItems.insert( i->id(), i );

    i->setTitle( label );
    i->setResource( section, param );

    if ( !i->parentItem() && !myChildren.contains( i ) )
      myChildren.append( i );

    itemAdded( i );
  }

  return i->id();
}

/*!
  \return value of item property
  \param id - item id
  \propName - propertyName
*/
QVariant QtxResourceEdit::itemProperty( const int id, const QString& propName ) const
{
  QVariant propValue;
  Item* i = item( id );
  if ( i )
    propValue = i->property( propName );
  return propValue;
}

/*!
  Sets value of item property
  \param id - item id
  \propName - propertyName
  \propValue - new value of property
*/
void QtxResourceEdit::setItemProperty( const int id, const QString& propName, const QVariant& propValue )
{
  Item* i = item( id );
  if ( i )
    i->setProperty( propName, propValue );
}

/*!
  \return resource assigned with item
  \param id - item id
  \param section - to return section of resource
  \param param - to return name of resource
*/
void QtxResourceEdit::resource( const int id, QString& sec, QString& param ) const
{
  Item* i = item( id );
  if ( i )
    i->resource( sec, param );
}

/*!
  Stores all values to resource manager
*/
void QtxResourceEdit::store()
{
  QMap<Item*, QString> before;
  resourceValues( before );

  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end(); ++it )
    it.data()->store();

  QMap<Item*, QString> after;
  resourceValues( after );

  QMap<Item*, QString> changed;
  differentValues( before, after, changed );

  changedResources( changed );
}

/*!
  Retrieve all values from resource manager
*/
void QtxResourceEdit::retrieve()
{
  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end(); ++it )
    it.data()->retrieve();
}

/*!
  Stores all values to backup container
*/
void QtxResourceEdit::toBackup()
{
  myBackup.clear();
  resourceValues( myBackup );
}

/*!
  Retrieve all values from backup container
*/
void QtxResourceEdit::fromBackup()
{
  QMap<Item*, QString> before;
  resourceValues( before );

  setResourceValues( myBackup );

  QMap<Item*, QString> after;
  resourceValues( after );

  QMap<Item*, QString> changed;
  differentValues( before, after, changed );

  changedResources( changed );
}

/*!
  Updates resource edit (default implementation is empty)
*/
void QtxResourceEdit::update()
{
}

/*!
  \return item by it's id
  \param id - item id 
*/
QtxResourceEdit::Item* QtxResourceEdit::item( const int id ) const
{
  Item* i = 0;
  if ( myItems.contains( id ) )
    i = myItems[id];
  return i;
}

/*!
  \return item by it's title (finds first item)
  \param title - item title 
*/
QtxResourceEdit::Item* QtxResourceEdit::item( const QString& title ) const
{
  Item* i = 0;
  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end() && !i; ++it )
  {
    if ( it.data()->title() == title )
      i = it.data();
  }
  return i;
}

/*!
  \return item by it's title and parent id
  \param title - item title 
  \param pId - parent id
*/
QtxResourceEdit::Item* QtxResourceEdit::item( const QString& title, const int pId ) const
{
  Item* i = 0;
  Item* pItem = item( pId );
  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end() && !i; ++it )
  {
    if ( it.data()->parentItem() == pItem && it.data()->title() == title )
      i = it.data();
  }
  return i;
}

/*!
  Creates item
  \return new item
  \param label - text of label for new item
  \param type - type of new item
  \param pId - parent id
*/
QtxResourceEdit::Item* QtxResourceEdit::createItem( const QString& label, const int type, const int pId )
{
  Item* i = 0;
  if ( pId < 0 )
    i = createItem( label, type );
  else
  {
    Item* pItem = item( pId );
    if ( pItem )
    {
      i = pItem->createItem( label, type );
      pItem->insertChild( i );
    }
  }

  return i;
}

/*!
  Removes item
  \param item - item to be removed
*/
void QtxResourceEdit::removeItem( Item* item )
{
  if ( !item )
    return;

  myChildren.remove( item );
  myItems.remove( item->id() );

  itemRemoved( item );
}

/*!
  \return children items of resource edit
  \param lst - list of items to be filled with children
*/
void QtxResourceEdit::childItems( QPtrList<Item>& lst ) const
{
  lst.clear();
  for ( QPtrListIterator<Item> it( myChildren ); it.current(); ++it )
    lst.append( it.current() );
}

/*!
  \return all resources values from widgets
  \param map - map to be filled by resources values
*/
void QtxResourceEdit::resourceValues( QMap<int, QString>& map ) const
{
  QString sect, name;
  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end(); ++it )
  {
    it.data()->resource( sect, name );
    if( myResMgr->hasValue( sect, name ) )
      map.insert( it.key(), it.data()->resourceValue() );
  }
}

/*!
  \return all resources values from widgets
  \param map - map to be filled by resources values
*/
void QtxResourceEdit::resourceValues( QMap<Item*, QString>& map ) const
{
  QString sect, name;
  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end(); ++it )
  {
    it.data()->resource( sect, name );
    if( myResMgr->hasValue( sect, name ) )
      map.insert( it.data(), it.data()->resourceValue() );
  }
}

/*!
  Sets to widgets all resources values from map
  \param map - map with resources values
*/
void QtxResourceEdit::setResourceValues( QMap<int, QString>& map ) const
{
  for ( QMap<int, QString>::ConstIterator it = map.begin(); it != map.end(); ++it )
  {
    Item* i = item( it.key() );
    if ( i )
      i->setResourceValue( it.data() );
  }
}

/*!
  Sets to widgets all resources values from map
  \param map - map with resources values
*/
void QtxResourceEdit::setResourceValues( QMap<Item*, QString>& map ) const
{
  for ( QMap<Item*, QString>::ConstIterator it = map.begin(); it != map.end(); ++it )
    it.key()->setResourceValue( it.data() );
}

/*!
   Compares two map of resources values and finds different ones
   \param map1 - first map
   \param map2 - second map
   \param resMap - map to be filled with different values
   \param fromFirst - if it is true, then resMap will be filled with values from first map, otherwise - from second
*/
void QtxResourceEdit::differentValues( const QMap<int, QString>& map1, const QMap<int, QString>& map2,
                                       QMap<int, QString>& resMap, const bool fromFirst ) const
{
  resMap.clear();
  const QMap<int, QString>& later = fromFirst ? map1 : map2;
  const QMap<int, QString>& early = fromFirst ? map2 : map1;

  for ( QMap<int, QString>::ConstIterator it = later.begin(); it != later.end(); ++it )
  {
    if ( !early.contains( it.key() ) || early[it.key()] != it.data() )
      resMap.insert( it.key(), it.data() );
  }
}

/*!
   Compares two map of resources values and finds different ones
   \param map1 - first map
   \param map2 - second map
   \param resMap - map to be filled with different values
   \param fromFirst - if it is true, then resMap will be filled with values from first map, otherwise - from second
*/
void QtxResourceEdit::differentValues( const QMap<Item*, QString>& map1, const QMap<Item*, QString>& map2,
                                       QMap<Item*, QString>& resMap, const bool fromFirst ) const
{
  resMap.clear();
  const QMap<Item*, QString>& later = fromFirst ? map1 : map2;
  const QMap<Item*, QString>& early = fromFirst ? map2 : map1;

  for ( QMap<Item*, QString>::ConstIterator it = later.begin(); it != later.end(); ++it )
  {
    if ( !early.contains( it.key() ) || early[it.key()] != it.data() )
      resMap.insert( it.key(), it.data() );
  }
}

/*!
  Makes some activity on resource changing (called from store() method)
  \sa store()
*/
void QtxResourceEdit::changedResources( const QMap<Item*, QString>& )
{
}

/*!
  Some activity on item addition (default implementation is empty)
*/
void QtxResourceEdit::itemAdded( Item* )
{
}

/*!
  Some activity on item removing (default implementation is empty)
*/
void QtxResourceEdit::itemRemoved( Item* )
{
}

/*!
  Constructor
*/
QtxResourceEdit::Item::Item( QtxResourceEdit* edit, Item* parent )
: myEdit( edit ),
myParent( 0 )
{
  myId = generateId();

  if ( parent )
    parent->insertChild( this );
}

/*!
  Destructor
*/
QtxResourceEdit::Item::~Item()
{
  if ( resourceEdit() )
    resourceEdit()->removeItem( this );
}

/*!
  \return id of item
*/
int QtxResourceEdit::Item::id() const
{
  return myId;
}

/*!
  \return parent item 
*/
QtxResourceEdit::Item* QtxResourceEdit::Item::parentItem() const
{
  return myParent;
}

/*!
  Appends child and (if necessary) removes item from old parent
  \param item - item to be added
*/
void QtxResourceEdit::Item::insertChild( Item* item )
{
  if ( !item || myChildren.contains( item ) )
    return;

  if ( item->parentItem() && item->parentItem() != this )
    item->parentItem()->removeChild( item );

  item->myParent = this;
  myChildren.append( item );
}

/*!
  Removes child
  \param item - item to be removed
*/
void QtxResourceEdit::Item::removeChild( Item* item )
{
  if ( !item || !myChildren.contains( item ) )
    return;

  myChildren.remove( item );
  item->myParent = 0;
}

/*!
  Fills list with children items
  \param lst - list to be filled with
*/
void QtxResourceEdit::Item::childItems( QPtrList<Item>& lst ) const
{
  for ( ItemListIterator it( myChildren ); it.current(); ++it )
    lst.append( it.current() );
}

/*!
  \return true if there is no children of this item
*/
bool QtxResourceEdit::Item::isEmpty() const
{
  return myChildren.isEmpty();
}

/*!
  \return title of item
*/
QString QtxResourceEdit::Item::title() const
{
  return myTitle;
}

/*!
  \return assigned resource placement
  \param sec - to return section
  \param param - to return param name
*/
void QtxResourceEdit::Item::resource( QString& sec, QString& param ) const
{
  sec = myResSection;
  param = myResParameter;
}

/*!
  Sets item title 
  \param title - new item title
*/
void QtxResourceEdit::Item::setTitle( const QString& title )
{
  myTitle = title;
}

/*!
  Assigns new resource to item
  \param sec - section
  \param sec - param name
*/
void QtxResourceEdit::Item::setResource( const QString& sec, const QString& param )
{
  myResSection = sec;
  myResParameter = param;
}

/*!
  Updates item (default implementation is empty)
*/
void QtxResourceEdit::Item::update()
{
}

/*!
  \return property value
*/
QVariant QtxResourceEdit::Item::property( const QString& ) const
{
  return QVariant();
}

/*!
  Sets property value
*/
void QtxResourceEdit::Item::setProperty( const QString&, const QVariant& )
{
}

/*!
  \return value of assigned resource
*/
QString QtxResourceEdit::Item::resourceValue() const
{
  return getString();
}

/*!
  Sets value of assigned resource
  \param val - new value
*/
void QtxResourceEdit::Item::setResourceValue( const QString& val )
{
  setString( val );
}

/*!
  \return corresponding resource manager
*/
QtxResourceMgr* QtxResourceEdit::Item::resourceMgr() const
{
  QtxResourceMgr* resMgr = 0;
  if ( resourceEdit() )
    resMgr = resourceEdit()->resourceMgr();
  return resMgr;
}

/*!
  \return corresponding resource edit
*/
QtxResourceEdit* QtxResourceEdit::Item::resourceEdit() const
{
  return myEdit;
}

/*!
  \return integer value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
int QtxResourceEdit::Item::getInteger( const int val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->integerValue( myResSection, myResParameter, val ) : val;
}

/*!
  \return double value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
double QtxResourceEdit::Item::getDouble( const double val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->doubleValue( myResSection, myResParameter, val ) : val;
}

/*!
  \return boolean value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
bool QtxResourceEdit::Item::getBoolean( const bool val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->booleanValue( myResSection, myResParameter, val ) : val;
}

/*!
  \return string value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
QString QtxResourceEdit::Item::getString( const QString& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->stringValue( myResSection, myResParameter, val ) : val;
}

/*!
  \return color value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
QColor QtxResourceEdit::Item::getColor( const QColor& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->colorValue( myResSection, myResParameter, val ) : val;
}

/*!
  \return font value of resource corresponding to item
  \param val - default value (it is returned if there is no such resource)
*/
QFont QtxResourceEdit::Item::getFont( const QFont& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->fontValue( myResSection, myResParameter, val ) : val;
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxResourceEdit::Item::setInteger( const int val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxResourceEdit::Item::setDouble( const double val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxResourceEdit::Item::setBoolean( const bool val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxResourceEdit::Item::setString( const QString& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxResourceEdit::Item::setColor( const QColor& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

/*!
  Sets value of resource
  \param val - value
*/
void QtxResourceEdit::Item::setFont( const QFont& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

/*!
  \return other item
  \param id - other item id
*/
QtxResourceEdit::Item* QtxResourceEdit::Item::item( const int id ) const
{
  return resourceEdit() ? resourceEdit()->item( id ) : 0;
}

/*!
  \return other item
  \param title - other item title
*/
QtxResourceEdit::Item* QtxResourceEdit::Item::item( const QString& title ) const
{
  return resourceEdit() ? resourceEdit()->item( title ) : 0;
}

/*!
  \return other item
  \param title - other item title
  \param id - parent item id
*/
QtxResourceEdit::Item* QtxResourceEdit::Item::item( const QString& title, const int id ) const
{
  return resourceEdit() ? resourceEdit()->item( title, id ) : 0;
}

/*!
  \return free item id
*/
int QtxResourceEdit::Item::generateId()
{
  static int _id = 0;
  return _id++;
}
