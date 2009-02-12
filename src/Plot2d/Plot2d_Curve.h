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
// File   : Plot2d_Curve.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef PLOT2D_CURVE_H
#define PLOT2D_CURVE_H

#include "Plot2d.h"

#include <QList>
#include <qwt_plot.h>

class QColor;

typedef struct
{
  double x;
  double y;
  QString text;
} Plot2d_Point;

typedef QList<Plot2d_Point> pointList;

class PLOT2D_EXPORT Plot2d_Curve
{
public:
  Plot2d_Curve();
  virtual ~Plot2d_Curve();
  Plot2d_Curve( const Plot2d_Curve& );
  Plot2d_Curve& operator= ( const Plot2d_Curve& );

  virtual QString    getTableTitle() const;
  
  void               setHorTitle( const QString& );
  QString            getHorTitle() const;
  void               setVerTitle( const QString& );
  QString            getVerTitle() const;

  void               setHorUnits( const QString& );
  QString            getHorUnits() const;
  void               setVerUnits( const QString& );
  QString            getVerUnits() const;

  void               addPoint( double, double, const QString& = QString() );
  void               insertPoint( int, double, double, const QString& = QString() );
  void               deletePoint( int );
  void               clearAllPoints();
  pointList          getPointList() const;

  void               setData( const double*, const double*, 
			      long, const QStringList& = QStringList() );
  double*            horData() const;
  double*            verData() const;

  void               setText( const int, const QString& );
  QString            text( const int ) const;

  int                nbPoints() const;
  bool               isEmpty() const;

  void               setAutoAssign( bool );
  bool               isAutoAssign() const;

  void               setColor( const QColor& );
  QColor             getColor() const;

  void               setMarker( Plot2d::MarkerType );
  Plot2d::MarkerType getMarker() const;

  void               setLine( Plot2d::LineType, const int = 0 );
  Plot2d::LineType   getLine() const;
  int                getLineWidth() const;

  void               setYAxis( QwtPlot::Axis );
  QwtPlot::Axis      getYAxis() const;

  // Protection against QwtCurve::drawLines() bug in Qwt 0.4.x: 
  // it crashes if switched to X/Y logarithmic mode, when one or more points have
  // non-positive X/Y coordinate
  double             getMinX() const;
  double             getMinY() const;

protected:
  bool               myAutoAssign;
  QString            myHorTitle;
  QString            myVerTitle;
  QString            myHorUnits;
  QString            myVerUnits;
  QColor             myColor;
  Plot2d::MarkerType myMarker;
  Plot2d::LineType   myLine;
  int                myLineWidth;
  QwtPlot::Axis      myYAxis;

  pointList          myPoints;
};

typedef QList<Plot2d_Curve*> curveList;

#endif
