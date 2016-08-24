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

#include "Plot2d_ViewFrame.h"

#include "Plot2d_Prs.h"
#include "Plot2d_Curve.h"
#include "Plot2d_PlotItems.h"
#include "Plot2d_FitDataDlg.h"
#ifndef NO_SUIT
#include "Plot2d_ViewWindow.h"
#endif
#include "Plot2d_SetupViewDlg.h"
#ifndef NO_ANALYTICAL_CURVES
#include "Plot2d_AnalyticalCurveDlg.h"
#include "Plot2d_AnalyticalCurve.h"
#endif
#include "Plot2d_ToolTip.h"

#ifndef NO_SUIT
#include "SUIT_Tools.h"
#include "SUIT_Session.h"
#include "SUIT_MessageBox.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_Application.h"
#else
#include <QMessageBox>
#endif

#include <QApplication>
#include <QToolBar>
#include <QToolButton>
#include <QCursor>
#include <QColorDialog>
#include <QLayout>
#include <QMap>
#include <QPainter>
#include <QPaintDevice>
#include <QEvent>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QPrinter>
#include <QPalette>
#include <QLocale>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <qwt_math.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_scale_div.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_curve_fitter.h>
#include <qwt_plot_renderer.h>
#include <qwt_legend.h>
#include <qwt_scale_widget.h>

#include <stdlib.h>
#include <limits>

#define DEFAULT_LINE_WIDTH     0     // (default) line width
#define DEFAULT_MARKER_SIZE    9     // default marker size
#define MIN_RECT_SIZE          11    // min sensibility area size

#define FITALL_EVENT           ( QEvent::User + 9999 )

const char* imageZoomCursor[] = {
"32 32 3 1",
". c None",
"a c #000000",
"# c #ffffff",
"................................",
"................................",
".#######........................",
"..aaaaaaa.......................",
"................................",
".............#####..............",
"...........##.aaaa##............",
"..........#.aa.....a#...........",
".........#.a.........#..........",
".........#a..........#a.........",
"........#.a...........#.........",
"........#a............#a........",
"........#a............#a........",
"........#a............#a........",
"........#a............#a........",
".........#...........#.a........",
".........#a..........#a.........",
".........##.........#.a.........",
"........#####.....##.a..........",
".......###aaa#####.aa...........",
"......###aa...aaaaa.......#.....",
".....###aa................#a....",
"....###aa.................#a....",
"...###aa...............#######..",
"....#aa.................aa#aaaa.",
".....a....................#a....",
"..........................#a....",
"...........................a....",
"................................",
"................................",
"................................",
"................................"};

const char* imageCrossCursor[] = {
  "32 32 3 1",
  ". c None",
  "a c #000000",
  "# c #ffffff",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "...............#................",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  ".......#################........",
  "........aaaaaaa#aaaaaaaaa.......",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "...............#a...............",
  "................a...............",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................",
  "................................"};

#ifdef WIN32
  #ifdef max
    #undef max
  #endif

  #ifdef min
    #undef min
  #endif
#endif

/*!
  Constructor
*/
Plot2d_ViewFrame::Plot2d_ViewFrame( QWidget* parent, const QString& title )
     : QWidget (parent, 0),
       myOperation( NoOpId ),
       myCurveType( 1 ),
       myShowLegend( true ), myLegendPos( 1 ), myLegendSymbolType( 0 ), myLegendFont("Helvetic",12),
       myLegendColor(Qt::black), mySelectedLegendFontColor( Qt::darkBlue ),
       myMarkerSize( DEFAULT_MARKER_SIZE ),
       myBackground( Qt::white ),
       mySelectionColor( Qt::gray ),
       myTitle( "" ), myXTitle( "" ), myYTitle( "" ), myY2Title( "" ),
       myTitleEnabled( true ), myXTitleEnabled( true ),
       myYTitleEnabled( true ), myY2TitleEnabled (true),
       myXGridMajorEnabled( true ), myYGridMajorEnabled( true ), myY2GridMajorEnabled( true ),
       myXGridMinorEnabled( false ), myYGridMinorEnabled( false ), myY2GridMinorEnabled( false ),
       myXGridMaxMajor( 8 ), myYGridMaxMajor( 8 ), myY2GridMaxMajor( 8 ),
       myXGridMaxMinor( 5 ), myYGridMaxMinor( 5 ), myY2GridMaxMinor( 5 ),
       myXMode( 0 ), myYMode( 0 ),myNormLMin(false), myNormLMax(false), myNormRMin(false), myNormRMax(false),
       mySecondY( false ), myIsDefTitle( true )
{
  setObjectName( title );
  myRNormAlgo = new Plot2d_NormalizeAlgorithm(this);
  myLNormAlgo = new Plot2d_NormalizeAlgorithm(this);
  /* Plot 2d View */
  QVBoxLayout* aLayout = new QVBoxLayout( this );
  aLayout->setMargin(0);
  myPlot = new Plot2d_Plot2d( this );
  new Plot2d_ToolTip( this );

  aLayout->addWidget( myPlot );

#ifndef NO_SUIT
  Init();
#endif
}
/*!
  Destructor
*/
Plot2d_ViewFrame::~Plot2d_ViewFrame()
{
}
/*!
  Performs the initial setup.
*/
void Plot2d_ViewFrame::Init()
{
  /* Initial Setup - get from the preferences */
  readPreferences();

  setCurveType( myCurveType, false );
  setXGrid( myXGridMajorEnabled, myXGridMaxMajor, myXGridMinorEnabled, myXGridMaxMinor, false );
  setYGrid( myYGridMajorEnabled, myYGridMaxMajor, myYGridMinorEnabled, myYGridMaxMinor,
            myY2GridMajorEnabled, myY2GridMaxMajor, myY2GridMinorEnabled, myY2GridMaxMinor, false );

  setTitle( myTitleEnabled,  myTitle,  MainTitle, false );
  setTitle( myXTitleEnabled, myXTitle, XTitle, false );
  setTitle( myYTitleEnabled, myYTitle, YTitle, false );

  if (mySecondY)
    setTitle( myY2TitleEnabled, myY2Title, Y2Title, false );
  setHorScaleMode( myXMode, false );
  setVerScaleMode( myYMode, false );
  setBackgroundColor( myBackground );
  setSelectionColor( mySelectionColor );
  setLegendPos( myLegendPos );
  setLegendSymbolType( myLegendSymbolType );
  setLegendFont( myLegendFont );
  setLegendFontColor( myLegendColor );
  setSelectedLegendFontColor( mySelectedLegendFontColor );
  showLegend( myShowLegend, false );
  myPlot->replot();

  QWidget* aParent = parentWidget();
  if ( aParent ) {
    resize( (int)(0.8 * aParent->width()), (int)(0.8 * aParent->height()) );
  }
  QwtScaleMap xMap = myPlot->canvasMap( QwtPlot::xBottom );
  QwtScaleMap yMap = myPlot->canvasMap( QwtPlot::yLeft );
  myXDistance = xMap.s2() - xMap.s1();
  myYDistance = yMap.s2() - yMap.s1();
  myYDistance2 = 0;
  if (mySecondY) {
    QwtScaleMap yMap2 = myPlot->canvasMap( QwtPlot::yRight );
    myYDistance2 = yMap2.s2() - yMap2.s1();
  }
  myPlot->canvas()->installEventFilter( this );
}
void Plot2d_ViewFrame::SetPreference()
{
  readPreferences();
  setBackgroundColor( myBackground );
  setSelectionColor( mySelectionColor );
  setCurveType( myCurveType, true );
  setMarkerSize( myMarkerSize, true );
  showLegend( myShowLegend, true );
  setLegendPos( myLegendPos );
  setLegendSymbolType( myLegendSymbolType );
  setLegendFont( myLegendFont );
  setLegendFontColor( myLegendColor );
  setSelectedLegendFontColor( mySelectedLegendFontColor );
  myPlot->replot();
}
/*!
  Gets window's central widget
*/
QWidget* Plot2d_ViewFrame::getViewWidget()
{
  return (QWidget*)myPlot;
}
/*!
  Actually this method just re-displays all curves which are presented in the viewer
*/
void Plot2d_ViewFrame::DisplayAll()
{
  objectList olist;
  getObjects( olist );
  foreach ( Plot2d_Object* o, olist )
    updateObject( o, false );
  myPlot->replot();
  if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase();
}
/*!
   Removes all curves from the view
*/
void Plot2d_ViewFrame::EraseAll()
{
  objectList anObjects;
  getObjects( anObjects );
  eraseObjects( anObjects, false );
  myObjects.clear();

  // 1)- Erase all the intermittent segments who connect curves

  int nbSeg = myIntermittentSegmentList.size();
  if (nbSeg > 0)
  {
      for (int iseg=0; iseg < nbSeg; iseg++)
      {
          QwtPlotCurve *segment = myIntermittentSegmentList[iseg];

          segment->detach();  // erase in QwtPlot window
          delete segment;
      }
      myIntermittentSegmentList.clear();
  }

  // 3)- Erase all QwtPlotCurve associated with the Plot2d_Curve

  int nbCur1 = myQwtPlotCurveList.size();
  if (nbCur1 > 0)
  {
      for (int icur=0; icur < nbCur1; icur++)
      {
          QwtPlotItem *curve0 = myQwtPlotCurveList[icur];
          QwtPlotCurve *curve = static_cast<QwtPlotCurve*>(curve0);

          if (curve)
          {
              delete curve;
          }
      }
      myQwtPlotCurveList.clear();
  }

  // 4)- Erase all curves Plot2d_Curve

  int nbCur = myPlot2dCurveList.size();
  if (nbCur > 0)
  {
      for (int icur=0; icur < nbCur; icur++)
      {
          Plot2d_Curve *curve = myPlot2dCurveList[icur];

          if (curve)
          {
              delete curve;
          }
      }
      myPlot2dCurveList.clear();
  }
  myPlot->replot();
  if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase();
}
/*!
  Redraws viewframe contents
*/
void Plot2d_ViewFrame::Repaint()
{
  myPlot->replot();
}
/*!
  Display presentation
*/
void Plot2d_ViewFrame::Display( const Plot2d_Prs* prs )
{
  if ( !prs || prs->IsNull() )
    return;

  setEnableAxis( QwtPlot::yRight, prs->isSecondY() ); // VSR: is it correct? maybe we should only enable second Y axis if required

  // display all objects from presentation
  objectList anObjects = prs->getObjects();
  displayObjects( anObjects );
  setXGrid( myXGridMajorEnabled, myXGridMaxMajor, myXGridMinorEnabled, myXGridMaxMinor, true );
  setYGrid( myYGridMajorEnabled, myYGridMaxMajor, myYGridMinorEnabled, myYGridMaxMinor,
            myY2GridMajorEnabled, myY2GridMaxMajor, myY2GridMinorEnabled, myY2GridMaxMinor, true );
  if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase();
}

/*!
  Erase presentation
*/
void Plot2d_ViewFrame::Erase( const Plot2d_Prs* prs, const bool )
{
  if ( !prs || prs->IsNull() )
    return;

  // erase all objects from presentation
  objectList anObjects = prs->getObjects();
  eraseObjects( anObjects );
  if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase();
}

bool Plot2d_ViewFrame::eventFilter( QObject* watched, QEvent* e )
{
  if ( watched == myPlot->canvas() ) {
    int aType = e->type();
    switch( aType ) {
      case QEvent::MouseMove: {
        QMouseEvent* me = (QMouseEvent*)e;
        if ( me && ( me->buttons() != 0 || me->button() != 0 ) ) {
          QMouseEvent m( QEvent::MouseMove, me->pos(), me->button(),
                         me->buttons(), me->modifiers() );
          if ( plotMouseMoved( m ) )
            return true;
        }
        break;
      }
      case QEvent::MouseButtonPress: {
        QMouseEvent* me = (QMouseEvent*)e;
        if ( me && ( me->buttons() != 0 || me->button() != 0 ) ) {
          QMouseEvent m( QEvent::MouseButtonPress, me->pos(), me->button(),
                          me->buttons(), me->modifiers() );
          plotMousePressed( m );
        }
        break;
      }
      case QEvent::MouseButtonRelease: {
        QMouseEvent* me = (QMouseEvent*)e;
        if ( me && ( me->buttons() != 0 || me->button() != 0 ) ) {
          QMouseEvent m( QEvent::MouseButtonRelease, me->pos(), me->button(),
                         me->buttons(), me->modifiers() );
          plotMouseReleased( m );
        }
        break;
      }
    case QEvent::ContextMenu:
      // Fix from SLN
      // Do nothing because context menu is called from MouseRelease
      return true;
    }
  }
  return QWidget::eventFilter( watched, e );
}

/*!
  Sets title
*/
void Plot2d_ViewFrame::setTitle( const QString& title )
{
  setTitle( myTitleEnabled, title, MainTitle, true );
  myIsDefTitle = false;
}

/*!
  Gets title
*/
QString Plot2d_ViewFrame::getTitle() const
{
  return myTitle;
}

/*!
  Reads Plot2d view settings from the preferences
*/
void Plot2d_ViewFrame::readPreferences()
{
#ifndef NO_SUIT
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  myCurveType = resMgr->integerValue( "Plot2d", "CurveType", myCurveType );
  setCurveType( resMgr->integerValue( "Plot2d", "CurveType", myCurveType ) );

  myShowLegend = resMgr->booleanValue( "Plot2d", "ShowLegend", myShowLegend );
  myLegendPos = resMgr->integerValue( "Plot2d", "LegendPos", myLegendPos );
  myLegendSymbolType = resMgr->integerValue( "Plot2d", "LegendSymbolType", myLegendSymbolType );
  myLegendFont = resMgr->fontValue( "Plot2d", "LegendFont", myLegendFont );
  myLegendColor = resMgr->colorValue( "Plot2d", "LegendFontColor", myLegendColor );
  mySelectedLegendFontColor = resMgr->colorValue( "Plot2d", "SelectedLegendFontColor", mySelectedLegendFontColor );
  myMarkerSize = resMgr->integerValue( "Plot2d", "MarkerSize", myMarkerSize );
  myBackground = resMgr->colorValue( "Plot2d", "Background", myBackground );
  mySelectionColor = resMgr->colorValue( "Plot2d", "SelectionColor", mySelectionColor );

  myTitleEnabled = resMgr->booleanValue( "Plot2d", "ShowTitle", myTitleEnabled );
  myXTitleEnabled = resMgr->booleanValue( "Plot2d", "ShowHorTitle", myXTitleEnabled );
  myYTitleEnabled = resMgr->booleanValue( "Plot2d", "ShowVerLeftTitle", myYTitleEnabled );
  myY2TitleEnabled = resMgr->booleanValue( "Plot2d", "ShowVerRightTitle", myY2TitleEnabled );

  myXGridMajorEnabled = resMgr->booleanValue( "Plot2d", "EnableHorMajorGrid", myXGridMajorEnabled );
  myYGridMajorEnabled = resMgr->booleanValue( "Plot2d", "EnableVerMajorGrid", myYGridMajorEnabled );
  myY2GridMajorEnabled = resMgr->booleanValue( "Plot2d", "EnableRightVerMajorGrid", myY2GridMajorEnabled );

  myXGridMinorEnabled = resMgr->booleanValue( "Plot2d", "EnableHorMinorGrid", myXGridMinorEnabled );
  myYGridMinorEnabled = resMgr->booleanValue( "Plot2d", "EnableVerMinorGrid", myYGridMinorEnabled );
  myY2GridMinorEnabled = resMgr->booleanValue( "Plot2d", "EnableRightVerMinorGrid", myY2GridMinorEnabled );

  myXGridMaxMajor = resMgr->integerValue( "Plot2d", "HorMajorGridMax", myXGridMaxMajor );
  myYGridMaxMajor = resMgr->integerValue( "Plot2d", "VerMajorGridMax", myYGridMaxMajor );
  if ( mySecondY )
    myY2GridMaxMajor = resMgr->integerValue( "Plot2d", "VerMajorRightGridMax", myY2GridMaxMajor );

  myXGridMaxMinor = resMgr->integerValue( "Plot2d", "HorMinorGridMax", myXGridMaxMinor );
  myYGridMaxMinor = resMgr->integerValue( "Plot2d", "VerMinorGridMax", myYGridMaxMinor );
  if ( mySecondY )
    myY2GridMaxMinor = resMgr->integerValue( "Plot2d", "VerMinorGridMax", myY2GridMaxMinor );

  setHorScaleMode( qMax( 0, qMin( 1, resMgr->integerValue( "Plot2d", "HorScaleMode", myXMode ) ) ), false );
  setVerScaleMode( qMax( 0, qMin( 1, resMgr->integerValue( "Plot2d", "VerScaleMode", myYMode ) ) ), false );
  setNormLMinMode( resMgr->booleanValue( "Plot2d", "VerNormLMinMode", myNormLMin ) );
  setNormLMaxMode( resMgr->booleanValue( "Plot2d", "VerNormLMaxMode", myNormLMax ) );
  setNormRMinMode( resMgr->booleanValue( "Plot2d", "VerNormRMinMode", myNormRMin ) );
  setNormRMaxMode( resMgr->booleanValue( "Plot2d", "VerNormRMaxMode", myNormRMax ) );
  QColor c = resMgr->colorValue( "Plot2d", "DeviationMarkerColor", QColor(255,0,0));
  myPlot->setProperty(PLOT2D_DEVIATION_COLOR, c);
  myPlot->setProperty(PLOT2D_DEVIATION_LW,
                      resMgr->integerValue( "Plot2d", "DeviationMarkerLineWidth", 1));
  myPlot->setProperty(PLOT2D_DEVIATION_TS,
                      resMgr->integerValue( "Plot2d", "DeviationMarkerTickSize", 2));
#endif
}

/*!
  Writes Plot2d view settings to the preferences
*/
void Plot2d_ViewFrame::writePreferences()
{
#ifndef NO_SUIT
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  resMgr->setValue( "Plot2d", "CurveType", myCurveType );
  resMgr->setValue( "Plot2d", "ShowLegend", myShowLegend );
  resMgr->setValue( "Plot2d", "LegendPos", myLegendPos );
  resMgr->setValue( "Plot2d", "LegendSymbolType", myLegendSymbolType );
  resMgr->setValue( "Plot2d", "LegendFont", myLegendFont );
  resMgr->setValue( "Plot2d", "LegendFontColor", myLegendColor );
  resMgr->setValue( "Plot2d", "SelectedLegendFontColor", mySelectedLegendFontColor );
  resMgr->setValue( "Plot2d", "MarkerSize", myMarkerSize );
  resMgr->setValue( "Plot2d", "Background", myBackground );
  resMgr->setValue( "Plot2d", "SelectionColor", mySelectionColor );
  resMgr->setValue( "Plot2d", "ShowTitle", myTitleEnabled );
  resMgr->setValue( "Plot2d", "ShowHorTitle", myXTitleEnabled );
  resMgr->setValue( "Plot2d", "ShowVerLeftTitle", myYTitleEnabled );
  resMgr->setValue( "Plot2d", "DeviationMarkerColor", myPlot->property(PLOT2D_DEVIATION_COLOR).value<QColor>() );
  resMgr->setValue( "Plot2d", "DeviationMarkerLineWidth", myPlot->property(PLOT2D_DEVIATION_LW).toInt() );
  resMgr->setValue( "Plot2d", "DeviationMarkerTickSize", myPlot->property(PLOT2D_DEVIATION_TS).toInt() );
  if ( mySecondY )
    resMgr->setValue( "Plot2d", "ShowVerRightTitle", myY2TitleEnabled );

  resMgr->setValue( "Plot2d", "EnableHorMajorGrid", myXGridMajorEnabled );
  resMgr->setValue( "Plot2d", "EnableVerMajorGrid", myYGridMajorEnabled );
  resMgr->setValue( "Plot2d", "EnableHorMinorGrid", myXGridMinorEnabled );
  resMgr->setValue( "Plot2d", "EnableVerMinorGrid", myYGridMinorEnabled );

  resMgr->setValue( "Plot2d", "HorMajorGridMax", myXGridMaxMajor );
  resMgr->setValue( "Plot2d", "VerMajorGridMax", myYGridMaxMajor );

  resMgr->setValue( "Plot2d", "HorMinorGridMax", myXGridMaxMinor );
  resMgr->setValue( "Plot2d", "VerMinorGridMax", myYGridMaxMinor );

  resMgr->setValue( "Plot2d", "HorScaleMode", myXMode );

  if ( mySecondY )
  {
    resMgr->setValue( "Plot2d", "EnableRightVerMajorGrid", myY2GridMajorEnabled );
    resMgr->setValue( "Plot2d", "EnableRightVerMinorGrid", myY2GridMinorEnabled );
    resMgr->setValue( "Plot2d", "VerRightMajorGridMax", myY2GridMaxMajor );
    resMgr->setValue( "Plot2d", "VerRightMinorGridMax", myY2GridMaxMinor );
  }

  resMgr->setValue( "Plot2d", "VerScaleMode", myYMode );
  resMgr->setValue( "Plot2d", "VerNormLMinMode", myNormLMin );
  resMgr->setValue( "Plot2d", "VerNormLMaxMode", myNormLMax );
  resMgr->setValue( "Plot2d", "VerNormRMinMode", myNormRMin );
  resMgr->setValue( "Plot2d", "VerNormRMaxMode", myNormRMax );
#endif
}

/*!
  Prints mouse cursor coordinates into string
*/
QString Plot2d_ViewFrame::getInfo( const QPoint& pnt )
{
  int i;
  QwtValueList aTicks;
  bool xFound = false, yFound = false;
  double xCoord, yCoord;
  const QwtScaleDiv& aXscale = myPlot->axisScaleDiv( QwtPlot::xBottom );
  aTicks = aXscale.ticks( QwtScaleDiv::MajorTick );
  for ( i = 0; i < aTicks.count(); i++ ) {
    double majXmark = aTicks[i];
    int xmark = myPlot->transform( QwtPlot::xBottom, majXmark );
    if ( xmark-2 == pnt.x() ) {
      xCoord = majXmark;
      xFound = true;
      break;
    }
  }
  if ( !xFound ) {
    aTicks = aXscale.ticks( QwtScaleDiv::MinorTick );
    for ( i = 0; i < aTicks.count(); i++ ) {
      double minXmark = aTicks[i];
      int xmark = myPlot->transform( QwtPlot::xBottom, minXmark );
      if ( xmark-2 == pnt.x() ) {
        xCoord = minXmark;
        xFound = true;
        break;
      }
    }
  }
  const QwtScaleDiv& aYscale = myPlot->axisScaleDiv( QwtPlot::yLeft );
  aTicks = aYscale.ticks( QwtScaleDiv::MajorTick );
  for ( i = 0; i < aTicks.count(); i++ ) {
    double majYmark = aTicks[i];
    int ymark = myPlot->transform( QwtPlot::yLeft, majYmark );
    if ( ymark-2 == pnt.y() ) {
      yCoord = majYmark;
      yFound = true;
      break;
    }
  }
  if ( !yFound ) {
    aTicks = aYscale.ticks( QwtScaleDiv::MinorTick );
    for ( i = 0; i < aTicks.count(); i++ ) {
      double minYmark = aTicks[i];
      int ymark = myPlot->transform( QwtPlot::yLeft, minYmark );
      if ( ymark-2 == pnt.y() ) {
        yCoord = minYmark;
        yFound = true;
        break;
      }
    }
  }

  QString strX = QString::number( xFound ? xCoord : myPlot->invTransform( QwtPlot::xBottom, pnt.x() ) ).trimmed();
  if ( strX == "-0" )
    strX = "0";
  QString strY = QString::number( yFound ? yCoord : myPlot->invTransform( QwtPlot::yLeft, pnt.y() ) ).trimmed();
  if ( strY == "-0" )
    strY = "0";
  QString info = "";

  if (mySecondY) {
    bool yFound2 = false;
    double yCoord2;

    const QwtScaleDiv& aYscale2 = myPlot->axisScaleDiv( QwtPlot::yRight );
    aTicks = aYscale2.ticks( QwtScaleDiv::MajorTick );
    for ( i = 0; i < aTicks.count(); i++ ) {
      double majYmark = aTicks[i];
      int ymark = myPlot->transform( QwtPlot::yRight, majYmark );
      if ( ymark-2 == pnt.y() ) {
        yCoord2 = majYmark;
        yFound2 = true;
        break;
      }
    }
    if ( !yFound2 ) {
      aTicks = aYscale2.ticks( QwtScaleDiv::MinorTick );
      for ( i = 0; i < aTicks.count(); i++ ) {
        double minYmark = aTicks[i];
        int ymark = myPlot->transform( QwtPlot::yRight, minYmark );
        if ( ymark-2 == pnt.y() ) {
          yCoord2 = minYmark;
          yFound2 = true;
          break;
        }
      }
    }
    QString strY2 = QString::number( yFound2 ? yCoord2 :
                      myPlot->invTransform( QwtPlot::yRight, pnt.y() ) ).trimmed();
    if ( strY2 == "-0" )
    strY2 = "0";
    info = tr("INF_COORDINATES_SOME_Y").arg( strX ).arg( strY ).arg( strY2 );
  }
  else
    info = tr("INF_COORDINATES").arg( strX ).arg( strY );

  return info;
}

/*!
 * Display curves of the list of lists by systems and components
 * - the first level list contains NbSytems lists of second level
 * - a second level list contains NbComponents curves
 * |         system 1         |         system 2         | ..... |          system N        |
 * | compo1 compo2 ... compoM | compo1 compo2 ... compoM | ..... | compo1 compo2 ... compoM |
 *
 * Draw points markers and create associated tooltips.
 * Draw connection segments (intermittent line) between all the curves of a component.
 * \return the list of underlying plot curve that defines the complex cuve at once. In case of success the vector is at least of size 1. The first one is the curve used by the legend.
 */
QVector< QVector<QwtPlotCurve *> > Plot2d_ViewFrame::displayPlot2dCurveList( const QList< QList<Plot2d_Curve*> >& sysCoCurveList,
                                                                             bool                          displayLegend,
                                                                             const QList< QList<bool> >&   sides)
{
  //std::cout << "Plot2d_ViewFrame::displayPlot2dCurveList() 1" << std::endl;

  // Systems number
  int nbSystem = sysCoCurveList.size();

  // Composants number by system
  int nbComponent = (sysCoCurveList.at(0)).size();

  // Total number of curves

   // 1)- Construction of a list by component and by system

   // |      component 1      |      component 2      | ..... |      component M      |
   // | syst1 syst2 ... systN | syst1 syst2 ... systN | ..... | syst1 syst2 ... systN |

  QList<Plot2d_Curve*> plot2dCurveCoSysList;
  QList<bool>          sidesList;
  for (int icom = 0; icom < nbComponent; icom++)
  {
      for (int isys = 0; isys < nbSystem; isys++)
      {
	  // The system curves list
          const QList<Plot2d_Curve*>& sysCurveList=sysCoCurveList.at(isys);
	  plot2dCurveCoSysList.append(sysCurveList.at(icom));
          //
          const QList<bool>& sysSideList=sides.at(isys);
          sidesList.append(sysSideList.at(icom));
      }
  }
  // 2)- Display list curves by a component's curves group
  //     Draw connection segments (intermittent line) between the curves
  QVector< QVector<QwtPlotCurve *> > ret=displayPlot2dCurveList( plot2dCurveCoSysList, nbSystem, displayLegend, sidesList);
  // 3)- Size of graduations labels and texts under X axis
  QwtScaleWidget *wid = myPlot->axisWidget( QwtPlot::xBottom);
  wid->setTitle("  "); // to make the names readable under X axis.
  QFont xFont = myPlot->axisFont(QwtPlot::xBottom);
  xFont.setPointSize(8);
  myPlot->setAxisFont(QwtPlot::xBottom, xFont);
  return ret;
}


/*!
 * Display list of curves by group of consecutive curves.
 *
 * Draw points markers and create associated tooltips
 * Draw connection segments (intermittent line) between the curves
 * \param [in] sides sorted as in \b curveList. If true->right if false->left
 * \return the list of underlying plot curve that defines the complex cuve at once. In case of success the vector is at least of size 1. The first one is the curve used by the legend.
 */
QVector< QVector<QwtPlotCurve *> > Plot2d_ViewFrame::displayPlot2dCurveList( const QList<Plot2d_Curve*>& curveList,
                                                                             int  groupSize,
                                                                             bool  displayLegend, const QList< bool >& sides)
{
  // Consider the new legend's entries
  // (PB: to update the legend we must remove it and put a new QwtAbstractLegend in the QwtPlot)
  myPlot->insertLegend( (QwtAbstractLegend*)NULL); // we remove here, we shall put at the end

  int nbAllCurves = curveList.size();
  int nbGroups    = nbAllCurves / groupSize;
  QVector< QVector<QwtPlotCurve *> > vectCurve(nbGroups);
  int ig, icur;
  int icur1, icur2;  // curves indices in a group

  // I)- Compute X range and Y range for all the curves' points of all groups
  //     In the graphic view, set the Y range 's bounds for all groups of curves

  // For all groups of curves
  double XallGroupMin=std::numeric_limits<double>::max(), XallGroupMax=-std::numeric_limits<double>::max();
  double YRightallGroupMin=std::numeric_limits<double>::max(), YRightallGroupMax=-std::numeric_limits<double>::max();
  double YLeftallGroupMin=std::numeric_limits<double>::max(), YLeftallGroupMax=-std::numeric_limits<double>::max();
  icur1 = 0;
  for (ig=0; ig < nbGroups; ig++)  //*1*
  {
      icur2 = icur1 + groupSize -1;

      // For all curves in one group
      double XgroupMin, XgroupMax;
      double YgroupMin, YgroupMax;

      // For one curve
      double XcurveMin, XcurveMax;
      double YcurveMin, YcurveMax;
      double *Xval;
      double *Yval;
      int nbPoints;

      // Compute X range and Y range for all the curves' points in the group
      bool side=false;
      for (icur=icur1; icur <= icur2; icur++)  //*2*
      {
          Plot2d_Curve *plot2dCurve = curveList.at(icur);
          side=sides.at(icur);
          // Curve points
          nbPoints = plot2dCurve->getData( &Xval, &Yval);  // dynamic allocation

          for (int ip=0; ip < nbPoints; ip++)
          {
              if (ip == 0)  // first point
              {
                  XcurveMin = Xval[ip];  XcurveMax = Xval[ip];
                  YcurveMin = Yval[ip];  YcurveMax = Yval[ip];
              }
              else
              {
                  if      (Xval[ip] < XcurveMin)  XcurveMin = Xval[ip];
                  else if (Xval[ip] > XcurveMax)  XcurveMax = Xval[ip];
                  if      (Yval[ip] < YcurveMin)  YcurveMin = Yval[ip];
                  else if (Yval[ip] > YcurveMax)  YcurveMax = Yval[ip];
              }
          }
          delete [] Xval;
          delete [] Yval;

          if (icur == icur1)  // first curve
          {
              XgroupMin = XcurveMin;  XgroupMax = XcurveMax;
              YgroupMin = YcurveMin;  YgroupMax = YcurveMax;
          }
          else
          {
              if (XcurveMin < XgroupMin)  XgroupMin = XcurveMin;
              if (XcurveMax > XgroupMax)  XgroupMax = XcurveMax;
              if (YcurveMin < YgroupMin)  YgroupMin = YcurveMin;
              if (YcurveMax > YgroupMax)  YgroupMax = YcurveMax;
          }
      } //*2*

      if (XgroupMin < XallGroupMin)  XallGroupMin = XgroupMin;
      if (XgroupMax > XallGroupMax)  XallGroupMax = XgroupMax;
      if(side)
        {
          if (YgroupMin < YRightallGroupMin)  YRightallGroupMin = YgroupMin;
          if (YgroupMax > YRightallGroupMax)  YRightallGroupMax = YgroupMax;
        }
      else
        {
          if (YgroupMin < YLeftallGroupMin)  YLeftallGroupMin = YgroupMin;
          if (YgroupMax > YLeftallGroupMax)  YLeftallGroupMax = YgroupMax;
        }
      // First curve of the following group
      icur1 = icur2 + 1;
  } //*1*
  // Set the XY range 's bounds for all groups of curves
  if(YRightallGroupMin!=std::numeric_limits<double>::max())
    {
      double deltaY = YRightallGroupMax - YRightallGroupMin;
      YRightallGroupMin-=0.05*deltaY; YRightallGroupMax+= 0.05*deltaY;
      myPlot->setAxisScale( QwtPlot::yRight, YRightallGroupMin,YRightallGroupMax);
    }
  if(YLeftallGroupMin!=std::numeric_limits<double>::max())
    {
      double deltaY = YLeftallGroupMax - YLeftallGroupMin;
      YLeftallGroupMin-=0.05*deltaY; YLeftallGroupMax+= 0.05*deltaY;
      myPlot->setAxisScale( QwtPlot::yLeft, YLeftallGroupMin, YLeftallGroupMax);
    }
  // II)- Drawing curves, points markers and connection segments

  icur1 = 0;
  for (ig=0; ig < nbGroups; ig++)
  {
      // 1)- Graphical attributs of group's curves

      // Graphical attributes of the first group's curve
      //
      Plot2d_Curve *plot2dCurve1 = curveList.at(icur1);
      //
      QColor color1 = plot2dCurve1->getColor();
      Plot2d::LineType linetype1 = plot2dCurve1->getLine();
      int lineWidth1 = plot2dCurve1->getLineWidth();
      QwtSymbol::Style symbolStyle1 = plot2dCurve1->getMarkerStyle();
      // We attribute to the current group's curve, the color, the line's kind
      // and the marker's kind of the first group's curve

      for (icur=icur1+1; icur<icur1+groupSize; icur++)
        {
          Plot2d_Curve *plot2dCurve = curveList.at(icur);
          //
          plot2dCurve->setColor(color1);
          plot2dCurve->setLine(linetype1,lineWidth1);
          plot2dCurve->setMarkerStyle(symbolStyle1);
        }

      // 2)- Display the group's curves

      for (icur=icur1; icur<icur1+groupSize; icur++)
      {
          Plot2d_Curve *plot2dCurve = curveList.at(icur);

          QString title = plot2dCurve->getVerTitle();
          std::string std_title = title.toStdString();
          // Create the graphic curve (QwtPlotCurve) et display it in the drawing zone
          // (Qwtplot)
          displayCurve(plot2dCurve);

          // Get the graphic curve
          QwtPlotCurve* plotCurve = dynamic_cast<QwtPlotCurve *>(getPlotObject(plot2dCurve));
          vectCurve[ig].push_back(plotCurve);
          // Modify the points' markers
          QwtSymbol* symbol = new QwtSymbol(plotCurve->symbol()->style(), plotCurve->symbol()->brush(),
                                            plotCurve->symbol()->pen(), plotCurve->symbol()->size()) ;
          symbol->setStyle(symbolStyle1);
          symbol->setPen(QPen(color1,lineWidth1));
          //symbol.setBrush( QBrush( color1));
          //QSize size = 0.5*(symbol.size());
          //symbol.setSize(size);
          //
          plotCurve->setPen(QPen(color1,lineWidth1));
          plotCurve->setSymbol(symbol);

          if (icur > icur1)
          {
              //std::cout << "  courbe d'indice " << icur << " sans entree dans la legende" << std::endl;

              // The curve must not have legend's entry
              plotCurve->setItemAttribute( QwtPlotItem::Legend, false);
          }
          else
          {
              plotCurve->setItemAttribute( QwtPlotItem::Legend, true);
          }
      }

      // 3)- Intermittent segments to connect all the group's curves

      if (groupSize > 1)
      {
          double *Xval;
          double *Yval;
          int nbPoints;
          double Xseg[2], Yseg[2];
          Plot2d_Curve *plot2dCurve1 = curveList.at(icur1);
          bool side = sides.at(icur1);
          // Last point of the first curve
          nbPoints = plot2dCurve1->getData( &Xval, &Yval);  // dynamic allocation
          Xseg[0] = Xval[ nbPoints -1];
          Yseg[0] = Yval[ nbPoints -1];
          delete [] Xval;
          delete [] Yval;

          for (icur=icur1+1; icur<icur1+groupSize; icur++)
          {
              Plot2d_Curve *plot2dCurve = curveList.at(icur);

              // First curve's point
              nbPoints = plot2dCurve->getData( &Xval, &Yval);
              Xseg[1] = Xval[0];
              Yseg[1] = Yval[0];

              vectCurve[ig].push_back(createSegment(Xseg,Yseg,2,Qt::DotLine,lineWidth1,color1,QwtSymbol::NoSymbol,side));

              // Last curve's point
              Xseg[0] = Xval[ nbPoints -1];
              Yseg[0] = Yval[ nbPoints -1];
              delete [] Xval;
              delete [] Yval;
          }
      }
      // First curve of the following group
      icur1 += groupSize;
  }

  if (displayLegend)
    {
      // Consider the new legend's entries
      if(!curveList.empty())
        showLegend( true, true);  // show, update
    }
  return vectCurve;
}


/*!
 * Create and display an y=f(x) curve of points
 * Parameters :
 *   toDraw : true => Display the created curve
 *                    Draw the points'markers and create associated tooltips
 */
Plot2d_Curve* Plot2d_ViewFrame::createPlot2dCurve( QString & title,
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
                                                   bool displayLegend)
{
  //std::cout << "Plot2d_ViewFrame::createPlot2dCurve()" << std::endl;

  // Mathematical curve
  Plot2d_Curve* plot2dCurve = new Plot2d_Curve();

  // To deallocate in EraseAll()
  myPlot2dCurveList.append( plot2dCurve);

  int nbPoint = xList.size();
  double xVal, yVal;
  QString tooltip;

  for (int ip=0; ip < nbPoint; ip++)
  {
      xVal = xList.at(ip);
      yVal = yList.at(ip);
      tooltip = tooltipList.at(ip);

      plot2dCurve->addPoint( xVal, yVal, tooltip);
  }

  plot2dCurve->setVerTitle( title);
  plot2dCurve->setVerUnits( unit);
  if (lineColor.isValid())
  {
      plot2dCurve->setColor( lineColor);
  }
  plot2dCurve->setLine( lineKind, lineWidth);
  plot2dCurve->setMarkerStyle( markerKind);
  plot2dCurve->setMarkerSize(1);

  // Graphical curve (QwtPlotCurve) in the drawing zone (QwtPlot) myPlot
  if (toDraw)
  {
      if (!displayLegend)
        {
          myPlot->insertLegend( (QwtAbstractLegend*)NULL);
        }
      displayCurve( plot2dCurve);

      // Get the graphical curve
      QwtPlotCurve* plotCurve = dynamic_cast<QwtPlotCurve *>( getPlotObject( plot2dCurve));

      QColor theColor;

      if (lineColor.isValid())
      {
        //std::cout << "  valid color" << std::endl;
          theColor = lineColor;
      }
      else
      {
        //std::cout << "  valid color" << std::endl;
          QPen pen = plotCurve->pen();
          theColor = pen.color();
      }

      // Modify points' markers
      QwtSymbol* symbol = new QwtSymbol( plotCurve->symbol()->style(), plotCurve->symbol()->brush(),
    		                             plotCurve->symbol()->pen(), plotCurve->symbol()->size() ) ;
      symbol->setStyle( markerKind );
      //
      if (markerKind != QwtSymbol::NoSymbol)
      {
        symbol->setPen( QPen( theColor, lineWidth));
        symbol->setBrush( QBrush( theColor));
        QSize size = 2.0*(symbol->size()); //0.5
        symbol->setSize(size);
      }

      plotCurve->setSymbol( symbol);
      plotCurve->setStyle( QwtPlotCurve::Lines);
      plotCurve->setPen( QPen( theColor, lineWidth));

      // The curve must not have legend's entry
      plotCurve->setItemAttribute( QwtPlotItem::Legend, false);
  }
  return plot2dCurve;
}


/*!
 * Get curve's color
 */
QColor Plot2d_ViewFrame::getPlot2dCurveColor( Plot2d_Curve* plot2dCurve)
{

  // Get graphical curve
  QwtPlotCurve* plotCurve = dynamic_cast<QwtPlotCurve *>( getPlotObject( plot2dCurve));

  QPen pen = plotCurve->pen();
  QColor color = pen.color();

  return color;
}


/*!
 * Create and display a segment with nbPoint=2 points
 */
QwtPlotCurve *Plot2d_ViewFrame::createSegment( double *X, double *Y, int nbPoint,
                                               Qt::PenStyle lineKind,
                                               int lineWidth,
                                               QColor & lineColor,
                                               QwtSymbol::Style markerKind, bool side)
{
  QwtPlotCurve* aPCurve = new QwtPlotCurve( QString::null );

  aPCurve->setSamples( X, Y, nbPoint);

  aPCurve->setPen( QPen( lineColor, lineWidth, lineKind));
  QwtSymbol* aSymbol = new QwtSymbol();
  aSymbol->setStyle( markerKind );
  aPCurve->setSymbol( aSymbol );

  // The segment must not have legend's entry
  aPCurve->setItemAttribute( QwtPlotItem::Legend, false);

  aPCurve->attach(myPlot);
  aPCurve->setYAxis(side ? QwtPlot::yRight : QwtPlot::yLeft);
  // To deallocate in EraseAll()
  myIntermittentSegmentList.append(aPCurve);
  return aPCurve;
}

/*!
  Adds curve into view
*/
void Plot2d_ViewFrame::displayCurve( Plot2d_Curve* curve, bool update )
{
  QwtPlotItem* anItem = displayObject( curve, update );
  // To deallocate in EraseAll()
  myQwtPlotCurveList.append( anItem);
}

/*!
  Adds curves into view
*/
void Plot2d_ViewFrame::displayCurves( const curveList& curves, bool update )
{
  objectList objects;
  foreach ( Plot2d_Curve* curve, curves )
    objects << curve;
  displayObjects( objects, update );
}

/*!
  Erases curve
*/
void Plot2d_ViewFrame::eraseCurve( Plot2d_Curve* curve, bool update )
{
  eraseObject( curve, update );
}

/*!
  Erases curves
*/
void Plot2d_ViewFrame::eraseCurves( const curveList& curves, bool update )
{
  objectList objects;
  foreach ( Plot2d_Curve* curve, curves )
    objects << curve;
  eraseObjects( objects, update );
}

/*!
  Updates curves attributes
*/
void Plot2d_ViewFrame::updateCurve( Plot2d_Curve* curve, bool update )
{
  updateObject( curve, update );
}

void Plot2d_ViewFrame::processFiltering(bool update)
{
  CurveDict aCurves = getCurves();
  AlgoPlot2dInputData aLData, aRData;
  CurveDict::iterator it;
  for ( it = aCurves.begin(); it != aCurves.end(); it++ ) {
    Plot2d_Object* objItem = it.value();
    if (objItem->getYAxis() == QwtPlot::yRight)
      aRData.append(objItem);
    else
      aLData.append(objItem);
  }

// Normalization by left Y axis
  if (!myNormLMin && !myNormLMax)
    myLNormAlgo->setNormalizationMode(Plot2d_NormalizeAlgorithm::NormalizeNone);
  if(myNormLMin && myNormLMax)
    myLNormAlgo->setNormalizationMode(Plot2d_NormalizeAlgorithm::NormalizeToMinMax);
  else if(myNormLMin)
    myLNormAlgo->setNormalizationMode(Plot2d_NormalizeAlgorithm::NormalizeToMin);
  else if(myNormLMax)
    myLNormAlgo->setNormalizationMode(Plot2d_NormalizeAlgorithm::NormalizeToMax);

  myLNormAlgo->setInput(aLData);
  myLNormAlgo->execute();

// Normalization by right Y axis
  if (!myNormRMin && !myNormRMax)
    myRNormAlgo->setNormalizationMode(Plot2d_NormalizeAlgorithm::NormalizeNone);
  if(myNormRMin && myNormRMax)
    myRNormAlgo->setNormalizationMode(Plot2d_NormalizeAlgorithm::NormalizeToMinMax);
  else if(myNormRMin)
    myRNormAlgo->setNormalizationMode(Plot2d_NormalizeAlgorithm::NormalizeToMin);
  else if(myNormRMax)
    myRNormAlgo->setNormalizationMode(Plot2d_NormalizeAlgorithm::NormalizeToMax);

  myRNormAlgo->setInput(aRData);
  myRNormAlgo->execute();

  for ( it = aCurves.begin(); it != aCurves.end(); it++) {
    QwtPlotCurve* item = it.key();
    Plot2d_Object* objItem = it.value();
    updatePlotItem(objItem, item);
  }
  if(update)
  myPlot->replot();
}

/*!
  Gets lsit of displayed curves
*/
int Plot2d_ViewFrame::getCurves( curveList& curves ) const
{
  curves.clear();

  CurveDict aCurves = getCurves();
  CurveDict::iterator it;
  for ( it = aCurves.begin(); it != aCurves.end(); it++ )
    curves << it.value();
  return curves.count();
}

CurveDict Plot2d_ViewFrame::getCurves() const
{
  CurveDict curves;
  ObjectDict::const_iterator it = myObjects.begin(), aLast = myObjects.end();
  for ( ; it != aLast; it++ ) {
    QwtPlotItem* anItem = it.key();
    if ( anItem && anItem->rtti() == QwtPlotItem::Rtti_PlotCurve ) {
      QwtPlotCurve* aPCurve = dynamic_cast<QwtPlotCurve*>( anItem );
      Plot2d_Curve* aCurve = dynamic_cast<Plot2d_Curve*>( it.value() );
      if ( aPCurve && aCurve )
        curves.insert( aPCurve, aCurve );
    }
  }
  return curves;
}

/*!
  Adds object into view
*/
QwtPlotItem* Plot2d_ViewFrame::displayObject( Plot2d_Object* object, bool update )
{
  QwtPlotItem* anItem = 0;
  if ( !object )
    return anItem;

  if ( object->getYAxis() == QwtPlot::yRight )
    mySecondY = true;

  // san -- Protection against QwtCurve bug in Qwt 0.4.x:
  // it crashes if switched to X/Y logarithmic mode, when one or more points have
  // non-positive X/Y coordinate
  if ( myXMode && object->getMinX() <= 0. )
    setHorScaleMode( 0, false );
  if ( myYMode && object->getMinY() <= 0. )
    setVerScaleMode( 0, false );

  if ( object->isAutoAssign() )
    object->autoFill( myPlot );

  if ( hasPlotObject( object ) ) {
    processFiltering(update);
    updateObject( object, update );
  }
  else {
    anItem = object->createPlotItem();
    anItem->attach( myPlot );
    myObjects.insert( anItem, object );
    //myPlot->setCurveYAxis(curveKey, curve->getYAxis());

    if ( object->rtti() == QwtPlotItem::Rtti_PlotCurve )
    {
      Plot2d_Curve* aCurve = dynamic_cast<Plot2d_Curve*>( object );
      if ( aCurve )
      {
        if ( aCurve->getMarkerSize() == 0 )
          aCurve->setMarkerSize( myMarkerSize );

        processFiltering(update);
        updatePlotItem( aCurve, anItem );
        setCurveType( getPlotCurve( aCurve ), myCurveType );
      }
    }
  }
  updateTitles( false );
  myPlot->updateYAxisIdentifiers();
  if ( update ) {
    myPlot->replot();
    if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase(false);
  }
  return anItem;
}

/*!
  Adds objects into view
*/
void Plot2d_ViewFrame::displayObjects( const objectList& objects, bool update )
{
  //myPlot->setUpdatesEnabled( false ); // call this function deprecate update of legend
  foreach ( Plot2d_Object* object, objects )
    displayObject( object, false );
  fitAll();
  //myPlot->setUpdatesEnabled( true );
  // update legend
  if ( update )
    myPlot->replot();
}

/*!
  Erases object
*/
void Plot2d_ViewFrame::eraseObject( Plot2d_Object* object, bool update )
{
  if ( !object )
    return;

  if ( hasPlotObject( object ) ) {
    QwtPlotItem* anObject = getPlotObject( object );
    eraseBasicObject(anObject,update);
  }
  if (update) {
    if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase();
  }
}

void Plot2d_ViewFrame::eraseBasicObject( QwtPlotItem *object, bool update )
{
  if(!object)
    return;
  object->hide();
  object->detach();
  myObjects.remove(object);
  updateTitles(update);
  myPlot->updateYAxisIdentifiers();
  if ( update )
    myPlot->replot();
}

/*!
  Erases objects
*/
void Plot2d_ViewFrame::eraseObjects( const objectList& objects, bool update )
{
  foreach ( Plot2d_Object* object, objects )
    eraseObject( object, false );

  //  fitAll();
  if ( update )
    myPlot->replot();
  if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase();
}

void Plot2d_ViewFrame::eraseBasicObjects( const QList<QwtPlotItem*> &objects, bool update)
{
  foreach ( QwtPlotItem* object, objects )
    eraseBasicObject( object, false );
  //  fitAll();
  if ( update )
    myPlot->replot();
  if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase();
}

/*!
  Updates objects attributes
*/
void Plot2d_ViewFrame::updateObject( Plot2d_Object* object, bool update )
{
  if ( !object )
    return;
  if ( hasPlotObject( object ) ) {
    QwtPlotItem* anItem = getPlotObject( object );
    if ( !anItem )
      return;
    updatePlotItem(object, anItem );
    anItem->setVisible( true );
    if ( update ) {
      myPlot->replot();
      if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase(false);
    }
  }
}

/*!
  Gets lsit of displayed curves
*/
int Plot2d_ViewFrame::getObjects( objectList& objects ) const
{
  objects.clear();

  ObjectDict::const_iterator it;
  for ( it = myObjects.begin(); it != myObjects.end(); it++ )
    objects << it.value();
  return objects.count();
}

/*!
  Returns true if the curve is visible
*/
bool Plot2d_ViewFrame::isVisible( Plot2d_Object* object ) const
{
  return object && hasPlotObject( object ) && getPlotObject( object )->isVisible();
}

/*!
  update legend
*/
void Plot2d_ViewFrame::updateLegend( const Plot2d_Prs* prs )
{
  if ( !prs || prs->IsNull() )
    return;

  ObjectDict::iterator it = myObjects.begin();
  Plot2d_Object* anObj;
  for (; it != myObjects.end(); ++it ) {
    anObj = *it;
    if ( hasPlotObject( anObj ) )
      getPlotObject( anObj )->setTitle( !anObj->getName().isEmpty() ?
                            anObj->getName() : anObj->getVerTitle() );
  }
}

/*!
  update legend
*/
void Plot2d_ViewFrame::updateLegend() {
  const QwtPlotItemList& items = myPlot->itemList();
  QwtPlotItemIterator it;
  for ( it = items.begin(); it != items.end(); it++ ) {
    QwtPlotItem* item = *it;
    if ( item )
      item->updateLegend( item, item->legendData() );
  }
}


/*!
  Fits the view to see all data
*/
void Plot2d_ViewFrame::fitAll()
{
  // no need to set auto scaling, it will be disabled by setAxisScale() method call
  // myPlot->setAxisAutoScale( QwtPlot::yLeft );
  // myPlot->setAxisAutoScale( QwtPlot::xBottom );
  // myPlot->replot();

  double xmin, xmax, y1min, y1max, y2min, y2max;
  getFitRangeByCurves(xmin, xmax, y1min, y1max, y2min, y2max);
  getFitRangeByMarkers(xmin, xmax, y1min, y1max, y2min, y2max);

  myPlot->setAxisScale( QwtPlot::xBottom, xmin, xmax );
  myPlot->setAxisScale( QwtPlot::yLeft, y1min, y1max );

  if (mySecondY) {
    myPlot->setAxisAutoScale( QwtPlot::yRight );
    myPlot->replot();
    myPlot->setAxisScale( QwtPlot::yRight, y2min, y2max );
  }
  myPlot->replot();
  if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase(false);
}

/*!
  Fits the view to rectangle area (pixels)
*/
void Plot2d_ViewFrame::fitArea( const QRect& area )
{
  QRect rect = area.normalized();
  if ( rect.width() < MIN_RECT_SIZE ) {
    rect.setWidth( MIN_RECT_SIZE );
    rect.setLeft( rect.left() - MIN_RECT_SIZE/2 );
  }
  if ( rect.height() < MIN_RECT_SIZE ) {
    rect.setHeight( MIN_RECT_SIZE );
    rect.setTop( rect.top() - MIN_RECT_SIZE/2 );
  }
  myPlot->setAxisScale( QwtPlot::yLeft,
            myPlot->invTransform( QwtPlot::yLeft, rect.top() ),
            myPlot->invTransform( QwtPlot::yLeft, rect.bottom() ) );
  if (mySecondY)
    myPlot->setAxisScale( QwtPlot::yRight,
            myPlot->invTransform( QwtPlot::yRight, rect.top() ),
            myPlot->invTransform( QwtPlot::yRight, rect.bottom() ) );
  myPlot->setAxisScale( QwtPlot::xBottom,
            myPlot->invTransform( QwtPlot::xBottom, rect.left() ),
            myPlot->invTransform( QwtPlot::xBottom, rect.right() ) );
  myPlot->replot();
  if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase();
}

/*!
  "Fit Data" command for TUI interface
*/
void Plot2d_ViewFrame::fitData(const int mode,
                               const double xMin, const double xMax,
                               const double yMin, const double yMax,
                               double y2Min, double y2Max)
{
  if ( mode == 0 || mode == 2 ) {
    myPlot->setAxisScale( QwtPlot::yLeft, yMin, yMax );
    if (mySecondY)
      myPlot->setAxisScale( QwtPlot::yRight, y2Min, y2Max );
  }
  if ( mode == 0 || mode == 1 )
    myPlot->setAxisScale( QwtPlot::xBottom, xMin, xMax );
  myPlot->replot();
  if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase();
}

/*!
  Gets current fit ranges for view frame
*/
void Plot2d_ViewFrame::getFitRanges(double& xMin,double& xMax,
                                    double& yMin, double& yMax,
                                    double& y2Min, double& y2Max)
{
  int ixMin = myPlot->canvasMap( QwtPlot::xBottom ).transform( myPlot->canvasMap( QwtPlot::xBottom ).s1() );
  int ixMax = myPlot->canvasMap( QwtPlot::xBottom ).transform( myPlot->canvasMap( QwtPlot::xBottom ).s2() );
  int iyMin = myPlot->canvasMap( QwtPlot::yLeft ).transform( myPlot->canvasMap( QwtPlot::yLeft ).s1() );
  int iyMax = myPlot->canvasMap( QwtPlot::yLeft ).transform( myPlot->canvasMap( QwtPlot::yLeft ).s2() );
  xMin = myPlot->invTransform(QwtPlot::xBottom, ixMin);
  xMax = myPlot->invTransform(QwtPlot::xBottom, ixMax);
  yMin = myPlot->invTransform(QwtPlot::yLeft, iyMin);
  yMax = myPlot->invTransform(QwtPlot::yLeft, iyMax);
  y2Min = 0;
  y2Max = 0;
  if (mySecondY) {
    int iyMin = myPlot->canvasMap( QwtPlot::yRight ).transform( myPlot->canvasMap( QwtPlot::yRight ).s1() );
    int iyMax = myPlot->canvasMap( QwtPlot::yRight ).transform( myPlot->canvasMap( QwtPlot::yRight ).s2() );
    y2Min = myPlot->invTransform(QwtPlot::yRight, iyMin);
    y2Max = myPlot->invTransform(QwtPlot::yRight, iyMax);
  }
}

/*!
  Gets current fit ranges by Curves
*/
void Plot2d_ViewFrame::getFitRangeByCurves(double& xMin,  double& xMax,
                                           double& yMin,  double& yMax,
                                           double& y2Min, double& y2Max)
{
  bool emptyV1 = true, emptyV2 = true;
  if ( !myObjects.isEmpty() ) {
    ObjectDict::const_iterator it = myObjects.begin();
    for ( ; it != myObjects.end(); it++ ) {
      bool isV2 = it.value()->getYAxis() == QwtPlot::yRight;
      if ( !it.value()->isEmpty() ) {
	if ( emptyV1 && emptyV2 ) {
	  xMin = 1e150;
	  xMax = -1e150;
	}
	if ( emptyV1 ) {
	  yMin = 1e150;
	  yMax = -1e150;
	}
	if ( emptyV2 ) {
	  y2Min = 1e150;
	  y2Max = -1e150;
	}
	isV2 ? emptyV2 = false : emptyV1 = false;
	xMin = qMin( xMin, it.value()->getMinX() );
	xMax = qMax( xMax, it.value()->getMaxX() );
        if ( isV2 ) {
          y2Min = qMin( y2Min, it.value()->getMinY() );
          y2Max = qMax( y2Max, it.value()->getMaxY() );
        }
        else {
          yMin = qMin( yMin, it.value()->getMinY() );
          yMax = qMax( yMax, it.value()->getMaxY() );
        }
      }
    }
    if ( xMin == xMax ) {
      xMin = xMin == 0. ? -1. : xMin - xMin/10.;
      xMax = xMax == 0. ?  1. : xMax + xMax/10.;
    }
    if ( yMin == yMax ) {
      yMin = yMin == 0. ? -1. : yMin - yMin/10.;
      yMax = yMax == 0. ?  1  : yMax + yMax/10.;
    }
    if ( y2Min == y2Max ) {
      y2Min = y2Min == 0. ? -1. : y2Min - y2Min/10.;
      y2Max = y2Max == 0. ?  1  : y2Max + y2Max/10.;
    }
  }
  // default values
  if ( emptyV1 && emptyV2 ) {
    xMin = isModeHorLinear() ? 0.    : 1.;
    xMax = isModeHorLinear() ? 1000. : 1e5;
  }
  if ( emptyV1  ) {
    yMin = isModeVerLinear() ? 0.    : 1.;
    yMax = isModeVerLinear() ? 1000. : 1e5;
  }
  if ( emptyV2  ) {
    y2Min = isModeVerLinear() ? 0.    : 1.;
    y2Max = isModeVerLinear() ? 1000. : 1e5;
  }
}

/*!
  Gets current fit ranges by Markers
  All parameters are inout.
*/
void Plot2d_ViewFrame::getFitRangeByMarkers(double& xMin,  double& xMax,
                                            double& yMin,  double& yMax,
                                            double& y2Min, double& y2Max)
{
  Plot2d_QwtPlotPicker *picker=myPlot->getPicker();
  if(!picker)
    return;
  if(picker->pMarkers.empty())
    return;
  foreach(QwtPlotMarker *mrker,picker->pMarkers)
    {
      bool isV2 = mrker->yAxis() == QwtPlot::yRight;
      xMin = qMin( xMin, mrker->xValue() );
      xMax = qMax( xMax, mrker->xValue() );
      if ( isV2 ) {
        y2Min = qMin( y2Min, mrker->yValue() );
        y2Max = qMax( y2Max, mrker->yValue() );
      }
      else {
        yMin = qMin( yMin, mrker->yValue() );
        yMax = qMax( yMax, mrker->yValue() );
      }
      if ( xMin == xMax ) {
        xMin = xMin == 0. ? -1. : xMin - xMin/10.;
        xMax = xMax == 0. ?  1. : xMax + xMax/10.;
      }
      if ( yMin == yMax ) {
        yMin = yMin == 0. ? -1. : yMin - yMin/10.;
        yMax = yMax == 0. ?  1  : yMax + yMax/10.;
      }
      if ( y2Min == y2Max ) {
        y2Min = y2Min == 0. ? -1. : y2Min - y2Min/10.;
        y2Max = y2Max == 0. ?  1  : y2Max + y2Max/10.;
      }
    }
}

/*!
  Tests if it is necessary to start operation on mouse action
*/
int Plot2d_ViewFrame::testOperation( const QMouseEvent& me )
{
  int btn = (int)me.button() | (int)me.modifiers();
  const int zoomBtn = (int)Qt::ControlModifier | (int)Qt::LeftButton;
  const int panBtn  = (int)Qt::ControlModifier | (int)Qt::MidButton;
  const int fitBtn  = (int)Qt::ControlModifier | (int)Qt::RightButton;

  int op = NoOpId;
  if ( btn == zoomBtn ) {
    QPixmap zoomPixmap (imageZoomCursor);
    QCursor zoomCursor (zoomPixmap);
    myPlot->canvas()->setCursor( zoomCursor );
    op = ZoomId;
  }
  else if ( btn == panBtn ) {
    myPlot->canvas()->setCursor( QCursor( Qt::SizeAllCursor ) );
    op = PanId;
  }
  else if ( btn == fitBtn ) {
    myPlot->canvas()->setCursor( QCursor( Qt::PointingHandCursor ) );
    op = FitAreaId;
  }
  return op;
}

/*!
  "Settings" toolbar action slot
*/
void Plot2d_ViewFrame::onSettings()
{
#ifdef TEST_AUTOASSIGN
  typedef QMap<int,int> IList;
  typedef QMap<QString,int> SList;
  IList mars, lins;
  SList cols;
  cols[ "red-min" ]   = 1000;
  cols[ "red-max" ]   = -1;
  cols[ "green-min" ] = 1000;
  cols[ "green-max" ] = -1;
  cols[ "blue-min" ]  = 1000;
  cols[ "blue-max" ]  = -1;
  for ( unsigned i = 0; i < 10000; i++ ) {
    QwtSymbol::Style typeMarker;
    QColor           color;
    Qt::PenStyle     typeLine;
    myPlot->getNextMarker( typeMarker, color, typeLine );
    if ( mars.contains(typeMarker) )
      mars[ typeMarker ] = mars[ typeMarker ]+1;
    else
      mars[ typeMarker ] = 0;
    if ( lins.contains(typeLine) )
      lins[ typeLine ] = lins[ typeLine ]+1;
    else
      lins[ typeLine ] = 0;
    if ( cols[ "red-max" ] < color.red() )
      cols[ "red-max" ] = color.red();
    if ( cols[ "red-min" ] > color.red() )
      cols[ "red-min" ] = color.red();
    if ( cols[ "green-max" ] < color.green() )
      cols[ "green-max" ] = color.green();
    if ( cols[ "green-min" ] > color.green() )
      cols[ "green-min" ] = color.green();
    if ( cols[ "blue-max" ] < color.blue() )
      cols[ "blue-max" ] = color.blue();
    if ( cols[ "blue-min" ] > color.blue() )
      cols[ "blue-min" ] = color.blue();
  }
#endif

  Plot2d_SetupViewDlg* dlg = new Plot2d_SetupViewDlg( this, true, mySecondY );
  dlg->setMainTitle( myTitleEnabled, myTitle );
  dlg->setXTitle( myXTitleEnabled, myXTitle );
  dlg->setYTitle( myYTitleEnabled, myYTitle );
  if (mySecondY)
    dlg->setY2Title( myY2TitleEnabled, myY2Title );
  dlg->setCurveType( myCurveType );
  dlg->setLegend( myShowLegend, myLegendPos, myLegendSymbolType, myLegendFont,
                  myLegendColor, mySelectedLegendFontColor );
  dlg->setMarkerSize( myMarkerSize );
  dlg->setBackgroundColor( myBackground );
  dlg->setSelectionColor( mySelectionColor );
  dlg->setScaleMode(myXMode, myYMode);
  dlg->setLMinNormMode(myNormLMin);
  dlg->setLMaxNormMode(myNormLMax);
  dlg->setRMinNormMode(myNormRMin);
  dlg->setRMaxNormMode(myNormRMax);

  QVariant v = myPlot->property(PLOT2D_DEVIATION_LW);
  int lw = v.isValid() ? v.toInt() : 1;

  v = myPlot->property(PLOT2D_DEVIATION_TS);
  int ts = v.isValid() ? v.toInt() : 2;

  v = myPlot->property(PLOT2D_DEVIATION_COLOR);
  QColor cl =  v.isValid() ? v.value<QColor>() : QColor(255,0,0);

  dlg->setDeviationMarkerLw(lw);
  dlg->setDeviationMarkerTs(ts);
  dlg->setDeviationMarkerCl(cl);

  //
  dlg->setMajorGrid( myXGridMajorEnabled, myPlot->axisMaxMajor( QwtPlot::xBottom ),
         myYGridMajorEnabled, myPlot->axisMaxMajor( QwtPlot::yLeft ),
         myY2GridMajorEnabled, myPlot->axisMaxMajor( QwtPlot::yRight ) );
  dlg->setMinorGrid( myXGridMinorEnabled, myPlot->axisMaxMinor( QwtPlot::xBottom ),
         myYGridMinorEnabled, myPlot->axisMaxMinor( QwtPlot::yLeft ),
         myY2GridMinorEnabled, myPlot->axisMaxMinor( QwtPlot::yRight ) );
  if ( dlg->exec() == QDialog::Accepted ) {
    // horizontal axis title
    setTitle( dlg->isXTitleEnabled(), dlg->getXTitle(), XTitle, false );
    // vertical left axis title
    setTitle( dlg->isYTitleEnabled(), dlg->getYTitle(), YTitle, false );
    if (mySecondY) // vertical right axis title
      setTitle( dlg->isY2TitleEnabled(), dlg->getY2Title(), Y2Title, false );

    // main title
    if( dlg->isMainTitleEnabled() && myTitle != dlg->getMainTitle() )
      myIsDefTitle = false;
    setTitle( dlg->isMainTitleEnabled(), dlg->getMainTitle(), MainTitle, true );
    // curve type
    if ( myCurveType != dlg->getCurveType() ) {
      setCurveType( dlg->getCurveType(), false );
    }
    // legend
    if ( myShowLegend != dlg->isLegendEnabled() ) {
      showLegend( dlg->isLegendEnabled(), false );
    }
    if ( myLegendPos != dlg->getLegendPos() ) {
      setLegendPos( dlg->getLegendPos() );
    }
    if ( myLegendSymbolType != dlg->getLegendSymbolType() ) {
      setLegendSymbolType( dlg->getLegendSymbolType() );
    }
	if ( myLegendFont != dlg->getLegendFont() ) {
      setLegendFont( dlg->getLegendFont() );
    }
	if ( myLegendColor != dlg->getLegendColor() ) {
      setLegendFontColor( dlg->getLegendColor() );
    }
    if ( mySelectedLegendFontColor != dlg->getSelectedLegendColor() ) {
      setSelectedLegendFontColor( dlg->getSelectedLegendColor() );
    }

    // marker size
    if ( myMarkerSize != dlg->getMarkerSize() ) {
      setMarkerSize( dlg->getMarkerSize(), false );
    }
    // background color
    if ( myBackground != dlg->getBackgroundColor() ) {
      setBackgroundColor( dlg->getBackgroundColor() );
    }
    // selection color
    if ( mySelectionColor != dlg->getSelectionColor() ) {
      setSelectionColor( dlg->getSelectionColor() );
    }
    // grid
    bool aXGridMajorEnabled, aXGridMinorEnabled, aYGridMajorEnabled, aYGridMinorEnabled,
         aY2GridMajorEnabled, aY2GridMinorEnabled;
    int  aXGridMaxMajor, aXGridMaxMinor, aYGridMaxMajor, aYGridMaxMinor,
         aY2GridMaxMajor, aY2GridMaxMinor;
    dlg->getMajorGrid( aXGridMajorEnabled, aXGridMaxMajor, aYGridMajorEnabled, aYGridMaxMajor,
                       aY2GridMajorEnabled, aY2GridMaxMajor);
    dlg->getMinorGrid( aXGridMinorEnabled, aXGridMaxMinor, aYGridMinorEnabled, aYGridMaxMinor,
                       aY2GridMinorEnabled, aY2GridMaxMinor);
    setXGrid( aXGridMajorEnabled, aXGridMaxMajor, aXGridMinorEnabled, aXGridMaxMinor, false );
    setYGrid( aYGridMajorEnabled, aYGridMaxMajor, aYGridMinorEnabled, aYGridMaxMinor,
              aY2GridMajorEnabled, aY2GridMaxMajor, aY2GridMinorEnabled, aY2GridMaxMinor, false );
    if ( myXMode != dlg->getXScaleMode() ) {
      setHorScaleMode( dlg->getXScaleMode() );
    }
    if ( myYMode != dlg->getYScaleMode() ) {
      setVerScaleMode( dlg->getYScaleMode() );
    }
    if ( myNormLMin != dlg->getLMinNormMode() ) {
      setNormLMinMode( dlg->getLMinNormMode() );
    }
    if ( myNormLMax != dlg->getLMaxNormMode() ) {
      setNormLMaxMode( dlg->getLMaxNormMode() );
    }
    if ( myNormRMin != dlg->getRMinNormMode() ) {
      setNormRMinMode( dlg->getRMinNormMode() );
    }
    if ( myNormRMax != dlg->getRMaxNormMode() ) {
      setNormRMaxMode( dlg->getRMaxNormMode() );
    }

    myPlot->setProperty(PLOT2D_DEVIATION_COLOR,
                        dlg->getDeviationMarkerCl());
    myPlot->setProperty(PLOT2D_DEVIATION_LW,
                        dlg->getDeviationMarkerLw());
    myPlot->setProperty(PLOT2D_DEVIATION_TS,
                         dlg->getDeviationMarkerTs());


    // update view
    myPlot->replot();
    // update preferences
    if ( dlg->isSetAsDefault() )
      writePreferences();
  }
  delete dlg;
}

#ifndef NO_ANALYTICAL_CURVES
/*!
  "Analytical Curves" toolbar action slot
*/
void Plot2d_ViewFrame::onAnalyticalCurve()
{
  Plot2d_AnalyticalCurveDlg dlg( this, this );
  dlg.exec();
  updateAnalyticalCurves();
}

void Plot2d_ViewFrame::addAnalyticalCurve( Plot2d_AnalyticalCurve* theCurve)
{
  myAnalyticalCurves.append(theCurve);
}

void Plot2d_ViewFrame::removeAnalyticalCurve( Plot2d_AnalyticalCurve* theCurve)
{
  theCurve->setAction(Plot2d_AnalyticalCurve::ActRemoveFromView);
}

/*
  Update Analytical curve
*/
void Plot2d_ViewFrame::updateAnalyticalCurve(Plot2d_AnalyticalCurve* c, bool updateView)
{
  if(!c) return;
  QwtScaleDiv div = myPlot->axisScaleDiv(QwtPlot::xBottom);
  c->setRangeBegin(div.lowerBound());
  c->setRangeEnd(div.upperBound());
  c->calculate();
  c->setMarkerSize(myMarkerSize);
  QwtPlotItem* item = c->plotItem();

  switch( c->getAction() ) {
  case Plot2d_AnalyticalCurve::ActAddInView:
    if( c->isActive() ) {
      c->updatePlotItem();
      item->attach( myPlot );
      item->itemChanged();
      item->show();
    }
    c->setAction(Plot2d_AnalyticalCurve::ActNothing);
    break;

  case Plot2d_AnalyticalCurve::ActUpdateInView:
    if(c->isActive()) {
      c->updatePlotItem();
      item->show();
    } else {
      item->hide();
      item->detach();
    }

    c->setAction(Plot2d_AnalyticalCurve::ActNothing);
    break;
  case Plot2d_AnalyticalCurve::ActRemoveFromView:
    item->hide();
    item->detach();
    break;
  }

  if(updateView)
    myPlot->replot();
}

/*
  Update Analytical curves
*/
void Plot2d_ViewFrame::updateAnalyticalCurves()
{
  AnalyticalCurveList::iterator it = myAnalyticalCurves.begin();
  AnalyticalCurveList toDelete;
  for( ; it != myAnalyticalCurves.end(); it++) {
    Plot2d_AnalyticalCurve* c = (*it);
    updateAnalyticalCurve(c);
    if(c && c->getAction() == Plot2d_AnalyticalCurve::ActRemoveFromView)
      toDelete.push_back(c);
  }
  it = toDelete.begin();
  for( ; it != toDelete.end(); it++) {
    Plot2d_AnalyticalCurve* c = (*it);
    myAnalyticalCurves.removeAll(c);
    delete c;
    c = NULL;
  }
  myPlot->replot();
}

/*!
  Return list of the alalytical curves.
*/
AnalyticalCurveList Plot2d_ViewFrame::getAnalyticalCurves() const
{
  return myAnalyticalCurves  ;
}

/*!
  Get analytical curve by plot item.
*/
Plot2d_AnalyticalCurve* Plot2d_ViewFrame::getAnalyticalCurve(QwtPlotItem * theItem) {
  AnalyticalCurveList::iterator it = myAnalyticalCurves.begin();
  for( ; it != myAnalyticalCurves.end(); it++) {
    if((*it)->plotItem() == theItem);
		return (*it);
  }
  return 0;
}
#endif

/*!
  "Fit Data" command slot
*/
void Plot2d_ViewFrame::onFitData()
{
  Plot2d_FitDataDlg* dlg = new Plot2d_FitDataDlg( this, mySecondY );
  double xMin,xMax,yMin,yMax,y2Min,y2Max;
  getFitRanges(xMin,xMax,yMin,yMax,y2Min,y2Max);

  dlg->setRange( xMin, xMax, yMin, yMax, y2Min, y2Max );
  if ( dlg->exec() == QDialog::Accepted ) {
    int mode = dlg->getRange( xMin, xMax, yMin, yMax, y2Min, y2Max );
    fitData(mode,xMin,xMax,yMin,yMax,y2Min,y2Max);
  }
  delete dlg;
#ifndef NO_ANALYTICAL_CURVES
  updateAnalyticalCurves();
#endif
}

/*!
  Change background color
*/
void Plot2d_ViewFrame::onChangeBackground()
{
  QColor selColor = QColorDialog::getColor ( backgroundColor(), this );
  if ( selColor.isValid() ) {
    setBackgroundColor( selColor );
  }
}

/*!
  Sets curve type
*/
void Plot2d_ViewFrame::setCurveType( int curveType, bool update )
{
  myCurveType = curveType;
  CurveDict aCurves = getCurves();
  CurveDict::iterator it = aCurves.begin();
  for ( ; it != aCurves.end(); it++ ) {
    QwtPlotCurve* crv = it.key();
    if ( crv )
      setCurveType( crv, myCurveType );
  }
  for(int i=0 ; i < myAnalyticalCurves.count(); i++) {
    QwtPlotCurve* aPCurve = dynamic_cast<QwtPlotCurve*>( myAnalyticalCurves[i]->plotItem() );
    Plot2d_AnalyticalCurve* aCurve = dynamic_cast<Plot2d_AnalyticalCurve*>( myAnalyticalCurves[i] );
    if ( aPCurve && aCurve ) {
      setCurveType( aPCurve, myCurveType );
    }
  }
  if ( update )
    myPlot->replot();
  emit vpCurveChanged();
}

/*!
  Gets curve type
*/
int Plot2d_ViewFrame::getCurveType() const
{
  return myCurveType;
}

/*!
  Sets curve title
  \param curveKey - curve id
  \param title - new title
*/
void Plot2d_ViewFrame::setCurveTitle( Plot2d_Curve* curve, const QString& title )
{
  setObjectTitle( curve, title );
}

/*!
  Sets object title
  \param object - object id
  \param title - new title
*/
void Plot2d_ViewFrame::setObjectTitle( Plot2d_Object* object, const QString& title )
{
  if ( object && hasPlotObject( object ) )
    getPlotObject( object )->setTitle( title );
}

/*!
  Shows/hides legend
*/
void Plot2d_ViewFrame::showLegend( bool show, bool update )
{
  myShowLegend = show;
  if ( myShowLegend ) {
    QwtAbstractLegend* absLegend = myPlot->legend();
    QwtLegend* legend = 0;
    if ( !absLegend ) {
      legend = new Plot2d_QwtLegend( myPlot );
      legend->setDefaultItemMode(QwtLegendData::Clickable);
      connect( legend, SIGNAL( clicked (const QVariant&, int) ),
               this, SIGNAL( clicked (const QVariant&, int) ) );
      legend->setFrameStyle( QFrame::Box | QFrame::Sunken );
    }
    else {
      legend = dynamic_cast<QwtLegend*>(absLegend);
    }
    if(legend) {
      myPlot->insertLegend( legend );
      setLegendPos( myLegendPos );
      setLegendFont( myLegendFont );
      setLegendFontColor( myLegendColor );
    }
  }
  else
    myPlot->insertLegend( 0 );

  if( show && update )
    updateLegend();

  if ( update )
    myPlot->replot();
}

/*!
  Sets legend position : 0 - left, 1 - right, 2 - top, 3 - bottom
*/
void Plot2d_ViewFrame::setLegendPos( int pos )
{
  myLegendPos = pos;
  QwtAbstractLegend* legend = myPlot->legend();
  if ( legend ) {
    switch( pos ) {
    case 0:
      myPlot->insertLegend( legend, QwtPlot::LeftLegend );
      break;
    case 1:
      myPlot->insertLegend( legend, QwtPlot::RightLegend );
      break;
    case 2:
      myPlot->insertLegend( legend, QwtPlot::TopLegend );
      break;
    case 3:
      myPlot->insertLegend( legend, QwtPlot::BottomLegend );
      break;
    }
  }
}

/*!
  Gets legend position : 0 - left, 1 - right, 2 - top, 3 - bottom
*/
int Plot2d_ViewFrame::getLegendPos() const
{
  return myLegendPos;
}

/*!
  Sets legend symbol type : 0 - marker on line, 1 - marker above line
*/
void Plot2d_ViewFrame::setLegendSymbolType( int type )
{
  myLegendSymbolType = type;
  myPlot->setLegendSymbolType( type );
  updateLegend();
}

/*!
  Gets legend symbol type : 0 - marker on line, 1 - marker above line
*/
int Plot2d_ViewFrame::getLegendSymbolType() const
{
  return myLegendSymbolType;
}

/*!
  Sets legend font
*/
void Plot2d_ViewFrame::setLegendFont( const QFont& fnt )
{
  myLegendFont = fnt;
  QwtAbstractLegend* legend = myPlot->legend();
  if ( legend ) {
    legend->setFont(fnt);
  }
}

/*!
  Gets legend font
*/
QFont Plot2d_ViewFrame::getLegendFont() const
{
  return myLegendFont;
}

/*!
  Sets legend font color
*/
void Plot2d_ViewFrame::setLegendFontColor( const QColor& col )
{
  myLegendColor = col;
  QwtAbstractLegend* legend = myPlot->legend();
  if ( legend ) {
    QPalette pal = legend->palette();
    pal.setColor( QPalette::Text, col );
    legend->setPalette( pal );
  }
}

/*!
  Gets legend font color
*/
QColor Plot2d_ViewFrame::getLegendFontColor() const
{
  return myLegendColor;
}

/*!
  Sets selected legend font color
*/
void Plot2d_ViewFrame::setSelectedLegendFontColor( const QColor& col )
{
  mySelectedLegendFontColor = col;
  Plot2d_Object::setHighlightedLegendTextColor( col );
}

/*!
  Gets selected legend font color
*/
QColor Plot2d_ViewFrame::getSelectedLegendFontColor() const
{
  return mySelectedLegendFontColor;
}

/*!
  Sets new marker size
*/
void Plot2d_ViewFrame::setMarkerSize( const int size, bool update )
{
  if ( myMarkerSize != size )
  {
    myMarkerSize = size;
    CurveDict aCurves = getCurves();
    CurveDict::iterator it = aCurves.begin();
    for ( ; it != aCurves.end(); it++ ) {
      QwtPlotCurve* crv = it.key();
      if ( crv )
      {
        QwtSymbol* aSymbol = new QwtSymbol( crv->symbol()->style(), crv->symbol()->brush(),
        		                            crv->symbol()->pen(), QSize( myMarkerSize, myMarkerSize ) );
        crv->setSymbol( aSymbol );
        if( it.value() )
	      it.value()->setMarkerSize( myMarkerSize );
      }
    }
    if ( update )
      myPlot->replot();
  }
}

/*!
  Gets new marker size
*/
int Plot2d_ViewFrame::getMarkerSize() const
{
  return myMarkerSize;
}

/*!
  Sets background color
*/
void Plot2d_ViewFrame::setBackgroundColor( const QColor& color )
{
  myBackground = color;
  myPlot->canvas()->setPalette( myBackground );
  myPlot->setPalette( myBackground );
  if ( myPlot->getLegend() ) {
    QPalette aPal = myPlot->getLegend()->palette();
    for ( int i = 0; i < QPalette::NColorGroups; i++ ) {
      aPal.setColor( QPalette::Base, myBackground );
      aPal.setColor( QPalette::Background, myBackground );
    }
    myPlot->getLegend()->setPalette( aPal );
    updateLegend();
  }
  Repaint();
}

/*!
  Gets background color
*/
QColor Plot2d_ViewFrame::backgroundColor() const
{
  return myBackground;
}

/*!
  Sets selection color
*/
void Plot2d_ViewFrame::setSelectionColor( const QColor& color )
{
  mySelectionColor = color;
  Plot2d_Object::setSelectionColor( color );
}

/*!
  Gets selection color
*/
QColor Plot2d_ViewFrame::selectionColor() const
{
  return mySelectionColor;
}

/*!
  Sets hor.axis grid parameters
*/
void Plot2d_ViewFrame::setXGrid( bool xMajorEnabled, const int xMajorMax,
         bool xMinorEnabled, const int xMinorMax,
         bool update )
{
  myXGridMajorEnabled = xMajorEnabled;
  myXGridMinorEnabled = xMinorEnabled;
  myXGridMaxMajor = xMajorMax;
  myXGridMaxMinor = xMinorMax;

  myPlot->setAxisMaxMajor( QwtPlot::xBottom, myXGridMaxMajor );
  myPlot->setAxisMaxMinor( QwtPlot::xBottom, myXGridMaxMinor );

  QwtPlotGrid* grid = myPlot->grid();
  if ( !myPlot->axisScaleDiv( QwtPlot::xBottom ).isEmpty() )
    grid->setXDiv( myPlot->axisScaleDiv( QwtPlot::xBottom ) );
  grid->enableX( myXGridMajorEnabled );
  grid->enableXMin( myXGridMinorEnabled );

  if ( update )
    myPlot->replot();
}
/*!
  Sets ver.axis grid parameters
*/
void Plot2d_ViewFrame::setYGrid( bool yMajorEnabled, const int yMajorMax,
                                 bool yMinorEnabled, const int yMinorMax,
                                 bool y2MajorEnabled, const int y2MajorMax,
                                 bool y2MinorEnabled, const int y2MinorMax,
                                 bool update )
{
  myYGridMajorEnabled = yMajorEnabled;
  myYGridMinorEnabled = yMinorEnabled;
  myYGridMaxMajor = yMajorMax;
  myYGridMaxMinor = yMinorMax;

  if (mySecondY) {
    myY2GridMajorEnabled = y2MajorEnabled;
    myY2GridMinorEnabled = y2MinorEnabled;
    myY2GridMaxMajor = y2MajorMax;
    myY2GridMaxMinor = y2MinorMax;
  }
  myPlot->setAxisMaxMajor( QwtPlot::yLeft, myYGridMaxMajor );
  myPlot->setAxisMaxMinor( QwtPlot::yLeft, myYGridMaxMinor );

  if (mySecondY) {
    myPlot->setAxisMaxMajor( QwtPlot::yRight, myY2GridMaxMajor );
    myPlot->setAxisMaxMinor( QwtPlot::yRight, myY2GridMaxMinor );
  }

  QwtPlotGrid* grid = myPlot->grid();
  if ( !myPlot->axisScaleDiv( QwtPlot::yLeft ).isEmpty() )
    grid->setYDiv( myPlot->axisScaleDiv( QwtPlot::yLeft ) );

  if (mySecondY) {
    if (myYGridMajorEnabled) {
      grid->enableY( myYGridMajorEnabled );
      grid->enableYMin( myYGridMinorEnabled );
    }
    else if (myY2GridMajorEnabled) {
      if ( !myPlot->axisScaleDiv( QwtPlot::yRight ).isEmpty() )
        grid->setYDiv( myPlot->axisScaleDiv( QwtPlot::yRight ) );
      grid->enableY( myY2GridMajorEnabled );
      grid->enableYMin( myY2GridMinorEnabled );
    }
    else {
      grid->enableY( false );
      grid->enableYMin( false );
    }
  }
  else {
    grid->enableY( myYGridMajorEnabled );
    grid->enableYMin( myYGridMinorEnabled );
  }
  if ( update )
    myPlot->replot();
}

/*!
  Sets title for some axis
*/
void Plot2d_ViewFrame::setTitle( bool enabled, const QString& title,
                                 ObjectType type, bool update )
{
  switch (type) {
    case MainTitle:
      myTitleEnabled = enabled;
      myTitle = title;
      myPlot->setTitle( myTitleEnabled ? myTitle : QString() );
      break;
    case XTitle:
      myXTitleEnabled = enabled;
      myXTitle = title;
      myPlot->setAxisTitle( QwtPlot::xBottom, myXTitleEnabled ? myXTitle : QString() );
      break;
    case YTitle:
      myYTitleEnabled = enabled;
      myYTitle = title;
      myPlot->setAxisTitle( QwtPlot::yLeft, myYTitleEnabled ? myYTitle : QString() );
      break;
    case Y2Title:
      myY2TitleEnabled = enabled;
      myY2Title = title;
      myPlot->setAxisTitle( QwtPlot::yRight, myY2TitleEnabled ? myY2Title : QString() );
      break;
    default:
      break;
  }
  if ( update )
    myPlot->replot();
}
/*!
  Sets title for some axis
*/
QString Plot2d_ViewFrame::getTitle( ObjectType type ) const
{
  QString title = "";
  switch (type) {
    case MainTitle:
      title = myTitle;   break;
    case XTitle:
      title = myXTitle;  break;
    case YTitle:
      title = myYTitle;  break;
    case Y2Title:
      title = myY2Title; break;
    default:
      break;
  }
  return title;
}
/*!
  Sets font for Plot2d object : title or axis
*/
void Plot2d_ViewFrame::setFont( const QFont& font, ObjectType type, bool update)
{
  switch (type) {
    case MainTitle:
      myPlot->title().setFont(font);
      break;
    case XTitle:
       myPlot->axisTitle(QwtPlot::xBottom).setFont(font); break;
    case YTitle:
      myPlot->axisTitle(QwtPlot::yLeft).setFont(font);    break;
    case Y2Title:
      myPlot->axisTitle(QwtPlot::yRight).setFont(font);   break;
    case XAxis:
      myPlot->setAxisFont(QwtPlot::xBottom, font);        break;
    case YAxis:
      myPlot->setAxisFont(QwtPlot::yLeft, font);          break;
    case Y2Axis:
      myPlot->setAxisFont(QwtPlot::yRight, font);         break;
  }
  if ( update )
    myPlot->replot();
}

/*!
  Sets scale mode for horizontal axis: 0 - linear, 1 - logarithmic
*/
void Plot2d_ViewFrame::setHorScaleMode( const int mode, bool update )
{
  if ( myXMode == mode )
    return;

  // san -- Protection against QwtCurve bug in Qwt 0.4.x:
  // it crashes if switched to X/Y logarithmic mode, when one or more points have
  // non-positive X/Y coordinate
  if ( mode && !isXLogEnabled() ){
#ifndef NO_SUIT
    SUIT_MessageBox::warning(this, tr("WRN_WARNING"), tr("WRN_XLOG_NOT_ALLOWED"));
#else
    QMessageBox::warning(this, tr("WRN_WARNING"), tr("WRN_XLOG_NOT_ALLOWED"));
#endif
    return;
  }

  myXMode = mode;

  // set bounds of logarithmic scale
  if( myXMode != 0 ) {
    myPlot->setAxisScale( QwtPlot::xBottom, 1.0, 1e5 );
    myPlot->updateAxes();
  }

  myPlot->setLogScale(QwtPlot::xBottom, myXMode != 0);

  if ( update )
    fitAll();
  emit vpModeHorChanged();
}

/*!
  Gets scale mode for horizontal axis: 0 - linear, 1 - logarithmic
*/
int Plot2d_ViewFrame::getHorScaleMode() const
{
  return myXMode;
}

/*!
  Sets scale mode for vertical axis: 0 - linear, 1 - logarithmic
*/
void Plot2d_ViewFrame::setVerScaleMode( const int mode, bool update )
{
  if ( myYMode == mode )
    return;

  // san -- Protection against QwtCurve bug in Qwt 0.4.x:
  // it crashes if switched to X/Y logarithmic mode, when one or more points have
  // non-positive X/Y coordinate
  if ( mode && !isYLogEnabled() ){
#ifndef NO_SUIT
    SUIT_MessageBox::warning(this, tr("WRN_WARNING"), tr("WRN_YLOG_NOT_ALLOWED"));
#else
    QMessageBox::warning(this, tr("WRN_WARNING"), tr("WRN_YLOG_NOT_ALLOWED"));
#endif
    return;
  }

  myYMode = mode;

  // set bounds of logarithmic scale
  if( myYMode != 0 ){
    myPlot->setAxisScale( QwtPlot::yLeft, 1.0, 1e5 );
    myPlot->updateAxes();
  }

  myPlot->setLogScale(QwtPlot::yLeft, myYMode != 0);
  if (mySecondY) {
    if( myYMode != 0 ){
      myPlot->setAxisScale( QwtPlot::yRight, 1.0, 1e5 );
      myPlot->updateAxes();
    }
    myPlot->setLogScale( QwtPlot::yRight, myYMode != 0 );
  }

  if ( update )
    fitAll();
  emit vpModeVerChanged();
}

/*!
  Gets scale mode for vertical axis: 0 - linear, 1 - logarithmic
*/
int Plot2d_ViewFrame::getVerScaleMode() const
{
  return myYMode;
}

/*!
  Sets normalization mode to the global maximum by left Y axis
*/
void Plot2d_ViewFrame::setNormLMaxMode( bool mode, bool update )
{
  if ( myNormLMax == mode )
    return;

  myNormLMax = mode;
  processFiltering(true);
  if ( update )
    fitAll();
  emit vpNormLModeChanged();
}

/*!
  Gets normalization mode to the global maximum by left Y axis
*/
bool Plot2d_ViewFrame::getNormLMaxMode() const
{
  return myNormLMax;
}

/*!
  Sets normalization mode to the global minimum by left Y axis
*/
void Plot2d_ViewFrame::setNormLMinMode( bool mode, bool update )
{
  if ( myNormLMin == mode )
    return;

  myNormLMin = mode;
  processFiltering(true);
  if ( update )
    fitAll();
  emit vpNormLModeChanged();
}

/*!
  Gets normalization mode to the global minimum by left Y axis
*/
bool Plot2d_ViewFrame::getNormLMinMode() const
{
  return myNormLMax;
}

/*!
  Sets normalization mode to the global maximum by right Y axis
*/
void Plot2d_ViewFrame::setNormRMaxMode( bool mode, bool update )
{
  if ( myNormRMax == mode )
    return;

  myNormRMax = mode;
  processFiltering(true);
  if ( update )
    fitAll();
  emit vpNormRModeChanged();
}

/*!
  Gets normalization mode to the global maximum by right Y axis
*/
bool Plot2d_ViewFrame::getNormRMaxMode() const
{
  return myNormRMax;
}

/*!
  Sets normalization mode to the global minimum by right Y axis
*/
void Plot2d_ViewFrame::setNormRMinMode( bool mode, bool update )
{
  if ( myNormRMin == mode )
    return;

  myNormRMin = mode;
  processFiltering(true);
  if ( update )
    fitAll();
  emit vpNormRModeChanged();
}

/*!
  Gets normalization mode to the global minimum by right Y axis
*/
bool Plot2d_ViewFrame::getNormRMinMode() const
{
  return myNormRMax;
}

/*!
  Return, scale mode for horizontal axis
*/
bool Plot2d_ViewFrame::isModeHorLinear()
{
  return (myXMode == 0 ? true : false);
}

/*!
  Return, scale mode for vertical axis
*/
bool Plot2d_ViewFrame::isModeVerLinear()
{
  return (myYMode == 0 ? true : false);
}

/*!
  Return \c True if curves are normalize to the global maximum by left Y axis
*/
bool Plot2d_ViewFrame::isNormLMaxMode()
{
  return (myNormLMax ? true : false);
}

/*!
  Return \c True if curves are normalize to the global minimum by left Y axis
*/
bool Plot2d_ViewFrame::isNormLMinMode()
{
  return (myNormLMin ? true : false);
}

/*!
  Return \c True if curves are normalize to the global maximum by right Y axis
*/
bool Plot2d_ViewFrame::isNormRMaxMode()
{
  return (myNormRMax ? true : false);
}

/*!
  Return \c True if curves are normalize to the global minimum by right Y axis
*/
bool Plot2d_ViewFrame::isNormRMinMode()
{
  return (myNormRMin ? true : false);
}

/*!
  Return \c True if legend is shown
*/
bool Plot2d_ViewFrame::isLegendShow() const
{
  return myShowLegend;
}

/*!
  Slot, called when user presses mouse button
*/
void Plot2d_ViewFrame::plotMousePressed( const QMouseEvent& me )
{
#ifndef NO_SUIT
  Plot2d_ViewWindow* aParent = dynamic_cast<Plot2d_ViewWindow*>(parent());
  if (aParent)
     aParent->putInfo(getInfo(me.pos()));
#endif
  if ( myOperation == NoOpId )
    myOperation = testOperation( me );
  if ( myOperation != NoOpId ) {
    myPnt = me.pos();
    if ( myOperation == GlPanId ) {
      myPlot->setAxisScale( QwtPlot::yLeft,
          myPlot->invTransform( QwtPlot::yLeft, myPnt.y() ) - myYDistance/2,
          myPlot->invTransform( QwtPlot::yLeft, myPnt.y() ) + myYDistance/2 );
      myPlot->setAxisScale( QwtPlot::xBottom,
          myPlot->invTransform( QwtPlot::xBottom, myPnt.x() ) - myXDistance/2,
          myPlot->invTransform( QwtPlot::xBottom, myPnt.x() ) + myXDistance/2 );
      if (mySecondY)
        myPlot->setAxisScale( QwtPlot::yRight,
          myPlot->invTransform( QwtPlot::yRight, myPnt.y() ) - myYDistance2/2,
          myPlot->invTransform( QwtPlot::yRight, myPnt.y() ) + myYDistance2/2 );
      myPlot->replot();
    }
  }
  else {
    int btn = (int)me.button() | (int)me.modifiers();
    if (btn == Qt::RightButton) {
      QMouseEvent* aEvent = new QMouseEvent(QEvent::MouseButtonPress,
                                            me.pos(), me.button(), me.buttons(), me.modifiers() );
      // QMouseEvent 'me' has the 'MouseButtonDblClick' type. In this case we create new event 'aEvent'.
      parent()->eventFilter(this, aEvent);
    }
  }
  setFocus();
}
/*!
  Slot, called when user moves mouse
*/
bool Plot2d_ViewFrame::plotMouseMoved( const QMouseEvent& me )
{
  int    dx = me.pos().x() - myPnt.x();
  int    dy = me.pos().y() - myPnt.y();

  bool aRes = false;
  if ( myOperation != NoOpId) {
    if ( myOperation == ZoomId ) {
      this->incrementalZoom( dx, dy );
      myPnt = me.pos();
      aRes = true;
    }
    else if ( myOperation == PanId ) {
      this->incrementalPan( dx, dy );
      myPnt = me.pos();
      aRes = true;
    }
  }
  else {
#ifndef NO_SUIT
     Plot2d_ViewWindow* aParent = dynamic_cast<Plot2d_ViewWindow*>(parent());
     if (aParent)
       aParent->putInfo(getInfo(me.pos()));
#endif
  }
  return aRes;
}
/*!
  Slot, called when user releases mouse
*/
void Plot2d_ViewFrame::plotMouseReleased( const QMouseEvent& me )
{
  if ( myOperation == NoOpId && me.button() == Qt::RightButton && me.modifiers() != Qt::ControlModifier )
  {
    QContextMenuEvent aEvent( QContextMenuEvent::Mouse,
                              me.pos(), me.globalPos() );
    emit contextMenuRequested( &aEvent );
  }
  else {
#ifndef NO_ANALYTICAL_CURVES
    updateAnalyticalCurves();
#endif
  }
  myPlot->canvas()->setCursor( QCursor( Qt::CrossCursor ) );
  myPlot->defaultPicker();

#ifndef NO_SUIT
  Plot2d_ViewWindow* aParent = dynamic_cast<Plot2d_ViewWindow*>(parent());
   if (aParent)
     aParent->putInfo(tr("INF_READY"));
#endif
  myOperation = NoOpId;
}
/*!
  Slot, called when user wheeling mouse
*/
void Plot2d_ViewFrame::wheelEvent(QWheelEvent* event)
{
  QwtPlotLayout* pl = myPlot->plotLayout();

  // compute zooming factor
  double aDelta = event->delta();
  double aScale = (aDelta < 0) ? 100./(-aDelta) : aDelta/100.;

  bool scaleXBottom = pl->scaleRect(QwtPlot::xBottom).contains( event->pos() ) ||
                      pl->canvasRect().contains( event->pos() );
  bool scaleYLeft   = pl->scaleRect(QwtPlot::yLeft).contains( event->pos() ) ||
                      pl->canvasRect().contains( event->pos() );
  bool scaleYRight  = mySecondY && ( pl->scaleRect(QwtPlot::yRight).contains( event->pos() ) ||
				     pl->canvasRect().contains( event->pos() ) );

  // scale x bottom axis
  if ( scaleXBottom ) {
    QwtScaleMap xMap = myPlot->canvasMap( QwtPlot::xBottom );
    if ( xMap.s2() - xMap.s1() > 1.0e-12 || aScale > 1 )
      myPlot->setAxisScale( QwtPlot::xBottom, xMap.s1(), xMap.s1() + aScale*(xMap.s2() - xMap.s1()) );
  }

  // scale y left axis
  if ( scaleYLeft ) {
    QwtScaleMap yMap = myPlot->canvasMap( QwtPlot::yLeft );
    if ( yMap.s2() - yMap.s1() > 1.0e-12 || aScale > 1 )
      myPlot->setAxisScale( QwtPlot::yLeft, yMap.s1(), yMap.s1() + aScale*(yMap.s2() - yMap.s1()) );
  }

  // scale y right axis (note: mySecondY value is checked above)
  if ( scaleYRight ) {
    QwtScaleMap yMap = myPlot->canvasMap( QwtPlot::yRight );
    if ( yMap.s2() - yMap.s1() > 10e-12 || aScale > 1 )
      myPlot->setAxisScale( QwtPlot::yRight, yMap.s1(), yMap.s1() + aScale*(yMap.s2() - yMap.s1()) );
  }

  // redraw
  myPlot->replot();
  // update zoomer
  if ( myPlot->zoomer() ) myPlot->zoomer()->setZoomBase();
  // store current mouse position
  myPnt = event->pos();
#ifndef NO_ANALYTICAL_CURVES
  // update analytical curves
  updateAnalyticalCurves();
#endif
}

/*!
  Returns qwt plot curve if it is existed in map of curves and 0 otherwise
*/
QwtPlotCurve* Plot2d_ViewFrame::getPlotCurve( Plot2d_Curve* curve ) const
{
  return dynamic_cast<QwtPlotCurve*>( getPlotObject( curve ) );
}
/*!
  Returns true if qwt plot curve is existed in map of curves and false otherwise
*/
bool Plot2d_ViewFrame::hasPlotCurve( Plot2d_Curve* curve ) const
{
  return hasPlotObject( curve );
}

/*!
  Returns qwt plot curve if it is existed in map of curves and 0 otherwise
*/
QwtPlotItem* Plot2d_ViewFrame::getPlotObject( Plot2d_Object* object ) const
{
  ObjectDict::const_iterator it = myObjects.begin();
  for ( ; it != myObjects.end(); it++ ) {
    if ( it.value() == object )
      return it.key();
  }
  return 0;
}
/*!
  Returns true if qwt plot curve is existed in map of curves and false otherwise
*/
bool Plot2d_ViewFrame::hasPlotObject( Plot2d_Object* object ) const
{
  ObjectDict::const_iterator it = myObjects.begin();
  for ( ; it != myObjects.end(); it++ ) {
    if ( it.value() == object )
      return true;
  }
  return false;
}

/*!
  Sets curve type
*/
void Plot2d_ViewFrame::setCurveType( QwtPlotCurve* curve, int curveType )
{
  if ( !curve )
    return;
  if ( myCurveType == 0 )
    curve->setStyle( QwtPlotCurve::Dots );//QwtCurve::NoCurve
  else if ( myCurveType == 1 ) {
    curve->setStyle( QwtPlotCurve::Lines );
    curve->setCurveAttribute( QwtPlotCurve::Fitted, false );
  }
  else if ( myCurveType == 2 ) {
    curve->setStyle( QwtPlotCurve::Lines );
    QwtSplineCurveFitter* fitter = new QwtSplineCurveFitter();
    fitter->setSplineSize( 250 );
    curve->setCurveAttribute( QwtPlotCurve::Fitted, true );
    curve->setCurveFitter( fitter );
  }
}

/*!
  View operations : Pan view
*/
void Plot2d_ViewFrame::onViewPan()
{
  // reset the previous operation
  myPlot->defaultPicker();

  QCursor panCursor (Qt::SizeAllCursor);
  myPlot->canvas()->setCursor( panCursor );
  myOperation = PanId;
}
/*!
  View operations : Zoom view
*/
void Plot2d_ViewFrame::onViewZoom()
{
  // reset the previous operation
  myPlot->defaultPicker();

  QPixmap zoomPixmap (imageZoomCursor);
  QCursor zoomCursor (zoomPixmap);
  myPlot->canvas()->setCursor( zoomCursor );
  myOperation = ZoomId;
}
/*!
  View operations : Fit All
*/
void Plot2d_ViewFrame::onViewFitAll()
{
  fitAll();

  // reset the previous operation
  myPlot->defaultPicker();
  myPlot->canvas()->setCursor( QCursor(Qt::CrossCursor) );
  myOperation = NoOpId;
}
/*!
  View operations : Fit Area
*/
void Plot2d_ViewFrame::onViewFitArea()
{
  myPlot->canvas()->setCursor( QCursor( Qt::PointingHandCursor ) );
  myOperation = FitAreaId;
  myPlot->setPickerMousePattern( Qt::LeftButton );
}
/*!
  View operations : Global panning
*/
void Plot2d_ViewFrame::onViewGlobalPan()
{
  // reset the previous operation
  myPlot->defaultPicker();

  QPixmap globalPanPixmap (imageCrossCursor);
  QCursor glPanCursor (globalPanPixmap);
  myPlot->canvas()->setCursor( glPanCursor );
  myPlot->setLogScale(QwtPlot::xBottom, false);
  myPlot->setLogScale(QwtPlot::yLeft, false);
  if (mySecondY)
    myPlot->setLogScale(QwtPlot::yRight, false);
  myPlot->replot();
  QwtScaleMap xMap = myPlot->canvasMap( QwtPlot::xBottom );
  QwtScaleMap yMap = myPlot->canvasMap( QwtPlot::yLeft );

  myXDistance = xMap.s2() - xMap.s1();
  myYDistance = yMap.s2() - yMap.s1();

  if (mySecondY) {
    QwtScaleMap yMap2 = myPlot->canvasMap( QwtPlot::yRight );
    myYDistance2 = yMap2.s2() - yMap2.s1();
  }
  // don't make fit all
  // fitAll();
  myOperation = GlPanId;
}

/*!
  Precaution for logarithmic X scale
*/
bool Plot2d_ViewFrame::isXLogEnabled() const
{
  bool allPositive = true;
  ObjectDict::const_iterator it = myObjects.begin();
  for ( ; allPositive && it != myObjects.end(); it++ )
    allPositive = ( it.value()->getMinX() > 0. );
  return allPositive;
}

/*!
  Precaution for logarithmic Y scale
*/
bool Plot2d_ViewFrame::isYLogEnabled() const
{
  bool allPositive = true;
  ObjectDict::const_iterator it = myObjects.begin();
  for ( ; allPositive && it != myObjects.end(); it++ )
    allPositive = ( it.value()->getMinY() > 0. );
  return allPositive;
}

/**
 *
 */
void Plot2d_ViewFrame::setEnableAxis( QwtPlot::Axis theAxis, bool isEnable )
{
  if ( myPlot->axisEnabled( theAxis ) == isEnable )
    return;
  myPlot->enableAxis( theAxis, isEnable );
  if ( theAxis == QwtPlot::yRight )
    mySecondY = isEnable;
}

class Plot2d_QwtPlotZoomer : public QwtPlotZoomer
{
public:
  Plot2d_QwtPlotZoomer( int xAxis, int yAxis, QWidget* canvas )
  : QwtPlotZoomer( xAxis, yAxis, canvas )
  {
    qApp->installEventFilter( this );
    // now picker working after only a button pick.
    // after click on button FitArea in toolbar of the ViewFrame.
  };
  ~Plot2d_QwtPlotZoomer() {};
};

/*!
  Constructor
*/
Plot2d_Plot2d::Plot2d_Plot2d( QWidget* parent )
  : QwtPlot( parent ),
    myPicker( 0 )
{
  // Create alternative scales
  setAxisScaleDraw( QwtPlot::yLeft,   new Plot2d_ScaleDraw() );
  setAxisScaleDraw( QwtPlot::xBottom, new Plot2d_ScaleDraw() );
  setAxisScaleDraw( QwtPlot::yRight,  new Plot2d_ScaleDraw() );

  myPlotZoomer = new Plot2d_QwtPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, canvas() );
  myPlotZoomer->setTrackerMode( QwtPicker::AlwaysOff );
  myPlotZoomer->setRubberBand( QwtPicker::RectRubberBand );
  myPlotZoomer->setRubberBandPen( QColor( Qt::green ) );

  defaultPicker();

  // auto scaling by default
  setAxisAutoScale( QwtPlot::yLeft );
  setAxisAutoScale( QwtPlot::yRight );
  setAxisAutoScale( QwtPlot::xBottom );

  myScaleDraw = NULL;
// grid
  myGrid = new QwtPlotGrid();
  QPen aMajPen = myGrid->majorPen();
  aMajPen.setStyle( Qt::DashLine );
  myGrid->setPen( aMajPen );

  myGrid->enableX( false );
  myGrid->enableXMin( false );
  myGrid->enableY( false );
  myGrid->enableYMin( false );

  myGrid->attach( this );

  setMouseTracking( false );
  canvas()->setMouseTracking( true );

  myPlotZoomer->setEnabled( true );
  myPlotZoomer->setZoomBase();

  setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
}

Plot2d_Plot2d::~Plot2d_Plot2d()
{
}

/*!
  \set axis scale engine - linear or log10
*/
void Plot2d_Plot2d::setLogScale( int axisId, bool log10 )
{
  if ( log10 )
    setAxisScaleEngine( axisId, new QwtLogScaleEngine() );
  else
    setAxisScaleEngine( axisId, new QwtLinearScaleEngine() );
}

/*!
  Recalculates and redraws Plot 2d view
*/
void Plot2d_Plot2d::replot()
{
  // the following code is intended to enable only axes
  // that are really used by displayed objects
  bool enableXBottom = false, enableXTop   = false;
  bool enableYLeft   = false, enableYRight = false;
  const QwtPlotItemList& items = itemList();
  QwtPlotItemIterator it;
  for ( it = items.begin(); it != items.end(); it++ ) {
    QwtPlotItem* item = *it;
    if ( item ) {
      enableXBottom |= item->xAxis() == QwtPlot::xBottom;
      enableXTop    |= item->xAxis() == QwtPlot::xTop;
      enableYLeft   |= item->yAxis() == QwtPlot::yLeft;
      enableYRight  |= item->yAxis() == QwtPlot::yRight;
    }
  }
  enableAxis( QwtPlot::xBottom, enableXBottom );
  enableAxis( QwtPlot::xTop,    enableXTop );
  enableAxis( QwtPlot::yLeft,   enableYLeft );
  enableAxis( QwtPlot::yRight,  enableYRight );

  updateLayout();  // to fix bug(?) of Qwt - view is not updated when title is changed
  QwtPlot::replot();
}

/*!
  Get legend
*/
QwtAbstractLegend* Plot2d_Plot2d::getLegend()
{
#if QWT_VERSION < 0x040200
  return d_legend;
#else
  return legend(); /* mpv: porting to the Qwt 4.2.0 */
#endif
}

/*!
  \return the recommended size for the widget
*/
QSize Plot2d_Plot2d::sizeHint() const
{
  return QwtPlot::minimumSizeHint();
}

/*!
  return minimum size for qwt plot
*/
QSize Plot2d_Plot2d::minimumSizeHint() const
{
  return QSize( 0, 0 );
//  QSize aSize = QwtPlot::minimumSizeHint();
//  return QSize(aSize.width()*3/4, aSize.height());
}

void Plot2d_Plot2d::defaultPicker()
{
  myPlotZoomer->setMousePattern( QwtEventPattern::MouseSelect1,
                                 Qt::RightButton, Qt::ControlModifier ); // zooming button
  for ( int i = QwtEventPattern::MouseSelect2; i < QwtEventPattern::MouseSelect6; i++ )
    myPlotZoomer->setMousePattern( (QwtEventPattern::MousePatternCode)i, Qt::NoButton,
    		                       Qt::NoModifier );
}

void Plot2d_Plot2d::setPickerMousePattern( int button, int state )
{
  myPlotZoomer->setMousePattern( QwtEventPattern::MouseSelect1, (Qt::MouseButton)button,
		                         (Qt::KeyboardModifiers)state );
}

/*!
 * Set the point picker associated with a graphic view
 */
void Plot2d_Plot2d::setPicker( Plot2d_QwtPlotPicker *picker)
{
  myPicker = picker;
}

/*!
 * Create marker and tooltip associed with a point
 */
QwtPlotMarker* Plot2d_Plot2d::createMarkerAndTooltip( QwtSymbol* symbol,
                                                      double    X,
                                                      double    Y,
                                                      QString & tooltip,
                                                      Plot2d_QwtPlotPicker *picker)
{
  QwtPlotMarker* aPlotMarker = new QwtPlotMarker();

  aPlotMarker->setSymbol( symbol );  // symbol must have a color
  aPlotMarker->setLabelAlignment( Qt::AlignTop);
  aPlotMarker->setXValue(X);
  aPlotMarker->setYValue(Y);
  //
  aPlotMarker->attach(this);

  // Associate a tooltip with the point's marker
  // PB: how to obtain a tooltip with a rectangular frame ?
  //QwtText tooltip ("X=" + QString::number(X) + " Y=" + QString::number(Y) );

  QwtText text (tooltip);
  //QColor tooltipColor( 245, 222, 179);            // Wheat  -RGB (0 a 255)
  QColor tooltipColor( 253, 245, 230);            // OldLace
  text.setBackgroundBrush( QBrush(tooltipColor)); //, Qt::SolidPattern));
  //
  if ( picker ) {
    picker->pMarkers.append( aPlotMarker);
    picker->pMarkersToolTip[ aPlotMarker] = text;
  }
  return aPlotMarker;
}

QwtPlotGrid* Plot2d_Plot2d::grid() const
{
  return myGrid;
};

QwtPlotZoomer* Plot2d_Plot2d::zoomer() const
{
  return myPlotZoomer;
}

/*!
  Updates identifiers of Y axis type in the legend.
*/
void Plot2d_Plot2d::updateYAxisIdentifiers()
{
  bool enableYLeft = false, enableYRight = false;
  const QwtPlotItemList& items = itemList();
  QwtPlotItemIterator it;
  for ( it = items.begin(); it != items.end(); it++ ) {
    QwtPlotItem* item = *it;
    if ( item ) {
      enableYLeft  |= item->yAxis() == QwtPlot::yLeft;
      enableYRight |= item->yAxis() == QwtPlot::yRight;
    }
  }

  // if several curves are attached to different axes
  // display corresponding identifiers in the legend,
  // otherwise hide them
  for ( it = items.begin(); it != items.end(); it++ ) {
    QwtPlotItem* item = *it;
    if ( Plot2d_QwtPlotCurve* aPCurve = dynamic_cast<Plot2d_QwtPlotCurve*>( item ) )
      aPCurve->setYAxisIdentifierEnabled( enableYLeft && enableYRight );
    if ( item && item->isVisible() && legend() )
      item->updateLegend( item, item->legendData() );
  }
}

// Methods to manage axis graduations

/* Create definition and graduations of axes
 */
void Plot2d_Plot2d::createAxisScaleDraw()
{
  myScaleDraw = new Plot2d_AxisScaleDraw( this);
}


/* Stock X axis's ticks in the drawing zone
*/
void Plot2d_Plot2d::applyTicks()
{
  myScaleDraw->applyTicks();
}


/* Unactivate automatic ticks drawing (call to method Plot2d_AxisScaleDraw::draw() )
 * Parameters :
 * - number call to ticks drawing (for information) : numcall
 */
void Plot2d_Plot2d::unactivAxisScaleDraw( int numcall)
{
  // Memorize X axis (myScaleDraw already exists) in the drawing zone
  //setAxisScaleDraw( QwtPlot::xBottom, myScaleDraw);  // heritage of QwtPlot

  myScaleDraw->unactivTicksDrawing( numcall);
}


/* Draw ticks and labels on X axis of the drawing zone
 * Draw systems' names under the X axis of the drawing zone
 * Draw vertical segments between X axis's intervals of the systems
 * Parameters :
 * - left and right margins for ticks : XLeftMargin, XRightMargin
 * - for each named system :
 *     positions and labels for ticks on X axis : devicesPosLabelTicks
 *
 * The true drawings will be realized by the method Plot2d_AxisScaleDraw::draw()
 * PB: who call il ?
 */
void Plot2d_Plot2d::displayXTicksAndLabels(
                      double XLeftMargin, double XRightMargin,
                      const QList< QPair< QString, QMap<double, QString> > > & devicesPosLabelTicks)
                      //                    name        position  label
                      //                   system         tick    tick
{
  //std::cout << "Plot2d_Plot2d::displayXTicksAndLabels() 1" << std::endl;

  int nbDevices = devicesPosLabelTicks.size();
  //
  //std::cout << "  Nombre de systemes = " << nbDevices << std::endl;
  if (nbDevices == 0)  return;

  // For drawing systems' names, their positions must be in the allTicks list
  // (cf class Plot2d_AxisScaleDraw)

  // Liste of ticks' positions and systems' names
  QList<double> allTicks;

  double devXmin, devXmax;  // X interval of a system
  double gapXmin, gapXmax;  // X interval between two systems
  double devLabPos;         // Label's position of a system
  double segmentPos;  // Position of the vertical segment between current system and the next

  // 1)- Search for the system whose X interval is the most to the left

  int ileftDev = 0;
  double XminMin = 1.e+12;

  if (nbDevices > 1)
  {
      for (int idev=0; idev < nbDevices; idev++)
      {
          QPair< QString, QMap<double,QString> > paire = devicesPosLabelTicks.at(idev);

          QString deviceLabel = paire.first;

          // Ticks' map of the system
          QMap<double,QString> devPosLabelTicks = paire.second;

          QList<double> posTicks = devPosLabelTicks.keys();

          // List's items increasing sorting
          qSort( posTicks.begin(), posTicks.end() );  // iterators

          // X interval for the system
          devXmin = posTicks.first();
          devXmax = posTicks.last();

          if (devXmin < XminMin)
          {
              XminMin = devXmin;
              ileftDev = idev;
          }
      }
  }

  // 2)- Ticks, systems' names, verticals segments

  for (int idev=0; idev < nbDevices; idev++)
  {
      QPair< QString, QMap<double,QString> > paire = devicesPosLabelTicks.at(idev);

      QString deviceLabel = paire.first;

      std::string std_label = deviceLabel.toStdString();
      //const char *c_label = std_label.c_str();
      //std::cout << "  deviceLabel: |" << c_label << "|" << std::endl;

      // Ticks' map of the system
      QMap<double,QString> devPosLabelTicks = paire.second;

      int nbTicks = devPosLabelTicks.size();

      QList<double> posTicks = devPosLabelTicks.keys();

      // List's items increasing sorting
      qSort( posTicks.begin(), posTicks.end() );  // iterators

      // X interval for the system
      devXmin = posTicks.first();
      devXmax = posTicks.last();

      // Stock ticks' positions and labels on X axis
      double pos;
      QString label;
      //
      for (int itic=0; itic < nbTicks; itic++)
      {
          pos   = posTicks.at(itic);
          label = devPosLabelTicks[pos];

          myScaleDraw->setLabelTick( pos, label, false);

          std::string std_label = label.toStdString();
          //const char *c_label = std_label.c_str();
          //std::cout << "    tick " << itic << " : pos= " << pos << ", label= |" << c_label << "|" << std::endl;
      }
      allTicks.append( posTicks);

      // Compute the position of the system's label
      if (idev == ileftDev)
      {
          devLabPos = devXmin + 0.25*(devXmax - devXmin);
      }
      else
      {
          devLabPos = devXmin + 0.50*(devXmax - devXmin);
      }
      allTicks.append( devLabPos);

      // Stock position and name of the system under X axis
      myScaleDraw->setLabelTick( devLabPos, deviceLabel, true);

      if (idev > 0)
      {
          // Create the vertical segment between the current system and the next
          gapXmax = devXmin;
          segmentPos = gapXmin + 0.5*(gapXmax - gapXmin);

          createSeparationLine( segmentPos);
      }
      gapXmin = devXmax;
  }

  // List's items increasing sorting
  qSort( allTicks.begin(), allTicks.end() );  // iterators

  // Stock the interval of X's values
  double lowerBound = allTicks.first() - XLeftMargin;
  double upperBound = allTicks.last() + XRightMargin;
  myScaleDraw->setInterval( lowerBound, upperBound);

  // For each system, stock the position of the X's ticks and those of the name
  myScaleDraw->setTicks( allTicks);  // do not draw the ticks

  // Memorize the X axis in the drawing zone
  setAxisScaleDraw( QwtPlot::xBottom, myScaleDraw);  // heritage of QwtPlot

  //std::cout << "Plot2d_Plot2d::displayXTicksAndLabels() 1" << std::endl;
}


/* Create vertical segment between two curves
 */
void Plot2d_Plot2d::createSeparationLine( double Xpos)
{
  QwtPlotMarker* aPlotMarker = new QwtPlotMarker();

  aPlotMarker->setLineStyle( QwtPlotMarker::VLine);
  aPlotMarker->setXValue( Xpos);
  aPlotMarker->setLinePen( QPen(Qt::black));
  aPlotMarker->attach(this);  // Add to drawing zone
  // To deallocate in EraseAll()
  mySeparationLineList.append( aPlotMarker);
}

void Plot2d_Plot2d::clearSeparationLineList()
{
  mySeparationLineList.clear();
}

/* Set type of legend symbol
 */
void Plot2d_Plot2d::setLegendSymbolType( const int type )
{
  myLegendSymbolType = type;
}

/* Get type of legend symbol
 */
int Plot2d_Plot2d::getLegendSymbolType()
{
  return myLegendSymbolType;
}

/*!
  Creates presentation of object
  Default implementation is empty
*/
Plot2d_Prs* Plot2d_ViewFrame::CreatePrs( const char* /*entry*/ )
{
  return 0;
}

/*!
  Copies preferences from other viewframe
  \param vf - other view frame
*/
void Plot2d_ViewFrame::copyPreferences( Plot2d_ViewFrame* vf )
{
  if( !vf )
    return;

  myCurveType = vf->myCurveType;
  myShowLegend = vf->myShowLegend;
  myLegendPos = vf->myLegendPos;
  myLegendSymbolType = vf->myLegendSymbolType;
  myLegendFont = vf->myLegendFont;
  mySelectedLegendFontColor = vf->mySelectedLegendFontColor;
  myMarkerSize = vf->myMarkerSize;
  myBackground = vf->myBackground;
  mySelectionColor = vf->mySelectionColor;
  myTitle = vf->myTitle;
  myXTitle = vf->myXTitle;
  myYTitle = vf->myYTitle;
  myY2Title = vf->myY2Title;
  myTitleEnabled = vf->myTitleEnabled;
  myXTitleEnabled = vf->myXTitleEnabled;
  myYTitleEnabled = vf->myYTitleEnabled;
  myY2TitleEnabled = vf->myY2TitleEnabled;
  myXGridMajorEnabled = vf->myXGridMajorEnabled;
  myYGridMajorEnabled = vf->myYGridMajorEnabled;
  myY2GridMajorEnabled = vf->myY2GridMajorEnabled;
  myXGridMinorEnabled = vf->myXGridMinorEnabled;
  myYGridMinorEnabled = vf->myYGridMinorEnabled;
  myY2GridMinorEnabled = vf->myY2GridMinorEnabled;
  myXGridMaxMajor = vf->myXGridMaxMajor;
  myYGridMaxMajor = vf->myYGridMaxMajor;
  myY2GridMaxMajor = vf->myY2GridMaxMajor;
  myXGridMaxMinor = vf->myXGridMaxMinor;
  myYGridMaxMinor = vf->myYGridMaxMinor;
  myY2GridMaxMinor = vf->myY2GridMaxMinor;
  myXMode = vf->myXMode;
  myYMode = vf->myYMode;
  mySecondY = vf->mySecondY;
}

/*!
  Updates titles according to curves
*/
#define BRACKETIZE(x) QString( "[ " ) + x + QString( " ]" )
void Plot2d_ViewFrame::updateTitles( const bool update )
{
  ObjectDict::iterator it = myObjects.begin();
  QStringList aXTitles;
  QStringList aYTitles;
  QStringList aY2Titles;
  QStringList aXUnits;
  QStringList aYUnits;
  QStringList aY2Units;
  QStringList aTables;
  int i = 0;

  Plot2d_Object* anObject;
  for ( ; it != myObjects.end(); it++ ) {
    // collect titles and units from all curves...
    anObject = it.value();
    QString xTitle = anObject->getHorTitle().trimmed();
    QString yTitle = anObject->getVerTitle().trimmed();
    QString xUnits = anObject->getHorUnits().trimmed();
    QString yUnits = anObject->getVerUnits().trimmed();

    if ( anObject->getYAxis() == QwtPlot::yLeft ) {
      if ( !aYTitles.contains( yTitle ) )
        aYTitles.append( yTitle );
      if ( !aYUnits.contains( yUnits ) )
        aYUnits.append( yUnits );
    }
    else {
      if ( !aY2Titles.contains( yTitle ) )
        aY2Titles.append( yTitle );
      if ( !aY2Units.contains( yUnits ) )
        aY2Units.append( yUnits );
    }
    if ( !aXTitles.contains( xTitle ) )
      aXTitles.append( xTitle );
    if ( !aXUnits.contains( xUnits ) )
      aXUnits.append( xUnits );

    QString aName = anObject->getTableTitle();
    if( !aName.isEmpty() && !aTables.contains( aName ) )
      aTables.append( aName );
    ++i;
  }
  // ... and update plot 2d view
  QString xUnits, yUnits, y2Units;
  if ( aXUnits.count() == 1 && !aXUnits[0].isEmpty() )
    xUnits = BRACKETIZE( aXUnits[0] );
  if ( aYUnits.count() == 1 && !aYUnits[0].isEmpty())
    yUnits = BRACKETIZE( aYUnits[0] );
  if ( aY2Units.count() == 1 && !aY2Units[0].isEmpty())
    y2Units = BRACKETIZE( aY2Units[0] );
  QString xTitle, yTitle, y2Title;
  if ( aXTitles.count() == 1 && aXUnits.count() == 1 )
    xTitle = aXTitles[0];
  if ( aYTitles.count() == 1 )
    yTitle = aYTitles[0];
  if ( mySecondY && aY2Titles.count() == 1 )
    y2Title = aY2Titles[0];

  if ( !xTitle.isEmpty() && !xUnits.isEmpty() )
    xTitle += " ";
  if ( !yTitle.isEmpty() && !yUnits.isEmpty() )
    yTitle += " ";
  if ( !y2Title.isEmpty() && !y2Units.isEmpty() )
    y2Title += " ";

  setTitle( myXTitleEnabled, xTitle + xUnits, XTitle, false );
  setTitle( myYTitleEnabled, yTitle + yUnits, YTitle, false );
  if ( mySecondY )
    setTitle( myY2TitleEnabled, y2Title + y2Units, Y2Title, false );
  if( myIsDefTitle )
    setTitle( true, aTables.join("; "), MainTitle, false );

  // do update if necessary
  if ( update ) {
    myPlot->replot();
  }
}

/*!
  Outputs content of viewframe to file
  \param file - file name
  \param format - file format
*/
bool Plot2d_ViewFrame::print( const QString& file, const QString& format ) const
{
#ifdef WIN32
  return false;

#else
  bool res = false;
  if( myPlot )
  {
    QPaintDevice* pd = 0;
    if( format=="PS" || format=="EPS" || format == "PDF" )
    {
      QPrinter* pr = new QPrinter( QPrinter::HighResolution );
      if(format == "PDF")
    	  pr->setOutputFormat(QPrinter::PdfFormat);
      pr->setPageSize( QPrinter::A4 );
      pr->setOutputFileName( file );
      pr->setPrintProgram( "" );
      pd = pr;
    }

    if( pd )
    {
      QwtPlotRenderer* pr = new QwtPlotRenderer();
      pr->renderTo( myPlot, *pd );
      res = true;
      delete pd;
    }
  }
  return res;
#endif

}

/**
 * Print Plot2d window
 */
void Plot2d_ViewFrame::printPlot( QPainter* p, const QRectF& rect) const
{
  QwtPlotRenderer* pr = new QwtPlotRenderer();
  pr->render( myPlot, p, rect );
}

/*!
  \return string with all visual parameters
*/
QString Plot2d_ViewFrame::getVisualParameters()
{

  return getXmlVisualParameters();

  /*
  RNV: Old case, now visual parameters stored in the XML format.
  //
  double xmin, xmax, ymin, ymax, y2min, y2max;
  getFitRanges( xmin, xmax, ymin, ymax, y2min, y2max );
  QString retStr;
  //Store font in the visual parameters string as:
  //
  // ...*FontFamily|FontSize|B|I|U|r:g:b*...

  retStr.sprintf( "%d*%d*%d*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%s|%i|%i|%i|%i|%i:%i:%i",
		  myXMode, myYMode, mySecondY, xmin, xmax, ymin, ymax, y2min, y2max,
		  qPrintable(myLegendFont.family()), myLegendFont.pointSize(),myLegendFont.bold(),
		  myLegendFont.italic(), myLegendFont.underline(),myLegendColor.red(),
		  myLegendColor.green(), myLegendColor.blue());

 //store all Analytical curves
  //store each curve in the following format
  // ...*Name|isActive|Expresion|NbInervals|isAutoAssign[|MarkerType|LineType|LineWidth|r:g:b]
  // parameters in the [ ] is optional in case if isAutoAssign == true
  AnalyticalCurveList::iterator it = myAnalyticalCurves.begin();
  Plot2d_AnalyticalCurve* c = 0;
  bool isAuto;
  for( ; it != myAnalyticalCurves.end(); it++) {
    c = (*it);
    if(!c) continue;
    QString curveString("");
    isAuto = c->isAutoAssign();
    curveString.sprintf("*%s|%i|%s|%i|%i",
			qPrintable(c->getName()),
			c->isActive(),
			qPrintable(c->getExpression()),
			c->getNbIntervals(),
			isAuto);

    retStr+=curveString;
    if(!isAuto) {
      QString optCurveString("");
      optCurveString.sprintf("|%i|%i|%i|%i:%i:%i",
			     (int)c->getMarker(),
			     (int)c->getLine(),
			     c->getLineWidth(),
			     c->getColor().red(),
			     c->getColor().green(),
			     c->getColor().blue());
      retStr+=optCurveString;
    }
  }
  retStr += QString( "*%1" ).arg( Qtx::colorToString( backgroundColor() ) );
  return retStr;
  */
}

/*!
  Restores all visual parameters from string
*/
void Plot2d_ViewFrame::setVisualParameters( const QString& parameters )
{
  if(setXmlVisualParameters(parameters))
    return;

  double xmin, xmax;
  QStringList paramsLst = parameters.split( '*' );
  if ( paramsLst.size() >= 9 ) {
    double ymin, ymax, y2min, y2max;
    myXMode = paramsLst[0].toInt();
    myYMode = paramsLst[1].toInt();
    mySecondY = (bool)paramsLst[2].toInt();
    xmin =  paramsLst[3].toDouble();
    xmax =  paramsLst[4].toDouble();
    ymin =  paramsLst[5].toDouble();
    ymax =  paramsLst[6].toDouble();
    y2min = paramsLst[7].toDouble();
    y2max = paramsLst[8].toDouble();

    if (mySecondY)
      setTitle( myY2TitleEnabled, myY2Title, Y2Title, false );
    setHorScaleMode( myXMode, /*update=*/false );
    setVerScaleMode( myYMode, /*update=*/false );

    if (mySecondY) {
      QwtScaleMap yMap2 = myPlot->canvasMap( QwtPlot::yRight );
      myYDistance2 = yMap2.s2() - yMap2.s1();
    }

    fitData( 0, xmin, xmax, ymin, ymax, y2min, y2max );
    fitData( 0, xmin, xmax, ymin, ymax, y2min, y2max );
  }

  //Restore legend font
  if(paramsLst.size() >= 10) {
    QStringList fontList = paramsLst[9].split( '|' );
    if(fontList.size() == 6) {
      myLegendFont = QFont(fontList[0]);
      myLegendFont.setPointSize(fontList[1].toInt());
      myLegendFont.setBold(fontList[2].toInt());
      myLegendFont.setItalic(fontList[3].toInt());
      myLegendFont.setUnderline(fontList[4].toInt());
      QStringList colorList = fontList[5].split(":");
      setLegendFont( myLegendFont );

      if(colorList.size() == 3) {
	      myLegendColor = QColor(colorList[0].toInt(),
			                         colorList[1].toInt(),
			                         colorList[2].toInt());
	      setLegendFontColor( myLegendColor );
      }
    }
  }

#ifndef NO_ANALYTICAL_CURVES
  //Restore all Analytical curves
  int startCurveIndex = 10;
  if( paramsLst.size() >= startCurveIndex+1 ) {
    for( int i=startCurveIndex; i<paramsLst.size() ; i++ ) {
      QStringList curveLst = paramsLst[i].split("|");
      if( curveLst.size() == 5 || curveLst.size() == 9 ) {
	Plot2d_AnalyticalCurve* c = new Plot2d_AnalyticalCurve();
	c->setName(curveLst[0]);
	c->setActive(curveLst[1].toInt());
	c->setExpression(curveLst[2]);
	c->setNbIntervals(curveLst[3].toLong());
	c->setAutoAssign(curveLst[4].toInt());
	if( !c->isAutoAssign() ) {
	  c->setMarker((Plot2d::MarkerType)curveLst[5].toInt());
	  c->setLine((Plot2d::LineType)curveLst[6].toInt());
	  c->setLineWidth(curveLst[7].toInt());
	  QStringList colorList = curveLst[8].split(":");
	  if( colorList.size() == 3 ) {
	    c->setColor(QColor(colorList[0].toInt(),
			       colorList[1].toInt(),
			       colorList[2].toInt()));
	  }
	} else {
	  c->autoFill( myPlot );
	}
	addAnalyticalCurve(c);
	updateAnalyticalCurve(c);
      }
      else if( curveLst.size() == 1 ) {
	// background color can be set here
	QColor c;
	if ( Qtx::stringToColor( paramsLst[i], c ) )
	  setBackgroundColor( c );
      }
    }
    myPlot->replot();
  }
#endif
}

/*!
  Store visual parameters in xml format.
*/
QString Plot2d_ViewFrame::getXmlVisualParameters() {
  QString retStr;
  QXmlStreamWriter aWriter(&retStr);
  aWriter.setAutoFormatting(true);

  //Ranges
  aWriter.writeStartDocument();
  aWriter.writeStartElement("ViewState");
  aWriter.writeStartElement("Range");
  double xmin, xmax, ymin, ymax, y2min, y2max;
  getFitRanges( xmin, xmax, ymin, ymax, y2min, y2max );
  aWriter.writeAttribute("Xmin", QString("").sprintf("%.12e",xmin));
  aWriter.writeAttribute("Xmax", QString("").sprintf("%.12e",xmax));
  aWriter.writeAttribute("Ymin", QString("").sprintf("%.12e",ymin));
  aWriter.writeAttribute("Ymax", QString("").sprintf("%.12e",ymax));
  aWriter.writeAttribute("Y2min", QString("").sprintf("%.12e",y2min));
  aWriter.writeAttribute("Y2max", QString("").sprintf("%.12e",y2max));
  aWriter.writeEndElement();

  //Display mode
  aWriter.writeStartElement("DisplayMode");
  aWriter.writeAttribute("SecondAxis", QString("").sprintf("%d",mySecondY));
  aWriter.writeStartElement("ScaleMode");
  aWriter.writeAttribute("Xscale", QString("").sprintf("%d",myXMode));
  aWriter.writeAttribute("Yscale", QString("").sprintf("%d",myYMode));
  aWriter.writeEndElement();
  aWriter.writeStartElement("NormalizationMode");
  aWriter.writeAttribute("LeftMin", QString("").sprintf("%d",myNormLMin));
  aWriter.writeAttribute("LeftMax", QString("").sprintf("%d",myNormLMax));
  aWriter.writeAttribute("RightMin", QString("").sprintf("%d",myNormRMin));
  aWriter.writeAttribute("RightMax", QString("").sprintf("%d",myNormRMax));
  aWriter.writeEndElement();
  aWriter.writeEndElement();

  //Legend
  aWriter.writeStartElement("Legend");
  aWriter.writeAttribute("Visibility", QString("").sprintf("%d", myShowLegend));
  aWriter.writeStartElement("LegendFont");
  aWriter.writeAttribute("Family", myLegendFont.family());
  aWriter.writeAttribute("Size", QString("").sprintf("%d",myLegendFont.pointSize()));
  aWriter.writeAttribute("Bold", QString("").sprintf("%d",myLegendFont.bold()));
  aWriter.writeAttribute("Italic", QString("").sprintf("%d",myLegendFont.italic()));
  aWriter.writeAttribute("Underline", QString("").sprintf("%d",myLegendFont.underline()));
  aWriter.writeAttribute("R", QString("").sprintf("%d",myLegendColor.red()));
  aWriter.writeAttribute("G", QString("").sprintf("%d",myLegendColor.green()));
  aWriter.writeAttribute("B", QString("").sprintf("%d",myLegendColor.blue()));
  aWriter.writeEndElement();
  aWriter.writeEndElement();

#ifndef NO_ANALYTICAL_CURVES
  //AnalyticalCurve
  aWriter.writeStartElement("AnalyticalCurves");
  AnalyticalCurveList::iterator it = myAnalyticalCurves.begin();
  Plot2d_AnalyticalCurve* c = 0;
  bool isAuto;
  int id = 1;
  for( ; it != myAnalyticalCurves.end(); it++) {
    c = (*it);
    if(!c) continue;
    aWriter.writeStartElement(QString("AnalyticalCurve_%1").arg(id));
    isAuto = c->isAutoAssign();
    aWriter.writeAttribute("Name",c->getName());
    aWriter.writeAttribute("IsActive", QString("").sprintf("%d",	c->isActive()));
		aWriter.writeAttribute("Expression", c->getExpression());
    aWriter.writeAttribute("NbIntervals", QString("").sprintf("%d",	c->getNbIntervals()));
    aWriter.writeAttribute("isAuto", QString("").sprintf("%d",isAuto));
    if(!isAuto) {
      aWriter.writeAttribute("Marker", QString("").sprintf("%d",(int)c->getMarker()));
      aWriter.writeAttribute("Line", QString("").sprintf("%d",(int)c->getLine()));
      aWriter.writeAttribute("LineWidth", QString("").sprintf("%d",c->getLineWidth()));
      aWriter.writeAttribute("R", QString("").sprintf("%d",c->getColor().red()));
      aWriter.writeAttribute("G", QString("").sprintf("%d",c->getColor().green()));
      aWriter.writeAttribute("B", QString("").sprintf("%d",c->getColor().blue()));
    }
    aWriter.writeEndElement();
    id++;
  }
  aWriter.writeEndElement(); //AnalyticalCurve
#endif

  //Background
  aWriter.writeStartElement(QString("Background"));
  aWriter.writeStartElement(QString("BackgroundColor"));
  aWriter.writeAttribute("R", QString("").sprintf("%d",backgroundColor().red()));
  aWriter.writeAttribute("G", QString("").sprintf("%d",backgroundColor().green()));
  aWriter.writeAttribute("B", QString("").sprintf("%d",backgroundColor().blue()));
  aWriter.writeEndElement();
  aWriter.writeEndElement();


  aWriter.writeEndDocument();
  return retStr;
}
/*!
  Restore visual parameters from xml format.
*/
bool Plot2d_ViewFrame::setXmlVisualParameters(const QString& parameters) {
  QXmlStreamReader aReader(parameters);
  double xmin, xmax, ymin, ymax, y2min, y2max;
  bool leftMin,leftMax,rightMin,rightMax;
  leftMin = leftMax = rightMin = rightMax = false;
  while(!aReader.atEnd()) {
      aReader.readNext();
      if (aReader.isStartElement()) {
      QXmlStreamAttributes aAttr = aReader.attributes();
        if(aReader.name() == "Range") {
          xmin = aAttr.value("Xmin").toString().toDouble();
          xmax = aAttr.value("Xmax").toString().toDouble();
          ymin = aAttr.value("Ymin").toString().toDouble();
          ymax = aAttr.value("Ymax").toString().toDouble();
          y2min = aAttr.value("Y2min").toString().toDouble();
          y2max = aAttr.value("Y2max").toString().toDouble();
        } else if(aReader.name() == "DisplayMode") {
          mySecondY = aAttr.value("Y2max").toString().toDouble();
        } else if(aReader.name() == "ScaleMode") {
           myXMode = aAttr.value("Xscale").toString().toInt();
           myYMode = aAttr.value("Yscale").toString().toInt();
        } else if(aReader.name() == "NormalizationMode") {
            leftMin = (bool)aAttr.value("LeftMin").toString().toInt();
            leftMax = (bool)aAttr.value("LeftMax").toString().toInt();
            rightMin = (bool)aAttr.value("RightMin").toString().toInt();
            rightMax = (bool)aAttr.value("RightMax").toString().toInt();
        } else if(aReader.name() == "Legend") {
          myShowLegend = (bool)aAttr.value("Visibility").toString().toInt();
        } else if (aReader.name() == "LegendFont") {
            myLegendFont = QFont(aAttr.value("Family").toString());
            myLegendFont.setPointSize(aAttr.value("Size").toString().toInt());
            myLegendFont.setBold((bool)aAttr.value("Bold").toString().toInt());
            myLegendFont.setItalic((bool)aAttr.value("Italic").toString().toInt());
            myLegendFont.setUnderline((bool)aAttr.value("Underline").toString().toInt());
    	      myLegendColor = QColor(aAttr.value("R").toString().toInt(),
			                             aAttr.value("G").toString().toInt(),
			                             aAttr.value("B").toString().toInt());
	          setLegendFontColor( myLegendColor );
            setLegendFont(myLegendFont);
        } else if(aReader.name().toString().indexOf("AnalyticalCurve_") >= 0) {
#ifndef NO_ANALYTICAL_CURVES
            Plot2d_AnalyticalCurve* c = new Plot2d_AnalyticalCurve();
	          c->setName(aAttr.value("Name").toString());
	          c->setActive((bool)aAttr.value("IsActive").toString().toInt());
	          c->setExpression(aAttr.value("Expression").toString());
	          c->setNbIntervals(aAttr.value("NbIntervals").toString().toLong());
	          c->setAutoAssign((bool)aAttr.value("isAuto").toString().toInt());
	          if( !c->isAutoAssign() ) {
	            c->setMarker((Plot2d::MarkerType)aAttr.value("Marker").toString().toInt());
	            c->setLine((Plot2d::LineType)aAttr.value("Line").toString().toInt());
	            c->setLineWidth(aAttr.value("LineWidth").toString().toInt());
	            c->setColor(QColor(aAttr.value("R").toString().toInt(),
			                           aAttr.value("G").toString().toInt(),
			                           aAttr.value("B").toString().toInt()));
            } else {
	            c->autoFill( myPlot );
	          }
	          addAnalyticalCurve(c);
	          updateAnalyticalCurve(c);
#endif
        } else if(aReader.name().toString() == "BackgroundColor") {
          setBackgroundColor(QColor(aAttr.value("R").toString().toInt(),
			                              aAttr.value("G").toString().toInt(),
			                              aAttr.value("B").toString().toInt()));
        }
      }
  }

  if(aReader.hasError())
    return false;

  if (mySecondY)
    setTitle( myY2TitleEnabled, myY2Title, Y2Title, false );
  setHorScaleMode( myXMode, /*update=*/false );
  setVerScaleMode( myYMode, /*update=*/false );
  if (mySecondY) {
    QwtScaleMap yMap2 = myPlot->canvasMap( QwtPlot::yRight );
    myYDistance2 = yMap2.s2() - yMap2.s1();
  }
  setNormLMinMode(leftMin);
  setNormLMaxMode(leftMax);
  setNormRMinMode(rightMin);
  setNormRMaxMode(rightMax);

  showLegend( myShowLegend, false );

  fitData( 0, xmin, xmax, ymin, ymax, y2min, y2max );
  return true;
}

/*!
  Incremental zooming operation
*/
void Plot2d_ViewFrame::incrementalPan( const int incrX, const int incrY ) {
  QwtScaleMap xMap = myPlot->canvasMap( QwtPlot::xBottom );
  QwtScaleMap yMap = myPlot->canvasMap( QwtPlot::yLeft );

  myPlot->setAxisScale( QwtPlot::yLeft,
                        myPlot->invTransform( QwtPlot::yLeft, yMap.transform( yMap.s1() )-incrY ),
                        myPlot->invTransform( QwtPlot::yLeft, yMap.transform( yMap.s2() )-incrY ) );
  myPlot->setAxisScale( QwtPlot::xBottom,
                        myPlot->invTransform( QwtPlot::xBottom, xMap.transform( xMap.s1() )-incrX ),
                        myPlot->invTransform( QwtPlot::xBottom, xMap.transform( xMap.s2() )-incrX ) );
  if (mySecondY) {
    QwtScaleMap y2Map = myPlot->canvasMap( QwtPlot::yRight );
    myPlot->setAxisScale( QwtPlot::yRight,
                          myPlot->invTransform( QwtPlot::yRight, y2Map.transform( y2Map.s1() )-incrY ),
                          myPlot->invTransform( QwtPlot::yRight, y2Map.transform( y2Map.s2() )-incrY ) );
  }
  myPlot->replot();
}

/*!
  Incremental panning operation
*/
void Plot2d_ViewFrame::incrementalZoom( const int incrX, const int incrY ) {
  QwtScaleMap xMap = myPlot->canvasMap( QwtPlot::xBottom );
  QwtScaleMap yMap = myPlot->canvasMap( QwtPlot::yLeft );

  myPlot->setAxisScale( QwtPlot::yLeft, yMap.s1(),
                        myPlot->invTransform( QwtPlot::yLeft, yMap.transform( yMap.s2() ) + incrY ) );
  myPlot->setAxisScale( QwtPlot::xBottom, xMap.s1(),
                        myPlot->invTransform( QwtPlot::xBottom, xMap.transform( xMap.s2() ) - incrX ) );
  if (mySecondY) {
    QwtScaleMap y2Map = myPlot->canvasMap( QwtPlot::yRight );
    myPlot->setAxisScale( QwtPlot::yRight, y2Map.s1(),
                          myPlot->invTransform( QwtPlot::yRight, y2Map.transform( y2Map.s2() ) + incrY ) );
  }
  myPlot->replot();
}

/*
  Update plot item
*/
void Plot2d_ViewFrame::updatePlotItem(Plot2d_Object* theObject, QwtPlotItem* theItem) {
  theObject->updatePlotItem( theItem );
  Plot2d_Curve* c = dynamic_cast<Plot2d_Curve*>(theObject);
  QwtPlotCurve* cu = dynamic_cast<QwtPlotCurve*>(theItem);
  Plot2d_NormalizeAlgorithm* aNormAlgo;
  if(c && cu) {
    if(c->getYAxis() == QwtPlot::yRight)
      aNormAlgo = myRNormAlgo;
    else
      aNormAlgo = myLNormAlgo;
    if(aNormAlgo->getNormalizationMode() != Plot2d_NormalizeAlgorithm::NormalizeNone) {
      AlgoPlot2dOutputData aResultData =  aNormAlgo->getOutput();
      AlgoPlot2dOutputData::iterator itTmp = aResultData.find(theObject);
      double *xNew,*yNew;
      int size = itTmp.value().size();
      xNew = new double[size];
      yNew = new double[size];
      int j = 0;
      for (; j < size; ++j) {
        xNew[j] = itTmp.value().at(j).first;
        yNew[j] = itTmp.value().at(j).second;
      }
      cu->setSamples(xNew, yNew,j);
      delete [] xNew;
      delete [] yNew;
      if(aNormAlgo->getNormalizationMode() != Plot2d_NormalizeAlgorithm::NormalizeNone) {
        QString name = c->getName().isEmpty() ? c->getVerTitle() : c->getName();
        name = name + QString("(B=%1, K=%2)");
        name = name.arg(aNormAlgo->getBkoef(c)).arg(aNormAlgo->getKkoef(c));
        cu->setTitle(name);
      }
    }
  }
}

/**
 *
 */
QwtPlotCanvas* Plot2d_ViewFrame::getPlotCanvas() const
{
  return myPlot ? (QwtPlotCanvas*)myPlot->canvas() : 0;
}

/*!
  return closest curve if it exist, else 0
*/
Plot2d_Curve* Plot2d_ViewFrame::getClosestCurve( QPoint p, double& distance, int& index ) const
{
  CurveDict aCurves = getCurves();
  CurveDict::iterator it = aCurves.begin();
  Plot2d_Curve* pCurve = 0;
  distance = -1.;
  for ( ; it != aCurves.end(); it++ ) {
    QwtPlotCurve* aCurve = it.key();
    if ( !aCurve )
      continue;
    double d;
    int i = aCurve->closestPoint( p, &d );
    if ( i > -1 && ( distance < 0 || d < distance ) ) {
      pCurve = it.value();
      distance = d;
      index = i;
    }
  }
  return pCurve;
}

#ifndef NO_ANALYTICAL_CURVES
/*!
 \brief  Deselect all analytical curves.
*/
void Plot2d_ViewFrame::deselectAnalyticalCurves() {
  foreach(Plot2d_AnalyticalCurve* c, myAnalyticalCurves) {
   c->setSelected(false);
  }
}
#endif

/*!
 \brief  Deselect all objects, except analytical curves.
*/
void Plot2d_ViewFrame::deselectObjects() {
  ObjectDict::const_iterator it = myObjects.begin(), aLast = myObjects.end();
  for ( ; it != aLast; it++ ) {
	  it.value()->setSelected(false);
   }
}

#define INCREMENT_FOR_OP 10

/*!
  Performs incremental panning to the left
*/
void Plot2d_ViewFrame::onPanLeft()
{
  this->incrementalPan( -INCREMENT_FOR_OP, 0 );
#ifndef NO_ANALYTICAL_CURVES
  updateAnalyticalCurves();
#endif
}

/*!
  Performs incremental panning to the right
*/
void Plot2d_ViewFrame::onPanRight()
{
  this->incrementalPan( INCREMENT_FOR_OP, 0 );
#ifndef NO_ANALYTICAL_CURVES
  updateAnalyticalCurves();
#endif
}

/*!
  Performs incremental panning to the top
*/
void Plot2d_ViewFrame::onPanUp()
{
  this->incrementalPan( 0, -INCREMENT_FOR_OP );
#ifndef NO_ANALYTICAL_CURVES
  updateAnalyticalCurves();
#endif
}

/*!
  Performs incremental panning to the bottom
*/
void Plot2d_ViewFrame::onPanDown()
{
  this->incrementalPan( 0, INCREMENT_FOR_OP );
#ifndef NO_ANALYTICAL_CURVES
  updateAnalyticalCurves();
#endif
}

/*!
  Performs incremental zooming in
*/
void Plot2d_ViewFrame::onZoomIn()
{
  this->incrementalZoom( INCREMENT_FOR_OP, INCREMENT_FOR_OP );
#ifndef NO_ANALYTICAL_CURVES
  updateAnalyticalCurves();
#endif
}

/*!
  Performs incremental zooming out
*/
void Plot2d_ViewFrame::onZoomOut()
{
  this->incrementalZoom( -INCREMENT_FOR_OP, -INCREMENT_FOR_OP );
#ifndef NO_ANALYTICAL_CURVES
  updateAnalyticalCurves();
#endif
}

/*!
  Schedules a FitAll operation by putting it to the application's
  event queue. This ensures that other important events (show, resize, etc.)
  are processed first.
*/
void Plot2d_ViewFrame::customEvent( QEvent* ce )
{
  if ( ce->type() == FITALL_EVENT )
    fitAll();
}


/*!
 * Return Plot2d_Object by the QwtPlotItem
 *
*/
Plot2d_Object* Plot2d_ViewFrame::getPlotObject( QwtPlotItem* plotItem ) const {

  ObjectDict::const_iterator it = myObjects.begin();
  for( ; it != myObjects.end(); ++it ) {
    if ( it.key() == plotItem ) {
      return it.value();
    }
  }
  return 0;
}

Plot2d_ScaleDraw::Plot2d_ScaleDraw( char f, int prec )
  : QwtScaleDraw(),
    myFormat(f),
    myPrecision(prec)
{
  invalidateCache();
}

QwtText Plot2d_ScaleDraw::label( double value ) const
{
  QwtScaleMap m = scaleMap();
  QString str1 = QwtScaleDraw::label( m.s1() ).text();
  QString str2 = QwtScaleDraw::label( m.s2() ).text();
  if ( str1 == str2 ) {
    double aDist = fabs(scaleMap().s2()-scaleMap().s1())/5;
    int precision = 0;
    while (aDist < 1 ) {
      precision++;
      aDist *= 10.;
    }
    if ( precision > 0 && value > 0 )
      return QLocale::system().toString( value,'f', precision );
  }

  return QwtScaleDraw::label( value );
}

Plot2d_YScaleDraw::Plot2d_YScaleDraw()
  : QwtScaleDraw()
{
}

QwtText Plot2d_YScaleDraw::label( double value ) const
{
  // Axis labels format
  QString strD = QString( "%1").arg( value, 10, 'e', 3); // format 10.3e

  return QwtText( strD);
}

/* Definition of X axis graduations
 */
const QString Plot2d_AxisScaleDraw::DEVICE_FONT = QString("Times");
const int     Plot2d_AxisScaleDraw::DEVICE_FONT_SIZE = 12;
const int     Plot2d_AxisScaleDraw::DEVICE_BY = 40;

Plot2d_AxisScaleDraw::Plot2d_AxisScaleDraw( Plot2d_Plot2d* plot)
: myPlot(plot)
{
  myLowerBound = -1;
  myUpperBound = -1;
  setLabelAlignment(Qt::AlignRight);
  setLabelRotation(45.);

  applyTicks();

  myActivTicksDrawing   = true;
  myNumTicksDrawingCall = 1;
}


Plot2d_AxisScaleDraw::~Plot2d_AxisScaleDraw()
{
}


/* Unactivate automatic ticks drawing
 */
void Plot2d_AxisScaleDraw::unactivTicksDrawing( int numcall)
{
  myActivTicksDrawing   = false;
  myNumTicksDrawingCall = numcall;
}


/* Draw X ticks and labels.
 * Draw systems names under X axis.
 * Overload the same name QwtScaleDraw method.
 * (PB: who call automaticaly this method)
 */
void Plot2d_AxisScaleDraw::draw( QPainter* painter, const QPalette & palette) const
{
  //std::cout << "Plot2d_AxisScaleDraw::draw() : activ= " << myActivTicksDrawing
  //                           << "  numcall= " << myNumTicksDrawingCall << std::endl;

  if (!myActivTicksDrawing)  return;

  //std::cout << "Plot2d_AxisScaleDraw::draw()" << std::endl;

  QList<double> major_ticks  = scaleDiv().ticks(QwtScaleDiv::MajorTick);
  QList<double> medium_ticks = scaleDiv().ticks(QwtScaleDiv::MediumTick);
  QList<double> minor_ticks  = scaleDiv().ticks(QwtScaleDiv::MinorTick);

  medium_ticks.clear();
  minor_ticks.clear();
  major_ticks.clear();

  major_ticks.append( myTicks);

  QwtScaleDiv aScaleDiv = myPlot->axisScaleDiv( QwtPlot::xBottom );
  aScaleDiv.setTicks( QwtScaleDiv::MajorTick,  major_ticks );
  aScaleDiv.setTicks( QwtScaleDiv::MediumTick, medium_ticks );
  aScaleDiv.setTicks( QwtScaleDiv::MinorTick,  minor_ticks );
  myPlot->setAxisScaleDiv( QwtPlot::xBottom, aScaleDiv );

  QwtScaleDraw *scale = myPlot->axisScaleDraw(QwtPlot::xBottom);
  ((Plot2d_AxisScaleDraw*)(scale))->applyTicks();

  QwtScaleDraw::draw( painter, palette);

  for (int i = 0; i < myTicks.size(); i++)
  {
      drawLabel( painter, myTicks[i]);
  }

  //std::cout << "Ok for Plot2d_AxisScaleDraw::draw()" << std::endl;
}


double Plot2d_AxisScaleDraw::extent( const QFont &font ) const
{
  QFontMetrics fm( font );
  return QwtScaleDraw::extent( font ) + fm.height() * 2;
}

QwtText Plot2d_AxisScaleDraw::label( double value) const
{
  if (myLabelX.contains(value))
          return myLabelX[value];

  return QwtText(QString::number(value, 'f', 1));
}


/* Stock position and label of a X tick
 */
void Plot2d_AxisScaleDraw::setLabelTick( double value, QString label, bool isDevice)
{
  //qDebug()<< "setLabelTick ( " << value << ","<< label <<" )";
  if ( isDevice )
  {
      // For systems names under X axis
      myLabelDevice[value] = label;
  }
  else
  {
      // For X axis graduations
      myLabelX[value] = label;
  }
}


/* Stock ticks positions of a system, and draw them
 */
void Plot2d_AxisScaleDraw::setTicks(const QList<double> aTicks)
{
  //std::cout << "  Plot2d_AxisScaleDraw::setTicks()" << std::endl;
  myTicks = aTicks;

  applyTicks();
}


void Plot2d_AxisScaleDraw::setInterval(double lowerBound, double upperBound)
{
  myLowerBound = lowerBound;
  myUpperBound = upperBound;
  myPlot->setAxisScale( QwtPlot::xBottom, myLowerBound, myUpperBound );
}


/* Stock X ticks in drawing zone
 */
void Plot2d_AxisScaleDraw::applyTicks()
{
  //std::cout << "  Plot2d_AxisScaleDraw::applyTicks()" << std::endl;

  QList<double> major_ticks = scaleDiv().ticks(QwtScaleDiv::MajorTick);
  QList<double> medium_ticks = scaleDiv().ticks(QwtScaleDiv::MediumTick);
  QList<double> minor_ticks = scaleDiv().ticks(QwtScaleDiv::MinorTick);

  medium_ticks.clear();
  minor_ticks.clear();

  QwtScaleDiv aQwtScaleDiv = myPlot->axisScaleDiv( QwtPlot::xBottom );
  aQwtScaleDiv.setTicks( QwtScaleDiv::MajorTick, myTicks );
  aQwtScaleDiv.setTicks( QwtScaleDiv::MediumTick, medium_ticks );
  aQwtScaleDiv.setTicks( QwtScaleDiv::MinorTick, minor_ticks );
  myPlot->setAxisScaleDiv( QwtPlot::xBottom, aQwtScaleDiv );

  QwtScaleDiv* aScaleDiv = (QwtScaleDiv*) &scaleDiv();

  aScaleDiv->setTicks(QwtScaleDiv::MajorTick, myTicks);
  aScaleDiv->setTicks(QwtScaleDiv::MediumTick, medium_ticks);
  aScaleDiv->setTicks(QwtScaleDiv::MinorTick, minor_ticks);

  if (myLowerBound != -1 && myUpperBound != -1)
              aScaleDiv->setInterval(myLowerBound, myUpperBound);

  //for (int i = 0; i < myTicks.size(); i++){
  //  QPoint p = labelPosition( i );
  //  qDebug() << i<< ") applyTicks -> LABEL" <<p;
  //}
}


void Plot2d_AxisScaleDraw::drawLabel( QPainter* painter, double value) const
{
  //std::cout << "  Plot2d_AxisScaleDraw::drawLabel( " << value << " ) : "; //<< std::endl;

  //qDebug() << "drawLabel  ( " <<value<<" )";
  if ( myLabelDevice.contains(value) )
  {
      QString deviceLabel = myLabelDevice[value];
      //
      std::string std_label = deviceLabel.toStdString();
      //const char *c_label = std_label.c_str();
      //std::cout << "    deviceLabel= |" << c_label << "|" << std::endl;

      QPointF p = labelPosition( value );
      p += QPoint(0, DEVICE_BY);
      QFont  prevf = painter->font();
      //QColor prevc = (painter->pen()).color();

      QFont devicef( DEVICE_FONT, DEVICE_FONT_SIZE, QFont::Bold);
      //
      //painter->setPen( QColor("blue") );
      painter->setFont( devicef );
      painter->drawText( p, myLabelDevice[value] );
      //painter->setPen( prevc );
      painter->setFont( prevf );
  }
  if ( myLabelX.contains(value) )
  {
      QString xLabel = myLabelX[value];
      //
      std::string std_label = xLabel.toStdString();
      //const char *c_label = std_label.c_str();
      //std::cout << "    xLabel= |" << c_label << "|" << std::endl;

      QwtScaleDraw::drawLabel( painter, value );
  }
}


void Plot2d_AxisScaleDraw::drawTick( QPainter* painter, double value, int len) const
{
  //qDebug() << "drawTick  ( " <<value<<" , "<<len<<" )  " ;
  //qDebug() << "myLabelX" << myLabelX;
  //
  if ( myLabelX.contains(value) )
  {
      QwtScaleDraw::drawTick( painter, value, len);
  }
}


/* Management of tooltips associated with markers for curves points or others points
 */
const double Plot2d_QwtPlotPicker::BOUND_HV_SIZE = 0.2;

Plot2d_QwtPlotPicker::Plot2d_QwtPlotPicker( int            xAxis,
                                            int            yAxis,
                                            RubberBand     rubberBand,
                                            DisplayMode    trackerMode,
                                            QwtPlotCanvas *canvas)
: QwtPlotPicker( xAxis,
                 yAxis,
                 rubberBand,
                 trackerMode,
                 canvas)    // of drawing zone QwtPlot
{
}

Plot2d_QwtPlotPicker::Plot2d_QwtPlotPicker( int            xAxis,
                                            int            yAxis,
                                            QwtPlotCanvas *canvas)
: QwtPlotPicker( xAxis,
                 yAxis,
                 canvas)
{
}

Plot2d_QwtPlotPicker::~Plot2d_QwtPlotPicker()
{
}
// http://www.qtcentre.org/threads/22751-How-do-i-select-a-QwtPlotMarker-using-a-QPlotPicker

/* Return the tooltip associated with a point when the mouse cursor pass near
 */
QwtText Plot2d_QwtPlotPicker::trackerText( const QPoint & pos ) const
{
  for (QList<QwtPlotMarker* >::const_iterator pMarkerIt = pMarkers.begin();pMarkerIt != pMarkers.end(); ++pMarkerIt )
    {
      QwtPlotMarker* pMarker = *pMarkerIt;
      if ( pMarker != NULL )
        {
          const QwtSymbol* symb=pMarker->symbol();
          const QSize& sz=symb->size();
          const QwtScaleMap yMapRef=plot()->canvasMap(QwtPlot::yLeft);
          const QwtScaleMap xMap=plot()->canvasMap(pMarker->xAxis());
          const QwtScaleMap yMap=plot()->canvasMap(pMarker->yAxis());
          QwtDoubleRect  bound0=pMarker->boundingRect();
          int x1 = qRound(xMap.transform(bound0.left()));
          int x2 = qRound(xMap.transform(bound0.right()));
          int y1 = qRound(yMap.transform(bound0.top()));
          int y2 = qRound(yMap.transform(bound0.bottom()));
	  
          if ( x2 < x1 )
            qSwap(x1, x2);
          if ( y2 < y1 )
            qSwap(y1, y2);
	  
          QRect bound00=QRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
          QPoint toto(xMap.transform(pos.x()),yMapRef.transform(pos.y()));
          bound00.setX(bound00.x()-sz.width());
          bound00.setY(bound00.y()-sz.height());
          bound00.setWidth(bound00.width()+sz.width());
          bound00.setHeight(bound00.height()+sz.height());
          if( bound00.contains(toto) )
            {
              return pMarkersToolTip[pMarker];
            }
        }
  }
  return QwtText();
}
