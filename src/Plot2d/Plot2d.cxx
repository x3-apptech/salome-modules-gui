//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : Plot2d.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "Plot2d.h"

#include <QPainter>

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
