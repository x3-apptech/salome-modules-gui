// File:      QtxResourceEdit.cxx
// Author:    Sergey TELKOV

#include "QtxResourceEdit.h"

#include "QtxResourceMgr.h"

/*
  Class: QtxResourceEdit
  Descr: Class for managing preferences items
*/

QtxResourceEdit::QtxResourceEdit( QtxResourceMgr* mgr )
: myResMgr( mgr )
{
}

QtxResourceEdit::~QtxResourceEdit()
{
  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end(); ++it )
    delete it.data();
}

QtxResourceMgr* QtxResourceEdit::resourceMgr() const
{
  return myResMgr;
}

int QtxResourceEdit::addItem( const QString& label, const int pId, const int type,
                              const QString& section, const QString& param )
{
  Item* i = createItem( label, type, pId );
  if ( !i )
    return -1;

  myItems.insert( i->id(), i );

  i->setTitle( label );
  i->setResource( section, param );

  return i->id();
}

QVariant QtxResourceEdit::property( const int id, const QString& propName ) const
{
  QVariant propValue;
  Item* i = item( id );
  if ( i )
    propValue = i->property( propName );
  return propValue;
}

void QtxResourceEdit::setProperty( const int id, const QString& propName, const QVariant& propValue )
{
  Item* i = item( id );
  if ( i )
    i->setProperty( propName, propValue );
}

void QtxResourceEdit::resource( const int id, QString& sec, QString& param ) const
{
  Item* i = item( id );
  if ( i )
    i->resource( sec, param );
}

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

void QtxResourceEdit::retrieve()
{
  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end(); ++it )
    it.data()->retrieve();
}

void QtxResourceEdit::toBackup()
{
  myBackup.clear();
  resourceValues( myBackup );
}

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

void QtxResourceEdit::update()
{
}

QtxResourceEdit::Item* QtxResourceEdit::item( const int id ) const
{
  Item* i = 0;
  if ( myItems.contains( id ) )
    i = myItems[id];
  return i;
}

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

QtxResourceEdit::Item* QtxResourceEdit::createItem( const QString& label, const int type, const int pId )
{
  Item* i = 0;
  if ( pId < 0 )
    i = createItem( label, type );
  else
  {
    Item* pItem = item( pId );
    if ( pItem )
      i = pItem->createItem( label, type );
  }

  return i;
}

void QtxResourceEdit::removeItem( Item* item )
{
  if ( !item )
    return;

  myItems.remove( item->id() );
}

void QtxResourceEdit::resourceValues( QMap<int, QString>& map ) const
{
  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end(); ++it )
    map.insert( it.key(), it.data()->resourceValue() );
}

void QtxResourceEdit::resourceValues( QMap<Item*, QString>& map ) const
{
  for ( ItemMap::ConstIterator it = myItems.begin(); it != myItems.end(); ++it )
    map.insert( it.data(), it.data()->resourceValue() );
}

void QtxResourceEdit::setResourceValues( QMap<int, QString>& map ) const
{
  for ( QMap<int, QString>::ConstIterator it = map.begin(); it != map.end(); ++it )
  {
    Item* i = item( it.key() );
    if ( i )
      i->setResourceValue( it.data() );
  }
}

void QtxResourceEdit::setResourceValues( QMap<Item*, QString>& map ) const
{
  for ( QMap<Item*, QString>::ConstIterator it = map.begin(); it != map.end(); ++it )
    it.key()->setResourceValue( it.data() );
}


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

void QtxResourceEdit::changedResources( const QMap<Item*, QString>& )
{
}

/*
  Class: QtxResourceEdit::Item
  Descr: Class for incapsulation of one preference item
*/

QtxResourceEdit::Item::Item( QtxResourceEdit* edit, Item* parent )
: myEdit( edit ),
myParent( parent )
{
  myId = generateId();
}

QtxResourceEdit::Item::~Item()
{
  if ( resourceEdit() )
    resourceEdit()->removeItem( this );
}

int QtxResourceEdit::Item::id() const
{
  return myId;
}

QtxResourceEdit::Item* QtxResourceEdit::Item::parentItem() const
{
  return myParent;
}

void QtxResourceEdit::Item::childItems( QPtrList<Item>& lst ) const
{
  for ( ItemListIterator it( myChildren ); it.current(); ++it )
    lst.append( it.current() );
}

QString QtxResourceEdit::Item::title() const
{
  return myTitle;
}

void QtxResourceEdit::Item::resource( QString& sec, QString& param ) const
{
  sec = myResSection;
  param = myResParameter;
}

void QtxResourceEdit::Item::setTitle( const QString& title )
{
  myTitle = title;
}

void QtxResourceEdit::Item::setResource( const QString& sec, const QString& param )
{
  myResSection = sec;
  myResParameter = param;
}

void QtxResourceEdit::Item::update()
{
}

QVariant QtxResourceEdit::Item::property( const QString& ) const
{
  return QVariant();
}

void QtxResourceEdit::Item::setProperty( const QString&, const QVariant& )
{
}

QString QtxResourceEdit::Item::resourceValue() const
{
  return getString();
}

void QtxResourceEdit::Item::setResourceValue( const QString& val )
{
  setString( val );
}

QtxResourceMgr* QtxResourceEdit::Item::resourceMgr() const
{
  QtxResourceMgr* resMgr = 0;
  if ( resourceEdit() )
    resMgr = resourceEdit()->resourceMgr();
  return resMgr;
}

QtxResourceEdit* QtxResourceEdit::Item::resourceEdit() const
{
  return myEdit;
}

int QtxResourceEdit::Item::getInteger( const int val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->integerValue( myResSection, myResParameter, val ) : val;
}

double QtxResourceEdit::Item::getDouble( const double val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->doubleValue( myResSection, myResParameter, val ) : val;
}

bool QtxResourceEdit::Item::getBoolean( const bool val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->booleanValue( myResSection, myResParameter, val ) : val;
}

QString QtxResourceEdit::Item::getString( const QString& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->stringValue( myResSection, myResParameter, val ) : val;
}

QColor QtxResourceEdit::Item::getColor( const QColor& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->colorValue( myResSection, myResParameter, val ) : val;
}

QFont QtxResourceEdit::Item::getFont( const QFont& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->fontValue( myResSection, myResParameter, val ) : val;
}

void QtxResourceEdit::Item::setInteger( const int val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

void QtxResourceEdit::Item::setDouble( const double val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

void QtxResourceEdit::Item::setBoolean( const bool val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

void QtxResourceEdit::Item::setString( const QString& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

void QtxResourceEdit::Item::setColor( const QColor& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

void QtxResourceEdit::Item::setFont( const QFont& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( myResSection, myResParameter, val );
}

QtxResourceEdit::Item* QtxResourceEdit::Item::item( const int id ) const
{
  return resourceEdit() ? resourceEdit()->item( id ) : 0;
}

QtxResourceEdit::Item* QtxResourceEdit::Item::item( const QString& title ) const
{
  return resourceEdit() ? resourceEdit()->item( title ) : 0;
}

QtxResourceEdit::Item* QtxResourceEdit::Item::item( const QString& title, const int id ) const
{
  return resourceEdit() ? resourceEdit()->item( title, id ) : 0;
}

int QtxResourceEdit::Item::generateId()
{
  static int _id = 0;
  return _id++;
}
