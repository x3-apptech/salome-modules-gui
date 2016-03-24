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

// File   : Plot2d.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef PLOT2D_H
#define PLOT2D_H

#ifdef WIN32
#  if defined PLOT2D_EXPORTS || defined Plot2d_EXPORTS
#    define PLOT2D_EXPORT __declspec(dllexport)
#  else
#    define PLOT2D_EXPORT __declspec(dllimport)
#  endif
#else
#  define PLOT2D_EXPORT
#endif

#include <qwt_symbol.h>
#include <QString>
#include <QColor>
#include <QPixmap>

class QPainter;
class QwtPlot;

// Properties on the deviation marker.
#define PLOT2D_DEVIATION_COLOR "DEVIATION_COLOR"
#define PLOT2D_DEVIATION_LW "DEVIATION_LW"
#define PLOT2D_DEVIATION_TS "DEVIATION_TS"

struct PLOT2D_EXPORT Plot2d_Point
{
  double x;
  double y;
  double* deviationPtr;
  QString text;
  Plot2d_Point();
  Plot2d_Point( double theX, double theY, const QString& theText = QString() );
  ~Plot2d_Point();
  bool deviation(double& min, double& max) const;
  bool hasDeviation() const;
  void setDeviation(double min, double max);
  void clearDeviation();
  bool minDeviation(double& min) const;
  bool maxDeviation(double& max) const;
};

typedef QList<Plot2d_Point> pointList;


namespace Plot2d
{
  typedef enum { None, 
                 Circle,  
                 Rectangle,
                 Diamond,
                 DTriangle,
                 UTriangle,
                 LTriangle,
                 RTriangle,
                 Cross,
                 XCross
  } MarkerType;

  typedef enum {
    NoPen,
    Solid, 
    Dash, 
    Dot, 
    DashDot, 
    DashDotDot
  } LineType;

  QwtSymbol::Style         plot2qwtMarker( MarkerType );
  MarkerType qwt2plotMarker( QwtSymbol::Style );

  Qt::PenStyle             plot2qwtLine( LineType );
  LineType                 qwt2plotLine( Qt::PenStyle );

  void                     drawLine( QPainter*, const QPoint&, const QPoint&, 
                                     Qt::PenStyle = Qt::SolidLine, 
                                     const QColor& = Qt::black, int = 0 );
  void                     drawLine( QPainter*, const QPoint&, const QPoint&, 
                                     LineType = Solid, 
                                     const QColor& = Qt::black, int = 0 );
  void                     drawLine( QPainter*, int, int, int, int,
                                     Qt::PenStyle = Qt::SolidLine, 
                                     const QColor& = Qt::black, int = 0 );
  void                     drawLine( QPainter*, int, int, int, int,
                                     LineType = Solid, 
                                     const QColor& = Qt::black, int = 0 );

  void                     drawMarker( QPainter*, const QPoint&, const QRect&,
                                       QwtSymbol::Style = QwtSymbol::Ellipse,
                                       const QColor& = Qt::black );
  void                     drawMarker( QPainter*, const QPoint&, const QRect&,
                                       MarkerType = Circle,
                                       const QColor& = Qt::black );
  void                     drawMarker( QPainter*, int, int, int, int,
                                       QwtSymbol::Style = QwtSymbol::Ellipse,
                                       const QColor& = Qt::black );
  void                     drawMarker( QPainter*, int, int, int, int,
                                       MarkerType = Circle,
                                       const QColor& = Qt::black );
  
  QPixmap                  markerIcon( const QSize&, const QColor&, 
				      Plot2d::MarkerType );

  QPixmap                  lineIcon( const QSize&,  const QColor&, 
				     Plot2d::LineType );

  void                     getNextMarker( const int rtti, const QwtPlot*, QwtSymbol::Style&,
					  QColor&, Qt::PenStyle& );
  
  bool                     existMarker( const int rtti , const QwtPlot*, const QwtSymbol::Style,
					const QColor&, const Qt::PenStyle );


  bool                     closeColors( const QColor&, 
					const QColor&, 
					int distance = -1 );

}

#if defined WIN32
#  pragma warning ( disable: 4251 )
#endif

#endif // PLOT2D_H
