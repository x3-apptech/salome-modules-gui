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

// File:      QtxDockWidget.cxx
// Author:    Sergey TELKOV
//
#include "QtxDockWidget.h"

#include <QAction>
#include <QLayout>
#include <QMainWindow>
#include <QResizeEvent>
#include <QApplication>

/*!
  \class QtxDockWidget::Watcher
  \internal
  \brief Internal class which goal is to watch parent dockable widget state changing.
*/

class QtxDockWidget::Watcher : public QObject
{
public:
  Watcher( QtxDockWidget* );

  void           shown( QtxDockWidget* );
  void           hidden( QtxDockWidget* );

  virtual bool   eventFilter( QObject*, QEvent* );

  bool           isEmpty() const;
  bool           isVisible() const;

protected:
  enum { Update = QEvent::User, Remove };

  virtual void   customEvent( QEvent* );

private:
  void           installFilters();

  void           showContainer();
  void           hideContainer();

  void           updateIcon();
  void           updateCaption();
  void           updateVisibility();

  void           setEmpty( const bool );
  void           setVisible( const bool );

private:
  QtxDockWidget* myCont;
  bool           myState;
  bool           myEmpty;
  bool           myBlock;
  bool           myShown;
};

/*!
  \brief Constructor.
  \param cont dockable widget to be watched
*/
QtxDockWidget::Watcher::Watcher( QtxDockWidget* cont )
: QObject( cont ), myCont( cont ),
  myState( true ),
  myEmpty( false ),
  myBlock( false )
{
  myCont->installEventFilter( this );

  installFilters();

  myShown = myCont->isVisibleTo( myCont->parentWidget() );
}

/*!
  \brief Custom event filter.
  \param o event receiver object
  \param e event sent to object
  \return \c true if further event processing should be stopped
*/
bool QtxDockWidget::Watcher::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myCont && ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ||
                        e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) )
  {
    installFilters();
  }

  if ( o == myCont && e->type() == QEvent::ChildAdded )
  {
    QChildEvent* ce = (QChildEvent*)e;
    if ( ce->child()->isWidgetType() )
      ce->child()->installEventFilter( this );

    QApplication::postEvent( this, new QEvent( (QEvent::Type)Update ) );
  }

  if ( o != myCont && e->type() == QEvent::WindowIconChange )
    updateIcon();

  if ( o != myCont && e->type() == QEvent::WindowTitleChange )
    updateCaption();

  if ( o != myCont && ( e->type() == QEvent::HideToParent || e->type() == QEvent::ShowToParent ) )
    updateVisibility();

  if ( o == myCont && e->type() == QEvent::ChildRemoved )
  {
    QApplication::postEvent( this, new QEvent( (QEvent::Type)Remove ) );
  }

  return false;
}

/*!
  \brief Set internal status to "shown"
  \param dw dockable widget
*/
void QtxDockWidget::Watcher::shown( QtxDockWidget* dw )
{
  if ( dw != myCont )
    return;

  setVisible( true );
}

/*!
  \brief Set internal status to "hidden"
  \param dw dockable widget
*/
void QtxDockWidget::Watcher::hidden( QtxDockWidget* dw )
{
  if ( dw != myCont )
    return;

  setVisible( false );
}

bool QtxDockWidget::Watcher::isEmpty() const
{
  return myEmpty;
}

bool QtxDockWidget::Watcher::isVisible() const
{
  return myShown;
}

void QtxDockWidget::Watcher::setEmpty( const bool on )
{
  myEmpty = on;
}

void QtxDockWidget::Watcher::setVisible( const bool on )
{
  myShown = on;
}

/*!
  \brief Show the dock window being watched
*/
void QtxDockWidget::Watcher::showContainer()
{
  if ( !myCont )
    return;

  bool vis = isVisible();

  QtxDockWidget* cont = myCont;
  myCont = 0;
  cont->show();
  myCont = cont;

  setVisible( vis );
}

/*!
  \brief Hide the dock window being watched
*/
void QtxDockWidget::Watcher::hideContainer()
{
  if ( !myCont )
    return;

  bool vis = isVisible();

  QtxDockWidget* cont = myCont;
  myCont = 0;
  cont->hide();
  myCont = cont;

  setVisible( vis );
}

/*!
  \brief Proces custom events.
  \param e custom event (not used)
*/
void QtxDockWidget::Watcher::customEvent( QEvent* e )
{
  if ( e->type() == (QEvent::Type)Update )
  {
    updateIcon();
    updateCaption();
    updateVisibility();
  }
  else if ( myCont && e->type() == (QEvent::Type)Remove && !myCont->widget() )
  {
    myCont->deleteLater();
    myCont = 0;
  }
}

/*!
  \brief Install this object as event filter to all children widgets
         of the dockable widget being watched.
*/
void QtxDockWidget::Watcher::installFilters()
{
  if ( !myCont )
    return;

  QLayout* l = myCont->layout();
  if ( !l )
    return;

  for ( int i = 0; i < (int)l->count(); i++ )
  {
    if ( l->itemAt( i ) && l->itemAt( i )->widget() )
      l->itemAt( i )->widget()->installEventFilter( this );
  }
}

/*!
  \brief Update visibility state of all children widgets of the dockable widget
         being watched.
*/
void QtxDockWidget::Watcher::updateVisibility()
{
  if ( !myCont )
    return;

  bool vis = false;
  if ( myCont->widget() )
    vis = myCont->widget()->isVisibleTo( myCont );
  else
  {
    QLayout* l = myCont->layout();
    if ( l )
    {
      for ( int i = 0; i < (int)l->count() && !vis; i++ )
        vis = l->itemAt( i ) && l->itemAt( i )->widget() && l->itemAt( i )->widget()->isVisibleTo( myCont );
    }
  }

  bool empty = isEmpty();
  if ( empty == vis )
  {
    empty = !vis;
    setEmpty( empty );
    if ( !empty )
      myCont->toggleViewAction()->setVisible( myState );
    else
    {
      myState = myCont->toggleViewAction()->isVisible();
      myCont->toggleViewAction()->setVisible( false );
    }
  }

  vis = !empty && isVisible();
  if ( vis != myCont->isVisibleTo( myCont->parentWidget() ) )
    vis ? showContainer() : hideContainer();
}

/*!
  \brief Update the icon of dockable window being watched
*/
void QtxDockWidget::Watcher::updateIcon()
{
  if ( !myCont || !myCont->widget() || myBlock )
    return;

  myBlock = true;
  myCont->setWindowIcon( myCont->widget()->windowIcon() );
  myBlock = false;
}

/*!
  \brief Update the title of dockable window being watched
*/
void QtxDockWidget::Watcher::updateCaption()
{
  if ( myCont && myCont->widget() && !myCont->widget()->windowTitle().isNull() )
    myCont->setWindowTitle( myCont->widget()->windowTitle() );
}

/*!
  \class QtxDockWidget
  \brief Enhanced dockable widget class.
*/

/*!
  \brief Constructor.
  \param title dockable widget title
  \param parent parent widget
  \param f widget flags
*/
QtxDockWidget::QtxDockWidget( const QString& title, QWidget* parent, Qt::WindowFlags f )
: QDockWidget( title, parent, f ),
  myWatcher( 0 ),
  myOrientation( (Qt::Orientation)-1 )
{
  updateState();
}

/*!
  \brief Constructor.
  \param watch if \c true the event filter is installed to watch wigdet state changes
         to update it properly
  \param parent parent widget
  \param f widget flags
*/
QtxDockWidget::QtxDockWidget( const bool watch, QWidget* parent, Qt::WindowFlags f )
: QDockWidget( parent, f ),
  myWatcher( 0 ),
  myOrientation( (Qt::Orientation)-1 )
{
  if ( watch )
    myWatcher = new Watcher( this );

  updateState();
}

/*!
  \brief Constructor.
  \param parent parent widget
  \param f widget flags
*/
QtxDockWidget::QtxDockWidget( QWidget* parent, Qt::WindowFlags f )
: QDockWidget( parent, f ),
myWatcher( 0 )
{
}

/*!
  \brief Destructor.
*/
QtxDockWidget::~QtxDockWidget()
{
}

/*!
  \brief Get the recommended size for the widget.
  \return recommended dockable widget size
*/
QSize QtxDockWidget::sizeHint() const
{
  QSize sz = QDockWidget::sizeHint();

  // printf( "----------------> QtxDockWidget::sizeHint()\n" );

  return QSize( 500, 100 );

  return sz;
}

/*!
  \brief Get the recommended minimum size for the widget.
  \return recommended dockable widget minimum size
*/
QSize QtxDockWidget::minimumSizeHint() const
{
  QSize sz = QDockWidget::minimumSizeHint();

  return sz;
}

/*!
  \brief Show/hide the dockable window.
  \param on new visibility state
*/
void QtxDockWidget::setVisible( bool on )
{
  updateGeometry();
  if ( widget() )
    widget()->updateGeometry();

  QDockWidget::setVisible( on && ( myWatcher ? !myWatcher->isEmpty() : true )  );

  if ( myWatcher )
  {
    if ( on )
      myWatcher->shown( this );
    else
      myWatcher->hidden( this );
  }
}

/*!
  \brief Process resize event
  \param e event
*/
void QtxDockWidget::resizeEvent( QResizeEvent* e )
{
  QDockWidget::resizeEvent( e );
  updateState();
}

/*!
  \brief Get dockable window orientation.
  \return orientation type
*/
Qt::Orientation QtxDockWidget::orientation() const
{
  QMainWindow* mw = 0;
  QWidget* wid = parentWidget();
  while ( wid && !mw )
  {
    mw = ::qobject_cast<QMainWindow*>( wid );
    wid = wid->parentWidget();
  }

  Qt::Orientation res = (Qt::Orientation)-1;

  if ( !mw )
    return res;

  Qt::DockWidgetArea area = mw->dockWidgetArea( (QtxDockWidget*)this );
  switch ( area )
  {
  case Qt::LeftDockWidgetArea:
  case Qt::RightDockWidgetArea:
    res = Qt::Vertical;
    break;
  case Qt::TopDockWidgetArea:
  case Qt::BottomDockWidgetArea:
    res = Qt::Horizontal;
    break;
  default:
    break;
  }

  return res;
}

/*!
  \brief Update dockable window state.
*/
void QtxDockWidget::updateState()
{
  Qt::Orientation o = orientation();
  if ( myOrientation == o )
    return;

  myOrientation = o;

  emit orientationChanged( myOrientation );
}

/*!
  \fn QtxDockWidget::orientationChanged(Qt::Orientation o)
  \brief Emitted when the dockable window orientation is changed.
  \param o new window orientation
*/
