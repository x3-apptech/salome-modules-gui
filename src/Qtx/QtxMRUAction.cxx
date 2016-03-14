// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// File:      QtxMRUAction.cxx
// Author:    Sergey TELKOV
//
#include "QtxMRUAction.h"

#include "QtxResourceMgr.h"

#include <QMenu>
#include <QIcon>

/*!
  \class QtxMRUAction
  \brief Menu action which provides most recent used items support.
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
QtxMRUAction::QtxMRUAction( QObject* parent )
: QtxAction( tr( "Most Recently Used" ), tr( "Most Recently Used" ), 0, parent ),
  myVisCount( 5 ),
  myHistoryCount( -1 ),
  myLinkType( LinkAuto ),
  myInsertMode( MoveFirst )
{
  myClear = new QAction( tr( "Clear" ), this );
  myClear->setVisible( false );

  setMenu( new QMenu( 0 ) );

  connect( menu(), SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
  connect( myClear, SIGNAL( triggered( bool ) ), this, SLOT( onCleared( bool ) ) );
}

/*!
  \brief Constructor.
  \param description (tooltip) text
  \param menuText menu text
  \param parent parent object
*/
QtxMRUAction::QtxMRUAction( const QString& text, const QString& menuText, QObject* parent )
: QtxAction( text, menuText, 0, parent ),
  myVisCount( 5 ),
  myHistoryCount( -1 ),
  myLinkType( LinkAuto ),
  myInsertMode( MoveFirst )
{
  myClear = new QAction( tr( "Clear" ), this );
  myClear->setVisible( false );

  setMenu( new QMenu( 0 ) );
  connect( menu(), SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
  connect( myClear, SIGNAL( triggered( bool ) ), this, SLOT( onCleared( bool ) ) );
}

/*!
  \brief Constructor.
  \param description (tooltip) text
  \param icon action icon
  \param menuText menu text
  \param parent parent object
*/
QtxMRUAction::QtxMRUAction( const QString& text, const QIcon& icon,
                            const QString& menuText, QObject* parent )
: QtxAction( text, icon, menuText, 0, parent ),
  myVisCount( 5 ),
  myHistoryCount( -1 ),
  myLinkType( LinkAuto ),
  myInsertMode( MoveFirst )
{
  myClear = new QAction( tr( "Clear" ), this );
  myClear->setVisible( false );

  setMenu( new QMenu( 0 ) );
  connect( menu(), SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
  connect( myClear, SIGNAL( triggered( bool ) ), this, SLOT( onCleared( bool ) ) );
}

/*!
  \brief Destructor.
*/
QtxMRUAction::~QtxMRUAction()
{
  delete menu();
}

/*!
  \brief Get items insertion policy.
  \return insertion policy (QtxMRUAction::InsertionMode)
*/
int QtxMRUAction::insertMode() const
{
  return myInsertMode;
}

/*!
  \brief Set items insertion policy.
  \param mode insertion policy (QtxMRUAction::InsertionMode)
*/
void QtxMRUAction::setInsertMode( const int mode )
{
  myInsertMode = mode;
}

/*!
  \brief Get the type of link menu name.
  \return link type (QtxMRUAction::LinkType)
*/
int QtxMRUAction::linkType() const
{
  return myLinkType;
}

/*!
  \brief Set the type of link menu name.
  \param link type (QtxMRUAction::LinkType)
*/
void QtxMRUAction::setLinkType( const int type )
{
  myLinkType = type;
}

/*!
  \brief Get number of MRU items.
  \return number of MRU items
*/
int QtxMRUAction::count() const
{
  return myLinks.count();
}

/*!
  \brief Check if the MRU items list is empty.
  \return \c true if there are no MRU items
*/
bool QtxMRUAction::isEmpty() const
{
  return myLinks.isEmpty();
}

/*!
  \brief Get number of visible MRU items.
  \return visible MRU items number
  \sa setVisibleCount()
*/
int QtxMRUAction::visibleCount() const
{
  return myVisCount;
}

/*!
  \brief Set number of visible MRU items.

  This method sets the maximum number of MRU items
  to be displayed in the popup menu (5 by default).

  If \a num < 1, then all MRU items will be displayed.

  \param num visible MRU items number
*/
void QtxMRUAction::setVisibleCount( int num )
{
  if ( myVisCount == num )
    return;

  myVisCount = num;
}

/*!
  \brief Return visible status of the menu item which clear all MRU items.
*/
bool QtxMRUAction::isClearPossible() const
{
  return myClear->isVisible();
}

/*!
  \brief Set visible the menu item which clear all MRU items.
*/
void QtxMRUAction::setClearPossible( const bool on )
{
  myClear->setVisible( on );
}

/*!
  \brief Get number of totally stored MRU items.
  \return number of MRU items stored in the preferences
  \sa setHistoryCount(), saveLinks(), loadLinks()
*/
int QtxMRUAction::historyCount() const
{
  return myHistoryCount;
}

/*!
  \brief Set number of totally stored MRU items.

  This option allows setting number of MRU items to be stored
  in the preferences file.

  If \a num < 0, then number of stored MRU items is not limited.

  \return number of MRU items stored in the preferences
  \sa historyCount(), saveLinks(), loadLinks()
*/
void QtxMRUAction::setHistoryCount( const int num )
{
  myHistoryCount = num;
}

/*!
  \brief Insert MRU item.

  The item is inserted according to the current insertion policy.

  \param link MRU item to be added
*/
void QtxMRUAction::insert( const QString& link )
{
  if ( myLinks.contains( link ) && ( insertMode() == AddFirst || insertMode() == AddLast ) )
    return;

  myLinks.removeAll( link );

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
}

/*!
  \brief Remove MRU item.

  Does nothing if \a idx is out of range.

  \param idx MRU item index
*/
void QtxMRUAction::remove( const int idx )
{
  if ( idx < 0 || idx >= (int)myLinks.count() )
    return;

  myLinks.removeAt( idx );
}

/*!
  \brief Remove MRU item.

  Does nothing if there is no speicified item in the list.

  \param link MRU item to be removed
*/
void QtxMRUAction::remove( const QString& link )
{
  myLinks.removeAll( link );
}

/*!
  \brief Remove all MRU items.
*/
void QtxMRUAction::clear()
{
  myLinks.clear();
}

/*!
  \brief Get MRU item
  \param idx MRU item index
  \return MRU item or null QString if \a idx is out of range
*/
QString QtxMRUAction::item( const int idx ) const
{
  QString res;
  if ( idx >= 0 && idx < (int)myLinks.count() )
    res = myLinks[idx];
  return res;
}

/*!
  \brief Get MRU item index.
  \param link MRU item
  \return MRU item index or -1 if item is not found
*/
int QtxMRUAction::find( const QString& link ) const
{
  return myLinks.indexOf( link );
}

/*!
  \brief Check if MRU item is in the list.
  \param link MRU item
  \return \c true if specified item is already added to the list
*/
bool QtxMRUAction::contains( const QString& link ) const
{
  return myLinks.contains( link );
}

/*!
  \brief Load the MRU items from specified resources section.
  \param resMgr resources manager
  \param section resources section
  \param clear if \c true, previous MRU items list is cleared
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

    QString link = resMgr->stringValue( section, *it, QString() );
    if ( link.isEmpty() || map.contains( link ) )
      continue;

    myLinks.append( link );
    map.insert( link, 0 );
  }
}

/*!
  \brief Save the MRU items to specified resources section.
  \param resMgr resources manager
  \param section resources section
  \param clear if \c true, the resources section is first cleared
*/
void QtxMRUAction::saveLinks( QtxResourceMgr* resMgr, const QString& section, const bool clear ) const
{
  if ( !resMgr || section.isEmpty() )
    return;

  QString itemPrefix( "item_" );

  if ( clear ) {
    QStringList items = resMgr->parameters( section );
    for ( QStringList::const_iterator it = items.begin(); it != items.end(); ++it )
    {
      if ( (*it).startsWith( itemPrefix ) )
        resMgr->remove( section, *it );
    }
  }

  QStringList lst;
  QMap<QString, int> map;
  for ( QStringList::const_iterator itr = myLinks.begin(); itr != myLinks.end(); ++itr )
  {
    lst.append( *itr );
    map.insert( *itr, 0 );
  }

  QStringList items = resMgr->parameters( section );
  for ( QStringList::const_iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( !(*it).startsWith( itemPrefix ) )
      continue;

    QString link = resMgr->stringValue( section, *it, QString() );
    if ( !link.isEmpty() && !map.contains( link ) )
    {
      lst.append( link );
      map.insert( link, 0 );
    }

    resMgr->remove( section, *it );
  }

  int counter = 0;
  for ( QStringList::const_iterator iter = lst.begin();
        iter != lst.end() && ( myHistoryCount < 0 || counter < myHistoryCount );
        ++iter, counter++ )
    resMgr->setValue( section, itemPrefix + QString().sprintf( "%03d", counter ), *iter );
}

/*!
  \brief Prepare MRU items popup menu.

  This method is called when the parent menu is shown.
  Enables or disables sub menu item according to the number of MRU items.
*/
void QtxMRUAction::onAboutToShow()
{
  updateMenu();
}

/*!
  \brief Called when any MRU item is selected by the user.

  Emits signal activated(const QString&) passing selected MRU item as parameter.
*/
void QtxMRUAction::onActivated()
{
  QAction* a = ::qobject_cast<QAction*>( sender() );
  if ( !a )
    return;

  QString link = a->data().toString();
  if ( !link.isEmpty() && myLinks.contains( link ) )
    emit activated( link );
}

void QtxMRUAction::onCleared( bool )
{
  clear();
}

/*!
  \brief Update MRU items popup menu.
*/
void QtxMRUAction::updateMenu()
{
  QMenu* pm = menu();
  if ( !pm )
    return;

  pm->clear();

  QStringList links;
  QMap<QString, int> map;
  int count = visibleCount() < 0 ? myLinks.count() : visibleCount();
  int i = insertMode() == AddLast || insertMode() == MoveLast ? qMax( 0, myLinks.count()-count ) : 0;
  for ( ; i < myLinks.count() && count > 0; ++i, count-- )
  {
    links.append( myLinks[i] );
    if ( linkType() == LinkAuto )
    {
      QString shortName = Qtx::file( myLinks[i] );
      if ( map.contains( shortName ) )
        map[shortName]++;
      else
        map.insert( shortName, 0 );
    }
  }

  i = 1;
  for ( QStringList::const_iterator it = links.begin(); it != links.end(); ++it, i++ )
  {
    QString linkName;
    switch( linkType() )
    {
    case LinkAuto:
      linkName = Qtx::file( *it );
      if ( map.contains( linkName ) && map[linkName] )
        linkName = *it;
      break;
    case LinkShort:
      linkName = Qtx::file( *it );
      break;
    case LinkFull:
    default:
      linkName = *it;
      break;
    }

    if ( links.count() < 10 )
      linkName = QString( "&%1 %2" ).arg( i ).arg( linkName );

    pm->addAction( linkName, this, SLOT( onActivated() ) )->setData( *it );
  }

  if ( pm->isEmpty() )
    pm->addAction( tr( "<Empty>" ) )->setEnabled( false );

  if ( isClearPossible() )
  {
    pm->addSeparator();
    pm->addAction( myClear );
    myClear->setEnabled( !pm->isEmpty() );
  }
}

/*!
  \fn void QtxMRUAction::activated( const QString& link );
  \brief Emitted when user selects any MRU item in the menu.
  \param link selected MRU item
*/
