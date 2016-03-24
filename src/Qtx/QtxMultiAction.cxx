// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxMultiAction.cxx
// Author:    Sergey TELKOV
//
#include "QtxMultiAction.h"

#include <QMenu>
#include <QLayout>
#include <QToolBar>
#include <QPainter>
#include <QHelpEvent>
#include <QToolButton>
#include <QApplication>
#include <QStyleOptionButton>

/*!
  \class QtxMultiAction::Filter
  \brief Waches for the buttons in the popup menu 
  to update the tool buttons state.
  \internal
*/

class QtxMultiAction::Filter : public QObject
{
public:
  //! \brief Constructor
  Filter( QObject* parent ) : QObject( parent ) {}
  //! \brief Destructor
  ~Filter() {}
  //! \brief Process events from the child tool buttons
  bool eventFilter( QObject* o, QEvent* e )
  {
    if ( e->type() == QEvent::Leave ) {
      QToolButton* tb = qobject_cast<QToolButton*>( o );
      if ( tb )
        tb->setDown( false );
    }
    return QObject::eventFilter( o, e );
  }
};

/*!
  \class QtxMultiAction::Menu
  \brief Custom menu to be used with the toolbuttons as drop down list.
  \internal
*/

class QtxMultiAction::Menu : public QMenu
{
public:
  //! \brief Constructor
  Menu( QWidget* parent = 0 ) : QMenu( parent ) {}
  //! \brief Destructor
  ~Menu() {};

protected:
  //! \brief Paint the button
  virtual bool event( QEvent* e )
  {
    bool res = false;
    switch ( e->type() )
    {
    case QEvent::ToolTip:
    case QEvent::WhatsThis:
    case QEvent::QueryWhatsThis:
      {
        QHelpEvent* help = static_cast<QHelpEvent*>( e );
        QWidget* w = QApplication::widgetAt( help->globalPos() );
        if ( w && Qtx::isParent( w, this ) )
        {
          QHelpEvent he( help->type(), w->mapFromGlobal( help->globalPos() ), help->globalPos() );
          QApplication::sendEvent( w, &he );
          res = true;
        }
      }
      break;
    case QEvent::StatusTip:
    case QEvent::WhatsThisClicked:
      if ( parentWidget() )
      {
        QApplication::sendEvent( parentWidget(), e );
        res = true;
      }
      break;
    default:
      res = QMenu::event( e );
      break;
    }
    return res;
  }
};

/*!
  \class QtxMultiAction::Button
  \brief Custom button to be used in the toolbar.
  \internal
*/

class QtxMultiAction::Button : public QToolButton
{
public:
  //! \brief Constructor
  Button( QWidget* parent = 0 ) : QToolButton( parent ) {}
  //! \brief Destructor
  ~Button() {};

protected:
  //! \brief Paint the button
  virtual void paintEvent( QPaintEvent* e )
  {
    QToolButton::paintEvent( e );

    int s = 10;
    int m = -2;
    int w = width();
    int h = height();

    QStyleOptionButton opt;
    opt.initFrom( this );
    QRect rect = opt.rect;
    int x = rect.x(), y = rect.y();
    if ( isDown() )
      opt.rect = QRect( x + w - s - m, y + h - s - m, s, s );
    else
      opt.rect = QRect( x + w - s - m - 1, y + h - s - m - 1, s, s );

    QPainter p( this );
    style()->drawPrimitive( QStyle::PE_IndicatorSpinDown, &opt, &p );
  }
};

/*!
  \class QtxMultiAction
  \brief The class QtxMultiAction implements modifiable action.

  The QtxMultiAction class provides a possibility to assign a set of actions 
  (insertAction() function). The action can be used in the toolbar (and even
  in the menu) to show drop-down menu with the list of the assigned actions.

  Initially the first action from the list becomes current and it is activated
  when the tool button is clicked by the user. If user presses and holds the mouse
  button at the tool button, it shows the popup menu with all the assigned actions.
  When the user selects any action from the popup menu, it becames current.
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
QtxMultiAction::QtxMultiAction( QObject* parent )
: QtxActionSet( parent ),
  myCurrent( 0 )
{
  setVisible( true );
  setMenu( new QMenu( 0 ) );

  connect( this, SIGNAL( triggered( QAction* ) ), this, SLOT( onTriggered( QAction* ) ) );
}

/*!
  \brief Constructor.
  \param txt action menu text
  \param parent parent object
*/
QtxMultiAction::QtxMultiAction( const QString& txt, QObject* parent )
: QtxActionSet( parent ),
  myCurrent( 0 )
{
  setText( txt );
  setVisible( true );
  setMenu( new QMenu( 0 ) );

  connect( this, SIGNAL( triggered( QAction* ) ), this, SLOT( onTriggered( QAction* ) ) );
}

/*!
  \brief Constructor.
  \param ico action menu icon
  \param txt action menu text
  \param parent parent object
*/
QtxMultiAction::QtxMultiAction( const QIcon& ico, const QString& txt, QObject* parent )
: QtxActionSet( parent ),
  myCurrent( 0 )
{
  setIcon( ico );
  setText( txt );
  setVisible( true );
  setMenu( new QMenu( 0 ) );

  connect( this, SIGNAL( triggered( QAction* ) ), this, SLOT( onTriggered( QAction* ) ) );
}

/*!
  \brief Destructor
*/
QtxMultiAction::~QtxMultiAction()
{
  // to avoid memory leak
  if( QMenu* aMenu = menu() )
  {
    delete aMenu;
    aMenu = 0;
  }
}

/*!
  \brief Set current action.
  \param a action to be set current
*/
void QtxMultiAction::setActiveAction( QAction* a )
{
  if ( a && actions().contains( a ) && a != myCurrent && a->isEnabled() )
  {
    myCurrent = a;
    updateAction();
  }
}

/*!
  \brief Get current action.
  \return current action (0 if there is no active action)
*/
QAction* QtxMultiAction::activeAction() const
{
  return myCurrent;
}

/*!
  \brief Called when the user activates the current action 
  (for example by clicking the tool button).
  \param on (not used)
*/
void QtxMultiAction::onClicked( bool /*on*/ )
{
  if ( myCurrent )
    myCurrent->activate( QAction::Trigger );
}

/*!
  \brief Called when user activates any action from the
  dropdown menu.
  \param a action being activated
*/
void QtxMultiAction::onTriggered( QAction* a )
{
  if ( !a )
    return;

  QList<QWidget*> lst = createdWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QToolButton* tb = ::qobject_cast<QToolButton*>( *it );
    if ( tb && tb->menu() )
      tb->menu()->hide();
  }

  if ( myCurrent != a )
  {
    myCurrent = a;
    updateAction();
  }
}

/*!
  \brief Update action.
*/
void QtxMultiAction::updateAction()
{
  QtxActionSet::updateAction();

  QList<QWidget*> lst = createdWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
    updateButton( ::qobject_cast<QToolButton*>( *it ) );
}

/*!
  \brief Update child (popup menu) action.
  \param w widget menu widget
*/
void QtxMultiAction::updateAction( QWidget* w )
{
  if ( !w )
    return;

  if ( w->inherits( "QMenu" ) )
  {
    QtxActionSet::updateAction( menu() );

    QApplication::instance()->removeEventFilter( this );

    menu()->removeAction( this );

    QApplication::instance()->installEventFilter( this );
  }
}

/*!
  \brief Check if the action itself should be invisible
  (only child action are shown)
  \return \c true if the action itself should be visible
*/
bool QtxMultiAction::isEmptyAction() const
{
  return false;
}

/*!
  \brief Create widget to be displayed in the toolbar.
  \param parent parent widget (should be toolbar)
  \return toolbar button
*/
QWidget* QtxMultiAction::createWidget( QWidget* parent )
{
  QToolBar* tb = ::qobject_cast<QToolBar*>( parent );
  if ( !tb )
    return 0;

  QToolButton* w = new QToolButton( tb );
  w->setMenu( new Menu( w ) );
  w->setMouseTracking( true );
  w->setFocusPolicy( Qt::NoFocus );
  w->setIconSize( tb->iconSize() );
  w->setToolButtonStyle( tb->toolButtonStyle() );

  connect( w, SIGNAL( clicked( bool ) ), this, SLOT( onClicked( bool ) ) );
  connect( tb, SIGNAL( iconSizeChanged( const QSize& ) ), w, SLOT( setIconSize( QSize ) ) );
  connect( tb, SIGNAL( toolButtonStyleChanged( Qt::ToolButtonStyle ) ),
           w, SLOT( setToolButtonStyle( Qt::ToolButtonStyle ) ) );

  updateButton( w );
  return w;
}

/*!
  \brief Called when the child action is added to this action.
  \param a child action being added
*/
void QtxMultiAction::actionAdded( QAction* a )
{
  connect( a, SIGNAL( changed() ), this, SLOT( onActionChanged() ) );
  onActionChanged();
}

/*!
  \brief Called when the child action is removed from this action.
  \param a child action being removed
*/
void QtxMultiAction::actionRemoved( QAction* a )
{
  disconnect( a, SIGNAL( changed() ), this, SLOT( onActionChanged() ) );

  if ( myCurrent != a )
    return;

  myCurrent = 0;

  onActionChanged();

  updateAction();
}

/*!
  \brief Update toolbar button.
  \param btn toolbar button
*/
void QtxMultiAction::updateButton( QToolButton* btn )
{
  if ( !btn )
    return;

  btn->setIcon( myCurrent ? myCurrent->icon() : QIcon() );
  btn->setText( myCurrent ? myCurrent->text() : QString() );
  btn->setToolTip( myCurrent ? myCurrent->toolTip() : QString() );
  btn->setStatusTip( myCurrent ? myCurrent->statusTip() : QString() );

  QMenu* pm = btn->menu();
  if ( !pm )
    return;

  pm->clear();
  for ( int i = 0; pm->layout() && i < pm->layout()->count(); i++ )
    delete pm->layout()->widget();

  delete pm->layout();

  QVBoxLayout* vbox = new QVBoxLayout( pm );
  vbox->setMargin( 1 );
  vbox->setSpacing( 0 );
  Filter* filter = new Filter( vbox );
  QList<QAction*> actList = actions();
  for ( QList<QAction*>::iterator itr = actList.begin(); itr != actList.end(); ++itr )
  {
    QToolButton* b = new QToolButton( pm );
    b->setDefaultAction( *itr );
    b->setToolTip( (*itr)->toolTip() );
    b->setStatusTip( (*itr)->statusTip() );
    b->setAutoRaise( true );
    b->setIconSize( btn->iconSize() );
    b->setToolButtonStyle( btn->toolButtonStyle() );
    b->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    b->installEventFilter( filter );
    vbox->addWidget( b );
  }
}

/*!
  \brief Called when any child action is enabled/disabled.
  
  If the current action is disabled, the multi-action switches
  to first found enabled. If all child actions are disabled, the
  action itself is also disabled.
*/
void QtxMultiAction::onActionChanged()
{
  if ( myCurrent && myCurrent->isEnabled() )
    return;

  QList<QAction*> alist = actions();
  QAction* a = 0;
  for ( QList<QAction*>::ConstIterator it = alist.begin(); it != alist.end() && !a; ++it ) {
    if ( (*it)->isEnabled() )
      a = *it;
  }

  if ( a )
    myCurrent = a;
  else
    myCurrent = alist.isEmpty() ? 0 : alist.first();

  setEnabled( myCurrent && myCurrent->isEnabled() );
  updateAction();
}
