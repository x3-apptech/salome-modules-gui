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

// File:      QtxToolTip.cxx
// Author:    Sergey TELKOV
//
#include "QtxToolTip.h"

#include <QFont>
#include <QTimer>
#include <QCursor>
#include <QFontMetrics>
#include <QApplication>
#include <QPalette>
#include <QMouseEvent>

#define TOOLTIP_SHOW_DELAY 0500
#define TOOLTIP_HIDE_DELAY 7000

/*!
  Constructor
*/
QtxToolTip::QtxToolTip( QWidget* parent )
: QLabel( parent, Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::Window )
{
  setObjectName( "" );
  setIndent( 3 );
  setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  QPalette palette;
  palette.setColor( backgroundRole(), QColor( 255, 255, 231 ) );
  setPalette( palette );

        myWidgetRegion = QRect( -1, -1, -1, -1 );

        setFrameShape( QFrame::Panel );
        setFrameShadow( QFrame::Plain );

  parent->setMouseTracking( true );
        parent->installEventFilter( this );
        installEventFilter( this );

        mySleepTimer = new QTimer( this );
        mySleepTimer->setSingleShot( true );
        myWakeUpTimer = new QTimer( this );
        myWakeUpTimer->setSingleShot( true );
        connect( mySleepTimer,  SIGNAL( timeout() ), this, SLOT( onSleepTimeOut()  ) );
        connect( myWakeUpTimer, SIGNAL( timeout() ), this, SLOT( onWakeUpTimeOut() ) );

  myWakeUpDelayTime = 700;
  myShowDelayTime = 5000;
}

/*!
  Destructor
*/
QtxToolTip::~QtxToolTip()
{
}

/*!
  Custom event filter
*/
bool QtxToolTip::eventFilter( QObject* o, QEvent* e )
{
        if ( ( e->type() == QEvent::Destroy ) || ( e->type() == QEvent::Close ) || ( e->type() == QEvent::Hide ) )
        {
                hideTip();
        }
        if ( e->type() == QEvent::Leave )
        {
                if ( isVisible() && ( o == this ) )
                        hideTip();
                myWakeUpTimer->stop();
        }
        if ( e->type() == QEvent::MouseMove )
        {
                QMouseEvent* me = (QMouseEvent*)e;
                QPoint thePos = parentWidget()->mapFromGlobal( me->globalPos() );
                if ( myWakeUpTimer->isActive() )
                {
                        myWakeUpTimer->stop();
                        myWakeUpTimer->start( myWakeUpDelayTime );
                }
                if ( isVisible() )
                {
                        if ( !myWidgetRegion.contains( thePos ) )
      {
                                hideTip();
        myWidgetRegion = QRect( -1, -1, -1, -1 );
      }
                }
                else
                {
                        if ( !myWidgetRegion.isValid() || myWidgetRegion.contains( thePos ) )
                                myWakeUpTimer->start( myWakeUpDelayTime );
                }
        }
        if ( e->type() == QEvent::KeyPress )
        {
                hideTip();
        }
        if ( o == parent() && ( e->type() == QEvent::MouseButtonPress ||
                          e->type() == QEvent::MouseButtonRelease ) )
        {
                hideTip();
        }
        return false;
}

/*!
  Shows tool tip
  \param aPos - position
  \param text - tooltip text
  \param aWidgetRegion - rectangle
*/
void QtxToolTip::showTip( const QPoint& aPos, const QString& text, const QRect& aWidgetRegion )
{
        QFontMetrics theFM = fontMetrics();
        int theHeight = theFM.height();
        int theWidth = theFM.width( text ) + 2;
        showTip( QRect( QPoint( aPos.x(), aPos.y() + 10 ), QSize( theWidth, theHeight ) ), text, aWidgetRegion );
}

/*!
  Shows tool tip
  \param aRegion - tooltip region
  \param text - tooltip text
  \param aWidgetRegion - widget rectangle
*/
void QtxToolTip::showTip( const QRect& aRegion, const QString& text, const QRect& aWidgetRegion )
{
        setText( text );
        myWidgetRegion = aWidgetRegion;
        setGeometry( aRegion );
        if ( myShowDelayTime != 0 )
                mySleepTimer->start( myShowDelayTime );
        show();
}

/*!
  Hides tooltip
*/
void QtxToolTip::hideTip()
{
        hide();
  myWidgetRegion = QRect( -1, -1, -1, -1 );
        mySleepTimer->stop();
}

/*!
  It is called when there is a possibility that a tool tip should be shown and
  must decide whether there is a tool tip for the point p in the widget that this QToolTip object relates to
  \param pos - position
*/
void QtxToolTip::maybeTip( const QPoint& pos )
{
        QString text;
        QRect textRegion, theRegion( -1, -1, -1, -1 );
        QFont theFnt = font();

    emit maybeTip( pos, text, theFnt, textRegion, theRegion );

    if ( theRegion.isValid() )
        {
                setFont( theFnt );
                int margin = lineWidth() + indent();
                QRect dspRegion( QPoint( textRegion.x() - margin, textRegion.y() ),
                                         QSize( textRegion.width() + 2 * margin, textRegion.height() ) );
                QRect tipRegion( parentWidget()->mapToGlobal( dspRegion.topLeft() ), dspRegion.size() );
                if ( tipRegion.left() < 0 )
                        tipRegion.translate( -1 * tipRegion.left(), 0 );
                showTip( tipRegion, text, theRegion );
        }
}

/*!
  SLOT: called when sleep time is out
*/
void QtxToolTip::onSleepTimeOut()
{
        mySleepTimer->stop();
        hideTip();
}

/*!
  SLOT: called when wake time is out
*/
void QtxToolTip::onWakeUpTimeOut()
{
        myWakeUpTimer->stop();
  QPoint pos = QCursor::pos();
  if ( parentWidget() )
    pos = parentWidget()->mapFromGlobal( pos );
  maybeTip( pos );
}

/*!
  Custom mouse press event handler
*/
void QtxToolTip::mousePressEvent( QMouseEvent* e )
{
        hideTip();
        QWidget* reciever = parentWidget();
        QMouseEvent* me = new QMouseEvent( QEvent::MouseButtonPress,
                                                                           reciever->mapFromGlobal( e->globalPos() ),
                                                                           e->button(), e->buttons(), Qt::KeypadModifier );
        QApplication::sendEvent( reciever, me );
}

/*!
  Custom mouse double click event handler
*/
void QtxToolTip::mouseDoubleClickEvent( QMouseEvent* e )
{
        hideTip();
        QWidget* reciever = parentWidget();
        QMouseEvent* me = new QMouseEvent( QEvent::MouseButtonDblClick,
                                                                           reciever->mapFromGlobal( e->globalPos() ),
                                                                           e->button(), e->buttons(), Qt::KeypadModifier );
        QApplication::sendEvent( reciever, me );
}

/*!
  Sets wake delay time
  \param theTime
*/
void QtxToolTip::setWakeUpDelayTime( int theTime )
{
  if( !(theTime < 0) )
    myWakeUpDelayTime = theTime;
}

/*!
  Sets show delay time
  \param theTime
*/
void QtxToolTip::setShowDelayTime( int theTime )
{
  if( !(theTime < 0) )
    myShowDelayTime = theTime;
}

/*!
  \return timer measuring time of sleeping
*/
QTimer* QtxToolTip::sleepTimer() const
{
  return mySleepTimer;
}

/*!
  \return timer measuring time of waking up
*/
QTimer* QtxToolTip::wakeUpTimer() const
{
  return myWakeUpTimer;
}
