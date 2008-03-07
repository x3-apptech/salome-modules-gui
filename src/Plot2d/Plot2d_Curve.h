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
#ifndef PLOT2D_CURVE_H
#define PLOT2D_CURVE_H

#include "Plot2d.h"
#include <qvaluelist.h>
#include <qptrlist.h>
#include <qwt_plot.h>

class QColor;

typedef struct
{
  double x;
  double y;
  QString text;
} Plot2d_Point;

typedef QValueList<Plot2d_Point> pointList;

class PLOT2D_EXPORT Plot2d_Curve
{
public:
  enum MarkerType { None,      Circle,    Rectangle, Diamond,
        DTriangle, UTriangle, LTriangle, RTriangle,
        Cross, XCross };
  enum LineType { NoPen, Solid, Dash, Dot, DashDot, DashDotDot };

  Plot2d_Curve();
  virtual ~Plot2d_Curve();
  Plot2d_Curve( const Plot2d_Curve& curve );
  Plot2d_Curve& operator= ( const Plot2d_Curve& curve );

  virtual QString getTableTitle() const;
  
  void        setHorTitle( const QString& title );
  QString     getHorTitle() const;
  void        setVerTitle( const QString& title );
  QString     getVerTitle() const;
  void        setHorUnits( const QString& units );
  QString     getHorUnits() const;
  void        setVerUnits( const QString& units );
  QString     getVerUnits() const;
  void        addPoint(double theX, double theY, const QString& = QString::null );
  void        insertPoint(int thePos, double theX, double theY, const QString& = QString::null );
  void        deletePoint(int thePos);
  void        clearAllPoints();
  pointList   getPointList() const;

  void        setData( const double* hData, const double* vData, long size, const QStringList& = QStringList() );
  double*     horData() const;
  double*     verData() const;

  void        setText( const int, const QString& );
  QString     text( const int ) const;

  int         nbPoints() const;
  bool        isEmpty() const;

  void        setAutoAssign( bool on );
  bool        isAutoAssign() const;
  void        setColor( const QColor& color );
  QColor      getColor() const;
  void        setMarker( MarkerType marker );
  MarkerType  getMarker() const;
  void        setLine( LineType line, const int lineWidth = 0 );
  LineType    getLine() const;
  int         getLineWidth() const;
  void        setYAxis(QwtPlot::Axis theYAxis);
  QwtPlot::Axis getYAxis() const;

  // Protection against QwtCurve::drawLines() bug in Qwt 0.4.x: 
  // it crashes if switched to X/Y logarithmic mode, when one or more points have
  // non-positive X/Y coordinate
  double      getMinX() const;
  double      getMinY() const;

protected:
  bool        myAutoAssign;
  QString     myHorTitle;
  QString     myVerTitle;
  QString     myHorUnits;
  QString     myVerUnits;
  QColor      myColor;
  MarkerType  myMarker;
  LineType    myLine;
  int         myLineWidth;
  QwtPlot::Axis myYAxis;

  pointList   myPoints;
};

typedef QPtrList<Plot2d_Curve> curveList;

#endif
