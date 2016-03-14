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

#ifndef PLOT2D_VIEWFRAME_H
#define PLOT2D_VIEWFRAME_H

#include "Plot2d.h"
#include "Plot2d_Curve.h"
#ifndef NO_ANALYTICAL_CURVES
#include "Plot2d_AnalyticalCurve.h"
#endif
#include "Plot2d_NormalizeAlgorithm.h"

#include <QWidget>
#include <QMultiHash>
#include <QMap>
#include <QList>
#include <QPainter>
#include <qwt_symbol.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_canvas.h>
#include <qwt_legend.h>

#include <iostream>
#include <ostream>

class Plot2d_Plot2d;
class Plot2d_Prs;
class Plot2d_Curve;
class Plot2d_Object;
class QCustomEvent;
class QwtPlotItem;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtPlotZoomer;
class Plot2d_AxisScaleDraw;
class Plot2d_QwtPlotPicker;

typedef QMultiHash<QwtPlotCurve*, Plot2d_Curve*>  CurveDict;
typedef QMultiHash<QwtPlotItem*,  Plot2d_Object*> ObjectDict;

class PLOT2D_EXPORT Plot2d_ViewFrame : public QWidget
#ifndef NO_ANALYTICAL_CURVES
  , public Plot2d_CurveContainer
#endif
{
  Q_OBJECT

  enum { NoOpId, FitAreaId, ZoomId, PanId, GlPanId, DumpId,
	 ModeXLinearId, ModeXLogarithmicId, ModeYLinearId, ModeYLogarithmicId,
	 LegendId, CurvePointsId, CurveLinesId, CurveSplinesId };
public:
  /* Construction/destruction */
  Plot2d_ViewFrame( QWidget*, const QString& = "" );
  virtual ~Plot2d_ViewFrame();

  void Init();
  void SetPreference();

  enum ObjectType { MainTitle, XTitle, YTitle, Y2Title, XAxis, YAxis, Y2Axis };

  QWidget*       getViewWidget();

  /* display */
  virtual void   DisplayAll();
  virtual void   EraseAll();
  void           Repaint();

  void           Display( const Plot2d_Prs* );
  void           Erase( const Plot2d_Prs*, const bool = false );
  Plot2d_Prs*    CreatePrs( const char* = 0 );

  virtual bool   eventFilter( QObject*, QEvent* );

  /* operations */
  void           updateTitles( const bool = true );
  void           setTitle( const QString& );
  QString        getTitle() const;

  QVector< QVector<QwtPlotCurve *> > displayPlot2dCurveList( const QList< QList<Plot2d_Curve*> >& sysCoCurveList,
                                                             bool                                 displayLegend,
                                                             const QList< QList<bool> >&          sides);

  QVector< QVector<QwtPlotCurve *> > displayPlot2dCurveList( const QList<Plot2d_Curve*>&  curveList,
                                                             int  groupsize,
                                                             bool  displayLegend,
                                                             const QList< bool >& sides);

  Plot2d_Curve* createPlot2dCurve( QString & title,
                                   QString & unit,
                                   QList<double> & xList,
                                   QList<double> & yList,
                                   QList<QString> & tooltipList,
                                   Plot2d::LineType lineKind,
                                   int lineWidth,
                                   QColor & lineColor,
                                   QwtSymbol::Style markerKind,
                                   Plot2d_QwtPlotPicker* picker,
                                   bool toDraw,
                                   bool displayLegend=true);

  QColor getPlot2dCurveColor( Plot2d_Curve* plot2dCurve);

  QwtPlotCurve *createSegment( double *X, double *Y, int nbPoint,
                               Qt::PenStyle lineKind,
                               int lineWidth,
                               QColor & lineColor,
                               QwtSymbol::Style markerKind,
                               bool side=false);

  /* curves operations [ obsolete ] */
  void           displayCurve( Plot2d_Curve*, bool = false );
  void           displayCurves( const curveList&, bool = false );
  void           eraseCurve( Plot2d_Curve*, bool = false );
  void           eraseCurves( const curveList&, bool = false );
  int            getCurves( curveList& ) const;
  CurveDict      getCurves() const;
  void           updateCurve( Plot2d_Curve*, bool = false );
  void           processFiltering(bool = false);

  /* objects operations */
  QwtPlotItem*   displayObject( Plot2d_Object*, bool = false );
  void           displayObjects( const objectList&, bool = false );
  void           eraseObject( Plot2d_Object*, bool = false );
  void           eraseObjects( const objectList&, bool = false );
  void           eraseBasicObject( QwtPlotItem*, bool = false );
  void           eraseBasicObjects( const QList<QwtPlotItem*> &, bool = false );
  int            getObjects( objectList& ) const;
  bool           isVisible( Plot2d_Object* ) const;
  void           updateObject( Plot2d_Object*, bool = false );

  void           updateLegend( const Plot2d_Prs* );
  void           updateLegend();
  void           fitAll();
  void           fitArea( const QRect& );
  void           fitData( const int, const double, const double,
			  const double, const double,
			  const double = 0, const double = 0 );

  void           getFitRanges( double&, double&, double&, double&,
			       double&, double&);

  void           getFitRangeByCurves( double&, double&, double&, double&,
				      double&, double& );

  void           getFitRangeByMarkers(double&, double&, double&, double&,
                                      double&, double& );

#ifndef NO_ANALYTICAL_CURVES
  void              addAnalyticalCurve( Plot2d_AnalyticalCurve* );
  void              removeAnalyticalCurve( Plot2d_AnalyticalCurve* );
  void              updateAnalyticalCurve( Plot2d_AnalyticalCurve*, bool = false );
  void              updateAnalyticalCurves();
  void              deselectAnalyticalCurves();

  AnalyticalCurveList getAnalyticalCurves() const;
  Plot2d_AnalyticalCurve* getAnalyticalCurve(QwtPlotItem *);
#endif

  void              deselectObjects();

  /* view parameters */
  void           copyPreferences( Plot2d_ViewFrame* );
  void           setCurveType( int, bool = true );
  int            getCurveType() const;
  void           setCurveTitle( Plot2d_Curve*, const QString& );
  void           setObjectTitle( Plot2d_Object*, const QString& );
  void           showLegend( bool, bool = true );
  void           setLegendPos( int );
  int            getLegendPos() const;
  void           setLegendSymbolType( int );
  int            getLegendSymbolType() const;
  void           setLegendFont( const QFont& );
  QFont          getLegendFont() const;
  void           setLegendFontColor( const QColor& );
  QColor         getLegendFontColor() const;
  void           setSelectedLegendFontColor( const QColor& );
  QColor         getSelectedLegendFontColor() const;
  void           setMarkerSize( const int, bool = true  );
  int            getMarkerSize() const;
  virtual void   setBackgroundColor( const QColor& );
  QColor         backgroundColor() const;
  virtual void   setSelectionColor( const QColor& );
  QColor         selectionColor() const;
  void           setXGrid( bool, const int, bool, const int, bool = true );
  void           setYGrid( bool, const int, bool, const int,
			   bool, const int, bool, const int, bool = true );
  void           setTitle( bool, const QString&, ObjectType, bool = true );
  QString        getTitle( ObjectType ) const;

  void           setFont( const QFont&, ObjectType, bool = true );
  void           setHorScaleMode( const int, bool = true );
  int            getHorScaleMode() const;
  void           setVerScaleMode( const int, bool = true );
  int            getVerScaleMode() const;
  void           setNormLMaxMode( bool, bool = true);
  bool           getNormLMaxMode()const;
  void           setNormLMinMode( bool, bool = true);
  bool           getNormLMinMode()const;
  void           setNormRMaxMode( bool, bool = true);
  bool           getNormRMaxMode()const;
  void           setNormRMinMode( bool, bool = true);
  bool           getNormRMinMode()const;


  bool           isModeHorLinear();
  bool           isModeVerLinear();
  bool           isNormLMaxMode();
  bool           isNormLMinMode();
  bool           isNormRMaxMode();
  bool           isNormRMinMode();

  bool           isLegendShow() const;

  // Protection against QwtCurve::drawLines() bug in Qwt 0.4.x:
  // it crashes if switched to X/Y logarithmic mode, when one or more points have
  // non-positive X/Y coordinate
  bool           isXLogEnabled() const;
  bool           isYLogEnabled() const;
  void           setEnableAxis( QwtPlot::Axis, bool );

  virtual bool   print( const QString&, const QString& ) const;
  void           printPlot( QPainter*, const QRectF& ) const;

  QString        getVisualParameters();
  void           setVisualParameters( const QString& );

  void           incrementalPan ( const int, const int );
  void           incrementalZoom( const int, const int );

  QwtPlotCanvas* getPlotCanvas() const;
  Plot2d_Curve*  getClosestCurve( QPoint, double&, int& ) const;

  Plot2d_Object* getPlotObject( QwtPlotItem* ) const;
  QwtPlotItem*   getPlotObject( Plot2d_Object* ) const;
  QwtPlotCurve*  getPlotCurve( Plot2d_Curve* ) const;
  Plot2d_Plot2d* getPlot() const { return myPlot; }

  void           updatePlotItem(Plot2d_Object*, QwtPlotItem*);
protected:
  int            testOperation( const QMouseEvent& );
  virtual void   readPreferences();
  virtual void   writePreferences();
  QString        getInfo( const QPoint& );
  virtual void   wheelEvent( QWheelEvent* );
  bool           hasPlotCurve( Plot2d_Curve* ) const;
  void           setCurveType( QwtPlotCurve*, int );
  bool           hasPlotObject( Plot2d_Object* ) const;
  QString        getXmlVisualParameters();
  bool           setXmlVisualParameters(const QString&);

public slots:
  void           onViewPan();
  void           onViewZoom();
  void           onViewFitAll();
  void           onViewFitArea();
  void           onViewGlobalPan();
  void           onSettings();
#ifndef NO_ANALYTICAL_CURVES
  void           onAnalyticalCurve();
#endif
  void           onFitData();
  void           onChangeBackground();
  void           onPanLeft();
  void           onPanRight();
  void           onPanUp();
  void           onPanDown();
  void           onZoomIn();
  void           onZoomOut();

protected:
  virtual void   customEvent( QEvent* );
  void           plotMousePressed( const QMouseEvent& );
  bool           plotMouseMoved( const QMouseEvent& );
  void           plotMouseReleased( const QMouseEvent& );

signals:
  void           vpModeHorChanged();
  void           vpModeVerChanged();
  void           vpNormLModeChanged();
  void           vpNormRModeChanged();
  void           vpCurveChanged();
  void           contextMenuRequested( QContextMenuEvent* );
  void           clicked (const QVariant&, int );

protected:
  Plot2d_Plot2d*      myPlot;
  int                 myOperation;
  QPoint              myPnt;

  int                 myCurveType;
  bool                myShowLegend;
  int                 myLegendPos;
  int                 myLegendSymbolType;
  QFont               myLegendFont;
  QColor              myLegendColor;
  QColor              mySelectedLegendFontColor;
  int                 myMarkerSize;
  QColor              myBackground;
  QColor              mySelectionColor;
  QString             myTitle, myXTitle, myYTitle, myY2Title;
  bool                myTitleEnabled, myXTitleEnabled, myYTitleEnabled, myY2TitleEnabled;
  bool                myXGridMajorEnabled, myYGridMajorEnabled, myY2GridMajorEnabled;
  bool                myXGridMinorEnabled, myYGridMinorEnabled, myY2GridMinorEnabled;
  int                 myXGridMaxMajor, myYGridMaxMajor, myY2GridMaxMajor;
  int                 myXGridMaxMinor, myYGridMaxMinor, myY2GridMaxMinor;
  int                 myXMode, myYMode;
  bool                myNormLMin, myNormLMax, myNormRMin, myNormRMax;
  double              myXDistance, myYDistance, myYDistance2;
  bool                mySecondY;
  ObjectDict          myObjects;
#ifndef NO_ANALYTICAL_CURVES
  AnalyticalCurveList myAnalyticalCurves;
#endif
  Plot2d_NormalizeAlgorithm* myLNormAlgo;
  Plot2d_NormalizeAlgorithm* myRNormAlgo;
  bool                myIsDefTitle;
 private:
  // List of QwtPlotCurve curves to draw (created by Plot2d_Curve::createPlotItem() )
  QList<QwtPlotItem*> myQwtPlotCurveList;

  // List of intermittent segments to connect curves
  QList<QwtPlotCurve*> myIntermittentSegmentList;

  // List of curves Plot2d_Curve
  QList<Plot2d_Curve*> myPlot2dCurveList;
};

class Plot2d_Plot2d : public QwtPlot
{
  Q_OBJECT
public:
  Plot2d_Plot2d( QWidget* );
  virtual ~Plot2d_Plot2d();

  void               setLogScale( int, bool );

  void               replot();
  QwtAbstractLegend* getLegend();
  QSize              sizeHint() const;
  QSize              minimumSizeHint() const;
  void               defaultPicker();
  void               setPickerMousePattern( int, int = Qt::NoButton );
  void               setPicker( Plot2d_QwtPlotPicker *picker);
  Plot2d_QwtPlotPicker* getPicker() { return myPicker; }
  Plot2d_AxisScaleDraw* getScaleDraw() { return myScaleDraw; }
  QList<QwtPlotMarker*> getSeparationLineList() { return mySeparationLineList; }
  void clearSeparationLineList();
  void setLegendSymbolType( const int );
  int  getLegendSymbolType();
  QwtPlotMarker *createMarkerAndTooltip( QwtSymbol* symbol,
                                         double    X,
                                         double    Y,
                                         QString & tooltip,
                                         Plot2d_QwtPlotPicker *picker);

  QwtPlotGrid*   grid() const;
  QwtPlotZoomer* zoomer() const;

  virtual void   updateYAxisIdentifiers();

  // Methods to deal with axes ticks

  void createAxisScaleDraw();
  void applyTicks();
  void unactivAxisScaleDraw( int numcall);

  void displayXTicksAndLabels(
         double XLeftmargin, double XRightMargin,
         const QList< QPair< QString, QMap<double,QString> > > & devicesPosLabelTicks);

  void createSeparationLine( double Xpos);

protected:
  QwtPlotGrid*   myGrid;
  QList<QColor>  myColors;
  QwtPlotZoomer* myPlotZoomer;
  Plot2d_AxisScaleDraw* myScaleDraw;
  // The point picker associated with the graphic view
  Plot2d_QwtPlotPicker *myPicker;
private:
  // List of verticals segments between two curves
  QList<QwtPlotMarker*> mySeparationLineList;
  int myLegendSymbolType;
};

class Plot2d_ScaleDraw: public QwtScaleDraw
{
public:
  Plot2d_ScaleDraw( char f = 'g', int prec = 6 );

  virtual QwtText label( double value ) const;

  int precision() const { return myPrecision; }

private:
  char myFormat;
  int  myPrecision;
};

class Plot2d_YScaleDraw: public QwtScaleDraw
{
public:
  Plot2d_YScaleDraw();

  virtual QwtText label( double value ) const;
};

/* Definition of X axis graduations
 */
class Plot2d_AxisScaleDraw: public QwtScaleDraw
{
public:
  static const QString DEVICE_FONT;
  static const int     DEVICE_FONT_SIZE;
  static const int     DEVICE_BY;

  Plot2d_AxisScaleDraw(Plot2d_Plot2d* plot);

  virtual ~Plot2d_AxisScaleDraw();

  void unactivTicksDrawing( int numcall);

  virtual void draw( QPainter * painter, const QPalette & palette) const;

  virtual QwtText label(double value) const;

  void setLabelTick(double value, QString label, bool isDevice = false);

  void setTicks(const QList<double> aTicks);

  void setInterval(double lowerBound, double upperBound);

  void applyTicks();

  double extent( const QFont &font ) const;

protected:

  void drawLabel( QPainter* painter, double value) const;

  void drawTick( QPainter* painter, double value, int len) const;

private:
  bool myActivTicksDrawing;   // true => activate drawing ticks (with draw() )
  int  myNumTicksDrawingCall; // call number to ticks drawing

  // Ticks list to display on X axis
  QMap<double, QString> myLabelX;  // position, label
  //
  QList<double> myTicks;  // positions

  // Systems names to display under X axis
  QMap<double, QString> myLabelDevice;

  Plot2d_Plot2d* myPlot;  // Drawing zone QwtPlot

  double myLowerBound;
  double myUpperBound;
};




/* Management of tooltips associated with markers for curves points or others points
 */
class Plot2d_QwtPlotPicker : public QwtPlotPicker
{
public:
  static const double BOUND_HV_SIZE;

  Plot2d_QwtPlotPicker( int            xAxis,
                        int            yAxis,
                        RubberBand     rubberBand,
                        DisplayMode    trackerMode,
                        QwtPlotCanvas *canvas);

  Plot2d_QwtPlotPicker( int  xAxis,
                        int  yAxis,
                        QwtPlotCanvas *canvas);

  virtual ~Plot2d_QwtPlotPicker();

  QList<QwtPlotMarker*>             pMarkers;         // points markers
  QMap<QwtPlotMarker*, QwtText>  pMarkersToolTip;  // associations (marker,tooltip)

protected:

  virtual QwtText trackerText( const QPoint & pos ) const;

};

#endif
