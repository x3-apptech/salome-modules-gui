// File:      QtxWorkstack.cxx
// Author:    Sergey TELKOV

#include "QtxWorkstack.h"

#include <qstyle.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qiconset.h>
#include <qpainter.h>
#include <qsplitter.h>
#include <qpopupmenu.h>
#include <qobjectlist.h>
#include <qpushbutton.h>
#include <qwidgetstack.h>
#include <qapplication.h>

/*!
    Class: QtxWorkstack [Public]
    Descr:
*/

QtxWorkstack::QtxWorkstack( QWidget* parent )
: QWidget( parent ),
myWin( 0 ),
myArea( 0 )
{
  QVBoxLayout* base = new QVBoxLayout( this );
  mySplit = new QSplitter( this );
  mySplit->setChildrenCollapsible( false );
  base->addWidget( mySplit );
}

QtxWorkstack::~QtxWorkstack()
{
}

QWidgetList QtxWorkstack::windowList() const
{
  QPtrList<QtxWorkstackArea> lst;
  areas( mySplit, lst, true );

  QWidgetList widList;
  for ( QPtrListIterator<QtxWorkstackArea> it( lst ); it.current(); ++it )
  {
    QWidgetList wids = it.current()->widgetList();
    for ( QWidgetListIt itr( wids ); itr.current(); ++itr )
      widList.append( itr.current() );
  }

  return widList;
}

QWidgetList QtxWorkstack::splitWindowList() const
{
  return myArea ? myArea->widgetList() : QWidgetList();
}

QWidget* QtxWorkstack::activeWindow() const
{
  return myWin;
}

void QtxWorkstack::split( const int o )
{
  QtxWorkstackArea* area = activeArea();
  if ( !area )
    return;

  if ( area->widgetList().count() < 2 )
    return;

  QWidget* curWid = area->activeWidget();
  if ( !curWid )
    return;

  QSplitter* s = splitter( area );
  QPtrList<QtxWorkstackArea> areaList;
  areas( s, areaList );

  QPtrList<QSplitter> splitList;
  splitters( s, splitList );

  QSplitter* trg = 0;
  if ( areaList.count() + splitList.count() < 2 || s->orientation() == o )
    trg = s;

  if ( !trg )
    trg = wrapSplitter( area );

  if ( !trg )
    return;

  trg->setOrientation( (Orientation)o );

  QtxWorkstackArea* newArea = createArea( 0 );
  insertWidget( newArea, trg, area );

  area->removeWidget( curWid );
  newArea->insertWidget( curWid );

  distributeSpace( trg );

  curWid->show();
  curWid->setFocus();
}

void QtxWorkstack::distributeSpace( QSplitter* split ) const
{
  if ( !split )
    return;

  QIntList szList = split->sizes();
  int size = ( split->orientation() == Horizontal ?
               split->width() : split->height() ) / szList.count();
  for ( QIntList::iterator it = szList.begin(); it != szList.end(); ++it )
    *it = size;
  split->setSizes( szList );
}

void QtxWorkstack::splitVertical()
{
  split( Qt::Vertical );
}

void QtxWorkstack::splitHorizontal()
{
  split( Qt::Horizontal );
}

QSplitter* QtxWorkstack::wrapSplitter( QtxWorkstackArea* area )
{
  if ( !area )
    return 0;

  QSplitter* pSplit = splitter( area );
  if ( !pSplit )
    return 0;

  bool upd = pSplit->isUpdatesEnabled();
  pSplit->setUpdatesEnabled( false );

  QIntList szList = pSplit->sizes();

  QSplitter* wrap = new QSplitter( 0 );
#if defined QT_VERSION && QT_VERSION >= 0x30200
  wrap->setChildrenCollapsible( false );
#endif
  insertWidget( wrap, pSplit, area );
  area->reparent( wrap, QPoint( 0, 0 ), true );

  pSplit->setSizes( szList );

  pSplit->setUpdatesEnabled( upd );

  return wrap;
}

void QtxWorkstack::insertWidget( QWidget* wid, QWidget* pWid, QWidget* after )
{
  if ( !wid || !pWid )
    return;

  QWidgetList moveList;
  const QObjectList* lst = pWid->children();
  if ( lst )
  {
    bool found = false;
    for ( QObjectListIt it( *lst ); it.current(); ++it )
    {
      if ( found && ( it.current()->inherits( "QSplitter" ) ||
                      it.current()->inherits( "QtxWorkstackArea" ) ) )
        moveList.append( (QWidget*)it.current() );
      if ( it.current() == after )
        found = true;
    }
  }

  QMap<QWidget*, bool> map;
  for ( QWidgetListIt it( moveList ); it.current(); ++it )
  {
    map.insert( it.current(), it.current()->isVisibleTo( it.current()->parentWidget() ) );
    it.current()->reparent( 0, QPoint( 0, 0 ), false );
  }

  wid->reparent( pWid, QPoint( 0, 0 ), true );

  for ( QWidgetListIt itr( moveList ); itr.current(); ++itr )
    itr.current()->reparent( pWid, QPoint( 0, 0 ), map.contains( itr.current() ) ? map[itr.current()] : false );
}

void QtxWorkstack::onPopupActivated( int id )
{
  switch ( id )
  {
  case SplitVertical:
    splitVertical();
    break;
  case SplitHorizontal:
    splitHorizontal();
    break;
  case Close:
    if ( activeWindow() )
      activeWindow()->close();
    break;
  }
}

void QtxWorkstack::onDestroyed( QObject* obj )
{
  QtxWorkstackArea* area = (QtxWorkstackArea*)obj;

  if ( area == myArea )
    myArea = 0;

  if ( !myArea )
  {
    QtxWorkstackArea* cur = neighbourArea( area );
    if ( cur )
      cur->setFocus();
  }

  QApplication::postEvent( this, new QCustomEvent( QEvent::User ) );
}

void QtxWorkstack::onWindowActivated( QWidget* wid )
{
  const QObject* obj = sender();
  if ( !obj->inherits( "QtxWorkstackArea" ) )
    return;

  setActiveArea( (QtxWorkstackArea*)obj );
}

void QtxWorkstack::onDeactivated( QtxWorkstackArea* area )
{
  if ( myArea != area )
    return;

  QPtrList<QtxWorkstackArea> lst;
  areas( mySplit, lst, true );

  int idx = lst.find( area );
  if ( idx == -1 )
    return;

  QtxWorkstackArea* newArea = neighbourArea( area );
  if ( newArea )
    newArea->setFocus();

  QApplication::postEvent( this, new QCustomEvent( QEvent::User ) );
}

void QtxWorkstack::onContextMenuRequested( QPoint p )
{
  if ( !activeArea() )
    return;

  QWidgetList lst = activeArea()->widgetList();
  if ( lst.isEmpty() )
    return;

  QPopupMenu* pm = new QPopupMenu();
  connect( pm, SIGNAL( activated( int ) ), this, SLOT( onPopupActivated( int ) ) );

  if ( lst.count() > 1 )
  {
    pm->insertItem( tr( "Split vertically" ), SplitVertical );
    pm->insertItem( tr( "Split horizontally" ), SplitHorizontal );
    pm->insertSeparator();
  }
  pm->insertItem( tr( "Close" ), Close );

  pm->exec( p );

  delete pm;
}

void QtxWorkstack::childEvent( QChildEvent* e )
{
  if ( e->inserted() && e->child()->isWidgetType() )
  {
	  QWidget* w = (QWidget*)e->child();
	  if ( w && w != mySplit )
    {
      targetArea()->insertWidget( w );
      return;
    }
  }
  QWidget::childEvent( e );
}

void QtxWorkstack::customEvent( QCustomEvent* e )
{
  updateState();
}

QSplitter* QtxWorkstack::splitter( QtxWorkstackArea* area ) const
{
  if ( !area )
    return 0;

  QSplitter* split = 0;

  QWidget* wid = area->parentWidget();
  if ( wid && wid->inherits( "QSplitter" ) )
    split = (QSplitter*)wid;

  return split;
}

void QtxWorkstack::splitters( QSplitter* split, QPtrList<QSplitter>& splitList, const bool rec ) const
{
  if ( !split )
    return;

  const QObjectList* objs = split->children();
  if ( objs )
  {
    for ( QObjectListIt it( *objs ); it.current(); ++it )
    {
      if ( rec )
        splitters( (QSplitter*)it.current(), splitList, rec );
      if ( it.current()->inherits( "QSplitter" ) )
        splitList.append( (QSplitter*)it.current() );
    }
  }
}

void QtxWorkstack::areas( QSplitter* split, QPtrList<QtxWorkstackArea>& areaList, const bool rec ) const
{
  if ( !split )
    return;

  const QObjectList* objs = split->children();
  if ( objs )
  {
    for ( QObjectListIt it( *objs ); it.current(); ++it )
    {
      if ( it.current()->inherits( "QtxWorkstackArea" ) )
        areaList.append( (QtxWorkstackArea*)it.current() );
      else if ( rec && it.current()->inherits( "QSplitter" ) )
        areas( (QSplitter*)it.current(), areaList, rec );
    }
  }
}

QtxWorkstackArea* QtxWorkstack::activeArea() const
{
  return myArea;
}

QtxWorkstackArea* QtxWorkstack::targetArea()
{
  QtxWorkstackArea* area = activeArea();
  if ( !area )
    area = currentArea();
  if ( !area )
  {
    QPtrList<QtxWorkstackArea> lst;
    areas( mySplit, lst );
    if ( !lst.isEmpty() )
      area = lst.first();
  }

  if ( !area )
    area = createArea( mySplit );

  return area;
}

QtxWorkstackArea* QtxWorkstack::currentArea() const
{
  QtxWorkstackArea* area = 0;
  QWidget* wid = focusWidget();
  while ( wid && !area )
  {
    if ( wid->inherits( "QtxWorkstackArea" ) )
      area = (QtxWorkstackArea*)wid;
    wid = wid->parentWidget();
  }

  return area;
}

QtxWorkstackArea* QtxWorkstack::createArea( QWidget* parent ) const
{
  QtxWorkstackArea* area = new QtxWorkstackArea( parent );

  connect( area, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
  connect( area, SIGNAL( activated( QWidget* ) ), this, SLOT( onWindowActivated( QWidget* ) ) );
  connect( area, SIGNAL( contextMenuRequested( QPoint ) ), this, SLOT( onContextMenuRequested( QPoint ) ) );
  connect( area, SIGNAL( deactivated( QtxWorkstackArea* ) ), this, SLOT( onDeactivated( QtxWorkstackArea* ) ) );

  return area;
}

void QtxWorkstack::setActiveArea( QtxWorkstackArea* area )
{
  QWidget* oldCur = myWin;

  QtxWorkstackArea* oldArea = myArea;

  myArea = area;

  if ( myArea != oldArea )
  {
    if ( oldArea )
      oldArea->updateActiveState();
    if ( myArea )
      myArea->updateActiveState();
  }

  if ( myArea )
    myWin = myArea->activeWidget();

  if ( myWin && oldCur != myWin )
    emit windowActivated( myWin );
}

QtxWorkstackArea* QtxWorkstack::neighbourArea( QtxWorkstackArea* area ) const
{
  QPtrList<QtxWorkstackArea> lst;
  areas( mySplit, lst, true );
  int pos = lst.find( area );
  if ( pos < 0 )
    return 0;

  QtxWorkstackArea* na = 0;
  for ( int i = pos - 1; i >= 0 && !na; i-- )
  {
    if ( !lst.at( i )->isEmpty() )
      na = lst.at( i );
  }

  for ( int j = pos + 1; j < (int)lst.count() && !na; j++ )
  {
    if ( !lst.at( j )->isEmpty() )
        na = lst.at( j );
  }
  return na;
}

QtxWorkstackArea* QtxWorkstack::areaAt( const QPoint& p ) const
{
  QtxWorkstackArea* area = 0;
  QPtrList<QtxWorkstackArea> lst;
  areas( mySplit, lst, true );
  for ( QPtrListIterator<QtxWorkstackArea> it( lst ); it.current() && !area; ++it )
  {
    QtxWorkstackArea* cur = it.current();
    QRect r = cur->geometry();
    if ( cur->parentWidget() )
      r = QRect( cur->parentWidget()->mapToGlobal( r.topLeft() ), r.size() );
    if ( r.contains( p ) )
      area = cur;
  }
  return area;
}

void QtxWorkstack::updateState()
{
  updateState( mySplit );
}

void QtxWorkstack::updateState( QSplitter* split )
{
  QPtrList<QSplitter> recList;
  splitters( split, recList, false );
  for ( QPtrListIterator<QSplitter> itr( recList ); itr.current(); ++itr )
    updateState( itr.current() );

  QPtrList<QSplitter> splitList;
  splitters( split, splitList, false );

  QPtrList<QtxWorkstackArea> areaList;
  areas( split, areaList, false );

  bool vis = false;
  for ( QPtrListIterator<QtxWorkstackArea> it( areaList ); it.current(); ++it )
  {
    if ( it.current()->isEmpty() )
      it.current()->hide();
    else
    {
      it.current()->show();
      vis = true;
    }
  }

  if ( split == mySplit )
    return;

  for ( QPtrListIterator<QSplitter> iter( splitList ); iter.current() && !vis; ++iter )
    vis = iter.current()->isVisibleTo( iter.current()->parentWidget() );

  if ( areaList.isEmpty() && splitList.isEmpty() )
    delete split;
  else if ( vis )
    split->show();
  else
    split->hide();
}

/*!
    Class: QtxWorkstackArea [Internal]
    Descr:
*/

QtxWorkstackArea::QtxWorkstackArea( QWidget* parent )
: QWidget( parent )
{
  QVBoxLayout* base = new QVBoxLayout( this );

  QHBox* top = new QHBox( this );
  base->addWidget( top );

  myBar = new QtxWorkstackTabBar( top );

  QPushButton* close = new QPushButton( top );
  close->setPixmap( style().stylePixmap( QStyle::SP_TitleBarCloseButton ) );
  close->setAutoDefault( true );
  close->setFlat( true );
  myClose = close;

  top->setStretchFactor( myBar, 1 );

  myStack = new QWidgetStack( this );

  base->addWidget( myStack, 1 );

  connect( myClose, SIGNAL( clicked() ), this, SLOT( onClose() ) );
  connect( myBar, SIGNAL( selected( int ) ), this, SLOT( onSelected( int ) ) );
  connect( myBar, SIGNAL( dragActiveTab() ), this, SLOT( onDragActiveTab() ) );
  connect( myBar, SIGNAL( contextMenuRequested( QPoint ) ), this, SIGNAL( contextMenuRequested( QPoint ) ) );

  updateState();

  updateActiveState();

  qApp->installEventFilter( this );
}

QtxWorkstackArea::~QtxWorkstackArea()
{
  qApp->removeEventFilter( this );
}

bool QtxWorkstackArea::isEmpty() const
{
  bool res = false;
  for ( WidgetInfoMap::ConstIterator it = myInfo.begin(); it != myInfo.end() && !res; ++it )
    res = it.data().vis;
  return !res;
}

void QtxWorkstackArea::insertWidget( QWidget* wid, const int idx )
{
  if ( !wid )
    return;

  int pos = myList.find( wid );
  if ( pos != -1 && ( pos == idx || ( idx < 0 && pos == (int)myList.count() - 1 ) ) )
    return;

  myList.removeRef( wid );
  pos = idx < 0 ? myList.count() : idx;
  myList.insert( QMIN( pos, (int)myList.count() ), wid );
  if ( !myInfo.contains( wid ) )
  {
    QtxWorkstackChild* child = new QtxWorkstackChild( wid, myStack );
    myChild.insert( wid, child );
    myInfo.insert( wid, WidgetInfo() );
    myInfo[wid].id = generateId();
    myInfo[wid].vis = wid->isVisibleTo( wid->parentWidget() );

    connect( child, SIGNAL( destroyed( QObject* ) ), this, SLOT( onChildDestroyed( QObject* ) ) );
    connect( child, SIGNAL( shown( QtxWorkstackChild* ) ), this, SLOT( onChildShown( QtxWorkstackChild* ) ) );
    connect( child, SIGNAL( hided( QtxWorkstackChild* ) ), this, SLOT( onChildHided( QtxWorkstackChild* ) ) );
    connect( child, SIGNAL( activated( QtxWorkstackChild* ) ), this, SLOT( onChildActivated( QtxWorkstackChild* ) ) );
    connect( child, SIGNAL( captionChanged( QtxWorkstackChild* ) ), this, SLOT( onChildCaptionChanged( QtxWorkstackChild* ) ) );
  }

  updateState();

  setWidgetActive( wid );
}

void QtxWorkstackArea::removeWidget( QWidget* wid )
{
  if ( !myList.contains( wid ) )
    return;

  if ( myBar->tab( widgetId( wid ) ) )
    myBar->removeTab( myBar->tab( widgetId( wid ) ) );
  myStack->removeWidget( child( wid ) );

  myList.remove( wid );
  myInfo.remove( wid );
  myChild.remove( wid );

  delete child( wid );

  if ( myList.isEmpty() )
    delete this;
  else
    updateState();
}

QWidgetList QtxWorkstackArea::widgetList() const
{
  QWidgetList lst;
  for ( QWidgetListIt it( myList ); it.current(); ++it )
  {
    if ( widgetVisibility( it.current() ) )
      lst.append( it.current() );
  }
  return lst;
}

QWidget* QtxWorkstackArea::activeWidget() const
{
  return widget( myBar->currentTab() );
}

void QtxWorkstackArea::setActiveWidget( QWidget* wid )
{
  myBar->setCurrentTab( widgetId( wid ) );
}

bool QtxWorkstackArea::contains( QWidget* wid ) const
{
  return myList.contains( wid );
}

void QtxWorkstackArea::show()
{
  QMap<QWidget*, bool> map;
  for ( QWidgetListIt it( myList ); it.current(); ++it )
  {
    map.insert( it.current(), isBlocked( it.current() ) );
    setBlocked( it.current(), true );
  }

  QWidget::show();

  for ( QWidgetListIt itr( myList ); itr.current(); ++itr )
    setBlocked( itr.current(), map.contains( itr.current() ) ? map[itr.current()] : false );
}

void QtxWorkstackArea::hide()
{
  QMap<QWidget*, bool> map;
  for ( QWidgetListIt it( myList ); it.current(); ++it )
  {
    map.insert( it.current(), isBlocked( it.current() ) );
    setBlocked( it.current(), true );
  }

  QWidget::hide();

  for ( QWidgetListIt itr( myList ); itr.current(); ++itr )
    setBlocked( itr.current(), map.contains( itr.current() ) ? map[itr.current()] : false );
}

bool QtxWorkstackArea::isActive() const
{
  QtxWorkstack* ws = workstack();
  if ( !ws )
    return false;

  return ws->activeArea() == this;
}

void QtxWorkstackArea::updateActiveState()
{
  myBar->setActive( isActive() );
}

QtxWorkstack* QtxWorkstackArea::workstack() const
{
  QtxWorkstack* ws = 0;
  QWidget* wid = parentWidget();
  while ( wid && !ws )
  {
    if ( wid->inherits( "QtxWorkstack" ) )
      ws = (QtxWorkstack*)wid;
    wid = wid->parentWidget();
  }
  return ws;
}

bool QtxWorkstackArea::eventFilter( QObject* o, QEvent* e )
{
  if ( o->isWidgetType() )
  {
    QWidget* wid = (QWidget*)o;
    if ( e->type() == QEvent::FocusIn || e->type() == QEvent::MouseButtonPress )
    {
      bool ok = false;
      while ( !ok && wid && wid != myClose )
      {
        ok = wid == this;
        wid = wid->parentWidget();
      }
      if ( ok )
        QApplication::postEvent( this, new QCustomEvent( (QEvent::Type)( e->type() == QEvent::FocusIn ? ActivateWidget : FocusWidget ) ) );
    }
  }
  return false;
}

QRect QtxWorkstackArea::floatRect() const
{
  QRect r = myStack->geometry();
  return QRect( mapToGlobal( r.topLeft() ), mapToGlobal( r.bottomRight() ) );
}

QRect QtxWorkstackArea::floatTab( const int idx ) const
{
  return myBar->tabRect( idx );
}

int QtxWorkstackArea::tabAt( const QPoint& p ) const
{
  int idx = -1;
  for ( int i = 0; i < myBar->count() && idx == -1; i++ )
  {
    QRect r = myBar->tabRect( i );
    if ( r.isValid() && r.contains( p ) )
      idx = i;
  }
  return idx;
}

void QtxWorkstackArea::customEvent( QCustomEvent* e )
{
  switch ( e->type() )
  {
  case ActivateWidget:
    emit activated( activeWidget() );
    break;
  case FocusWidget:
    if ( activeWidget() && !activeWidget()->focusWidget() )
      activeWidget()->setFocus();
    break;
  case RemoveWidget:
    removeWidget( (QWidget*)e->data() );
    break;
  }
}

void QtxWorkstackArea::focusInEvent( QFocusEvent* e )
{
  QWidget::focusInEvent( e );

  emit activated( activeWidget() );
}

void QtxWorkstackArea::mousePressEvent( QMouseEvent* e )
{
  QWidget::mousePressEvent( e );

  emit activated( activeWidget() );
}

void QtxWorkstackArea::onClose()
{
  QWidget* wid = activeWidget();
  if ( wid )
    wid->close();
}

void QtxWorkstackArea::onSelected( int id )
{
  updateCurrent();

  emit activated( activeWidget() );
}

void QtxWorkstackArea::onDragActiveTab()
{
  QtxWorkstackChild* c = child( activeWidget() );
  if ( !c )
    return;

  new QtxWorkstackDrag( workstack(), c );
}

void QtxWorkstackArea::onChildDestroyed( QObject* obj )
{
  QtxWorkstackChild* child = (QtxWorkstackChild*)obj;
  myStack->removeWidget( child );

  QWidget* wid = 0;
  for ( ChildMap::ConstIterator it = myChild.begin(); it != myChild.end() && !wid; ++it )
  {
    if ( it.data() == child )
      wid = it.key();
  }

  myChild.remove( wid );

  QApplication::postEvent( this, new QCustomEvent( (QEvent::Type)RemoveWidget, wid ) );
}

void QtxWorkstackArea::onChildShown( QtxWorkstackChild* c )
{
  setWidgetShown( c->widget(), true );
}

void QtxWorkstackArea::onChildHided( QtxWorkstackChild* c )
{
  setWidgetShown( c->widget(), false );
}

void QtxWorkstackArea::onChildActivated( QtxWorkstackChild* c )
{
  setWidgetActive( c->widget() );
}

void QtxWorkstackArea::onChildCaptionChanged( QtxWorkstackChild* c )
{
  updateTab( c->widget() );
}

void QtxWorkstackArea::updateCurrent()
{
  QMap<QWidget*, bool> map;
  for ( QWidgetListIt it( myList ); it.current(); ++it )
  {
    map.insert( it.current(), isBlocked( it.current() ) );
    setBlocked( it.current(), true );
  }

  myStack->raiseWidget( myBar->currentTab() );

  for ( QWidgetListIt itr( myList ); itr.current(); ++itr )
    setBlocked( itr.current(), map.contains( itr.current() ) ? map[itr.current()] : false );
}

void QtxWorkstackArea::updateTab( QWidget* wid )
{
  QTab* tab = myBar->tab( widgetId( wid ) );
  if ( !tab )
    return;

  QIconSet icoSet;
  if ( wid->icon() )
    icoSet = QIconSet( *wid->icon() );

  tab->setIconSet( icoSet );
  tab->setText( wid->caption() );
}

QWidget* QtxWorkstackArea::widget( const int id ) const
{
  QWidget* wid = 0;
  for ( WidgetInfoMap::ConstIterator it = myInfo.begin(); it != myInfo.end() && !wid; ++it )
  {
    if ( it.data().id == id )
      wid = it.key();
  }
  return wid;
}

int QtxWorkstackArea::widgetId( QWidget* wid ) const
{
  int id = -1;
  if ( myInfo.contains( wid ) )
    id = myInfo[wid].id;
  return id;
}

bool QtxWorkstackArea::widgetVisibility( QWidget* wid ) const
{
  bool res = false;
  if ( myInfo.contains( wid ) )
    res = myInfo[wid].vis;
  return res;
}

void QtxWorkstackArea::setWidgetActive( QWidget* wid )
{
  int id = widgetId( wid );
  if ( id < 0 )
    return;

  myBar->setCurrentTab( id );
}

void QtxWorkstackArea::setWidgetShown( QWidget* wid, const bool on )
{
  if ( isBlocked( wid ) || !myInfo.contains( wid ) || myInfo[wid].vis == on )
    return;

  myInfo[wid].vis = on;
  updateState();
}

void QtxWorkstackArea::updateState()
{
  bool updBar = myBar->isUpdatesEnabled();
  bool updStk = myStack->isUpdatesEnabled();
  myBar->setUpdatesEnabled( false );
  myStack->setUpdatesEnabled( false );

  bool block = myBar->signalsBlocked();
  myBar->blockSignals( true );

  QWidget* prev = activeWidget();

  int idx = 0;
  for ( QWidgetListIt it( myList ); it.current(); ++it )
  {
    QWidget* wid = it.current();
    int id = widgetId( wid );

    if ( id < 0 )
      continue;

    bool vis = widgetVisibility( wid );

    if ( myBar->tab( id ) && ( !vis || myBar->indexOf( id ) != idx ) )
      myBar->removeTab( myBar->tab( id ) );

    if ( !myBar->tab( id ) && vis )
    {
      QTab* tab = new QTab( wid->caption() );
      myBar->insertTab( tab, idx );
      tab->setIdentifier( id );
    }

    updateTab( wid );

    bool block = isBlocked( wid );
    setBlocked( wid, true );

    QtxWorkstackChild* cont = child( wid );

    if ( !vis )
      myStack->removeWidget( cont );
    else if ( !myStack->widget( id ) )
      myStack->addWidget( cont, id );

    if ( vis )
      idx++;

    setBlocked( wid, block );
  }

  int curId = widgetId( prev );
  if ( !myBar->tab( curId ) )
  {
    QWidget* wid = 0;
    int pos = myList.find( prev );
    for ( int i = pos - 1; i >= 0 && !wid; i-- )
    {
      if ( widgetVisibility( myList.at( i ) ) )
        wid = myList.at( i );
    }

    for ( int j = pos + 1; j < (int)myList.count() && !wid; j++ )
    {
      if ( widgetVisibility( myList.at( j ) ) )
        wid = myList.at( j );
    }

    if ( wid )
      curId = widgetId( wid );
  }

  myBar->setCurrentTab( curId );

  myBar->blockSignals( block );

  updateCurrent();

  myBar->setUpdatesEnabled( updBar );
  myStack->setUpdatesEnabled( updStk );
  if ( updBar )
    myBar->update();
  if ( updStk )
    myStack->update();

  QResizeEvent re( myBar->size(), myBar->size() );
  QApplication::sendEvent( myBar, &re );

  if ( isEmpty() )
  {
    hide();
    emit deactivated( this );
  }
  else
  {
    show();
    if ( prev != activeWidget() )
      emit activated( activeWidget() );
  }
}

int QtxWorkstackArea::generateId() const
{
  QMap<int, int> map;

  for ( WidgetInfoMap::ConstIterator it = myInfo.begin(); it != myInfo.end(); ++it )
    map.insert( it.data().id, 0 );

  int id = 0;
  while ( map.contains( id ) )
    id++;

  return id;
}

bool QtxWorkstackArea::isBlocked( QWidget* wid ) const
{
  return myBlock.contains( wid );
}

void QtxWorkstackArea::setBlocked( QWidget* wid, const bool on )
{
  if ( on )
    myBlock.insert( wid, 0 );
  else
    myBlock.remove( wid );
}

QtxWorkstackChild* QtxWorkstackArea::child( QWidget* wid ) const
{
  QtxWorkstackChild* res = 0;
  if ( myChild.contains( wid ) )
    res = myChild[wid];
  return res;
}

/*!
    Class: QtxWorkstackChild [Internal]
    Descr:
*/

QtxWorkstackChild::QtxWorkstackChild( QWidget* wid, QWidget* parent )
: QHBox( parent ),
myWidget( wid )
{
  myWidget->reparent( this, QPoint( 0, 0 ), myWidget->isVisibleTo( myWidget->parentWidget() ) );
  myWidget->installEventFilter( this );

  connect( myWidget, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
}

QtxWorkstackChild::~QtxWorkstackChild()
{
  qApp->removeEventFilter( this );

  if ( !widget() )
    return;

  widget()->removeEventFilter( this );
  widget()->reparent( 0, QPoint( 0, 0 ), false );
  disconnect( widget(), SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
}

QWidget* QtxWorkstackChild::widget() const
{
  return myWidget;
}

bool QtxWorkstackChild::eventFilter( QObject* o, QEvent* e )
{
  if ( o->isWidgetType() )
  {
    if ( e->type() == QEvent::CaptionChange || e->type() == QEvent::IconChange )
      emit captionChanged( this );

    if ( !e->spontaneous() && ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ) )
      emit shown( this );

    if ( !e->spontaneous() && ( e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) )
      emit hided( this );

    if ( e->type() == QEvent::FocusIn )
      emit activated( this );
  }
  return QHBox::eventFilter( o, e );
}

void QtxWorkstackChild::onDestroyed( QObject* obj )
{
  if ( obj != widget() )
    return;

  myWidget = 0;
  deleteLater();
}

void QtxWorkstackChild::childEvent( QChildEvent* e )
{
  if ( e->type() == QEvent::ChildRemoved && e->child() == widget() )
  {
    myWidget = 0;
    deleteLater();
  }
  QHBox::childEvent( e );
}

/*!
    Class: QtxWorkstackTabBar [Internal]
    Descr:
*/

QtxWorkstackTabBar::QtxWorkstackTabBar( QWidget* parent )
: QTabBar( parent ),
myId( -1 )
{
}

QtxWorkstackTabBar::~QtxWorkstackTabBar()
{
}

void QtxWorkstackTabBar::setActive( const bool on )
{
  QFont aFont = font();
  aFont.setUnderline( on );
  setFont( aFont );

  update();
}

QRect QtxWorkstackTabBar::tabRect( const int idx ) const
{
  QRect r;
  QTab* t = tabAt( idx );
  if ( t )
  {
    r = t->rect();
    r.setLeft( QMAX( r.left(), 0 ) );

    int x1 = tabAt( 0 )->rect().left();
    int x2 = tabAt( count() - 1 )->rect().right();

    int bw = 0;
    if ( QABS( x2 - x1 ) > width() )
#if defined QT_VERSION && QT_VERSION >= 0x30300
      bw = 2 * style().pixelMetric( QStyle::PM_TabBarScrollButtonWidth, this );
#else
      bw = 2 * 16;
#endif

    int limit = width() - bw;
    r.setRight( QMIN( r.right(), limit ) );

    r = QRect( mapToGlobal( r.topLeft() ), r.size() );
  }
  return r;
}

void QtxWorkstackTabBar::mouseMoveEvent( QMouseEvent* e )
{
  if ( myId != -1 && !tab( myId )->rect().contains( e->pos() ) )
  {
    myId = -1;
    emit dragActiveTab();
  }

  QTabBar::mouseMoveEvent( e );
}

void QtxWorkstackTabBar::mousePressEvent( QMouseEvent* e )
{
  QTabBar::mousePressEvent( e );

  if ( e->button() == LeftButton )
    myId = currentTab();
}

void QtxWorkstackTabBar::mouseReleaseEvent( QMouseEvent* e )
{
  QTabBar::mouseReleaseEvent( e );

  myId = -1;

  if ( e->button() == RightButton )
    emit contextMenuRequested( e->globalPos() );
}

void QtxWorkstackTabBar::contextMenuEvent( QContextMenuEvent* e )
{
  if ( e->reason() != QContextMenuEvent::Mouse )
    emit contextMenuRequested( e->globalPos() );
}

void QtxWorkstackTabBar::paintLabel( QPainter* p, const QRect& br, QTab* t, bool has_focus ) const
{
  if ( currentTab() != t->identifier() )
  {
    QFont fnt = p->font();
    fnt.setUnderline( false );
    p->setFont( fnt );
  }
  QTabBar::paintLabel( p, br, t, has_focus );
}

/*!
    Class: QtxWorkstackDrag [Internal]
    Descr:
*/

QtxWorkstackDrag::QtxWorkstackDrag( QtxWorkstack* ws, QtxWorkstackChild* child )
: QObject( 0 ),
myWS( ws ),
myTab( -1 ),
myArea( 0 ),
myPainter( 0 ),
myChild( child )
{
  qApp->installEventFilter( this );
}

QtxWorkstackDrag::~QtxWorkstackDrag()
{
  qApp->removeEventFilter( this );

  endDrawRect();
}

bool QtxWorkstackDrag::eventFilter( QObject*, QEvent* e )
{
  switch ( e->type() )
  {
  case QEvent::MouseMove:
    updateTarget( ((QMouseEvent*)e)->globalPos() );
    break;
  case QEvent::MouseButtonRelease:
    drawRect();
    endDrawRect();
    dropWidget();
    deleteLater();
    break;
  default:
    return false;
  }
  return true;
}

void QtxWorkstackDrag::updateTarget( const QPoint& p )
{
  int tab = -1;
  QtxWorkstackArea* area = detectTarget( p, tab );
  setTarget( area, tab );
}

QtxWorkstackArea* QtxWorkstackDrag::detectTarget( const QPoint& p, int& tab ) const
{
  if ( p.isNull() )
    return 0;

  QtxWorkstackArea* area = myWS->areaAt( p );
  if ( area )
    tab = area->tabAt( p );
  return area;
}

void QtxWorkstackDrag::setTarget( QtxWorkstackArea* area, const int tab )
{
  if ( !area || ( myArea == area && tab == myTab ) )
    return;

  startDrawRect();

  if ( myArea )
    drawRect();

  myTab = tab;
  myArea = area;

  if ( myArea )
    drawRect();
}

void QtxWorkstackDrag::dropWidget()
{
  if ( myArea )
    myArea->insertWidget( myChild->widget(), myTab );
}

void QtxWorkstackDrag::drawRect()
{
  if ( !myPainter || !myArea )
    return;

  QRect r = myArea->floatRect();
  int m = myPainter->pen().width();

  r.setTop( r.top() + m + 2 );
  r.setLeft( r.left() + m + 2 );
  r.setRight( r.right() - m - 2 );
  r.setBottom( r.bottom() - m - 2 );

  myPainter->drawRect( r );

  QRect tr = myArea->floatTab( myTab );
  tr.setTop( tr.top() + m );
  tr.setLeft( tr.left() + m );
  tr.setRight( tr.right() - m );
  tr.setBottom( tr.bottom() - m );

  myPainter->drawRect( tr );
}

void QtxWorkstackDrag::endDrawRect()
{
  delete myPainter;
  myPainter = 0;
}

void QtxWorkstackDrag::startDrawRect()
{
  if ( myPainter )
    return;

  int scr = QApplication::desktop()->screenNumber( (QWidget*)this );
  QWidget* paint_on = QApplication::desktop()->screen( scr );

  myPainter = new QPainter( paint_on, true );
  myPainter->setPen( QPen( gray, 3 ) );
  myPainter->setRasterOp( XorROP );
}
