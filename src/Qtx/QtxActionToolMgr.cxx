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

// File:      QtxActionToolMgr.cxx
// Author:    Alexander SOLOVYOV, Sergey TELKOV
//
#include "QtxActionToolMgr.h"

#include "QtxAction.h"
#include "QtxToolBar.h"

#include <QApplication>
#include <QHideEvent>
#include <QMainWindow>

/*!
  \class QtxActionToolMgr::ToolNode
  \brief Represents a toolbutton inside toolbar structure.
  \internal
*/

/*!
  \fn QtxActionToolMgr::ToolNode::ToolNode()
  \internal
  \brief Default constructor.
*/

/*!
  \fn QtxActionToolMgr::ToolNode::ToolNode( const int _id )
  \brief Constructor.
  \internal
  \param _id toolbar node ID
*/

/*!
  \class QtxActionToolMgr
  \brief Toolbar actions manager.
  
  Toolbar manager allows using of set of action for automatic generating of
  application toolbars and dynamic update of toolbars contents.

  Use insert(), append() and remove() methods to create toolbar and add actions to it.
  Methods show(), hide() allow displaying/erasing of specified toolbar items.

  Toolbar manager automatically optimizes toolbars by removing extra separators, etc.
*/

/*!
  \brief Constructor.
  \param p parent main window
*/
QtxActionToolMgr::QtxActionToolMgr( QMainWindow* p )
: QtxActionMgr( p ),
  myMainWindow( p )
{
}

/*!
  \brief Destructor.
*/
QtxActionToolMgr::~QtxActionToolMgr()
{
}

/*!
  \brief Get parent main window.
  \return main window pointer
*/
QMainWindow* QtxActionToolMgr::mainWindow() const
{
  return myMainWindow;
}

/*!
  \brief Create toolbar and assign \a id to it.

  If \a tid is less than 0, the identifier is generated automatically.
  If toolbar with given \a tid is already registered, the toolbar will not be created.

  \param title toolbar title
  \param tid requested toolbar ID
  \param mw parent main window; if it is null, the tool manager's main window is used
  \param vis show toolbar visible immediately after creation (true by default)
  \return id of created/found toolbar
*/
int QtxActionToolMgr::createToolBar( const QString& title, const int tid, QMainWindow* mw, bool vis )
{
  return createToolBar( title, true, Qt::AllToolBarAreas, tid, mw, vis );
}

/*!
  \brief Create toolbar and assign \a id to it.

  If \a tid is less than 0, the identifier is generated automatically.
  If toolbar with given \a tid is already registered, the toolbar will not be created.

  The parameter \a name can be specified to give an unique string identifier to the toolbar.
  This can be useful in the multi-language environment where identifier of the toolbar should
  not be dependant on the language chosen (e.g. to store positions of toolbars of main menu
  in the preferences).

  \param title toolbar title
  \param name toolbar name (identifier)
  \param tid requested toolbar ID
  \param mw parent main window; if it is null, the tool manager's main window is used
  \param vis show toolbar visible immediately after creation (true by default)
  \return id of created/found toolbar
*/
int QtxActionToolMgr::createToolBar( const QString& title, const QString& name, const int tid, QMainWindow* mw, bool vis )
{
  return createToolBar( title, name, true, Qt::AllToolBarAreas, tid, mw, vis );
}

/*!
  \brief Create toolbar and assign \a id to it.

  If \a tid is less than 0, the identifier is generated automatically.
  If toolbar with given \a tid is already registered, the toolbar will not be created.

  The parameter \a name can be specified to give an unique string identifier to the toolbar.
  This can be useful in the multi-language environment where identifier of the toolbar should
  not be dependant on the language chosen (e.g. to store positions of toolbars of main menu
  in the preferences).

  \param title toolbar title
  \param name toolbar name (identifier)
  \param floatable if \c true, new toolbar is made floatable
  \param dockAreas dock areas of the main window where the new toolbar can be situated
  \param tid requested toolbar ID
  \param mw parent main window; if it is null, the tool manager's main window is used
  \param vis show toolbar visible immediately after creation (true by default)
  \return id of created/found toolbar
*/
int QtxActionToolMgr::createToolBar( const QString& title, const QString& name, bool floatable, Qt::ToolBarAreas dockAreas, 
                                     int tid, QMainWindow* mw, bool vis )
{
  static int _toolBarId = -1;

  int tbId = -1;
  for ( ToolBarMap::ConstIterator it = myToolBars.begin(); it != myToolBars.end() && tbId == -1; ++it )
  {
    if( it.value().toolBar->windowTitle().toLower() == title.toLower() &&
        ( !mw || it.value().toolBar->parent()==mw ) )
      tbId = it.key();
  }

  if ( tbId != -1 )
    return tbId;

  QMainWindow* tbw = mw ? mw : mainWindow();
  QToolBar* tb = find( title, tbw );

  tbId = tid < 0 ? --_toolBarId : tid;

  myToolBars.insert( tbId, ToolBarInfo() );
  ToolBarInfo& tInfo = myToolBars[tbId];

  if ( !tb )
  {
    tb = new QtxToolBar( true, tbw );
    //tb->setVisible( false );  // VSR: create toolbar visible initially
    tb->setFloatable( floatable );
    tb->setAllowedAreas( dockAreas );
    tb->setMovable( dockAreas & Qt::AllToolBarAreas );
    //mainWindow()->addToolBar( tb );
    tb->setWindowTitle( title );
    tb->setObjectName( name.isEmpty() ? title : name );
    tb->setToolTip( title );
    if ( !vis )
      QApplication::postEvent( tb, new QHideEvent());
   }

  tInfo.toolBar = tb;
  connect( tInfo.toolBar, SIGNAL( destroyed() ), this, SLOT( onToolBarDestroyed() ) );

  return tbId;
}

/*!
  \brief Create toolbar and assign \a id to it.

  If \a tid is less than 0, the identifier is generated automatically.
  If toolbar with given \a tid is already registered, the toolbar will not be created.

  \param title toolbar title
  \param floatable if \c true, new toolbar is made floatable
  \param dockAreas dock areas of the main window where the new toolbar can be situated
  \param tid requested toolbar ID
  \param mw parent main window; if it is null, the tool manager's main window is used
  \param vis show toolbar visible immediately after creation (true by default)
  \return id of created/found toolbar
*/
int QtxActionToolMgr::createToolBar( const QString& title, bool floatable, Qt::ToolBarAreas dockAreas, 
                                     int tid, QMainWindow* mw, bool vis )
{
  return createToolBar( title, QString(), floatable, dockAreas, tid, mw, vis );
}

/*!
  \brief Search toolbar with given \a title owned by main window \mw. 
  \param title toolbar title
  \param mw main window
  \return toolbar or 0 if it is not found
*/
QToolBar* QtxActionToolMgr::find( const QString& title, QMainWindow* mw ) const
{
  if ( !mw )
    return 0;

  QString pattern = title.toLower();

  QToolBar* res = 0;
  QList<QToolBar*> toolbars = mw->findChildren<QToolBar*>();
  for ( QList<QToolBar*>::iterator it = toolbars.begin(); it != toolbars.end() && !res; ++it )
  {
    if ( (*it)->windowTitle().toLower() == pattern )
      res = *it;
  }
  return res;
}

/*!
  \brief Remove toolbar.
  \param tid toolbar ID
*/
void QtxActionToolMgr::removeToolBar( const int tid )
{
  if ( !myToolBars.contains( tid ) )
    return;

  delete myToolBars[tid].toolBar;
  myToolBars.remove( tid );
}

/*!
  \brief Remove toolbar.
  \param title toolbar title
*/
void QtxActionToolMgr::removeToolBar( const QString& title )
{
  removeToolBar( find( title ) );
}

/*!
  \brief Insert action into toolbar.
  \param id action ID
  \param tid toolbar ID
  \param idx action index in the toolbar (if < 0, action is appended to the end)
  \return action ID
*/
int QtxActionToolMgr::insert( const int id, const int tid, const int idx )
{
  if ( !contains( id ) || !hasToolBar( tid ) )
    return -1;
/*
  if ( containsAction( id, tid ) )
    remove( id, tid );
*/
  ToolNode node( id );

  NodeList& list = myToolBars[tid].nodes;
  int index = idx < 0 ? list.count() : qMin( idx, (int)list.count() );
  list.insert( index, node );
  triggerUpdate( tid );

  return id;
}

/*!
  \brief Insert action into toolbar.
  \param a action
  \param tid toolbar ID
  \param idx action index in the toolbar (if < 0, action is appended to the end)
  \return action ID
*/
int QtxActionToolMgr::insert( QAction* a, const int tid, const int idx )
{
  return insert( registerAction( a ), tid, idx );
}

/*!
  \brief Insert action into toolbar.
  \param id action ID
  \param title toolbar title
  \param idx action index in the toolbar (if < 0, action is appended to the end)
  \return action ID
*/
int QtxActionToolMgr::insert( const int id, const QString& title, const int idx )
{
  return insert( id, createToolBar( title ), idx );
}

/*!
  \brief Insert action into toolbar.
  \param a action
  \param title toolbar title
  \param idx action index in the toolbar (if < 0, action is appended to the end)
  \return action ID
*/
int QtxActionToolMgr::insert( QAction* a, const QString& title, const int idx )
{
  return insert( registerAction( a ), createToolBar( title ), idx );
}

/*!
  \brief Append action to the end of toolbar.
  \param id action ID
  \param tid toolbar ID
  \return action ID
*/
int QtxActionToolMgr::append( const int id, const int tid )
{
  return insert( id, tid );
}

/*!
  \brief Append action to the end of toolbar.
  \param a action
  \param tid toolbar ID
  \return action ID
*/
int QtxActionToolMgr::append( QAction* a, const int tid )
{
  return insert( a, tid );
}

/*!
  \brief Append action to the end of toolbar.
  \param id action ID
  \param title toolbar title
  \return action ID
*/
int QtxActionToolMgr::append( const int id, const QString& title )
{
  return insert( id, title );
}

/*!
  \brief Append action to the end of toolbar.
  \param a action
  \param title toolbar title
  \return action ID
*/
int QtxActionToolMgr::append( QAction* a, const QString& title )
{
  return insert( a, title );
}

/*!
  \brief Insert action to the beginning of toolbar.
  \param id action ID
  \param tid toolbar ID
  \return action ID
*/
int QtxActionToolMgr::prepend( const int id, const int tid )
{
  return insert( id, tid, 0 );
}

/*!
  \brief Insert action to the beginning of toolbar.
  \param a action
  \param tid toolbar ID
  \return action ID
*/
int QtxActionToolMgr::prepend( QAction* a, const int tid )
{
  return insert( a, tid, 0 );
}

/*!
  \brief Insert action to the beginning of toolbar.
  \param id action ID
  \param title toolbar title
  \return action ID
*/
int QtxActionToolMgr::prepend( const int id, const QString& title )
{
  return insert( id, title, 0 );
}

/*!
  \brief Insert action to the beginning of toolbar.
  \param a action ID
  \param title toolbar title
  \return action ID
*/
int QtxActionToolMgr::prepend( QAction* a, const QString& title )
{
  return insert( a, title, 0 );
}

/*!
  \brief Remove action from toolbar.
  \param id action ID
  \param tid toolbar ID
*/
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

  triggerUpdate( tid );
}

/*!
  \brief Remove action from toolbar.
  \param id action ID
  \param title toolbar title
*/
void QtxActionToolMgr::remove( const int id, const QString& title )
{
  remove( id, find( title ) );
}

/*!
  \brief Get toolbar by given \a tid.
  \param tid toolbar ID
  \return toolbar or 0 if it is not found
*/
QToolBar* QtxActionToolMgr::toolBar( const int tid ) const
{
  QToolBar* tb = 0;
  if ( myToolBars.contains( tid ) )
    tb = myToolBars[tid].toolBar;
  return tb;
}

/*!
  \brief Get toolbar by given \a title.
  \param title toolbar title
  \return toolbar or 0 if it is not found
*/
QToolBar* QtxActionToolMgr::toolBar( const QString& title ) const
{
  return toolBar( find( title ) );
}

/*!
  \bried Get all registered toolbars identifiers
  \return list of toolbars ids
*/
QIntList QtxActionToolMgr::toolBarsIds() const
{
  return myToolBars.keys();
}

/*!
  \brief Check if toolbar with given \a id already registered.
  \param tid toolbar ID
  \return \c true if toolbar is registered in the toolbar manager
*/
bool QtxActionToolMgr::hasToolBar( const int tid ) const
{
  return myToolBars.contains( tid );
}

/*!
  \brief Check if toolbar with given \a id already registered.
  \param title toolbar title
  \return \c true if toolbar is registered in the toolbar manager
*/
bool QtxActionToolMgr::hasToolBar( const QString& title ) const
{
  return find( title ) != -1;
}

/*!
  \brief Check if toolbar contains given action.
  \param id action ID
  \param tid toolbar ID
  \return \c true if toolbar contains action
*/
bool QtxActionToolMgr::containsAction( const int id, const int tid ) const
{
  for ( ToolBarMap::ConstIterator it = myToolBars.begin(); it != myToolBars.end(); ++it )
  {
    if ( tid == -1 || it.key() == tid )
    {
      const NodeList& list = it.value().nodes;
      for ( NodeList::const_iterator nit = list.begin(); nit != list.end(); ++nit )
        if ( (*nit).id == id )
          return true;
    }
  }
  return false;
}

/*!
  \brief Get index of the action \a id within the toolbar \a tid
  \param id action ID
  \param tid toolbar ID
  \return index of the action in the toolbar or -1 if action is not contained in the toolbar
*/
int QtxActionToolMgr::index( const int id, const int tid ) const
{
  for ( ToolBarMap::ConstIterator it = myToolBars.begin(); it != myToolBars.end(); ++it )
  {
    if ( it.key() == tid )
    {
      const NodeList& list = it.value().nodes;
      int idx = 0;
      for ( NodeList::const_iterator nit = list.begin(); nit != list.end(); ++nit, ++idx )
        if ( (*nit).id == id ) return idx;
    }
  }
  return -1;
}

/*!
  \brief Called when toolbar is destroyed.

  Clears internal pointer to the toolbar to disable crashes.
*/
void QtxActionToolMgr::onToolBarDestroyed()
{
  myToolBars.remove( find( (QToolBar*)sender() ) );
}

/*!
  \brief Search toolbar by given \a name.
  \param title toolbar title
  \return toolbar ID or -1 if it is not found
*/
int QtxActionToolMgr::find( const QString& title ) const
{
  int id = -1;
  for ( ToolBarMap::ConstIterator it = myToolBars.begin(); it != myToolBars.end() && id == -1; ++it )
  {
    if ( it.value().toolBar->windowTitle() == title )
      id = it.key();
  }
  return id;
}

/*!
  \brief Get toolbar identifier.
  \param tb toolbar
  \return toolbar ID or -1 if toolbar is not registered
*/
int QtxActionToolMgr::find( QToolBar* tb ) const
{
  int id = -1;
  for ( ToolBarMap::ConstIterator it = myToolBars.begin(); it != myToolBars.end() && id == -1; ++it )
  {
    if ( it.value().toolBar == tb )
      id = it.key();
  }
  return id;
}

/*!
  \brief Update toolbar.
  \param tid toolbar ID
*/
void QtxActionToolMgr::updateToolBar( const int tid )
{
  if ( !isUpdatesEnabled() )
    return;

  if ( !myToolBars.contains( tid ) )
    return;

  QToolBar* tb = myToolBars[tid].toolBar;
  const NodeList& list = myToolBars[tid].nodes;

  for ( NodeList::const_iterator it = list.begin(); it != list.end(); ++it )
  {
    QAction* a = action( (*it).id );
    tb->removeAction( a );
//    if ( a )
//      a->removeFrom( tb );
  }

  tb->clear();

  for ( NodeList::const_iterator itr = list.begin(); itr != list.end(); ++itr )
  {
    if ( !isVisible( (*itr).id, tid ) )
      continue;

    QAction* a = action( (*itr).id );
    tb->addAction( a );
//    if ( a )
//      a->addTo( tb );
  }

  simplifySeparators( tb );
  
  // fix of 19921 -->
  if ( !tb->isVisible() )
    tb->adjustSize();
  // fix of 19921 <--
}

/*!
  \brief Update all registered toolbars.
*/
void QtxActionToolMgr::internalUpdate()
{
  if ( !isUpdatesEnabled() )
    return;

  for ( ToolBarMap::ConstIterator it1 = myToolBars.begin(); it1 != myToolBars.end(); ++it1 )
    updateToolBar( it1.key() );

  myUpdateIds.clear();
}

/*!
  \brief Remove extra separators from the toolbar.
  \param tb toolbar
*/
void QtxActionToolMgr::simplifySeparators( QToolBar* tb )
{
  Qtx::simplifySeparators( tb );
}

/*!
  \brief Show action (in all toolbars).
  \param id action ID
*/
void QtxActionToolMgr::show( const int id )
{
  setShown( id, true );
}

/*!
  \brief Hide action (in all toolbars).
  \param id action ID
*/
void QtxActionToolMgr::hide( const int id )
{
  setShown( id, false );
}

/*!
  \brief Set visibility status for toolbar action with given \a id.
  \param id action ID
  \param on new visibility status
*/
void QtxActionToolMgr::setShown( const int id, const bool on )
{
  for ( ToolBarMap::Iterator it = myToolBars.begin(); it != myToolBars.end(); ++it )
    setVisible( id, it.key(), on );
}

/*!
  \brief Get visibility status for toolbar action with given \a id.
  \param id action ID
  \return \c true if action is shown in all toolbars
*/
bool QtxActionToolMgr::isShown( const int id ) const
{
  QList<const ToolNode*> nodes;
  for ( ToolBarMap::ConstIterator it = myToolBars.begin(); it != myToolBars.end(); ++it )
  {
    const NodeList& nl = it.value().nodes;
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
  for ( QList<const ToolNode*>::iterator itr = nodes.begin(); itr != nodes.end() && vis; ++itr )
    vis = (*itr)->visible;

  return vis;
}

/*!
  \brief Check if an action with given \a id is visible in the toolbar \a tid.
  \param id action ID
  \param tid toolbar ID
  \return \c true if action is shown in the toolbar
*/
bool QtxActionToolMgr::isVisible( const int id, const int tid ) const
{
  if ( !myToolBars.contains( tid ) )
    return false;

  bool vis = false;
  const ToolBarInfo& inf = myToolBars[tid];
  for ( NodeList::const_iterator it = inf.nodes.begin(); it != inf.nodes.end() && !vis; ++it )
  {
    const ToolNode& node = *it;
    if ( node.id == id )

      vis = node.visible;
  }
  return vis;
}

/*!
  \brief Show/hide action with given \a id in the toolbar \a tid.
  \param id action ID
  \param tid toolbar ID
  \param on new visibility status
*/
void QtxActionToolMgr::setVisible( const int id, const int tid, const bool on )
{
  if ( !myToolBars.contains( tid ) )
    return;

  bool changed = false;
  NodeList& lst = myToolBars[tid].nodes;
  for ( NodeList::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    ToolNode& node = *it;
    if ( node.id == id )
    {
      changed = changed || node.visible != on;
      node.visible = on;
    }
  }

  if ( changed ) {
    triggerUpdate( tid );
    updateContent();
  }
}

/*!
  \brief Load toolbar contents from the file.
  \param fname file name
  \param r actions reader
  \return \c true on success and \c false on error
*/
bool QtxActionToolMgr::load( const QString& fname, QtxActionMgr::Reader& r )
{
  ToolCreator cr( &r, this );
  return r.read( fname, cr );
}

/*!
  \brief Called when delayed content update is performed.

  Customizes the content update operation.
*/
void QtxActionToolMgr::updateContent()
{
  if ( !isUpdatesEnabled() )
    return;

  for ( QMap<int,int>::const_iterator it = myUpdateIds.constBegin(); it != myUpdateIds.constEnd(); ++it )
    updateToolBar( it.key() );
  myUpdateIds.clear();
}

/*!
  \brief Perform delayed toolbar update.
  \param tid toolbar ID
*/
void QtxActionToolMgr::triggerUpdate( const int tid )
{
  myUpdateIds.insert( tid, 0 );
  QtxActionMgr::triggerUpdate();
}


/*!
  \class QtxActionToolMgr::ToolCreator
  \brief Toolbars creator.

  Used by Reader to create actions by reading descriptions from the file,
  create toolbars and fill in the toolbara with the actions.
*/

/*!
  \brief Constructor.
  \param r actions reader
  \param mgr toolbar manager
*/
QtxActionToolMgr::ToolCreator::ToolCreator( QtxActionMgr::Reader* r,
                                            QtxActionToolMgr* mgr )
: QtxActionMgr::Creator( r ),
  myMgr( mgr )
{
}

/*!
  \brief Destructor.
*/
QtxActionToolMgr::ToolCreator::~ToolCreator()
{
}

/*!
  \brief Create and append to the action manager a new toolbar or toolbar action.
  \param tag item tag name
  \param subMenu \c true if this item is submenu (not used)
  \param attr attributes map
  \param tid toolbar ID
  \return toolbar or toolbar action ID
*/
int QtxActionToolMgr::ToolCreator::append( const QString& tag, const bool /*subMenu*/,
                                           const ItemAttributes& attr, const int tid )
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
  if( tid==-1 )
    res = myMgr->createToolBar( strValue( attr, label ), intValue( attr, id, -1 ) );
  else if( tag==sep )
    res = myMgr->insert( separator(), tid, intValue( attr, pos, -1 ) );
  else
  {
    QIcon set;
    QPixmap pix;
    QString name = strValue( attr, icon );
    if( !name.isEmpty() && loadPixmap( name, pix ) )
      set = QIcon( pix );

    QtxAction* newAct = new QtxAction( strValue( attr, tooltip ), set, strValue( attr, label ),
                                       QKeySequence( strValue( attr, accel ) ), myMgr );
    QString toggleact = strValue( attr, toggle );
    newAct->setCheckable( !toggleact.isEmpty() );
    newAct->setChecked( toggleact.toLower() == "true" );
        
    connect( newAct );
    int aid = myMgr->registerAction( newAct, actId );
    res = myMgr->insert( aid, tid, intValue( attr, pos, -1 ) );
  }

  return res;
}
