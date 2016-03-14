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

// File:      QtxToolBar.cxx
// Author:    Sergey TELKOV
//
#include "QtxToolBar.h"

#include <QAction>
#include <QMainWindow>
#include <QApplication>

/*!
  \class QtxToolBar::Watcher
  \internal
  \brief Internal class which goal is to watch parent toolbar state changing.
*/

class QtxToolBar::Watcher : public QObject
{
public:
  Watcher( QtxToolBar* );

  void         shown( QtxToolBar* );
  void         hidden( QtxToolBar* );

  virtual bool eventFilter( QObject*, QEvent* );

  bool           isEmpty() const;
  bool           isVisible() const;

protected:
  virtual void customEvent( QEvent* );

private:
  enum { Install = QEvent::User, Update };

private:
  void         installFilters();

  void         showContainer();
  void         hideContainer();

  void         updateVisibility();

  void         setEmpty( const bool );
  void         setVisible( const bool );

private:
  QtxToolBar*  myCont;
  bool         myState;
  bool         myEmpty;
};

/*!
  \brief Constructor.
  \param cont toolbar to be watched
*/
QtxToolBar::Watcher::Watcher( QtxToolBar* cont )
: QObject( cont ),
  myCont( cont ),
  myState( true ),
  myEmpty( false )
{
  setVisible( myCont->isVisibleTo( myCont->parentWidget() ) );

  myCont->installEventFilter( this );

  installFilters();
}

/*!
  \brief Custom event filter.
  \param o event receiver object
  \param e event sent to object
  \return \c true if further event processing should be stopped
*/
bool QtxToolBar::Watcher::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myCont && e->type() == QEvent::ChildAdded )
    QApplication::postEvent( this, new QEvent( (QEvent::Type)Install ) );

  bool updVis = ( o != myCont && ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ||
                                   e->type() == QEvent::Hide || e->type() == QEvent::HideToParent ) ) ||
                ( o == myCont && ( e->type() == QEvent::ChildAdded   || e->type() == QEvent::ChildRemoved || e->type() == QEvent::Show ||
                                   e->type() == QEvent::ShowToParent || e->type() == QEvent::Hide ||
				   e->type() == QEvent::HideToParent || e->type() == QEvent::ActionAdded ||
                                   e->type() == QEvent::ActionRemoved ) );

  if ( updVis )
  {
    QtxToolBar* cont = myCont;
    myCont = 0;
    QApplication::sendPostedEvents( this, Update );
    myCont = cont;
    QApplication::postEvent( this, new QEvent( (QEvent::Type)Update ) );
  }

  return false;
}

/*!
  \brief Set internal status to "shown"
  \param tb toolbar
*/
void QtxToolBar::Watcher::shown( QtxToolBar* tb )
{
  if ( tb != myCont )
    return;

  setVisible( true );
}

/*!
  \brief Set internal status to "hidden"
  \param tb toolbar
*/
void QtxToolBar::Watcher::hidden( QtxToolBar* tb )
{
  if ( tb != myCont )
    return;

  setVisible( false );
}

bool QtxToolBar::Watcher::isEmpty() const
{
  return myEmpty;
}

bool QtxToolBar::Watcher::isVisible() const
{
  bool vis = false;
  if ( myCont && myCont->toggleViewAction() )
    vis = myCont->toggleViewAction()->isChecked();
  return vis;
}

void QtxToolBar::Watcher::setEmpty( const bool on )
{
  myEmpty = on;
}

void QtxToolBar::Watcher::setVisible( const bool on )
{
  if ( !myCont || !myCont->toggleViewAction() )
    return;

  bool block = myCont->toggleViewAction()->signalsBlocked();
  myCont->toggleViewAction()->blockSignals( true );
  myCont->toggleViewAction()->setChecked( on );
  myCont->toggleViewAction()->blockSignals( block );
}

/*!
  \brief Show the toolbar being watched
*/
void QtxToolBar::Watcher::showContainer()
{
  if ( !myCont )
    return;

  bool vis = isVisible();

  QtxToolBar* cont = myCont;
  myCont = 0;
  cont->show();
  myCont = cont;

  setVisible( vis );
}

/*!
  \brief Hide the toolbar being watched
*/
void QtxToolBar::Watcher::hideContainer()
{
  if ( !myCont )
    return;

  bool vis = isVisible();

  QtxToolBar* cont = myCont;
  myCont = 0;
  cont->hide();
  myCont = cont;

  setVisible( vis );
}

/*!
  \brief Proces custom events.
  \param e custom event
*/
void QtxToolBar::Watcher::customEvent( QEvent* e )
{
  switch ( e->type() )
  {
  case Install:
    installFilters();
    break;
  case Update:
    updateVisibility();
    break;
  default:
    break;
  }
}

/*!
  \brief Install this object as event dilter to all children widgets
         of the toolbar being watched.
*/
void QtxToolBar::Watcher::installFilters()
{
  if ( !myCont )
    return;

  const QObjectList& objList = myCont->children();
  for ( QObjectList::const_iterator it = objList.begin(); it != objList.end(); ++it )
  {
    if ( (*it)->isWidgetType() && qstrcmp( "qt_dockwidget_internal", (*it)->objectName().toLatin1() ) )
      (*it)->installEventFilter( this );
  }
}

/*!
  \brief Update visibility state of all children widgets of the toolbar
         being watched.
*/
void QtxToolBar::Watcher::updateVisibility()
{
  if ( !myCont )
    return;

  bool vis = false;
  QList<QAction*> actList = myCont->actions();
  for ( QList<QAction*>::const_iterator it = actList.begin(); it != actList.end() && !vis; ++it )
  {
    if ( (*it)->isSeparator() )
      continue;

    vis = (*it)->isVisible();
  }

  QMainWindow* mw = myCont->mainWindow();
  bool empty = isEmpty();
  if ( mw && empty == vis )
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

  vis = (!isEmpty() && isVisible());
  if ( vis != myCont->isVisibleTo( myCont->parentWidget() ) )
    vis ? showContainer() : hideContainer();
}

/*!
  \class QtxToolBar
  \brief Enhanced toolbar class.
*/

/*!
  \brief Constructor.
  \param watch if \c true the event filter is installed to watch toolbar state changes 
         to update it properly
  \param label toolbar title
  \param parent parent widget
*/
QtxToolBar::QtxToolBar( const bool watch, const QString& label, QWidget* parent )
: QToolBar( label, parent ),
  myWatcher( 0 ),
  myStretch( false )
{
  if ( watch )
    myWatcher = new Watcher( this );

  if ( QMainWindow* mw = ::qobject_cast<QMainWindow*>( parent ) )
    mw->addToolBar( this );
}

/*!
  \brief Constructor.
  \param label toolbar title
  \param parent parent widget
*/
QtxToolBar::QtxToolBar( const QString& label, QWidget* parent )
: QToolBar( label, parent ),
  myWatcher( 0 ),
  myStretch( false )
{
  if ( QMainWindow* mw = ::qobject_cast<QMainWindow*>( parent ) )
    mw->addToolBar( this );
}

/*!
  \brief Constructor.
  \param watch if \c true the event filter is installed to watch toolbar state changes 
         to update it properly
  \param parent parent widget
*/
QtxToolBar::QtxToolBar( const bool watch, QWidget* parent )
: QToolBar( parent ),
  myWatcher( 0 ),
  myStretch( false )
{
  if ( watch )
    myWatcher = new Watcher( this );

  if ( QMainWindow* mw = ::qobject_cast<QMainWindow*>( parent ) )
    mw->addToolBar( this );
}

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxToolBar::QtxToolBar( QWidget* parent )
: QToolBar( parent ),
  myWatcher( 0 ),
  myStretch( false )
{
  if ( QMainWindow* mw = ::qobject_cast<QMainWindow*>( parent ) )
    mw->addToolBar( this );
}

/*!
  \brief Destructor.
*/
QtxToolBar::~QtxToolBar()
{
}

/*!
  \brief Show/hide the toolbar.
  \param on new visibility state
*/
void QtxToolBar::setVisible( bool visible )
{
  if ( isVisible() != visible ) {
    if ( myWatcher )
      {
	if ( visible )
	  myWatcher->shown( this );
	else
	  myWatcher->hidden( this );
      }
  }

  QToolBar::setVisible( visible );
}

/*!
  \brief Get parent main window.
  \return main window pointer
*/
QMainWindow* QtxToolBar::mainWindow() const
{
  QMainWindow* mw = 0;
  QWidget* wid = parentWidget();
  while ( !mw && wid )
  {
    mw = ::qobject_cast<QMainWindow*>( wid );
    wid = wid->parentWidget();
  }
  return mw;
}

bool QtxToolBar::event( QEvent* e )
{
  if ( e->type() == QEvent::WindowTitleChange && objectName().isEmpty() )
    setObjectName( windowTitle() );

  return QToolBar::event( e );
}
