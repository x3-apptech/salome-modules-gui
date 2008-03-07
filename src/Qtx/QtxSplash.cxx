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
// File:      QtxSplash.cxx
// Author:    Vadim SANDLER

#include "QtxSplash.h"

#include <qapplication.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qmessagebox.h>

const int _PROGRESS_EVENT = QEvent::User + 10;
const int _PROGRESS_WIDTH = 10;

/*!
  Class ProgressEvent [ internal ].
*/
class ProgressEvent: public QCustomEvent
{
public:
  ProgressEvent( const QString& msg, const int progress = 0 )
    : QCustomEvent( id() ),
      myMessage( msg ),
      myProgress( progress )
  {}
  QString    message()  const { return myMessage;       } 
  int        progress() const { return myProgress;      }
  static int id()             { return _PROGRESS_EVENT; }

private:
  QString myMessage;
  int     myProgress;
};

// Only one instance of splash screen is allowed
QtxSplash* QtxSplash::mySplash = 0;

/*!
  Construct a splash screen that will display the \a pixmap.
*/
QtxSplash::QtxSplash( const QPixmap& pixmap )
  : QWidget( 0, 0, WStyle_Customize | WStyle_StaysOnTop | WX11BypassWM | WStyle_NoBorder )
{
  myAlignment    = AlignBottom | AlignRight;
  myColor        = white;
  myHideOnClick  = false;
  myProgress     = 0;
  myTotal        = 0;
  myGradientType = Vertical;
  myError        = 0;
  myStartColor   = red;
  myMargin       = 5;

  setPixmap( pixmap );
}

/*!
  Destructor.
*/
QtxSplash::~QtxSplash()
{
  mySplash = 0;
}

/*!
  Returns an only instance of splash screen.
  If \a px is valid sets this pixmap to the splash screen.
*/
QtxSplash* QtxSplash::splash( const QPixmap& px )
{
  if ( !mySplash )
    mySplash = new QtxSplash( px );
  else if ( !px.isNull() )
    mySplash->setPixmap( px );
  return mySplash;
}

/*!
  Sends the status message and (optionally) progress to the splash screen.
  Can be used, for example, from the progress thread.
*/
void QtxSplash::setStatus( const QString& msg, 
			   const int      progress )
{
  if ( mySplash ) {
    QApplication::postEvent( mySplash, new ProgressEvent( msg, progress ) );
    qApp->processEvents();
  }
}

/*!
  Sets error status and shows error message box to the user.
*/
void QtxSplash::error( const QString& error, const QString& title, const int code )
{
  printf("QtxSplash::error: %s\n",error.latin1());
  if ( mySplash ) {
    mySplash->setError( code );
    QMessageBox::critical( mySplash, 
			   title.isEmpty() ? tr( "Error" ) : title,
			   error,
			   tr( "&OK" ) );
  }
}

/*!
  Sets the pixmap that will be used as the splash screen's image to
  \a pixmap.
*/
void QtxSplash::setPixmap( const QPixmap& pixmap )
{
  myPixmap = pixmap;
  QRect r( 0, 0, myPixmap.size().width(), myPixmap.size().height() );
  resize( myPixmap.size() );
  move( QApplication::desktop()->screenGeometry().center() - r.center() );
  repaint();
}

/*!
  Returns the pixmap that is used in the splash screen.
*/
QPixmap QtxSplash::pixmap() const
{
  return myPixmap;
}

/*!
  Sets/clear the 'hide on mouse click' flag.
  Default is FALSE.
  When this flag is set, user can hide the splash screen window
  by clicking on it with mouse.
  But for this to work it is necessary to call periodically
  QApplication::processEvents() in order to allow event loop to process
  events because usually main application loop is not yet started
  at that moment.
*/
void QtxSplash::setHideOnClick( const bool on )
{
  myHideOnClick = on;
}

/*!
  Returns the 'hide on mouse click' flag.
*/
bool QtxSplash::hideOnClick() const
{
  return myHideOnClick;
}

/*!
  Sets total progress steps to \a total.
*/
void QtxSplash::setTotalSteps( const int total )
{
  myTotal = total;
  repaint();
}

/*!
  Return total progress steps number.
  \sa setTotalSteps(), setProgress()
*/
int QtxSplash::totalSteps() const
{
  return myTotal;
}
    
/*!
  Sets progress to \a progress.
*/
void QtxSplash::setProgress( const int progress )
{
  myProgress = progress;
  repaint();
}

/*!
  Return current progress.
  \sa setProgress(), setTotalSteps()
*/
int QtxSplash::progress() const
{
  return myProgress;
}

/*!
  Sets progress to \a progress and total progress steps to \a total.
*/
void QtxSplash::setProgress( const int progress, const int total )
{
  myTotal    = total;
  myProgress = progress;
  repaint();
}

/*!
  Sets progress bar colors to \a startColor and \a endColor.
  If the colors differ the gradient color bar is drawed.
  If the \a endColor is not valid, \a startColor is used instead.
  \a gradientType sets the type of gradient to be used for progress
  bar - horizontal or vertical. Default is vertical.
*/
void QtxSplash::setProgressColors( const QColor& startColor, 
				   const QColor& endColor,
				   const int     gradientType )
{
  myStartColor   = startColor;
  myEndColor     = endColor;
  myGradientType = gradientType;
  repaint();
}

/*!
  Return progress colors and gradient type (horizontal or vertical).
  \sa setProgressColors()
*/
int QtxSplash::progressColors( QColor& startColor, QColor& endColor )
{
  startColor = myStartColor;
  endColor   = myEndColor;
  return myGradientType;
}

/*!
  Sets message text alignment flags to \a alignment.
  Default is AlignBottom | AlignRight.
*/
void QtxSplash::setTextAlignment( const int alignment )
{
  myAlignment = alignment;
  repaint();
}

/*!
  Return message text alignment flags.
  \sa setTextAlignment()
*/
int QtxSplash::textAlignment() const
{
  return myAlignment;
}

/*!
  \brief Set margin.

  Margin is used when drawing progress bar and status messages.
  
  \param m new margin
  \sa margin()
*/
void QtxSplash::setMargin( const int m )
{
  myMargin = m;
  repaint();
}

/*!
  \brief Get margin.
  \return current margin.
  \sa setMargin()
*/
int QtxSplash::margin() const
{
  return myMargin;
}

/*!
  Sets message text color to \a color.
  Default is white.
  \sa setTextColors()
*/
void QtxSplash::setTextColor( const QColor& color )
{
  myColor = color;
  myShadowColor = QColor();
  repaint();
}

/*!
  Return message text color.
  \sa setTextColor()
*/
QColor QtxSplash::textColor() const
{
  return myColor;
}

/*!
  Sets message text color to \a color and text shadow color to \a shadow.
  \sa setTextColor()
*/
void QtxSplash::setTextColors( const QColor& color, const QColor& shadow )
{
  myColor       = color;
  myShadowColor = shadow;
  repaint();
}

/*!
  Return message text color and text shadow color.
  \sa setTextColors()
*/
void QtxSplash::textColors( QColor& color, QColor& shadow ) const
{
  color  = myColor;
  shadow = myShadowColor;
}

/*!
  Returns current status message.
*/
QString QtxSplash::message() const
{
  return myMessage;
}

/*!
  Return error code. If no errors were occured returns 0.
  Error code can be set by error( QString&, QString, int ).
*/
int QtxSplash::error() const
{
  return myError;
}

/*!
    Makes the splash screen wait until the widget \a mainWin is displayed
    before calling close() on itself.
*/
void QtxSplash::finish( QWidget* mainWin )
{
  if ( mainWin ) {
#if defined(Q_WS_X11)
    extern void qt_wait_for_window_manager( QWidget* w );
    qt_wait_for_window_manager( mainWin );
#endif
  }
  close();
}

/*!
  Repaint the splash screen.
*/
void QtxSplash::repaint()
{
  drawContents();
  QWidget::repaint();
  QApplication::flush();
}

/*!
  Draws the \a message text onto the splash screen with color \a
  color and aligns the text according to the flags in \a alignment.
*/
void QtxSplash::message( const QString& msg, 
			 int            alignment,
			 const QColor&  color )
{
  myMessage   = msg;
  myAlignment = alignment;
  myColor     = color;
  repaint();
}

/*!
  This is an overloaded member function, provided for convenience. 
  It behaves essentially like the above function.
  Draws the \a message text onto the splash screen with default color
  and aligns the text according to the default alignment flags.
*/
void QtxSplash::message( const QString& msg )
{
  myMessage = msg;
  repaint();
}

/*!
  Removes the message being displayed on the splash screen.
  \sa message()
*/
void QtxSplash::clear()
{
  myMessage = QString::null;
  repaint();
}

/*!
  Draw the contents of the splash screen using painter \a painter.
*/
void QtxSplash::drawContents( QPainter* painter )
{
  QRect r = rect();
  int m = margin();
  if ( myTotal > 0 ) {
    // draw progress bar
    if ( myGradientType == Horizontal ) {
      int tng = r.width() - r.x() - m*2;
      int ng = (int) ( 1.0 * tng * ( myProgress > 0 ? myProgress : 0 ) / myTotal ); 
      int h1, h2, s1, s2, v1, v2;
      myStartColor.hsv( &h1, &s1, &v1 );
      myEndColor.isValid() ? myEndColor.hsv( &h2, &s2, &v2 ) :
	                     myStartColor.hsv( &h2, &s2, &v2 );
      for ( int i = 0; i < ng; i++ ) {
	painter->setPen( QColor( h1 + ((h2-h1)*i)/(tng-1),
				 s1 + ((s2-s1)*i)/(tng-1),
				 v1 + ((v2-v1)*i)/(tng-1), 
				 QColor::Hsv ) );
	painter->drawLine( r.x()+m+i,
			   r.height()-m-_PROGRESS_WIDTH,
			   r.x()+m+i,
			   r.height()-m );
      }
    }
    else {
      int ng = (int) ( 1.0 * (r.width() - r.x() - m*2 - 1) * ( myProgress > 0 ? myProgress : 0 ) / myTotal ); 
      int h1, h2, s1, s2, v1, v2;
      myStartColor.hsv( &h1, &s1, &v1 );
      myEndColor.isValid() ? myEndColor.hsv( &h2, &s2, &v2 ) :
	                     myStartColor.hsv( &h2, &s2, &v2 );
      for ( int i = 0; i < _PROGRESS_WIDTH; i++ ) {
	painter->setPen( QColor( h1 + ((h2-h1)*i)/(_PROGRESS_WIDTH-1),
				 s1 + ((s2-s1)*i)/(_PROGRESS_WIDTH-1),
				 v1 + ((v2-v1)*i)/(_PROGRESS_WIDTH-1), 
				 QColor::Hsv ) );
	painter->drawLine( r.x()+m,
			   r.height()-m-_PROGRESS_WIDTH+i,
			   r.x()+m+ng,
			   r.height()-m-_PROGRESS_WIDTH+i );
      }
    }
    // draw progress bar outline rectangle
    painter->setPen( palette().active().dark() );
    painter->drawLine( r.x()+m, 
		       r.height()-m-_PROGRESS_WIDTH,
		       r.width()-m,
		       r.height()-m-_PROGRESS_WIDTH );
    painter->drawLine( r.x()+m,
		       r.height()-m-_PROGRESS_WIDTH,
		       r.x()+m,
		       r.height()-m );
    painter->setPen( palette().active().light() );
    painter->drawLine( r.x()+m,
		       r.height()-m,
		       r.width()-m,
		       r.height()-m );
    painter->drawLine( r.width()-m,
		       r.height()-m-_PROGRESS_WIDTH,
		       r.width()-m,
		       r.height()-m );
  }
  // draw status
  if ( !myMessage.isEmpty() ) {
    QFontMetrics f( font() );
    int spacing = f.lineSpacing();
    int shift = myTotal > 0 ? _PROGRESS_WIDTH : _PROGRESS_WIDTH; // : 0
    int i = myMessage.length() - 1;
    while( i >= 0 && myMessage[ i-- ] == '\n' )
      shift += spacing;
    QRect r1( r.x() + m, r.y() + m, r.width() - m*2, r.height() - m*2 - shift );
    QRect r2 = r1;
    if ( myAlignment & Qt::AlignLeft   ) r2.setLeft  ( r2.left()   + 1 );
    if ( myAlignment & Qt::AlignTop    ) r2.setTop   ( r2.top()    + 1 );
    if ( myAlignment & Qt::AlignRight  ) r2.setRight ( r2.right()  + 1 );
    if ( myAlignment & Qt::AlignBottom ) r2.setBottom( r2.bottom() + 1 );
    if ( myShadowColor.isValid() ) {
      painter->setPen( myShadowColor );
      painter->drawText( r2, myAlignment, myMessage );
    }
    painter->setPen( myColor );
    painter->drawText( r1, myAlignment, myMessage );
  }
}

/*!
  Mouse press event.
  Hides splash screen if the 'hide on mouse click' flag is set.
  \sa setHideOnClick()
*/
void QtxSplash::mousePressEvent( QMouseEvent* )
{
  if ( myHideOnClick )
    hide();
}

/*!
  Processes custom event sent by setStatus() method.
  \sa setStatus().
*/
void QtxSplash::customEvent( QCustomEvent* ce )
{
  if ( ce->type() == ProgressEvent::id() ) {
    ProgressEvent* pe = (ProgressEvent*)ce;
    pe->message().isEmpty() ? clear() : message( pe->message() );
    setProgress( pe->progress() );
    qApp->processEvents();
  }
}

/*!
  Draws the splash screen window [ internal ].
*/
void QtxSplash::drawContents()
{
  QPixmap textPix = myPixmap;
  QPainter painter( &textPix, this );
  drawContents( &painter );
  setErasePixmap( textPix );
}

/*!
  Sets error code [ internal ].
*/
void QtxSplash::setError( const int code )
{
  myError = code;
}
