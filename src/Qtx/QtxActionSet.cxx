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

// File:      QtxActionSet.cxx
// Author:    Sergey TELKOV
//
#include "QtxActionSet.h"

#include <QApplication>

/*!
  \class QtxActionSet
  \brief An action class which is represented in the menu bar (or toolbar) as
  a group of items (which can be customized).

  Example: Window menu in the MDI application with menu items:
  - Cascade
  - Tile vertically
  - Tile horizontally
  - <separator>
  - Window1
  - Window2
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
QtxActionSet::QtxActionSet( QObject* parent )
: QtxAction( parent )
{
  connect( this, SIGNAL( changed() ), this, SLOT( onChanged() ) );

  setVisible( false );
}

/*!
  \brief Destructor.
*/
QtxActionSet::~QtxActionSet()
{
}

/*!
  \brief Get list of child actions.
  \return list of assigned actions
*/
QList<QAction*> QtxActionSet::actions() const
{
  return mySet;
}

/*!
  \brief Assign child actions.
  \param lst list of actions
*/
void QtxActionSet::setActions( const QList<QAction*>& lst )
{
  for ( ActionList::iterator it = mySet.begin(); it != mySet.end(); ++it )
  {
    if ( !lst.contains( *it ) )
      delete *it;
  }

  mySet.clear();

  insertActions( lst );
}

/*!
  \brief Insert actions at the specified position.
  \param lst list of actions
  \param index position in the action list (if < 0, items are appended to the end of list)
*/
void QtxActionSet::insertActions( const QList<QAction*>& lst, const int index )
{
  int idx = qMin( index < 0 ? mySet.count() : index, mySet.count() );

  for ( QList<QAction*>::const_iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QAction* a = *it;
    int ident = generateId();

    a->setParent( this );
    mySet.insert( idx++, a );
    a->setData( ident );

    connect( a, SIGNAL( triggered( bool ) ), this, SLOT( onActionTriggered( bool ) ) );
  }

  updateAction();
}

/*!
  \brief Insert action at the specified position.

  If \a id < 0, it is generated automatically.

  \param a action being inserted
  \param id action ID
  \param index position in the action list (if < 0, item is appended to the end of list)
  \return action identifier
*/
int QtxActionSet::insertAction( QAction* a, const int id, const int index )
{
  if ( !a )
    return -1;

  int ident = id < 0 ? generateId() : id;
  int idx = qMin( index < 0 ? mySet.count() : index, mySet.count() );

  a->setParent( this );
  mySet.insert( idx, a );
  a->setData( ident );

  connect( a, SIGNAL( triggered( bool ) ), this, SLOT( onActionTriggered( bool ) ) );

  actionAdded( a );

  updateAction();

  return ident;
}

/*!
  \brief Insert action at the specified position.

  If \a id < 0, it is generated automatically.

  \param txt action text
  \param id action ID
  \param index position in the action list (if < 0, item is appended to the end of list)
  \return action identifier
*/
int QtxActionSet::insertAction( const QString& txt, const int id, const int index )
{
  return insertAction( new QtxAction( txt, txt, 0, this ), id, index );
}

/*!
  \brief Insert action at the specified position.

  If \a id < 0, it is generated automatically.

  \param txt action text
  \param icon action icon
  \param id action ID
  \param index position in the action list (if < 0, item is appended to the end of list)
  \return action identifier
*/
int QtxActionSet::insertAction( const QString& txt, const QIcon& icon, const int id, const int index )
{
  return insertAction( new QtxAction( txt, icon, txt, 0, this ), id, index );
}

/*!
  \brief Remove specified action.

  An action is removed from the action list and destroyed.

  \param a action to be removed.
*/
void QtxActionSet::removeAction( QAction* a )
{
  if ( !mySet.contains( a ) )
    return;

  mySet.removeAll( a );
  actionRemoved( a );
  delete a;
}

/*!
  \brief Remove specified action.

  An action is removed from the action list and destroyed.

  \param id action identifier
*/
void QtxActionSet::removeAction( const int id )
{
  removeAction( action( id ) );
}

/*!
  \brief Remove all actions.

  An actions list is cleared and all actions are destroyed.
*/
void QtxActionSet::clear()
{
  qDeleteAll( mySet );
  mySet.clear();

  updateAction();
}

/*!
  \brief Called when action is changed.

  Update action state.
*/
void QtxActionSet::onChanged()
{
  QList<QWidget*> lst = createdWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
    (*it)->setEnabled( isEnabled() );

  if ( !isVisible() || !isEmptyAction() )
    return;

  bool block = signalsBlocked();
  blockSignals( true );
  setVisible( false );
  blockSignals( block );
}

/*!
  \brief Called when some action is activated by the user.
  \param on toggled state (not used)
*/
void QtxActionSet::onActionTriggered( bool /*on*/ )
{
  QAction* a = ::qobject_cast<QAction*>( sender() );
  if ( !a )
    return;

  int id = actionId( a );
  if ( id != -1 )
    emit triggered( id );
  emit triggered( a );
}

/*!
  \brief Called when this action set is added to the menu bar (or toolbar).
  \param w widget this action set is added to
*/
void QtxActionSet::addedTo( QWidget* w )
{
  QtxAction::addedTo( w );

  updateAction( w );
}

/*!
  \brief Called when this action set is removed from the menu bar (or toolbar).
  \param w widget this action set is removed from
*/
void QtxActionSet::removedFrom( QWidget* w )
{
  QtxAction::removedFrom( w );

  updateAction( w );
}

/*!
  \brief Get action by specified identifier.
  \param id action ID
  \return action or 0 if it is not found
*/
QAction* QtxActionSet::action( int id ) const
{
  QAction* a = 0;
  for ( ActionList::const_iterator it = mySet.begin(); it != mySet.end() && !a; ++it )
  {
    if ( actionId( *it ) == id )
      a = *it;
  }
  return a;
}

/*!
  \brief Get action identifier for the action.
  \param a action
  \return action ID or -1 if it is not found
*/
int QtxActionSet::actionId( QAction* a ) const
{
  int id = -1;
  if ( a && a->data().canConvert( QVariant::Int ) )
    id = a->data().toInt();
  return id;
}

/*!
  \brief Set action identifier for the action.
  \param a action
  \param id new action ID
*/
void QtxActionSet::setActionId( QAction* a, const int id )
{
  if ( !a || id == -1 )
    return;

  a->setData( id );
}

/*!
  \brief Notify that action was added
*/
void QtxActionSet::actionAdded( QAction* )
{
}

/*!
  \brief Notify that action was removed
*/
void QtxActionSet::actionRemoved( QAction* )
{
}

/*!
  \brief Getneration unique action identifier
  \return generation action ID
*/
int QtxActionSet::generateId() const
{
  QMap<int, int> map;
  for ( ActionList::const_iterator it = mySet.begin(); it != mySet.end(); ++it )
    map.insert( (*it)->data().toInt(), 0 );

  int id = -2;
  while ( map.contains( id ) )
    id--;

  return id;
}

/*!
  \brief Update action set.
*/
void QtxActionSet::updateAction()
{
  QList<QWidget*> lst = associatedWidgets();
  for ( QList<QWidget*>::iterator it = lst.begin(); it != lst.end(); ++it )
    updateAction( *it );
}

/*!
  \brief Update action set for the specified widget.
  \param w a widget this action is added to
*/
void QtxActionSet::updateAction( QWidget* w )
{
  if ( !w )
    return;

  for ( ActionList::iterator it = mySet.begin(); it != mySet.end(); ++it )
    w->removeAction( *it );

  if ( !w->actions().contains( this ) )
    return;

  QAction* first = 0;
  for ( int i = 0; i < mySet.count(); i++ )
  {
    QAction* a = mySet.at( i );
    if ( !first )
      first = a;
    w->insertAction( this, a );
  }
  if ( first )
  {
    QApplication::instance()->removeEventFilter( this );

    w->insertAction( first, this );

    QApplication::instance()->installEventFilter( this );
  }
}

/*!
  \brief Check if the action itself should be invisible
  (only child action are shown)
  \return \c true if the action itself should be visible
*/
bool QtxActionSet::isEmptyAction() const
{
  return true;
}

/*!
  \fn void QtxActionSet::triggered( int id );
  \brief Emitted when some child action is activated by the user.
  \param action ID
*/

/*!
  \fn void QtxActionSet::triggered( QAction* a );
  \brief Emitted when some child action is activated by the user.
  \param a action being activated
*/
