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
#include "STD_MDIDesktop.h"

#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ResourceMgr.h>

#include <QtxAction.h>
#include <QtxActionMenuMgr.h>
#include <QtxWorkspaceAction.h>

#include <qvbox.h>
#include <qmenubar.h>
#include <qworkspace.h>
#include <qobjectlist.h>

#include <stdarg.h>

/*!Constructor.*/
STD_MDIDesktop::STD_MDIDesktop()
: SUIT_Desktop(),
myWorkspace( 0 ),
myWorkspaceAction( 0 )
{
  QVBox* base = new QVBox( this );
  base->setFrameStyle( QFrame::Panel | QFrame::Sunken );

  setCentralWidget( base );

  myWorkspace = new QWorkspace( base );

  connect( myWorkspace, SIGNAL( windowActivated( QWidget* ) ),
           this, SLOT( onWindowActivated( QWidget* ) ) );

  createActions();
}

/*!destructor.*/
STD_MDIDesktop::~STD_MDIDesktop()
{
}

/*!\retval SUIT_ViewWindow - return const active window.*/
SUIT_ViewWindow* STD_MDIDesktop::activeWindow() const
{
  SUIT_ViewWindow* wnd = 0;

  QWidget* wid = myWorkspace->activeWindow();
  if ( wid && wid->inherits( "SUIT_ViewWindow" ) )
    wnd = (SUIT_ViewWindow*)wid;

  return wnd;
}

/*!\retval QPtrList<SUIT_ViewWindow> - return const active window list.*/
QPtrList<SUIT_ViewWindow> STD_MDIDesktop::windows() const
{
  QPtrList<SUIT_ViewWindow> winList;

  QWidgetList children = myWorkspace->windowList();
  for ( QWidgetListIt it( children ); it.current(); ++it )
  {
    if ( it.current()->inherits( "SUIT_ViewWindow" ) )
      winList.append( (SUIT_ViewWindow*)it.current() );
  }

  return winList;
}

/*!\retval QWidget - pointer to work space.*/
QWidget* STD_MDIDesktop::parentArea() const
{
  return workspace();
}

/*!Call method perform for operation \a type.*/
void STD_MDIDesktop::windowOperation( const int type )
{
  myWorkspaceAction->perform( operationFlag( type ) );
}

/*!Sets window operations by \a first ... parameters.*/
void STD_MDIDesktop::setWindowOperations( const int first, ... )
{
  va_list ints;
	va_start( ints, first );

	QValueList<int> typeList;

	int cur = first;
	while ( cur )
	{
	  typeList.append( cur );
		cur = va_arg( ints, int );
  }

	setWindowOperations( typeList );
}

/*!Sets window operations by variable \a opList - operation list.*/
void STD_MDIDesktop::setWindowOperations( const QValueList<int>& opList )
{
  int flags = 0;

  for ( QValueList<int>::const_iterator it = opList.begin(); it != opList.end(); ++it )
    flags = flags | operationFlag( *it );

  myWorkspaceAction->setItems( flags );
}

/*!\retval QWorkspace pointer - work space.*/
QWorkspace* STD_MDIDesktop::workspace() const
{
  return myWorkspace;
}

/*!Emit window activated.*/
void STD_MDIDesktop::onWindowActivated( QWidget* w )
{
  if ( w && w->inherits( "SUIT_ViewWindow" ) )
    emit windowActivated( (SUIT_ViewWindow*)w );
}

/*!Create actions: cascade, Tile, Tile Horizontal, Tile Vertical*/
void STD_MDIDesktop::createActions()
{
  if ( myWorkspaceAction )
    return;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if ( !resMgr )
    return;

  myWorkspaceAction = new QtxWorkspaceAction( workspace(), this );

  myWorkspaceAction->setItems( QtxWorkspaceAction::Cascade | QtxWorkspaceAction::Tile |
                               QtxWorkspaceAction::HTile | QtxWorkspaceAction::VTile |
                               QtxWorkspaceAction::Windows );

  // Cascade
  myWorkspaceAction->setIconSet( QtxWorkspaceAction::Cascade,
                                 resMgr->loadPixmap( "STD", tr( "ICON_DESK_WINDOW_CASCADE" ) ) );
  myWorkspaceAction->setMenuText( QtxWorkspaceAction::Cascade, tr( "MEN_DESK_WINDOW_CASCADE" ) );
  myWorkspaceAction->setStatusTip( QtxWorkspaceAction::Cascade, tr( "PRP_DESK_WINDOW_CASCADE" ) );

  // Tile
  myWorkspaceAction->setIconSet( QtxWorkspaceAction::Tile,
                                 resMgr->loadPixmap( "STD", tr( "ICON_DESK_WINDOW_TILE" ) ) );
  myWorkspaceAction->setMenuText( QtxWorkspaceAction::Tile, tr( "MEN_DESK_WINDOW_TILE" ) );
  myWorkspaceAction->setStatusTip( QtxWorkspaceAction::Tile, tr( "PRP_DESK_WINDOW_TILE" ) );

  // Tile Horizontal
  myWorkspaceAction->setIconSet( QtxWorkspaceAction::HTile,
                                 resMgr->loadPixmap( "STD", tr( "ICON_DESK_WINDOW_HTILE" ) ) );
  myWorkspaceAction->setMenuText( QtxWorkspaceAction::HTile, tr( "MEN_DESK_WINDOW_HTILE" ) );
  myWorkspaceAction->setStatusTip( QtxWorkspaceAction::HTile, tr( "PRP_DESK_WINDOW_HTILE" ) );

  // Tile Vertical
  myWorkspaceAction->setIconSet( QtxWorkspaceAction::VTile,
                                 resMgr->loadPixmap( "STD", tr( "ICON_DESK_WINDOW_VTILE" ) ) );
  myWorkspaceAction->setMenuText( QtxWorkspaceAction::VTile, tr( "MEN_DESK_WINDOW_VTILE" ) );
  myWorkspaceAction->setStatusTip( QtxWorkspaceAction::VTile, tr( "PRP_DESK_WINDOW_VTILE" ) );


  QtxActionMenuMgr* mMgr = menuMgr();
  if ( !mMgr )
    return;

  int winMenuId = mMgr->insert( tr( "MEN_DESK_WINDOW" ), -1, 100, MenuWindowId );
  mMgr->insert( myWorkspaceAction, winMenuId, -1 );
  mMgr->insert( QtxActionMenuMgr::separator(), winMenuId, -1 );
}

/*!Convert STD_MDIDesktop enumerations to QtxWorkspaceAction.*/
int STD_MDIDesktop::operationFlag( const int type ) const
{
  int res = 0;
  switch ( type )
  {
  case Cascade:
    res = QtxWorkspaceAction::Cascade;
    break;
  case Tile:
    res = QtxWorkspaceAction::Tile;
    break;
  case HTile:
    res = QtxWorkspaceAction::HTile;
    break;
  case VTile:
    res = QtxWorkspaceAction::VTile;
    break;
  }
  return res;
}
