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

// File   : Plot2d.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "Plot2d.h"

#include <QPainter>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

const int MSIZE  = 9;
const int MAX_ATTEMPTS        = 10;     // max attempts

// color tolerance (used to compare color values)
const long COLOR_DISTANCE = 100;



/*!
  Constructor
*/
Plot2d_Point::Plot2d_Point()
  : x( 0. ), y( 0. ), deviationPtr(0)
{
}

/*!
  Constructor
*/
Plot2d_Point::Plot2d_Point( double theX, double theY, const QString& theText )
  : x( theX ), y( theY ), text( theText ), deviationPtr(0)
{
}

/*!
  Destructor.
*/
Plot2d_Point::~Plot2d_Point() {
  clearDeviation();
}

/*!
  Free memory allocated for the deviation data.
*/
void Plot2d_Point::clearDeviation() {
  if(deviationPtr)
    delete deviationPtr;
  deviationPtr = 0;
}

/*!
  Return true in case if deviation data is assigned to the point.
*/
bool Plot2d_Point::hasDeviation() const {
  return !(deviationPtr == 0);
}

/*!
  Assign deviation data to the point.
*/
void Plot2d_Point::setDeviation(double min, double max) {
 clearDeviation();
 deviationPtr = new double[2];
 deviationPtr[0] = min;deviationPtr[1] = max;
}

/*!
  Return true in case if deviation data is assigned to the point
  and store deviation data in the input parameters.
*/
bool Plot2d_Point::deviation(double& min, double& max) const {
  if(hasDeviation()) {
    min = deviationPtr[0];
    max = deviationPtr[1];
  }
  return false;
}

/*!
  Return minimal deviation value.
*/
bool Plot2d_Point::minDeviation(double& min) const {
  if(hasDeviation()) {
    min = deviationPtr[0];
    return true;
  } else {
    min = 0;
  }
  return false;
}

/*!
  Return minimal deviation value.
*/
bool Plot2d_Point::maxDeviation(double& max) const {
  if(hasDeviation()) {
    max = deviationPtr[1];
    return true;
  } else {
    max = 0;
  }
  return false;
}



/*!
  \brief Convert Plot2d marker type to Qwt marker type.
  \param m Plot2d marker type
  \return Qwt marker type
*/
QwtSymbol::Style Plot2d::plot2qwtMarker( Plot2d::MarkerType m )
{
  QwtSymbol::Style ms = QwtSymbol::NoSymbol;  
  switch ( m ) {
  case Plot2d::Circle:
    ms = QwtSymbol::Ellipse;   break;
  case Plot2d::Rectangle:
    ms = QwtSymbol::Rect;      break;
  case Plot2d::Diamond:
    ms = QwtSymbol::Diamond;   break;
  case Plot2d::DTriangle:
    ms = QwtSymbol::DTriangle; break;
  case Plot2d::UTriangle:
    ms = QwtSymbol::UTriangle; break;
  case Plot2d::LTriangle:
    ms = QwtSymbol::LTriangle; break;
  case Plot2d::RTriangle:
    ms = QwtSymbol::RTriangle; break;
  case Plot2d::Cross:
    ms = QwtSymbol::Cross;     break;
  case Plot2d::XCross:
    ms = QwtSymbol::XCross;    break;
  case Plot2d::None:
  default:
    ms = QwtSymbol::NoSymbol;  break;
  }
  return ms;
}

/*!
  \brief Convert Qwt marker type to Plot2d marker type.
  \param m Qwt marker type
  \return Plot2d marker type
*/
Plot2d::MarkerType Plot2d::qwt2plotMarker( QwtSymbol::Style m )
{
  Plot2d::MarkerType ms = Plot2d::None;  
  switch ( m ) {
  case QwtSymbol::Ellipse:
    ms = Plot2d::Circle;    break;
  case QwtSymbol::Rect:
    ms = Plot2d::Rectangle; break;
  case QwtSymbol::Diamond:
    ms = Plot2d::Diamond;   break;
  case QwtSymbol::DTriangle:
    ms = Plot2d::DTriangle; break;
  case QwtSymbol::UTriangle:
    ms = Plot2d::UTriangle; break;
  case QwtSymbol::RTriangle:
    ms = Plot2d::RTriangle; break;
  case QwtSymbol::LTriangle:
    ms = Plot2d::LTriangle; break;
  case QwtSymbol::Cross:
    ms = Plot2d::Cross;     break;
  case QwtSymbol::XCross:
    ms = Plot2d::XCross;    break;
  case QwtSymbol::NoSymbol:
  default:
    ms = Plot2d::None;      break;
  }
  return ms;
}

/*!
  \brief Convert Plot2d line type to Qt/Qwt line type.
  \param p Plot2d line type
  \return Qt/Qwt line type
*/
Qt::PenStyle Plot2d::plot2qwtLine( Plot2d::LineType p )
{
  Qt::PenStyle ps = Qt::NoPen;
  switch ( p ) {
  case Plot2d::Solid:
    ps = Qt::SolidLine;      break;
  case Plot2d::Dash:
    ps = Qt::DashLine;       break;
  case Plot2d::Dot:
    ps = Qt::DotLine;        break;
  case Plot2d::DashDot:
    ps = Qt::DashDotLine;    break;
  case Plot2d::DashDotDot:
    ps = Qt::DashDotDotLine; break;
  case Plot2d::NoPen:
  default:
    ps = Qt::NoPen;          break;
  }
  return ps;
}

/*!
  \brief Convert Qt/Qwt line type to Plot2d line type.
  \param p Qt/Qwt line type
  \return Plot2d line type
*/
Plot2d::LineType Plot2d::qwt2plotLine( Qt::PenStyle p )
{
  Plot2d::LineType ps = Plot2d::NoPen;
  switch ( p ) {
  case Qt::SolidLine:
    ps = Plot2d::Solid;      break;
  case Qt::DashLine:
    ps = Plot2d::Dash;       break;
  case Qt::DotLine:
    ps = Plot2d::Dot;        break;
  case Qt::DashDotLine:
    ps = Plot2d::DashDot;    break;
  case Qt::DashDotDotLine:
    ps = Plot2d::DashDotDot; break;
  case Qt::NoPen:
  default:
    ps = Plot2d::NoPen;      break;
  }
  return ps;
}

/*!
  \brief Draw line.
  \param painter painter
  \param p1 starting point
  \param p2 ending point
  \param type line type
  \param color line color
  \param width line width
*/
void Plot2d::drawLine( QPainter* painter, const QPoint& p1, const QPoint& p2, 
                       Qt::PenStyle type, const QColor& color, int width )
{
  painter->save();
  QPen pen( type );
  pen.setColor( color );
  pen.setWidth( width );
  painter->setPen( pen );
  painter->drawLine( p1, p2 );
  painter->restore();
}

/*!
  \brief Draw line.
  \param painter painter
  \param p1 starting point
  \param p2 ending point
  \param type line type
  \param color line color
  \param width line width
*/
void Plot2d::drawLine( QPainter* painter, const QPoint& p1, const QPoint& p2, 
                       Plot2d::LineType type, const QColor& color, int width )
{
  drawLine( painter, p1, p2, plot2qwtLine( type ), color, width );
}

/*!
  \brief Draw line.
  \param painter painter
  \param x1 X coordinate of the starting point
  \param y1 Y coordinate of the starting point
  \param x2 X coordinate of the ending point
  \param y2 Y coordinate of the ending point
  \param type line type
  \param color line color
  \param width line width
*/
void Plot2d::drawLine( QPainter* painter, int x1, int y1, int x2, int y2,
                       Qt::PenStyle type, const QColor& color, int width )
{
  drawLine( painter, QPoint( x1, y1 ), QPoint( x2, y2 ), type, color, width );
}

/*!
  \brief Draw line.
  \param painter painter
  \param x1 X coordinate of the starting point
  \param y1 Y coordinate of the starting point
  \param x2 X coordinate of the ending point
  \param y2 Y coordinate of the ending point
  \param type line type
  \param color line color
  \param width line width
*/
void Plot2d::drawLine( QPainter* painter, int x1, int y1, int x2, int y2,
                       Plot2d::LineType type, const QColor& color, int width )
{
  drawLine( painter, QPoint( x1, y1 ), QPoint( x2, y2 ), 
            plot2qwtLine( type), color, width );
}

/*!
  \brief Draw marker.
  \param painter painter
  \param p central point
  \param r marker rectangle
  \param type marker type
  \param color marker color
*/
void Plot2d::drawMarker( QPainter* painter, const QPoint& p, const QRect& r,
                         QwtSymbol::Style type, const QColor& color )
{
  painter->save();
  painter->setPen( color );
  painter->setBrush( color );
  
  QRect ar = r;
  ar.moveCenter( p );
  const int w2 = ar.width()  / 2;
  const int h2 = ar.height() / 2;

  switch( type ) {
  case QwtSymbol::Ellipse:
    painter->drawEllipse( ar );
    break;
  case QwtSymbol::Rect:
    painter->drawRect( ar );
    painter->fillRect( ar, QBrush( color ) );
    break;
  case QwtSymbol::Diamond:
    {
      QPolygon polygon;
      polygon << QPoint( ar.x() + w2, ar.y() );
      polygon << QPoint( ar.right(), ar.y() + h2 );
      polygon << QPoint( ar.x() + w2, ar.bottom() );
      polygon << QPoint( ar.x(), ar.y() + h2 );
      painter->drawPolygon( polygon );
      break;
    }
  case QwtSymbol::Cross:
    painter->drawLine( ar.left() + w2, ar.top(), ar.left() + w2, ar.bottom() );
    painter->drawLine( ar.left(), ar.top() + h2, ar.right(), ar.top() + h2 );
    break;
  case QwtSymbol::XCross:
    painter->drawLine( ar.left(), ar.top(), ar.right(), ar.bottom() );
    painter->drawLine( ar.left(), ar.bottom(), ar.right(), ar.top() );
    break;
  case QwtSymbol::UTriangle:
    {
      QPolygon polygon;
      polygon << QPoint( ar.left() + w2, ar.top() );
      polygon << QPoint( ar.right(), ar.bottom() );
      polygon << QPoint( ar.left(), ar.bottom() );
      painter->drawPolygon( polygon );
      break;
    }
  case QwtSymbol::DTriangle:
    {
      QPolygon polygon;
      polygon << QPoint( ar.left() + w2, ar.bottom() );
      polygon << QPoint( ar.right(), ar.top() );
      polygon << QPoint( ar.left(), ar.top() );
      painter->drawPolygon( polygon );
      break;
    }
  case QwtSymbol::RTriangle:
    {
      QPolygon polygon;
      polygon << QPoint( ar.left(), ar.top() );
      polygon << QPoint( ar.right(), ar.top() + h2 );
      polygon << QPoint( ar.left(), ar.bottom() );
      painter->drawPolygon( polygon );
      break;
    }
  case QwtSymbol::LTriangle:
    {
      QPolygon polygon;
      polygon << QPoint( ar.left(), ar.top() + h2 );
      polygon << QPoint( ar.right(), ar.top() );
      polygon << QPoint( ar.right(), ar.bottom() );
      painter->drawPolygon( polygon );
      break;
    }
  default:
    break;
  }
  painter->restore();
}

/*!
  \brief Draw marker.
  \param painter painter
  \param p central point
  \param r marker rectangle
  \param type marker type
  \param color marker color
*/
void Plot2d::drawMarker( QPainter* painter, const QPoint& p, const QRect& r,
                         Plot2d::MarkerType type, const QColor& color )
{
  drawMarker( painter, p, r, plot2qwtMarker( type ), color ); 
}

/*!
  \brief Draw marker.
  \param painter painter
  \param x X coordinate of the central point
  \param y Y coordinate of the central point
  \param w marker rectangle width
  \param h marker rectangle height
  \param type marker type
  \param color marker color
*/
void Plot2d::drawMarker( QPainter* painter, int x, int y, int w, int h,
                         QwtSymbol::Style type, const QColor& color )
{
  drawMarker( painter, QPoint( x, y ), QRect( 0, 0, w, h ), type, color ); 
}

/*!
  \brief Draw marker.
  \param painter painter
  \param x X coordinate of the central point
  \param y Y coordinate of the central point
  \param w marker rectangle width
  \param h marker rectangle height
  \param type marker type
  \param color marker color
*/
void Plot2d::drawMarker( QPainter* painter, int x, int y, int w, int h,
                         Plot2d::MarkerType type, const QColor& color )
{
  drawMarker( painter, QPoint( x, y ), QRect( 0, 0, w, h ), plot2qwtMarker( type ), color ); 
}


/*!
  \brief Create icon pixmap according to the marker type.
  \param size icon size
  \param type marker type
  \param color icon color
  \return icon
*/
QPixmap Plot2d::markerIcon(const QSize &size, const QColor& color, Plot2d::MarkerType type )
{

  QPixmap px( size );
  px.fill( QColor( 255, 255, 255, 0 ) );
  QPainter p( &px );
  Plot2d::drawMarker( &p, size.width()/2, size.height()/2, MSIZE, MSIZE, type, color );
  return px;
}


/*!
  \brief Create icon pixmap according to the line type.
  \param size icon size
  \param type line type
  \param color icon color
  \return icon
*/
QPixmap Plot2d::lineIcon( const QSize& size,  const QColor& color, Plot2d::LineType type )
{

  QPixmap px( size );
  px.fill( QColor( 255, 255, 255, 0 ) );
  QPainter p( &px );
  drawLine( &p, 5, size.height()/2, size.width()-5, size.height()/2, type,
	    color, 1 );
  return px;
}

/*!
  Gets new unique marker for item if possible
*/
void Plot2d::getNextMarker( const int rtti, const QwtPlot* thePlot, QwtSymbol::Style& typeMarker,
			    QColor& color, Qt::PenStyle& typeLine ) 
{
  bool bOk = false;
  int cnt = 0;
  while ( !bOk ) {
    int aRed    = (int)( 256.0 * rand() / RAND_MAX );  // generate random color
    int aGreen  = (int)( 256.0 * rand() / RAND_MAX );  // ...
    int aBlue   = (int)( 256.0 * rand() / RAND_MAX );  // ...
    int aMarker = (int)( 9.0 * rand() / RAND_MAX ) + 1;// 9 markers types( not including empty )
    int aLine   = (int)( 5.0 * rand() / RAND_MAX ) + 1;// 5 line types ( not including empty )
    
    typeMarker = ( QwtSymbol::Style )aMarker;
    color      = QColor( aRed, aGreen, aBlue );
    typeLine   = ( Qt::PenStyle )aLine;
    
    bOk = ( ++cnt == MAX_ATTEMPTS ) || !existMarker( rtti, thePlot, typeMarker, color, typeLine );
  }
}

/*!
  Checks if marker belongs to any enitity
*/
bool Plot2d::existMarker( const int rtti, const QwtPlot* thePlot, const QwtSymbol::Style typeMarker,
			  const QColor& color, const Qt::PenStyle typeLine ) 
{
  bool ok = false;
  
  QColor bgColor = thePlot->palette().color( QPalette::Background );
  if ( closeColors( color, bgColor ) ) {
    ok = true;
  }
  else {
    QwtPlotItemList anItems = thePlot->itemList();
    QwtPlotItemIterator anIt = anItems.begin(), aLast = anItems.end();
    QwtPlotItem* anItem;
    for ( ; anIt != aLast && !ok; anIt++ ) {
      anItem = *anIt;
      if ( anItem && anItem->rtti() == rtti ) {
	QwtPlotCurve* crv = dynamic_cast<QwtPlotCurve*>( anItem );
	if ( crv ) {
	  QwtSymbol::Style aStyle = crv->symbol()->style();
	  QColor           aColor = crv->pen().color();
	  Qt::PenStyle     aLine  = crv->pen().style();
	  ok = closeColors( aColor, color ) && aStyle == typeMarker && aLine == typeLine;
	}
      }
    }
  }
  return ok;
}

/*!
  Checks if two colors are close to each other
  uses COLOR_DISTANCE variable as max tolerance for comparing of colors
*/

bool Plot2d::closeColors( const QColor& color1,
			  const QColor& color2,
			  int distance )
{
  long tol = 
    qAbs( color2.red()   - color1.red()   ) + 
    qAbs( color2.green() - color1.green() ) +
    qAbs( color2.blue()  - color1.blue()  ) -
    ( distance < 0 ? COLOR_DISTANCE : distance );

  return tol <= 0;
}
