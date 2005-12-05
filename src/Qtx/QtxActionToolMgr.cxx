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
// File:      QtxActionToolMgr.cxx
// Author:    Alexander SOLOVYEV, Sergey TELKOV

#include "QtxActionToolMgr.h"

#include "QtxAction.h"
#include "QtxToolBar.h"

#include <qmainwindow.h>
#include <qobjectlist.h>

QtxActionToolMgr::QtxActionToolMgr( QMainWindow* p )
: QtxActionMgr( p ),
myMainWindow( p )
{
}

QtxActionToolMgr::~QtxActionToolMgr()
{
}

QMainWindow* QtxActionToolMgr::mainWindow() const
{
  return myMainWindow;
}

int QtxActionToolMgr::createToolBar( const QString& name, const int tid )
{
  static int _toolBarId = -1;

  int tbId = -1;
  for ( ToolBarMap::ConstIterator it = myToolBars.begin(); it != myToolBars.end() && tbId == -1; ++it )
  {
    if ( it.data().toolBar->label().lower() == name.lower() )
      tbId = it.key();
  }

  if ( tbId != -1 )
    return tbId;

  QToolBar* tb = find( name, mainWindow() );

  tbId = tid < 0 ? --_toolBarId : tid;

  myToolBars.insert( tbId, ToolBarInfo() );
  ToolBarInfo& tInfo = myToolBars[tbId];

  if ( !tb )
  {
    tb = new QtxToolBar( true, mainWindow() );
    tb->setLabel( name );
  }

  tInfo.toolBar = tb;
  connect( tInfo.toolBar, SIGNAL( destroyed() ), this, SLOT( onToolBarDestroyed() ) );

  return tbId;
}

QToolBar* QtxActionToolMgr::find( const QString& label, QMainWindow* mw ) const
{
  if ( !mw )
    return 0;

  QString pattern = label.lower();

  QToolBar* res = 0;
  QPtrList<QDockWindow> lst = mw->dockWindows();
  for ( QPtrListIterator<QDockWindow> it( lst ); it.current() && !res; ++it )
  {
    if ( !it.current()->inherits( "QToolBar" ) )
      continue;

    QToolBar* cur = (QToolBar*)it.current();
    if ( cur->label().lower() == pattern )
      res = cur;
  }
  return res;
}

void QtxActionToolMgr::removeToolBar( const int tid )
{
  if ( !myToolBars.contains( tid ) )
    return;

  delete myToolBars[tid].toolBar;
  myToolBars.remove( tid );
}

void QtxActionToolMgr::removeToolBar( const QString& tname )
{
  removeToolBar( find( tname ) );
}

int QtxActionToolMgr::insert( const int id, const int tid, const int idx )
{
  if ( !contains( id ) || !hasToolBar( tid ) )
    return -1;

  ToolNode node;
  node.id = id;

  NodeList& list = myToolBars[tid].nodes;
  int index = idx < 0 ? list.count() : QMIN( idx, (int)list.count() );
  list.insert( list.at( index ), node );
  updateToolBar( tid );

  return id;
}

int QtxActionToolMgr::insert( QAction* act, const int tid, const int pos )
{
  return insert( registerAction( act ), tid, pos );
}

int QtxActionToolMgr::insert( const int id, const QString& tname, const int pos )
{
  return insert( id, createToolBar( tname ), pos );
}

int QtxActionToolMgr::insert( QAction* act, const QString& tname, const int pos )
{
  return insert( registerAction( act ), createToolBar( tname ), pos );
}

int QtxActionToolMgr::append( const int id, const int tid )
{
  return insert( id, tid );
}

int QtxActionToolMgr::append( QAction* act, const int tid )
{
  return insert( act, tid );
}

int QtxActionToolMgr::append( const int id, const QString& tname )
{
  return insert( id, tname );
}

int QtxActionToolMgr::append( QAction* act, const QString& tname )
{
  return insert( act, tname );
}

int QtxActionToolMgr::prepend( const int id, const int tid )
{
  return insert( id, tid, 0 );
}

int QtxActionToolMgr::prepend( QAction* act, const int tid )
{
  return insert( act, tid, 0 );
}

int QtxActionToolMgr::prepend( const int id, const QString& tname )
{
  return insert( id, tname, 0 );
}

int QtxActionToolMgr::prepend( QAction* act, const QString& tname )
{
  return insert( act, tname, 0 );
}

void QtxActionToolMgr::remove( const int id, const int tid )
{
  if ( !myToolBars.contains( tid ) )
    return;

  NodeList newList;
  const NodeList& nodes = myToolBars[tid].nodes;
  for ( NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it )
  {
    if ( (*it).id != id )
      newList.append( *it );
  }

  myToolBars[tid].nodes = newList;

  updateToolBar( tid );
}

void QtxActionToolMgr::remove( const int id, const QString& tname )
{
  remove( id, find( tname ) );
}

QToolBar* QtxActionToolMgr::toolBar( const int tid ) const
{
  QToolBar* tb = 0;
  if ( myToolBars.contains( tid ) )
    tb = myToolBars[tid].toolBar;
  return tb;
}

QToolBar* QtxActionToolMgr::toolBar( const QString& tname ) const
{
  return toolBar( find( tname ) );
}

bool QtxActionToolMgr::hasToolBar( const int tid ) const
{
  return myToolBars.contains( tid );
}

bool QtxActionToolMgr::hasToolBar( const QString& tname ) const
{
  return find( tname ) != -1;
}

void QtxActionToolMgr::onToolBarDestroyed()
{
  myToolBars.remove( find( (QToolBar*)sender() ) );
}

int QtxActionToolMgr::find( const QString& tname ) const
{
  int id = -1;
  for ( ToolBarMap::ConstIterator it = myToolBars.begin(); it != myToolBars.end() && id == -1; ++it )
  {
    if ( it.data().toolBar->label() == tname )
      id = it.key();
  }
  return id;
}

int QtxActionToolMgr::find( QToolBar* t ) const
{
  int id = -1;
  for ( ToolBarMap::ConstIterator it = myToolBars.begin(); it != myToolBars.end() && id == -1; ++it )
  {
    if ( it.data().toolBar == t )
      id = it.key();
  }
  return id;
}

void QtxActionToolMgr::updateToolBar( const int tId )
{
  if ( !isUpdatesEnabled() )
    return;

  if ( !myToolBars.contains( tId ) )
    return;

  QToolBar* tb = myToolBars[tId].toolBar;
  const NodeList& list = myToolBars[tId].nodes;

  for ( NodeList::const_iterator it = list.begin(); it != list.end(); ++it )
  {
    QAction* a = action( (*it).id );
    if ( a )
      a->removeFrom( tb );
  }

  tb->clear();

  for ( NodeList::const_iterator itr = list.begin(); itr != list.end(); ++itr )
  {
    if ( !isVisible( (*itr).id, tId ) )
      continue;

    QAction* a = action( (*itr).id );
    if ( a )
      a->addTo( tb );
  }

  simplifySeparators( tb );
}

void QtxActionToolMgr::internalUpdate()
{
  for ( ToolBarMap::ConstIterator it1 = myToolBars.begin(); it1 != myToolBars.end(); ++it1 )
    updateToolBar( it1.key() );
}

void QtxActionToolMgr::simplifySeparators( QToolBar* t )
{
  if ( t )
    Qtx::simplifySeparators( t );
}

void QtxActionToolMgr::show( const int actId )
{
  setShown( actId, true );
}

void QtxActionToolMgr::hide( const int actId )
{
  setShown( actId, false );
}

void QtxActionToolMgr::setShown( const int id, const bool on )
{
  for ( ToolBarMap::Iterator it = myToolBars.begin(); it != myToolBars.end(); ++it )
    setVisible( id, it.key(), on );
}

bool QtxActionToolMgr::isShown( const int id ) const
{
  QPtrList<ToolNode> nodes;
  for ( ToolBarMap::ConstIterator it = myToolBars.begin(); it != myToolBars.end(); ++it )
  {
    const NodeList& nl = it.data().nodes;
    for ( NodeList::const_iterator itr = nl.begin(); itr != nl.end(); ++itr )
    {
      const ToolNode& node = *itr;
      if ( node.id == id )
        nodes.append( &node );
    }
  }

  if ( nodes.isEmpty() )
    return false;

  bool vis = true;
  for ( QPtrListIterator<ToolNode> itr( nodes ); itr.current() && vis; ++itr )
    vis = itr.current()->visible;

  return vis;
}

bool QtxActionToolMgr::isVisible( const int id, const int tId ) const
{
  if ( !myToolBars.contains( tId ) )
    return false;

  bool vis = false;
  const NodeList& lst = myToolBars[tId].nodes;
  for ( NodeList::const_iterator it = lst.begin(); it != lst.end() && !vis; ++it )
  {
    const ToolNode& node = *it;
    if ( node.id == id )
      vis = node.visible;
  }
  return vis;
}

void QtxActionToolMgr::setVisible( const int id, const int tId, const bool on )
{
  if ( !myToolBars.contains( tId ) )
    return;

  bool changed = false;
  NodeList& lst = myToolBars[tId].nodes;
  for ( NodeList::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    ToolNode& node = *it;
    if ( node.id == id )
    {
      changed = changed || node.visible != on;
      node.visible = on;
    }
  }

  if ( changed )
    updateToolBar( tId );
}

bool QtxActionToolMgr::load( const QString& fname, QtxActionMgr::Reader& r )
{
  ToolCreator cr( &r, this );
  return r.read( fname, cr );
}


/*!
	Class: QtxActionToolMgr::ToolCreator
	Level: Public
*/
QtxActionToolMgr::ToolCreator::ToolCreator( QtxActionMgr::Reader* r,
                                            QtxActionToolMgr* mgr )
: QtxActionMgr::Creator( r ),
  myMgr( mgr )
{
}

QtxActionToolMgr::ToolCreator::~ToolCreator()
{
}

int QtxActionToolMgr::ToolCreator::append( const QString& tag, const bool subMenu,
                                           const ItemAttributes& attr, const int tId )
{  
  if( !myMgr || !reader() )
    return -1;

  QString label   = reader()->option( "label",     "label"     ),
          id      = reader()->option( "id",        "id"        ),
          pos     = reader()->option( "pos",       "pos"       ),
          group   = reader()->option( "group",     "group"     ),
          tooltip = reader()->option( "tooltip",   "tooltip"   ),
          sep     = reader()->option( "separator", "separator" ),
          accel   = reader()->option( "accel",     "accel"     ),
          icon    = reader()->option( "icon",      "icon"      ),
          toggle  = reader()->option( "toggle",    "toggle"    );

  int res = -1, actId = intValue( attr, id, -1 );
  if( tId==-1 )
    res = myMgr->createToolBar( strValue( attr, label ), intValue( attr, id, -1 ) );
  else if( tag==sep )
    res = myMgr->insert( separator(), tId, intValue( attr, pos, -1 ) );
  else
  {
    QPixmap pix; QIconSet set;
    QString name = strValue( attr, icon );
    if( !name.isEmpty() && loadPixmap( name, pix ) )
      set = QIconSet( pix );

    QtxAction* newAct = new QtxAction( strValue( attr, tooltip ), set,
                                       strValue( attr, label ), 
                                       QKeySequence( strValue( attr, accel ) ),
                                       myMgr );
    QString toggleact = strValue( attr, toggle );
    newAct->setToggleAction( !toggleact.isEmpty() );
    newAct->setOn( toggleact.lower()=="true" );
        
    connect( newAct );
    int aid = myMgr->registerAction( newAct, actId );
    res = myMgr->insert( aid, tId, intValue( attr, pos, -1 ) );
  }

  return res;
}


