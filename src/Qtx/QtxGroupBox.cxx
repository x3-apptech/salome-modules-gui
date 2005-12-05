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
// File:      QtxGroupBox.cxx
// Author:    Sergey TELKOV

#include "QtxGroupBox.h"

#include <qhbox.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qapplication.h>
#include <qobjectlist.h>

QtxGroupBox::QtxGroupBox( QWidget* parent, const char* name )
: QGroupBox( parent, name ),
myContainer( 0 )
{
}

QtxGroupBox::QtxGroupBox( const QString& title, QWidget* parent, const char* name )
: QGroupBox( title, parent, name ),
myContainer( 0 )
{
  initialize();
}

QtxGroupBox::QtxGroupBox( int strips, Orientation o, QWidget* parent, const char* name )
: QGroupBox( strips, o, parent, name ),
myContainer( 0 )
{
  initialize();
}

QtxGroupBox::QtxGroupBox( int strips, Orientation o, const QString& title,
                                          QWidget* parent, const char* name )
: QGroupBox( strips, o, title, parent, name ),
myContainer( 0 )
{
  initialize();
}

QtxGroupBox::~QtxGroupBox()
{
  delete myContainer;
}

void QtxGroupBox::initialize()
{
  myContainer = new QHBox( this, 0, WStyle_Customize | WStyle_NoBorderEx | WStyle_Tool );

  updateTitle();
}

#if QT_VER < 3

int QtxGroupBox::insideMargin() const
{
  int m = 0;
  if ( layout() )
    m = layout()->margin();
  return m;
}

int QtxGroupBox::insideSpacing() const
{
  int s = 0;
  if ( layout() )
    s = layout()->spacing();
  return s;
}

void QtxGroupBox::setInsideMargin( int m )
{
  if ( layout() )
    layout()->setMargin( m );
}

void QtxGroupBox::setInsideSpacing( int s )
{
  if ( layout() )
    layout()->setSpacing( s );
}

#endif

void QtxGroupBox::insertTitleWidget( QWidget* wid )
{
  if ( !myContainer )
    return;

  wid->reparent( myContainer, QPoint( 0, 0 ), true );
  wid->installEventFilter( this );

  updateTitle();
}

void QtxGroupBox::removeTitleWidget( QWidget* wid )
{
  if ( !myContainer || wid->parentWidget() != myContainer )
    return;

  wid->reparent( 0, QPoint( 0, 0 ), false );
  wid->removeEventFilter( this );

  updateTitle();
}

void QtxGroupBox::adjustInsideMargin()
{
  QApplication::sendPostedEvents( myContainer, QEvent::ChildInserted );

  myContainer->resize( myContainer->minimumSizeHint() );

  setInsideMargin( myContainer->height() );
}

void QtxGroupBox::setAlignment( int align )
{
  QGroupBox::setAlignment( align );

  updateTitle();
}

void QtxGroupBox::setTitle( const QString& title )
{
  QGroupBox::setTitle( title );

  updateTitle();
}

void QtxGroupBox::setColumnLayout( int strips, Orientation o )
{
  if ( myContainer )
    myContainer->reparent( 0, QPoint( 0, 0 ), false );

  QGroupBox::setColumnLayout( strips, o );

  if ( myContainer )
    myContainer->reparent( this, QPoint( 0, 0 ), false );

  updateTitle();
}

void QtxGroupBox::show()
{
  QGroupBox::show();

  updateTitle();
}

void QtxGroupBox::update()
{
  QGroupBox::update();

  updateTitle();
}

QSize QtxGroupBox::sizeHint() const
{
  QSize sz = QGroupBox::sizeHint();

  int sw = titleSize().width();

  if ( myContainer )
  {
    if ( alignment() == AlignCenter )
      sw += 2 * ( myContainer->width() + 5 );
    else
      sw += 1 * ( myContainer->width() + 5 );
  }

  sw += frameRect().left();

  return QSize( QMAX( sz.width(), sw ), sz.height() );
}

QSize QtxGroupBox::minimumSizeHint() const
{
  QSize sz = QGroupBox::minimumSizeHint();

  int sw = titleSize().width() + myContainer ? myContainer->width() + 5 : 0;

  if ( myContainer )
  {
    if ( alignment() == AlignCenter )
      sw += 2 * ( myContainer->width() + 5 );
    else
      sw += 1 * ( myContainer->width() + 5 );
  }

  sw += frameRect().left();

  return QSize( QMAX( sz.width(), sw ), sz.height() );
}

bool QtxGroupBox::eventFilter( QObject* obj, QEvent* e )
{
  QEvent::Type type = e->type();
  if ( myContainer && obj->parent() == myContainer &&
       ( type == QEvent::Show || type == QEvent::ShowToParent ||
         type == QEvent::Hide || type == QEvent::HideToParent ) )
    QApplication::postEvent( this, new QCustomEvent( QEvent::User ) );

  return QGroupBox::eventFilter( obj, e );
}

void QtxGroupBox::resizeEvent( QResizeEvent* e )
{
  QGroupBox::resizeEvent( e );

  updateTitle();
}

void QtxGroupBox::childEvent( QChildEvent* e )
{
  if ( e->type() == QEvent::ChildInserted && e->child() == myContainer )
    return;

  QGroupBox::childEvent( e );
}

void QtxGroupBox::customEvent( QCustomEvent* )
{
  updateTitle();
}

void QtxGroupBox::frameChanged()
{
  updateTitle();
}

QSize QtxGroupBox::titleSize() const
{
  QSize sz( 0, 0 );

  if ( layout() )
  {
    QSpacerItem* si = 0;
    for ( QLayoutIterator it = layout()->iterator(); it.current() && !si; ++it )
      si = it.current()->spacerItem();
    if ( si )
      sz = si->sizeHint();
  }

  int w = sz.width();
  int h = sz.height() + insideMargin();

  return QSize( w, h );
}

void QtxGroupBox::updateTitle()
{
  if ( !myContainer )
    return;

  int align = alignment();
  if ( align == AlignAuto )
    align = QApplication::reverseLayout() ? AlignRight : AlignLeft;

  if ( title().isEmpty() )
    align = AlignRight;

  QSize ts = titleSize();

  int m = 5;

  int w = frameRect().width() - ts.width();
  if ( align == AlignCenter )
    w = w / 2;

  w -= m;

  QApplication::sendPostedEvents( myContainer, QEvent::ChildInserted );
  myContainer->resize( myContainer->minimumSizeHint() );

  bool vis = false;
  const QObjectList* list = myContainer->children();
  if ( list )
  {
    for ( QObjectListIt it( *list ); it.current() && !vis; ++it )
      vis = it.current()->isWidgetType() &&
            ((QWidget*)it.current())->isVisibleTo( myContainer );
  }

  if ( myContainer->height() > ts.height() || myContainer->width() > w || !vis )
    myContainer->hide();
  else
  {
    int x = 0;
    if ( align == AlignRight )
      x = frameRect().left() + m;
    else
      x = frameRect().right() - myContainer->width() - m;

    int y = frameRect().top() - ( myContainer->height() - frameWidth() ) / 2;

    QPoint pos( x, QMAX( 0, y ) );
    pos = mapToGlobal( pos );
    if ( myContainer->parentWidget() )
      pos = myContainer->parentWidget()->mapFromGlobal( pos );
    myContainer->move( pos );
    myContainer->show();
  }

  updateGeometry();
}
