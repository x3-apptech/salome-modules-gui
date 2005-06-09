#ifndef PLOT2D_CURVE_H
#define PLOT2D_CURVE_H

#include "Plot2d.h"

#include <qwt_plot.h>

#include <qcolor.h>
#include <qstring.h>
#include <qvaluelist.h>

typedef struct
{
  double x;
  double y;
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

  void        setHorTitle( const QString& title );
  QString     getHorTitle() const;
  void        setVerTitle( const QString& title );
  QString     getVerTitle() const;
  void        setHorUnits( const QString& units );
  QString     getHorUnits() const;
  void        setVerUnits( const QString& units );
  QString     getVerUnits() const;
  void        addPoint(double theX, double theY);
  void        insertPoint(int thePos, double theX, double theY);
  void        deletePoint(int thePos);
  void        clearAllPoints();
  pointList   getPointList() const;

  void        setData( const double* hData, const double* vData, long size );
  double*     horData() const;
  double*     verData() const;

  int         nbPoints() const;
  bool        isEmpty() const;

  void        setAutoAssign( bool on );
  bool        isAutoAssign() const;
  void        setColor( const QColor color );
  QColor      getColor() const;
  void        setMarker( MarkerType marker );
  MarkerType  getMarker() const;
  void        setLine( LineType line, const int lineWidth = 0 );
  LineType    getLine() const;
  int         getLineWidth() const;
  void        setYAxis(QwtPlot::Axis theYAxis);
  QwtPlot::Axis getYAxis() const;

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

#endif
