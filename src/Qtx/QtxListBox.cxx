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

// File:      QtxListBox.cxx
// Author:    Sergey TELKOV
//
#include "QtxListBox.h"

#include <qpixmap.h>
#include <qlineedit.h>

/*!
  Constructor
*/
QtxListBox::QtxListBox( QWidget* parent, const char* name, WFlags f )
: QListBox( parent, name, f ),
myEditor( 0 ),
myEditIndex( -1 ),
myEditState( false ),
myEditDefault( true ),
myModifEnabled( true )
{
  connect( this, SIGNAL( contentsMoving( int, int ) ),
           this, SLOT( onContentsMoving( int, int ) ) );
}

/*!
  Destructor
*/
QtxListBox::~QtxListBox()
{
}

/*!
  \return true if editing is enabled
*/
bool QtxListBox::isEditEnabled() const
{
  return myEditState;
}

/*!
  Enables/disables editing
  \param on - new state
*/
void QtxListBox::setEditEnabled( bool on )
{
  if ( isEditEnabled() == on )
    return;

  myEditState = on;

  if ( !isEditEnabled() )
    endEdition( defaultEditAction() );
}

/*!
  \return default edit action
  \sa setDefaultEditAction()
*/
bool QtxListBox::defaultEditAction() const
{
  return myEditDefault;
}

/*!
  Changes default edit action. 
  Pressing of ENTER button always accepts new value of edited item.
  But other ways, such as focus out or edition of other item accepts
  new value of edited item only if "default edit action" is true
  \param def - new value
*/
void QtxListBox::setDefaultEditAction( bool def )
{
  myEditDefault = def;
}

/*!
  \return modification enabled state
  \sa setModificationEnabled()
*/
bool QtxListBox::isModificationEnabled() const
{
  return myModifEnabled;
}

/*!
  Changes "modification enabled" state
  If it is true, then pressing of CTRL + { Up, Down, Home, End } allows move items in list
  \param on - new state
*/
void QtxListBox::setModificationEnabled( bool on )
{
  myModifEnabled = on;
}

/*!
  \return current edited item
*/
QListBoxItem* QtxListBox::editedItem() const
{
  return item( editedIndex() );
}

/*!
  \return current edited index
*/
int QtxListBox::editedIndex() const
{
  return myEditIndex;
}

/*!
  Starts edition of item
  \param idx - index of item
*/
void QtxListBox::startEdition( const int idx )
{
  if ( idx < 0 || editedIndex() == idx || !isEditEnabled() )
    return;

  QLineEdit* ed = editor();
  if ( !ed )
    return;

  endEdition( defaultEditAction() );

  myEditIndex = idx;

  ensureItemVisible( myEditIndex );

  ed->setText( text( myEditIndex ) );
  updateEditor();
  ed->show();

  ed->setFocus();
}

/*!
  Starts edition of item
  \param item - item to be edited
*/
void QtxListBox::startEdition( const QListBoxItem* item )
{
  startEdition( index( item ) );
}

/*!
  Finishes edition of item
  \param action - if it is true, then new values must be accepted
*/
void QtxListBox::endEdition( const bool action )
{
  int idx = editedIndex();
  QLineEdit* ed = editor();

  if ( idx < 0 || !ed )
    return;

  myEditIndex = -1;

  ed->hide();

  if ( action )
  {
    int cur = currentItem();

    if ( pixmap( idx ) )
      changeItem( *pixmap( idx ), ed->text(), idx );
    else
      changeItem( ed->text(), idx );

    setCurrentItem( cur );

    emit itemEdited( idx );
    emit itemEdited( item( idx ) );
  }
}

/*!
  Ensures that the item is visible.
  \param idx - index of item
*/
void QtxListBox::ensureItemVisible( const int idx )
{
  if ( idx < 0 )
    return;

  if ( itemVisible( idx ) )
    return;

  setTopItem( idx );
}

/*!
  Ensures that the item is visible.
  \param item - item to be made visible
*/
void QtxListBox::ensureItemVisible( const QListBoxItem* item )
{
  ensureItemVisible( index( item ) );
}

/*!
  \return validator of item editor
*/
const QValidator* QtxListBox::validator() const
{
  const QValidator* v = 0;
  if ( editor() )
    v = editor()->validator();
  return v;
}

/*!
  Removes validator of item editor
*/
void QtxListBox::clearValidator()
{
  if ( editor() )
    editor()->clearValidator();
}

/*!
  Changes validator of item editor
  \param v - new validator
*/
void QtxListBox::setValidator( const QValidator* v )
{
  if ( editor() )
    editor()->setValidator( v );
}

/*!
  Moves item to top
  \param idx - index of item
*/
void QtxListBox::moveItemToTop( const int idx )
{
  moveItem( idx, -idx );
}

/*!
  Moves item to bottom
  \param idx - index of item
*/
void QtxListBox::moveItemToBottom( const int idx )
{
  moveItem( idx, count() - idx );
}

/*!
  Moves item
  \param idx - index of item
  \param step - changing of position
*/
void QtxListBox::moveItem( const int idx, const int step )
{
  QListBoxItem* i = item( idx );
  if ( !i || step == 0 )
    return;

  QListBoxItem* cur = item( currentItem() );

  takeItem( i );
  insertItem( i, QMAX( 0, idx + step ) );

  setCurrentItem( index( cur ) );

  int pos = index( i );
  if ( myEditIndex == idx )
    myEditIndex = pos;

  updateEditor();

  if ( idx != pos )
    emit itemMoved( idx, pos );
}

/*!
  Inserts empty item
  \param i - position of item (if it is less than 0, then current position is used)
*/
void QtxListBox::createItem( const int i )
{
  if ( !isEditEnabled() )
    return;

  int idx = i < 0 ? currentItem() : i;
  idx = idx < 0 ? count() : idx;
  idx = QMIN( (int)count(), idx );

  insertItem( "", idx );
  setCurrentItem( idx );
  startEdition( idx );
}

/*!
  Removes item
  \param i - position of item (if it is less than 0, then current position is used)
*/
void QtxListBox::deleteItem( const int i )
{
  if ( !isEditEnabled() )
    return;

  int idx = i < 0 ? currentItem() : i;
  if ( idx < 0 )
    return;

  if ( editedIndex() == idx )
    endEdition( defaultEditAction() );

  removeItem( idx );
  updateEditor();
}

/*!
  Scrolls the content so that the point is in the top-left corner.
  \param x, y - point co-ordinates
*/
void QtxListBox::setContentsPos( int x, int y )
{
  QListBox::setContentsPos( x, y );

  updateEditor();
}

/*!
  Custom event filter, performs finish of edition on focus out, escape/return/enter pressing
*/
bool QtxListBox::eventFilter( QObject* o, QEvent* e )
{
  if ( editor() == o )
  {
    if ( e->type() == QEvent::FocusOut )
      endEdition( defaultEditAction() );

    if ( e->type() == QEvent::KeyPress )
    {
      QKeyEvent* ke = (QKeyEvent*)e;
      if ( ke->key() == Key_Escape )
        endEdition( false );
      else if ( ke->key() == Key_Enter || ke->key() == Key_Return )
        endEdition( true );
    }
  }

  return QListBox::eventFilter( o, e );
}

/*!
  Custom key press event handler
  Allows to move items by CTRL + { Up, Down, Home, End }
*/
void QtxListBox::keyPressEvent( QKeyEvent* e )
{
  if ( e->key() == Key_Up && e->state() & ControlButton && isModificationEnabled() )
    moveItem( currentItem(), -1 );
  else if ( e->key() == Key_Down && e->state() & ControlButton && isModificationEnabled() )
    moveItem( currentItem(), 1 );
  else if ( e->key() == Key_Home && e->state() & ControlButton && isModificationEnabled() )
    moveItemToTop( currentItem() );
  else if ( e->key() == Key_End && e->state() & ControlButton && isModificationEnabled() )
    moveItemToBottom( currentItem() );
  else if ( e->key() == Key_Insert && e->state() & ControlButton )
    createItem( currentItem() );
  else if ( e->key() == Key_Delete && e->state() & ControlButton )
    deleteItem( currentItem() );
  else
    QListBox::keyPressEvent( e );
}

/*!
  Custom resize event handler
*/
void QtxListBox::viewportResizeEvent( QResizeEvent* e )
{
  QListBox::viewportResizeEvent( e );

  updateEditor();
}

/*!
  Custom mouse double click event handler
*/
void QtxListBox::mouseDoubleClickEvent( QMouseEvent* e )
{
  if ( isEditEnabled() )
    startEdition( itemAt( e->pos() ) );
  else
    QListBox::mouseDoubleClickEvent( e );
}

/*!
  Updates editor on contents moving
*/
void QtxListBox::onContentsMoving( int, int )
{
  updateEditor();
}

/*!
  \return item editor
*/
QLineEdit* QtxListBox::editor() const
{
  if ( !myEditor )
  {
    QtxListBox* that = (QtxListBox*)this;
    that->createEditor();
  }
  return myEditor;
}

/*!
  Creates item editor
*/
void QtxListBox::createEditor()
{
  if ( myEditor )
    return;

  myEditor = new QLineEdit( viewport() );

  myEditor->setLineWidth( 1 );
  myEditor->setMidLineWidth( 0 );
  myEditor->setFrameStyle( QFrame::Box | QFrame::Plain );
  myEditor->installEventFilter( this );

  myEditor->hide();

  addChild( myEditor );
}

/*!
  Updates item editor
*/
void QtxListBox::updateEditor()
{
  if ( !editedItem() || !editor() )
    return;

  QRect r = itemRect( editedItem() );
  if ( !r.isValid() )
    return;

  int m = editor()->lineWidth();
  r.addCoords( m, 0, 0, 0 );

  const QPixmap* pix = pixmap( editedIndex() );
  if ( pix )
    r.addCoords( pix->width() + 2, 0, 0, 0 );

  editor()->setGeometry( r );
}
