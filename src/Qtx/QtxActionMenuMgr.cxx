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

// File:      QtxActionMenuMgr.cxx
// Author:    Alexander SOLOVYOV, Sergey TELKOV
//
#include "QtxActionMenuMgr.h"

#include "QtxAction.h"

#include <QMenu>
#include <QMenuBar>
#include <QWidget>
#include <QMainWindow>

/*!
  \class QtxActionMenuMgr::MenuNode
  \brief Represents a menu item inside main menu structure.
  \internal
*/

class QtxActionMenuMgr::MenuNode
{
public:
  MenuNode();
  MenuNode( MenuNode*, const int, const int, const int );
  ~MenuNode();
  
  MenuNode* parent;       //!< parent menu node
  int       id;           //!< menu nodeID
  int       idx;          //!< menu node index 
  int       group;        //!< menu group ID
  bool      visible;      //!< visibility status
  int       emptyEnabled; //!< enable empty menu flag
  NodeList  children;     //!< children menu nodes list
};

/*!
  \brief Default constructor.
  \internal
*/
QtxActionMenuMgr::MenuNode::MenuNode()
  : parent( 0 ), id( -1 ), idx( -1 ), group( -1 ), visible( true ), emptyEnabled( 0 )
{
}

/*!
  \brief Constructor.
  \internal
  \param p parent menu node
  \param _id menu node ID
  \param _idx menu node index
  \param _group menu node group ID
*/
QtxActionMenuMgr::MenuNode::MenuNode( MenuNode* p,
                                      const int _id,
                                      const int _idx,
                                      const int _group )
: parent( p ), id( _id ), idx( _idx ), group( _group ), visible( true ), emptyEnabled( 0 )
{
  if ( p )
    p->children.append( this );
}

/*!
  \brief Destructor.
  \internal
*/
QtxActionMenuMgr::MenuNode::~MenuNode()
{
  for ( NodeList::iterator it = children.begin(); it != children.end(); ++it )
    delete *it;
}

/*!
  \class QtxActionMenuMgr
  \brief Main menu actions manager.

  Menu manager allows using of set of action for automatic generating of
  application main menu and dynamic update of its contents.

  Use insert(), append() and remove() methods to create main menu.
  Methods show(), hide() allow displaying/erasing of specified menu items.

  Actions can be grouped with help of group identificator. Inside the popup
  or main menu bar menu items are ordered by the group identifier (ascending).

  Menu manager automatically optimizes the menu by removing extra separators,
  hiding empty popup menus etc.
*/

/*!
  \brief Constructor.
  \param p parent main window
*/
QtxActionMenuMgr::QtxActionMenuMgr( QMainWindow* p )
: QtxActionMgr( p ), 
  myRoot( new MenuNode() ),
  myMenu( p ? p->menuBar() : 0 )
{
  if ( myMenu ) {
    connect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
  }
}

/*!
  \brief Constructor.
  \param mw menu widget
  \param p parent object
*/
QtxActionMenuMgr::QtxActionMenuMgr( QWidget* mw, QObject* p )
: QtxActionMgr( p ), 
  myRoot( new MenuNode() ),
  myMenu( mw )
{
  if ( myMenu ) {
    connect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
  }
}

/*!
  \brief Destructor.
*/
QtxActionMenuMgr::~QtxActionMenuMgr()
{
  for ( MenuMap::Iterator itr = myMenus.begin(); itr != myMenus.end(); ++itr )
  {
    QPointer<QAction> a = itr.value();
    delete a->menu();
    delete a;
  }

  delete myRoot;
}

/*!
  \brief Check if an action with \a actId identifier is visible to
  the parent action with \a place identifier.
  \param actId action ID
  \param place some parent action ID
  \return \c true if an action is visible to the parent
  \sa setVisible()
*/
bool QtxActionMenuMgr::isVisible( const int actId, const int place ) const
{
  MenuNode* node = find( actId, place );
  return node && node->visible;
}

/*!
  \brief Set action's visibility flag.
  \param actId action ID
  \param place some parent action ID
  \param v new visibility state
  \sa isVisible()
*/
void QtxActionMenuMgr::setVisible( const int actId, const int place, const bool v )
{
  MenuNode* node = find( actId, place );
  if ( node )
    node->visible = v;
}

/*!
  \brief Insert action to the menu.

  Insert an action to the named menu. The \a menus parameter represents 
  the menu name: it can be a sequence of strings, separated by '|' symbol.
  For example, "File|Edit" means \c File->Edit submenu.
  If submenu doesn't exist, it will be created.

  \param id action ID
  \param menus menu name
  \param group group ID
  \param idx menu index inside the menu group
  \return action ID
*/
int QtxActionMenuMgr::insert( const int id, const QString& menus, const int group, const int idx )
{
  return insert( id, menus.split( "|", QString::SkipEmptyParts ), group, idx );
}

/*!
  \brief Insert action to the menu.

  Insert an action to the named menu. The \a menus parameter represents 
  the menu name: it can be a sequence of strings, separated by '|' symbol.
  For example, "File|Edit" means \c File->Edit submenu.
  If submenu doesn't exist, it will be created.

  \param a action
  \param menus menu name
  \param group group ID
  \param idx menu index inside the menu group
  \return action ID
*/
int QtxActionMenuMgr::insert( QAction* a, const QString& menus, const int group, const int idx )
{
  return insert( a, menus.split( "|", QString::SkipEmptyParts ), group, idx );
}

/*!
  \brief Insert action to the menu.

  Insert an action to the named menu. The \a menus parameter represents 
  the menu names list.
  For example, string list consisting from two items "File" and "Edit"
  means \c File->Edit submenu.
  If submenu doesn't exist, it will be created.

  \param id action ID
  \param menus menu names list
  \param group group ID
  \param idx menu index inside the menu group
  \return action ID
*/
int QtxActionMenuMgr::insert( const int id, const QStringList& menus, const int group, const int idx )
{
  int pId = createMenu( menus, -1 );
  if ( pId == -1 )
    return -1;

  return insert( id, pId, group, idx );
}

/*!
  \brief Insert action to the menu.

  Insert an action to the named menu. The \a menus parameter represents 
  the menu names list.
  For example, string list consisting from two items "File" and "Edit"
  means \c File->Edit submenu.
  If submenu doesn't exist, it will be created.

  \param a action
  \param menus menu names list
  \param group group ID
  \param idx menu index inside the menu group
  \return action ID
*/
int QtxActionMenuMgr::insert( QAction* a, const QStringList& menus, const int group, const int idx )
{
  int pId = createMenu( menus, -1 );
  if ( pId == -1 )
    return -1;

  return insert( a, pId, group, idx );
}

/*!
  \brief Insert action to the menu.
  \param id action ID
  \param pId parent menu action ID
  \param group group ID
  \param idx menu index inside the menu group
  \return action ID
*/
int QtxActionMenuMgr::insert( const int id, const int pId, const int group, const int idx )
{
  if ( id == -1 )
    return -1;

  MenuNode* pNode = pId == -1 ? myRoot : find( pId );
  if ( !pNode )
    return -1;

  MenuNode* node = new MenuNode( pNode, id, idx, group );

  triggerUpdate( pNode->id, false );

  return node->id;
}

/*!
  \brief Insert action to the menu.
  \param a action
  \param pId parent menu action ID
  \param group group ID
  \param idx menu index inside the menu group
  \return action ID
*/
int QtxActionMenuMgr::insert( QAction* a, const int pId, const int group, const int idx )
{
  return insert( registerAction( a ), pId, group, idx );
}

/*!
  \brief Create and insert menu item action to the menu.
  \param title menu text
  \param pId parent menu action ID
  \param group group ID
  \param id action ID
  \param idx menu index inside the menu group
  \return action ID
*/
int QtxActionMenuMgr::insert( const QString& title, const int pId, const int group,
                              const int id, const int idx,  QMenu* _menu)
{
  MenuNode* pNode = pId == -1 ? myRoot : find( pId );
  if ( !pNode )
    return -1;

  MenuNode* eNode = id == -1 ? 0 : find( id );

  int fid = -1;
  for ( NodeList::iterator it = pNode->children.begin(); it != pNode->children.end() && fid == -1; ++it )
  {
    if ( myMenus.contains( (*it)->id ) &&
         clearTitle( myMenus[(*it)->id]->text() ) == clearTitle( title ) )
      fid = (*it)->id;
  }

  if ( fid != -1 )
    return fid;

  int gid = (id == -1 || eNode ) ? generateId() : id;

  QMenu* menu;
  if (_menu)
    menu = _menu;
  else
    menu = new QMenu( 0 );
  QAction* ma = menu->menuAction();
  ma->setText( title );

  connect( ma->menu(), SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
  connect( ma->menu(), SIGNAL( aboutToHide() ), this, SLOT( onAboutToHide() ) );

  MenuNode* node = new MenuNode( pNode, myMenus.insert( gid, ma ).key(), idx, group );

  triggerUpdate( pNode->id, false );

  return node->id;
}

/*!
  \brief Create and insert menu item action to the menu.

  Insert an action to the named menu. The \a menus parameter represents 
  the menu name: it can be a sequence of strings, separated by '|' symbol.
  For example, "File|Edit" means \c File->Edit submenu.
  If submenu doesn't exist, it will be created.

  \param title menu text
  \param menus menu name
  \param group group ID
  \param id action ID
  \param idx menu index inside the menu group
  \return action ID
*/
int QtxActionMenuMgr::insert( const QString& title, const QString& menus, const int group, const int id, const int idx )
{
  return insert( title, menus.split( "|", QString::SkipEmptyParts ), group, id, idx );
}

/*!
  \brief Create and insert menu item action to the menu.

  Insert an action to the named menu. The \a menus parameter represents 
  the menu names list.
  For example, string list consisting from two items "File" and "Edit"
  means \c File->Edit submenu.
  If submenu doesn't exist, it will be created.

  \param title menu text
  \param menus menu names list
  \param group group ID
  \param id action ID
  \param idx menu index inside the menu group
  \return action ID
*/
int QtxActionMenuMgr::insert( const QString& title, const QStringList& menus, const int group, const int id, const int idx )
{
  int pId = createMenu( menus, -1 );
  return insert( title, pId, group, id, idx );
}

/*!
  \brief Create and add menu item action to the end of menu.
  \param title menu text
  \param pId parent menu action ID
  \param group group ID
  \param id action ID
  \return action ID
*/
int QtxActionMenuMgr::append( const QString& title, const int pId, const int group, const int id )
{
  return insert( title, pId, group, id );
}

/*!
  \brief Create and add menu item action to the end of menu.
  \param id action ID
  \param pId parent menu action ID
  \param group group ID
  \return action ID
*/
int QtxActionMenuMgr::append( const int id, const int pId, const int group )
{
  return insert( id, pId, group );
}

/*!
  \brief Create and add menu item action to the end of menu.
  \param a action
  \param pId parent menu action ID
  \param group group ID
  \return action ID
*/
int QtxActionMenuMgr::append( QAction* a, const int pId, const int group )
{
  return insert( a, pId, group );
}

/*!
  \brief Create and add menu item action to the beginning of menu.
  \param title menu text
  \param pId parent menu action ID
  \param group group ID
  \param id action ID
  \return action ID
*/
int QtxActionMenuMgr::prepend( const QString& title, const int pId, const int group, const int id )
{
  return insert( title, pId, group, id, 0 );
}

/*!
  \brief Create and add menu item action to the beginning of menu.
  \param id action ID
  \param pId parent menu action ID
  \param group group ID
  \return action ID
*/
int QtxActionMenuMgr::prepend( const int id, const int pId, const int group )
{
  return insert( id, pId, group, 0 );
}

/*!
  \brief Create and add menu item action to the beginning of menu.
  \param a action
  \param pId parent menu action ID
  \param group group ID
  \return action ID
*/
int QtxActionMenuMgr::prepend( QAction* a, const int pId, const int group )
{
  return insert( a, pId, group, 0 );
}

/*!
  \brief Remove menu item with given \a id.
  \param id menu action ID
*/
void QtxActionMenuMgr::remove( const int id )
{
  removeMenu( id, 0 );
  update();
}

/*!
  \brief Remove menu item with given \a id.
  \param id menu action ID
  \param pId parent menu action ID
  \param group group ID
*/
void QtxActionMenuMgr::remove( const int id, const int pId, const int group )
{
  MenuNode* pNode = pId == -1 ? myRoot : find( pId );
  if ( !pNode )
    return;

  NodeList delNodes;
  for ( NodeList::iterator it = pNode->children.begin(); it != pNode->children.end(); ++it )
  {
    if ( (*it)->id == id && ( (*it)->group == group || group == -1 ) )
      delNodes.append( *it );
  }

  QWidget* mW = menuWidget( pNode );
  for ( NodeList::iterator itr = delNodes.begin(); itr != delNodes.end(); ++itr )
  {
    int id = (*itr)->id;
    if( mW && menuAction( id ) )
    {
      mW->removeAction( menuAction( id ) );
      myMenus.remove( id );
    }
    else if( mW && itemAction( id ) )
      mW->removeAction( itemAction( id ) );
    pNode->children.removeAll( *itr );
  }

  triggerUpdate( pNode->id, false );
}

/*!
  \brief Show menu item with given \a id.
  \param id menu action ID
  \sa hide()
*/
void QtxActionMenuMgr::show( const int id )
{
  setShown( id, true );
}

/*!
  \brief Hide menu item with given \a id.
  \param id menu action ID
  \sa show()
*/
void QtxActionMenuMgr::hide( const int id )
{
  setShown( id, false );
}

/*!
  \brief Get visibility status for menu item with given \a id.
  \param id menu action ID
  \return \c true if an item is shown
  \sa setShown()
*/
bool QtxActionMenuMgr::isShown( const int id ) const
{
  bool res = false;
  MenuNode* node = find( id );
  if ( node )
    res = node->visible;
  return res;
}

/*!
  \brief Set visibility status for menu item with given \a id.
  \param id menu action ID
  \param on new visibility status
  \sa isShown()
*/
void QtxActionMenuMgr::setShown( const int id, const bool on )
{
  NodeList aNodes;
  find( id, aNodes );

  for ( NodeList::iterator it = aNodes.begin(); it != aNodes.end(); ++it )
  {
    if ( (*it)->visible != on )
    {
      (*it)->visible = on;
      triggerUpdate( (*it)->parent ? (*it)->parent->id : myRoot->id, false );
    }
  }
}

/*!
  \brief Change menu title for the action with given \a id.
  \param id menu action ID
  \param title new menu title
*/
void QtxActionMenuMgr::change( const int id, const QString& title )
{
  QAction* a = menuAction( id );
  if ( a )
    a->setText( title );
}

/*!
  \brief Called when the submenu is about to show.
  
  Emits the signal menuAboutToShow(QMenu*).
*/
void QtxActionMenuMgr::onAboutToShow()
{
  QMenu* m = ::qobject_cast<QMenu*>( sender() );
  if ( m )
    emit menuAboutToShow( m );
}

/*!
  \brief Called when the submenu is about to hide.
  
  Emits the signal menuAboutToHide(QMenu*).
*/
void QtxActionMenuMgr::onAboutToHide()
{
  QMenu* m = ::qobject_cast<QMenu*>( sender() );
  if ( m )
    emit menuAboutToHide( m );
}

/*!
  \brief Called when the corresponding menu object is destroyed.

  Clears internal pointer to menu to disable crashes.
  
  \param obj (menu) object being destroyed
*/
void QtxActionMenuMgr::onDestroyed( QObject* obj )
{
  if ( myMenu == obj )
    myMenu = 0;
}


/*!
  \fn void QtxActionMenuMgr::menuAboutToShow( QMenu* m )
  \brief Emitted when the menu is about to be shown.
  \param m menu being shown
*/

/*!
  \fn void QtxActionMenuMgr::menuAboutToHide( QMenu* m )
  \brief Emitted when the menu is about to be hidden.
  \param m menu being hidden
*/

/*!
  \brief Get the menu widget.
  \return menu widget (QMenuBar)
*/
QWidget* QtxActionMenuMgr::menuWidget() const
{
  return myMenu;
}

/*!
  \brief Assign new menu widget to the menu manager.
  \param mw new menu widget
*/
void QtxActionMenuMgr::setMenuWidget( QWidget* mw )
{
  if ( myMenu == mw )
    return;

  if ( myMenu )
    disconnect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

  myMenu = mw;

  if ( myMenu )
    connect( myMenu, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

  triggerUpdate( -1, true );
}

/*!
  \brief Search menu node.
  \param id menu action ID
  \param pId parent menu item ID
  \param rec if \c true perform recursive search
  \return menu node or 0 if it is not found
*/
QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const int id, const int pId, const bool rec ) const
{
  return find( id, find( pId ), rec );
}

/*!
  \brief Search menu node.
  \param id menu action ID
  \param startNode start menu node (if 0, search from root node)
  \param rec if \c true perform recursive search
  \return menu node or 0 if it is not found
*/
QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const int id, MenuNode* startNode, const bool rec ) const
{
  MenuNode* node = 0;
  MenuNode* start = startNode ? startNode : myRoot;
  for ( NodeList::iterator it = start->children.begin(); it != start->children.end() && !node; ++it )
  {
    if ( (*it)->id == id )
      node = *it;
    else if ( rec )
      node = find( id, *it, rec );
  }
  return node;
}

/*!
  \brief Search recursively all menu nodes with given \a id.
  \param id menu action ID
  \param NodeList resulting list of menu nodes
  \param startNode start menu node (if 0, search from root node)
  \return \c true if at least one node is found
*/
bool QtxActionMenuMgr::find( const int id, NodeList& lst, MenuNode* startNode ) const
{
  MenuNode* start = startNode ? startNode : myRoot;
  for ( NodeList::iterator it = start->children.begin(); it != start->children.end(); ++it )
  {
    MenuNode* node = *it;
    if ( node->id == id )
      lst.prepend( node );

    find( id, lst, node );
  }
  return !lst.isEmpty();
}

/*!
  \brief Search menu node.
  \param title menu item title
  \param pId parent menu item ID
  \param rec if \c true perform recursive search
  \return menu node or 0 if it is not found
*/
QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const QString& title, const int pId, const bool rec ) const
{
  return find( title, find( pId ), rec );
}

/*!
  \brief Search recursively all menu nodes with given \a title.
  \param title menu item title
  \param NodeList resulting list of menu nodes
  \param startNode start menu node (if 0, search from root node)
  \return \c true if at least one node is found
*/
bool QtxActionMenuMgr::find( const QString& title, NodeList& lst, MenuNode* startNode ) const
{
  MenuNode* start = startNode ? startNode : myRoot;
  for ( NodeList::iterator it = start->children.begin(); it != start->children.end(); ++it )
  {
    QAction* a = itemAction( (*it)->id );
    if ( !a )
      a = menuAction( (*it)->id );
    if ( a && clearTitle( a->text() ) == clearTitle( title ) )
      lst.prepend( *it );

    find( title, lst, *it );
  }
  return !lst.isEmpty();
}

/*!
  \brief Search menu node.
  \param title menu item title
  \param startNode start menu node (if 0, search from root node)
  \param rec if \c true perform recursive search
  \return menu node or 0 if it is not found
*/
QtxActionMenuMgr::MenuNode* QtxActionMenuMgr::find( const QString& title, MenuNode* startNode, const bool rec ) const
{
  MenuNode* node = 0;
  MenuNode* start = startNode ? startNode : myRoot;
  for ( NodeList::iterator it = start->children.begin(); it != start->children.end() && !node; ++it )
  {
    QAction* a = itemAction( (*it)->id );
    if ( !a )
      a = menuAction( (*it)->id );
    if ( a && clearTitle( a->text() ) == clearTitle( title ) )
      node = *it;
    if ( !node && rec )
      node = find( title, *it, rec );
  }
  return node;
}

/*!
  \brief Find menu item by given ID (one-level only).
  \param id menu action ID
  \param pid parent meun item ID
  \return id (>0) on success or -1 if menu item is not found
*/
int QtxActionMenuMgr::findId( const int id, const int pid ) const
{
  MenuNode* start = pid != -1 ? find( pid ) : myRoot;
  if ( start )
  {
    for ( NodeList::iterator it = start->children.begin(); it != start->children.end(); ++it )
    {
      if ( (*it)->id == id )
        return id;
    }
  }
  return -1;
}

/*!
  \brief Removes menu node (with all its children).
  \param id menu action ID
  \param startNode parent menu node which search starts from (if 0, search starts from root)
*/
void QtxActionMenuMgr::removeMenu( const int id, MenuNode* startNode )
{
  MenuNode* start = startNode ? startNode : myRoot;
  for ( NodeList::iterator it = start->children.begin(); it != start->children.end(); ++it )
  {
    if ( (*it)->id == id )
      start->children.removeAll( *it );
    else
      removeMenu( id, *it );
  }
}

/*!
  \brief Get action by \a id.
  \param id action ID
  \return action or 0 if \a id is invalid
*/
QAction* QtxActionMenuMgr::itemAction( const int id ) const
{
  return action( id );
}

/*!
  \brief Get submenu action by \a id.
  \param id submenu ID
  \return submenu action or 0 if action is not found
*/
QAction* QtxActionMenuMgr::menuAction( const int id ) const
{
  QAction* a = 0;

  if ( myMenus.contains( id ) )
    a = myMenus[id];

  return a;
}

/*!
  \brief Get submenu action by \a id.
  \param id submenu ID
  \return submenu action or 0 if it is not found
*/
int QtxActionMenuMgr::menuActionId( QAction* a ) const
{
  int id = -1;
  for ( MenuMap::ConstIterator itr = myMenus.begin(); itr != myMenus.end() && id == -1; ++itr )
  {
    if ( itr.value() == a )
      id = itr.key();
  }
  return id;
}

/*!
  \brief Update menu.

  Does nothing if update is disabled.

  \param startNode start menu item to be updated
  \param rec if \c true, perform recursive update
  \param updParent if \c true update also parent item (without recursion)

  \sa isUpdatesEnabled() and setUpdatesEnabled()
*/
void QtxActionMenuMgr::updateMenu( MenuNode* startNode, const bool rec, const bool updParent )
{
  if ( !isUpdatesEnabled() )
    return;

  MenuNode* node = startNode ? startNode : myRoot;

  QWidget* mw = menuWidget( node );
  if ( !mw )
    return;

  bool filled = checkWidget( mw );

  // first remove all own actions and collect foreign ones
  QMap< QAction*, QList<QAction*> > foreign;
  QAction* a;
  QAction* preva = 0;
  QListIterator<QAction*> ait( mw->actions() ); ait.toBack();
  while ( ait.hasPrevious() )
  {
    a = ait.previous();
    if ( ownAction( a, node ) )
    {
      preva = a;
      mw->removeAction( a );     // remove own actions
    }
    else
    {
      foreign[preva].prepend(a); // do not yet remove foreign actions
    }
  }
  // now only foreign actions should stay in the menu, thus remove them also
  QMap< QAction*, QList<QAction*> >::Iterator formapit;
  for( formapit = foreign.begin(); formapit != foreign.end(); ++formapit )
  {
    QMutableListIterator<QAction*> foralit( formapit.value() );
    while ( foralit.hasNext() )
    {
      a = foralit.next();
      if ( !mw->actions().contains( a ) )
        foralit.remove();
    }
  }
  QList<QAction*> alist = mw->actions();
  foreach( a, alist ) mw->removeAction( a );

  // collect all registered menus by group id
  QMap<int, NodeList> idMap;
  for ( NodeList::iterator it2 = node->children.begin(); it2 != node->children.end(); ++it2 )
  {
    NodeList& lst = idMap[(*it2)->group];
    int idx = (*it2)->idx;
    if ( idx < 0 || idx >= (int)lst.count() )
      lst.append( *it2 );
    else
      lst.insert( idx, *it2 );
  }

  QIntList groups = idMap.keys();
  qSort( groups );

  groups.removeAll( -1 );
  groups.append( -1 );

  // rebuild menu: 1. add all registered actions
  for ( QIntList::const_iterator gIt = groups.begin(); gIt != groups.end(); ++gIt )
  {
    if ( !idMap.contains( *gIt ) )
      continue;

    const NodeList& lst = idMap[*gIt];
    for ( NodeList::const_iterator iter = lst.begin(); iter != lst.end(); ++iter )
    {
      MenuNode* node = *iter;
      if ( !node ) continue;

      if ( rec )
        updateMenu( node, rec, false );

      MenuNode* par = node->parent;
      if ( !isVisible( node->id, par ? par->id : -1 ) )
        continue;

      bool isMenu = false;
      QAction* a = itemAction( node->id );
      if ( !a )
      {
        isMenu = true;
        a = menuAction( node->id );
      }
      if ( !a ) continue;

      if ( !isMenu || !a->menu()->isEmpty() || node->emptyEnabled > 0 )
        mw->addAction( a );
    }
  }

  // rebuild menu: 2. insert back all foreign actions
  for( formapit = foreign.begin(); formapit != foreign.end(); ++formapit ) {
    preva = formapit.key();
    foreach( a, formapit.value() )
      mw->insertAction( preva, a );
  }
  
  // remove extra separators
  simplifySeparators( mw );

  // update parent menu if necessary
  if ( updParent && node->parent && filled != checkWidget( mw ) )
    updateMenu( node->parent, false );
}

/*!
  \brief Internal update.
  
  Customizes the menu update processing.
*/
void QtxActionMenuMgr::internalUpdate()
{
  if ( !isUpdatesEnabled() )
    return;

  updateMenu();
  myUpdateIds.clear();
}

/*!
  \brief Check if action belongs to the menu manager
  \internal
  \param a action being checked
  \param node parent menu node
  \return \c true if action belongs to the menu \a node
*/
bool QtxActionMenuMgr::ownAction( QAction* a, MenuNode* node ) const
{
  for ( NodeList::const_iterator iter = node->children.begin(); iter != node->children.end(); ++iter )
  {
    QAction* mya = itemAction( (*iter)->id );
    if ( !mya )
      mya = menuAction( (*iter)->id );
    if ( mya && mya == a )
      return true;
  }
  return false;
}

/*!
  \brief Check if menu widget has any actions.
  \param wid widget to be checked
  \return \c true if widget contains action(s)
*/
bool QtxActionMenuMgr::checkWidget( QWidget* wid ) const
{
  if ( !wid )
    return false;

  return !wid->actions().isEmpty();
}

/*!
  \brief Get menu widget for the given \a node.
  \param node menu node
  \return popup menu or main menu corresponding to the menu node
  (or 0 if it is not found)
*/
QWidget* QtxActionMenuMgr::menuWidget( MenuNode* node ) const
{
  if ( !node || node == myRoot )
     return myMenu;

  if ( !myMenus.contains( node->id ) || !myMenus[node->id] )
    return 0;

  return myMenus[node->id]->menu();
}

/*!
  \brief Remove extra separators from menu widget.
  \param wid menu widget to be processed
*/
void QtxActionMenuMgr::simplifySeparators( QWidget* wid )
{
  Qtx::simplifySeparators( wid, false );
}

/*!
  \brief Remove special symbols (&) from string to get clear menu title.
  \param txt string to be processed
  \return clear title
*/
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

/*!
  \brief Create and inserts menu item recursively.
  \param lst list of menu names
  \param pId parent menu item ID
  \return created menu item ID (last in the chain)
*/
int QtxActionMenuMgr::createMenu( const QStringList& lst, const int pId )
{
  if ( lst.isEmpty() )
    return -1;

  QStringList sl( lst );

  QString title = sl.last().trimmed();
  sl.removeLast();

  int parentId = sl.isEmpty() ? pId : createMenu( sl, pId );

  return insert( title, parentId, -1 );
}

/*!
  \brief Load actions description from the file.
  \param fname file name
  \param r action reader
  \return \c true on success and \c false on error
*/
bool QtxActionMenuMgr::load( const QString& fname, QtxActionMgr::Reader& r )
{
  MenuCreator cr( &r, this );
  return r.read( fname, cr );
}

/*!
  \brief Check if the parent menu contains menu item with given \a title.
  \param title menu title
  \param pid parent menu item ID
  \return \c true if parent menu item contains such child item
*/
bool QtxActionMenuMgr::containsMenu( const QString& title, const int pid, const bool rec ) const
{
  return (bool)find( title, pid, rec );
}

/*!
  \brief Check if the parent menu contains menu item with given \a id.
  \param id menu item ID
  \param pid parent menu item ID
  \return \c true if parent menu item contains such child item
*/
bool QtxActionMenuMgr::containsMenu( const int id, const int pid, const bool rec ) const
{
  return (bool)find( id, pid, rec );
}

/*!
  \brief Get menu by the specified identifier.
  \param id menu item ID
  \return menu pointer or 0 if menu is not found
*/
QMenu* QtxActionMenuMgr::findMenu( const int id ) const
{
  QMenu* m = 0;
  QAction* a = menuAction( id );
  if ( a )
    m = a->menu();
  return m;
}

/*!
  \brief Get menu by the title.
  \param title menu text
  \param pid parent menu item ID (to start search)
  \param rec if \c true, perform recursive update
  \return menu pointer or 0 if menu is not found
*/
QMenu* QtxActionMenuMgr::findMenu( const QString& title, const int pid, const bool rec ) const
{
  QMenu* m = 0;
  MenuNode* node = find( title, pid, rec );
  if ( node )
  {
    QAction* a = menuAction( node->id );
    if ( a )
      m = a->menu();
  }
  return m;
}

/*!
  \brief Check if empty menu is enabled
  \param id menu item ID
  \return \c true if empty menu is enabled
*/
bool QtxActionMenuMgr::isEmptyEnabled( const int id ) const
{
  MenuNode* node = find( id );
  if ( node && menuAction( id ) )
    return node->emptyEnabled > 0;
  
  return false;
}

/*!
  \brief Enable/disable empty menu
  \param id menu item ID
  \param enable if \c true, empty menu will be enabled, otherwise empty menu will be disabled
*/
void QtxActionMenuMgr::setEmptyEnabled( const int id, const bool enable )
{
  MenuNode* node = find( id );
  if ( node && menuAction( id ) ) {
    int old = node->emptyEnabled;
    node->emptyEnabled += enable ? 1 : -1;
    if ( ( old <= 0 && enable ) || ( old > 0 && !enable ) ) // update menu only if enabled state has been changed
      updateMenu( node, true, true );
  }
}

/*!
  \brief Perform delayed menu update.
  \param id menu item ID
  \param rec if \c true, perform recursive update
*/
void QtxActionMenuMgr::triggerUpdate( const int id, const bool rec )
{
  bool isRec = rec;
  if ( myUpdateIds.contains( id ) )
    isRec = isRec || myUpdateIds[ id ];
  myUpdateIds.insert( id, isRec );

  QtxActionMgr::triggerUpdate();
}

/*!
  \brief Called when delayed content update is performed.

  Customizes the content update operation.
*/
void QtxActionMenuMgr::updateContent()
{
  // Warning: For correct updating it is necessary to update the most enclosed submenu in first turn
  //          because not updated empty submenu will be skipped. Now the submenus are iterated in
  //          ascending order according to their identifiers. For a submenus with automatically generated 
  //          identifiers this will work correctly since the uppermost submenus have the biggest number
  //          (identifiers are generated by decrementing 1 starting from -1). In general, if any submenu
  //          have positive identifiers this method might not work correctly. In this case it would be
  //          necessary to improve this method and to add preliminary sorting a submenus by depth of an 
  //          enclosure.
  for ( QMap<int, bool>::const_iterator it = myUpdateIds.constBegin(); it != myUpdateIds.constEnd(); ++it )
  {
    MenuNode* node = it.key() == -1 ? myRoot : find( it.key() );
    if ( node )
      updateMenu( node, it.value() );
  }
  myUpdateIds.clear();
}

/*!
  \class QtxActionMenuMgr::MenuCreator
  \brief Menu actions creator.

  Used by Reader to create actions by reading descriptions from the file
  and fill in the action manager with the actions.
*/

/*!
  \brief Constructor.
  \param r menu actions reader
  \param mgr menu manager
*/
QtxActionMenuMgr::MenuCreator::MenuCreator( QtxActionMgr::Reader* r, QtxActionMenuMgr* mgr )
: QtxActionMgr::Creator( r ),
  myMgr( mgr )
{
}

/*!
  \brief Destructor.
*/
QtxActionMenuMgr::MenuCreator::~MenuCreator()
{
}

/*!
  \brief Create and append to the action manager a new action.
  \param tag item tag name
  \param subMenu \c true if this item is submenu
  \param attr attributes map
  \param pId parent action ID
  \return menu action ID
*/
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
    QIcon set;
    QPixmap pix;
    QString name = strValue( attr, icon );
    if( !name.isEmpty() && loadPixmap( name, pix ) )
      set = QIcon( pix );

    QtxAction* newAct = new QtxAction( strValue( attr, tooltip ), set,
                                       strValue( attr, label ),
                                       QKeySequence( strValue( attr, accel ) ),
                                       myMgr );
    newAct->setToolTip( strValue( attr, tooltip ) );
    QString toggleact = strValue( attr, toggle );
    newAct->setCheckable( !toggleact.isEmpty() );
    newAct->setChecked( toggleact.toLower() == "true" );

    connect( newAct );
    int aid = myMgr->registerAction( newAct, actId );
    res = myMgr->insert( aid, pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  }

  return res;
}
