// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "QtxToolButton.h"
#include "QtxMenu.h"

#include <QKeyEvent>
#include <QWheelEvent>

namespace
{
  bool isSeparator( QAction* a )
  {
    return a->property( "separator" ).toBool();
  }
  void setSeparator( QAction* a )
  {
    a->setProperty( "separator", true );
  }
}

/*!
  \class QtxToolButton
  \brief Drop-down tool button that behaves like a drop-down combo-box.

  In contrast to the standard combo box, QtxToolButton can show drop-down
  menu containing groups of items where each group has a separate title.
*/

/*!
  \brief Constructor.
  \param parent Parent widget.
*/
QtxToolButton::QtxToolButton( QWidget* parent )
  : QToolButton( parent )
{
  setMenu( new QtxMenu( this ) );
  setPopupMode( InstantPopup );
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  connect( this, SIGNAL( triggered( QAction* ) ), this, SLOT( actionTriggered( QAction* ) ) );
}

/*!
  \brief Destructor.
*/
QtxToolButton::~QtxToolButton()
{
}

/*!
  \brief Add an item with the given \a text, and containing the specified \a userData.
  The item is appended to the list of existing items.
  \param text Item's text.
  \param userData Item's data.
  \return Index of just added item.
*/
int QtxToolButton::addItem( const QString& text, const QVariant& userData )
{
  QAction* action = menu()->addAction( text );
  action->setData( userData );
  bool currentChanged = false;
  if ( !menu()->defaultAction() ) {
    menu()->setDefaultAction( action );
    currentChanged = true;
  }
  internalUpdate();
  if ( currentChanged ) emitCurrentChanged( false, true );
  return count()-1;
}

/*!
  \brief Add an item with the given \a icon and \a text, and containing the specified
  \a userData. The item is appended to the list of existing items.
  \param text Item's text.
  \param icon Item's icon.
  \param userData Item's data.
  \return Index of just added item.
*/
int QtxToolButton::addItem( const QIcon& icon, const QString& text, const QVariant& userData )
{
  QAction* action = menu()->addAction( icon, text );
  action->setData( userData );
  bool currentChanged = false;
  if ( !menu()->defaultAction() ) {
    menu()->setDefaultAction( action );
    currentChanged = true;
  }
  internalUpdate();
  if ( currentChanged ) emitCurrentChanged( false, true );
  return count()-1;
}

/*!
  \brief Add items with given \a texts. Each item is appended to the list of existing
  items in turn.
  \param texts Items being added.
*/
void QtxToolButton::addItems( const QStringList& texts )
{
  Q_FOREACH( QString text, texts )
    menu()->addAction( text );
  bool currentChanged = false;
  if ( !menu()->defaultAction() && menu()->actions().count() > 0 ) {
    menu()->setDefaultAction( menu()->actions()[0] );
    currentChanged = true;
  }
  if ( currentChanged ) emitCurrentChanged( false, true );
  internalUpdate();
}

/*!
  \brief Add separator to the end of the items list.
  \param text Separator's text.
  \return Index of just added item.
*/
int QtxToolButton::addSeparator( const QString& text )
{
  (qobject_cast<QtxMenu*>(menu()))->addGroup( text );
  setSeparator( actionAt( count()-1 ) );
  return count()-1;
}

/*!
  \brief Add separator to the end of the items list.
  \param icon Separator's icon.
  \param text Separator's text.
  \return Index of just added item.
*/
int QtxToolButton::addSeparator( const QIcon& icon, const QString& text )
{
  (qobject_cast<QtxMenu*>(menu()))->addGroup( icon, text );
  setSeparator( actionAt( count()-1 ) );
  return count()-1;
}

/*!
  \brief Remove item with given \a index.
  \param index Index of item to be removed.
*/
void QtxToolButton::removeItem( int index )
{
  QAction* action = actionAt( index );
  if ( !action ) return;
  QAction* current = menu()->defaultAction();
  menu()->removeAction( action );
  bool currentChanged = false;
  if ( action == current ) {
    for ( int i = index; i < count(); i++ ) {
      QAction* a = actionAt( i );
      if ( a && !isSeparator( a ) ) {
        menu()->setDefaultAction( a );
        currentChanged = true;
        break;
      }
    }
    for ( int i = index-1; i >= 0; i-- ) {
      QAction* a = actionAt( i );
      if ( a && !isSeparator( a ) ) {
        menu()->setDefaultAction( a );
        currentChanged = true;
        break;
      }
    }
  }
  internalUpdate();
  if ( currentChanged ) emitCurrentChanged( false, true );
}

/*!
  \brief Get the number of items.
  \return Number of items.
*/
int QtxToolButton::count() const
{
  return menu()->actions().count();
}

/*!
  \brief Get data of the current item.
  \return Current item's data (invalid QVariant if list of items is empty
  or current item doesn't have data).
*/
QVariant QtxToolButton::currentData() const
{
  QAction* action = menu()->defaultAction();
  return action == 0 ? QVariant() : action->data();
}

/*!
  \brief Get index of current item.
  \return Current item's index; -1 if list of items is empty or if there's no
  current item.
*/
int QtxToolButton::currentIndex() const
{
  QAction* action = menu()->defaultAction();
  return action == 0 ? -1 : menu()->actions().indexOf( action );
}

/*!
  \brief Get text of current item.
  \return Current item's text; null sting if list of items is empty or
  if there's no current item.
*/
QString QtxToolButton::currentText() const
{
  QAction* action = menu()->defaultAction();
  return action == 0 ? QString() : action->text();
}

/*!
  \brief Get custom data of the item at given \a index.
  \param index Item's index.
  \return Item's data (invalid QVariant if index is out of range).
*/
QVariant QtxToolButton::itemData( int index ) const
{
  QAction* action = actionAt( index );
  return action == 0 ? QVariant() : action->data();
}

/*!
  \brief Get icon of the item at given \a index.
  \param index Item's index.
  \return Item's icon.
*/
QIcon QtxToolButton::itemIcon( int index ) const
{
  QAction* action = actionAt( index );
  return action == 0 ? QIcon() : action->icon();
}

/*!
  \brief Get text of the item at given \a index.
  \param index Item's index.
  \return Item's text.
*/
QString QtxToolButton::itemText( int index ) const
{
  QAction* action = actionAt( index );
  return action == 0 ? QString() : action->text();
}

/*!
  \brief Set custom data of the item at given \a index.
  \param index Item's index.
  \param value Item's data.
*/
void QtxToolButton::setItemData( int index, const QVariant& value )
{
  QAction* action = actionAt( index );
  if ( action ) action->setData( value );
}

/*!
  \brief Set icon of the item at given \a index.
  \param index Item's index.
  \param icon Item's icon.
*/
void QtxToolButton::setItemIcon( int index, const QIcon& icon )
{
  QAction* action = actionAt( index );
  if ( action ) action->setIcon( icon );
  internalUpdate();
}

/*!
  \brief Set text of the item at given \a index.
  \param index Item's index.
  \param text Item's text.
*/
void QtxToolButton::setItemText( int index, const QString& text )
{
  QAction* action = actionAt( index );
  bool currentChanged = false;
  if ( action ) {
    currentChanged = menu()->defaultAction() == action && action->text() != text;
    action->setText( text );
  }
  internalUpdate();
  if ( currentChanged )
    emit currentTextChanged( text );
}

/*!
  \brief Search item with given \a text.
  \param Item's text.
  \return Item's index; -1 if item is not found.
*/
int QtxToolButton::findText( const QString& text )
{
  int index = -1;
  for ( int i = 0; i < count() && index == -1; i++ ) {
    QAction* action = actionAt( i );
    if ( isSeparator( action ) ) continue;
    if ( action->text() == text ) index = i;
  }
  return index;
}

/*!
  \brief Clear widget.
*/
void QtxToolButton::clear()
{
  QAction* action = menu()->defaultAction();
  menu()->clear();
  internalUpdate();
  if ( action ) emitCurrentChanged( false, true );
}

/*!
  \brief Set current item by given \a index.
  \param index Item's index.
*/
void QtxToolButton::setCurrentIndex( int index )
{
  bool currentChanged = false;
  if ( index == -1 ) {
    currentChanged = currentIndex() != -1;
    menu()->setDefaultAction( 0 );
  }
  else if ( index >= count() )
    return;
  else {
    QAction* action = actionAt( index );
    if ( !isSeparator( action ) ) {
      currentChanged = currentIndex() != index;
      menu()->setDefaultAction( action );
    }
  }
  internalUpdate();
  if ( currentChanged ) emitCurrentChanged( false, true );
}

/*!
  \brief Set current item by given \a text.
  \param index Item's index.
*/
void QtxToolButton::setCurrentText( const QString& text )
{
  int index = findText( text );
  if ( index != -1 )
    setCurrentIndex( index );
}

/*!
  \brief Reimplemented from QToolButton::keyPressEvent().
  Process key press event.
  \param e Key press event.
*/
void QtxToolButton::keyPressEvent( QKeyEvent* e )
{
  Move move = NoMove;
  switch ( e->key() ) {
  case Qt::Key_Up:
  case Qt::Key_PageUp:
    move = ( e->modifiers() & Qt::ControlModifier ) ? MoveFirst : MoveUp;
    break;
  case Qt::Key_Down:
    if ( e->modifiers() & Qt::AltModifier ) {
      showMenu();
      return;
    }
    // fall through!
  case Qt::Key_PageDown:
    move = ( e->modifiers() & Qt::ControlModifier ) ? MoveLast : MoveDown;
    break;
  case Qt::Key_Home:
    move = MoveFirst;
    break;
  case Qt::Key_End:
    move = MoveLast;
    break;
  case Qt::Key_F4:
    if ( !e->modifiers() ) {
      showMenu();
      return;
    }
    break;
  case Qt::Key_Space:
    showMenu();
    return;
  case Qt::Key_Enter:
  case Qt::Key_Return:
  case Qt::Key_Escape:
    e->ignore();
    break;
  case Qt::Key_Select:
    showMenu();
    return;
  case Qt::Key_Left:
    move = ( e->modifiers() & Qt::ControlModifier ) ? MoveFirst : MoveUp;
    break;
  case Qt::Key_Right:
    move = ( e->modifiers() & Qt::ControlModifier ) ? MoveLast : MoveDown;
    break;
  default:
    e->ignore();
    break;
  }
  moveIndex( move );
}

/*!
  \brief Reimplemented from QToolButton::wheelEvent().
  Process mouse wheel event.
  \param e Mouse wheel event.
*/
void QtxToolButton::wheelEvent( QWheelEvent* e )
{
  Move move = NoMove;
  if ( e->delta() > 0 )
    move = ( e->modifiers() & Qt::ControlModifier ) ? MoveFirst : MoveUp;
  else if ( e->delta() < 0 )
    move = ( e->modifiers() & Qt::ControlModifier ) ? MoveLast : MoveDown;
  moveIndex( move );
}

/*!
  \brief Called when menu action is triggered.
  \internal
*/
void QtxToolButton::actionTriggered( QAction* action )
{
  if ( action && !isSeparator( action ) ) {
    int index = currentIndex();
    menu()->setDefaultAction( action );
    internalUpdate();
    int newIndex = currentIndex();
    emitCurrentChanged( true, index != newIndex );
  }
}

/*!
  \brief Update content of the widget.
  \internal
*/
void QtxToolButton::internalUpdate()
{
  QAction* action = menu()->defaultAction();
  setText( action == 0 ? "" : action->text() );
  setIcon( action == 0 ? QIcon() : action->icon() );
}

/*!
  \brief Get menu action at given index.
  \internal
*/
QAction* QtxToolButton::actionAt( int index ) const
{
  return ( index >=0 && index < count() ) ? menu()->actions()[index] : 0;
}

/*!
  \brief Move current index.
  \internal
*/
void QtxToolButton::moveIndex( Move move )
{
  int index = currentIndex();
  int newIndex = index;
  switch ( move ) {
  case MoveUp:
  case MoveFirst:
    for ( int i = index-1; i >= 0; i-- ) {
      QAction* a = actionAt( i );
      if ( a && !isSeparator( a ) ) {
        newIndex = i;
        if ( move == MoveUp )
          break;
      }
    }
    break;
  case MoveDown:
  case MoveLast:
    for ( int i = index+1; i < count(); i++ ) {
      QAction* a = actionAt( i );
      if ( a && !isSeparator( a ) ) {
        newIndex = i;
        if ( move == MoveDown )
          break;
      }
    }
    break;
  default:
    break;
  }
  if ( newIndex != index ) {
    menu()->setDefaultAction( actionAt( newIndex ) );
    internalUpdate();
    emitCurrentChanged( true, true );
  }
}

/*!
  \brief Emit `currentChanged()` signal.
  \internal
*/
void QtxToolButton::emitCurrentChanged( bool activate, bool changed )
{
  QAction* action = menu()->defaultAction();
  int index = action == 0 ? -1 : menu()->actions().indexOf( action );
  QString text = action == 0 ? QString() : action->text();
  if ( activate ) {
    emit activated( index );
    emit activated( text );
  }
  if ( changed ) {
    emit currentIndexChanged( index );
    emit currentIndexChanged( text );
    emit currentTextChanged( text );
  }
}
