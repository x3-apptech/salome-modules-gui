#ifndef PLOT2D_VIEWFRAME_H
#define PLOT2D_VIEWFRAME_H

#include "Plot2d_Curve.h"
#include <qwidget.h>
#include <qintdict.h>

class Plot2d_Plot2d;
class Plot2d_Prs;

typedef QIntDict<Plot2d_Curve> CurveDict;

class PLOT2D_EXPORT Plot2d_ViewFrame : public QWidget
{ 
  Q_OBJECT

  enum { NoOpId, FitAreaId, ZoomId, PanId, GlPanId, DumpId, 
   ModeXLinearId, ModeXLogarithmicId, ModeYLinearId, ModeYLogarithmicId,
   LegendId, CurvePointsId, CurveLinesId, CurveSplinesId };
public:
  /* Construction/destruction */
  Plot2d_ViewFrame( QWidget* parent, const QString& title = "" );
  virtual ~Plot2d_ViewFrame();

  enum ObjectType { MainTitle, XTitle, YTitle, Y2Title, XAxis, YAxis, Y2Axis };

public:
  QWidget* getViewWidget();

  /* display */
  void    DisplayAll();
  void    EraseAll();
  void    Repaint();

  void    Display( const Plot2d_Prs* );
  void    Erase( const Plot2d_Prs*, const bool = false );
  Plot2d_Prs* CreatePrs( const char* entry = 0 );

  /* operations */
  void    setTitle( const QString& title );
  QString getTitle() const { return myTitle; }
  void    displayCurve( Plot2d_Curve* curve, bool update = false );
  void    displayCurves( const curveList& curves, bool update = false );
  void    eraseCurve( Plot2d_Curve* curve, bool update = false );
  void    eraseCurves( const curveList& curves, bool update = false );
  int     getCurves( curveList& clist );
  const   CurveDict& getCurves() { return myCurves; }
  int     hasCurve( Plot2d_Curve* curve );
  bool    isVisible( Plot2d_Curve* curve );
  void    updateCurve( Plot2d_Curve* curve, bool update = false );
  void    updateLegend( const Plot2d_Prs* prs );
  void    fitAll();
  void    fitArea( const QRect& area );

  /* view parameters */
  void    copyPreferences( Plot2d_ViewFrame* );
  void    setCurveType( int curveType, bool update = true );
  int     getCurveType() const { return myCurveType; }
  void    setCurveTitle( int curveKey, const QString& title );
  void    showLegend( bool show, bool update = true );
  void    setLegendPos( int pos );
  int     getLegendPos() const { return myLegendPos; }
  void    setMarkerSize( const int size, bool update = true  );
  int     getMarkerSize() const { return myMarkerSize; }
  void    setBackgroundColor( const QColor& color );
  QColor  backgroundColor() const;
  void    setXGrid( bool xMajorEnabled, const int xMajorMax,
                    bool xMinorEnabled, const int xMinorMax, bool update = true );
  void    setYGrid( bool yMajorEnabled, const int yMajorMax,
                    bool yMinorEnabled, const int yMinorMax,
                    bool y2MajorEnabled, const int y2MajorMax,
                    bool y2MinorEnabled, const int y2MinorMax, bool update = true );
  void    setTitle( bool enabled, const QString& title, ObjectType type, bool update = true );
  QString getTitle( ObjectType type ) const;

  void    setFont( const QFont& font, ObjectType type, bool update = true );
  void    setHorScaleMode( const int mode, bool update = true );
  int     getHorScaleMode() const { return myXMode; }
  void    setVerScaleMode( const int mode, bool update = true );
  int     getVerScaleMode() const { return myYMode; }

  bool    isModeHorLinear();
  bool    isModeVerLinear();
  bool    isLegendShow() { return myShowLegend; };

protected:
  int     testOperation( const QMouseEvent& );
  void    readPreferences();
  void    writePreferences();
  QString getInfo( const QPoint& pnt );
  virtual void wheelEvent( QWheelEvent* );

public slots:
  void    onViewPan(); 
  void    onViewZoom();
  void    onViewFitAll();
  void    onViewFitArea();
  void    onViewGlobalPan(); 
  void    onSettings();
  void    onFitData();
  void    onChangeBackground();

protected slots:
  void    plotMousePressed( const QMouseEvent& );
  void    plotMouseMoved( const QMouseEvent& );
  void    plotMouseReleased( const QMouseEvent& );

signals:
  void    vpModeHorChanged();
  void    vpModeVerChanged();
  void    vpCurveChanged();
  void    contextMenuRequested( QContextMenuEvent *e );

protected:
  Plot2d_Plot2d* myPlot;
  int            myOperation;
  QPoint         myPnt;
  CurveDict      myCurves;

  int            myCurveType;
  bool           myShowLegend;
  int            myLegendPos;
  int            myMarkerSize;
  QColor         myBackground;
  QString        myTitle, myXTitle, myYTitle, myY2Title;
  bool           myTitleEnabled, myXTitleEnabled, myYTitleEnabled, myY2TitleEnabled;
  bool           myXGridMajorEnabled, myYGridMajorEnabled, myY2GridMajorEnabled;
  bool           myXGridMinorEnabled, myYGridMinorEnabled, myY2GridMinorEnabled;
  int            myXGridMaxMajor, myYGridMaxMajor, myY2GridMaxMajor;
  int            myXGridMaxMinor, myYGridMaxMinor, myY2GridMaxMinor;
  int            myXMode, myYMode;
  double         myXDistance, myYDistance, myYDistance2;
  bool           mySecondY;
};

class Plot2d_Plot2d : public QwtPlot 
{
public:
  Plot2d_Plot2d( QWidget* parent );

  void       replot();
  void       getNextMarker( QwtSymbol::Style& typeMarker, QColor& color, Qt::PenStyle& typeLine );
  QwtLegend* getLegend() {
#if QWT_VERSION < 0x040200
     return d_legend;
#else  
     return legend(); /* mpv: porting to the Qwt 4.2.0 */
#endif
  }
  virtual QSize       sizeHint() const;
  virtual QSizePolicy sizePolicy() const;
  virtual QSize       minimumSizeHint() const;

protected:
  bool       existMarker( const QwtSymbol::Style typeMarker, const QColor& color, const Qt::PenStyle typeLine );

protected:
  QValueList<QColor> myColors;
};

#endif
