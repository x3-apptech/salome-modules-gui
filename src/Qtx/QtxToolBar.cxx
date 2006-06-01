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
// File:      QtxToolBar.cxx
// Author:    Sergey TELKOV

#include "QtxToolBar.h"

#include <qlayout.h>
#include <qpixmap.h>
#include <qdockarea.h>
#include <qobjectlist.h>
#include <qmainwindow.h>
#include <qapplication.h>

/*!
    Class: QtxToolBar::Watcher [Internal]
    Descr: Internal object with event filter.
*/

class QtxToolBar::Watcher : public QObject
{
public:
  Watcher( QtxToolBar* );

  void         shown( QtxToolBar* );
  void         hided( QtxToolBar* );

  virtual bool eventFilter( QObject*, QEvent* );

protected:
  virtual void customEvent( QCustomEvent* );

private:
  enum { Install = QEvent::User, Update };

private:
  void         installFilters();

  void         showContainer();
  void         hideContainer();

  void         updateIcon();
  void         updateCaption();
  void         updateVisibility();

private:
  QtxToolBar*  myCont;
  bool         myState;
  bool         myEmpty;
  bool         myVisible;
};

/*!
  Constructor
*/
QtxToolBar::Watcher::Watcher( QtxToolBar* cont )
: QObject( cont ),
myCont( cont ),
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
bool QtxToolBar::Watcher::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myCont && e->type() == QEvent::ChildInserted )
    QApplication::postEvent( this, new QCustomEvent( Install ) );

  if ( o != myCont && e->type() == QEvent::IconChange )
    updateIcon();

  if ( o != myCont && e->type() == QEvent::CaptionChange )
    updateCaption();

  bool updVis = ( o != myCont && ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ||
                                   e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) ) ||
                ( o == myCont && ( e->type() == QEvent::ChildRemoved || e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ) );

  if ( updVis )
  {
    QtxToolBar* cont = myCont;
    myCont = 0;
    QApplication::sendPostedEvents( this, Update );
    myCont = cont;
    QApplication::postEvent( this, new QCustomEvent( Update ) );
  }

  return false;
}

/*!
  Sets internal visibility state to true
*/
void QtxToolBar::Watcher::shown( QtxToolBar* tb )
{
  if ( tb != myCont )
    return;

  myVisible = true;
}

/*!
  Sets internal visibility state to false
*/
void QtxToolBar::Watcher::hided( QtxToolBar* tb )
{
  if ( tb != myCont )
    return;

  myVisible = false;
}

/*!
  Shows corresponding QtxToolBar
*/
void QtxToolBar::Watcher::showContainer()
{
  if ( !myCont )
    return;

  QtxToolBar* cont = myCont;
  myCont = 0;
  cont->show();
  myCont = cont;
}

/*!
  Hides corresponding QtxToolBar
*/
void QtxToolBar::Watcher::hideContainer()
{
  if ( !myCont )
    return;

  QtxToolBar* cont = myCont;
  myCont = 0;
  cont->hide();
  myCont = cont;
}

/*!
  Event handler of custom events
*/
void QtxToolBar::Watcher::customEvent( QCustomEvent* e )
{
  switch ( e->type() )
  {
  case Install:
    installFilters();
    updateIcon();
    updateCaption();
  case Update:
    updateVisibility();
  }
}

/*!
  Installs event filters
*/
void QtxToolBar::Watcher::installFilters()
{
  if ( !myCont )
    return;

  const QObjectList* objList = myCont->children();
  if ( !objList )
    return;

  for ( QObjectListIt it( *objList ); it.current(); ++it )
  {
    if ( it.current()->isWidgetType() &&
         qstrcmp( "qt_dockwidget_internal", it.current()->name() ) )
      it.current()->installEventFilter( this );
  }
}

/*!
  Update visibility state
*/
void QtxToolBar::Watcher::updateVisibility()
{
  if ( !myCont )
    return;

  bool vis = false;

  const QObjectList* objList = myCont->children();
  if ( objList )
  {
    for ( QObjectListIt it( *objList ); it.current() && !vis; ++it )
    {
      if ( !it.current()->isWidgetType() ||
           !qstrcmp( "qt_dockwidget_internal", it.current()->name() ) )
        continue;

      QWidget* wid = (QWidget*)it.current();
      vis = wid->isVisibleTo( wid->parentWidget() );
    }
  }

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
  Updates icon
*/
void QtxToolBar::Watcher::updateIcon()
{
  if ( !myCont || !myCont->widget() )
    return;
  
  const QPixmap* ico = myCont->widget()->icon();
  myCont->setIcon( ico ? *ico : QPixmap() );
}

/*!
  Updates caption
*/
void QtxToolBar::Watcher::updateCaption()
{
  if ( myCont && myCont->widget() && !myCont->widget()->caption().isNull() )
    myCont->setCaption( myCont->widget()->caption() );
}

/*!
  Constructor
*/
QtxToolBar::QtxToolBar( const bool watch, const QString& label, QMainWindow* main,
                        QWidget* parent, bool newLine, const char* name, WFlags f )
: QToolBar( label, main, parent, newLine, name, f ),
myWatcher( 0 ),
myStretch( false )
{
  if ( watch )
    myWatcher = new Watcher( this );
}

/*!
  Constructor
*/
QtxToolBar::QtxToolBar( const QString& label, QMainWindow* main,
                        QWidget* parent, bool newLine, const char* name, WFlags f )
: QToolBar( label, main, parent, newLine, name, f ),
myWatcher( 0 ),
myStretch( false )
{
}

/*!
  Constructor
*/
QtxToolBar::QtxToolBar( const bool watch, QMainWindow* main, const char* name )
: QToolBar( main, name ),
myWatcher( 0 ),
myStretch( false )
{
  if ( watch )
    myWatcher = new Watcher( this );
}

/*!
  Constructor
*/
QtxToolBar::QtxToolBar( QMainWindow* main, const char* name )
: QToolBar( main, name ),
myWatcher( 0 ),
myStretch( false )
{
}

/*!
  Destructor
*/
QtxToolBar::~QtxToolBar()
{
}

/*!
  Change the toolbar's main widget
  \param wid - new main widget
*/
void QtxToolBar::setWidget( QWidget* wid )
{
  if ( wid )
    wid->reparent( this, QPoint( 0, 0 ), wid->isVisibleTo( wid->parentWidget() ) );

  QToolBar::setWidget( wid );

  if ( !boxLayout() )
    return;

  for ( QLayoutIterator it = boxLayout()->iterator(); it.current(); ++it )
  {
    if ( it.current()->widget() == wid )
    {
      it.deleteCurrent();
      break;
    }
  }
}

/*!
  \return true if toolbar is stretchable
*/
bool QtxToolBar::isStretchable() const
{
  return myStretch;
}

/*!
  Sets stretchable state of toolbar
  \param on - new state
*/
void QtxToolBar::setStretchable( const bool on )
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
QSize QtxToolBar::sizeHint() const
{
  QSize sz = QToolBar::sizeHint();

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
QSize QtxToolBar::minimumSizeHint() const
{
  QSize sz = QToolBar::minimumSizeHint();

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
  Shows toolbar
*/
void QtxToolBar::show()
{
  if ( myWatcher )
    myWatcher->shown( this );

  QToolBar::show();
}

/*!
  Hides toolbar
*/
void QtxToolBar::hide()
{
  if ( myWatcher )
    myWatcher->hided( this );

  QToolBar::hide();
}
