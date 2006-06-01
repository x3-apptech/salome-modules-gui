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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      QtxToolTip.cxx
// Author:    Sergey TELKOV

#include "QtxToolTip.h"

#include <qfont.h>
#include <qtimer.h>
#include <qcursor.h>
#include <qfontmetrics.h>
#include <qapplication.h>

#define TOOLTIP_SHOW_DELAY 0500
#define TOOLTIP_HIDE_DELAY 7000

/*!
  Constructor
*/
QtxToolTip::QtxToolTip( QWidget* parent )
: QLabel( parent, "", WStyle_Customize | WStyle_NoBorder | WX11BypassWM | WStyle_Tool | WStyle_StaysOnTop | WType_TopLevel )
{
  setIndent( 3 );
	setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
	setBackgroundColor( QColor( 255, 255, 231 ) );

	myWidgetRegion = QRect( -1, -1, -1, -1 );

	setFrameShape( QFrame::Panel );
	setFrameShadow( QFrame::Plain );

  parent->setMouseTracking( true );
	parent->installEventFilter( this );
	installEventFilter( this );

	mySleepTimer = new QTimer( this );
	myWakeUpTimer = new QTimer( this );
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
			myWakeUpTimer->start( myWakeUpDelayTime, true );
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
				myWakeUpTimer->start( myWakeUpDelayTime, true );
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
		mySleepTimer->start( myShowDelayTime, true );
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
			tipRegion.moveBy( -1 * tipRegion.left(), 0 );
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
									   e->button(), e->state() );
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
									   e->button(), e->state() );
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
