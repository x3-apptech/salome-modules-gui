//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      QtxWorkstackAction.cxx
// Author:    Sergey TELKOV
//
#include "QtxWorkstackAction.h"

#include "QtxWorkstack.h"

#include <QMenu>
#include <QWidgetList>

/*!
  \class QtxWorkstackAction
  \brief Implements actions group for menu Windows with standard operations, like
         "Split vertical", "Split horizontal", etc.
*/

/*!
  \brief Constructor.
  \param ws workstack
  \param parent parent object (owner of the action)
*/
QtxWorkstackAction::QtxWorkstackAction( QtxWorkstack* ws, QObject* parent )
: QtxActionSet( parent ),
  myWorkstack( ws ),
  myWindowsFlag( true )
{
  insertAction( new QtxAction( tr( "Split the active window on two vertical parts" ),
                               tr( "Split vertically" ), 0, this ), SplitVertical );
  insertAction( new QtxAction( tr( "Split the active window on two horizontal parts" ),
                               tr( "Split horizontally" ), 0, this ), SplitHorizontal );

  connect( this, SIGNAL( triggered( int ) ), this, SLOT( onTriggered( int ) ) );

  setMenuActions( Standard );
}

/*!
  \brief Destructor.
*/
QtxWorkstackAction::~QtxWorkstackAction()
{
}

/*!
  \brief Get workstack.
  \return parent workstack
*/
QtxWorkstack* QtxWorkstackAction::workstack() const
{
  return myWorkstack;
}

/*!
  \brief Set actions to be visible in the menu.
  
  Actions, which IDs are set in \a flags parameter, will be shown in the 
  menu bar. Other actions will not be shown.

  \param flags ORed together actions flags
*/
void QtxWorkstackAction::setMenuActions( const int flags )
{
  action( SplitVertical )->setVisible( flags & SplitVertical );
  action( SplitHorizontal )->setVisible( flags & SplitHorizontal );
  myWindowsFlag = flags & Windows;
}

/*!
  \brief Get menu actions which are currently visible in the menu bar.
  \return ORed together actions flags
  \sa setMenuActions()
*/
int QtxWorkstackAction::menuActions() const
{
  int ret = 0;
  ret = ret | ( action( SplitVertical )->isVisible() ? SplitVertical : 0 );
  ret = ret | ( action( SplitHorizontal )->isVisible() ? SplitHorizontal : 0 );
  ret = ret | ( myWindowsFlag ? Windows : 0 );
  return ret;
}

/*!
  \brief Get keyboard accelerator for the specified action.
  \param id menu action ID
  \return keyboard accelerator of menu item or 0 if there is no such action
*/
int QtxWorkstackAction::accel( const int id ) const
{
  int a = 0;
  if ( action( id ) )
    a = action( id )->shortcut();
  return a;
}

/*!
  \brief Get icon for the specified action.

  If \a id is invalid, null icon is returned.

  \param id menu action ID
  \return menu item icon
*/
QIcon QtxWorkstackAction::icon( const int id ) const
{
  QIcon ico;
  if ( action( id ) )
    ico = action( id )->icon();
  return ico;
}

/*!
  \brief Get menu item text for the specified action.
  \param id menu action ID
  \return menu item text or null QString if there is no such action
*/
QString QtxWorkstackAction::text( const int id ) const
{
  QString txt;
  if ( action( id ) )
    txt = action( id )->text();
  return txt;
}

/*!
  \brief Get status bar tip for the specified action.
  \param id menu action ID
  \return status bar tip menu item or null QString if there is no such action
*/
QString QtxWorkstackAction::statusTip( const int id ) const
{
  QString txt;
  if ( action( id ) )
    txt = action( id )->statusTip();
  return txt;
}

/*!
  \brief Set keyboard accelerator for the specified action.
  \param id menu action ID
  \param a new keyboard accelerator
*/
void QtxWorkstackAction::setAccel( const int id, const int a )
{
  if ( action( id ) )
    action( id )->setShortcut( a );
}

/*!
  \brief Set menu item icon for the specified action.
  \param id menu action ID
  \param ico new menu item icon
*/
void QtxWorkstackAction::setIcon( const int id, const QIcon& icon )
{
  if ( action( id ) )
    action( id )->setIcon( icon );
}

/*!
  \brief Set menu item text for the specified action.
  \param id menu action ID
  \param txt new menu item text
*/
void QtxWorkstackAction::setText( const int id, const QString& txt )
{
  if ( action( id ) )
    action( id )->setText( txt );
}

/*!
  \brief Set menu item status bar tip for the specified action.
  \param id menu action ID
  \param txt new menu item status bar tip
*/
void QtxWorkstackAction::setStatusTip( const int id, const QString& txt )
{
  if ( action( id ) )
    action( id )->setStatusTip( txt );
}

/*!
  \brief Process action activated by the user.
  \param type action ID
*/
void QtxWorkstackAction::perform( const int type )
{
  switch ( type )
  {
  case SplitVertical:
    splitVertical();
    break;
  case SplitHorizontal:
    splitHorizontal();
    break;
  }
}

/*!
  \brief Split the window area in the workstack in the vertical direction.
*/
void QtxWorkstackAction::splitVertical()
{
  QtxWorkstack* ws = workstack();
  if ( ws )
    ws->splitVertical();
}

/*!
  \brief Split the window area in the workstack in the horizontal direction.
*/
void QtxWorkstackAction::splitHorizontal()
{
  QtxWorkstack* ws = workstack();
  if ( ws )
    ws->splitHorizontal();
}

/*!
  \brief Called when action is added to the menu bar.
  \param w menu bar widget this action is being added to
*/
void QtxWorkstackAction::addedTo( QWidget* w )
{
  QtxActionSet::addedTo( w );

  QMenu* pm = ::qobject_cast<QMenu*>( w );
  if ( pm )
    connect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Called when action is removed from the menu bar.
  \param w menu bar widget this action is being removed from
*/
void QtxWorkstackAction::removedFrom( QWidget* w )
{
  QtxActionSet::removedFrom( w );

  QMenu* pm = ::qobject_cast<QMenu*>( w );
  if ( pm )
    disconnect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Update all menu action state.
*/
void QtxWorkstackAction::updateContent()
{
  bool count = workstack() ? workstack()->splitWindowList().count() > 1 : 0;
  action( SplitVertical )->setEnabled( count );
  action( SplitHorizontal )->setEnabled( count );

  updateWindows();
}

/*!
  \brief Update actions which refer to the opened child windows.
*/
void QtxWorkstackAction::updateWindows()
{
  QtxWorkstack* ws = workstack();
  if ( !ws )
    return;

  QList<QAction*> lst = actions();
  for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    int id = actionId( *it );
    if ( id >= Windows )
      removeAction( *it );
  }

  bool base = action( SplitVertical )->isVisible() || action( SplitHorizontal )->isVisible();

  QList<QAction*> items;
  QMap<QAction*, int> map;
  if ( menuActions() & Windows )
  {
    int index = 1;
    QWidgetList wList = ws->windowList();
    for ( QWidgetList::iterator it = wList.begin(); it != wList.end(); ++it, index++ )
    {
      QWidget* wid = *it;
      QAction* a = new QtxAction( wid->windowTitle(), wid->windowTitle(), 0, this, true );
      a->setChecked( wid == ws->activeWindow() );
      items.append( a );
      map.insert( a, Windows + index );
    }

    if ( base && !items.isEmpty() )
    {
      QAction* sep = new QtxAction( this );
      sep->setSeparator( true );
      items.prepend( sep );
      map.insert( sep, Windows );
    }
  }

  if ( !items.isEmpty() )
    insertActions( items );

  for ( QMap<QAction*, int>::const_iterator itr = map.begin(); itr != map.end(); ++itr )
    setActionId( itr.key(), itr.value() );
}

/*!
  \brief Called when parent menu is about to show.

  Updates all menu items.
*/
void QtxWorkstackAction::onAboutToShow()
{
  QMenu* pm = ::qobject_cast<QMenu*>( sender() );
  if ( pm )
    updateContent();
}

/*!
  \brief Called when menu item corresponding to some child window is activated.

  Activates correposponding child window.

  \param idx menu item index
*/
void QtxWorkstackAction::activateItem( const int idx )
{
  QtxWorkstack* ws = workstack();
  if ( !ws )
    return;

  QWidgetList wList = ws->windowList();
  if ( idx >= 0 && idx < (int)wList.count() )
    wList.at( idx )->setFocus();
}

/*!
  \brief Called when menu item is activated by the user.
  
  Perform the corresponding action.

  \param id menu item identifier
*/
void QtxWorkstackAction::onTriggered( int id )
{
  if ( id < Windows )
    perform( id );
  else
    activateItem( id - Windows - 1 );
}
