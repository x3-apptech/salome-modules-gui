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

// File:      QtxActionGroup.cxx
// Author:    Sergey TELKOV
//
#include "QtxActionGroup.h"

#include "QtxComboBox.h"

#include <QMenu>
#include <QMenuBar>
#include <QActionGroup>

/*!
  \class QtxActionGroup
  \brief The QtxActionGroup class groups actions together.

  QtxActionGroup class operates with a list of actions in the similar way as it does QActionGroup class.
  But in contrast to the Qt 4's class, QtxActrionGroup behaves rather like it was in Qt series 3x.
  For example, it automatically shows exclusive combo box widget when action group is added to the toolbar
  and if \a usesDropDown and \a exclusive flags are both set to \c true.
  
  The setExclusive() function is used to ensure that only one action is active at any moment:
  it should be used with actions which have their \a checkable state set to \c true.

  Action group actions appear as individual menu options and toolbar buttons. For exclusive action
  groups use setUsesDropDown() to display the actions in a subwidget of the toolbar or menu the action group
  is added on.

  Actions can be added to the action group using add() function. Add the action group to the menu or
  toolbar in the same way as for single action - using addAction() method of QMenu or QToolbar class.
*/

/*!
  \brief Constructor

  The created action group is exclusive by default.

  \param parent owner object
  \sa setExclusive()
*/
QtxActionGroup::QtxActionGroup( QObject* parent )
: QtxActionSet( parent ),
  myDropDown( false )
{
  setMenu( new QMenu( 0 ) );
  myActionGroup = new QActionGroup( this );

  connect( myActionGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( onTriggered( QAction* ) ) );
}

/*!
  \brief Constructor
  \param parent owner object
  \param exclusive if \c true only one action in the group will ever be active
  \sa setExclusive()
*/
QtxActionGroup::QtxActionGroup( QObject* parent, const bool exclusive )
: QtxActionSet( parent ),
  myDropDown( false )
{
  setMenu( new QMenu( 0 ) );
  myActionGroup = new QActionGroup( this );
  myActionGroup->setExclusive( exclusive );

  connect( myActionGroup, SIGNAL( triggered( QAction* ) ), this, SIGNAL( selected( QAction* ) ) );
}

/*!
  \brief Destructor.
*/
QtxActionGroup::~QtxActionGroup()
{
}

/*!
  \brief Check if the action group is exclusive
  \return \c true if the action group is exclusive and \c false otherwise
  \sa setExclusive(), setUsesDropDown()
*/
bool QtxActionGroup::isExclusive() const
{
  return myActionGroup->isExclusive();
}

/*!
  \brief Set/clear the action group exclusiveness
  \param on if \c true the action group will be exclusive
  \sa isExclusive(), setUsesDropDown()
*/
void QtxActionGroup::setExclusive( const bool on )
{
  if ( myActionGroup->isExclusive() == on )
    return;

  bool e = isEmptyAction();

  myActionGroup->setExclusive( on );

  if ( e != isEmptyAction() )
    updateType();
}

/*!
  \brief Check if action group should appear in a subwidget of parent widget

  Note: for this option to take into effect, the \a exclusive flag should
  be also set to \c true

  \return \c true if the action group is shown in subwidget
  \sa setUsesDropDown(), setExclusive()
*/
bool QtxActionGroup::usesDropDown() const
{
  return myDropDown;
}

/*!
  \brief Defines a way how the group's actions should be displayed in parent widget 
  action group is added to - as a group of actions or in a subwidget (e.g. in the
  combo box).
  \param on if \c true, action group will be shown in the subwidget
  \sa usesDropDown(), setExclusive()
*/
void QtxActionGroup::setUsesDropDown( const bool on )
{
  if ( myDropDown == on )
    return;

  bool e = isEmptyAction();

  myDropDown = on;

  if ( e != isEmptyAction() )
    updateType();
}

/*!
  \brief Append the specified action into group.
  \a action action to be added to the action group
*/
void QtxActionGroup::add( QAction* a )
{
  insertAction( a );
}

/*!
  \brief Called when some subwidget item is activated by the user.
  \param id item identifier
*/
void QtxActionGroup::onActivated( int id )
{
  const QObject* s = sender();

  QAction* a = action( id );
  if ( !a )
    return;

  if ( a->isChecked() )
    return;

  a->setChecked( true );
  a->trigger();

  QList<QWidget*> lst = createdWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QtxComboBox* cb = ::qobject_cast<QtxComboBox*>( *it );
    if ( cb && cb != s )
      cb->setCurrentId( id );
  }
}

/*!
  \brief Called when some action owned by this action group is activated by the user
  \param a action being activated
*/
void QtxActionGroup::onTriggered( QAction* a )
{
  int id = actionId( a );
  if ( id != -1 ) {
    QList<QWidget*> lst = createdWidgets();
    for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
    {
      QtxComboBox* cb = ::qobject_cast<QtxComboBox*>( *it );
      if ( cb )
        cb->setCurrentId( id );
    }
  }
  
  emit selected( a );
}

/*!
  \brief Enable/disable action group
*/
void QtxActionGroup::setEnabled( bool on )
{
  QtxActionSet::setEnabled( on );
  myActionGroup->setEnabled( on );
}

/*!
  \brief Update action group for the specified widget.
  \param w a widget this action group is added to
*/
void QtxActionGroup::updateAction( QWidget* w )
{
  if ( !::qobject_cast<QMenu*>( w ) && !::qobject_cast<QMenuBar*>( w ) ) {
    QtxComboBox* cb = createdWidget( w );
    if ( !cb )
      QtxActionSet::updateAction( w );
    else
    {
      updateAction( cb );
      
      QList<QAction*> lst = actions();
      for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
        w->removeAction( *it );
    }
  }
  else
  {
    if ( !usesDropDown() ) {
      QtxActionSet::updateAction( w );
    }
    else {
      QList<QAction*> lst = actions();
      for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
        w->removeAction( *it );
    }
  }
}

/*!
  \brief Update action group for the specified combo box.
  \param cb a combo box this action group is added to
*/
void QtxActionGroup::updateAction( QtxComboBox* cb )
{
  if ( !cb )
    return;

  cb->clear();
  cb->setCleared( false );

  QAction* cur = 0;
  QList<QAction*> lst = actions();
  for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QAction* a = *it;
    cb->addItem( a->icon(), a->text() );
    cb->setId( cb->count() - 1, actionId( a ) );
    if ( a->isChecked() )
      cur = a;
  }

  if ( cur )
    cb->setCurrentId( actionId( cur ) );
  else
    cb->setCleared( true );
}

/*!
  \brief Create widget representing action group in the widget
  this action group is added to.
  \param p widget this action group is being added to
  \return new widget representing this action group
*/
QWidget* QtxActionGroup::createWidget( QWidget* p )
{
  if ( ::qobject_cast<QMenu*>( p ) || ::qobject_cast<QMenuBar*>( p ) )
    return 0;

  QtxComboBox* cb = !isEmptyAction() ? new QtxComboBox( p ) : 0;
  if ( cb )
    connect( cb, SIGNAL( activatedId( int ) ), this, SLOT( onActivated( int ) ) );
  return cb;
}

/*!
  \brief Check if the action itself should be invisible
  (only child action are shown)
  \return \c true if the action itself should be visible
*/
bool QtxActionGroup::isEmptyAction() const
{
  return !isExclusive() || !usesDropDown();
}

/*!
  \brief Called when action is added to the action group
  \param a action being added to the action group
*/
void QtxActionGroup::actionAdded( QAction* a )
{
  myActionGroup->addAction( a );
  if ( menu() )
    menu()->addAction( a );
}

/*!
  \brief Called when action is removed from the action group
  \param a action being removed from the action group
*/
void QtxActionGroup::actionRemoved( QAction* a )
{
  myActionGroup->removeAction( a );
  if ( menu() )
    menu()->removeAction( a );
}

/*!
  \brief Internal update
*/
void QtxActionGroup::updateType()
{
  QList<QWidget*> lst = associatedWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QWidget* w = *it;
    QList<QAction*> lst = w->actions();

    int i = lst.indexOf( this );
    w->removeAction( this );

    lst = w->actions();
    w->insertAction( i < lst.count() ? lst.at( i ) : 0, this );
  }
  setVisible( !isEmptyAction() );
}

/*!
  \brief Get combo box created by this action group for the specified widget.
  \param p widget this action group is added to
  \return combo box if it was created for the specified widget or 0 otherwise
*/
QtxComboBox* QtxActionGroup::createdWidget( QWidget* p )
{
  QtxComboBox* cb = 0;
  QList<QWidget*> lst = createdWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end() && !cb; ++it )
  {
    if ( (*it)->parent() == p )
      cb = ::qobject_cast<QtxComboBox*>( *it );
  }
  return cb;
}

/*!
  \fn void QtxActionGroup::selected( QAction* a );
  \brief Emitted when some child action is toggled by the user.
  \param a action being toggled
*/
