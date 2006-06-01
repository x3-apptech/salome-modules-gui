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
// File:      QtxMRUAction.cxx
// Author:    Sergey TELKOV

#include "QtxMRUAction.h"

#include "QtxResourceMgr.h"

#include <qpopupmenu.h>

/*!
	Name: QtxMRUAction [public]
	Desc: Constructs an MRU action with given parent and name.
*/

QtxMRUAction::QtxMRUAction( QObject* parent, const char* name )
: QtxAction( "Most Recently Used", "Most Recently Used", 0, parent, name ),
myVisCount( 5 ),
myPopupMode( SubMenu ),
myInsertMode( MoveFirst )
{
}

/*!
	Name: QtxMRUAction [public]
	Desc: This constructor creates an action with the following properties: the
		    description text, the menu text and.  It is a child of given parent and
        named specified name.
*/

QtxMRUAction::QtxMRUAction( const QString& text, const QString& menuText, QObject* parent, const char* name )
: QtxAction( text, menuText, 0, parent, name ),
myVisCount( 5 ),
myPopupMode( SubMenu ),
myInsertMode( MoveFirst )
{
}

/*!
	Name: QtxMRUAction [public]
	Desc: This constructor creates an action with the following properties: the
		    description text, the menu text, the icon or iconset icon and keyboard
        accelerator. It is a child of given parent and named specified name.
*/

QtxMRUAction::QtxMRUAction( const QString& text, const QIconSet& icon, const QString& menuText, QObject* parent, const char* name )
: QtxAction( text, icon, menuText, 0, parent, name ),
myVisCount( 5 ),
myPopupMode( SubMenu ),
myInsertMode( MoveFirst )
{
}

/*!
	Name: ~QtxMRUAction [public]
	Desc: This destructor removes all added popup items.
*/

QtxMRUAction::~QtxMRUAction()
{
  for ( ItemsMap::ConstIterator iIt = myItems.begin(); iIt != myItems.end(); ++iIt )
    removeFrom( iIt.key() );

  for ( MenusMap::ConstIterator mIt = myMenus.begin(); mIt != myMenus.end(); ++mIt )
    removeFrom( mIt.key() );
}

/*!
	Name: insertMode [public]
	Desc: Returns the insert mode.
*/

int QtxMRUAction::insertMode() const
{
  return myInsertMode;
}

/*!
	Name: setInsertMode [public]
	Desc: Returns the insert mode. Can be following values:
      MoveFirst - place the specified link to the first position in any case
      MoveLast  - place the specified link to the last position in any case
      AddFirst  - if inserted link doesn't exist then add to the first position
      AddLast   - if inserted link doesn't exist then add to the lase position
*/

void QtxMRUAction::setInsertMode( const int mode )
{
  myInsertMode = mode;
}

/*!
	Name: popupMode [public]
	Desc: Returns the popup mode.
*/

int QtxMRUAction::popupMode() const
{
  return myPopupMode;
}

/*!
	Name: setPopupMode [public]
	Desc: Set the popup mode. If this mode is 'Items' then method "addTo" creates the
        items in the specified popup menu.  If mode is 'SubMenu' then items will be
        create in sub popup menu which will be placed in specified popup.
*/

void QtxMRUAction::setPopupMode( const int mode )
{
  myPopupMode = mode;
}

/*!
	Name: count [public]
	Desc: Returns the number of links.
*/

int QtxMRUAction::count() const
{
  return myLinks.count();
}

/*!
	Name: isEmpty [public]
	Desc: Returns 'true' if there is no links.
*/

bool QtxMRUAction::isEmpty() const
{
  return myLinks.isEmpty();
}

/*!
	Name: visibleCount [public]
	Desc: Returns the number of first links which will be added to popup menu.
        If 'visibleCount' less than 1 then all links will be used.
*/

int QtxMRUAction::visibleCount() const
{
  return myVisCount;
}

/*!
	Name: setVisibleCount [public]
	Desc: Sets the number of links which will be used in popup menu.
*/

void QtxMRUAction::setVisibleCount( int num )
{
  if ( myVisCount == num )
    return;

  myVisCount = num;

  updateState();
}

/*!
	Name: insert [public]
	Desc: Insert the link according to the insert mode.
*/

void QtxMRUAction::insert( const QString& link )
{
  if ( myLinks.contains( link ) && ( insertMode() == AddFirst || insertMode() == AddLast ) )
    return;

  myLinks.remove( link );

  switch ( insertMode() )
  {
  case AddFirst:
  case MoveFirst:
    myLinks.prepend( link );
    break;
  case AddLast:
  case MoveLast:
    myLinks.append( link );
    break;
  }

  updateState();
}

/*!
	Name: remove [public]
	Desc: Removes link with specified index.
*/

void QtxMRUAction::remove( const int idx )
{
  if ( idx < 0 || idx >= (int)myLinks.count() )
    return;

  myLinks.remove( myLinks.at( idx ) );

  updateState();
}

/*!
	Name: remove [public]
	Desc: Removes specified link.
*/

void QtxMRUAction::remove( const QString& link )
{
  if ( myLinks.remove( link ) )
    updateState();
}

/*!
	Name: item [public]
	Desc: Returns the link with specified index.
*/

QString QtxMRUAction::item( const int idx ) const
{
  QString res;
  if ( idx >= 0 && idx < (int)myLinks.count() )
    res = myLinks[idx];
  return res;
}

/*!
	Name: find [public]
	Desc: Find specified link. If link exists then returns index otherwise -1 returned.
*/

int QtxMRUAction::find( const QString& link ) const
{
  return myLinks.findIndex( link );
}

/*!
	Name: contains [public]
	Desc: Returns 'true' if given link exist.
*/

bool QtxMRUAction::contains( const QString& link ) const
{
  return myLinks.contains( link );
}

/*!
	Name: addTo [public]
	Desc: Add the MRU links to the end of specified popup according to the popup mode.
*/

bool QtxMRUAction::addTo( QWidget* wid )
{
  if ( !wid || !wid->inherits( "QPopupMenu" ) )
    return false;

  QPopupMenu* pm = (QPopupMenu*)wid;
  checkPopup( pm );

  int mode = popupMode();

  if ( ( mode == Items && myItems.contains( pm ) ) ||
       ( mode == SubMenu && myMenus.contains( pm ) ) )
    return false;

  bool exist = myItems.contains( pm ) || myMenus.contains( pm );

  if ( mode == SubMenu && !QtxAction::addTo( wid ) )
    return false;

  if ( mode == Items )
  {
    myItems.insert( pm, Item() );
    myItems[pm].pId = myItems[pm].nId -1;
    connect( pm, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
  }
  else if ( mode == SubMenu )
  {
    myMenus.insert( pm, new QPopupMenu( pm ) );
    setPopup( pm, pm->idAt( pm->count() - 1 ), myMenus[pm] );
    connect( myMenus[pm], SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
  }

  if ( !exist )
  {
    connect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
    connect( pm, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
  }

  return insertLinks( pm, mode );
}

/*!
	Name: addTo [public]
	Desc: Add the MRU links to the specified popup at given index according to the popup mode.
*/

bool QtxMRUAction::addTo( QWidget* wid, const int idx )
{
  if ( !QtxAction::addTo( wid, idx ) )
    return false;

  QPopupMenu* pm = (QPopupMenu*)wid;

  removeLinks( pm, popupMode() );
  insertLinks( pm, popupMode(), idx );

  return true;
}

/*!
	Name: removeFrom [public]
	Desc: Removes all MRU links from specified popup.
*/

bool QtxMRUAction::removeFrom( QWidget* wid )
{
  QtxAction::removeFrom( wid );

  QPopupMenu* pm = (QPopupMenu*)wid;
  if ( !wid || !wid->inherits( "QPopupMenu" ) )
    return false;

  if ( myItems.contains( pm ) )
  {
    removeLinks( pm, Items );
    myItems.remove( pm );
    disconnect( pm, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
  }
  if ( myMenus.contains( pm ) )
  {
    removeLinks( pm, SubMenu );
    delete myMenus[pm];
    myMenus.remove( pm );
  }

  disconnect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
  disconnect( pm, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

  return true;
}

/*!
	Name: loadLinks [public]
	Desc: Load the MRU links from specified resource manager section.
        If parameter 'clear' is 'true' then link list will be cleared first.
*/

void QtxMRUAction::loadLinks( QtxResourceMgr* resMgr, const QString& section, const bool clear )
{
  if ( !resMgr || section.isEmpty() )
    return;

  if ( clear )
    myLinks.clear();

  QString itemPrefix( "item_" );

  QMap<QString, int> map;
  for ( QStringList::const_iterator itr = myLinks.begin(); itr != myLinks.end(); ++ itr )
    map.insert( *itr, 0 );

  QStringList items = resMgr->parameters( section );
  for ( QStringList::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( !(*it).startsWith( itemPrefix ) )
      continue;

    QString link = resMgr->stringValue( section, *it, QString::null );
    if ( link.isEmpty() || map.contains( link ) )
      continue;

    myLinks.append( link );
    map.insert( link, 0 );
  }

  updateState();
}

/*!
	Name: saveLinks [public]
	Desc: Save the MRU links into specified resource manager section.
        If parameter 'clear' is 'true' then section will be cleared first.
*/

void QtxMRUAction::saveLinks( QtxResourceMgr* resMgr, const QString& section, const bool clear ) const
{
  if ( !resMgr || section.isEmpty() )
    return;

  if ( clear )
    resMgr->remove( section );

  QStringList lst;
  QMap<QString, int> map;
  for ( QStringList::const_iterator itr = myLinks.begin(); itr != myLinks.end(); ++itr )
    map.insert( *lst.append( *itr ), 0 );

  QString itemPrefix( "item_" );
  QStringList items = resMgr->parameters( section );
  for ( QStringList::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( !(*it).startsWith( itemPrefix ) )
      continue;

    QString link = resMgr->stringValue( section, *it, QString::null );
    if ( !link.isEmpty() && !map.contains( link ) )
      map.insert( *lst.append( link ), 0 );

    resMgr->remove( section, *it );
  }

  int counter = 0;
  for ( QStringList::const_iterator iter = lst.begin(); iter != lst.end(); ++iter, counter++ )
    resMgr->setValue( section, itemPrefix + QString().sprintf( "%03d", counter ), *iter );
}

/*!
	Name: setEnabled [public slot]
	Desc: Enable or disable all popup items with MRU links.
*/

void QtxMRUAction::setEnabled( bool on )
{
  QtxAction::setEnabled( on );

  for ( ItemsMap::ConstIterator iter = myItems.begin(); iter != myItems.end(); ++iter )
    for ( QIntList::const_iterator it = iter.data().idList.begin(); it != iter.data().idList.end(); ++it )
      iter.key()->setItemEnabled( *it, on );
}

/*!
	Name: onAboutToShow [private slots]
	Desc: Enable or disable sub menu item according to number of MRU links
        in sub popup when parent popup is shown.
*/

void QtxMRUAction::onAboutToShow()
{
  const QObject* obj = sender();
  if ( obj && obj->inherits( "QPopupMenu" ) )
  {
    QPopupMenu* pm = (QPopupMenu*)obj;
    if ( myMenus.contains( pm ) )
      pm->setItemEnabled( findId( pm, myMenus[pm]), isEnabled() && myMenus[pm] && myMenus[pm]->count() );
  }
}

/*!
	Name: onActivated [private slot]
	Desc: Process popup item activation and emit signal activated with selected MRU link.
*/

void QtxMRUAction::onActivated( int id )
{
  const QObject* obj = sender();
  if ( !obj->inherits( "QPopupMenu" ) )
    return;

  QPopupMenu* pm = (QPopupMenu*)obj;

  QString link;
  if ( ( myItems.contains( pm ) && myItems[pm].idList.contains( id ) ) ||
       ( myMenus.contains( (QPopupMenu*)pm->parent() ) && myMenus[(QPopupMenu*)pm->parent()] == pm ) )
    link = pm->text( id );

  if ( !link.isEmpty() )
    emit activated( link );
}

/*!
	Name: onDestroyed [private slot]
	Desc: Removes deleted popup menu from internal data structures.
*/

void QtxMRUAction::onDestroyed( QObject* obj )
{
  if ( !obj )
    return;

  myItems.remove( (QPopupMenu*)obj );
  myMenus.remove( (QPopupMenu*)obj );
}

/*!
	Name: updateState [private]
	Desc: Updates the state of all popup menus which contains MRU link items.
*/

void QtxMRUAction::updateState()
{
  for ( ItemsMap::ConstIterator iIt = myItems.begin(); iIt != myItems.end(); ++iIt )
    updatePopup( iIt.key(), Items );

  for ( MenusMap::ConstIterator mIt = myMenus.begin(); mIt != myMenus.end(); ++mIt )
    updatePopup( mIt.key(), SubMenu );
}

/*!
	Name: checkPopup [private]
	Desc: Check consistency the popup content and internal datas.
        Synchronize internal data structures with popup content.
*/

void QtxMRUAction::checkPopup( QPopupMenu* pm )
{
  if ( myItems.contains( pm ) )
  {
    bool found = true;
    for ( QIntList::const_iterator it = myItems[pm].idList.begin(); it != myItems[pm].idList.end() && found; ++it )
      found = pm->indexOf( *it ) != -1;
    if ( !found )
    {
      removeLinks( pm, Items );
      myItems.remove( pm );
      disconnect( pm, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
    }
  }
  if ( myMenus.contains( pm ) )
  {
    int id = findId( pm, myMenus[pm] );
    if ( id == -1 )
    {
      delete myMenus[pm];
      myMenus.remove( pm );
    }
  }

  if ( !myItems.contains( pm ) && !myMenus.contains( pm ) )
    disconnect( pm, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
}

/*!
	Name: updatePopup [private]
	Desc: Updates the MRU link items state in the specified popup menu.
*/

void QtxMRUAction::updatePopup( QPopupMenu* pm, const int mode )
{
  if ( !pm )
    return;

  int idx = -1;
  if ( mode == Items && myItems.contains( pm ) )
  {
    if ( !myItems[pm].idList.isEmpty() )
      idx = pm->indexOf( myItems[pm].idList.first() );
    else
    {
      int pIdx = pm->indexOf( myItems[pm].pId );
      int nIdx = pm->indexOf( myItems[pm].nId );
      if ( pIdx != -1 )
        idx = pIdx + 1;
      else if ( nIdx != -1 )
        idx = nIdx - 1;
    }
  }

  removeLinks( pm, mode );
  insertLinks( pm, mode, idx );
}

/*!
	Name: removeLinks [private]
	Desc: Removes MRU link items from specified popup.
*/

bool QtxMRUAction::removeLinks( QPopupMenu* pm, const int mode )
{
  if ( !pm )
    return false;

  if ( mode == SubMenu && myMenus.contains( pm ) )
    myMenus[pm]->clear();
  else if ( mode == Items && myItems.contains( pm ) )
  {
    for ( QIntList::const_iterator it = myItems[pm].idList.begin(); it != myItems[pm].idList.end(); ++it )
      pm->removeItem( *it );
    myItems[pm].idList.clear();
  }

  return true;
}

/*!
	Name: insertLinks [private]
	Desc: Inserts MRU link items to the specified popup.
*/

bool QtxMRUAction::insertLinks( QPopupMenu* pm, const int mode, const int idx )
{
  if ( !pm )
    return false;

  int count = visibleCount() < 0 ? myLinks.count() : visibleCount();
  bool isOn = isEnabled();
  if ( mode == SubMenu && myMenus.contains( pm ) )
  {
    for ( QStringList::const_iterator it = myLinks.begin(); it != myLinks.end() && count > 0; ++it, count-- )
    {
      int id = myMenus[pm]->insertItem( *it, -1 );
      myMenus[pm]->setItemEnabled( id, isOn );
    }
  }
  else if ( mode == Items )
  {
    QIntList ids;
    int index = idx;
    for ( QStringList::const_iterator it = myLinks.begin(); it != myLinks.end() && count > 0; ++it, count--  )
    {
      ids.append( pm->insertItem( *it, -1, index ) );
      pm->setItemEnabled( ids.last(), isOn );
      if ( index >= 0 )
        index++;
    }
    myItems[pm].idList = ids;
    if ( !myItems[pm].idList.isEmpty() )
    {
      myItems[pm].pId = pm->idAt( pm->indexOf( myItems[pm].idList.first() ) - 1 );
      myItems[pm].nId = pm->idAt( pm->indexOf( myItems[pm].idList.first() ) + 1 );
    }
  }
  return true;
}

/*!
	Name: findId [private]
	Desc: Returns identificator of popup item which contains sub popup 'pm' in the popup 'cont'.
*/

int QtxMRUAction::findId( QPopupMenu* cont, QPopupMenu* pm ) const
{
  if ( !cont || !pm )
    return -1;

  int id = -1;

  for ( int i = 0; i < (int)cont->count() && id == -1; i++ )
  {
    QMenuData* md = 0;
    QMenuItem* item = cont->findItem( cont->idAt( i ), &md );
    if ( item && md == cont && item->popup() == pm )
      id = item->id();
  }
  return id;
}
