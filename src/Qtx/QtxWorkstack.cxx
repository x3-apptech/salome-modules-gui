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

// File:      QtxWorkstack.cxx
// Author:    Sergey TELKOV
//
#include "QtxWorkstack.h"

#include "QtxAction.h"

#include <QMenu>
#include <QStyle>
#include <QRegExp>
#include <QLayout>
#include <QPainter>
#include <QDataStream>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QRubberBand>
#include <QApplication>
#include <QStyleOption>
#include <QInputDialog>
#include <QStackedWidget>
#include <QAbstractButton>

/*!
  \class QtxWorkstackArea::WidgetEvent
  \internal
  \brief Internal class used to forward child widgets events to the workarea
*/

class QtxWorkstackArea::WidgetEvent : public QEvent
{
public:
  WidgetEvent( Type t, QtxWorkstackChild* w = 0 ) : QEvent( t ), myChild( w ) {};

  QtxWorkstackChild* child() const { return myChild; }

private:
  QtxWorkstackChild* myChild;   // event receiver widget
};

/*!
  \class QtxWorkstackArea::RestoreEvent
  \internal
  \brief Internal class used to forward restore info events to the workarea
*/

class QtxWorkstackArea::RestoreEvent : public QtxWorkstackArea::WidgetEvent
{
public:
  RestoreEvent( Type t, int id, int f, QtxWorkstackChild* w )
  : WidgetEvent( t, w ), myId( id ), myFlags( f ) {};

  int                id() const { return myId; }
  int                flags() const { return myFlags; }

private:
  int                myId;
  int                myFlags;
};

/*!
  \class QtxWorkstackDrag
  \internal
  \brief Workstack drag object
*/

/*!
  \brief Constructor.
  \param ws parent workstack
  \param child child widget container
*/
QtxWorkstackDrag::QtxWorkstackDrag( QtxWorkstack* ws, QtxWorkstackChild* child )
: QObject( 0 ),
  myWS( ws ),
  myChild( child ),
  myTab( -1 ),
  myArea( 0 ),
  myTabRect( 0 ),
  myAreaRect( 0 )
{
  QApplication::instance()->installEventFilter( this );
}

/*!
  \brief Destructor.
*/
QtxWorkstackDrag::~QtxWorkstackDrag()
{
  QApplication::instance()->removeEventFilter( this );

  endDrawRect();
}

/*!
  \brief Custom event filter.
  \param o event receiver widget
  \param e event
  \return \c true if event should be filtered (stop further processing)
*/
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

/*!
  \brief Detect and set dropping target widget.
  \param p current dragging position
*/
void QtxWorkstackDrag::updateTarget( const QPoint& p )
{
  int tab = -1;
  QtxWorkstackArea* area = detectTarget( p, tab );
  setTarget( area, tab );
}

/*!
  \brief Detect dropping target.
  \param p current dragging position
  \param tab resulting target tab page index
  \return target workarea or 0 if there is no correct drop target
*/
QtxWorkstackArea* QtxWorkstackDrag::detectTarget( const QPoint& p, int& tab ) const
{
  if ( p.isNull() )
    return 0;

  QtxWorkstackArea* area = myWS->areaAt( p );
  if ( area )
    tab = area->tabAt( p );
  return area;
}

/*!
  \brief Set dropping target.
  \param area new target workarea
  \param tab target workarea's tab page index
*/
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

/*!
  \brief Called when drop operation is finished.

  Inserts dropped widget to the target workarea.
*/
void QtxWorkstackDrag::dropWidget()
{
  if ( myArea )
    myArea->insertChild( myChild, myTab );
}

/*!
  \brief Draw floating rectangle.
*/
void QtxWorkstackDrag::drawRect()
{
  if ( !myArea )
    return;

  QRect r = myArea->floatRect();
  int m = 2;

  r.setTop( r.top() + m + 2 );
  r.setLeft( r.left() + m + 2 );
  r.setRight( r.right() - m - 2 );
  r.setBottom( r.bottom() - m - 2 );

  if ( myAreaRect )
  {
    myAreaRect->setGeometry( r );
    myAreaRect->setVisible( r.isValid() );
  }

  QRect tr = myArea->floatTab( myTab );

  tr.setTop( tr.top() + m );
  tr.setLeft( tr.left() + m );
  tr.setRight( tr.right() - m );
  tr.setBottom( tr.bottom() - m );

  if ( myTabRect )
  {
    myTabRect->setGeometry( tr );
    myTabRect->setVisible( tr.isValid() );
  }
}

/*!
  \brief Delete rubber band on the end on the dragging operation.
*/
void QtxWorkstackDrag::endDrawRect()
{
  delete myAreaRect;
  myAreaRect = 0;

  delete myTabRect;
  myTabRect = 0;
}

/*!
  \brief Create rubber band to be drawn on the dragging operation.
*/
void QtxWorkstackDrag::startDrawRect()
{
  if ( !myTabRect )
    myTabRect = new QRubberBand( QRubberBand::Rectangle );

  myTabRect->hide();

  if ( !myAreaRect )
    myAreaRect = new QRubberBand( QRubberBand::Rectangle );

  myAreaRect->hide();
}


/*
  \class CloseButton
  \brief Workstack area close button.
  \internal
*/
class CloseButton : public QAbstractButton
{
public:
  CloseButton( QWidget* );

  QSize        sizeHint() const;
  QSize        minimumSizeHint() const;

  void enterEvent( QEvent* );
  void leaveEvent( QEvent* );
  void paintEvent( QPaintEvent* );
};

/*!
  \brief Constructor
  \internal
  \param parent parent widget
*/
CloseButton::CloseButton( QWidget* parent )
: QAbstractButton( parent )
{
 setFocusPolicy( Qt::NoFocus );
}

/*!
  \brief Get appropriate size for the button.
  \internal
  \return size value
*/
QSize CloseButton::sizeHint() const
{
  ensurePolished();
  int dim = 0;
  if( !icon().isNull() )
  {
    const QPixmap pm = icon().pixmap( style()->pixelMetric( QStyle::PM_SmallIconSize ),
                                      QIcon::Normal );
    dim = qMax( pm.width(), pm.height() );
  }
  return QSize( dim + 4, dim + 4 );
}

/*!
  \brief Get minimum appropriate size for the button.
  \internal
  \return minimum size value
*/
QSize CloseButton::minimumSizeHint() const
{
  return sizeHint();
}

/*!
  \brief Process mouse enter event.
  \internal
  \param event mouse enter event
*/
void CloseButton::enterEvent( QEvent *event )
{
  if ( isEnabled() )
    update();
  QAbstractButton::enterEvent( event );
}

/*!
  \brief Process mouse leave event.
  \internal
  \param event mouse leave event
*/
void CloseButton::leaveEvent( QEvent *event )
{
  if( isEnabled() )
    update();
  QAbstractButton::leaveEvent( event );
}

/*!
  \brief Process paint event.
  \internal
  \param event paint event
*/
void CloseButton::paintEvent( QPaintEvent* )
{
  QPainter p( this );

  QRect r = rect();
  QStyleOption opt;
  opt.init( this );
  opt.state |= QStyle::State_AutoRaise;
  if ( isEnabled() && underMouse() && !isChecked() && !isDown() )
    opt.state |= QStyle::State_Raised;
  if ( isChecked() )
    opt.state |= QStyle::State_On;
  if ( isDown() )
    opt.state |= QStyle::State_Sunken;
  style()->drawPrimitive( QStyle::PE_PanelButtonTool, &opt, &p, this );

  int shiftHorizontal = opt.state & QStyle::State_Sunken ? style()->pixelMetric( QStyle::PM_ButtonShiftHorizontal, &opt, this ) : 0;
  int shiftVertical = opt.state & QStyle::State_Sunken ? style()->pixelMetric( QStyle::PM_ButtonShiftVertical, &opt, this ) : 0;

  r.adjust( 2, 2, -2, -2 );
  r.translate( shiftHorizontal, shiftVertical );

  QPixmap pm = icon().pixmap( style()->pixelMetric( QStyle::PM_SmallIconSize ), isEnabled() ?
                              underMouse() ? QIcon::Active : QIcon::Normal
                              : QIcon::Disabled,
                              isDown() ? QIcon::On : QIcon::Off );
  style()->drawItemPixmap( &p, r, Qt::AlignCenter, pm );
}


/*!
  \class QtxWorkstackSplitter
  \internal
  \brief Workstack splitter.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxWorkstackSplitter::QtxWorkstackSplitter( QWidget* parent )
: QSplitter( parent )
{
  setChildrenCollapsible( false );
}

/*!
  \brief Destructor.
*/
QtxWorkstackSplitter::~QtxWorkstackSplitter()
{
}

/*!
  \brief Get parent workstack
  \return workstack owning this workarea
*/
QtxWorkstack* QtxWorkstackSplitter::workstack() const
{
  QtxWorkstack* ws = 0;
  QWidget* wid = parentWidget();
  while ( wid && !ws )
  {
    ws = ::qobject_cast<QtxWorkstack*>( wid );
    wid = wid->parentWidget();
  }
  return ws;
}

/*!
  \brief Save the widget area configuration into data stream.
*/
void QtxWorkstackSplitter::saveState( QDataStream& stream ) const
{
  stream << QtxWorkstack::SplitMarker;

  uchar flags = 0;
  if ( orientation() == Qt::Horizontal )
    flags |= QtxWorkstack::Horizontal;

  stream << flags;
  stream << count();

  QList<int> sz = sizes();
  for ( QList<int>::const_iterator it = sz.begin(); it != sz.end(); ++it )
    stream << *it;

  for ( int i = 0; i < count(); i++ )
  {
    QWidget* wid = widget( i );
    QtxWorkstackSplitter* split = ::qobject_cast<QtxWorkstackSplitter*>( wid );
    if ( split )
      split->saveState( stream );
    else
    {
      QtxWorkstackArea* area = ::qobject_cast<QtxWorkstackArea*>( wid );
      if ( area )
              area->saveState( stream );
    }
  }
}

/*!
  \brief Restore the widget area configuration from data stream info.
  \return \c true in successful case.
*/
bool QtxWorkstackSplitter::restoreState( QDataStream& stream, QMap<QString, QtxWorkstackChild*>& map )
{
  int num = 0;
  uchar flags = 0;

  stream >> flags;
  stream >> num;

  setOrientation( flags & QtxWorkstack::Horizontal ? Qt::Horizontal : Qt::Vertical );

  QList<int> sz;
  for ( int s = 0; s < num; s++ )
  {
    int sn = 0;
    stream >> sn;
    sz.append( sn );
  }

  bool ok = true;
  for ( int i = 0; i < num && ok; i++ )
  {
    int marker;
    stream >> marker;

    if ( stream.status() != QDataStream::Ok )
      return false;

    if ( marker == QtxWorkstack::SplitMarker )
    {
      QtxWorkstackSplitter* split = new QtxWorkstackSplitter( this );
      addWidget( split );
      split->setVisible( true );

      ok = split->restoreState( stream, map );
    }
    else if ( marker == QtxWorkstack::AreaMarker )
    {
      QtxWorkstack* ws = workstack();
      QtxWorkstackArea* area = ws->createArea( this );
      addWidget( area );
      area->setVisible( true );

      ok = area->restoreState( stream, map );
    }
    else
      return false;
  }

  if ( ok )
    setSizes( sz );

  return ok;
}

/*!
  \class QtxWorkstackArea
  \internal
  \brief Workstack widget workarea.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxWorkstackArea::QtxWorkstackArea( QWidget* parent )
: QFrame( parent )
{
  setFrameStyle( QFrame::Panel | QFrame::Sunken );

  QVBoxLayout* base = new QVBoxLayout( this );
  base->setMargin( frameWidth() );
  base->setSpacing( 0 );

  myTop = new QWidget( this );
  base->addWidget( myTop );

  QHBoxLayout* tl = new QHBoxLayout( myTop );
  tl->setMargin( 0 );

  myBar = new QtxWorkstackTabBar( myTop );
  tl->addWidget( myBar, 1 );

  CloseButton* close = new CloseButton( myTop );
  close->setIcon( style()->standardIcon( QStyle::SP_TitleBarCloseButton ) );
  myClose = close;
  tl->addWidget( myClose );

  myStack = new QStackedWidget( this );

  base->addWidget( myStack, 1 );

  connect( myClose, SIGNAL( clicked() ), this, SLOT( onClose() ) );
  connect( myBar, SIGNAL( currentChanged( int ) ), this, SLOT( onCurrentChanged( int ) ) );
  connect( myBar, SIGNAL( dragActiveTab() ), this, SLOT( onDragActiveTab() ) );
  connect( myBar, SIGNAL( contextMenuRequested( QPoint ) ), this, SLOT( onContextMenuRequested( QPoint ) ) );

  updateState();

  updateActiveState();

  QApplication::instance()->installEventFilter( this );
}

/*!
  \brief Destructor.
*/
QtxWorkstackArea::~QtxWorkstackArea()
{
  QApplication::instance()->removeEventFilter( this );
}

/*!
  \brief Check if workarea contains any widgets.
  \return \c true if area is null (havn't any child widgets)
*/
bool QtxWorkstackArea::isNull() const
{
  return myList.isEmpty();
}

/*!
  \brief Check if workarea contains visible widgets.
  \return \c true if area is empty (all child widgets are removed or now shown)
*/
bool QtxWorkstackArea::isEmpty() const
{
  bool res = false;
  for ( ChildList::const_iterator it = myList.begin(); it != myList.end() && !res; ++it )
    res = (*it)->visibility();
  return !res;
}

/*!
  \brief Add widget to the workarea.
  \param wid widget to be added
  \param idx position in the area widget to be added to
  \param f widget flags
  \return child widget container object (or 0 if index is invalid)
*/
QtxWorkstackChild* QtxWorkstackArea::insertWidget( QWidget* wid, const int idx, Qt::WindowFlags f )
{
  if ( !wid )
    return 0;

  QtxWorkstackChild* c = child( wid );
  if ( !c )
    c = new QtxWorkstackChild( wid, myStack, f );

  insertChild( c, idx );

  return c;
}

void QtxWorkstackArea::insertChild( QtxWorkstackChild* child, const int idx )
{
  if ( !child )
    return;

  QtxWorkstackArea* a = child->area();
  if ( a && a != this )
    a->removeChild( child, false );

  int pos = myList.indexOf( child );
  if ( pos != -1 && ( pos == idx || ( idx < 0 && pos == (int)myList.count() - 1 ) ) )
    return;

  bool found = myList.contains( child );

  myList.removeAll( child );
  pos = idx < 0 ? myList.count() : idx;
  myList.insert( qMin( pos, (int)myList.count() ), child );

  if ( !found )
  {
    bool hasId = false;
    for ( ChildList::const_iterator it = myList.begin(); it != myList.end() && !hasId; ++it )
      hasId = (*it)->id() == child->id();

    if ( hasId || child->id() < 0 )
      child->setId( generateId() );

    connect( child, SIGNAL( destroyed( QObject* ) ), this, SLOT( onChildDestroyed( QObject* ) ) );
    connect( child, SIGNAL( shown( QtxWorkstackChild* ) ), this, SLOT( onChildShown( QtxWorkstackChild* ) ) );
    connect( child, SIGNAL( hidden( QtxWorkstackChild* ) ), this, SLOT( onChildHidden( QtxWorkstackChild* ) ) );
    connect( child, SIGNAL( activated( QtxWorkstackChild* ) ), this, SLOT( onChildActivated( QtxWorkstackChild* ) ) );
    connect( child, SIGNAL( captionChanged( QtxWorkstackChild* ) ), this, SLOT( onChildCaptionChanged( QtxWorkstackChild* ) ) );
  }

  updateState();

  setWidgetActive( child->widget() );
  child->widget()->setFocus();
}

/*!
  \brief Create and show popup menu for the area.
  \param p mouse pointer position at which popup menu should be shown
*/
void QtxWorkstackArea::onContextMenuRequested( QPoint p )
{
  const QtxWorkstackTabBar* bar = ::qobject_cast<const QtxWorkstackTabBar*>( sender() );
  if ( !bar )
    return;

  QWidget* wid = 0;
  int idx = tabAt( p );
  if ( idx != -1 )
    wid = widget( myBar->tabId( idx ) );

  emit contextMenuRequested( wid, p );
}

/*!
  \brief Remove widget from workarea.
  \param wid widget to be removed
  \param del if \c true the widget should be also deleted
*/
void QtxWorkstackArea::removeWidget( QWidget* wid, const bool del )
{
  removeChild( child( wid ), del );
}

/*!
  \brief Remove child from workarea.
  \param child child to be removed
  \param del if \c true the widget should be also deleted
*/
void QtxWorkstackArea::removeChild( QtxWorkstackChild* child, const bool del )
{
  if ( !myList.contains( child ) )
    return;

  myStack->removeWidget( child );

  if ( myBar->indexOf( child->id() ) != -1 )
    myBar->removeTab( myBar->indexOf( child->id() ) );

  myList.removeAll( child );

  if ( del )
    delete child;
  else if ( child->widget() )
  {
    disconnect( child, SIGNAL( destroyed( QObject* ) ), this, SLOT( onChildDestroyed( QObject* ) ) );
    disconnect( child, SIGNAL( shown( QtxWorkstackChild* ) ), this, SLOT( onChildShown( QtxWorkstackChild* ) ) );
    disconnect( child, SIGNAL( hidden( QtxWorkstackChild* ) ), this, SLOT( onChildHidden( QtxWorkstackChild* ) ) );
    disconnect( child, SIGNAL( activated( QtxWorkstackChild* ) ), this, SLOT( onChildActivated( QtxWorkstackChild* ) ) );
    disconnect( child, SIGNAL( captionChanged( QtxWorkstackChild* ) ), this, SLOT( onChildCaptionChanged( QtxWorkstackChild* ) ) );
  }

  if ( isNull() )
    deleteLater();
  else
    updateState();
}

QList<QtxWorkstackChild*> QtxWorkstackArea::childList() const
{
  return myList;
}

/*!
  \brief Get all visible child widgets.
  \return list of visible child widgets
*/
QWidgetList QtxWorkstackArea::widgetList() const
{
  QWidgetList lst;
  for ( ChildList::const_iterator it = myList.begin(); it != myList.end(); ++it )
  {
    QtxWorkstackChild* c = *it;
    if ( c->visibility() )
      lst.append( c->widget() );
  }
  return lst;
}

/*!
  \brief Get active child widget.
  \return active widget
*/
QWidget* QtxWorkstackArea::activeWidget() const
{
  return widget( myBar->tabId( myBar->currentIndex() ) );
}

/*!
  \brief Set active widget.
  \param wid widget to be made active
*/
void QtxWorkstackArea::setActiveWidget( QWidget* wid )
{
  myBar->setCurrentIndex( myBar->indexOf( widgetId( wid ) ) );
}

/*!
  \brief Check if area owns the specified widget.
  \param wid widget to be checked
  \return \c true if area contains widget
*/
bool QtxWorkstackArea::contains( QWidget* wid ) const
{
  return child( wid );
}

/*!
  \brief Check if workarea is active.
  \return \c true if area is active
*/
bool QtxWorkstackArea::isActive() const
{
  QtxWorkstack* ws = workstack();
  if ( !ws )
    return false;

  return ws->activeArea() == this;
}

/*!
  \brief Update active tab bar state (active/inactive).
*/
void QtxWorkstackArea::updateActiveState()
{
  myBar->setActive( isActive() );
}

/*!
  \brief Get parent workstack
  \return workstack owning this workarea
*/
QtxWorkstack* QtxWorkstackArea::workstack() const
{
  QtxWorkstack* ws = 0;
  QWidget* wid = parentWidget();
  while ( wid && !ws )
  {
    ws = ::qobject_cast<QtxWorkstack*>( wid );
    wid = wid->parentWidget();
  }
  return ws;
}

/*!
  \brief Custom event filter.

  Process events from child widgets.

  \param o event receiver widget
  \param e event
  \return \c true if event should be filtered (stop further processing)
*/
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
        QApplication::postEvent( this, new WidgetEvent( (QEvent::Type)( e->type() == QEvent::FocusIn ? ActivateWidget : FocusWidget ) ) );
    }
  }
  return false;
}

/*!
  \brief Save the own widgets configuration into data stream.
*/
void QtxWorkstackArea::saveState( QDataStream& stream ) const
{
  stream << QtxWorkstack::AreaMarker;
  stream << myList.count();
  stream << myBar->tabId( myBar->currentIndex() );
  for ( ChildList::const_iterator it = myList.begin(); it != myList.end(); ++it )
  {
    QtxWorkstackChild* c = *it;

    stream << QtxWorkstack::WidgetMarker;

    stream << c->widget()->objectName();
    stream << c->id();

    uchar flags = 0;
    if ( c->visibility() )
      flags |= QtxWorkstack::Visible;

    stream << flags;
  }
}

/*!
  \brief Restore the widgets configuration from data stream info.
  \return \c true in successful case.
*/
bool QtxWorkstackArea::restoreState( QDataStream& stream, QMap<QString, QtxWorkstackChild*>& map )
{
  int num = 0;
  int cur = -1;

  stream >> num;
  stream >> cur;

  QtxWorkstackChild* curChild = 0;
  for ( int i = 0; i < num; i++ )
  {
    int marker;
    stream >> marker;

    if ( stream.status() != QDataStream::Ok || marker != QtxWorkstack::WidgetMarker )
      return false;

    QString name;
    stream >> name;

    int id = -1;
    stream >> id;

    uchar flags = 0;
    stream >> flags;

    QtxWorkstackChild* c = map.contains( name ) ? map[name] : 0;
    if ( !c )
    {
      qWarning( "QtxWorkstack: Restored child widget \"%s\" not found.", (const char*)name.toLatin1() );
      return false;
    }

    map.remove( name );

    if ( id == cur )
      curChild = c;

    QApplication::postEvent( this, new RestoreEvent( (QEvent::Type)RestoreWidget, id, flags, c ) );
  }

  if ( curChild )
    QApplication::postEvent( this, new WidgetEvent( (QEvent::Type)MakeCurrent, curChild ) );

  return true;
}

/*!
  \brief Show/Hide tab bar.
*/
void QtxWorkstackArea::showTabBar( bool visible)
{
  myTop->setVisible(visible);
  myBar->setVisible(visible);
}


/*!
  \brief Get rectangle to be drawn when highlighting drop area.
  \return area drop rectangle
*/
QRect QtxWorkstackArea::floatRect() const
{
  QRect r = myStack->geometry();
  return QRect( mapToGlobal( r.topLeft() ), mapToGlobal( r.bottomRight() ) );
}

/*!
  \brief Get rectangle to be drawn when highlighting drop area on tab bar.
  \param idx tab index
  \return tab bar drop rectrangle
*/
QRect QtxWorkstackArea::floatTab( const int idx ) const
{
  QRect r = myBar->tabRect( idx );
  return QRect( myBar->mapToGlobal( r.topLeft() ), r.size() );
}

/*!
  \brief Get tab index by point.
  \param p point
  \return tab covering point or -1 if there is no tab covering point
*/
int QtxWorkstackArea::tabAt( const QPoint& pnt ) const
{
  int idx = -1;
  QPoint p = myBar->mapFromGlobal( pnt );
  for ( int i = 0; i < myBar->count() && idx == -1; i++ )
  {
    QRect r = myBar->tabRect( i );
    if ( r.isValid() && r.contains( p ) )
      idx = i;
  }
  return idx;
}

/*!
  \brief Event handler for custom events.
  \param e custom event
*/
void QtxWorkstackArea::customEvent( QEvent* e )
{
  WidgetEvent* we = (WidgetEvent*)e;

  switch ( we->type() )
  {
  case ActivateWidget:
    myBar->updateActiveState();
    // IMN 27/03/2015: This workaround caused by the bug INT PAL 0052623: OCC view blinking when
    // using polyline sketcher which is reproduced on Unix systems with qt-4.8.4.
    myStack->setUpdatesEnabled( false );
    updateCurrent();
    myStack->setUpdatesEnabled( true );
    emit activated( activeWidget() );
    break;
  case FocusWidget:
    if ( activeWidget() )
    {
      if ( !activeWidget()->focusWidget() )
        activeWidget()->setFocus();
      else
      {
        if ( activeWidget()->focusWidget()->hasFocus() )
        {
          QFocusEvent in( QEvent::FocusIn );
                QApplication::sendEvent( this, &in );
              }
        else
        {
          activeWidget()->focusWidget()->setFocus();
                myBar->updateActiveState();
              }
      }
    }
    break;
  case MakeCurrent:
    if ( we->child()->widget() )
      setActiveWidget( we->child()->widget() );
    break;
  case RestoreWidget:
    if ( we->child() )
    {
      QtxWorkstackChild* c = we->child();
      RestoreEvent* re = (RestoreEvent*)we;
      if ( c->widget() )
        c->widget()->setVisible( re->flags() & QtxWorkstack::Visible );
      c->setId( re->id() );
      insertChild( c );
    }
    break;
  default:
    break;
  }
}

/*!
  \brief Customize focus in event handler.
  \param e focus in event
*/
void QtxWorkstackArea::focusInEvent( QFocusEvent* e )
{
  QFrame::focusInEvent( e );

  myBar->updateActiveState();

  emit activated( activeWidget() );
}

/*!
  \brief Customize mouse press event handler.
  \param e mouse press event
*/
void QtxWorkstackArea::mousePressEvent( QMouseEvent* e )
{
  QFrame::mousePressEvent( e );

  emit activated( activeWidget() );
}

/*!
  \brief Called when user presses "Close" button.
*/
void QtxWorkstackArea::onClose()
{
  QWidget* wid = activeWidget();
  if ( wid )
    wid->close();
}

/*!
  \brief Called when user selects any tab page.
  \param idx tab page index (not used)
*/
void QtxWorkstackArea::onCurrentChanged( int /*idx*/ )
{
  updateCurrent();

  emit activated( activeWidget() );
}

/*!
  \brief Called when user starts tab page dragging.
*/
void QtxWorkstackArea::onDragActiveTab()
{
  QtxWorkstackChild* c = child( activeWidget() );
  if ( !c )
    return;

  new QtxWorkstackDrag( workstack(), c );
}

/*!
  \brief Called when area's child widget container is destroyed.
  \param obj widget container being destroyed
*/
void QtxWorkstackArea::onChildDestroyed( QObject* obj )
{
  removeChild( (QtxWorkstackChild*)obj, false );
}

/*!
  \brief Called when child widget container is shown.
  \param c child widget container being shown
*/
void QtxWorkstackArea::onChildShown( QtxWorkstackChild* c )
{
  updateState();
}

/*!
  \brief Called when child widget container is hidden.
  \param c child widget container being hidden
*/
void QtxWorkstackArea::onChildHidden( QtxWorkstackChild* c )
{
  updateState();
}

/*!
  \brief Called when child widget container is activated.
  \param c child widget container being activated
*/
void QtxWorkstackArea::onChildActivated( QtxWorkstackChild* c )
{
  setWidgetActive( c->widget() );
}

/*!
  \brief Called when child widget container's title is changed.
  \param c child widget container which title is changed
*/
void QtxWorkstackArea::onChildCaptionChanged( QtxWorkstackChild* c )
{
  updateTab( c->widget() );
}

/*!
  \brief Update current child widget container.

  Raises widget when active tab page is changed.
*/
void QtxWorkstackArea::updateCurrent()
{
  QWidget* cur = child( myBar->tabId( myBar->currentIndex() ) );
  if ( cur )
    myStack->setCurrentWidget( cur );
}

/*!
  \brief Update tab bar.
  \param wid tab page widget
*/
void QtxWorkstackArea::updateTab( QWidget* wid )
{
  int idx = myBar->indexOf( widgetId( wid ) );
  if ( idx < 0 )
    return;

  myBar->setTabIcon( idx, wid->windowIcon() );
  myBar->setTabText( idx, wid->windowTitle() );
}

/*!
  \brief Get child widget by specified identifier.
  \param id widget ID
  \return widget or 0, if identifier in invalid
*/
QWidget* QtxWorkstackArea::widget( const int id ) const
{
  QtxWorkstackChild* c = child( id );

  return c ? c->widget() : 0;
}

/*!
  \brief Get child widget identifier.
  \param wid widget
  \return widget ID or -1 if widget is not found
*/
int QtxWorkstackArea::widgetId( QWidget* wid ) const
{
  QtxWorkstackChild* c = child( wid );

  return c ? c->id() : -1;
}

/*!
  \brief Set active child widget.
  \param wid widget to be set active
*/
void QtxWorkstackArea::setWidgetActive( QWidget* wid )
{
  int id = widgetId( wid );
  if ( id < 0 )
    return;

  myBar->setCurrentIndex( myBar->indexOf( id ) );
}

/*!
  \brief Update internal state.
*/
void QtxWorkstackArea::updateState()
{
  bool updBar = myBar->updatesEnabled();
  bool updStk = myStack->updatesEnabled();
  myBar->setUpdatesEnabled( false );
  myStack->setUpdatesEnabled( false );

  bool block = myBar->signalsBlocked();
  myBar->blockSignals( true );

  QWidget* prev = activeWidget();

  int idx = 0;
  for ( ChildList::iterator it = myList.begin(); it != myList.end(); ++it )
  {
    QtxWorkstackChild* cont = *it;
    QWidget* wid = cont->widget();;
    int id = cont->id();

    if ( id < 0 )
      continue;

    bool vis = cont->visibility();

    int cIdx = myBar->indexOf( id );
    if ( cIdx != -1 && ( !vis || myBar->indexOf( id ) != idx ) )
      myBar->removeTab( cIdx );

    if ( myBar->indexOf( id ) == -1 && vis )
      myBar->setTabId( myBar->insertTab( idx, wid->windowTitle() ), id );

    updateTab( wid );

    if ( !vis )
      myStack->removeWidget( cont );
    else if ( myStack->indexOf( cont ) < 0 )
      myStack->addWidget( cont );

    if ( vis )
      idx++;
  }

  int curId = widgetId( prev );
  if ( myBar->indexOf( curId ) < 0 )
  {
    QtxWorkstackChild* c = 0;
    int pos = myList.indexOf( child( prev ) );
    for ( int i = pos - 1; i >= 0 && !c; i-- )
    {
      if ( myList.at( i )->visibility() )
        c = myList.at( i );
    }

    for ( int j = pos + 1; j < (int)myList.count() && !c; j++ )
    {
      if ( myList.at( j )->visibility() )
        c = myList.at( j );
    }

    if ( c )
      curId = c->id();
  }

  myBar->setCurrentIndex( myBar->indexOf( curId ) );

  myBar->blockSignals( block );

  updateCurrent();

  myBar->updateActiveState();

  myBar->setUpdatesEnabled( updBar );
  myStack->setUpdatesEnabled( updStk );
  if ( updBar )
    myBar->update();
  if ( updStk )
    myStack->update();

  QResizeEvent re( myBar->size(), myBar->size() );
  QApplication::sendEvent( myBar, &re );

  myBar->updateGeometry();

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

/*!
  \brief Generate unique widget identifier.
  \return first non shared widget ID
*/
int QtxWorkstackArea::generateId() const
{
  QMap<int, int> map;

  for ( ChildList::const_iterator it = myList.begin(); it != myList.end(); ++it )
    map.insert( (*it)->id(), 0 );

  int id = 0;
  while ( map.contains( id ) )
    id++;

  return id;
}

/*!
  \brief Get child widget container.
  \param wid child widget
  \return child widget container corresponding to the \a wid
*/
QtxWorkstackChild* QtxWorkstackArea::child( QWidget* wid ) const
{
  QtxWorkstackChild* res = 0;
  for ( ChildList::const_iterator it = myList.begin(); it != myList.end() && !res; ++it )
  {
    if ( (*it)->widget() == wid )
      res = *it;
  }
  return res;
}

QtxWorkstackChild* QtxWorkstackArea::child( const int id ) const
{
  QtxWorkstackChild* c = 0;
  for ( ChildList::const_iterator it = myList.begin(); it != myList.end() && !c; ++it )
  {
    if ( (*it)->id() == id )
      c = *it;
  }
  return c;
}

/*!
  \fn void QtxWorkstackArea::activated( QWidget* w )
  \brief Emitted when child widget is activated.
  \param w child widget being activated
*/

/*!
  \fn void QtxWorkstackArea::contextMenuRequested( QWidget* w, QPoint p )
  \brief Emitted when context popup menu is requested.
  \param w child widget popup menu requested for
  \param p point popup menu to be shown at
*/

/*!
  \fn void QtxWorkstackArea::deactivated( QtxWorkstackArea* wa )
  \brief Emitted when workarea is deactivated.
  \param wa workarea being deactivated
*/

/*!
  \class QtxWorkstackChild
  \internal
  \brief Workarea child widget container.
*/

/*!
  \brief Constructor.
  \param wid child widget
  \param parent parent widget
  \param f widget flags
*/
QtxWorkstackChild::QtxWorkstackChild( QWidget* wid, QWidget* parent, Qt::WindowFlags f )
: QWidget( parent ),
  myId( 0 ),
  myWidget( wid )
{
  if ( myWidget )
  {
    myWidget->setParent( this, f );
    myWidget->installEventFilter( this );
    if ( myWidget->focusProxy() )
      myWidget->focusProxy()->installEventFilter( this );
    myWidget->setVisible( myWidget->isVisibleTo( myWidget->parentWidget() ) );

    QVBoxLayout* base = new QVBoxLayout( this );
    base->setMargin( 0 );
    base->addWidget( myWidget );

    connect( myWidget, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
  }
}

/*!
  \brief Destructor.
*/
QtxWorkstackChild::~QtxWorkstackChild()
{
  QApplication::instance()->removeEventFilter( this );

  if ( !widget() )
    return;

  disconnect( widget(), SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

  widget()->hide();
  widget()->removeEventFilter( this );
  if ( widget()->focusProxy() )
    widget()->focusProxy()->removeEventFilter( this );

  widget()->setParent( 0 );
}

/*!
  \brief Get child widget.
  \return child widget
*/
QWidget* QtxWorkstackChild::widget() const
{
  return myWidget;
}

/*!
  \brief Returns the id.
*/
int QtxWorkstackChild::id() const
{
  return myId;
}

/*!
  \brief Sets the id.
*/
void QtxWorkstackChild::setId( const int id )
{
  myId = id;
}

/*!
  \brief Returns true if this child window should be visible.
*/
bool QtxWorkstackChild::visibility()
{
  return myWidget ? myWidget->isVisibleTo( this ) : false;
}

QtxWorkstackArea* QtxWorkstackChild::area() const
{
  QtxWorkstackArea* a = 0;
  QWidget* w = parentWidget();
  while ( !a && w )
  {
    a = ::qobject_cast<QtxWorkstackArea*>( w );
    w = w->parentWidget();
  }

  return a;
}

/*!
  \brief Custom event filter.

  Process events from child widgets.

  \param o event receiver widget
  \param e event
  \return \c true if event should be filtered (stop further processing)
*/
bool QtxWorkstackChild::eventFilter( QObject* o, QEvent* e )
{
  if ( o->isWidgetType() )
  {
    if ( e->type() == QEvent::WindowTitleChange || e->type() == QEvent::WindowIconChange )
      emit captionChanged( this );

    if ( !e->spontaneous() && e->type() == QEvent::ShowToParent )
      emit shown( this );

    if ( !e->spontaneous() && e->type() == QEvent::HideToParent )
      emit hidden( this );

    if ( e->type() == QEvent::FocusIn )
      emit activated( this );
  }
  return QWidget::eventFilter( o, e );
}

/*!
  \brief Called when child widget is destroyed.
  \param obj child widget being destroyed
*/
void QtxWorkstackChild::onDestroyed( QObject* obj )
{
  deleteLater();
}

/*!
  \brief Customize child event handler.
  \param e child event
*/
void QtxWorkstackChild::childEvent( QChildEvent* e )
{
  if ( e->removed() && e->child() == widget() )
  {
    myWidget = 0;
    deleteLater();
  }
  QWidget::childEvent( e );
}

/*!
  \fn void QtxWorkstackChild::shown( QtxWorkstackChild* w )
  \brief Emitted when child widget is shown.
  \param w child widget container
*/

/*!
  \fn void QtxWorkstackChild::hidden( QtxWorkstackChild* w )
  \brief Emitted when child widget is hidden.
  \param w child widget container
*/

/*!
  \fn void QtxWorkstackChild::activated( QtxWorkstackChild* w )
  \brief Emitted when child widget is activated.
  \param w child widget container
*/

/*!
  \fn void QtxWorkstackChild::captionChanged( QtxWorkstackChild* w )
  \brief Emitted when child widget's title is changed.
  \param w child widget container
*/

/*!
  \class QtxWorkstackTabBar
  \internal
  \brief Workstack tab bar widget
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxWorkstackTabBar::QtxWorkstackTabBar( QWidget* parent )
: QTabBar( parent ),
  myId( -1 ),myActive(false)
{
  setDrawBase( true );
  setElideMode( Qt::ElideNone );

  connect( this, SIGNAL( currentChanged( int ) ), this, SLOT( onCurrentChanged( int ) ) );
}

/*!
  \brief Destructor.
*/
QtxWorkstackTabBar::~QtxWorkstackTabBar()
{
}

/*!
  \brief Get tab page identifier.
  \param index tab page index
  \return tab page ID or -1 if \a index is out of range
*/
int QtxWorkstackTabBar::tabId( const int index ) const
{
  QVariant v = tabData( index );
  if ( !v.canConvert( QVariant::Int ) )
    return -1;
  return v.toInt();
}

/*!
  \brief Set tab page identifier.
  \param index tab page index
  \param id tab page ID
*/
void QtxWorkstackTabBar::setTabId( const int index, const int id )
{
  setTabData( index, id );
}

/*!
  \brief Get tab page index by specified identifier.
  \param id tab page ID
  \return tab page index or -1 if not found
*/
int QtxWorkstackTabBar::indexOf( const int id ) const
{
  int index = -1;
  for ( int i = 0; i < (int)count() && index < 0; i++ )
  {
    if ( tabId( i ) == id )
      index = i;
  }
  return index;
}

/*!
  \brief Check if the tab bar is active.
  \return \c true if tab bar is active
*/
bool QtxWorkstackTabBar::isActive() const
{
  return myActive;
}

/*!
  \brief Set tab bar active/inactive.
  \param on new active state
*/
void QtxWorkstackTabBar::setActive( const bool on )
{
  if ( myActive == on )
    return;

  myActive = on;
  updateActiveState();
}

/*!
  \brief Update tab bar according to the 'active' state.
*/
void QtxWorkstackTabBar::updateActiveState()
{
  QColor bc = palette().color( QPalette::Text );
  QColor ac = isActive() ? palette().color( QPalette::Highlight ) : bc;
  for ( int i = 0; i < (int)count(); i++ )
    setTabTextColor( i, currentIndex() == i ? ac : bc );
}

/*!
  \brief Called when current tab page is changed.
  \param idx tab page index (not used)
*/
void QtxWorkstackTabBar::onCurrentChanged( int /*index*/ )
{
  updateActiveState();
}

/*!
  \brief Customize mouse move event handler.
  \param e mouse event
*/
void QtxWorkstackTabBar::mouseMoveEvent( QMouseEvent* e )
{
  if ( myId != -1 && !tabRect( indexOf( myId ) ).contains( e->pos() ) )
  {
    myId = -1;
    emit dragActiveTab();
  }

  QTabBar::mouseMoveEvent( e );
}

/*!
  \brief Customize mouse press event handler.
  \param e mouse event
*/
void QtxWorkstackTabBar::mousePressEvent( QMouseEvent* e )
{
  QTabBar::mousePressEvent( e );

  if ( e->button() == Qt::LeftButton )
    myId = tabId( currentIndex() );
}

/*!
  \brief Customize mouse release event handler.
  \param e mouse event
*/
void QtxWorkstackTabBar::mouseReleaseEvent( QMouseEvent* e )
{
  QTabBar::mouseReleaseEvent( e );

  myId = -1;

  if ( e->button() == Qt::RightButton )
    emit contextMenuRequested( e->globalPos() );
}

/*!
  \brief Customize context menu event handler.
  \param e context menu event
*/
void QtxWorkstackTabBar::contextMenuEvent( QContextMenuEvent* e )
{
  if ( e->reason() != QContextMenuEvent::Mouse )
    emit contextMenuRequested( e->globalPos() );
}

/*!
  \brief Process widget change state events (style, palette, enable state changing, etc).
  \param e change event (not used)
*/
void QtxWorkstackTabBar::changeEvent( QEvent* /*e*/ )
{
  updateActiveState();
}

/*
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
*/

/*!
  \fn void QtxWorkstackTabBar::dragActiveTab()
  \brief Emitted when dragging operation is started.
*/

/*!
  \fn void QtxWorkstackTabBar::contextMenuRequested( QPoint p )
  \brief Emitted when context popup menu is requested.
  \param p point popup menu to be shown at
*/

/*!
  \class QtxWorkstack
  \brief Workstack widget.

  Organizes the child widgets in the tabbed space.
  Allows splitting the working area to arrange the child widgets in
  arbitrary way. Any widgets can be moved to another working area with
  drag-n-drop operation.

  This widget can be used as workspace of the application main window,
  for example, as kind of implementation of multi-document interface.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxWorkstack::QtxWorkstack( QWidget* parent )
: QWidget( parent ),
  myWin( 0 ),
  myArea( 0 ),
  myWorkWin( 0 ),
  myWorkArea( 0 )
{
  myActionsMap.insert( SplitVertical,   new QtxAction( QString(), tr( "Split vertically" ), 0, this ) );
  myActionsMap.insert( SplitHorizontal, new QtxAction( QString(), tr( "Split horizontally" ), 0, this ) );
  myActionsMap.insert( Close,           new QtxAction( QString(), tr( "Close" ), 0, this ) );
  myActionsMap.insert( Rename,          new QtxAction( QString(), tr( "Rename" ), 0, this ) );

  connect( myActionsMap[SplitVertical], SIGNAL( triggered( bool ) ), this, SLOT( splitVertical() ) );
  connect( myActionsMap[SplitHorizontal], SIGNAL( triggered( bool ) ), this, SLOT( splitHorizontal() ) );
  connect( myActionsMap[Close], SIGNAL( triggered( bool ) ), this, SLOT( onCloseWindow() ) );
  connect( myActionsMap[Rename], SIGNAL( triggered( bool ) ), this, SLOT( onRename() ) );

  // Action shortcut will work when action added in any widget.
  for ( QMap<int, QAction*>::iterator it = myActionsMap.begin(); it != myActionsMap.end(); ++it )
  {
    addAction( it.value() );
    it.value()->setShortcutContext( Qt::ApplicationShortcut );
  }

  QVBoxLayout* base = new QVBoxLayout( this );
  base->setMargin( 0 );

  mySplit = new QtxWorkstackSplitter( this );
  base->addWidget( mySplit );
}

/*!
  \brief Destructor.
*/
QtxWorkstack::~QtxWorkstack()
{
}

/*!
  \brief Get list of all widgets in all areas or in specified area which given
         widget belongs to
  \param wid widget specifying area if it is equal to null when widgets of all
         areas are retuned
  \return list of widgets
*/
QWidgetList QtxWorkstack::windowList( QWidget* wid ) const
{
  QList<QtxWorkstackArea*> lst;
  if ( !wid )
  {
    areas( mySplit, lst, true );
  }
  else
  {
    QtxWorkstackArea* area = wgArea( wid );
    if ( area )
      lst.append( area );
  }

  QWidgetList widList;
  for ( QList<QtxWorkstackArea*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QWidgetList wids = (*it)->widgetList();
    for ( QWidgetList::iterator itr = wids.begin(); itr != wids.end(); ++itr )
      widList.append( *itr );
  }

  return widList;
}

/*!
  \brief Get all child widgets in the active workarea.
  \return list of widgets in active workarea
*/
QWidgetList QtxWorkstack::splitWindowList() const
{
  return myArea ? myArea->widgetList() : QWidgetList();
}

/*!
  \brief Get active widget.
  \return active widget
*/
QWidget* QtxWorkstack::activeWindow() const
{
  return myWin;
}

/*!
  \brief Set active widget
  \param wid widget to activate
*/
void QtxWorkstack::setActiveWindow( QWidget* wid )
{
  if ( activeArea() )
    activeArea()->setActiveWidget( wid );
}

/*!
  \brief Split workstack.

  Splitting is possible only if there are two or more widgets in the workarea.
  This function splits current workarea to two new ones.

  \param o splitting orientation (Qt::Orientation)
*/
void QtxWorkstack::split( const int o )
{
  QtxWorkstackArea* area = myWorkArea;
  if ( !area )
    area = activeArea();
  if ( !area )
    return;

  if ( area->widgetList().count() < 2 )
    return;

  QWidget* curWid = area->activeWidget();
  if ( !curWid )
    return;

  QSplitter* s = splitter( area );
  QList<QtxWorkstackArea*> areaList;
  areas( s, areaList );

  QList<QSplitter*> splitList;
  splitters( s, splitList );

  QSplitter* trg = 0;
  if ( areaList.count() + splitList.count() < 2 || s->orientation() == o )
    trg = s;

  if ( !trg )
    trg = wrapSplitter( area );

  if ( !trg )
    return;

  trg->setOrientation( (Qt::Orientation)o );

  QtxWorkstackArea* newArea = createArea( 0 );
  trg->insertWidget( trg->indexOf( area ) + 1, newArea );

  area->removeWidget( curWid );
  newArea->insertWidget( curWid );

  distributeSpace( trg );

  curWid->show();
  curWid->setFocus();
}

/*!
  \brief Split workarea of the given widget on two parts.

  Splitting is possible only if there are two or more widgets in the workarea.
  This function splits current workarea to two new ones.

  \param wid widget belonging to the workstack
  \param o splitting orientation type (Qt::Orientation)
  \param type splitting type (QtxWorkstack::SplitType)
*/
void QtxWorkstack::Split( QWidget* wid, const Qt::Orientation o, const SplitType type )
{
  if (!wid) return;

  // find area of the given widget
  QtxWorkstackArea* area = NULL;
  QList<QtxWorkstackArea*> allAreas;
  areas(mySplit, allAreas, true);

  for ( QList<QtxWorkstackArea*>::iterator it = allAreas.begin(); it != allAreas.end() && !area; ++it )
  {
    if ( (*it)->contains( wid ) )
      area = *it;
  }

  if ( !area )
    return;

  QWidget* curWid = area->activeWidget();
  if ( !curWid )
    return;

  QWidgetList wids = area->widgetList();
  if ( wids.count() < 2 )
    return;

  QSplitter* s = splitter( area );
  QList<QtxWorkstackArea*> areaList;
  areas( s, areaList );

  QList<QSplitter*> splitList;
  splitters(s, splitList);

  QSplitter* trg = 0;
  if (areaList.count() + splitList.count() < 2 || s->orientation() == o)
    trg = s;

  if (!trg) trg = wrapSplitter(area);
  if (!trg) return;

  trg->setOrientation(o);

  QtxWorkstackArea* newArea = createArea(0);
  insertWidget(newArea, trg, area);

  switch ( type )
  {
  case SplitStay:
    for ( QWidgetList::iterator itr = wids.begin(); itr != wids.end(); ++itr )
    {
      QWidget* wid_i = *itr;
      if ( wid_i != wid )
      {
        area->removeWidget( wid_i );
        newArea->insertWidget( wid_i );
        wid_i->showMaximized();
      }
    }
    break;
  case SplitAt:
    {
      QWidgetList::iterator itr = wids.begin();
      for ( ; itr != wids.end() && *itr != wid; ++itr )
      {
      }
      for ( ; itr != wids.end(); ++itr )
      {
        area->removeWidget( *itr );
        newArea->insertWidget( *itr );
        (*itr)->showMaximized();
      }
    }
    break;
  case SplitMove:
    area->removeWidget( wid );
    newArea->insertWidget( wid );
    wid->showMaximized();
    break;
  }

  distributeSpace( trg );

  curWid->show();
  curWid->setFocus();
}

/*!
 \brief Move widget(s) from the source workarea into the target workarea
        or reorder widgets inside one workarea.

 Move \a wid2 in target workarea. Put it right after \a wid1.
 If \a all parameter is \c true, all widgets from source workarea
 will be moved including \a wid2 and source workarea will be deleted then.

 If \a wid1 and \a wid2 belongs to one workarea, widgets will be just reordered
 in that workarea.

 \param wid1 widget from target workarea
 \param wid2 widget from source workarea
 \param all  if \c true, all widgets from source workarea will
             be moved into the target one, else only the \a wid2 will be moved
*/
void QtxWorkstack::Attract( QWidget* wid1, QWidget* wid2, const bool all )
{
  if ( !wid1 || !wid2 )
    return;

  // find area of the widgets
  QtxWorkstackArea *area1 = 0, *area2 = 0;
  QList<QtxWorkstackArea*> allAreas;
  areas( mySplit, allAreas, true );
  for ( QList<QtxWorkstackArea*>::iterator it = allAreas.begin(); it != allAreas.end() && !( area1 && area2 ); ++it )
  {
    if ( (*it)->contains( wid1 ) )
      area1 = *it;

    if ( (*it)->contains( wid2 ) )
      area2 = *it;
  }

  if ( !area1 || !area2 )
    return;

  QSplitter* s1 = splitter( area1 );

  QWidget* curWid = area1->activeWidget();
  if ( !curWid )
    return;

  if ( area1 == area2 )
  {
    if ( all )
    {
      // Set wid1 at first position, wid2 at second
      area1->insertWidget( wid1 );
      area1->insertWidget( wid2, 1 );
      wid1->showMaximized();
      wid2->showMaximized();
    }
    else
    {
      // Set wid2 right after wid1
      area1->removeWidget( wid2 );
      int wid1_ind = 0;
      QWidgetList wids1 = area1->widgetList();
      for ( QWidgetList::iterator itr1 = wids1.begin(); itr1 != wids1.end() && *itr1 != wid1; ++itr1, ++wid1_ind );
      area1->insertWidget( wid2, wid1_ind + 1 );
      wid2->showMaximized();
    }
  }
  else
  {
    int wid1_ind = 0;
    QWidgetList wids1 = area1->widgetList();
    for ( QWidgetList::iterator itr1 = wids1.begin(); itr1 != wids1.end() && *itr1 != wid1; ++itr1, ++wid1_ind );
    if ( all )
    {
      // Set wid2 right after wid1, other widgets from area2 right after wid2
      QWidgetList wids2 = area2->widgetList();
      QWidgetList::iterator itr2 = wids2.begin();
      for ( int ind = wid1_ind + 1; itr2 != wids2.end(); ++itr2, ++ind )
      {
        area2->removeWidget( *itr2 );
        if ( *itr2 == wid2 )
          area1->insertWidget( *itr2, wid1_ind + 1 );
        else
          area1->insertWidget( *itr2, ind );
        (*itr2)->showMaximized();
      }
    }
    else
    {
      // Set wid2 right after wid1
      area2->removeWidget( wid2 );
      area1->insertWidget( wid2, wid1_ind + 1 );
      wid2->showMaximized();
    }
  }

  distributeSpace( s1 );

  area1->setActiveWidget( curWid );

  wid2->show();
  wid1->setFocus();
  curWid->show();
  curWid->setFocus();
}

/*!
  \brief Calculate sizes of the splitter widget for the workarea.
  \internal
*/
static void setSizes (QIntList& szList, const int item_ind,
                      const int new_near, const int new_this, const int new_farr)
{
  // set size to all items before an item # <item_ind>
  int cur_pos = 0;
  QIntList::iterator its = szList.begin();
  for (; its != szList.end() && cur_pos < item_ind; ++its, ++cur_pos) {
    *its = new_near;
  }
  if (its == szList.end()) return;
  // set size to item # <item_ind>
  *its = new_this;
  ++its;
  // set size to all items after an item # <item_ind>
  for (; its != szList.end(); ++its) {
    *its = new_farr;
  }
}

/*!
  \brief Set position of the widget relatively to its parent splitter.

  Orientation of positioning will correspond to the splitter orientation.

  \param wid widget
  \param pos position relatively to the splitter; value in the range [0..1]
*/
void QtxWorkstack::SetRelativePositionInSplitter( QWidget* wid, const double position )
{
  if ( position < 0.0 || 1.0 < position)
    return;

  if ( !wid )
    return;

  // find area of the given widget
  QtxWorkstackArea* area = NULL;
  QList<QtxWorkstackArea*> allAreas;
  areas( mySplit, allAreas, true );
  for ( QList<QtxWorkstackArea*>::iterator it = allAreas.begin(); it != allAreas.end() && !area; ++it )
  {
    if ( (*it)->contains( wid ) )
      area = *it;
  }

  if ( !area )
    return;

  QSplitter* split = splitter( area );
  if ( !split )
    return;

  // find index of the area in its splitter
  int item_ind = -1;
  bool isFound = false;
  const QObjectList& was = split->children();
  for ( QObjectList::const_iterator ito = was.begin(); ito != was.end() && !isFound; ++ito, ++item_ind )
  {
    if ( *ito == area )
      isFound = true;
  }

  if ( !isFound || item_ind == 0 )
    return;

  QIntList szList = split->sizes();
  int splitter_size = ( split->orientation() == Qt::Horizontal ? split->width() : split->height());
  int nb = szList.count();

  int new_prev = int( splitter_size * position / item_ind );
  if (nb == item_ind) return;
  int new_next = int( splitter_size * ( 1.0 - position ) / ( nb - item_ind ) );
  setSizes( szList, item_ind, new_prev, new_next, new_next );
  split->setSizes( szList );
}

/*!
  \brief Set position of the widget relatively to the entire workstack.

  If \a o is \c Qt::Horizontal, the horizontal position of \a wid will be changed.
  If \a o is \c Qt::Vertical, the vertical position of \a wid will be changed.

  \param wid widget
  \param o   orientation of positioning (\c Qt::Horizontal or \c Qt::Vertical)
  \param pos position relatively to the workstack; value in range [0..1]
*/
void QtxWorkstack::SetRelativePosition( QWidget* wid, const Qt::Orientation o,
                                        const double position )
{
  if ( position < 0.0 || 1.0 < position )
    return;

  if ( !wid )
    return;

  int splitter_size = o == Qt::Horizontal ? mySplit->width() : mySplit->height();
  int need_pos = int( position * splitter_size );
  int splitter_pos = 0;

  if ( setPosition( wid, mySplit, o, need_pos, splitter_pos ) != 0 )
  {
    // impossible to set required position
  }
}

/*!
  \brief Set accelerator key-combination for the action with specified \a id.
  \param id action ID
  \param accel action accelerator
*/
void QtxWorkstack::setAccel( const int id, const int accel )
{
  if ( !myActionsMap.contains( id ) )
    return;

  myActionsMap[id]->setShortcut( accel );
}

/*!
  \brief Get the action's accelerator key-combination.
  \param id action ID
  \return action accelerator
*/
int QtxWorkstack::accel( const int id ) const
{
  int res = 0;
  if ( myActionsMap.contains( id ) )
    res = myActionsMap[id]->shortcut()[0];
  return res;
}

/*!
  \brief Get icon for the specified action.

  If \a id is invalid, null icon is returned.

  \param id menu action ID
  \return menu item icon
*/
QIcon QtxWorkstack::icon( const int id ) const
{
  QIcon ico;
  if ( myActionsMap.contains( id ) )
    ico = myActionsMap[id]->icon();
  return ico;
}

/*!
  \brief Set menu item icon for the specified action.
  \param id menu action ID
  \param ico new menu item icon
*/
void QtxWorkstack::setIcon( const int id, const QIcon& icon )
{
  if ( !myActionsMap.contains( id ) )
    return;

  myActionsMap[id]->setIcon( icon );
}

/*!
  \brief Set actions to be visible in the context popup menu.

  Actions, which IDs are set in \a flags parameter, will be shown in the
  context popup menu. Other actions will not be shown.

  \param flags ORed together actions flags
*/
void QtxWorkstack::setMenuActions( const int flags )
{
  myActionsMap[SplitVertical]->setVisible( flags & SplitVertical );
  myActionsMap[SplitHorizontal]->setVisible( flags & SplitHorizontal );
  myActionsMap[Close]->setVisible( flags & Close );
  myActionsMap[Rename]->setVisible( flags & Rename );
}

/*!
  \brief Set actions to be visible in the context popup menu.

  Actions, which IDs are set in \a flags parameter, will be shown in the
  context popup menu. Other actions will not be shown.

  \param flags ORed together actions flags
*/
int QtxWorkstack::menuActions() const
{
  int ret = 0;
  ret = ret | ( myActionsMap[SplitVertical]->isVisible() ? SplitVertical : 0 );
  ret = ret | ( myActionsMap[SplitHorizontal]->isVisible() ? SplitHorizontal : 0 );
  ret = ret | ( myActionsMap[Close]->isVisible() ? Close : 0 );
  ret = ret | ( myActionsMap[Rename]->isVisible() ? Rename : 0 );
  return ret;
}

/*!
  \brief Calculate sizes of the splitter widget for the workarea.
  \internal
*/
static int positionSimple (QIntList& szList, const int nb, const int splitter_size,
                           const int item_ind, const int item_rel_pos,
                           const int need_pos, const int splitter_pos)
{
  if (item_ind == 0) { // cannot move in this splitter
    return (need_pos - splitter_pos);
  }

  int delta = 0;
  int new_prev = 0;
  int new_this = szList[item_ind];
  int new_next = 0;

  bool isToCheck = false;

  if (need_pos < splitter_pos) {
    // Set size of all previous workareas to zero <--
    if (item_ind == nb - 1) {
      // item iz last in the splitter, it will occupy all the splitter
      new_this = splitter_size;
    } else {
      // recompute size of next items in splitter
      new_next = (splitter_size - new_this) / (nb - item_ind - 1);
    }
    delta = need_pos - splitter_pos;

  } else if (need_pos > (splitter_pos + splitter_size)) {
    // Set size of all next workareas to zero -->
    // recompute size of previous items in splitter
    new_this = 0;
    new_prev = (splitter_size - new_this) / item_ind;
    delta = need_pos - (splitter_pos + splitter_size - new_this);

  } else { // required position lays inside this splitter
    // Move workarea inside splitter into required position <->
    int new_item_rel_pos = need_pos - splitter_pos;
    new_prev = new_item_rel_pos / item_ind;
    if (need_pos < (splitter_pos + item_rel_pos)) {
      // Make previous workareas smaller, next - bigger
      // No problem to keep old size of the widget
    } else {
      // Make previous workareas bigger, next - smaller
      if (new_this > splitter_size - new_item_rel_pos) {
        new_this = splitter_size - new_item_rel_pos;
      }
      // jfa to do: in this case fixed size of next widgets could prevent right resizing
      isToCheck = true;
    }
    if (item_ind == nb - 1) {
      new_this = splitter_size - new_item_rel_pos;
    } else {
      new_next = (splitter_size - new_item_rel_pos - new_this) / (nb - item_ind - 1);
    }
    delta = 0;
  }

  setSizes (szList, item_ind, new_prev, new_this, new_next);
  return delta;
}

/*!
  \brief Set position of the widget.

  Called from SetRelativePosition() public method.

  \param wid   widget to be moved
  \param split currently processed splitter (goes from more common
               to more particular splitter in recursion calls)
  \param o     orientation of positioning
  \param need_pos required position of the given widget in pixels
               (from top/left side of workstack area)
  \param splitter_pos position of the splitter \a split
               (from top/left side of workstack area)
  \return difference between a required and a distinguished position
*/
int QtxWorkstack::setPosition( QWidget* wid, QSplitter* split, const Qt::Orientation o,
                               const int need_pos, const int splitter_pos )
{
  if ( !wid || !split )
    return need_pos - splitter_pos;

  // Find corresponding sub-splitter.
  // Find also index of appropriate item in current splitter.
  int cur_ind = 0, item_ind = 0;
  bool isBottom = false, isFound = false;
  QSplitter* sub_split = NULL;
  const QObjectList& objs = split->children();
  for ( QObjectList::const_iterator it = objs.begin(); it != objs.end() && !isFound; ++it )
  {
    QtxWorkstackArea* area = ::qobject_cast<QtxWorkstackArea*>( *it );
    if ( area )
    {
      if ( area->contains( wid ) )
      {
        item_ind = cur_ind;
        isBottom = true;
        isFound = true;
      }
      cur_ind++;
    }
    else if ( (*it)->inherits( "QSplitter" ) )
    {
      QList<QtxWorkstackArea*> areaList;
      areas( (QSplitter*)(*it), areaList, true );
      for ( QList<QtxWorkstackArea*>::iterator ita = areaList.begin(); ita != areaList.end() && !isFound; ++ita )
      {
        if ( (*ita)->contains( wid ) )
        {
          item_ind = cur_ind;
          isFound = true;
          sub_split = (QSplitter*)*it;
        }
      }
      cur_ind++;
    }
  }

  if ( !isFound )
    return ( need_pos - splitter_pos );

  if ( split->orientation() == o )
  {
    // Find coordinates of near and far sides of the appropriate item relatively current splitter
    int splitter_size = ( o == Qt::Horizontal ? split->width() : split->height() );
    QIntList szList = split->sizes();
    int nb = szList.count();
    int item_rel_pos = 0; // position of near side of item relatively this splitter
    for (int i = 0; i < item_ind; i++) {
      item_rel_pos += szList[i];
    }
    int item_size = szList[item_ind]; // size of item
    int item_pos = splitter_pos + item_rel_pos;

    // Resize splitter items to complete the conditions
    if (isBottom) {
      // I. Bottom of splitters stack reached

      int delta = positionSimple(szList, nb, splitter_size, item_ind, item_rel_pos, need_pos, splitter_pos);
      split->setSizes(szList);
      // Recompute delta, as some windows can reject given size
      int new_item_rel_pos = 0;
      QIntList szList1 = split->sizes();
      for (int i = 0; i < item_ind; i++) {
        new_item_rel_pos += szList1[i];
      }
      delta = need_pos - (splitter_pos + new_item_rel_pos);
      return delta;

    } else {
      // II. Bottom of splitters stack is not yet reached

      if (item_ind == 0) { // cannot move in this splitter
        // Process in sub-splitter
        return setPosition(wid, sub_split, o, need_pos, splitter_pos);
      }

      int new_prev = 0;
      int new_this = szList[item_ind];
      int new_next = 0;

      if (need_pos < splitter_pos) {
        // Set size of all previous workareas to zero <--
        if (item_ind == nb - 1) {
          new_this = splitter_size;
        } else {
          new_next = (splitter_size - new_this) / (nb - item_ind - 1);
        }
        setSizes (szList, item_ind, new_prev, new_this, new_next);
        split->setSizes(szList);
        // Recompute splitter_pos, as some windows can reject given size
        int new_item_rel_pos = 0;
        QIntList szList1 = split->sizes();
        for (int i = 0; i < item_ind; i++) {
          new_item_rel_pos += szList1[i];
        }
        // Process in sub-splitter
        return setPosition(wid, sub_split, o, need_pos, splitter_pos + new_item_rel_pos);
      } else if (need_pos > (splitter_pos + splitter_size)) {
        // Set size of all next workareas to zero -->
        new_prev = (splitter_size - new_this) / item_ind;
        setSizes (szList, item_ind, new_prev, new_this, new_next);
        split->setSizes(szList);
        // Recompute splitter_pos, as some windows can reject given size
        int new_item_rel_pos = 0;
        QIntList szList1 = split->sizes();
        for (int i = 0; i < item_ind; i++) {
          new_item_rel_pos += szList1[i];
        }
        // Process in sub-splitter
        return setPosition(wid, sub_split, o, need_pos, splitter_pos + new_item_rel_pos);
      } else {
        // Set appropriate size of all previous/next items <->
        int new_item_rel_pos = item_rel_pos;
        if (need_pos < item_pos || (item_pos + item_size) < need_pos) {
          // Move item inside splitter into required position <->
          int new_this = szList[item_ind];
          int new_next = 0;
          new_item_rel_pos = need_pos - splitter_pos;
          if ((item_pos + item_size) < need_pos) {
            //new_item_rel_pos = need_pos - (item_pos + item_size);
            new_item_rel_pos = item_rel_pos + (need_pos - (item_pos + item_size));
          }
          int new_prev = new_item_rel_pos / item_ind;
          if (need_pos < (splitter_pos + item_rel_pos)) {
            // Make previous workareas smaller, next - bigger
            // No problem to keep old size of the widget
          } else {
            // Make previous workareas bigger, next - smaller
            if (new_this > splitter_size - new_item_rel_pos) {
              new_this = splitter_size - new_item_rel_pos;
            }
          }
          if (item_ind == nb - 1) {
            new_this = splitter_size - new_item_rel_pos;
          } else {
            new_next = (splitter_size - new_item_rel_pos - new_this) / (nb - item_ind - 1);
          }
          setSizes (szList, item_ind, new_prev, new_this, new_next);
          split->setSizes(szList);
          // Recompute new_item_rel_pos, as some windows can reject given size
          new_item_rel_pos = 0;
          QIntList szList1 = split->sizes();
          for (int i = 0; i < item_ind; i++) {
            new_item_rel_pos += szList1[i];
          }
        } else {
          // Do nothing
        }
        // Process in sub-splitter
        int add_pos = setPosition(wid, sub_split, o, need_pos, splitter_pos + new_item_rel_pos);
        if (add_pos == 0)
          return 0;

        // this can be if corresponding workarea is first in sub-splitter
        // or sub-splitter has another orientation

        // Resize ones again to reach precize position <->
        int need_pos_1 = splitter_pos + new_item_rel_pos + add_pos;

        // Move workarea inside splitter into required position <->
        int delta_1 = positionSimple(szList, nb, splitter_size, item_ind,
                                     new_item_rel_pos, need_pos_1, splitter_pos);
        split->setSizes(szList);
        // Recompute new_item_rel_pos, as some windows can reject given size
        new_item_rel_pos = 0;
        QIntList szList1 = split->sizes();
        for (int i = 0; i < item_ind; i++) {
          new_item_rel_pos += szList1[i];
        }
        delta_1 = need_pos_1 - (splitter_pos + new_item_rel_pos);
        return delta_1;
      }
    }
  } else {
    return setPosition(wid, sub_split, o, need_pos, splitter_pos);
  }

  return 0;
}

/*!
  \brief Redistribute space among widgets equally.
  \param split splitter
*/
void QtxWorkstack::distributeSpace( QSplitter* split ) const
{
  if ( !split )
    return;

  QIntList szList = split->sizes();
  int size = ( split->orientation() == Qt::Horizontal ?
               split->width() : split->height() ) / szList.count();
  for ( QIntList::iterator it = szList.begin(); it != szList.end(); ++it )
    *it = size;
  split->setSizes( szList );
}

/*!
  \brief Split widgets vertically.
*/
void QtxWorkstack::splitVertical()
{
  split( Qt::Horizontal );
}

/*!
  \brief Split widgets horizontally.
*/
void QtxWorkstack::splitHorizontal()
{
  split( Qt::Vertical );
}

/*!
  \brief Called when user activates "Rename" menu item.

  Changes widget title.
*/
void QtxWorkstack::onRename()
{
  if ( !myWorkWin )
    return;

  bool ok = false;
  QString newName = QInputDialog::getText( topLevelWidget(),  tr( "Rename" ), tr( "Enter new name:" ),
                                           QLineEdit::Normal, myWorkWin->windowTitle(), &ok );
  if ( ok && !newName.isEmpty() )
    myWorkWin->setWindowTitle( newName );
}

/*!
  \brief Wrap area into the new splitter.
  \param workarea
  \return new splitter
*/
QSplitter* QtxWorkstack::wrapSplitter( QtxWorkstackArea* area )
{
  if ( !area )
    return 0;

  QSplitter* pSplit = splitter( area );
  if ( !pSplit )
    return 0;

  bool upd = pSplit->updatesEnabled();
  pSplit->setUpdatesEnabled( false );

  QIntList szList = pSplit->sizes();

  QSplitter* wrap = new QtxWorkstackSplitter( 0 );
  pSplit->insertWidget( pSplit->indexOf( area ) + 1, wrap );
  wrap->setVisible( true );
  wrap->addWidget( area );

  pSplit->setSizes( szList );

  pSplit->setUpdatesEnabled( upd );

  return wrap;
}

/*!
  \brief Reparent and add widget.
  \param wid widget
  \param pWid parent widget
  \param after widget after which \a wid should be added
*/
void QtxWorkstack::insertWidget( QWidget* wid, QWidget* pWid, QWidget* after )
{
  if ( !wid || !pWid )
    return;

  QWidgetList moveList;
  const QObjectList& lst = pWid->children();
  bool found = false;
  for ( QObjectList::const_iterator it = lst.begin(); it != lst.end(); ++it )
  {
    if ( found && ( (*it)->inherits( "QSplitter" ) ||
                    (*it)->inherits( "QtxWorkstackArea" ) ) )
      moveList.append( (QWidget*)(*it) );
    if ( *it == after )
      found = true;
  }

  QMap<QWidget*, bool> map;
  for ( QWidgetList::iterator it = moveList.begin(); it != moveList.end(); ++it )
  {
    map.insert( *it, (*it)->isVisibleTo( (*it)->parentWidget() ) );
    (*it)->setParent( 0 );
    (*it)->hide();
  }

  wid->setParent( pWid );

  for ( QWidgetList::iterator itr = moveList.begin(); itr != moveList.end(); ++itr )
  {
    (*itr)->setParent( pWid );
    (*itr)->setVisible( map.contains( *itr ) ? map[*itr] : false );
  }
}

/*!
  \brief Close active window.
*/
void QtxWorkstack::onCloseWindow()
{
  if ( myWorkWin )
    myWorkWin->close();
  else if( activeWindow() )
    activeWindow()->close();
}

/*!
  \brief Called when workarea is destroyed.

  Set input focus to the neighbour area.

  \param obj workarea being destroyed
*/
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

  QApplication::postEvent( this, new QEvent( QEvent::User ) );
}

/*!
  \brief Called on window activating.
  \param area workarea being activated (not used)
*/
void QtxWorkstack::onWindowActivated( QWidget* /*area*/ )
{
  const QObject* obj = sender();
  if ( !obj->inherits( "QtxWorkstackArea" ) )
    return;

  setActiveArea( (QtxWorkstackArea*)obj );
}

/*!
  \brief Called on window deactivating.
  \param area workarea being deactivated
*/
void QtxWorkstack::onDeactivated( QtxWorkstackArea* area )
{
  if ( myArea != area )
    return;

  QList<QtxWorkstackArea*> lst;
  areas( mySplit, lst, true );

  int idx = lst.indexOf( area );
  if ( idx == -1 )
    return;

  myWin = 0;
  myArea = 0;

  QtxWorkstackArea* newArea = neighbourArea( area );
  if ( newArea && newArea->activeWidget() )
    newArea->activeWidget()->setFocus();

  QApplication::postEvent( this, new QEvent( QEvent::User ) );
}

/*!
  \brief Create and show popup menu for workarea.
  \param w workarea
  \param p popup position
*/
void QtxWorkstack::onContextMenuRequested( QWidget* w, QPoint p )
{
  QtxWorkstackArea* anArea = ::qobject_cast<QtxWorkstackArea*>( (QObject*)sender() );
  if ( !anArea )
    anArea = activeArea();

  if ( !anArea )
    return;

  QWidgetList lst = anArea->widgetList();
  if ( lst.isEmpty() )
    return;

  myWorkWin = w;
  myWorkArea = anArea;

  QMenu* pm = new QMenu();

  if ( lst.count() > 1 )
  {
    if ( !myActionsMap[SplitVertical]->isEnabled() )
      myActionsMap[SplitVertical]->setEnabled(true);
    pm->addAction( myActionsMap[SplitVertical] );
    if ( !myActionsMap[SplitHorizontal]->isEnabled() )
      myActionsMap[SplitHorizontal]->setEnabled(true);
    pm->addAction( myActionsMap[SplitHorizontal] );
    pm->addSeparator();
  }

  if ( w )
  {
    if ( myActionsMap[Close]->isEnabled() )
      pm->addAction( myActionsMap[Close] );
    if ( myActionsMap[Rename]->isEnabled() )
      pm->addAction( myActionsMap[Rename] );
  }

  Qtx::simplifySeparators( pm );

  if ( !pm->actions().isEmpty() )
    pm->exec( p );

  delete pm;

  myWorkWin = 0;
  myWorkArea = 0;
}

/*!
  \brief Add child widget.
  \param w widget
  \param f widget flags
  \return child widget container
*/
QWidget* QtxWorkstack::addWindow( QWidget* w, Qt::WindowFlags f )
{
  if ( !w )
    return 0;

  return targetArea()->insertWidget( w, -1, f );
}

/*!
  \brief Handle custom events.
  \param e custom event (not used)
*/
void QtxWorkstack::customEvent( QEvent* /*e*/ )
{
  updateState();
}

/*!
  \brief Get splitter corresponding to the workarea.
  \param workarea
  \return splitter corresponding to the workarea
*/
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

/*!
  \brief Get list of child splitters.
  \param split parent splitter
  \param splitList list to be filled with child splitters
  \param rec if \c true, perform recursive search of children
*/
void QtxWorkstack::splitters( QSplitter* split, QList<QSplitter*>& splitList, const bool rec ) const
{
  if ( !split )
    return;

  const QObjectList& objs = split->children();
  for ( QObjectList::const_iterator it = objs.begin(); it != objs.end(); ++it )
  {
    if ( rec )
      splitters( (QSplitter*)*it, splitList, rec );
    if ( (*it)->inherits( "QSplitter" ) )
      splitList.append( (QSplitter*)*it );
  }
}

/*!
  \brief Get list of child workareas.
  \param split parent splitter
  \param areaList list to be filled with child workareas
  \param rec if \c true, perform recursive search of children
*/
void QtxWorkstack::areas( QSplitter* split, QList<QtxWorkstackArea*>& areaList, const bool rec ) const
{
  if ( !split )
    return;

  const QObjectList& objs = split->children();
  for ( QObjectList::const_iterator it = objs.begin(); it != objs.end(); ++it )
  {
    if ( (*it)->inherits( "QtxWorkstackArea" ) )
      areaList.append( (QtxWorkstackArea*)*it );
    else if ( rec && (*it)->inherits( "QSplitter" ) )
      areas( (QSplitter*)*it, areaList, rec );
  }
}

/*!
  \brief Get active workarea.
  \return active workarea
*/
QtxWorkstackArea* QtxWorkstack::activeArea() const
{
  return myArea;
}

/*!
  \brief Get target area (for which the current operation should be done).

  Returns active workarea or current area (if there is no active workarea).
  If there are no workareas, create new workarea and return it.

  \return workarea
*/
QtxWorkstackArea* QtxWorkstack::targetArea()
{
  QtxWorkstackArea* area = activeArea();
  if ( !area )
    area = currentArea();
  if ( !area )
  {
    QList<QtxWorkstackArea*> lst;
    areas( mySplit, lst );
    if ( !lst.isEmpty() )
      area = lst.first();
  }

  if ( !area )
    area = createArea( mySplit );

  return area;
}

/*!
  \brief Get current workarea.

  Current workarea is that one which has input focus.

  \return current area
*/
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

/*!
  \brief Create new workarea.
  \param parent parent widget
  \return created workarea
*/
QtxWorkstackArea* QtxWorkstack::createArea( QWidget* parent ) const
{
  QtxWorkstackArea* area = new QtxWorkstackArea( parent );

  connect( area, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
  connect( area, SIGNAL( activated( QWidget* ) ), this, SLOT( onWindowActivated( QWidget* ) ) );
  connect( area, SIGNAL( contextMenuRequested( QWidget*, QPoint ) ),
           this, SLOT( onContextMenuRequested( QWidget*, QPoint ) ) );
  connect( area, SIGNAL( deactivated( QtxWorkstackArea* ) ), this, SLOT( onDeactivated( QtxWorkstackArea* ) ) );

  return area;
}

/*!
  \brief Set active workarea.
  \param workarea
*/
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

/*!
  \brief Get workarea which is nearest to \a area.
  \param area area for which neighbour is searched
  \return neighbour area (or 0 if not found)
*/
QtxWorkstackArea* QtxWorkstack::neighbourArea( QtxWorkstackArea* area ) const
{
  QList<QtxWorkstackArea*> lst;
  areas( mySplit, lst, true );
  int pos = lst.indexOf( area );
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

/*!
  \brief Get workarea covering point.
  \return workarea
  \param p point
*/
QtxWorkstackArea* QtxWorkstack::areaAt( const QPoint& p ) const
{
  QtxWorkstackArea* area = 0;
  QList<QtxWorkstackArea*> lst;
  areas( mySplit, lst, true );
  for ( QList<QtxWorkstackArea*>::iterator it = lst.begin(); it != lst.end() && !area; ++it )
  {
    QtxWorkstackArea* cur = *it;
    QRect r = cur->geometry();
    if ( cur->parentWidget() )
      r = QRect( cur->parentWidget()->mapToGlobal( r.topLeft() ), r.size() );
    if ( r.contains( p ) )
      area = cur;
  }
  return area;
}

/*!
  \brief Update internal state.
*/
void QtxWorkstack::updateState()
{
  updateState( mySplit );
}

/*!
  \brief Update splitter state.
  \param split splitter to be updated
*/
void QtxWorkstack::updateState( QSplitter* split )
{
  QList<QSplitter*> recList;
  splitters( split, recList, false );
  for ( QList<QSplitter*>::iterator itr = recList.begin(); itr != recList.end(); ++itr )
    updateState( *itr );

  QList<QSplitter*> splitList;
  splitters( split, splitList, false );

  QList<QtxWorkstackArea*> areaList;
  areas( split, areaList, false );

  bool vis = false;
  for ( QList<QtxWorkstackArea*>::iterator it = areaList.begin(); it != areaList.end(); ++it )
  {
    if ( (*it)->isEmpty() )
      (*it)->hide();
    else
    {
      (*it)->show();
      vis = true;
    }
  }

  if ( split == mySplit )
    return;

  for ( QList<QSplitter*>::iterator iter = splitList.begin(); iter != splitList.end() && !vis; ++iter )
    vis = (*iter)->isVisibleTo( (*iter)->parentWidget() );

  if ( areaList.isEmpty() && splitList.isEmpty() )
    delete split;
  else
    split->setVisible( vis );
}

/*!
  \brief Dump workstack configuration to the state description array.
  \param version number
  \return state byte array.
*/
QByteArray QtxWorkstack::saveState( int version ) const
{
  QByteArray data;

  QDataStream stream( &data, QIODevice::WriteOnly );
  stream << QtxWorkstack::VersionMarker;
  stream << version;
  saveState( stream );

  return data;
}

/*!
  \brief Restore workstack configuration from the state description array.
  \param version number
  \return restore performing state
*/
bool QtxWorkstack::restoreState( const QByteArray& state, int version )
{
  if ( state.isEmpty() )
    return false;

  QByteArray sd = state;
  QDataStream stream( &sd, QIODevice::ReadOnly );
  int marker, ver;
  stream >> marker;
  stream >> ver;
  if ( stream.status() != QDataStream::Ok || marker != QtxWorkstack::VersionMarker || ver != version )
    return false;

  return restoreState( stream );
}

void QtxWorkstack::saveState( QDataStream& stream ) const
{
  mySplit->saveState( stream );
}

bool QtxWorkstack::restoreState( QDataStream& stream )
{
  QMap<QString, QtxWorkstackChild*> map;
  QList<QtxWorkstackArea*> areaList;
  areas( mySplit, areaList, true );
  for ( QList<QtxWorkstackArea*>::const_iterator it = areaList.begin(); it != areaList.end(); ++it )
  {
    QtxWorkstackArea* area = *it;
    QList<QtxWorkstackChild*> childList = area->childList();
    for ( QList<QtxWorkstackChild*>::iterator itr = childList.begin(); itr != childList.end(); ++itr )
    {
      QtxWorkstackChild* c = *itr;
      if ( !c->widget() )
        continue;

      map.insert( c->widget()->objectName(), c );

      qDebug( "QtxWorkstack::restoreState: found widget \"%s\"", (const char*)c->widget()->objectName().toLatin1() );
    }
  }

  int marker;
  stream >> marker;
  if ( stream.status() != QDataStream::Ok || marker != QtxWorkstack::SplitMarker )
    return false;

  QtxWorkstackSplitter* split = new QtxWorkstackSplitter( this );
  if ( layout() )
    layout()->addWidget( split );

  bool ok = split->restoreState( stream, map );
  if ( !ok )
    delete split;
  else
  {
    mySplit->deleteLater();
    mySplit = split;

    QList<QtxWorkstackArea*> aList;
    areas( mySplit, aList, true );

    QtxWorkstackArea* a = !aList.isEmpty() ? aList.first() : 0;
    for ( QMap<QString, QtxWorkstackChild*>::const_iterator it = map.begin(); it != map.end(); ++it )
    {
      QtxWorkstackChild* c = it.value();
      if ( c->widget() )
        c->widget()->setVisible( false );
      if ( a )
        a->insertChild( c );
      else
        c->setVisible( false );
    }
  }

  return ok;
}

/*!
  \brief Set resize mode of all splitters opaque or transparent.
  \param opaque opaque mode
*/
void QtxWorkstack::setOpaqueResize( bool opaque )
{
  QList<QSplitter*> splitList;
  splitters( mySplit, splitList, true );
  splitList << mySplit;
  foreach( QSplitter* split, splitList )
    split->setOpaqueResize( opaque );
}

/*!
  \brief Get resize mode of all splitters: opaque (\c true) or transparent (\c false).
  \return current opaque mode
*/
bool QtxWorkstack::opaqueResize() const
{
  return mySplit->opaqueResize();
}

/*!
  \brief Show/hide splitter state and area.
  \param wid widget (and parent area) will be shown/hidden
  \param parent_list parent splitters list
  \param split splitter will be shown/hidden
  \param visible splitter
*/
void QtxWorkstack::splitterVisible(QWidget* wid, QList<QSplitter*>& parent_list, QSplitter* split, bool visible)
{
  QList<QSplitter*> recList;
  splitters( split, recList, false );
  for ( QList<QSplitter*>::iterator itr = recList.begin(); itr != recList.end(); ++itr ) {
    parent_list.prepend( *itr );
    splitterVisible( wid, parent_list, *itr, visible );
  }

  QList<QtxWorkstackArea*> areaList;
  areas( split, areaList, false );
  for ( QList<QtxWorkstackArea*>::const_iterator it = areaList.begin(); it != areaList.end(); ++it ) {
    QtxWorkstackArea* area = *it;
    bool isCurrentWidget = false;

    area->showTabBar(visible);

    // 1. Looking for the selected widget at the lowest level among all splitted areas.
    QList<QtxWorkstackChild*> childList = area->childList();
    for ( QList<QtxWorkstackChild*>::iterator itr = childList.begin(); itr != childList.end(); ++itr ) {
      QWidget* aCurWid = (*itr)->widget();
      if ( aCurWid == wid ) {
        isCurrentWidget = true;
        aCurWid->setVisible( true );
      }
      else
        aCurWid->setVisible( visible );
    }

    // 2. Show/Hide other areas and widgets that don't contain the desired widget
    if ( !isCurrentWidget || visible )
      area->setVisible( visible );

    if ( !isCurrentWidget && !visible )
      continue;

    // 3. Show/hide all parent widgets
    QSplitter* pSplit = splitter( area );
    int count = pSplit->count();
    for ( int i = 0; i < count; i++ ) {
      if ( pSplit->indexOf( area ) == i && !visible )
        continue;
      pSplit->widget(i)->setVisible( visible );
    }

    // 4. Show/hide all parent splitters don't contain the selected widget
    if ( visible )
      pSplit->setVisible( true );

    if ( isCurrentWidget && !visible ) {
      for ( QList<QSplitter*>::iterator itr = parent_list.begin(); itr != parent_list.end() && pSplit != mySplit; ++itr ) {
        if ( pSplit == *itr )
          continue;
        QList<QSplitter*> splitList;
        splitters( *itr, splitList, false );
        for ( QList<QSplitter*>::iterator iter = splitList.begin(); iter != splitList.end(); ++iter ) {
          if ( pSplit == (*iter) ) {
            pSplit = *itr;
            continue;
          }
          (*iter)->setVisible( false );
        }
      }
    }
  }
}

/*!
  \brief Show/hide splitters state and area.
  \param wid widget (and parent area) will be shown/hidden
  \param visible splitters
*/
void QtxWorkstack::splittersVisible( QWidget* wid, bool visible )
{
  QList<QSplitter*> parent_list;
  parent_list.append( mySplit );
  splitterVisible( wid, parent_list, mySplit, visible );
}

/*!
  \fn void QtxWorkstack::windowActivated( QWidget* w )
  \brief Emitted when the workstack's child widget \w is activated.
  \param w widget being activated
*/

/*!
  \brief Gets area containing given widget
  \param wid widget
  \return pointer to QtxWorkstackArea* object
*/
QtxWorkstackArea* QtxWorkstack::wgArea( QWidget* wid ) const
{
  QtxWorkstackArea* resArea = 0;

  QList<QtxWorkstackArea*> areaList;
  areas( mySplit, areaList, true );

  QList<QtxWorkstackArea*>::ConstIterator it;
  for ( it = areaList.begin(); it != areaList.end() && !resArea; ++it )
  {
    if ( (*it)->contains( wid ) )
      resArea = *it;
  }

  return resArea;
}

/*!
  \brief Moves the first widget to the same area which the second widget belongs to
  \param wid widget to be moved
  \param wid_to widget specified the destination area
  \param before specifies whether the first widget has to be moved before or after
         the second widget
  \return \c true if operation is completed successfully, \c false otherwise
*/
bool QtxWorkstack::move( QWidget* wid, QWidget* wid_to, const bool before )
{
  if ( wid && wid_to )
  {
    QtxWorkstackArea* area_src = wgArea( wid );
    QtxWorkstackArea* area_to = wgArea( wid_to );
    if ( area_src && area_to )
    {
      // find index of the second widget
      QWidgetList wgList = area_to->widgetList();
      QWidgetList::ConstIterator it;
      int idx = 0;
      for ( it = wgList.begin(); it != wgList.begin(); ++it, idx++ )
      {
        if ( *it == wid_to )
          break;
      }

      if ( idx < wgList.count() ) // paranoidal check
      {
        if ( !before )
          idx++;
        area_src->removeWidget( wid, true );
        area_to->insertWidget( wid, idx );
        wid->showMaximized();
        return true;
      }
    }
  }
  return false;
}

/*!
  \brief Group all windows in one area
  \return \c true if operation is completed successfully, \c false otherwise
*/
void QtxWorkstack::stack()
{
  QWidgetList wgList = windowList();
  if ( !wgList.count() )
    return; // nothing to do

  QtxWorkstackArea* area_to = 0;
  QWidgetList::ConstIterator it;
  for ( it = wgList.begin(); it != wgList.end(); ++it )
  {
    QtxWorkstackArea* area_src = 0;
    if ( !area_to )
    {
      area_to = wgArea( *it );
      area_src = area_to;
    }
    else
      area_src = wgArea( *it );

    if ( area_src != area_to )
    {
      area_src->removeWidget( *it, true );
      area_to->insertWidget( *it, -1 );
      (*it)->showMaximized();
    }
  }
}

QAction* QtxWorkstack::action( const int id ) const
{
  return myActionsMap.contains( id ) ? myActionsMap[id] : 0;
}
