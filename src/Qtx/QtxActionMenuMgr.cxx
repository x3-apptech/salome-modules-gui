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
// File:      QtxActionMenuMgr.cxx
// Author:    Alexander SOLOVYEV, Sergey TELKOV

#include "QtxActionMenuMgr.h"

#include "QtxAction.h"

#include <qwidget.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qwidgetlist.h>
#include <qobjectlist.h>
#include <qmainwindow.h>
#include <qfile.h>
#include <qdom.h>

/*!
	Class: QtxActionMenuMgr::MenuAction
	Level: Internal
*/

class QtxActionMenuMgr::MenuAction : public QtxAction
{
public:
  MenuAction( const QString&, const QString&, QObject*, const bool = false );
  virtual ~MenuAction();

  virtual bool addTo( QWidget* );

  virtual bool removeFrom( QWidget* );

  QPopupMenu*  popup() const;

private:
  int          myId;
  QPopupMenu*  myPopup;
  bool         myEmptyEnabled;
};

QtxActionMenuMgr::MenuAction::MenuAction( const QString& text,
					  const QString& menuText,
					  QObject*       parent,
					  const bool     allowEmpty )
: QtxAction( text, menuText, 0, parent ),
  myId( -1 ),
  myPopup( 0 ),
  myEmptyEnabled( allowEmpty )
{
  myPopup = new QPopupMenu();
}

QtxActionMenuMgr::MenuAction::~MenuAction()
{
  delete myPopup;
}

bool QtxActionMenuMgr::MenuAction::addTo( QWidget* w )
{
  if ( myId != -1 || !w )
    return false;

  if ( !w->inherits( "QPopupMenu" ) && !w->inherits( "QMenuBar" ) )
    return false;

  if ( !myPopup )
    return false;

  if ( !myEmptyEnabled && !myPopup->count() )
    return false;

  if ( w->inherits( "QPopupMenu" ) && QAction::addTo( w ) )
  {
    QPopupMenu* pm = (QPopupMenu*)w;
    myId = pm->idAt( pm->count() - 1 );
    setPopup( pm, myId, myPopup );
  }
  else if ( w->inherits( "QMenuBar" ) )
  {
    QMenuBar* mb = (QMenuBar*)w;
    myId = iconSet().isNull() ? mb->insertItem( menuText(), myPopup ) :
                                mb->insertItem( iconSet(), menuText(), myPopup );
    mb->setItemEnabled( myId, isEnabled() );
  }
  else
    return false;

  return true;
}

bool QtxActionMenuMgr::MenuAction::removeFrom( QWidget* w )
{
  if ( w->inherits( "QPopupMenu" ) && QAction::removeFrom( w ) )
    myId = -1;
  else if ( w->inherits( "QMenuBar" ) )
  {
    QMenuBar* mb = (QMenuBar*)w;
    mb->removeItem( myId );
    myId = -1;
  }

  return myId == -1;
}

QPopupMenu* QtxActionMenuMgr::MenuAction::popup() const
{
  return myPopup;
}

/*!
	Class: QtxActionMenuMgr
	Level: Public
*/


QtxActionMenuMgr::QtxActionMenuMgr( QMainWindow* p )
: QtxActionMgr( p ),
myMenu( p ? p->menuBar() : 0 )
{
  myRoot.id = -1;
  myRoot.group = -1;

  if ( myMenu ) {
    connect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
    if ( myMenu->inherits( "QMenuBar" ) )
      connect( myMenu, SIGNAL( highlighted( int ) ), this, SLOT( onHighlighted( int ) ) );
  }
}

QtxActionMenuMgr::QtxActionMenuMgr( QWidget* mw, QObject* p )
: QtxActionMgr( p ),
myMenu( mw )
{
  myRoot.id = -1;
  myRoot.group = -1;

  if ( myMenu )
    connect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
}

QtxActionMenuMgr::~QtxActionMenuMgr()
{
  for ( NodeListIterator it( myRoot.children ); it.current() && myMenu; ++it )
  {
    QAction* a = itemAction( it.current()->id );
    if ( !a )
      a = menuAction( it.current()->id );

    if ( a )
      a->removeFrom( myMenu );
  }

  for ( MenuMap::Iterator itr = myMenus.begin(); itr != myMenus.end(); ++itr )
    delete itr.data();
}

bool QtxActionMenuMgr::isVisible( const int actId, const int place ) const
{
  MenuNode* node = find( actId, place );
  return node && node->visible;
}

void QtxActionMenuMgr::setVisible( const int actId, const int place, const bool v )
{
  MenuNode* node = find( actId, place );
  if ( node )
    node->visible = v;
}

int QtxActionMenuMgr::insert( const int id, const QString& menus, const int group, const int idx )
{
  return insert( id, QStringList::split( "|", menus ), group, idx );
}

int QtxActionMenuMgr::insert( QAction* a, const QString& menus, const int group, const int idx )
{
  return insert( a, QStringList::split( "|", menus ), group, idx );
}

int QtxActionMenuMgr::insert( const int id, const QStringList& menus, const int group, const int idx )
{
  int pId = createMenu( menus, -1 );
  if ( pId == -1 )
    return -1;

  return insert( id, pId, group, idx );
}

int QtxActionMenuMgr::insert( QAction* a, const QStringList& menus, const int group, const int idx )
{
  int pId = createMenu( menus, -1 );
  if ( pId == -1 )
    return -1;

  return insert( a, pId, group, idx );
}

int QtxActionMenuMgr::insert( const int id, const int pId, const int group, const int idx )
{
  if ( id == -1 )
    return -1;

  MenuNode* pNode = pId == -1 ? &myRoot : find( pId );
  if ( !pNode )
    return -1;

  MenuNode* node = new MenuNode( pNode );
  node->id = id;
  node->idx = idx;
  node->group = group;

  pNode->children.append( node );

  updateMenu( pNode, false );

  return node->id;
}

int QtxActionMenuMgr::insert( QAction* a, const int pId, const int group, const int idx )
{
  return insert( registerAction( a ), pId, group, idx );
}

int QtxActionMenuMgr::insert( const QString& title, const int pId, const int group, const int id, const int idx, const bool allowEmpty )
{
  MenuNode* pNode = pId == -1 ? &myRoot : find( pId );
  if ( !pNode )
    return -1;

  MenuNode* eNode = id == -1 ? 0 : find( id );

  int fid = -1;
  for ( NodeListIterator it( pNode->children ); it.current() && fid == -1; ++it )
  {
    if ( myMenus.contains( it.current()->id ) &&
         clearTitle( myMenus[it.current()->id]->menuText() ) == clearTitle( title ) )
      fid = it.current()->id;
  }

  if ( fid != -1 )
    return fid;

  MenuAction* ma = new MenuAction( clearTitle( title ), title, this, allowEmpty );
  connect( ma->popup(), SIGNAL( highlighted( int ) ), this, SLOT( onHighlighted( int ) ) );

  MenuNode* node = new MenuNode( pNode );
  node->group = group;
  node->idx = idx;
  node->id = myMenus.insert( (id == -1 || eNode ) ? generateId() : id, ma ).key();

  pNode->children.append( node );

  updateMenu( pNode, false );

  return node->id;
}

int QtxActionMenuMgr::insert( const QString& title, const QString& menus, const int group, const int id, const int idx, const bool allowEmpty )
{
  return insert( title, QStringList::split( "|", menus ), group, id, idx, allowEmpty );
}

int QtxActionMenuMgr::insert( const QString& title, const QStringList& menus, const int group, const int id, const int idx, const bool allowEmpty )
{
  int pId = createMenu( menus, -1 );
  return insert( title, pId, group, id, idx, allowEmpty );
}

int QtxActionMenuMgr::append( const QString& title, const int pId, const int group, const int id, const bool allowEmpty )
{
  return insert( title, pId, group, id, allowEmpty );
}

int QtxActionMenuMgr::append( const int id, const int pId, const int group )
{
  return insert( id, pId, group );
}

int QtxActionMenuMgr::append( QAction* a, const int pId, const int group )
{
  return insert( a, pId, group );
}

int QtxActionMenuMgr::prepend( const QString& title, const int pId, const int group, const int id, const bool allowEmpty )
{
  return insert( title, pId, group, id, 0, allowEmpty );
}

int QtxActionMenuMgr::prepend( const int id, const int pId, const int group )
{
  return insert( id, pId, group, 0 );
}

int QtxActionMenuMgr::prepend( QAction* a, const int pId, const int group )
{
  return insert( a, pId, group, 0 );
}

void QtxActionMenuMgr::remove( const int id )
{
  removeMenu( id, 0 );
  update();
}

void QtxActionMenuMgr::remove( const int id, const int pId, const int group )
{
  MenuNode* pNode = pId == -1 ? &myRoot : find( pId );
  if ( !pNode )
    return;

  NodeList delNodes;
  for ( NodeListIterator it( pNode->children ); it.current(); ++it )
  {
    if ( it.current()->id == id && ( it.current()->group == group || group == -1 ) )
      delNodes.append( it.current() );
  }

  for ( NodeListIterator itr( delNodes ); itr.current(); ++itr )
    pNode->children.remove( itr.current() );

  updateMenu( pNode, false );
}

void QtxActionMenuMgr::show( const int id )
{
  setShown( id, true );
}

void QtxActionMenuMgr::hide( const int id )
{
  setShown( id, false );
}

bool QtxActionMenuMgr::isShown( const int id ) const
{
  bool res = false;
  MenuNode* node = find( id );
  if ( node )
    res = node->visible;
  return res;
}

void QtxActionMenuMgr::setShown( const int id, const bool on )
{
  NodeList aNodes;
  find( id, aNodes );

  QMap<MenuNode*, int> updMap;
  for ( NodeListIterator it( aNodes ); it.current(); ++it )
  {
    if ( it.current()->visible != on )
    {
      it.current()->visible = on;
      updMap.insert( it.current()->parent, 0 );
    }
  }

  for ( QMap<MenuNode*, int>::ConstIterator itr = updMap.begin(); itr != updMap.end(); ++itr )
    updateMenu( itr.key(), false );
}

void QtxActionMenuMgr::onDestroyed( QObject* obj )
{
  if ( myMenu == obj )
    myMenu = 0;
}

void QtxActionMenuMgr::onHighlighted( int id )
{
  const QObject* snd = sender();
  int pid = 0, realId;
  if ( myMenu && snd == myMenu )
    pid = -1;
  else {
    for ( MenuMap::Iterator itr = myMenus.begin(); itr != myMenus.end(); ++itr ) {
      if ( itr.data()->popup() && itr.data()->popup() == snd )
	pid = itr.key();
    }
  }
  if ( pid ) {
    realId = findId( id, pid );
    if ( realId != -1 ) {
      bool updatesEnabled = isUpdatesEnabled();
      setUpdatesEnabled( false );
      emit menuHighlighted( pid, realId );
      setUpdatesEnabled( updatesEnabled );
      updateMenu( find( realId ) );
    }
  }
}

void QtxActionMenuMgr::setWidget( QWidget* mw )
{
  if ( myMenu == mw )
    return;

  if ( myMenu )
    disconnect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

  myMenu = mw;

  if ( myMenu )
    connect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
}

QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const int actId, const int pId, const bool rec ) const
{
  return find( actId, find( pId ), rec );
}

QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const int id, MenuNode* startNode, const bool rec ) const
{
  MenuNode* node = 0;
  MenuNode* start = startNode ? startNode : (MenuNode*)&myRoot;
  for ( NodeListIterator it( start->children ); it.current() && !node; ++it )
  {
    if ( it.current()->id == id )
      node = it.current();
    else if ( rec )
      node = find( id, it.current(), rec );
  }
  return node;
}

bool QtxActionMenuMgr::find( const int id, NodeList& lst, MenuNode* startNode ) const
{
  MenuNode* start = startNode ? startNode : (MenuNode*)&myRoot;
  for ( NodeListIterator it( start->children ); it.current(); ++it )
  {
    if ( it.current()->id == id )
      lst.prepend( it.current() );

    find( id, lst, it.current() );
  }
  return !lst.isEmpty();
}

QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const QString& title, const int pId, const bool rec ) const
{
  return find( title, find( pId ), rec );
}

bool QtxActionMenuMgr::find( const QString& title, NodeList& lst, MenuNode* startNode ) const
{
  MenuNode* start = startNode ? startNode : (MenuNode*)&myRoot;
  for ( NodeListIterator it( start->children ); it.current(); ++it )
  {
    QAction* a = itemAction( it.current()->id );
    if ( !a )
      a = menuAction( it.current()->id );
    if ( a && clearTitle( a->menuText() ) == clearTitle( title ) )
      lst.prepend( it.current() );

    find( title, lst, it.current() );
  }
  return !lst.isEmpty();
}

QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const QString& title, MenuNode* startNode, const bool rec ) const
{
  MenuNode* node = 0;
  MenuNode* start = startNode ? startNode : (MenuNode*)&myRoot;
  for ( NodeListIterator it( start->children ); it.current() && !node; ++it )
  {
    QAction* a = itemAction( it.current()->id );
    if ( !a )
      a = menuAction( it.current()->id );
    if ( a && clearTitle( a->menuText() ) == clearTitle( title ) )
      node = it.current();
    if ( !node && rec )
      node = find( title, it.current(), rec );
  }
  return node;
}

int QtxActionMenuMgr::findId( const int id, const int pid ) const
{
  MenuNode* start = pid != -1 ? find( pid ) : (MenuNode*)&myRoot;
  if ( start ) {
    for ( NodeListIterator it( start->children ); it.current(); ++it ) {
      if ( it.current()->id == id ) return id;
    }
  }
  return -1;
}

void QtxActionMenuMgr::removeMenu( const int id, MenuNode* startNode )
{
  MenuNode* start = startNode ? startNode : &myRoot;
  for ( NodeListIterator it( start->children ); it.current(); ++it )
  {
    if ( it.current()->id == id )
      start->children.remove( it.current() );
    else
      removeMenu( id, it.current() );
  }
}

QAction* QtxActionMenuMgr::itemAction( const int id ) const
{
  return action( id );
}

QtxActionMenuMgr::MenuAction* QtxActionMenuMgr::menuAction( const int id ) const
{
  MenuAction* a = 0;

  if ( myMenus.contains( id ) )
    a = myMenus[id];

  return a;
}

void QtxActionMenuMgr::updateMenu( MenuNode* startNode, const bool rec, const bool updParent )
{
  if ( !isUpdatesEnabled() )
    return;

  MenuNode* node = startNode ? startNode : &myRoot;

  QWidget* mw = menuWidget( node );
  if ( !mw )
    return;

  bool filled = checkWidget( mw );

  for ( NodeListIterator it1( node->children ); it1.current(); ++it1 )
  {
    QAction* a = itemAction( it1.current()->id );
    if ( !a )
      a = menuAction( it1.current()->id );

    if ( a )
      a->removeFrom( mw );
  }

  if ( mw->inherits( "QMenuBar" ) )
    ((QMenuBar*)mw)->clear();
  else if ( mw->inherits( "QPopupMenu" ) )
    ((QPopupMenu*)mw)->clear();

  QMap<int, NodeList> idMap;
  for ( NodeListIterator it2( node->children ); it2.current(); ++it2 )
  {
    NodeList& lst = idMap[it2.current()->group];
    int idx = it2.current()->idx;
    if ( idx < 0 || idx >= lst.count() )
      lst.append( it2.current() );
    else
      lst.insert( idx, it2.current() );
  }

  QIntList groups = idMap.keys();
  qHeapSort( groups );

  groups.remove( -1 );
  groups.append( -1 );

  for ( QIntList::const_iterator gIt = groups.begin(); gIt != groups.end(); ++gIt )
  {
    if ( !idMap.contains( *gIt ) )
      continue;

    const NodeList& lst = idMap[*gIt];
    for ( NodeListIterator iter( lst ); iter.current(); ++iter )
    {
      MenuNode* par = iter.current()->parent;
      if ( !isVisible( iter.current()->id, par ? par->id : -1 ) )
	continue;

      if ( rec )
        updateMenu( iter.current(), rec, false );

      QAction* a = itemAction( iter.current()->id );
      if ( !a )
        a = menuAction( iter.current()->id );
      if ( a ) {
	QMenuData* md = dynamic_cast<QMenuData*>( mw );
	int cnt = 0;
	if ( md ) cnt = md->count();
	a->addTo( mw );
	if ( md && md->count() - cnt == 1 ) { //&& iter.current()->id > 0
	  int lid = md->idAt( cnt ); 
	  QMenuItem* mi = md->findItem( lid );
	  if ( mi && !mi->isSeparator() )
	    md->setId( cnt, iter.current()->id );
	}
      }
    }
  }

  simplifySeparators( mw );

  if ( updParent && node->parent && filled != checkWidget( mw ) )
    updateMenu( node->parent, false );
}

void QtxActionMenuMgr::internalUpdate()
{
  if ( isUpdatesEnabled() )
    updateMenu();
}

bool QtxActionMenuMgr::checkWidget( QWidget* wid ) const
{
  if ( !wid )
    return false;

  QMenuData* md = 0;
  if ( wid->inherits( "QPopupMenu" ) )
    md = (QPopupMenu*)wid;
  else if ( wid->inherits( "QMenuBar" ) )
    md = (QMenuBar*)wid;

  return md->count();
}

QWidget* QtxActionMenuMgr::menuWidget( MenuNode* node) const
{
  if ( !node || node == &myRoot )
     return myMenu;

  if ( !myMenus.contains( node->id ) || !myMenus[node->id] )
    return 0;

  return myMenus[node->id]->popup();
}

void QtxActionMenuMgr::simplifySeparators( QWidget* wid )
{
  if ( wid && wid->inherits( "QPopupMenu" ) )
    Qtx::simplifySeparators( (QPopupMenu*)wid, false );
}

QString QtxActionMenuMgr::clearTitle( const QString& txt ) const
{
  QString res = txt;

  for ( int i = 0; i < (int)res.length(); i++ )
  {
    if ( res.at( i ) == '&' )
      res.remove( i--, 1 );
  }

  return res;
}

int QtxActionMenuMgr::createMenu( const QStringList& lst, const int pId )
{
  if ( lst.isEmpty() )
    return -1;

  QStringList sl( lst );

  QString title = sl.last().stripWhiteSpace();
  sl.remove( sl.fromLast() );

  int parentId = sl.isEmpty() ? pId : createMenu( sl, pId );

  return insert( title, parentId, -1 );
}

bool QtxActionMenuMgr::load( const QString& fname, QtxActionMgr::Reader& r )
{
  MenuCreator cr( &r, this );
  return r.read( fname, cr );
}

bool QtxActionMenuMgr::containsMenu( const QString& title, const int pid ) const
{
  return (bool)find( title, pid, false );
}

bool QtxActionMenuMgr::containsMenu( const int id, const int pid ) const
{
  return (bool)find( id, pid, false );
}

/*!
	Class: QtxActionMenuMgr::MenuCreator
	Level: Public
*/

QtxActionMenuMgr::MenuCreator::MenuCreator( QtxActionMgr::Reader* r,
                                            QtxActionMenuMgr* mgr )
: QtxActionMgr::Creator( r ),
  myMgr( mgr )
{
}

QtxActionMenuMgr::MenuCreator::~MenuCreator()
{
}

int QtxActionMenuMgr::MenuCreator::append( const QString& tag, const bool subMenu,
                                           const ItemAttributes& attr, const int pId )
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

  if( subMenu )
    res = myMgr->insert( strValue( attr, label ), pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  else if( tag==sep )
    res = myMgr->insert( separator(), pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
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
    newAct->setToolTip( strValue( attr, tooltip ) );
    QString toggleact = strValue( attr, toggle );
    newAct->setToggleAction( !toggleact.isEmpty() );
    newAct->setOn( toggleact.lower()=="true" );
        
    connect( newAct );
    int aid = myMgr->registerAction( newAct, actId );
    res = myMgr->insert( aid, pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  }

  return res;
}
