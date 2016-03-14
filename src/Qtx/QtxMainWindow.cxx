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
//  File:      QtxMainWindow.cxx
//  Author:    Sergey TELKOV

#include "QtxMainWindow.h"

#include "QtxToolBar.h"

#include <QEvent>
#include <QPoint>
#include <QTimer>
#include <QLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QRubberBand>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <cstdio>
/*!
  \class QtxMainWindow::Filter
  \internal
  \brief Internal object used to filter child removal events for 
         specified widget from parent widget.
*/

class QtxMainWindow::Filter : public QObject
{
public:
  Filter( QWidget*, QtxMainWindow*, QObject* = 0 );
  virtual ~Filter();

  virtual bool eventFilter( QObject*, QEvent* );

private:
  QMainWindow* myMain;      //!< parent main window
  QWidget*     myWidget;    //!< widget being watched
};

/*!
  \brief Constructor.
  \param wid widget to be watched
  \param mw parent main window
  \param parent parent object (in terms of QObject)
*/
QtxMainWindow::Filter::Filter( QWidget* wid, QtxMainWindow* mw, QObject* parent )
: QObject( parent ),
  myMain( mw ),
  myWidget( wid )
{
  QApplication::instance()->installEventFilter( this );
};

/*!
  \brief Destructor.
*/
QtxMainWindow::Filter::~Filter()
{
}

/*!
  \brief Event filter.

  Watches for the specified widget and prevents its removal from the
  parent main window.

  \param o recevier object
  \param e event
*/
bool QtxMainWindow::Filter::eventFilter( QObject* o, QEvent* e )
{
  if ( myMain == o && e->type() == QEvent::ChildRemoved &&
       myWidget == ((QChildEvent*)e)->child() )
    return true;

  return QObject::eventFilter( o, e );
}

/*!
  \class QtxMainWindow::Resizer
  \internal
  \brief Internal object used to resize dock widgets.
*/

class QtxMainWindow::Resizer : public QObject
{
public:
  Resizer( const QPoint&, const Qt::Orientation, QtxMainWindow* );
  virtual ~Resizer();

  QMouseEvent*    finalEvent() const;
  void            setFinalEvent( QMouseEvent* );

  void            setPosition( const QPoint& );
  virtual bool    eventFilter( QObject*, QEvent* );

private:
  void            setFilters( bool );

private:
  QPoint          myPos;
  QMainWindow*    myMain;
  QRubberBand*    myRubber;
  Qt::Orientation myOrient;
  QMouseEvent*    myFinEvent;
};

/*!
  \brief Constructor.
  \param mw parent main window
*/
QtxMainWindow::Resizer::Resizer( const QPoint& p, const Qt::Orientation o, QtxMainWindow* mw )
: QObject( mw ),
  myMain( mw ),
  myOrient( o ),
  myFinEvent( 0 )
{
  setFilters( true );

  myRubber = new QRubberBand( QRubberBand::Line, 0 );

  setPosition( p );

  myRubber->hide();
};

/*!
  \brief Destructor.
*/
QtxMainWindow::Resizer::~Resizer()
{
  delete myRubber;

  setFilters( false );
}

void QtxMainWindow::Resizer::setPosition( const QPoint& pos )
{
  myPos = pos;
  if ( myRubber ) {
    QRect r;
    QPoint min = myMain->mapToGlobal( myMain->rect().topLeft() );
    QPoint max = myMain->mapToGlobal( myMain->rect().bottomRight() );
    if ( myOrient == Qt::Horizontal ) {
      int p = qMax( qMin( myPos.y(), max.y() ), min.y() );
      r = QRect( myMain->mapToGlobal( QPoint( 0, 0 ) ).x(), p - 1, myMain->width(), 3 );
    }
    else {
      int p = qMax( qMin( myPos.x(), max.x() ), min.x() );
      r = QRect( p - 1, myMain->mapToGlobal( QPoint( 0, 0 ) ).y(), 3, myMain->height() );
    }
    myRubber->setGeometry( r );
    if ( !myRubber->isVisible() )
      myRubber->show();
  }
}

QMouseEvent* QtxMainWindow::Resizer::finalEvent() const
{
  return myFinEvent;
}

void QtxMainWindow::Resizer::setFinalEvent( QMouseEvent* e )
{
  myFinEvent = e;
}

/*!
  \brief Event filter.

  \param o recevier object
  \param e event
*/
bool QtxMainWindow::Resizer::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::Timer ) {
    if ( !finalEvent() )
      return true;

    setFilters( false );
    QApplication::postEvent( myMain, finalEvent() );
    myFinEvent = 0;
    deleteLater();
  }

  return QObject::eventFilter( o, e );
}

void QtxMainWindow::Resizer::setFilters( bool on )
{
  if ( myMain ) {
    if ( on )
      myMain->layout()->installEventFilter( this );
    else
      myMain->layout()->removeEventFilter( this );
  }

  QTimer* t = myMain->layout()->findChild<QTimer*>();
  if ( t ) {
    if ( on )
      t->installEventFilter( this );
    else
      t->removeEventFilter( this );
  }
}

/*!
  \class QtxMainWindow
  \brief Enhanced main window which supports dockable menubar and status bar
         plus geometry saving/restoring.
*/

/*!
  \brief Constructor.
  \param parent parent widget
  \param f widget flags (Qt::WindowFlags)
*/
QtxMainWindow::QtxMainWindow( QWidget* parent, Qt::WindowFlags f )
: QMainWindow( parent, f ),
  myMenuBar( 0 ),
  myStatusBar( 0 ),
  myOpaque( true ),
  myResizer( 0 ),
  myMouseMove( 0 ),
  myFullScreenAllowed(true),
  myMinimizeAllowed(true)
{
  //rnv: Enables tooltips for inactive windows.
  //rnv: For details see http://bugtracker.opencascade.com/show_bug.cgi?id=20893
  setAttribute(Qt::WA_AlwaysShowToolTips);
}

/*!
  \brief Destructor.
*/
QtxMainWindow::~QtxMainWindow()
{
  setDockableMenuBar( false );
  setDockableStatusBar( false );
}

/*!
  \brief Check if the menu bar is dockable.
  \return \c true if dockable menu bar exists
*/
bool QtxMainWindow::isDockableMenuBar() const
{
  return myMenuBar != 0;
}

/*!
  \brief Set menu bar dockable/undockable.
  \param on if \c true, make menu bar dockable, otherwise 
            make menu bar undockable
*/
void QtxMainWindow::setDockableMenuBar( const bool on )
{
  if ( isDockableMenuBar() == on )
    return;

  QMenuBar* mb = menuBar();
  if ( !mb )
    return;

  if ( on && !myMenuBar )
  {
    myMenuBar = new QtxToolBar( true, this );
    new Filter( mb, this, myMenuBar );
    myMenuBar->setObjectName( "menu_bar_container" );
    myMenuBar->setWindowTitle( tr( "Menu bar" ) );
    myMenuBar->addWidget( mb );
    myMenuBar->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );

    addToolBarBreak( Qt::TopToolBarArea );
    addToolBar( Qt::TopToolBarArea, myMenuBar );
    addToolBarBreak( Qt::TopToolBarArea );

    connect( myMenuBar, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
  }
  else if ( !on && myMenuBar )
  {
    setMenuBar( mb );
    disconnect( myMenuBar, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
    delete myMenuBar;
    myMenuBar = 0;
  }
}

/*!
  \brief Check if the status bar is dockable.
  \return \c true if dockable status bar exists
*/
bool QtxMainWindow::isDockableStatusBar() const
{
  return myStatusBar;
}

/*!
  \brief Set status bar dockable/undockable.
  \param on if \c true, make status bar dockable, otherwise 
            make status bar undockable
*/
void QtxMainWindow::setDockableStatusBar( const bool on )
{
  if ( isDockableStatusBar() == on )
    return;

  QStatusBar* sb = statusBar();
  if ( !sb )
    return;

  if ( on && !myStatusBar )
  {
    sb->setMinimumWidth( 250 );
    sb->setSizeGripEnabled( false );
    myStatusBar = new QtxToolBar( true, this );
    new Filter( sb, this, myStatusBar );
    myStatusBar->setObjectName( "status_bar_container" );
    myStatusBar->setWindowTitle( tr( "Status bar" ) );
    myStatusBar->addWidget( sb );
    myStatusBar->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );

    addToolBar( Qt::BottomToolBarArea, myStatusBar );

    connect( myStatusBar, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
  }
  else if ( !on && myStatusBar )
  {
    setStatusBar( sb );
    disconnect( myStatusBar, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
    delete myStatusBar;
    myStatusBar = 0;

    sb->setSizeGripEnabled( true );
  }
}

bool QtxMainWindow::isOpaqueResize() const
{
  return myOpaque;
}

void QtxMainWindow::setOpaqueResize( bool on )
{
  myOpaque = on;
}

/*!
  \brief Dump main window geometry to the string.
  \return string represenation of the window geometry
*/
QString QtxMainWindow::storeGeometry() const
{
  QRect frame = frameGeometry();
  QRect screen = QApplication::desktop()->availableGeometry( this );

  QString x;
  if ( frame.left() == screen.left() )
    x = QString( "+0" );
  else if ( frame.right() == screen.right() )
    x = QString( "-0" );
  else
    x = QString( "+%1" ).arg( frame.left() );

  QString y;
  if ( frame.top() == screen.top() )
    y = QString( "+0" );
  else if ( frame.bottom() == screen.bottom() )
    y = QString( "-0" );
  else
    y = QString( "+%1" ).arg( frame.top() );

  QString geom = QString( "%1x%2%3%4" ).arg( width() ).arg( height() ).arg( x ).arg( y );

  QString state;
  switch ( windowState() )
  {
  case Qt::WindowMaximized:
    state = QString( "max" );
    break;
  case Qt::WindowMinimized:
    if ( isMinimizeAllowed() ) state = QString( "min" );
    break;
  case Qt::WindowFullScreen:
    state = isFullScreenAllowed() ? QString( "full" ) : QString( "max" );
    break;
  }

  if ( !state.isEmpty() )
    geom += QString( ":" ) + state;

  return geom;
}

/*!
  \brief Restore main window geometry from the string.
  \param str string represenation of the window geometry
*/
void QtxMainWindow::retrieveGeometry( const QString& str )
{
  QString geom = str;
  geom.remove( '\t' );
  geom.remove( ' ' );

  QRect rect = geometry();
  QRect screen = QApplication::desktop()->availableGeometry( this );

  QRegExp szRx( "(\\d+%?)\\s*x\\s*(\\d+%?)" );
  if ( szRx.indexIn( geom ) != -1 )
  {
    int w = -1;
    bool wp = false;
    int ws = geometryValue( szRx.cap( 1 ).trimmed(), w, wp );
    bool wOk = ws != 0;
    if ( wOk && wp )
      w = screen.width() * qMax( qMin( w, 100 ), 0 ) / 100;
    wOk = wOk && w;

    int h = -1;
    bool hp = false;
    int hs = geometryValue( szRx.cap( 2 ).trimmed(), h, hp );
    bool hOk = hs != 0;
    if ( hOk && hp )
      h = screen.height() * qMax( qMin( h, 100 ), 0 ) / 100;
    hOk = hOk && h;

    if ( wOk && hOk )
      rect.setSize( QSize( w, h ) );
  }

  QRegExp posRx( "([+|-]\\d+%?)\\s*([+|-]\\d+%?)" );
  if ( posRx.indexIn( geom ) != -1 )
  {
    int x = -1;
    bool xp = false;
    int xs = geometryValue( posRx.cap( 1 ).trimmed(), x, xp );
    bool xOk = xs != 0;
    if ( xOk )
    {
      if ( xp )
	x = screen.width() * qMax( qMin( x, 100 ), 0 ) / 100;
      x = ( xs > 0 ? x : screen.right() - x - rect.width() ) + frameGeometry().x() - geometry().x();
    }

    int y = -1;
    bool yp = false;
    int ys = geometryValue( posRx.cap( 2 ).trimmed(), y, yp );
    bool yOk = ys != 0;
    if ( yOk )
    {
      if ( yp )
	y = screen.height() * qMax( qMin( y, 100 ), 0 ) / 100;
      y = ( ys > 0 ? y : screen.bottom() - y - rect.height() ) + frameGeometry().y() - geometry().y();
    }

    if ( xOk && yOk )
      rect.moveTo( x, y );
  }

  Qt::WindowState state = Qt::WindowNoState;

  QRegExp stRx( ":(\\w+)" );
  if ( stRx.indexIn( geom ) != -1 )
  {
    QString stStr = stRx.cap( 1 ).trimmed().toLower();
    if ( stStr.startsWith( QString( "max" ) ) ) {
      state = Qt::WindowMaximized;
    }
    else if ( stStr.startsWith( QString( "min" ) ) ) {
      if ( isMinimizeAllowed() )
	state = Qt::WindowMinimized;
    }
    else if ( stStr.startsWith( QString( "full" ) ) ) {
      state = isFullScreenAllowed() ? Qt::WindowFullScreen : Qt::WindowMaximized;
    }
  }

  resize( rect.size() );
  move( rect.topLeft() );

  if ( state != Qt::WindowNoState )
    setWindowState( state );
}

/*!
  \brief Retrieve numerical value from the string.
  
  Numerical value in the string have the structure [+|-]\d*[%],
  that is one or more digits which can start from "+" or "-" and
  can end with "%" symbol.

  \param str string being converted
  \param num returning value (> 0)
  \param percent if string ends with "%" this parameter is equal to \c true after
         returning from the function
  \return -1 if value < 0, 1 if value > 0 and 0 in case of error
*/
int QtxMainWindow::geometryValue( const QString& str, int& num, bool& percent ) const
{
  num = -1;
  int res = 1;
  QString numStr = str;
  if ( numStr.startsWith( "+" ) || numStr.startsWith( "-" ) )
  {
    res = numStr.startsWith( "+" ) ? 1 : -1;
    numStr = numStr.mid( 1 );
  }

  percent = numStr.endsWith( "%" );
  if ( percent )
    numStr = numStr.mid( 0, numStr.length() - 1 );

  bool ok = false;
  num = numStr.toInt( &ok );
  if ( !ok )
    res = 0;

  return res;
}

/*!
  \brief Called when child object (menu bar, status bar) is destroyed.
  
  Clears internal pointer to prevent crashes.

  \param obj signal sender (object being destroyed)
*/
void QtxMainWindow::onDestroyed( QObject* obj )
{
  QObject* o = 0;
  if ( obj == myMenuBar )
  {
    myMenuBar = 0;
    o = menuBar();
  }
  else if ( obj == myStatusBar )
  {
    myStatusBar = 0;
    o = statusBar();
  }

  if ( o )
  {
    QChildEvent ce( QEvent::ChildRemoved, o );
    QApplication::sendEvent( this, &ce );
  }
}

bool QtxMainWindow::event( QEvent* e )
{
//   if ( e->type() == QEvent::WindowDeactivate ) {
//     printf( "----------------> Deactivated\n" );
//   }

  if ( myResizer ) {
    QMouseEvent* me = static_cast<QMouseEvent*>( e );
    if ( ( e->type() == QEvent::MouseButtonRelease && me->button() == Qt::LeftButton ) || 
	 ( e->type() == QEvent::MouseButtonPress && me->button() != Qt::LeftButton ) ) {
      if ( me->button() == Qt::LeftButton ) {
	if ( myMouseMove ) {
	  QMainWindow::event( myMouseMove );
	  delete myMouseMove;
	  myMouseMove = 0;
	}

	QMouseEvent* me = static_cast<QMouseEvent*>( e );
	myResizer->setFinalEvent( new QMouseEvent( QEvent::MouseButtonRelease, me->pos(), me->globalPos(),
						   Qt::LeftButton, me->buttons(), me->modifiers() ) );
	myResizer = 0;
	return true;
      }
    }
  }

  if ( myResizer && e->type() == QEvent::MouseMove ) {
    QMouseEvent* me = static_cast<QMouseEvent*>( e );
    if ( myMouseMove )
      delete myMouseMove;
    myMouseMove = new QMouseEvent( me->type(), me->pos(), me->globalPos(),
                                   me->button(), me->buttons(), me->modifiers() );
    myResizer->setPosition( me->globalPos() );
  }

  bool ok = QMainWindow::event( e );

  if ( !myResizer && e->type() == QEvent::MouseButtonPress ) {
    QMouseEvent* me = static_cast<QMouseEvent*>( e );
    if ( !isOpaqueResize() && ok && testAttribute( Qt::WA_SetCursor ) && me->button() == Qt::LeftButton ) {
      bool status = true;
      Qt::Orientation o;
      switch ( cursor().shape() )
	{
	case Qt::SplitHCursor:
	  o = Qt::Vertical;
	  break;
	case Qt::SplitVCursor:
	  o = Qt::Horizontal;
	  break;
	default:
	  status = false;
	  break;
	}
      if ( status ) {
	myResizer = new Resizer( me->globalPos(), o, this );
	myMouseMove = new QMouseEvent( me->type(), me->pos(), me->globalPos(),
				       me->button(), me->buttons(), me->modifiers() );
      }
    }
  }

  return ok;
}

/*!
  \brief FullScreenAllowed flag allowed dump in the main window geometry 
         Qt::WindowFullScreen parameter.
  \return \c FullScreenAllowed flag.
*/
bool QtxMainWindow::isFullScreenAllowed() const {
  return myFullScreenAllowed;
}


/*!
  \brief Set FullScreenAllowed flag.
         The default value is true.
  \param f value of the FullScreenAllowed flag.
*/
void QtxMainWindow::setFullScreenAllowed( const bool f ) {
    myFullScreenAllowed = f;
}

/*!
  \brief MinimizeAllowed flag allowed dump in the main window geometry 
         Qt::WindowMinimized parameter.
  \return \c MinimizeAllowed flag.
*/
bool QtxMainWindow::isMinimizeAllowed() const {
  return myMinimizeAllowed;
}


/*!
  \brief Set MinimizeAllowed flag.
         The default value is true.
  \param f value of the MinimizeAllowed flag.
*/
void QtxMainWindow::setMinimizeAllowed( const bool f ) {
    myMinimizeAllowed = f;
}
