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

// File:      QtxWorkspaceAction.cxx
// Author:    Sergey TELKOV
//
#include "QtxWorkspaceAction.h"

#include "QtxWorkspace.h"

#include <QMenu>
#include <QMdiSubWindow>

/*!
  \class QtxWorkspaceAction
  \brief Implements actions group for menu Windows with standard operations, like
         "Cascade", "Tile", "Tile Horizontally", etc.
*/

/*!
  \brief Constructor.
  \param ws parent workspace
  \param parent parent object (owner of the action)
*/
QtxWorkspaceAction::QtxWorkspaceAction( QtxWorkspace* ws, QObject* parent )
: QtxActionSet( parent ),
  myWorkspace( ws ),
  myWindowsFlag( true )
{
  insertAction( new QtxAction( tr( "Arranges the windows as overlapping tiles" ),
                               tr( "Cascade" ), 0, this ), Cascade );
  insertAction( new QtxAction( tr( "Arranges the windows as nonoverlapping tiles" ),
                               tr( "Tile" ), 0, this ), Tile );
  insertAction( new QtxAction( tr( "Arranges the windows as nonoverlapping horizontal tiles" ),
                               tr( "Tile horizontally" ), 0, this ), HTile );
  insertAction( new QtxAction( tr( "Arranges the windows as nonoverlapping vertical tiles" ),
                               tr( "Tile vertically" ), 0, this ), VTile );

  connect( this, SIGNAL( triggered( int ) ), this, SLOT( onTriggered( int ) ) );

  setMenuActions( Standard );
}

/*!
  \brief Destructor.
*/
QtxWorkspaceAction::~QtxWorkspaceAction()
{
}

/*!
  \brief Get workspace.
  \return parent workspace
*/
QtxWorkspace* QtxWorkspaceAction::workspace() const
{
  return myWorkspace;
}

/*!
  \brief Set actions to be visible in the menu.
  
  Actions, which IDs are set in \a flags parameter, will be shown in the 
  menu bar. Other actions will not be shown.

  \param flags ORed together actions flags
*/
void QtxWorkspaceAction::setMenuActions( const int flags )
{
  action( Cascade )->setVisible( flags & Cascade );
  action( Tile )->setVisible( flags & Tile );
  action( VTile )->setVisible( flags & VTile );
  action( HTile )->setVisible( flags & HTile );
  myWindowsFlag = flags & Windows;
}

/*!
  \brief Get menu actions which are currently visible in the menu bar.
  \return ORed together actions flags
  \sa setMenuActions()
*/
int QtxWorkspaceAction::menuActions() const
{
  int ret = 0;
  ret = ret | ( action( Cascade )->isVisible() ? Cascade : 0 );
  ret = ret | ( action( Tile )->isVisible() ? Tile : 0 );
  ret = ret | ( action( VTile )->isVisible() ? VTile : 0 );
  ret = ret | ( action( HTile )->isVisible() ? HTile : 0 );
  ret = ret | ( myWindowsFlag ? Windows : 0 );
  return ret;
}

/*!
  \brief Get keyboard accelerator for the specified action.
  \param id menu action ID
  \return keyboard accelerator of menu item or 0 if there is no such action
*/
int QtxWorkspaceAction::accel( const int id ) const
{
  int a = 0;
  if ( action( id ) )
    a = action( id )->shortcut()[0];
  return a;
}

/*!
  \brief Get icon for the specified action.

  If \a id is invalid, null icon is returned.

  \param id menu action ID
  \return menu item icon
*/
QIcon QtxWorkspaceAction::icon( const int id ) const
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
QString QtxWorkspaceAction::text( const int id ) const
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
QString QtxWorkspaceAction::statusTip( const int id ) const
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
void QtxWorkspaceAction::setAccel( const int id, const int a )
{
  if ( action( id ) )
    action( id )->setShortcut( a );
}

/*!
  \brief Set menu item icon for the specified action.
  \param id menu action ID
  \param ico new menu item icon
*/
void QtxWorkspaceAction::setIcon( const int id, const QIcon& icon )
{
  if ( action( id ) )
    action( id )->setIcon( icon );
}

/*!
  \brief Set menu item text for the specified action.
  \param id menu action ID
  \param txt new menu item text
*/
void QtxWorkspaceAction::setText( const int id, const QString& txt )
{
  if ( action( id ) )
    action( id )->setText( txt );
}

/*!
  \brief Set menu item status bar tip for the specified action.
  \param id menu action ID
  \param txt new menu item status bar tip
*/
void QtxWorkspaceAction::setStatusTip( const int id, const QString& txt )
{
  if ( action( id ) )
    action( id )->setStatusTip( txt );
}

/*!
  \brief Process action activated by the user.
  \param type action ID
*/
void QtxWorkspaceAction::perform( const int type )
{
  switch ( type )
  {
  case Cascade:
    cascade();
    break;
  case Tile:
    tile();
    break;
  case VTile:
    tileVertical();
    break;
  case HTile:
    tileHorizontal();
    break;
  }
}

/*!
  \brief Tile child windows in the workspace.
*/
void QtxWorkspaceAction::tile()
{
  QtxWorkspace* ws = workspace();
  if ( ws )
    ws->tileSubWindows();
}

/*!
  \brief Cascade child windows in the workspace.
*/
void QtxWorkspaceAction::cascade()
{
  QtxWorkspace* ws = workspace();
  if ( !ws )
    return;

  ws->cascadeSubWindows();

        int w = ws->width();
        int h = ws->height();

        QList<QMdiSubWindow *> winList = ws->subWindowList();
  for ( QList<QMdiSubWindow *>::iterator it = winList.begin(); it != winList.end(); ++it )
                (*it)->resize( int( w * 0.8 ), int( h * 0.8 ) );
}

/*!
  \brief Tile child windows in the workspace in the vertical direction.
*/
void QtxWorkspaceAction::tileVertical()
{
  QtxWorkspace* ws = workspace();
  if ( ws )
    ws->tileVertical();
}

/*!
  \brief Tile child windows in the workspace in the horizontal direction.
*/
void QtxWorkspaceAction::tileHorizontal()
{
  QtxWorkspace* ws = workspace();
  if ( ws )
    ws->tileHorizontal();
}

/*!
  \brief Called when action is added to the menu bar.
  \param w menu bar widget this action is being added to
*/
void QtxWorkspaceAction::addedTo( QWidget* w )
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
void QtxWorkspaceAction::removedFrom( QWidget* w )
{
  QtxActionSet::removedFrom( w );

  QMenu* pm = ::qobject_cast<QMenu*>( w );
  if ( pm )
    disconnect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Update all menu action state.
*/
void QtxWorkspaceAction::updateContent()
{
  bool hasWindows = workspace() && workspace()->subWindowList().count() > 0;
  action( Cascade )->setEnabled( hasWindows );
  action( Tile )->setEnabled( hasWindows );
  action( HTile )->setEnabled( hasWindows );
  action( VTile )->setEnabled( hasWindows );

  updateWindows();
}

/*!
  \brief Update actions which refer to the opened child windows.
*/
void QtxWorkspaceAction::updateWindows()
{
  QtxWorkspace* ws = workspace();
  if ( !ws )
    return;

  QList<QAction*> lst = actions();
  for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    int id = actionId( *it );
    if ( id >= Windows )
      removeAction( *it );
  }

  bool base = action( Cascade )->isVisible() || action( Tile )->isVisible() ||
              action( HTile )->isVisible() || action( VTile )->isVisible();

  QList<QAction*> items;
  QMap<QAction*, int> map;
  if ( menuActions() & Windows )
  {
    int index = 1;
    QList<QMdiSubWindow *> wList = ws->subWindowList();
    for ( QList<QMdiSubWindow *>::iterator it = wList.begin(); it != wList.end(); ++it, index++ )
    {
      QWidget* wid = *it;
      QAction* a = new QtxAction( wid->windowTitle(), wid->windowTitle(), 0, this, true );
      a->setChecked( wid == ws->activeSubWindow() );
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
void QtxWorkspaceAction::onAboutToShow()
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
void QtxWorkspaceAction::activateItem( const int idx )
{
  QtxWorkspace* ws = workspace();
  if ( !ws )
    return;

  QList<QMdiSubWindow *> wList = ws->subWindowList();
  if ( idx >= 0 && idx < (int)wList.count() )
    wList.at( idx )->setFocus();
}

/*!
  \brief Called when menu item is activated by the user.
  
  Perform the corresponding action.

  \param id menu item identifier
*/
void QtxWorkspaceAction::onTriggered( int id )
{
  if ( id < Windows )
    perform( id );
  else
    activateItem( id - Windows - 1 );
}
