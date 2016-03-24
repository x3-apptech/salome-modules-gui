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

// File:      QtxTable.cxx
// Author:    Sergey TELKOV
//
#include "QtxTable.h"

#ifndef QT_NO_TABLE

#include <qlineedit.h>

/*!
  Constructor
*/
QtxTable::QtxTable( QWidget* parent, const char* name )
: QTable( parent, name ),
myHeaderEditor( 0 ),
myEditedHeader( 0 ),
myEditedSection( -1 )
{
  connect( verticalHeader(), SIGNAL( sizeChange( int, int, int ) ),
           this, SLOT( onHeaderSizeChange( int, int, int ) ) );
  connect( horizontalHeader(), SIGNAL( sizeChange( int, int, int ) ),
           this, SLOT( onHeaderSizeChange( int, int, int ) ) );
  connect( verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
  connect( horizontalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
}

/*!
  Constructor
*/
QtxTable::QtxTable( int numRows, int numCols, QWidget* parent, const char* name )
: QTable( numRows, numCols, parent, name ),
myHeaderEditor( 0 ),
myEditedHeader( 0 ),
myEditedSection( -1 )
{
  connect( verticalHeader(), SIGNAL( sizeChange( int, int, int ) ),
           this, SLOT( onHeaderSizeChange( int, int, int ) ) );
  connect( horizontalHeader(), SIGNAL( sizeChange( int, int, int ) ),
           this, SLOT( onHeaderSizeChange( int, int, int ) ) );
  connect( verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
  connect( horizontalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onScrollBarMoved( int ) ) );
}

/*!
  Destructor
*/
QtxTable::~QtxTable()
{
}

/*!
  \return true if header is editable
  \param o - header orientation
*/
bool QtxTable::headerEditable( Orientation o ) const
{
  return myHeaderEditable.contains( o ) ? myHeaderEditable[o] : false;
}

/*!
  Changes editable state of header
  \param o - header orientation
  \param on - new state
*/
void QtxTable::setHeaderEditable( Orientation o, const bool on )
{
  if ( headerEditable( o ) == on )
    return;

  myHeaderEditable.insert( o, on );

  QHeader* hdr = header( o );

  if ( !on && myEditedHeader == hdr )
    endHeaderEdit( false );

  if ( on )
    hdr->installEventFilter( this );
  else
    hdr->removeEventFilter( this );
}

/*!
  Starts edition of header
  \param o - header orientation
  \param sec - column/row
*/
bool QtxTable::editHeader( Orientation o, const int sec )
{
  return beginHeaderEdit( o, sec );
}

/*!
  Finishes edition of header
  \param accept - whether new value must be accepted
*/
void QtxTable::endEditHeader( const bool accept )
{
  endHeaderEdit( accept );
}

/*!
  Finishes edition and hides table
*/
void QtxTable::hide()
{
  endHeaderEdit();

  QTable::hide();
}

/*!
  Custom event filter
  Starts edition of header by double click
  Finishes edition by escape/return/enter pressing
*/
bool QtxTable::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::MouseButtonDblClick )
  {
    QMouseEvent* me = (QMouseEvent*)e;
    if ( o == horizontalHeader() )
    {
      beginHeaderEdit( Horizontal, me->pos() );
      return true;
    }
    else if ( o == verticalHeader() )
    {
      beginHeaderEdit( Vertical, me->pos() );
      return true;
    }
  }

  if ( o == myHeaderEditor && e->type() == QEvent::KeyPress && isHeaderEditing() )
  {
          QKeyEvent* ke = (QKeyEvent*)e;
    if ( ke->key() == Key_Escape )
    {
      endHeaderEdit( false );
      return true;
    }

    if ( ke->key() == Key_Return || ke->key() == Key_Enter )
    {
      endHeaderEdit( true );
      return true;
    }

    return false;
  }

  if ( o == myHeaderEditor && e->type() == QEvent::FocusOut &&
       isHeaderEditing() && ((QFocusEvent*)e)->reason() != QFocusEvent::Popup )
  {
                endHeaderEdit();
                return true;
  }

  if ( e->type() == QEvent::Wheel && isHeaderEditing() )
    return true;

  return QTable::eventFilter( o, e );
}

/*!
  SLOT: called on scroll
*/
void QtxTable::onScrollBarMoved( int )
{
  updateHeaderEditor();
}

/*!
  SLOT: called on header size changing
*/
void QtxTable::onHeaderSizeChange( int, int, int )
{
  if ( sender() == myEditedHeader )
    updateHeaderEditor();
}

/*!
  Custom resize event handler
*/
void QtxTable::resizeEvent( QResizeEvent* e )
{
  QTable::resizeEvent( e );

  updateHeaderEditor();
}

/*!
  Starts edition of header
  \param o - header orientation
  \param sec - column/row
*/
bool QtxTable::beginHeaderEdit( Orientation o, const int section )
{
  if ( !headerEditable( o ) || !header( o ) || !header( o )->isVisibleTo( this ) )
    return false;

  endHeaderEdit();

  QHeader* hdr = header( o );

  QRect r = headerSectionRect( hdr, section );
  if ( !r.isValid() )
    return false;

  if ( o == Horizontal )
    r.setLeft( QMAX( r.left(), leftMargin() ) );
  else
    r.setTop( QMAX( r.top(), topMargin() ) );

  myHeaderEditor = createHeaderEditor( hdr, section );
  if ( !myHeaderEditor )
    return false;

  myEditedHeader = hdr;
  myEditedSection = section;

  myHeaderEditor->reparent( this, QPoint( 0, 0 ), false );

  updateHeaderEditor();

  myHeaderEditor->show();

  myHeaderEditor->setActiveWindow();
  myHeaderEditor->setFocus();

  myHeaderEditor->installEventFilter( this );

  return true;
}

/*!
  Finishes edition of header
  \param accept - whether new value must be accepted
*/
void QtxTable::endHeaderEdit( const bool accept )
{
  if ( !isHeaderEditing() )
    return;

  QString oldTxt = myEditedHeader ? myEditedHeader->label( myEditedSection ) : QString();

  if ( accept && myEditedHeader )
    setHeaderContentFromEditor( myEditedHeader, myEditedSection, myHeaderEditor );

  QString newTxt = myEditedHeader ? myEditedHeader->label( myEditedSection ) : QString();

  int sec = myEditedSection;
  QHeader* hdr = myEditedHeader;

  myEditedHeader = 0;
  myEditedSection = -1;

  myHeaderEditor->hide();
  myHeaderEditor->deleteLater();
  myHeaderEditor = 0;

  if ( oldTxt != newTxt )
  {
    emit headerEdited( hdr, sec );
    emit headerEdited( hdr == horizontalHeader() ? Horizontal : Vertical, sec );
  }
}

/*!
  \return true if header is being edited
*/
bool QtxTable::isHeaderEditing() const
{
  return myHeaderEditor && myEditedHeader && myEditedSection != -1;
}

/*!
  Creates and \return header editor
  \param hdr - header
  \param sec - column/row
  \param init - init editor with value
*/
QWidget* QtxTable::createHeaderEditor( QHeader* hdr, const int sec, const bool init )
{
  QLineEdit* ed = new QLineEdit( 0 );

  if ( init && hdr )
    ed->setText( hdr->label( sec ) );

  return ed;
}

/*!
  Initialize editor with value
  \param hdr - header
  \param sec - column/row
  \param editor - editor
*/
void QtxTable::setHeaderContentFromEditor( QHeader* hdr, const int sec, QWidget* editor )
{
  if ( !hdr || !editor )
    return;

  if ( editor->inherits( "QLineEdit" ) )
    hdr->setLabel( sec, ((QLineEdit*)editor)->text() );
}

/*!
  \return header
  \param o - orientation
*/
QHeader* QtxTable::header( Orientation o ) const
{
  return o == Horizontal ? horizontalHeader() : verticalHeader();
}

/*!
  Starts edition of header
  \param o - header orientation
  \param p - point
*/
void QtxTable::beginHeaderEdit( Orientation o, const QPoint& p )
{
  QHeader* hdr = header( o );
  if ( !hdr )
    return;

  int pos = o == Horizontal ? p.x() : p.y();
  int sec = hdr->sectionAt( hdr->offset() + pos );

  beginHeaderEdit( o, sec );
}

/*!
  \return rectangle of header section
  \param hdr - header
  \param sec - column/row
*/
QRect QtxTable::headerSectionRect( QHeader* hdr, const int sec ) const
{
  QRect r( -1, -1, -1, -1 );

  if ( !hdr )
    return r;

  r = hdr->sectionRect( sec );
  if ( r.isValid() )
    r = QRect( mapFromGlobal( hdr->mapToGlobal( r.topLeft() ) ), r.size() );

  return r;
}

/*!
  Updates header editor
*/
void QtxTable::updateHeaderEditor()
{
  if ( !myHeaderEditor || !myEditedHeader || myEditedSection < 0 )
    return;

  QRect r = headerSectionRect( myEditedHeader, myEditedSection );
  if ( !r.isValid() )
    return;

  if ( myEditedHeader == horizontalHeader() )
  {
    r.setLeft( QMAX( r.left(), leftMargin() ) );
    r.setRight( QMIN( r.right(), width() - rightMargin() - 2 ) );
  }
  else
  {
    r.setTop( QMAX( r.top(), topMargin() ) );
    r.setBottom( QMIN( r.bottom(), height() - bottomMargin() - 2 ) );
  }

  myHeaderEditor->resize( r.size() );
  myHeaderEditor->move( r.topLeft() );
}

#endif
