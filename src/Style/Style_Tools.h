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

// File   : Style_Tools.h
// Author : Natalia Ermolaeva, Open CASCADE S.A.S.
//
#ifndef STYLE_TOOLS_H
#define STYLE_TOOLS_H

#include "Style.h"

#include <QPainterPath>
#include <QRect>
#include <QColor>
#include <QStyle>

//
// This class is private for Style package.
// Normally it should not be exported.
//
class STYLE_SALOME_EXPORT Style_Tools
{
public:
  //! Rounding operation type
  typedef enum {
    None        = 0x00000000,       //!< No rounding required
    All         = 0x00000001,       //!< All corners
    Right       = 0x00000002,       //!< Right corners
    Left        = 0x00000004,       //!< Left corners
    TopLeft     = 0x00000008,       //!< Top-left corner
    TopRight    = 0x00000010,       //!< Top-right corner
    BottomLeft  = 0x00000020,       //!< Bottom-left corner
    BottomRight = 0x00000040        //!< Bottom-right corner
  } RoundType;

  //! Shadow type
  typedef enum {
    WholePath,        //!< Whole path
    BottomPath,       //!< Bottom path
    TopPath           //!< Top path
  } ShadowType;

  //! Slider type
  typedef enum {
    SlUp,             //!< Up-to-down gradient
    SlDown,           //!< Down-to-up gradient
    SlLeft,           //!< Left-to-right gradient
    SlRight,          //!< Right-to-left gradient
    SlNone            //!< No gradient
  } SliderType;

  static QPainterPath painterPath( const QRect& );
  static QPainterPath substractPath( const QPainterPath&, const QPainterPath& );

  static QPainterPath roundRect( const QRect&, const double, int = 0, int = 0 );

  static void         roundRect( QPainter*, const QRect&, const double, const int,
                                 const QColor&, const QColor&, bool = true, bool = true );

  static void         shadowRect( QPainter*, const QRect&, const double,
                                  const double, const int, int,
                                  const QColor&, const QColor&, const QColor&,
                                  const QColor&, const bool, const bool,
                                  const bool = false, const bool = true );
  static void         shadowCheck( QPainter*, const QRect&, const double, const int,
                                   const QColor&, const QColor&,
                                   const QColor&, const QColor& );
  static void         arrowRect( QPainter*, const QRect&, const QColor&,
                                 const QColor&, const QColor& );
  static void         fillRect( QPainter*, const QRect&, const QColor&,
                                const QColor&, const int = 255 );
  static void         drawArrow( QStyle::PrimitiveElement, QPainter*, const QRect&,
                                 const QColor&, const QColor& );
  static void         drawSign( QStyle::PrimitiveElement, QPainter*, const QRect&,
                                const QColor&, const QColor& );
  static QPainterPath tabRect( QPainter*, const QRect&, const int, const double,
                               const double, const QColor&, const QColor&,
                               const QColor&, const QColor&,
                               const bool, const bool, const bool,
                               const bool = false, const bool = true );
  static void         drawFocus( QPainter*, const QRect&, const double, const int,
                                 const QColor& );
  static void         drawFocus( QPainter*, const QPainterPath&, const QColor&,
                                 const bool = true );
  static void         drawSlider( QPainter*, const QRect&, const double,
                                  SliderType, const QColor&, const QColor&,
                                  const QColor&, const QColor& );
  static void         highlightRect( QPainter*, const QRect&, const double, const int,
                                     const double, const QColor&, const QColor&,
                                     const QColor& );
  static int          getMinDelta( const QRect&, const QSize&, const int );
  static int          getMaxRect( const QRect&, const int );
};

#endif // STYLE_TOOLS_H
