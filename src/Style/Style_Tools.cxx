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

// File   : Style_Tools.cxx
// Author : Natalia Ermolaeva, Open CASCADE S.A.S.
//
#include "Style_Tools.h"

#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QColor>
#include <QSize>
#include <QTabWidget>

/*!
  \class Style_Tools
  \brief A set of utility functions used by SALOME style to draw widgets
*/

/*!
  \brief Create painter path for specified rectangle
  \param r rectangle on which painter path is build
  \return resulting painter path
*/
QPainterPath Style_Tools::painterPath( const QRect& r )
{
  QPainterPath res;
  double x = r.x(), y = r.y(), right = r.right(), bottom = r.bottom();
  res.moveTo( x, y );
  res.lineTo( right, y );
  res.lineTo( right, bottom );
  res.lineTo( x, bottom );
  res.closeSubpath();
  return res;
}

/*!
  \brief Create painter path by subtracting painter path \a path from another
  painter path \a fromPath
  \param fromPath initial painter path
  \param path painter path being subtracted from \a fromPath
  \return resulting painter path
*/
QPainterPath Style_Tools::substractPath( const QPainterPath& fromPath,
                                         const QPainterPath& path )
{
  QPainterPath res;
  QRegion aFromRegion( fromPath.toFillPolygon().toPolygon() );
  QRegion aRegion( path.toFillPolygon().toPolygon() );
  aFromRegion -= aRegion;
  res.addRegion( aFromRegion );
  return res;
}

/*!
  \brief Create painter path basing on specified rectangle \a r with rounded corners
  specified by radius \a r, operation \a type and shadow type \a shType.
  \param r initial rectangle
  \param rad rounding radius
  \param type rounding type operation (Style_Tools::RoundType)
  \param shType shadow type (Style_Tools::ShadowType)
  \return resulting painter path
*/
QPainterPath Style_Tools::roundRect( const QRect& r, const double rad, int type,
                                     int shType )
{
  QPainterPath res;
  if ( type & None )
    return painterPath( r );
  double x = r.x(), y = r.y(), right = r.right(), bottom = r.bottom();
  int aRad = getMaxRect( r, (int)rad );

  if ( shType == WholePath || shType == TopPath ) {
    if ( type & ( All | Left | BottomLeft ) ) {
      res.moveTo( x+aRad, bottom );
      res.arcTo( x, bottom-2*aRad, 2*aRad, 2*aRad, 270, -90 );
    }
    else
      res.moveTo( x, bottom );
    res.lineTo( x, y+aRad );
    if ( type & (All | Left | TopLeft ) )
      res.arcTo( x, y, 2*aRad, 2*aRad, 180, -90 );
    else
      res.lineTo( x, y );
    res.lineTo( right-aRad, y );
    if ( type & ( All | Right | TopRight ) )
      res.arcTo( right-2*aRad, y, 2*aRad, 2*aRad, 90, -90 );
    else
      res.lineTo( right, y );
  }
  if ( shType == WholePath || shType == BottomPath ) {
    if ( shType == BottomPath ) {
      if ( type & ( All | Right | TopRight ) )
        res.moveTo( right, y+aRad );
      else
        res.moveTo( right, y );
    }
    res.lineTo( right, bottom-aRad );
    if ( type & ( All | Right | BottomRight ) )
      res.arcTo( right-2*aRad, bottom-2*aRad, 2*aRad, 2*aRad, 0, -90 );
    else
      res.lineTo( right, bottom );

    if ( type & ( All | Left | BottomLeft ) )
      res.lineTo( x+aRad, bottom );
    else
      res.lineTo( x, bottom ); 
  }
  if ( shType == WholePath )
    res.closeSubpath();
  return res;
}

/*!
  \brief Draw rectangle with rounded corners.
  \param p painter
  \param r drawing rectangle
  \param rad corner rounding radius
  \param type rounding type operation (Style_Tools::RoundType)
  \param c1 first gradient color
  \param c2 second gradient color
  \param fill if \c true rectangle is filled with gradiented background according to \a c1 and \a c2
  \param antial if \c true, rectangle corners are anti-aliased
*/
void Style_Tools::roundRect( QPainter* p, const QRect& r, const double rad, const int type,
                             const QColor& c1, const QColor& c2, bool fill, bool antial )
{
  if ( antial )
    p->setRenderHint( QPainter::Antialiasing );

  QPainterPath path = roundRect( r, rad, type );

  if ( fill ) {
    QLinearGradient gr( r.x(), r.y(), r.x(), r.bottom() );
    gr.setColorAt( 0.0, c1 );
    gr.setColorAt( 0.5, c1 );
    gr.setColorAt( 1.0, c2 );
    p->fillPath( path, gr );
  }
  p->strokePath( path, QPen( c2, Qt::SolidLine ) );
}

/*!
  \brief Draw rectangle with rounded corners and shadow.
  \param p painter
  \param r drawing rectangle
  \param rad corner rounding radius
  \param marg drawing margin
  \param shad shadow size
  \param type rounding type operation (Style_Tools::RoundType)
  \param light background's first gradient color
  \param dark background's second gradient color
  \param border_top top-left border's color
  \param border_bot bottom-right border's color
  \param antialize if \c true, rectangle corners are anti-aliased
  \param isButton \c true if button is drawn
  \param btnOn \c true if button is drawn and it is pressed
  \param fill if \c true rectangle is filled with gradiented background according to \a light and \a dark
*/
void Style_Tools::shadowRect( QPainter* p, const QRect& r, const double rad, const double marg,
                              const int shad, int type, const QColor& light, const QColor& dark,
                              const QColor& border_top, const QColor& border_bot, const bool antialize,
                              const bool isButton, const bool btnOn, const bool fill )
{
  QColor aLight = light, aDark = dark, aBrdTop = border_top, aBrdBot = border_bot;
  if ( isButton && btnOn ) {
    aLight = dark;
    aDark = light;
    aBrdTop = border_bot;
    aBrdBot = border_top;
  }

  // gradients
  QLinearGradient
    gr1( r.x(), r.y(), r.right(), r.y() ),
    gr2( r.x(), r.y(), r.x(), r.bottom() );
  if ( fill ) {
    if ( isButton ) {
      gr2.setColorAt( 0.0, aLight );
      if ( marg > 0 )
        gr2.setColorAt( marg / r.height(), aLight );
      gr2.setColorAt( 1.0, aDark );
    }
    else {
      QColor _c1 = aLight; _c1.setAlpha( 0 );
      QColor _c2 = aDark;
      gr1.setColorAt( 0.0, _c2 );
      if ( marg > 0 )
        gr1.setColorAt( marg / r.width(), _c1 );
      gr1.setColorAt( 1.0, _c1 );
      gr2.setColorAt( 0.0, _c2 );
      if ( marg > 0 )
        gr2.setColorAt( marg / r.height(), _c1 );
      gr2.setColorAt( 1.0, _c1 );
    }
  }
  // paths
  int x = r.x(), y = r.y(), right = r.right(), bottom = r.bottom();
  // main path
  QRect rCont;
  if ( antialize )
   rCont = QRect( QPoint( x+shad, y+shad ), QPoint( right-shad, bottom-shad ) );
  else
    rCont = QRect( QPoint( x, y ), QPoint( right, bottom ) );

  QPainterPath path = roundRect( rCont, rad, type );

  // draw paths
  p->save();
  if ( antialize )
    p->setRenderHint( QPainter::Antialiasing, true );
  else
    p->setRenderHint( QPainter::Antialiasing, false );

  if( fill ) {
    p->fillPath( path, aLight );
    if ( !isButton )
      p->fillPath( path, gr1 );
    p->fillPath( path, gr2 );
  }
  if ( antialize )
    p->strokePath( path, QPen( aBrdBot, Qt::SolidLine ) );

  QPainterPath bottomPath = roundRect( rCont, rad, type, BottomPath ),
               topPath = roundRect( rCont, rad, type, TopPath );
  p->strokePath( bottomPath, aBrdBot );
  p->strokePath( topPath, aBrdTop );

  if ( antialize )
      p->setRenderHint( QPainter::Antialiasing, false );
  p->restore();
}

/*!
  \brief Draw shadow of the check-mark
  \param p painter
  \param r drawing rectangle
  \param rad corner rounding radius
  \param type rounding type operation (Style_Tools::RoundType)
  \param light background's first gradient color
  \param dark background's second gradient color
  \param border_top top-left border's color
  \param border_bot bottom-right border's color
*/
void Style_Tools::shadowCheck( QPainter* p, const QRect& r, const double rad, const int type,
                               const QColor& light, const QColor& dark,
                               const QColor& border_top, const QColor& border_bot  )
{
  // main path
  QPainterPath path = roundRect( r, rad, type );

  QLinearGradient gr( r.x(), r.y(), r.right(), r.bottom() );
  gr.setColorAt( 0.0, dark );
  gr.setColorAt( 1.0, light );
 
  p->fillPath( path, gr ); //fill

  QPainterPath bottomPath = roundRect( r, rad, type, BottomPath ),
               topPath = roundRect( r, rad, type, TopPath );
  p->strokePath( bottomPath, border_bot );
  p->strokePath( topPath, border_top );
}

/*!
  \brief Draw rectnagle arrow
  \param p painter
  \param re drawing rectangle
  \param frame frame color
  \param gr1 background's first gradient color
  \param gr2 background's second gradient color
*/
void Style_Tools::arrowRect( QPainter* p, const QRect& re, const QColor& frame,
                             const QColor& gr1, const QColor& gr2 )
{
  QPainterPath path;
  const int r = 4;
  const double d = re.height()/2;

  path.moveTo( re.x()+r,            re.y() );
  path.lineTo( re.x()+re.width(),   re.y() );
  path.lineTo( re.x()+re.width()+d, re.y()+d );
  path.lineTo( re.x()+re.width(),   re.y()+re.height() );
  path.lineTo( re.x()+r,            re.y()+re.height() );
  path.arcTo ( re.x(),              re.y()+2*d-2*r, 2*r, 2*r, 270, -90 );
  path.lineTo( re.x(),              re.y()+r );
  path.arcTo ( re.x(),              re.y(), 2*r, 2*r, 180, -90 );
  path.closeSubpath();

  QLinearGradient gr( re.x(), re.y(), re.x(), re.y()+re.height() );
  gr.setColorAt( 0.0, gr1 );
  gr.setColorAt( 0.4, gr2 );
  gr.setColorAt( 0.6, gr2 );
  gr.setColorAt( 1.0, gr1 );

  p->fillPath( path, gr );
  p->strokePath( path, QPen( frame, Qt::SolidLine ) );
}

/*!
  \brief Fill rectangle with gradiented background
  \param p painter
  \param re drawing rectangle
  \param c1 background's first gradient color
  \param c2 background's second gradient color
*/
void Style_Tools::fillRect( QPainter* p, const QRect& re, const QColor& c1,
                            const QColor& c2, const int alpha )
{
  QLinearGradient gr( re.x(), re.y(), re.x()+re.width(), re.y()+re.height() );
  QColor cc1 = c1, cc2 = c2;
  cc1.setAlpha( alpha );
  cc2.setAlpha( alpha );
  gr.setColorAt( 0.0, cc1 );
  gr.setColorAt( 1.0, cc2 );
  p->fillRect( re, gr );
}

/*!
  \brief Draw arrow (for example, for combo-box drop-down menu button)
  \param type primitive type
  \param p painter
  \param r drawing rectangle
  \param pen foreground painter pen
  \param brush background painter brush
*/
void Style_Tools::drawArrow( QStyle::PrimitiveElement type, QPainter* p, const QRect& r,
                             const QColor& pen, const QColor& brush )
{
  p->save();
  QPainterPath arrow1;
  int x = r.x(), y = r.y(), w = r.right()-x, h = r.bottom()-y;
  int x11 = 0, x12 = 0, y11 = 0, y12 = 0;
  int aDelta = qMin( (int)(w/3.5), (int)(h/3.5) );
  int deltaX = aDelta, deltaY = aDelta;
  QLineF line( 0, 0, 1, 0 );
  int xc = r.center().x(), yc = r.center().y();
  p->translate( xc, yc );
  bool correct = false;
  switch( type ) {
    case QStyle::PE_IndicatorArrowDown:
      correct = true;
    case QStyle::PE_IndicatorArrowUp: {
      int widthArr2 = (int)(deltaX/3.);
      if ( correct )
        deltaY = -deltaY; // change arrow direction
      arrow1.moveTo(  0,          deltaY );
      arrow1.lineTo(  widthArr2,  deltaY );
      arrow1.lineTo(  widthArr2,  0 );
      arrow1.lineTo(  deltaX,     0 );
      arrow1.lineTo(  0,          -deltaY );
      arrow1.lineTo( -deltaX,     0. );
      arrow1.lineTo( -widthArr2,  0 );
      arrow1.lineTo( -widthArr2, deltaY );
      arrow1.lineTo(  0,         deltaY );
      if ( correct )
        deltaX = -deltaX;
      x11 = -deltaX; y11 = -deltaY; x12 = deltaX; y12 = deltaY;
      break;
    }
    case QStyle::PE_IndicatorArrowLeft: // to change
      correct = true;
    case QStyle::PE_IndicatorArrowRight: {
      int widthArr2 = (int)(deltaX/3.);
      if ( correct )
        deltaX = -deltaX; // change arrow direction
      arrow1.moveTo( -deltaX,  0 );
      arrow1.lineTo( -deltaX,  widthArr2 );
      arrow1.lineTo(  0,       widthArr2 );
      arrow1.lineTo(  0,       deltaY );
      arrow1.lineTo(  deltaX,  0 );
      arrow1.lineTo(  0,      -deltaY );
      arrow1.lineTo(  0,      -widthArr2 );
      arrow1.lineTo( -deltaX, -widthArr2 );
      arrow1.lineTo( -deltaX,  0 );
      x11 = deltaX; y11 = deltaY; x12 = -deltaX; y12 = -deltaY;
      break;
    }
    case QStyle::PE_IndicatorSpinDown:
      correct = true;
    case QStyle::PE_IndicatorSpinUp: {
      aDelta = (int)(deltaY/2);
      if ( correct ) {
        aDelta = (int)(-aDelta/2);
        deltaY = -deltaY;
      }
      arrow1.moveTo(  deltaX,     -aDelta );
      arrow1.lineTo(  0,          -aDelta-deltaY );
      arrow1.lineTo( -deltaX,     -aDelta );
      arrow1.lineTo(  deltaX,     -aDelta );
      if ( correct )
        deltaX = -deltaX;
      x11 = -deltaX, y11 = -deltaY, x12 = deltaX, y12 = 0;
      break;
    }
    default:
      p->restore();
      return;
  }
  p->setPen( pen );
  p->setBrush( brush );

  QLinearGradient gr( x11, y11, x12, y12 );
  gr.setColorAt( 0.0, pen );                // grayer
  gr.setColorAt( 1.0, brush);               // lighter
  p->fillPath( arrow1, gr );
  p->strokePath( arrow1, QPen( pen, Qt::SolidLine ) );

  p->restore();
}

/*!
  \brief Draw indicator (for example, for spin box's increment/decrement buttons)
  \param type primitive type
  \param p painter
  \param r drawing rectangle
  \param pen foreground painter pen
  \param brush background painter brush
*/
void Style_Tools::drawSign( QStyle::PrimitiveElement type, QPainter* p, const QRect& r,
                            const QColor& pen, const QColor& brush )
{
  p->save();
  QPainterPath sign;
  int x = r.x(), y = r.y(), w = r.right()-x, h = r.bottom()-y;
  int x11 = 0, x12 = 0, y11 = 0, y12 = 0;
  int aDelta = qMin( (int)(w/3.5), (int)(h/3.5) );
  int deltaX = aDelta, deltaY = aDelta;
  QLineF line( 0, 0, 1, 0 );
  int xc = r.center().x(), yc = r.center().y();
  p->translate( xc, yc );
  bool correct = false;
  switch( type ) {
    case QStyle::PE_IndicatorSpinMinus:
      correct = true;
    case QStyle::PE_IndicatorSpinPlus: {
      aDelta = (int)(deltaY/2);
      if ( correct ) {
        aDelta = -aDelta;
        deltaY = -deltaY;
      }
      if ( correct ) 
        sign.moveTo(  deltaY/2,   -aDelta/2-(deltaY/2-aDelta/3) );
      else {
        sign.moveTo(  aDelta/3,   -aDelta/2 );
        sign.lineTo(  aDelta/3,   -aDelta/2-(deltaY/2-aDelta/3) );
        sign.lineTo(  deltaY/2,   -aDelta/2-(deltaY/2-aDelta/3) );
      }
      
      sign.lineTo(    deltaY/2,   -aDelta/2-(deltaY/2-aDelta/3)-2*aDelta/3 );
      
      if ( !correct ) {
        sign.lineTo(  aDelta/3,   -aDelta/2-(deltaY/2-aDelta/3)-2*aDelta/3 );
        sign.lineTo(  aDelta/3,   -aDelta/2-deltaY );
        sign.lineTo( -aDelta/3,   -aDelta/2-deltaY );
        sign.lineTo( -aDelta/3,   -aDelta/2-(deltaY/2-aDelta/3)-2*aDelta/3 );
      }
      
      sign.lineTo(   -deltaY/2,   -aDelta/2-(deltaY/2-aDelta/3)-2*aDelta/3 );
      sign.lineTo(   -deltaY/2,   -aDelta/2-(deltaY/2-aDelta/3) );

      if ( correct ) 
        sign.lineTo(  deltaY/2,   -aDelta/2-(deltaY/2-aDelta/3) );
      else {
        sign.lineTo( -aDelta/3,   -aDelta/2-(deltaY/2-aDelta/3) );
        sign.lineTo( -aDelta/3,   -aDelta/2 );
        sign.lineTo(  aDelta/3,   -aDelta/2);
      }

      if ( correct )
        deltaX = -deltaX;
      x11 = -deltaX, y11 = -deltaY, x12 = deltaX, y12 = 0;
      break;
    }
    default:
      p->restore();
      return;
  }
  p->setPen( pen );
  p->setBrush( brush );

  QLinearGradient gr( x11, y11, x12, y12 );
  gr.setColorAt( 0.0, pen );                // grayer
  gr.setColorAt( 1.0, brush);               // lighter
  p->fillPath( sign, gr );
  p->strokePath( sign, QPen( pen, Qt::SolidLine ) );

  p->restore();
}

/*!
  \brief Create painter path for tab bar and optionally draw it
  \param p painter
  \param r drawing rectangle
  \param position tab position
  \param rad rounding radius
  \param delta gap between tabs
  \param light background's first gradient color
  \param dark background's second gradient color
  \param border_top top-left border's color
  \param border_bot bottom-right border's color
  \param selected \c true if tab is selected
  \param isLast \c true if tab is last in the tabs list
  \param isHover \c true if tab is hovered
  \param focusRect focus rectangle
  \param draw if \c true, tab bar is drawn
  \return tab bar's painter path
*/
QPainterPath Style_Tools::tabRect( QPainter* p, const QRect& r, const int position, const double rad,
                                   const double delta, const QColor& light, const QColor& dark,
                                   const QColor& border_top, const QColor& border_bot,
                                   const bool selected, const bool isLast,  const bool isHover,
                                   const bool focusRect, const bool draw )
{
  // paths
  int x = r.x()+1, y = r.y()+1, right = r.right()+1, left = r.left()-1,
      bottom = r.bottom()-1, top = r.top()+1;
  int aDelta;
  if ( position == QTabWidget::North || position == QTabWidget::South )
    aDelta = (int)((bottom - top)*delta/2);
  else
    aDelta = (int)((right-left)*delta/2);  

  int aRad = getMaxRect( r, (int)rad );

  // main path
  QPainterPath path, strokePath;
  switch ( position ) {
    case QTabWidget::North: {
      path.moveTo( x, bottom );
      path.lineTo( x, y+aRad );
      path.arcTo( x, y, 2*aRad, 2*aRad, 180, -90 );
      if ( isLast )
        right = right - aDelta;
      path.lineTo( right-aDelta, y );
      path.lineTo( right+aDelta, bottom );
      strokePath.moveTo( x, bottom );
      strokePath.lineTo( right+aDelta, bottom );
      break;
    }
    case QTabWidget::South: {
      path.moveTo( x, y );
      path.lineTo( x, bottom-aRad );
      path.arcTo( x, bottom-2*aRad, 2*aRad, 2*aRad, 180, 90 );
      if ( isLast )
        right = right - aDelta;
      path.lineTo( right-aDelta, bottom );
      path.lineTo( right+aDelta, y );
      strokePath.moveTo( x, y );
      strokePath.lineTo( right+aDelta, y );
      break;
    }
    case QTabWidget::West: {
      path.moveTo( right, y );
      path.lineTo( x+aRad, y );
      path.arcTo( x, y, 2*aRad, 2*aRad, 90, 90 );
      if ( isLast )
        bottom = bottom - aDelta;
      path.lineTo( x, bottom-aDelta );
      path.lineTo( right, bottom+aDelta );
      strokePath.moveTo( right, y );
      strokePath.lineTo( right, bottom+aDelta );
      break;
    }
    case QTabWidget::East: {
      path.moveTo( x, y );
      path.lineTo( right-aRad, y );
      path.arcTo( right-2*aRad, y, 2*aRad, 2*aRad, 90, -90 );
      if ( isLast )
        bottom = bottom - aDelta;
      path.lineTo( right, bottom-aDelta );
      path.lineTo( x, bottom+aDelta );
      strokePath.moveTo( x, y );
      strokePath.lineTo( x, bottom+aDelta );
      break;
    }
  }
  if ( !draw )
    return path;

  // draw path
  p->save();
  p->setRenderHint( QPainter::Antialiasing, true );
  if ( focusRect ) {
    path.closeSubpath();
    drawFocus( p, path, dark, false );
  }
  else {
    QLinearGradient gr( x, y, x, bottom );
    if ( selected ) {
      gr.setColorAt( 0.0, light );
      gr.setColorAt( 1.0, light );
    }
    else if ( isHover ) {
      gr.setColorAt( 0.0, dark );
      gr.setColorAt( 0.2, light );
      gr.setColorAt( 0.8, light );
      gr.setColorAt( 1.0, dark );
      QLinearGradient gr_1( x, y, right, y );
      QColor aLight = light;
      aLight.setAlpha( 0 );
      gr_1.setColorAt( 0.0, dark );
      gr_1.setColorAt( 0.2, aLight );
      gr_1.setColorAt( 0.7, aLight );
      gr_1.setColorAt( 1.0, dark );
      p->fillPath( path, gr );
      p->fillPath( path, gr_1 );
    }
    else {
      gr.setColorAt( 0.0, light );
      gr.setColorAt( 0.3, dark );
      gr.setColorAt( 0.7, dark );
      gr.setColorAt( 1.0, light );
    }
    if ( !isHover || selected )
      p->fillPath( path, gr );

    QColor bordCol = border_top;
    if ( position == QTabWidget::South || position == QTabWidget::East )
      bordCol = border_bot;
    else
      bordCol = border_top;
    p->strokePath( path, QPen( bordCol, Qt::SolidLine ) );
    p->setRenderHint( QPainter::Antialiasing, false );
    p->strokePath( strokePath, QPen( selected ? light : bordCol, Qt::SolidLine ) );
  }
  p->restore();
  return path;
}

/*!
  \brief Draw widget's focus
  \param p painter
  \param aRect drawing rectangle
  \param rad rounding radius
  \param type rounding operation type
  \param border focus rectangle color
*/
void Style_Tools::drawFocus( QPainter* p, const QRect& aRect, const double rad, const int type,
                             const QColor& border )
{
  QPainterPath path = Style_Tools::roundRect( aRect, rad, type );

  drawFocus( p, path, border );
}

/*!
  \brief Draw widget's focus
  \param p painter
  \param path drawing painter path
  \param border focus rectangle color
  \param line if \c true, focus is drawn as dotted line
*/
void Style_Tools::drawFocus( QPainter* p, const QPainterPath& path, const QColor& border,
                             const bool line )
{
  QPen oldPen = p->pen();
  QPen aPen = oldPen;
  aPen.setColor( border );
  if ( !line )
    aPen.setStyle( Qt::DotLine );
  p->setPen( aPen );

  p->drawPath( path );
  p->setPen( oldPen );
}

/*!
  \brief Draw slider
  \param p painter
  \param r drawing rectangle
  \param rad rounding radius
  \param slider slider type
  \param light background's first gradient color
  \param dark background's second gradient color
  \param border_top top-left border's color
  \param border_bot bottom-right border's color
*/
void Style_Tools::drawSlider( QPainter* p, const QRect& r, const double rad,
                              SliderType type, const QColor& light, const QColor& dark,
                              const QColor& border_top, const QColor& border_bot )
{
  p->save();
  QPainterPath path, bottomPath;
  if ( rad == 0 )
    type = SlNone;
  int aRad = getMaxRect( r, (int)rad );
  int w = r.width(), h = r.height();
  int xc = r.center().x(), yc = r.center().y();
  p->translate( xc, yc );
  QLinearGradient gr;
  switch ( type ) {
    case SlUp:
    case SlDown: {
      int aDeltaX = (int)(w/2), aDeltaY = (int)(h/4), aDeltaArr = (int)(aDeltaY/2);
      if ( type == SlDown )
        p->rotate( 180 );
      path.moveTo(  0,       -2*aDeltaY );
      path.lineTo(  aDeltaX, -aDeltaY+aDeltaArr );
      path.lineTo(  aDeltaX, 2*aDeltaY-aRad );
      path.arcTo(   aDeltaX-2*aRad, 2*aDeltaY-2*aRad, 2*aRad, 2*aRad, 0, -90 );
      path.lineTo( -aDeltaX+aRad, 2*aDeltaY );
      path.arcTo(  -aDeltaX,  2*aDeltaY-2*aRad, 2*aRad, 2*aRad, 270, -90 );
      path.lineTo( -aDeltaX, -aDeltaY+aDeltaArr );
      path.lineTo( 0,       -2*aDeltaY );
      bottomPath = path;
      gr.setStart( 0, -2*aDeltaY );
      gr.setFinalStop( 0, 2*aDeltaY );
      break;
    }
    case SlLeft:
    case SlRight: {
      int aDeltaX = (int)(w/4), aDeltaY = (int)(h/2), aDeltaArr = (int)(aDeltaX/2);
      if ( type == SlRight )
        p->rotate( 180 );
      path.moveTo( -2*aDeltaX,        0 );
      path.lineTo( -aDeltaX+aDeltaArr,aDeltaY );
      path.lineTo(  2*aDeltaX-aRad,   aDeltaY );
      path.arcTo(   2*aDeltaX-2*aRad, aDeltaY-2*aRad, 2*aRad, 2*aRad, 270, 90 );
      path.lineTo(  2*aDeltaX,       -aDeltaY+aRad );
      path.arcTo(   2*aDeltaX-2*aRad, -aDeltaY, 2*aRad, 2*aRad, 0, 90 );
      path.lineTo( -aDeltaX+aDeltaArr,-aDeltaY );
      path.lineTo( -2*aDeltaX,       0 );
      gr.setStart( -2*aDeltaX, 0 );
      gr.setFinalStop( 2*aDeltaX, 0 );
      bottomPath = path;
      break;
    }
    case SlNone: {
      p->translate( -xc, -yc );
      path = Style_Tools::roundRect( r, rad, Style_Tools::All );
      bottomPath = path;
      gr.setStart( r.x(), r.y() );
      gr.setFinalStop( r.x(), r.bottom() );
      break;
    }
    default:
      return;
  }
  p->setRenderHint( QPainter::Antialiasing );
  gr.setColorAt( 0.0, light );
  gr.setColorAt( 1.0, dark );
  p->fillPath( path, gr );

  QColor bottomShadow = border_bot;
  p->strokePath( bottomPath, bottomShadow );
  p->setRenderHint( QPainter::Antialiasing, false );
  p->restore();
}

/*!
  \brief Draw highlighted rectangle
  \param p painter
  \param rect drawing rectangle
  \param rad rounding radius
  \param type rounding operation type
  \parma marg margin size
  \param center background's center gradient color
  \param out_center background's second color
  \param border border color
*/
void Style_Tools::highlightRect( QPainter* p, const QRect& rect, const double rad, const int type,
                                 const double marg, const QColor& center, const QColor& out_center,
                                 const QColor& border )
{
  QPainterPath path = Style_Tools::roundRect( rect, rad, type );
  QLinearGradient gr_h( rect.x(), rect.y(), rect.right(), rect.y() ),
                  gr_v( rect.x(), rect.y(), rect.x(), rect.bottom() );
  double aXDelta = marg > 0 ? marg/rect.width() : 0.5;
  double aYDelta = marg > 0 ? marg/rect.height() : 0.5;
  if ( aYDelta > 0.5 )
    aYDelta = 0.25;
  gr_v.setColorAt( 0.0, out_center );
  gr_v.setColorAt( aYDelta, center );
  gr_v.setColorAt( 1-aYDelta, center );
  gr_v.setColorAt( 1.0, out_center );

  QColor aCenter = center;
  aCenter.setAlpha( 0 );
  if ( aXDelta > 0.5 )
    aXDelta = 0.25;
  gr_h.setColorAt( 0.0, out_center );
  gr_h.setColorAt( aXDelta, aCenter );
  gr_h.setColorAt( 1-aXDelta, aCenter );
  gr_h.setColorAt( 1.0, out_center );

  p->fillPath( path, gr_v );
  if ( marg > 0 )
    p->fillPath( path, gr_h );
  p->strokePath( path, border );
}

/*!
  \brief Get minimal delta value (the minimum between \a rect and \a size dimensions)
  \param rect rectangle
  \param size size
  \param defDelta default minimum delta
  \return resulting minimum value
*/
int Style_Tools::getMinDelta( const QRect& rect, const QSize& size, const int defDelta )
{
  int aDelta = defDelta;
  aDelta = qMin( aDelta, ( rect.height() - size.height() ) / 2 );
  aDelta = qMin( aDelta, ( rect.width()  - size.width() )  / 2 );
  return aDelta;
}

/*!
  \brief Get halved size of the quadrangle covering specified rectangle
  \param rect rectangle
  \param defRect default quadranle size value
  \return resulting value
*/
int Style_Tools::getMaxRect( const QRect& rect, const int defRect )
{
  int aRect = defRect;
  aRect = qMin( aRect, rect.height() / 2 );
  aRect = qMin( aRect, rect.width()  / 2 );
  return aRect;
}
