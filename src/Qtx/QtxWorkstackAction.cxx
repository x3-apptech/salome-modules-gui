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
// File:      QtxWorkstackAction.cxx
// Author:    Sergey TELKOV

#include "QtxWorkstackAction.h"

#include "QtxWorkstack.h"

#include <qpopupmenu.h>
#include <qwidgetlist.h>

/*!
  Constructor
*/
QtxWorkstackAction::QtxWorkstackAction( QtxWorkstack* ws, QObject* parent, const char* name )
: QtxAction( tr( "Controls windows into workstack" ), tr( "Workstack management" ), 0, parent, name ),
myFlags( Standard ),
myWorkstack( ws )
{
  myItem.insert( VSplit, new QtxAction( tr( "Split the active window on two vertical parts" ),
                                        tr( "Split vertically" ), 0, this, 0, false ) );
  myItem.insert( HSplit, new QtxAction( tr( "Split the active window on two horizontal parts" ),
                                        tr( "Split horizontally" ), 0, this, 0, false ) );

  connect( myItem[VSplit], SIGNAL( activated() ), ws, SLOT( splitVertical() ) );
  connect( myItem[HSplit], SIGNAL( activated() ), ws, SLOT( splitHorizontal() ) );
}

/*!
  Destructor
*/
QtxWorkstackAction::~QtxWorkstackAction()
{
}

/*!
  \return corresponding workstack
*/
QtxWorkstack* QtxWorkstackAction::workstack() const
{
  return myWorkstack;
}

/*!
  \return set of action flags
*/
int QtxWorkstackAction::items() const
{
  return myFlags;
}

/*!
  Sets action flags
  \param flags - new set of flags
*/
void QtxWorkstackAction::setItems( const int flags )
{
  if ( !flags || flags == myFlags || !( flags & Split ) )
    return;

  myFlags = flags;
}

/*!
  \return true if action contains all flags
  \param flags - new set of flags
*/
bool QtxWorkstackAction::hasItems( const int flags ) const
{
  return ( myFlags & flags ) == flags;
}

/*!
  \return accelerator of item
  \param id - item id
*/
int QtxWorkstackAction::accel( const int id ) const
{
  int a = 0;
  if ( myItem.contains( id ) )
    a = myItem[id]->accel();
  return a;
}

/*!
  \return icons of item
  \param id - item id
*/
QIconSet QtxWorkstackAction::iconSet( const int id ) const
{
  QIconSet ico;
  if ( myItem.contains( id ) )
    ico = myItem[id]->iconSet();
  return ico;
}

/*!
  \return menu text of item
  \param id - item id
*/
QString QtxWorkstackAction::menuText( const int id ) const
{
  QString txt;
  if ( myItem.contains( id ) )
    txt = myItem[id]->menuText();
  return txt;
}

/*!
  \return status tip of item
  \param id - item id
*/
QString QtxWorkstackAction::statusTip( const int id ) const
{
  QString txt;
  if ( myItem.contains( id ) )
    txt = myItem[id]->statusTip();
  return txt;
}

/*!
  Changes accelerator of item
  \param id - item id
  \param a - new accelerator
*/
void QtxWorkstackAction::setAccel( const int id, const int a )
{
  if ( myItem.contains( id ) )
    myItem[id]->setAccel( a );
}

/*!
  Changes icons of item
  \param id - item id
  \param ico - new icons
*/
void QtxWorkstackAction::setIconSet( const int id, const QIconSet& ico )
{
  if ( myItem.contains( id ) )
    myItem[id]->setIconSet( ico );
}

/*!
  Changes menu text of item
  \param id - item id
  \param txt - new menu text
*/
void QtxWorkstackAction::setMenuText( const int id, const QString& txt )
{
  if ( myItem.contains( id ) )
    myItem[id]->setMenuText( txt );
}

/*!
  Changes status tip of item
  \param id - item id
  \param txt - new status tip
*/
void QtxWorkstackAction::setStatusTip( const int id, const QString& txt )
{
  if ( myItem.contains( id ) )
    myItem[id]->setStatusTip( txt );
}

/*!
  Adds action to widget
  \param wid - widget
*/
bool QtxWorkstackAction::addTo( QWidget* wid )
{
  return addTo( wid, -1 );
}

/*!
  Adds action to widget
  \param wid - widget
  \param idx - position
*/
bool QtxWorkstackAction::addTo( QWidget* wid, const int idx )
{
  if ( !wid || !wid->inherits( "QPopupMenu" ) )
    return false;

  QPopupMenu* pm = (QPopupMenu*)wid;
  checkPopup( pm );

  if ( myMenu.contains( pm ) )
    return false;

  myMenu.insert( pm, QIntList() );
  fillPopup( pm, idx );

  connect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
  connect( pm, SIGNAL( destroyed( QObject* ) ), this, SLOT( onPopupDestroyed( QObject* ) ) );

  return true;
}

/*!
  Removes action from widget
  \param wid - widget
*/
bool QtxWorkstackAction::removeFrom( QWidget* wid )
{
  if ( !wid || !wid->inherits( "QPopupMenu" ) )
    return false;

  QPopupMenu* pm = (QPopupMenu*)wid;
  if ( !myMenu.contains( pm ) )
    return false;

  clearPopup( pm );

  disconnect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
  disconnect( pm, SIGNAL( destroyed( QObject* ) ), this, SLOT( onPopupDestroyed( QObject* ) ) );

  myMenu.remove( pm );

  return true;
}

/*!
  Performs action
  \param type - action type
*/
void QtxWorkstackAction::perform( const int type )
{
  switch ( type )
  {
  case VSplit:
    workstack()->splitVertical();
    break;
  case HSplit:
    workstack()->splitHorizontal();
    break;
  }
}

/*!
  SLOT: called just before the popup menu is displayed, updates popup
*/
void QtxWorkstackAction::onAboutToShow()
{
  const QObject* obj = sender();
  if ( !obj || !obj->inherits( "QPopupMenu" ) )
    return;

  QtxWorkstack* ws = workstack();
  if ( ws && myItem.contains( VSplit ) )
    myItem[VSplit]->setAccel( ws->accel( QtxWorkstack::SplitVertical ) );
  if ( ws && myItem.contains( HSplit ) )
    myItem[HSplit]->setAccel( ws->accel( QtxWorkstack::SplitHorizontal ) );

  updatePopup( (QPopupMenu*)obj );
}

/*!
  SLOT: called when popup menu is destroyed, removes it from menu
*/
void QtxWorkstackAction::onPopupDestroyed( QObject* obj )
{
  myMenu.remove( (QPopupMenu*)obj );
}

/*!
  Updates popup
  \param pm - popup menu
*/
void QtxWorkstackAction::checkPopup( QPopupMenu* pm )
{
  if ( !myMenu.contains( pm ) )
    return;

  QIntList updList;
  for ( QIntList::const_iterator it = myMenu[pm].begin(); it != myMenu[pm].end(); ++it )
  {
    if ( pm->indexOf( *it ) != -1 )
      updList.append( *it );
  }

  myMenu.remove( pm );

  if ( !updList.isEmpty() )
    myMenu.insert( pm, updList );
  else
  {
    disconnect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
    disconnect( pm, SIGNAL( destroyed( QObject* ) ), this, SLOT( onPopupDestroyed( QObject* ) ) );
  }
}

/*!
  Clears and refills popup and updates state of actions
  \param pm - popup menu
*/
void QtxWorkstackAction::updatePopup( QPopupMenu* pm )
{
  if ( !myMenu.contains( pm ) )
    return;

  fillPopup( pm, clearPopup( pm ) );

  int count = workstack() ? workstack()->splitWindowList().count() : 0;
  myItem[VSplit]->setEnabled( count > 1 );
  myItem[HSplit]->setEnabled( count > 1 );
}

/*!
  Clears popup
  \param pm - popup menu
*/
int QtxWorkstackAction::clearPopup( QPopupMenu* pm )
{
  if ( !myMenu.contains( pm ) )
    return -1;

  int idx = -1;
  const QIntList& lst = myMenu[pm];
  for ( QIntList::const_iterator it = lst.begin(); it != lst.end() && idx == -1; ++it )
    idx = pm->indexOf( *it );

  for ( ItemMap::ConstIterator mit = myItem.begin(); mit != myItem.end(); ++mit )
    mit.data()->removeFrom( pm );

  for ( QIntList::const_iterator itr = lst.begin(); itr != lst.end(); ++itr )
    pm->removeItem( *itr );

  return idx;
}

/*!
  Fills popup with items
  \param pm - popup menu
  \param idx - position
*/
void QtxWorkstackAction::fillPopup( QPopupMenu* pm, const int idx )
{
  if ( !pm )
    return;

  int index = idx < 0 ? pm->count() : QMIN( (int)pm->count(), idx );

  myMenu.insert( pm, QIntList() );
  QIntList& lst = myMenu[pm];

  for ( ItemMap::ConstIterator mit = myItem.begin(); mit != myItem.end(); ++mit )
  {
    if ( !hasItems( mit.key() ) )
      continue;

    mit.data()->addTo( pm, index );
    lst.append( pm->idAt( index++ ) );
  }

  QtxWorkstack* ws = workstack();
  if ( !ws || !hasItems( Windows ) )
    return;

  QWidgetList wList = ws->windowList();
  if ( wList.isEmpty() )
    return;

  lst.append( pm->insertSeparator( index++ ) );

  int param = 0;
  pm->setCheckable( true );
  for ( QWidgetListIt it( wList ); it.current(); ++it )
  {
    int id = pm->insertItem( it.current()->caption(), this, SLOT( onItemActivated( int ) ), 0, -1, index++ );
    pm->setItemParameter( id, param++ );
    pm->setItemChecked( id, it.current() == ws->activeWindow() );
    lst.append( id );
  }
}

/*!
  SLOT: called when popup item corresponding to window is activated, activates window
*/
void QtxWorkstackAction::onItemActivated( int idx )
{
  QtxWorkstack* ws = workstack();
  if ( !ws )
    return;

  QWidgetList wList = ws->windowList();
  if ( idx < 0 || idx >= (int)wList.count() )
    return;

  wList.at( idx )->setFocus();
}
