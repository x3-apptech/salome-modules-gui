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
// File:      QtxDockWindow.cxx
// Author:    Sergey TELKOV

#include "QtxDockWindow.h"

#include <qlayout.h>
#include <qpixmap.h>
#include <qdockarea.h>
#include <qmainwindow.h>
#include <qapplication.h>

/*!
  \class QtxDockWindow::Watcher [Internal]
  Internal object with event filter.
*/
class QtxDockWindow::Watcher : public QObject
{
public:
  Watcher( QtxDockWindow* );

  void           shown( QtxDockWindow* );
  void           hided( QtxDockWindow* );

  virtual bool   eventFilter( QObject*, QEvent* );

protected:
  virtual void   customEvent( QCustomEvent* );

private:
  void           installFilters();

  void           showContainer();
  void           hideContainer();

  void           updateIcon();
  void           updateCaption();
  void           updateVisibility();

private:
  QtxDockWindow* myCont;
  bool           myState;
  bool           myEmpty;
  bool           myVisible;
};

/*!
  Constructor
*/
QtxDockWindow::Watcher::Watcher( QtxDockWindow* cont )
: QObject( cont ), myCont( cont ),
myState( true ),
myEmpty( true )
{
  if ( myCont->mainWindow() )
    myState = myCont->mainWindow()->appropriate( myCont );

  myCont->installEventFilter( this );
  myVisible = myCont->isVisibleTo( myCont->parentWidget() );

  installFilters();
}

/*!
  Custom event filter
*/
bool QtxDockWindow::Watcher::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myCont &&
       ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ||
         e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ||
         e->type() == QEvent::ChildInserted ) )
    QApplication::postEvent( this, new QCustomEvent( QEvent::User ) );

  if ( o != myCont && e->type() == QEvent::IconChange )
    updateIcon();

  if ( o != myCont && e->type() == QEvent::CaptionChange )
    updateCaption();

  if ( ( o != myCont && ( e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) ) ||
       ( o == myCont && ( e->type() == QEvent::ChildRemoved ) ) ||
       ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ) )
    updateVisibility();

  return false;
}

/*!
  Sets internal status to shown
*/
void QtxDockWindow::Watcher::shown( QtxDockWindow* dw )
{
  if ( dw != myCont )
    return;

  myVisible = true;
}

/*!
  Sets internal status to hidden
*/
void QtxDockWindow::Watcher::hided( QtxDockWindow* dw )
{
  if ( dw != myCont )
    return;

  myVisible = false;
}

/*!
  Shows corresponding dock window
*/
void QtxDockWindow::Watcher::showContainer()
{
  if ( !myCont )
    return;

  QtxDockWindow* cont = myCont;
  myCont = 0;
  cont->show();
  myCont = cont;
}

/*!
  Hides corresponding dock window
*/
void QtxDockWindow::Watcher::hideContainer()
{
  if ( !myCont )
    return;

  QtxDockWindow* cont = myCont;
  myCont = 0;
  cont->hide();
  myCont = cont;
}

/*!
  Event filter of custom events
*/
void QtxDockWindow::Watcher::customEvent( QCustomEvent* e )
{
  installFilters();

  updateIcon();
  updateCaption();
  updateVisibility();
}

/*!
  Installs this object as event filter to all widgets inside corresponding main window
*/
void QtxDockWindow::Watcher::installFilters()
{
  if ( !myCont )
    return;

  QBoxLayout* bl = myCont->boxLayout();
  if ( !bl )
    return;

  for ( QLayoutIterator it = bl->iterator(); it.current(); ++it )
  {
    if ( it.current()->widget() )
      it.current()->widget()->installEventFilter( this );
  }
}

/*!
  Updates visibility of all widgets inside corresponding main window
*/
void QtxDockWindow::Watcher::updateVisibility()
{
  if ( !myCont )
    return;

  QBoxLayout* bl = myCont->boxLayout();
  if ( !bl )
    return;

  bool vis = false;
  for ( QLayoutIterator it = bl->iterator(); it.current() && !vis; ++it )
    vis = it.current()->widget() && it.current()->widget()->isVisibleTo( myCont );

  QMainWindow* mw = myCont->mainWindow();
  if ( mw && myEmpty == vis )
  {
    myEmpty = !vis;
    if ( !myEmpty )
      mw->setAppropriate( myCont, myState );
    else
    {
      myState = mw->appropriate( myCont );
      mw->setAppropriate( myCont, false );
    }
  }

  vis = !myEmpty && myVisible;
  if ( vis != myCont->isVisibleTo( myCont->parentWidget() ) )
    vis ? showContainer() : hideContainer();
}

/*!
  Updates icon of corresponding main window
*/
void QtxDockWindow::Watcher::updateIcon()
{
  if ( !myCont || !myCont->widget() )
    return;
  
  const QPixmap* ico = myCont->widget()->icon();
  myCont->setIcon( ico ? *ico : QPixmap() );
}

/*!
  Updates caption of corresponding main window
*/
void QtxDockWindow::Watcher::updateCaption()
{
  if ( myCont && myCont->widget() && !myCont->widget()->caption().isNull() )
    myCont->setCaption( myCont->widget()->caption() );
}

/*!
  Constructor
*/
QtxDockWindow::QtxDockWindow( Place p, QWidget* parent, const char* name, WFlags f )
: QDockWindow( p, parent, name, f ),
myWatcher( 0 ),
myStretch( false )
{
}

/*!
  Constructor
*/
QtxDockWindow::QtxDockWindow( const bool watch, QWidget* parent, const char* name, WFlags f )
: QDockWindow( InDock, parent, name, f ),
myWatcher( 0 ),
myStretch( false )
{
  if ( watch )
    myWatcher = new Watcher( this );
}

/*!
  Constructor
*/
QtxDockWindow::QtxDockWindow( QWidget* parent, const char* name, WFlags f )
: QDockWindow( InDock, parent, name, f ),
myWatcher( 0 ),
myStretch( false )
{
}

/*!
  Destructor
*/
QtxDockWindow::~QtxDockWindow()
{
}

/*!
  Sets the dock window's main widget
  \param wid - new main widget
*/
void QtxDockWindow::setWidget( QWidget* wid )
{
  if ( wid )
  {
    if ( wid->parentWidget() != this )
      wid->reparent( this, QPoint( 0, 0 ), wid->isVisibleTo( wid->parentWidget() ) );
    if ( myWatcher )
    {
      setCaption( wid->caption() );
      if ( wid->icon() )
        setIcon( *wid->icon() );
    }
  }

  QDockWindow::setWidget( wid );
}

/*!
  \return true if the dock window is stretchable
*/
bool QtxDockWindow::isStretchable() const
{
  return myStretch;
}

/*!
  Sets the dock window "stretchable" state
  \param on - new state
*/
void QtxDockWindow::setStretchable( const bool on )
{
  if ( myStretch == on )
    return;

  myStretch = on;

  boxLayout()->setStretchFactor( widget(), myStretch ? 1 : 0 );

  if ( myStretch != isHorizontalStretchable() ||
       myStretch != isVerticalStretchable() )
  {
	  if ( orientation() == Horizontal )
	    setHorizontalStretchable( myStretch );
	  else
	    setVerticalStretchable( myStretch );
  }
}

/*!
  \return the recommended size for the widget
*/
QSize QtxDockWindow::sizeHint() const
{
  QSize sz = QDockWindow::sizeHint();

  if ( place() == InDock && isStretchable() && area() )
  {
    if ( orientation() == Horizontal )
      sz.setWidth( area()->width() );
    else
      sz.setHeight( area()->height() );
  }

  return sz;
}

/*!
  \return the recommended minimum size for the widget
*/
QSize QtxDockWindow::minimumSizeHint() const
{
  QSize sz = QDockWindow::minimumSizeHint();

  if ( orientation() == Horizontal )
	  sz = QSize( 0, QDockWindow::minimumSizeHint().height() );
  else
    sz = QSize( QDockWindow::minimumSizeHint().width(), 0 );

  if ( place() == InDock && isStretchable() && area() )
  {
    if ( orientation() == Horizontal )
      sz.setWidth( area()->width() );
    else
      sz.setHeight( area()->height() );
  }

  return sz;
}

/*!
  \return corresponding main window
*/
QMainWindow* QtxDockWindow::mainWindow() const
{
  QMainWindow* mw = 0;

  QWidget* wid = parentWidget();
  while ( !mw && wid )
  {
    if ( wid->inherits( "QMainWindow" ) )
      mw = (QMainWindow*)wid;
    wid = wid->parentWidget();
  }

  return mw;
}

/*!
  Shows window
*/
void QtxDockWindow::show()
{
  if ( myWatcher )
    myWatcher->shown( this );

  QDockWindow::show();
}

/*!
  Hides window
*/
void QtxDockWindow::hide()
{
  if ( myWatcher )
    myWatcher->hided( this );

  QDockWindow::hide();
}
