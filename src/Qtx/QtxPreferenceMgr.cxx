// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxPreferenceMgr.cxx
// Author:    Sergey TELKOV
//
#include "QtxPreferenceMgr.h"

#include "QtxResourceMgr.h"

#include <QEvent>
#include <QApplication>

/*!
  \class QtxPreferenceItem::Updater
  \brief Preference item updater.
  \internal
*/

class QtxPreferenceItem::Updater : public QObject
{
  Updater();
public:
  ~Updater();

  static Updater*           instance();

  void                      updateItem( QtxPreferenceItem* );
  void                      removeItem( QtxPreferenceItem* );

protected:
  virtual void              customEvent( QEvent* );

private:
  QList<QtxPreferenceItem*> myItems;
  static Updater*           _Updater;
};

QtxPreferenceItem::Updater* QtxPreferenceItem::Updater::_Updater = 0;

/*!
  \brief Constructor.
  \internal
*/
QtxPreferenceItem::Updater::Updater()
{
}

/*!
  \brief Destructor.
  \internal
*/
QtxPreferenceItem::Updater::~Updater()
{
}

/*!
  \brief Get the only updater instance.
  \internal
  \return the only updater instance
*/
QtxPreferenceItem::Updater* QtxPreferenceItem::Updater::instance()
{
  if ( !_Updater )
    _Updater = new Updater();
  return _Updater;
}

/*!
  \brief Update the preference item.
  \internal
  \param item preference item to be updated
*/
void QtxPreferenceItem::Updater::updateItem( QtxPreferenceItem* item )
{
  if ( !item || myItems.contains( item ) )
    return;

  myItems.append( item );
  QApplication::postEvent( this, new QEvent( QEvent::User ) );
}

/*!
  \brief Called when preference item is removed.
  \internal
  \param item preference item being removed
*/
void QtxPreferenceItem::Updater::removeItem( QtxPreferenceItem* item )
{
  myItems.removeAll( item );
}

/*!
  \brief Custom events provessing. Updates all the items.
  \internal
  \param e custom event (not used)
*/
void QtxPreferenceItem::Updater::customEvent( QEvent* /*e*/ )
{
  QList<QtxPreferenceItem*> lst = myItems;
  for ( QList<QtxPreferenceItem*>::const_iterator it = lst.begin(); it != lst.end(); ++it )
    (*it)->updateContents();
}

/*!
  \class QtxPreferenceItem
  \brief Base class for implementing of all the preference items.

  To implement any specific preference item, cubclass from the
  QtxPreferenceItem and redefine store() and retrieve() methods.
*/

/*!
  \brief Constructor.
  \param parent parent preference item
*/
QtxPreferenceItem::QtxPreferenceItem( QtxPreferenceItem* parent )
: myParent( 0 ),
  myEval( true ),
  myRestartNeeded( false )
{
  myId = generateId();

  if ( parent )
    parent->insertItem( this );
}

/*!
  \brief Constructor.
  \param title item title
  \param parent parent preference item
*/
QtxPreferenceItem::QtxPreferenceItem( const QString& title, QtxPreferenceItem* parent )
: myParent( 0 ),
  myEval( true ),
  myRestartNeeded( false ),
  myTitle( title )
{
  myId = generateId();

  if ( parent )
    parent->insertItem( this );
}

/*!
  \brief Constructor.
  \param title item title
  \param sect resource file section to be associated with the item
  \param param resource file parameter to be associated with the item
  \param parent parent preference item
*/
QtxPreferenceItem::QtxPreferenceItem( const QString& title, const QString& sect,
                                      const QString& param, QtxPreferenceItem* parent )
: myParent( 0 ),
  myEval( true ),
  myRestartNeeded( false ),
  myTitle( title ),
  mySection( sect ),
  myParameter( param )
{
  myId = generateId();

  if ( parent )
    parent->insertItem( this );
}

/*!
  \brief Destructor.
*/
QtxPreferenceItem::~QtxPreferenceItem()
{
  ItemList list = myChildren;
  myChildren.clear();
  qDeleteAll( list );

  if ( myParent )
    myParent->removeItem( this );

  Updater::instance()->removeItem( this );
}

/*!
  \brief Get unique item identifier.
  \return item ID
*/
int QtxPreferenceItem::id() const
{
  return myId;
}

/*!
  \brief Get unique item type identifier.
  \return item type ID
*/
int QtxPreferenceItem::rtti() const
{
  return QtxPreferenceItem::RTTI();
}

/*!
  \brief Specify unique item class identifier.
  \return item class ID
*/
int QtxPreferenceItem::RTTI()
{
  return 1;
}

/*!
  \brief Get root preference item.
  \return root item
*/
QtxPreferenceItem* QtxPreferenceItem::rootItem() const
{
  QtxPreferenceItem* item = (QtxPreferenceItem*)this;
  while ( item->parentItem() )
    item = item->parentItem();
  return item;
}

/*!
  \brief Get parent preference item.
  \return parent item
*/
QtxPreferenceItem* QtxPreferenceItem::parentItem() const
{
  return myParent;
}

/*!
  \brief Append child preference item.

  Removes (if necessary) the item from the previous parent.

  \param item item to be added
  \sa removeItem()
*/
void QtxPreferenceItem::appendItem( QtxPreferenceItem* item )
{
  insertItem( item, 0 );
}

/*!
  \brief Insert child preference item before specified item.
  If the before item is 0 then new item is appended.

  Removes (if necessary) the item from the previous parent.

  \param item item to be added
  \param before item before which is inserted new \aitem
  \sa removeItem()
*/
void QtxPreferenceItem::insertItem( QtxPreferenceItem* item, QtxPreferenceItem* before )
{
  if ( !item )
    return;

  if ( myChildren.contains( item ) && item == before )
    return;

  if ( myChildren.contains( item ) )
    myChildren.removeAll( item );

  int idx = myChildren.indexOf( before );
  if ( idx < 0 )
    idx = myChildren.count();

  if ( item->parentItem() && item->parentItem() != this )
    item->parentItem()->removeItem( item );

  item->myParent = this;
  myChildren.insert( idx, item );

  itemAdded( item );
}

/*!
  \brief Remove child preference item.
  \param item item to be removed
  \sa insertItem()
*/
void QtxPreferenceItem::removeItem( QtxPreferenceItem* item )
{
  if ( !item || !myChildren.contains( item ) )
    return;

  item->myParent = 0;
  myChildren.removeAll( item );

  itemRemoved( item );
}

/*!
  \brief Get all child preference items.
  \param rec recursion boolean flag
  \return list of child items
*/
QList<QtxPreferenceItem*> QtxPreferenceItem::childItems( const bool rec ) const
{
  QList<QtxPreferenceItem*> lst = myChildren;
  if ( rec )
  {
    for ( ItemList::const_iterator it = myChildren.begin(); it != myChildren.end(); ++it )
      lst += (*it)->childItems( rec );
  }

  return lst;
}

/*!
  \brief Get preference item depth.
  \return item depth
*/
int QtxPreferenceItem::depth() const
{
  return parentItem() ? parentItem()->depth() + 1 : 0;
}

/*!
  \brief Get child preference items number.
  \return number of child items
  \sa isEmpty()
*/
int QtxPreferenceItem::count() const
{
  return myChildren.count();
}

/*!
  \brief Check if the item has children.
  \return \c true if item does not have children
  \sa count()
*/
bool QtxPreferenceItem::isEmpty() const
{
  return myChildren.isEmpty();
}

/*!
  \brief Get preference item icon.
  \return item icon
  \sa setIcon()
*/
QIcon QtxPreferenceItem::icon() const
{
  return myIcon;
}

/*!
  \brief Get preference item title.
  \return item title
  \sa setTitle()
*/
QString QtxPreferenceItem::title() const
{
  return myTitle;
}

/*!
  \brief Get resource file settings associated to the preference item.
  \param sec used to return resource file section name
  \param param used to return resource file parameter name
  \sa setResource()
*/
void QtxPreferenceItem::resource( QString& sec, QString& param ) const
{
  sec = mySection;
  param = myParameter;
}

/*!
  \brief Set prefence item icon.
  \param ico new item icon
  \sa icon()
*/
void QtxPreferenceItem::setIcon( const QIcon& ico )
{
  if ( myIcon.cacheKey() == ico.cacheKey() )
    return;

  myIcon = ico;
  sendItemChanges();
}

/*!
  \brief Set preference item title .
  \param title new item title
  \sa title()
*/
void QtxPreferenceItem::setTitle( const QString& title )
{
  if ( myTitle == title )
    return;

  myTitle = title;
  sendItemChanges();
}

/*!
  \brief Assign resource file settings to the preference item.
  \param sec resource file section name
  \param param resource file parameter name
  \sa resource()
*/
void QtxPreferenceItem::setResource( const QString& sec, const QString& param )
{
  mySection = sec;
  myParameter = param;
}

/*!
  \brief Update preference item.
*/
void QtxPreferenceItem::updateContents()
{
  Updater::instance()->removeItem( this );
}

/*!
  \brief Get preference item option value.
  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOption()
*/
QVariant QtxPreferenceItem::option( const QString& name ) const
{
  return optionValue( name.toLower() );
}

/*!
  \brief Set preference item option value.
  \param name option name
  \param val new property value
  \sa option()
*/
void QtxPreferenceItem::setOption( const QString& name, const QVariant& val )
{
  QVariant old = optionValue( name.toLower() );
  setOptionValue( name.toLower(), val );
  if ( old != optionValue( name.toLower() ) )
    sendItemChanges();
}

/*!
  \brief Get variables auto-conversion option value
  \return option value
*/
bool QtxPreferenceItem::isEvaluateValues() const
{
  return myEval;
}

/*!
  \brief Switch variables auto-conversion option on/off
  \param on option value
*/
void QtxPreferenceItem::setEvaluateValues( const bool on )
{
  myEval = on;
}

/*!
  \brief Get restart needed option value
  \return option value
*/
bool QtxPreferenceItem::isRestartRequired() const
{
  return myRestartNeeded;
}

/*!
  \brief Switch restart needed option on/off
  \param on option value
*/
void QtxPreferenceItem::setRestartRequired( const bool on )
{
  myRestartNeeded = on;
}

/*!
  \fn void QtxPreferenceItem::store();
  \brief Save preference item (for example, to the resource file).

  This method should be implemented in the subclasses.

  \sa retrieve()
*/

/*!
  \fn virtual void QtxPreferenceItem::retrieve();
  \brief Restore preference item (for example, from the resource file).

  This method should be implemented in the subclasses.

  \sa store()
*/

/*!
  \brief Get the value of the associated resource file setting.
  \return associated resource file setting value
  \sa setResourceValue()
*/
QString QtxPreferenceItem::resourceValue() const
{
  return getString();
}

/*!
  \brief Get the value of the associated resource file setting.
  \param val new associated resource file setting value
  \sa resourceValue()
*/
void QtxPreferenceItem::setResourceValue( const QString& val )
{
  setString( val );
}

/*!
  \brief Get the resources manager.
  \return resource manager pointer or 0 if it is not defined
*/
QtxResourceMgr* QtxPreferenceItem::resourceMgr() const
{
  QtxPreferenceMgr* mgr = preferenceMgr();
  return mgr ? mgr->resourceMgr() : 0;
}

/*!
  \brief Get the parent preferences manager.
  \return preferences manager or 0 if it is not defined
*/
QtxPreferenceMgr* QtxPreferenceItem::preferenceMgr() const
{
  return parentItem() ? parentItem()->preferenceMgr() : 0;
}

/*!
  \brief Find the item by the specified identifier.
  \param id child item ID
  \param rec if \c true recursive search is done
  \return child item or 0 if it is not found
*/
QtxPreferenceItem* QtxPreferenceItem::findItem( const int id, const bool rec ) const
{
  QtxPreferenceItem* item = 0;
  for ( ItemList::const_iterator it = myChildren.begin(); it != myChildren.end() && !item; ++it )
  {
    QtxPreferenceItem* i = *it;
    if ( i->id() == id )
      item = i;
    else if ( rec )
      item = i->findItem( id, rec );
  }
  return item;
}

/*!
  \brief Find the item by the specified title.
  \param title child item title
  \param rec if \c true recursive search is done
  \return child item or 0 if it is not found
*/
QtxPreferenceItem* QtxPreferenceItem::findItem( const QString& title, const bool rec ) const
{
  QtxPreferenceItem* item = 0;
  for ( ItemList::const_iterator it = myChildren.begin(); it != myChildren.end() && !item; ++it )
  {
    QtxPreferenceItem* i = *it;
    if ( i->title() == title )
      item = i;
    else if ( rec )
      item = i->findItem( title, rec );
  }
  return item;
}

/*!
  \brief Find the item by the specified title and identifier.
  \param title child item title
  \param id child item ID
  \param rec if \c true recursive search is done
  \return child item or 0 if it is not found
*/
QtxPreferenceItem* QtxPreferenceItem::findItem( const QString& title, const int id, const bool rec ) const
{
  QtxPreferenceItem* item = 0;
  for ( ItemList::const_iterator it = myChildren.begin(); it != myChildren.end() && !item; ++it )
  {
    QtxPreferenceItem* i = *it;
    if ( i->title() == title && i->id() == id )
      item = i;
    else if ( rec )
      item = i->findItem( title, id, rec );
  }
  return item;
}

void QtxPreferenceItem::activate()
{
}

void QtxPreferenceItem::ensureVisible()
{
  if ( parentItem() )
    parentItem()->ensureVisible( this );
}

/*!
  \brief Get integer resources value corresponding to the item.
  \param val default value (returned if there is no such resource)
  \return integer value of the associated resource
  \sa setInteger()
*/
int QtxPreferenceItem::getInteger( const int val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->integerValue( mySection, myParameter, val ) : val;
}

/*!
  \brief Get double resources value corresponding to the item.
  \param val default value (returned if there is no such resource)
  \return double value of the associated resource
  \sa setDouble()
*/
double QtxPreferenceItem::getDouble( const double val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->doubleValue( mySection, myParameter, val ) : val;
}

/*!
  \brief Get boolean resources value corresponding to the item.
  \param val default value (returned if there is no such resource)
  \return boolean value of the associated resource
  \sa setBoolean()
*/
bool QtxPreferenceItem::getBoolean( const bool val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->booleanValue( mySection, myParameter, val ) : val;
}

/*!
  \brief Get string resources value corresponding to the item.
  \param val default value (returned if there is no such resource)
  \return string value of the associated resource
  \sa setString()
*/
QString QtxPreferenceItem::getString( const QString& val ) const
{
  QString res = val;
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->value( mySection, myParameter, res, isEvaluateValues() );
  return res;
}

/*!
  \brief Get color resources value corresponding to the item.
  \param val default value (returned if there is no such resource)
  \return color value of the associated resource
  \sa setColor()
*/
QColor QtxPreferenceItem::getColor( const QColor& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->colorValue( mySection, myParameter, val ) : val;
}

/*!
  \brief Get font resources value corresponding to the item.
  \param val default value (returned if there is no such resource)
  \return font value of the associated resource
  \sa setFont()
*/
QFont QtxPreferenceItem::getFont( const QFont& val ) const
{
  QtxResourceMgr* resMgr = resourceMgr();
  return resMgr ? resMgr->fontValue( mySection, myParameter, val ) : val;
}

/*!
  \brief Set integer resources value corresponding to the item.
  \param val new value
  \sa getInteger()
*/
void QtxPreferenceItem::setInteger( const int val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  \brief Set double resources value corresponding to the item.
  \param val new value
  \sa getDouble()
*/
void QtxPreferenceItem::setDouble( const double val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  \brief Set boolean resources value corresponding to the item.
  \param val new value
  \sa getBoolean()
*/
void QtxPreferenceItem::setBoolean( const bool val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  \brief Set string resources value corresponding to the item.
  \param val new value
  \sa getString()
*/
void QtxPreferenceItem::setString( const QString& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  \brief Set color resources value corresponding to the item.
  \param val new value
  \sa getColor()
*/
void QtxPreferenceItem::setColor( const QColor& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  \brief Set font resources value corresponding to the item.
  \param val new value
  \sa getFont()
*/
void QtxPreferenceItem::setFont( const QFont& val )
{
  QtxResourceMgr* resMgr = resourceMgr();
  if ( resMgr )
    resMgr->setValue( mySection, myParameter, val );
}

/*!
  \brief Callback function which is called when the child
  preference item is added.

  This function can be reimplemented in the subclasses to customize
  child item addition operation. Base implementation does nothing.

  \param item child item being added
  \sa itemRemoved(), itemChanged()
*/
void QtxPreferenceItem::itemAdded( QtxPreferenceItem* /*item*/ )
{
}

/*!
  \brief Callback function which is called when the child
  preference item is removed.

  This function can be reimplemented in the subclasses to customize
  child item removal operation. Base implementation does nothing.

  \param item child item being removed
  \sa itemAdded(), itemChanged()
*/
void QtxPreferenceItem::itemRemoved( QtxPreferenceItem* /*item*/ )
{
}

/*!
  \brief Callback function which is called when the child
  preference item is modified.

  This function can be reimplemented in the subclasses to customize
  child item modifying operation. Base implementation does nothing.

  \param item child item being modified
  \sa itemAdded(), itemRemoved()
*/
void QtxPreferenceItem::itemChanged( QtxPreferenceItem* )
{
}

void QtxPreferenceItem::ensureVisible( QtxPreferenceItem* )
{
  ensureVisible();
}

/*!
  \brief Initiate item updating.
*/
void QtxPreferenceItem::triggerUpdate()
{
  Updater::instance()->updateItem( this );
}

/*!
  \brief Get preference item option value.

  This function can be reimplemented in the subclasses.
  Base implementation does nothing.

  \param name option name
  \return property value or null QVariant if option is not set
  \sa setOptionValue()
*/
QVariant QtxPreferenceItem::optionValue( const QString& name ) const
{
  QVariant val;
  if ( name == "eval" || name == "evaluation" || name == "subst" || name == "substitution" )
    val = isEvaluateValues();
  else if ( name == "restart" )
    val = isRestartRequired();
  else if ( name == "title" )
    val = title();
  return val;
}

/*!
  \brief Set preference item option value.

  This function can be reimplemented in the subclasses.
  Base implementation does nothing.

  \param name option name
  \param val new property value
  \sa optionValue()
*/
void QtxPreferenceItem::setOptionValue( const QString& name, const QVariant& val )
{
  if ( name == "eval" || name == "evaluation" || name == "subst" || name == "substitution" )
  {
    if ( val.canConvert( QVariant::Bool ) )
      setEvaluateValues( val.toBool() );
  }
  if ( name == "restart" )
  {
    if ( val.canConvert( QVariant::Bool ) )
      setRestartRequired( val.toBool() );
  }
  else if ( name == "title" )
  {
    if ( val.canConvert( QVariant::String ) )
      setTitle( val.toString() );
  }
}

/*!
  \brief Initiate item changing call back operation.
*/
void QtxPreferenceItem::sendItemChanges()
{
  if ( parentItem() )
    parentItem()->itemChanged( this );
}

/*!
  \brief Generate unique preference item identifier.
  \return unique item ID
*/
int QtxPreferenceItem::generateId()
{
  static int _id = 0;
  return _id++;
}

/*!
  \class QtxPreferenceMgr
  \brief Class for managing preferences items.
*/

/*!
  \brief Constructor.
  \param mgr resources manager
*/
QtxPreferenceMgr::QtxPreferenceMgr( QtxResourceMgr* mgr )
: QtxPreferenceItem( 0 ),
  myResMgr( mgr )
{
}

/*!
  \brief Destructor.
*/
QtxPreferenceMgr::~QtxPreferenceMgr()
{
}

/*!
  \brief Get the resources manager.
  \return resource manager pointer or 0 if it is not defined
*/
QtxResourceMgr* QtxPreferenceMgr::resourceMgr() const
{
  return myResMgr;
}

/*!
  \brief Get the parent preferences manager.
  \return pointer to itself
*/
QtxPreferenceMgr* QtxPreferenceMgr::preferenceMgr() const
{
  return (QtxPreferenceMgr*)this;
}

/*!
  \brief Get preference item option value.
  \param id preference item ID
  \param propName option name
  \return property value or null QVariant if option is not set
  \sa setOption()
*/
QVariant QtxPreferenceMgr::option( const int id, const QString& propName ) const
{
  QVariant propValue;
  QtxPreferenceItem* i = findItem( id, true );
  if ( i )
    propValue = i->option( propName );
  return propValue;
}

/*!
  \brief Set preference item option value.
  \param id preference item ID
  \param propName option name
  \param propValue new property value
  \sa option()
*/
void QtxPreferenceMgr::setOption( const int id, const QString& propName, const QVariant& propValue )
{
  QtxPreferenceItem* i = findItem( id, true );
  if ( i )
    i->setOption( propName, propValue );
}

/*!
  \brief Store all preferences item to the resource manager.
  \sa retrieve()
*/
void QtxPreferenceMgr::store()
{
  ResourceMap before;
  resourceValues( before );

  QList<QtxPreferenceItem*> items = childItems( true );
  for ( QList<QtxPreferenceItem*>::iterator it = items.begin(); it != items.end(); ++it )
    (*it)->store();

  ResourceMap after;
  resourceValues( after );

  ResourceMap changed;
  differentValues( before, after, changed );

  changedResources( changed );
}

/*!
  \brief Retrieve all preference items from the resource manager.
  \sa store()
*/
void QtxPreferenceMgr::retrieve()
{
  QList<QtxPreferenceItem*> items = childItems( true );
  for ( QList<QtxPreferenceItem*>::iterator it = items.begin(); it != items.end(); ++it )
    (*it)->retrieve();
}

/*!
  \brief Dumps all values to the backup container.
  \sa fromBackup()
*/
void QtxPreferenceMgr::toBackup()
{
  myBackup.clear();
  resourceValues( myBackup );
}

/*!
  \brief Restore all values from the backup container.
  \sa toBackup()
*/
void QtxPreferenceMgr::fromBackup()
{
  ResourceMap before;
  resourceValues( before );

  setResourceValues( myBackup );

  ResourceMap after;
  resourceValues( after );

  ResourceMap changed;
  differentValues( before, after, changed );

  changedResources( changed );
}

/*!
  \brief Update preferences manager.

  Base implementation does nothing.
*/
void QtxPreferenceMgr::update()
{
}

/*!
  \brief Get all resources items values.
  \param map used as container filled with the resources values (<ID>:<value>)
  \sa setResourceValues()
*/
void QtxPreferenceMgr::resourceValues( QMap<int, QString>& map ) const
{
  QString sect, name;
  QtxResourceMgr* resMgr = resourceMgr();
  QList<QtxPreferenceItem*> items = childItems( true );
  for ( QList<QtxPreferenceItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QtxPreferenceItem* item = *it;
    item->resource( sect, name );
    if ( resMgr->hasValue( sect, name ) )
      map.insert( item->id(), item->resourceValue() );
  }
}

/*!
  \brief Get all resources items values.
  \param map used as container filled with the resources values
  (<item>:<value>)
  \sa setResourceValues()
*/
void QtxPreferenceMgr::resourceValues( ResourceMap& map ) const
{
  QString sect, name;
  QtxResourceMgr* resMgr = resourceMgr();
  QList<QtxPreferenceItem*> items = childItems( true );
  for ( QList<QtxPreferenceItem*>::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    QtxPreferenceItem* item = *it;
    item->resource( sect, name );
    if ( resMgr->hasValue( sect, name ) )
      map.insert( item, item->resourceValue() );
  }
}

/*!
  \brief Set all resources items values.
  \param map map with resources values (<ID>:<value>)
  \sa resourceValues()
*/
void QtxPreferenceMgr::setResourceValues( QMap<int, QString>& map ) const
{
  for ( QMap<int, QString>::const_iterator it = map.begin(); it != map.end(); ++it )
  {
    QtxPreferenceItem* i = findItem( it.key(), true );
    if ( i )
      i->setResourceValue( it.value() );
  }
}

/*!
  \brief Set all resources items values.
  \param map map with resources values (<item>:<value>)
  \sa resourceValues()
*/
void QtxPreferenceMgr::setResourceValues( ResourceMap& map ) const
{
  for ( ResourceMap::const_iterator it = map.begin(); it != map.end(); ++it )
    it.key()->setResourceValue( it.value() );
}

/*!
  \brief Compare two maps of resources values to find differences.
  \param map1 first map
  \param map2 second map
  \param resMap map to be filled with different values
  \param fromFirst if \c true, then \a resMap will be filled with the values
  from \a map1, otherwise - from \a map2
*/
void QtxPreferenceMgr::differentValues( const QMap<int, QString>& map1, const QMap<int, QString>& map2,
                                        QMap<int, QString>& resMap, const bool fromFirst ) const
{
  resMap.clear();
  const QMap<int, QString>& later = fromFirst ? map1 : map2;
  const QMap<int, QString>& early = fromFirst ? map2 : map1;

  for ( QMap<int, QString>::const_iterator it = later.begin(); it != later.end(); ++it )
  {
    if ( !early.contains( it.key() ) || early[it.key()] != it.value() )
      resMap.insert( it.key(), it.value() );
  }
}

/*!
  \brief Compare two maps of resources values to find differences.
  \param map1 first map
  \param map2 second map
  \param resMap map to be filled with different values
  \param fromFirst if \c true, then \a resMap will be filled with the values
  from \a map1, otherwise - from \a map2
*/
void QtxPreferenceMgr::differentValues( const ResourceMap& map1, const ResourceMap& map2,
                                        ResourceMap& resMap, const bool fromFirst ) const
{
  resMap.clear();
  const ResourceMap& later = fromFirst ? map1 : map2;
  const ResourceMap& early = fromFirst ? map2 : map1;

  for ( ResourceMap::const_iterator it = later.begin(); it != later.end(); ++it )
  {
    if ( !early.contains( it.key() ) || early[it.key()] != it.value() )
      resMap.insert( it.key(), it.value() );
  }
}

/*!
  \brief Perform custom activity on resource changing.

  This method is called from store() and fromBackup() methods.
  Base implementation does nothing.

  \sa store(), fromBackup()
*/
void QtxPreferenceMgr::changedResources( const ResourceMap& )
{
}
