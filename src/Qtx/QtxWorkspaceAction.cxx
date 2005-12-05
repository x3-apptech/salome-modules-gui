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
// See http://www.salome-platform.org/
//
// File:      QtxWorkspaceAction.cxx
// Author:    Sergey TELKOV

#include "QtxWorkspaceAction.h"

#include <qpopupmenu.h>
#include <qworkspace.h>
#include <qwidgetlist.h>

QtxWorkspaceAction::QtxWorkspaceAction( QWorkspace* ws, QObject* parent, const char* name )
: QtxAction( tr( "Controls windows into workspace" ), tr( "Workspace management" ), 0, parent, name ),
myFlags( Standard ),
myWorkspace( ws )
{
  myItem.insert( Cascade, new QtxAction( tr( "Arranges the windows as overlapping tiles" ),
                                         tr( "Cascade" ), 0, this, 0, false ) );
  myItem.insert( Tile,    new QtxAction( tr( "Arranges the windows as nonoverlapping tiles" ),
                                         tr( "Tile" ), 0, this, 0, false ) );
  myItem.insert( HTile,   new QtxAction( tr( "Arranges the windows as nonoverlapping horizontal tiles" ),
                                         tr( "Tile horizontally" ), 0, this, 0, false ) );
  myItem.insert( VTile,   new QtxAction( tr( "Arranges the windows as nonoverlapping vertical tiles" ),
                                         tr( "Tile vertically" ), 0, this, 0, false ) );

  connect( myItem[Tile], SIGNAL( activated() ), this, SLOT( tile() ) );
  connect( myItem[Cascade], SIGNAL( activated() ), this, SLOT( cascade() ) );
  connect( myItem[HTile], SIGNAL( activated() ), this, SLOT( tileVertical() ) );
  connect( myItem[VTile], SIGNAL( activated() ), this, SLOT( tileHorizontal() ) );
}

QtxWorkspaceAction::~QtxWorkspaceAction()
{
}

QWorkspace* QtxWorkspaceAction::workspace() const
{
  return myWorkspace;
}

int QtxWorkspaceAction::items() const
{
  return myFlags;
}

void QtxWorkspaceAction::setItems( const int flags )
{
  if ( !flags || flags == myFlags || !( flags & Operations ) )
    return;

  myFlags = flags;
}

bool QtxWorkspaceAction::hasItems( const int flags ) const
{
  return ( myFlags & flags ) == flags;
}

int QtxWorkspaceAction::accel( const int id ) const
{
  int a = 0;
  if ( myItem.contains( id ) )
    a = myItem[id]->accel();
  return a;
}

QIconSet QtxWorkspaceAction::iconSet( const int id ) const
{
  QIconSet ico;
  if ( myItem.contains( id ) )
    ico = myItem[id]->iconSet();
  return ico;
}

QString QtxWorkspaceAction::menuText( const int id ) const
{
  QString txt;
  if ( myItem.contains( id ) )
    txt = myItem[id]->menuText();
  return txt;
}

QString QtxWorkspaceAction::statusTip( const int id ) const
{
  QString txt;
  if ( myItem.contains( id ) )
    txt = myItem[id]->statusTip();
  return txt;
}

void QtxWorkspaceAction::setAccel( const int id, const int a )
{
  if ( myItem.contains( id ) )
    myItem[id]->setAccel( a );
}

void QtxWorkspaceAction::setIconSet( const int id, const QIconSet& ico )
{
  if ( myItem.contains( id ) )
    myItem[id]->setIconSet( ico );
}

void QtxWorkspaceAction::setMenuText( const int id, const QString& txt )
{
  if ( myItem.contains( id ) )
    myItem[id]->setMenuText( txt );
}

void QtxWorkspaceAction::setStatusTip( const int id, const QString& txt )
{
  if ( myItem.contains( id ) )
    myItem[id]->setStatusTip( txt );
}

bool QtxWorkspaceAction::addTo( QWidget* wid )
{
  return addTo( wid, -1 );
}

bool QtxWorkspaceAction::addTo( QWidget* wid, const int idx )
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

bool QtxWorkspaceAction::removeFrom( QWidget* wid )
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

void QtxWorkspaceAction::tile()
{
  QWorkspace* ws = workspace();
  if ( !ws )
    return;

  ws->tile();
}

void QtxWorkspaceAction::cascade()
{
  QWorkspace* ws = workspace();
  if ( !ws )
    return;

  ws->cascade();

	int w = ws->width();
	int h = ws->height();

	QWidgetList winList = ws->windowList();
	for ( QWidgetListIt it( winList ); it.current(); ++it )
		it.current()->resize( int( w * 0.8 ), int( h * 0.8 ) );
}

void QtxWorkspaceAction::tileVertical()
{
  QWorkspace* wrkSpace = workspace();
	if ( !wrkSpace )
		return;
	
	QWidgetList winList = wrkSpace->windowList();
	if ( winList.isEmpty() )
    return;

  int count = 0;
	for ( QWidgetListIt itr( winList ); itr.current(); ++itr )
    if ( !itr.current()->testWState( WState_Minimized ) )
      count++;

  if ( !count )
    return;

	int y = 0;

	int heightForEach = wrkSpace->height() / count;
	for ( QWidgetListIt it( winList ); it.current(); ++it )
	{
    QWidget* win = it.current();
    if ( win->testWState( WState_Minimized ) )
      continue;

    if ( win->testWState( WState_Maximized ) )
		{
			win->hide();
			win->showNormal();
    }
    int prefH = win->minimumHeight() + win->parentWidget()->baseSize().height();
    int actualH = QMAX( heightForEach, prefH );

    win->parentWidget()->setGeometry( 0, y, wrkSpace->width(), actualH );
    y += actualH;
	}
}

void QtxWorkspaceAction::tileHorizontal()
{
  QWorkspace* wrkSpace = workspace();
	if ( !wrkSpace )
		return;

	QWidgetList winList = wrkSpace->windowList();
	if ( winList.isEmpty() )
    return;

  int count = 0;
	for ( QWidgetListIt itr( winList ); itr.current(); ++itr )
    if ( !itr.current()->testWState( WState_Minimized ) )
      count++;

  if ( !count )
    return;

	int x = 0;
	int widthForEach = wrkSpace->width() / count;
	for ( QWidgetListIt it( winList ); it.current(); ++it )
	{
    QWidget* win = it.current();
    if ( win->testWState( WState_Minimized ) )
      continue;

    if ( win->testWState( WState_Maximized ) )
		{
			win->hide();
			win->showNormal();
    }
    int prefW = win->minimumWidth();
    int actualW = QMAX( widthForEach, prefW );
        
		win->parentWidget()->setGeometry( x, 0, actualW, wrkSpace->height() );
    x += actualW;
	}
}

void QtxWorkspaceAction::onAboutToShow()
{
  const QObject* obj = sender();
  if ( !obj || !obj->inherits( "QPopupMenu" ) )
    return;

  updatePopup( (QPopupMenu*)obj );
}

void QtxWorkspaceAction::onPopupDestroyed( QObject* obj )
{
  myMenu.remove( (QPopupMenu*)obj );
}

void QtxWorkspaceAction::checkPopup( QPopupMenu* pm )
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

void QtxWorkspaceAction::updatePopup( QPopupMenu* pm )
{
  if ( !myMenu.contains( pm ) )
    return;

  fillPopup( pm, clearPopup( pm ) );

  bool count = workspace() ? workspace()->windowList().count() : 0;
  myItem[Cascade]->setEnabled( count );
  myItem[Tile]->setEnabled( count );
  myItem[HTile]->setEnabled( count );
  myItem[VTile]->setEnabled( count );
}

int QtxWorkspaceAction::clearPopup( QPopupMenu* pm )
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

void QtxWorkspaceAction::fillPopup( QPopupMenu* pm, const int idx )
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

  QWorkspace* ws = workspace();
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

void QtxWorkspaceAction::onItemActivated( int idx )
{
  QWorkspace* ws = workspace();
  if ( !ws )
    return;

  QWidgetList wList = ws->windowList();
  if ( idx < 0 || idx >= (int)wList.count() )
    return;

  wList.at( idx )->setFocus();
}
