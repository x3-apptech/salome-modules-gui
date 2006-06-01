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
// File:      QtxDockAction.cxx
// Author:    Sergey TELKOV

#include "QtxDockAction.h"

#include "QtxResourceMgr.h"

#include <qevent.h>
#include <qtoolbar.h>
#include <qdockarea.h>
#include <qdockwindow.h>
#include <qmainwindow.h>
#include <qobjectlist.h>
#include <qapplication.h>

/*!
	Name: QtxDockAction [public]
	Desc: Constructs an Dock action with given main window and name.
*/

QtxDockAction::QtxDockAction( QMainWindow* mw, const char* name )
: QtxAction( "Windows and Toolbars", "Windows and Toolbars", 0, mw, name ),
myMain( mw ),
myAutoAdd( true ),
mySeparate( true ),
myAutoPlace( false )
{
  initialize( mw );
}

/*!
	Name: QtxDockAction [public]
	Desc: This constructor creates an action with the following properties: the
		    description text, the menu text and. It is a child of given main window
        and named specified name.
*/

QtxDockAction::QtxDockAction( const QString& text, const QString& menuText, QMainWindow* mw, const char* name )
: QtxAction( text, menuText, 0, mw, name ),
myMain( mw ),
myAutoAdd( true ),
mySeparate( true ),
myAutoPlace( false )
{
  initialize( mw );
}

/*!
	Name: QtxDockAction [public]
	Desc: This constructor creates an action with the following properties: the
		    description text, the menu text, the icon or iconset icon and keyboard
        accelerator. It is a child of given main window and named specified name.
*/

QtxDockAction::QtxDockAction( const QString& text, const QIconSet& icon, const QString& menuText, QMainWindow* mw, const char* name )
: QtxAction( text, icon, menuText, 0, mw, name ),
myMain( mw ),
myAutoAdd( true ),
mySeparate( true ),
myAutoPlace( false )
{
  initialize( mw );
}

/*!
	Name: ~QtxDockAction [public]
	Desc: Removes all added popup items.
*/

QtxDockAction::~QtxDockAction()
{
  for ( MenuMap::ConstIterator mIt = myMenu.begin(); mIt != myMenu.end(); ++mIt )
    removeFrom( mIt.key() );

  for ( InfoMap::ConstIterator iIt = myInfo.begin(); iIt != myInfo.end(); ++iIt )
    delete iIt.data().a;
}

/*!
	Name: mainWindow [public]
	Desc: Returns the main window which contains managed dock windows.
*/

QMainWindow* QtxDockAction::mainWindow() const
{
  return myMain;
}

/*!
	Name: isAutoAdd [public]
	Desc: Returns the auto add property. If this property is setted then all newly
        appeared dock windows will be automatically added.
*/

bool QtxDockAction::isAutoAdd() const
{
  return myAutoAdd;
}

/*!
	Name: setAutoAdd [public]
	Desc: Sets the auto add property. If this property is setted then all newly
        appeared dock windows will be automatically added.
*/

void QtxDockAction::setAutoAdd( const bool on )
{
  myAutoAdd = on;
}

/*!
	Name: isAutoPlace [public]
	Desc: Returns the auto place property. If this property is setted then all newly
        added dock windows will be automatically placed according stored place information.
*/

bool QtxDockAction::isAutoPlace() const
{
  return myAutoPlace;
}

/*!
	Name: setAutoPlace [public]
	Desc: Sets the auto place property. If this property is setted then all newly
        added dock windows will be automatically placed according stored place
        information.
*/

void QtxDockAction::setAutoPlace( const bool on )
{
  myAutoPlace = on;
}

/*!
	Name: isSeparate [public]
	Desc: Returns the 'separate' property.
*/

bool QtxDockAction::isSeparate() const
{
  return mySeparate;
}

/*!
	Name: setSeparate [public]
	Desc: Sets the 'separate' property. If this property is 'true' then toolbars and
        dock windows menu items will be placed in different popup menus  otherwise
        their will  be placed  in  one  common  popup  menu. This property will be
        affected in next method 'addTo'.
*/

void QtxDockAction::setSeparate( const bool on )
{
  if ( mySeparate == on )
    return;

  mySeparate = on;
  updateMenus();
}

/*!
	Name: addTo [public]
	Desc: Add the dock windows sub menu item to the end of specified popup.
*/

bool QtxDockAction::addTo( QWidget* wid )
{
  return addTo( wid, -1 );
}

/*!
	Name: addTo [public]
	Desc: Add the dock windows sub menu item to specified popup at the given index.
*/

bool QtxDockAction::addTo( QWidget* wid, const int idx )
{
  if ( !wid || !wid->inherits( "QPopupMenu" ) )
    return false;

  QPopupMenu* pm = (QPopupMenu*)wid;
  checkPopup( pm );

  if ( myMenu.contains( pm ) )
    return false;

  MenuInfo mInfo;
  mInfo.dock = new QPopupMenu( pm );
  mInfo.tool = isSeparate() ? new QPopupMenu( pm ) : 0;

  QString dock, tool;
  splitMenuText( dock, tool );

  myMenu.insert( pm, mInfo );

  int index = idx;

  if ( mInfo.dock )
    iconSet().isNull() ? pm->insertItem ( dock, mInfo.dock, -1, index ) :
                         pm->insertItem ( iconSet(), dock, mInfo.dock, -1, index );

  if ( index >= 0 )
    index++;

  if ( mInfo.tool )
    iconSet().isNull() ? pm->insertItem ( tool, mInfo.tool, -1, index ) :
                         pm->insertItem ( iconSet(), tool, mInfo.tool, -1, index );

  connect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
  connect( pm, SIGNAL( destroyed( QObject* ) ), this, SLOT( onPopupDestroyed( QObject* ) ) );

  return true;
}

/*!
	Name: removeFrom [public]
	Desc: Removes dock window sub menu from specified popup.
*/

bool QtxDockAction::removeFrom( QWidget* wid )
{
  QPopupMenu* pm = (QPopupMenu*)wid;

  if ( myMenu.contains( pm ) )
  {
    pm->removeItem( findId( pm, myMenu[pm].dock ) );
    pm->removeItem( findId( pm, myMenu[pm].tool ) );

    delete myMenu[pm].dock;
    delete myMenu[pm].tool;
    myMenu.remove( pm );

    disconnect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
    disconnect( pm, SIGNAL( destroyed( QObject* ) ), this, SLOT( onPopupDestroyed( QObject* ) ) );
  }

  return QtxAction::removeFrom( wid );
}

/*!
  Sets menu text of action
  \param txt - new menu text
*/
void QtxDockAction::setMenuText( const QString& txt )
{
  if ( menuText() == txt )
    return;

  QtxAction::setMenuText( txt );
  updateMenus();
}

/*!
	Name: addDockWindow [public]
	Desc: Add dock window to internal data structures. Action will be include all added
        dock windows in to menu and manage their place configuration.
*/

bool QtxDockAction::addDockWindow( QDockWindow* dw )
{
  if ( !dw || !mainWindow() )
    return false;

  if ( myInfo.contains( dw ) )
    return false;

  myInfo.insert( dw, DockInfo() );

  DockInfo& inf = myInfo[dw];
  inf.name = windowName( dw );
  QAction* a = inf.a = new QAction( mainWindow(), 0, true );

  autoLoadPlaceInfo( dw );

  bool block = a->signalsBlocked();
  a->blockSignals( true );
  a->setOn( dw->isVisibleTo( mainWindow() ) );
  a->blockSignals( block );

  updateInfo( dw );
  savePlaceInfo( dw );

  dw->installEventFilter( this );

  connect( a, SIGNAL( toggled( bool ) ), this, SLOT( onToggled( bool ) ) );
  connect( dw, SIGNAL( destroyed( QObject* ) ), this, SLOT( onWindowDestroyed( QObject* ) ) );
  connect( dw, SIGNAL( visibilityChanged( bool ) ), this, SLOT( onVisibilityChanged( bool ) ) );

  return true;
}

/*!
	Name: removeDockWindow [public]
	Desc: Remove dock window from internal data structures. Action will not manage this window.
*/

bool QtxDockAction::removeDockWindow( QDockWindow* dw )
{
  if ( !myInfo.contains( dw ) )
    return false;

  myGeom.remove( myInfo[dw].name );

  delete myInfo[dw].a;
  myInfo.remove( dw );

  dw->removeEventFilter( this );

  disconnect( dw, SIGNAL( destroyed( QObject* ) ), this, SLOT( onWindowDestroyed( QObject* ) ) );
  disconnect( dw, SIGNAL( visibilityChanged( bool ) ), this, SLOT( onVisibilityChanged( bool ) ) );

  return true;
}

/*!
	Name: eventFilter [public]
	Desc: Event filter process caption and icon changing of managed dock windows
        and try to add newly appeared dock windows.
*/

bool QtxDockAction::eventFilter( QObject* o, QEvent* e )
{
  if ( o->inherits( "QDockWindow" ) && ( e->type() == QEvent::CaptionChange ||
                                         e->type() == QEvent::IconChange ) )
    updateInfo( (QDockWindow*)o );

  if ( o->inherits( "QDockArea" ) && e->type() == QEvent::ChildRemoved )
  {
    QChildEvent* ce = (QChildEvent*)e;
    if ( ce->child() && ce->child()->inherits( "QDockWindow" ) )
      savePlaceInfo( (QDockWindow*)ce->child() );
  }

  if ( o->inherits( "QDockArea" ) && e->type() == QEvent::ChildInserted )
  {
    QChildEvent* ce = (QChildEvent*)e;
    if ( ce->child() && ce->child()->inherits( "QDockWindow" ) )
      QApplication::postEvent( this, new QCustomEvent( (QEvent::Type)AutoAdd, ce->child() ) );
  }

  return false;
}

/*!
	Name: restoreGeometry [public]
	Desc: Retrieve the dock window geometry. If dock window specified as 0
        then all windows geometry will be restored.
*/

void QtxDockAction::restoreGeometry( QDockWindow* dw ) const
{
  if ( !dw )
    loadPlaceInfo();
  else
    loadPlaceInfo( dw );
}

/*!
	Name: storeGeometry [public]
	Desc: Store the dock window geometry. If dock window specified as 0
        then all windows geometry will be stored.
*/

void QtxDockAction::storeGeometry( QDockWindow* dw )
{
  QPtrList<QDockWindow> dwList;

  if ( dw )
    dwList.append( dw );
  else
    dockWindows( dwList );

  for ( QPtrListIterator<QDockWindow> it( dwList ); it.current(); ++it )
    savePlaceInfo( it.current() );
}

/*!
	Name: loadGeometry [public]
	Desc: Retrieve the dock windows geometry information from the specified resource manager section.
*/

void QtxDockAction::loadGeometry( QtxResourceMgr* resMgr, const QString& section, const bool clear )
{
  QString sec = section.stripWhiteSpace();
  if ( !resMgr || sec.isEmpty() )
    return;

  myNames = QStringList::split( "|", resMgr->stringValue( sec, "windows_list", QString::null ) );

  QMap<QString, int> map;
  QStringList params = resMgr->parameters( sec );
  for ( QStringList::const_iterator it = params.begin(); it != params.end(); ++it )
  {
    QString p = QStringList::split( ".", *it, true ).first().stripWhiteSpace();
    if ( !p.isEmpty() )
      map.insert( p, 0 );
  }

  for ( QMap<QString, int>::ConstIterator itr = map.begin(); itr != map.end(); ++itr )
  {
    GeomInfo inf;
    if ( !clear && myGeom.contains( itr.key() ) )
      inf = myGeom[itr.key()];
    else
    {
      inf.vis = true; inf.newLine = false; inf.place = DockTop;
      inf.index = 0; inf.offset = 0;
      inf.x = 0; inf.y = 0; inf.w = 0; inf.h = 0;
      inf.fixW = -1; inf.fixH = -1;
    }
    if ( loadGeometry( resMgr, sec, itr.key(), inf ) )
      myGeom.insert( itr.key(), inf );
  }
}

/*!
	Name: saveGeometry [public]
	Desc: Store the dock windows geometry information into the specified resource manager section.
*/

void QtxDockAction::saveGeometry( QtxResourceMgr* resMgr, const QString& section, const bool clear ) const
{
  QString sec = section.stripWhiteSpace();
  if ( !resMgr || sec.isEmpty() )
    return;

  QtxDockAction* that = (QtxDockAction*)this;
  that->storeGeometry();

  that->myNames.clear();
  collectNames( Minimized, that->myNames );
  for ( int i = DockTornOff; i < Minimized; i++ )
    collectNames( i, that->myNames );

  if ( clear )
    resMgr->remove( sec );

  resMgr->setValue( sec, "windows_list", myNames.join( "|" ) );

  for ( GeomMap::ConstIterator it = myGeom.begin(); it != myGeom.end(); ++it )
    saveGeometry( resMgr, sec, it.key(), it.data() );
}

/*!
	Name: onAboutToShow [private slots]
	Desc: Prepare sub popup with dock windows list when parent popup is shown.
*/

void QtxDockAction::onAboutToShow()
{
  const QObject* obj = sender();
  if ( obj && obj->inherits( "QPopupMenu" ) )
  {
    QPopupMenu* pm = (QPopupMenu*)obj;
    fillPopup( pm );
    pm->setItemEnabled( findId( pm, myMenu[pm].dock ), isEnabled() && myMenu[pm].dock && myMenu[pm].dock->count() );
    pm->setItemEnabled( findId( pm, myMenu[pm].tool ), isEnabled() && myMenu[pm].tool && myMenu[pm].tool->count() );
  }
}

/*!
	Name: onToggled [private slots]
	Desc: Show or hide dock window when user toggled window item in popup.
*/

void QtxDockAction::onToggled( bool on )
{
  QDockWindow* dw = dockWindow( (QAction*)sender() );
  if ( dw )
    on ? dw->show() : dw->hide();
}

/*!
	Name: onPopupDestroyed [private slots]
	Desc: Remove destroyed popup from data structures.
*/

void QtxDockAction::onPopupDestroyed( QObject* obj )
{
  myMenu.remove( (QPopupMenu*)obj );
}

/*!
	Name: onWindowDestroyed [private slots]
	Desc: Remove information about destroyed dock window.
*/

void QtxDockAction::onWindowDestroyed( QObject* obj )
{
  QDockWindow* dw = (QDockWindow*)obj;
  if ( !myInfo.contains( dw ) )
    return;

  delete myInfo[dw].a;
  myInfo.remove( dw );
}

/*!
	Name: onVisibilityChanged [private slots]
	Desc: Toggle corresponded action when visibility state of dock window changed.
*/

void QtxDockAction::onVisibilityChanged( bool on )
{
  const QObject* obj = sender();
  if ( !obj || !obj->inherits( "QDockWindow" ) )
    return;

  QDockWindow* dw = (QDockWindow*)obj;

  QAction* a = action( dw );
  if ( a && a->isOn() != on )
  {
    bool block = a->signalsBlocked();
    a->blockSignals( true );
    a->setOn( on );
    a->blockSignals( block );
  }

  savePlaceInfo( dw );
}

/*!
	Name: onDockWindowPositionChanged [private slots]
	Desc: Update dock window place information
*/

void QtxDockAction::onDockWindowPositionChanged( QDockWindow* dw )
{
  savePlaceInfo( dw );
}

/*!
	Name: event [protected]
	Desc: Check consistency the popup content and internal datas.
        Synchronize internal data structures with popup content.
*/

bool QtxDockAction::event( QEvent* e )
{
  if ( e->type() == (int)AutoAdd )
  {
    QCustomEvent* ce = (QCustomEvent*)e;
    QDockWindow* dw = (QDockWindow*)ce->data();
    if ( !myInfo.contains( dw ) )
    {
      autoAddDockWindow( dw );
      autoLoadPlaceInfo( dw );
    }
  }

  return QtxAction::event( e );
}

/*!
	Name: checkPopup [private]
	Desc: Check consistency the popup content and internal datas.
        Synchronize internal data structures with popup content.
*/

void QtxDockAction::checkPopup( QPopupMenu* pm )
{
  if ( !myMenu.contains( pm ) )
    return;

  int id = findId( pm, myMenu[pm].dock );
  if ( id == -1 )
  {
    delete myMenu[pm].dock;
    myMenu[pm].dock = 0;
  }
  id = findId( pm, myMenu[pm].tool );
  if ( id == -1 )
  {
    delete myMenu[pm].tool;
    myMenu[pm].tool = 0;
  }

  if ( !myMenu[pm].dock )
  {
    delete myMenu[pm].tool;
    myMenu.remove( pm );
    disconnect( pm, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
  }
}

/*!
	Name: fillPopup [private]
	Desc: Clear the popup and the add to it required actions.
*/

void QtxDockAction::fillPopup( QPopupMenu* pm ) const
{
  if ( !pm || !mainWindow() )
    return;

  if ( !myMenu.contains( pm ) )
    return;

  QPopupMenu* dockMenu = myMenu[pm].dock;
  QPopupMenu* toolMenu = myMenu[pm].tool;

  for ( InfoMap::ConstIterator mit = myInfo.begin(); mit != myInfo.end(); ++mit )
  {
    mit.data().a->removeFrom( dockMenu );
    mit.data().a->removeFrom( toolMenu );
  }

  if ( dockMenu )
    dockMenu->clear();

  if ( toolMenu )
    toolMenu->clear();

  QPtrList<QDockWindow> dockList;
  dockWindows( dockList, mainWindow() );

  if ( dockList.isEmpty() )
    return;

  QPtrList<QAction> toolBars, windows;
  for ( QPtrListIterator<QDockWindow> it( dockList ); it.current(); ++it )
  {
    if ( !myInfo.contains( it.current() ) )
    {
      QtxDockAction* that = (QtxDockAction*)this;
      that->autoAddDockWindow( it.current() );
    }

    if ( !mainWindow()->appropriate( it.current() ) ||
         it.current()->caption().isEmpty() || !action( it.current() ) )
      continue;

    if ( isToolBar( it.current() ) )
      toolBars.append( action( it.current() ) );
    else
      windows.append( action( it.current() ) );
  }

  for ( QPtrListIterator<QAction> wit( windows ); wit.current(); ++wit )
    wit.current()->addTo( dockMenu );

  dockMenu->insertSeparator();

  for ( QPtrListIterator<QAction> tit( toolBars ); tit.current(); ++tit )
    tit.current()->addTo( toolMenu ? toolMenu : dockMenu );

  Qtx::simplifySeparators( dockMenu );
  Qtx::simplifySeparators( toolMenu );
}

/*!
	Name: isToolBar [private]
	Desc: Returns 'true' if dock window is a toolbar.
*/

bool QtxDockAction::isToolBar( QDockWindow* dw ) const
{
  return dw && dw->inherits( "QToolBar" );
}

/*!
	Name: findId [private]
	Desc: Returns identificator of popup item which contains sub popup 'pm' in the popup 'cont'.
*/

int QtxDockAction::findId( QPopupMenu* cont, QPopupMenu* pm ) const
{
  if ( !cont || !pm )
    return -1;

  int id = -1;

  for ( int i = 0; i < (int)cont->count() && id == -1; i++ )
  {
    QMenuData* md = 0;
    QMenuItem* item = cont->findItem( cont->idAt( i ), &md );
    if ( item && md == cont && item->popup() == pm )
      id = item->id();
  }
  return id;
}

/*!
	Name: dockWindows [private]
	Desc: Returns all dock windows of the main window.
*/

void QtxDockAction::dockWindows( QPtrList<QDockWindow>& lst, QMainWindow* main ) const
{
  lst.clear();

  QMainWindow* mw = main ? main : mainWindow();
  if ( !mw )
    return;

  QObjectList* objs = mw->queryList( "QDockWindow" );
  if ( objs )
  {
    for ( QObjectListIt it( *objs ); it.current(); ++it )
    {
      QDockWindow* dockWin = ::qt_cast<QDockWindow*>( it.current() );
      if ( dockWin && dockMainWindow( mw, dockWin ) )
        lst.append( dockWin );
    }
  }
  delete objs;
}

/*!
  \return true if main window is parent of object
  \param mw - main window
  \param win - object
*/
bool QtxDockAction::dockMainWindow( QMainWindow* mw, QObject* win ) const
{
  if ( !mw || !win )
    return false;

  while ( win )
  {
    if ( win->parent() && win->parent() == mw )
      return true;

    if ( ::qt_cast<QMainWindow*>( win->parent() ) )
      return false;

    win = win->parent();
  }

  return false;
}

/*!
	Name: updateInfo [private]
	Desc: Updates icon and caption info of dock window in the corresponded action.
*/

void QtxDockAction::updateInfo( QDockWindow* dw )
{
  QAction* a = action( dw );
  if ( !a )
    return;

  a->setText( dw->caption() );
  a->setMenuText( dw->caption() );

  if ( isToolBar( dw ) )
    a->setStatusTip( tr( "Toggles toolbar \"%1\" on/off" ).arg( dw->caption() ) );
  else
    a->setStatusTip( tr( "Toggles window \"%1\" on/off" ).arg( dw->caption() ) );

  const QPixmap* icon = dw->icon();
  if ( icon )
    a->setIconSet( *icon );
}

/*!
	Name: savePlaceInfo [private]
	Desc: Store the place and geometry information from specified dock window.
*/

void QtxDockAction::savePlaceInfo( QDockWindow* dw )
{
  if ( !myInfo.contains( dw ) )
    return;

  if ( !myGeom.contains( myInfo[dw].name ) )
    myGeom.insert( myInfo[dw].name, GeomInfo() );

  GeomInfo& inf = myGeom[myInfo[dw].name];

  Dock dock;
  inf.vis = dw->isVisibleTo( mainWindow() );
  mainWindow()->getLocation( dw, dock, inf.index, inf.newLine, inf.offset );

  inf.place = dock;
  inf.x = dw->x();
  inf.y = dw->y();
  inf.w = dw->width();
  inf.h = dw->height();
  inf.fixW = dw->fixedExtent().width();
  inf.fixH = dw->fixedExtent().height();
}

/*!
	Name: loadPlaceInfo [private]
	Desc: Retrieve the stored place and geometry information to specified dock window.
*/

void QtxDockAction::loadPlaceInfo( QDockWindow* dw ) const
{
  if ( !myInfo.contains( dw ) )
    return;

  QMainWindow* mw = mainWindow();
  if ( !mw )
    return;

  QObject* p = dw->parent();
  if ( !( !p || p == mw || ( p->parent() && p->parent() == mw ) ) )
    return;

  QString winName = myInfo[dw].name;
  if ( winName.isEmpty() || !myGeom.contains( winName ) )
    return;

  GeomInfo inf = myGeom[myInfo[dw].name];

  mainWindow()->moveDockWindow( dw, (Qt::Dock)inf.place, inf.newLine, inf.index, inf.offset );
  dw->setGeometry( inf.x, inf.y, inf.w, inf.h );

  dw->setFixedExtentWidth( inf.fixW );
  dw->setFixedExtentHeight( inf.fixH );

  QtxDockAction* that = (QtxDockAction*)this;
  that->myGeom.insert( myInfo[dw].name, inf );
}

/*!
	Name: loadPlaceInfo [private]
	Desc: Retrieve the stored place and geometry information to all dock windows.
*/

void QtxDockAction::loadPlaceInfo() const
{
  QMainWindow* mw = mainWindow();
  if ( !mw )
    return;

  typedef QPtrList<QDockWindow> DockWinList;

  DockWinList lst;
  dockWindows( lst, mw );

  QMap<QString, QDockWindow*> nameMap;
  for ( QPtrListIterator<QDockWindow> itr( lst ); itr.current(); ++itr )
  {
    QObject* p = itr.current()->parent();
    if ( !( !p || p == mw || ( p->parent() && p->parent() == mw ) ) )
      continue;

    QString name;
    if ( myInfo.contains( itr.current() ) )
      name = myInfo[itr.current()].name;

    if ( !myGeom.contains( name ) )
      continue;

    nameMap.insert( name, itr.current() );
  }

  DockWinList winList;
  for ( QStringList::const_iterator iter = myNames.begin(); iter != myNames.end(); ++iter )
  {
    if ( nameMap.contains( *iter ) )
      winList.append( nameMap[*iter] );
    nameMap.remove( *iter );
  }

  for ( QMap<QString, QDockWindow*>::ConstIterator mIt = nameMap.begin(); mIt != nameMap.end(); ++mIt )
    winList.append( mIt.data() );

  QMap<int, DockWinList> winMap;
  QMap<QDockWindow*, GeomInfo*> geomMap;

  for ( QPtrListIterator<QDockWindow> it( winList ); it.current(); ++it )
  {
    QString name;
    if ( myInfo.contains( it.current() ) )
      name = myInfo[it.current()].name;

    if ( !myGeom.contains( name ) )
      continue;

    //! collect pointer of info to have fresh info data after processEvents();
    GeomInfo* inf = (GeomInfo*)&( myGeom[name] );
    geomMap.insert( it.current(), inf );
    if ( !winMap.contains( inf->place ) )
      winMap.insert( inf->place, DockWinList() );
    winMap[inf->place].append( it.current() );
  }

  loadPlaceArea( DockMinimized, mw, 0,
                 winMap.contains( DockMinimized ) ? winMap[DockMinimized] : DockWinList(), geomMap );
  for ( int i = DockTornOff; i < DockMinimized; i++ )
  {
    loadPlaceArea( i, mw, dockArea( i ), winMap.contains( i ) ? winMap[i] : DockWinList(), geomMap );
  }
}

/*!
	Name: loadPlaceArea [private]
	Desc: Set the place and geometry information to all dock windows in the area.
*/

void QtxDockAction::loadPlaceArea( const int place, QMainWindow* mw, QDockArea* area,
                                   const QPtrList<QDockWindow>& dockList,
                                   const QMap<QDockWindow*, GeomInfo*>& geomMap ) const
{
  for ( QPtrListIterator<QDockWindow> it( dockList ); it.current(); ++it )
  {
    if ( !geomMap.contains( it.current() ) )
      continue;

    GeomInfo* inf = geomMap[it.current()];
    mw->moveDockWindow( it.current(), (Qt::Dock)place, inf->newLine, inf->index, inf->offset );
  }

  if ( !area )
    return;

  qApp->processEvents();

  for ( QPtrListIterator<QDockWindow> itr( dockList ); itr.current(); ++itr )
  {
    QDockWindow* dw = itr.current();
    if ( !geomMap.contains( dw ) )
      continue;

    GeomInfo* inf = geomMap[dw];
    if ( place != DockTornOff )
    {
      dw->setNewLine( inf->newLine );
		  dw->setOffset( inf->offset );
		  dw->setFixedExtentWidth( inf->fixW );
		  dw->setFixedExtentHeight( inf->fixH );
    }
    dw->setGeometry( inf->x, inf->y, inf->w, inf->h );

    QAction* a = action( dw );
    if ( a )
    {
      bool block = a->signalsBlocked();
      a->blockSignals( true );
      a->setOn( inf->vis );
      a->blockSignals( block );
    }

    if ( mainWindow() && mainWindow()->appropriate( dw ) )
      inf->vis ? dw->show() : dw->hide();
  }

  QWidget* wid = area;
  if ( wid->layout() )
  {
    wid->layout()->invalidate();
    wid->layout()->activate();
  }
}

/*!
	Name: action [private]
	Desc: Returns action for the given dock window.
*/

QAction* QtxDockAction::action( QDockWindow* dw ) const
{
  QAction* a = 0;
  if ( myInfo.contains( dw ) )
    a = myInfo[dw].a;
  return a;
}

/*!
	Name: dockWindow [private]
	Desc: Returns dock window for the given action.
*/

QDockWindow* QtxDockAction::dockWindow( const QAction* a ) const
{
  QDockWindow* dw = 0;
  for ( InfoMap::ConstIterator it = myInfo.begin(); it != myInfo.end() && !dw; ++it )
  {
    if ( it.data().a == a )
      dw = it.key();
  }
  return dw;
}

/*!
	Name: initialize [private]
	Desc: Initialisation of the object. Sets the event filters and add existing dock windows.
*/

void QtxDockAction::initialize( QMainWindow* mw )
{
  if ( !mw )
    return;

  QPtrList<QDockWindow> lst;
  dockWindows( lst, mw );

  for ( QPtrListIterator<QDockWindow> it( lst ); it.current(); ++it )
    QApplication::postEvent( this, new QCustomEvent( (QEvent::Type)AutoAdd, it.current() ) );

  if ( mw->topDock() )
    mw->topDock()->installEventFilter( this );
  if ( mw->leftDock() )
    mw->leftDock()->installEventFilter( this );
  if ( mw->rightDock() )
    mw->rightDock()->installEventFilter( this );
  if ( mw->bottomDock() )
    mw->bottomDock()->installEventFilter( this );

  connect( mw, SIGNAL( dockWindowPositionChanged( QDockWindow* ) ),
           this, SLOT( onDockWindowPositionChanged( QDockWindow* ) ) );
}

/*!
	Name: windowName [private]
	Desc: Generate the dock window name.
*/

QString QtxDockAction::windowName( QDockWindow* dw ) const
{
  QString name;

  if ( dw )
  {
    name = dw->name( "" );
    if ( name.isEmpty() )
      name = dw->caption();
  }

  return name;
}

/*!
	Name: autoAddDockWindow [private]
	Desc: Add the dock window if auto add property is setted.
*/

bool QtxDockAction::autoAddDockWindow( QDockWindow* dw )
{
  if ( !isAutoAdd() )
    return false;

  return addDockWindow( dw );
}

/*!
	Name: autoLoadPlaceInfo [private]
	Desc: Retieve the dock window place geometry if auto place property is setted.
*/

void QtxDockAction::autoLoadPlaceInfo( QDockWindow* dw )
{
  if ( isAutoPlace() )
    loadPlaceInfo( dw );
}

/*!
	Name: splitMenuText [private]
	Desc: 
*/

void QtxDockAction::splitMenuText( QString& dock, QString& tool ) const
{
  dock = tool = menuText();
  if ( !isSeparate() )
    return;

  QStringList lst = splitText( menuText(), "|" );
  if ( lst.count() < 2 )
    lst = splitText( menuText(), "and" );

  dock = lst.first();
  tool = lst.last();
}

/*!
	Name: splitText [private]
	Desc: 
*/

QStringList QtxDockAction::splitText( const QString& str, const QString& sep ) const
{
  QStringList res;

  int idx = str.lower().find( sep.lower() );
  if ( idx != -1 )
  {
    res.append( str.mid( 0, idx ).stripWhiteSpace() );
    res.append( str.mid( idx + sep.length() ).stripWhiteSpace() );
  }

  return res;
}

/*!
	Name: dockPlace [private]
	Desc: 
*/

int QtxDockAction::dockPlace( const QString& dockName ) const
{
  static QMap<QString, int> dockNameMap;
  if ( dockNameMap.isEmpty() )
  {
    dockNameMap["top"]       = DockTop;
    dockNameMap["bottom"]    = DockBottom;
    dockNameMap["left"]      = DockLeft;
    dockNameMap["right"]     = DockRight;
    dockNameMap["tornoff"]   = DockTornOff;
    dockNameMap["torn_off"]  = DockTornOff;
    dockNameMap["outside"]   = DockTornOff;
    dockNameMap["undock"]    = DockTornOff;
    dockNameMap["minimized"] = DockMinimized;
    dockNameMap["unmanaged"] = DockUnmanaged;
  }

  int res = -1;
  if ( dockNameMap.contains( dockName.lower() ) )
    res = dockNameMap[dockName.lower()];
  return res;
}

/*!
	Name: dockArea [private]
	Desc: 
*/

QDockArea* QtxDockAction::dockArea( const int place ) const
{
  if ( !mainWindow() )
    return 0;

  QDockArea* area = 0;
  switch ( place )
  {
  case DockTop:
    area = mainWindow()->topDock();
    break;
  case DockBottom:
    area = mainWindow()->bottomDock();
    break;
  case DockLeft:
    area = mainWindow()->leftDock();
    break;
  case DockRight:
    area = mainWindow()->rightDock();
    break;
  }
  return area;
}

/*!
	Name: loadGeometry [private]
	Desc: 
*/

bool QtxDockAction::loadGeometry( QtxResourceMgr* resMgr, const QString& sec,
                                  const QString& name, GeomInfo& inf ) const
{
  if ( !resMgr || sec.isEmpty() || name.isEmpty() )
    return false;

  QString tmpl = QString( "%1.%2" ).arg( name );

  inf.vis     = resMgr->booleanValue( sec, tmpl.arg( "visible" ), inf.vis );
  inf.newLine = resMgr->booleanValue( sec, tmpl.arg( "new_line" ), inf.newLine );

  inf.index   = resMgr->integerValue( sec, tmpl.arg( "index" ), inf.index );
  inf.offset  = resMgr->integerValue( sec, tmpl.arg( "offset" ), inf.offset );

  inf.x       = resMgr->integerValue( sec, tmpl.arg( "x" ), inf.x );
  inf.y       = resMgr->integerValue( sec, tmpl.arg( "y" ), inf.y );
  inf.w       = resMgr->integerValue( sec, tmpl.arg( "width" ), inf.w );
  inf.h       = resMgr->integerValue( sec, tmpl.arg( "height" ), inf.h );

  inf.fixW    = resMgr->integerValue( sec, tmpl.arg( "fixed_width" ), inf.fixW );
  inf.fixH    = resMgr->integerValue( sec, tmpl.arg( "fixed_height" ), inf.fixH );

  int place = -1;
  if ( !resMgr->value( sec, tmpl.arg( "place" ), place ) )
  {
    QString placeStr;
    if ( resMgr->value( sec, tmpl.arg( "place" ), placeStr ) )
      place = dockPlace( placeStr );
  }

  if ( place >= DockUnmanaged && place <= DockMinimized )
    inf.place = place;

  return true;
}

/*!
	Name: saveGeometry [private]
	Desc: 
*/

bool QtxDockAction::saveGeometry( QtxResourceMgr* resMgr, const QString& sec,
                                  const QString& name, const GeomInfo& inf ) const
{
  if ( !resMgr || sec.isEmpty() || name.isEmpty() )
    return false;

  QString tmpl = QString( "%1.%2" ).arg( name );

  resMgr->setValue( sec, tmpl.arg( "visible" ), inf.vis );
  resMgr->setValue( sec, tmpl.arg( "new_line" ), inf.newLine );
  resMgr->setValue( sec, tmpl.arg( "index" ), inf.index );
  resMgr->setValue( sec, tmpl.arg( "offset" ), inf.offset );
  resMgr->setValue( sec, tmpl.arg( "x" ), inf.x );
  resMgr->setValue( sec, tmpl.arg( "y" ), inf.y );
  resMgr->setValue( sec, tmpl.arg( "width" ), inf.w );
  resMgr->setValue( sec, tmpl.arg( "height" ), inf.h );
  resMgr->setValue( sec, tmpl.arg( "fixed_width" ), inf.fixW );
  resMgr->setValue( sec, tmpl.arg( "fixed_height" ), inf.fixH );
  resMgr->setValue( sec, tmpl.arg( "place" ), inf.place );

  return true;
}

/*!
	Name: collectNames [private]
	Desc: 
*/

void QtxDockAction::collectNames( const int place, QStringList& lst ) const
{
  QPtrList<QDockWindow> winList;
  QDockArea* area = dockArea( place );
  if ( area )
    winList = area->dockWindowList();
  else
    winList = mainWindow()->dockWindows( (Qt::Dock)place );

  for ( QPtrListIterator<QDockWindow> it( winList ); it.current(); ++it )
  {
    QString name;
    if ( myInfo.contains( it.current() ) )
      name = myInfo[it.current()].name;
    if ( name.isEmpty() )
      name = windowName( it.current() );
    if ( name.isEmpty() )
      continue;

    lst.append( name );
  }
}

/*!
  Updates menu of action
*/
void QtxDockAction::updateMenus()
{
  for ( MenuMap::Iterator it = myMenu.begin(); it != myMenu.end(); ++it )
  {
    QPopupMenu* pm = it.key();
    MenuInfo& inf = it.data();

    int toolId = findId( pm, inf.tool );
    int dockId = findId( pm, inf.dock );

    int index = pm->indexOf( dockId );

    if ( isSeparate() && !inf.tool )
      inf.tool = new QPopupMenu( pm );

    pm->removeItem( dockId );
    pm->removeItem( toolId );

    if ( !isSeparate() && inf.tool )
    {
      delete inf.tool;
      inf.tool = 0;
    }

    QString dock, tool;
    splitMenuText( dock, tool );

    if ( inf.dock )
      iconSet().isNull() ? pm->insertItem ( dock, inf.dock, -1, index ) :
                           pm->insertItem ( iconSet(), dock, inf.dock, -1, index );

    if ( index >= 0 )
      index++;

    if ( inf.tool )
      iconSet().isNull() ? pm->insertItem ( tool, inf.tool, -1, index ) :
                          pm->insertItem ( iconSet(), tool, inf.tool, -1, index );
  }
}
