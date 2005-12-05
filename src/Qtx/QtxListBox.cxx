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
// See http://www.salome-platform.org/
//
// File:      QtxListBox.cxx
// Author:    Sergey TELKOV

#include "QtxListBox.h"

#include <qpixmap.h>
#include <qlineedit.h>

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

QtxListBox::~QtxListBox()
{
}

bool QtxListBox::isEditEnabled() const
{
  return myEditState;
}

void QtxListBox::setEditEnabled( bool on )
{
  if ( isEditEnabled() == on )
    return;

  myEditState = on;

  if ( !isEditEnabled() )
    endEdition( defaultEditAction() );
}

bool QtxListBox::defaultEditAction() const
{
  return myEditDefault;
}

void QtxListBox::setDefaultEditAction( bool def )
{
  myEditDefault = def;
}

bool QtxListBox::isModificationEnabled() const
{
  return myModifEnabled;
}

void QtxListBox::setModificationEnabled( bool on )
{
  myModifEnabled = on;
}

QListBoxItem* QtxListBox::editedItem() const
{
  return item( editedIndex() );
}

int QtxListBox::editedIndex() const
{
  return myEditIndex;
}

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

void QtxListBox::startEdition( const QListBoxItem* item )
{
  startEdition( index( item ) );
}

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

void QtxListBox::ensureItemVisible( const int idx )
{
  if ( idx < 0 )
    return;

  if ( itemVisible( idx ) )
    return;

  setTopItem( idx );
}

void QtxListBox::ensureItemVisible( const QListBoxItem* item )
{
  ensureItemVisible( index( item ) );
}

const QValidator* QtxListBox::validator() const
{
  const QValidator* v = 0;
  if ( editor() )
    v = editor()->validator();
  return v;
}

void QtxListBox::clearValidator()
{
  if ( editor() )
    editor()->clearValidator();
}

void QtxListBox::setValidator( const QValidator* v )
{
  if ( editor() )
    editor()->setValidator( v );
}

void QtxListBox::moveItemToTop( const int idx )
{
  moveItem( idx, -idx );
}

void QtxListBox::moveItemToBottom( const int idx )
{
  moveItem( idx, count() - idx );
}

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

void QtxListBox::setContentsPos( int x, int y )
{
  QListBox::setContentsPos( x, y );

  updateEditor();
}

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

void QtxListBox::viewportResizeEvent( QResizeEvent* e )
{
  QListBox::viewportResizeEvent( e );

  updateEditor();
}

void QtxListBox::mouseDoubleClickEvent( QMouseEvent* e )
{
  if ( isEditEnabled() )
    startEdition( itemAt( e->pos() ) );
  else
    QListBox::mouseDoubleClickEvent( e );
}

void QtxListBox::onContentsMoving( int, int )
{
  updateEditor();
}

QLineEdit* QtxListBox::editor() const
{
  if ( !myEditor )
  {
    QtxListBox* that = (QtxListBox*)this;
    that->createEditor();
  }
  return myEditor;
}

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
