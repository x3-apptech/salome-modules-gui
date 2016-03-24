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

// File   : Plot2d_ViewWindow.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "Plot2d_ViewWindow.h"
#include "Plot2d_ViewFrame.h"

#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>

#include <Qtx.h>
#include <QtxAction.h>
#include <QtxMultiAction.h>
#include <QtxActionToolMgr.h>

#include <QStatusBar>
#include <QLayout>
#include <QApplication>
#include <QMenu>
#include <QImage>
#include <QToolBar>
#include <QPaintEvent>
#include <QActionGroup>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>

#include <qwt_plot_curve.h>

/*!
  \class Plot2d_ViewWindow
  \brief Plot2d view window.
*/

/*!
  \brief Constructor.
  \param theDesktop parent desktop window
  \param theModel plt2d view model
*/
Plot2d_ViewWindow::Plot2d_ViewWindow( SUIT_Desktop* theDesktop, Plot2d_Viewer* theModel )
: SUIT_ViewWindow( theDesktop )
{
  myModel = theModel;
}

/*!
  \brief Destructor.
*/
Plot2d_ViewWindow::~Plot2d_ViewWindow()
{
}

/*!
  \brief Internal initialization.
*/
void Plot2d_ViewWindow::initLayout()
{
  myDumpImage = QImage();
  myViewFrame = new Plot2d_ViewFrame( this, "plotView" );
  setCentralWidget( myViewFrame );

  createActions();
  createToolBar();

  connect( myViewFrame, SIGNAL( vpModeHorChanged() ), this, SLOT( onChangeHorMode() ) );
  connect( myViewFrame, SIGNAL( vpModeVerChanged() ), this, SLOT( onChangeVerMode() ) );
  connect( myViewFrame, SIGNAL( vpNormLModeChanged() ), this, SLOT( onChangeNormLMode() ) );
  connect( myViewFrame, SIGNAL( vpNormRModeChanged() ), this, SLOT( onChangeNormRMode() ) );
  connect( myViewFrame, SIGNAL( vpCurveChanged() ),   this, SLOT( onChangeCurveMode() ) );
  connect( myViewFrame, SIGNAL( contextMenuRequested( QContextMenuEvent* ) ),
           this,        SIGNAL( contextMenuRequested( QContextMenuEvent* ) ) );

  myViewFrame->installEventFilter( this );
}

/*!
  \brief Get view model.
  \return Plot2d view model
*/
Plot2d_Viewer* Plot2d_ViewWindow::getModel()
{
  return myModel;
}

/*!
  \brief Put message to the status bar.
  \param theMsg message text
*/
void Plot2d_ViewWindow::putInfo( const QString& theMsg )
{
  QStatusBar* aStatusBar = myDesktop->statusBar();
  aStatusBar->showMessage( theMsg/*, 3000*/ );
}

/*!
  \brief Get view frame window.
  \return view frame window
*/
Plot2d_ViewFrame* Plot2d_ViewWindow::getViewFrame()
{ 
  return myViewFrame;
}

/*!
  \brief Get view window's toolbar.
  \return toolbar
*/
QToolBar* Plot2d_ViewWindow::getToolBar()
{
  return toolMgr()->toolBar( myToolBar );
}

/*!
  \brief Fill popup menu with the actions,
  \param thePopup popup menu
*/
void Plot2d_ViewWindow::contextMenuPopup( QMenu* thePopup )
{
  QtxActionToolMgr* mgr = toolMgr();
  // scaling
  QMenu* scalingPopup = thePopup->addMenu( tr( "SCALING_POPUP" ) );
  scalingPopup->addAction( mgr->action( PModeXLinearId ) );
  scalingPopup->addAction( mgr->action( PModeXLogarithmicId ) );
  scalingPopup->addSeparator();
  scalingPopup->addAction( mgr->action( PModeYLinearId ) );
  scalingPopup->addAction( mgr->action( PModeYLogarithmicId ) );

  // fit data
  thePopup->addAction( tr( "TOT_PLOT2D_FITDATA" ), myViewFrame, SLOT( onFitData() ) );

  // curve type
  QMenu* curTypePopup = thePopup->addMenu( tr( "CURVE_TYPE_POPUP" ) );
  curTypePopup->addAction( mgr->action( CurvPointsId ) );
  curTypePopup->addAction( mgr->action( CurvLinesId ) );
  curTypePopup->addAction( mgr->action( CurvSplinesId ) );

  //Normalization type
  QMenu* normTypePopup = thePopup->addMenu( tr( "NORMALIZATION_TYPE_POPUP" ) );
  normTypePopup->addAction( mgr->action( PModeNormLMinId ) );
  normTypePopup->addAction( mgr->action( PModeNormLMaxId ) );
  normTypePopup->addSeparator();
  normTypePopup->addAction( mgr->action( PModeNormRMinId ) );
  normTypePopup->addAction( mgr->action( PModeNormRMaxId ) );


  // legend
  thePopup->addAction( mgr->action( LegendId ) );

  // settings
  thePopup->addAction( mgr->action( CurvSettingsId ) );
}

/*!
  \brief Custom event filter.
  \param watched event receiver object
  \param e event
  \return \c true if further event processing should be stopped
*/
bool Plot2d_ViewWindow::eventFilter( QObject* watched, QEvent* e )
{
  if ( watched == myViewFrame ) {
    switch( e->type() ) {
    case QEvent::MouseButtonPress:
      emit mousePressed( this, (QMouseEvent*)e );
      return true;
    case QEvent::MouseButtonRelease:
      emit mouseReleased( this, (QMouseEvent*)e );
      return true;
    case QEvent::MouseMove:
      emit mouseMoving( this, (QMouseEvent*)e );
      return true;
    default:
      break;
    }
  }
  return SUIT_ViewWindow::eventFilter( watched, e );
}

/*!
  \brief Create actions for the view window.
*/
void Plot2d_ViewWindow::createActions()
{
  QtxActionToolMgr* mgr = toolMgr();
  QtxAction* aAction;
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  // 1. Dump View
  aAction = new QtxAction( tr( "MNU_DUMP_VIEW" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_DUMP" ) ),
                           tr( "MNU_DUMP_VIEW" ),
                           0, this);
  aAction->setStatusTip( tr( "DSC_DUMP_VIEW" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onDumpView() ) );
  mgr->registerAction( aAction, DumpId );

  // 2. Scaling operations

  // 2.1. Fit All
  aAction = new QtxAction( tr( "MNU_FITALL" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_FIT_ALL" ) ),
                           tr( "MNU_FITALL" ),
                           0, this);
  aAction->setStatusTip( tr( "DSC_FITALL" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onFitAll() ) );
  mgr->registerAction( aAction, FitAllId );

  // 2.2. Fit Rect
  aAction = new QtxAction( tr( "MNU_FITRECT" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_FIT_AREA" ) ),
                           tr( "MNU_FITRECT" ),
                           0, this);
  aAction->setStatusTip( tr( "DSC_FITRECT" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onFitRect() ) );
  mgr->registerAction( aAction, FitRectId );

  // 2.3. Zoom
  aAction = new QtxAction( tr( "MNU_ZOOM_VIEW" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ),
                           0, this);
  aAction->setStatusTip( tr( "DSC_ZOOM_VIEW" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onZoom() ) );
  mgr->registerAction( aAction, ZoomId );

  // 2.4. Create multi-action for scaling operations
  QtxMultiAction* aScaleAction = new QtxMultiAction( this );
  aScaleAction->insertAction( mgr->action( FitAllId  ) );
  aScaleAction->insertAction( mgr->action( FitRectId ) );
  aScaleAction->insertAction( mgr->action( ZoomId    ) );
  mgr->registerAction( aScaleAction, ScaleOpId );

  // 3. Moving operations

  // 3.1. Panning
  aAction = new QtxAction( tr( "MNU_PAN_VIEW" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 
                           0, this);
  aAction->setStatusTip( tr( "DSC_PAN_VIEW" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onPanning() ) );
  mgr->registerAction( aAction, PanId );

  // 3.2. Global Panning
  aAction = new QtxAction( tr( "MNU_GLOBALPAN_VIEW" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ),
                           0, this);
  aAction->setStatusTip( tr( "DSC_GLOBALPAN_VIEW" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onGlobalPanning() ) );
  mgr->registerAction( aAction, GlobalPanId );

  // 3.3. Create multi-action for moving operations
  QtxMultiAction* aPanAction = new QtxMultiAction( this );
  aPanAction->insertAction( mgr->action( PanId ) );
  aPanAction->insertAction( mgr->action( GlobalPanId ) );
  mgr->registerAction( aPanAction, MoveOpId );

  // 4. Curve type operations
  
  // 4.1. Points
  aAction = new QtxAction( tr( "TOT_PLOT2D_CURVES_POINTS" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_CURVES_POINTS" ) ),
                           tr( "MEN_PLOT2D_CURVES_POINTS" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_CURVES_POINTS" ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, CurvPointsId );

  // 4.2. Lines
  aAction = new QtxAction( tr( "TOT_PLOT2D_CURVES_LINES" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_CURVES_LINES" ) ),
                           tr( "MEN_PLOT2D_CURVES_LINES" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_CURVES_LINES" ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, CurvLinesId );

  // 4.3. Splines
  aAction = new QtxAction( tr( "TOT_PLOT2D_CURVES_SPLINES" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_CURVES_SPLINES" ) ),
                           tr( "MEN_PLOT2D_CURVES_SPLINES" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_CURVES_SPLINES" ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, CurvSplinesId );

  // 4.4. Create action group for curve type operations
  QActionGroup* aCurveGroup = new QActionGroup( this );
  aCurveGroup->addAction( mgr->action( CurvPointsId ) );
  aCurveGroup->addAction( mgr->action( CurvLinesId ) );
  aCurveGroup->addAction( mgr->action( CurvSplinesId ) );
  connect( aCurveGroup, SIGNAL( triggered( QAction* ) ), this, SLOT( onCurves() ) );

  // 5. Horizontal scaling mode operations

  // 5.1. Linear
  aAction = new QtxAction( tr( "TOT_PLOT2D_MODE_LINEAR_HOR" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_MODE_LINEAR_HOR" ) ),
                           tr( "MEN_PLOT2D_MODE_LINEAR_HOR" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_MODE_LINEAR_HOR" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewHorMode() ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, PModeXLinearId );
  
  // 5.2. Logarithmic
  aAction = new QtxAction( tr( "TOT_PLOT2D_MODE_LOGARITHMIC_HOR" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_MODE_LOGARITHMIC_HOR" ) ),
                           tr( "MEN_PLOT2D_MODE_LOGARITHMIC_HOR" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_MODE_LOGARITHMIC_HOR" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewHorMode() ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, PModeXLogarithmicId );

  // 5.3. Create action group for horizontal scaling mode operations
  QActionGroup* aHorGroup = new QActionGroup( this );
  aHorGroup->addAction( mgr->action( PModeXLinearId ) );
  aHorGroup->addAction( mgr->action( PModeXLogarithmicId ) );

  // 6. Vertical scaling mode operations

  // 6.1. Linear
  aAction = new QtxAction( tr( "TOT_PLOT2D_MODE_LINEAR_VER" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_MODE_LINEAR_VER" ) ),
                           tr( "MEN_PLOT2D_MODE_LINEAR_VER" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_MODE_LINEAR_VER" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewVerMode() ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, PModeYLinearId );

  // 6.2. Logarithmic
  aAction = new QtxAction( tr( "TOT_PLOT2D_MODE_LOGARITHMIC_VER" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_MODE_LOGARITHMIC_VER" ) ),
                           tr( "MEN_PLOT2D_MODE_LOGARITHMIC_VER" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_MODE_LOGARITHMIC_VER" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewVerMode() ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, PModeYLogarithmicId );

  // 6.3. Create action group for vertical scaling mode operations
  QActionGroup* aVerGroup = new QActionGroup( this );
  aVerGroup->addAction( mgr->action( PModeYLinearId ) );
  aVerGroup->addAction( mgr->action( PModeYLogarithmicId ) );

  // 7. Normalization mode operations

  // 7.1. Normalize to the global minimum by left Y axis
  aAction = new QtxAction( tr( "TOT_PLOT2D_NORMALIZE_MODE_LMIN" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_NORMALIZE_MODE_LMIN" ) ),
                           tr( "MEN_PLOT2D_NORMALIZE_MODE_LMIN" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_NORMALIZE_MODE_LMIN" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewNormLMode() ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, PModeNormLMinId );

  // 7.2. Normalize to the global maximum by right Y axis
  aAction = new QtxAction( tr( "TOT_PLOT2D_NORMALIZE_MODE_LMAX" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_NORMALIZE_MODE_LMAX" ) ),
                           tr( "MEN_PLOT2D_NORMALIZE_MODE_LMAX" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_NORMALIZE_MODE_LMAX" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewNormLMode() ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, PModeNormLMaxId );

    // 7.3. Normalize to the global minimum by right Y axis
  aAction = new QtxAction( tr( "TOT_PLOT2D_NORMALIZE_MODE_RMIN" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_NORMALIZE_MODE_RMIN" ) ),
                           tr( "MEN_PLOT2D_NORMALIZE_MODE_RMIN" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_NORMALIZE_MODE_RMIN" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewNormRMode() ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, PModeNormRMinId );

  // 7.4. Normalize to the global maximum by left Y axis
  aAction = new QtxAction( tr( "TOT_PLOT2D_NORMALIZE_MODE_RMAX" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_NORMALIZE_MODE_RMAX" ) ),
                           tr( "MEN_PLOT2D_NORMALIZE_MODE_RMAX" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_NORMALIZE_MODE_RMAX" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewNormRMode() ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, PModeNormRMaxId );

  // 8. Legend
  aAction = new QtxAction( tr( "TOT_PLOT2D_SHOW_LEGEND" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_SHOW_LEGEND" ) ),
                           tr( "MEN_PLOT2D_SHOW_LEGEND" ),
                           0, this );
  aAction->setStatusTip( tr( "PRP_PLOT2D_SHOW_LEGEND" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onLegend() ) );
  aAction->setCheckable( true );
  mgr->registerAction( aAction, LegendId );

  // 9. Settings
  aAction = new QtxAction( tr( "TOT_PLOT2D_SETTINGS" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_SETTINGS" ) ),
                           tr( "MEN_PLOT2D_SETTINGS" ),
                           0, this );

  aAction->setStatusTip( tr( "PRP_PLOT2D_SETTINGS" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), myViewFrame, SLOT( onSettings() ) );
  mgr->registerAction( aAction, CurvSettingsId );

  // 9. Analytical curves
  aAction = new QtxAction( tr( "TOT_PLOT2D_ANALYTICAL_CURVES" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_ANALYTICAL_CURVES" ) ),
                           tr( "MEN_PLOT2D_ANALYTICAL_CURVES" ),
                           0, this );

  aAction->setStatusTip( tr( "PRP_PLOT2D_ANALYTICAL_CURVES" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), myViewFrame, SLOT( onAnalyticalCurve() ) );
  mgr->registerAction( aAction, AnalyticalCurveId );

  // 10. Clone
  aAction = new QtxAction( tr( "MNU_CLONE_VIEW" ),
                           aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_CLONE_VIEW" ) ),
                           tr( "MNU_CLONE_VIEW" ),
                           0, this);
  aAction->setStatusTip( tr( "DSC_CLONE_VIEW" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SIGNAL( cloneView() ) );
  mgr->registerAction( aAction, CloneId );

  // 11. Print 
  aAction = new QtxAction( tr( "MNU_PRINT_VIEW" ),
			   aResMgr->loadPixmap( "STD", tr( "ICON_PLOT2D_PRINT" ) ),
                           tr( "MNU_PRINT_VIEW" ),
			   0, this);
  aAction->setStatusTip( tr( "DSC_PRINT_VIEW" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onPrintView() ) );
  mgr->registerAction( aAction, PrintId );

  // Set initial values
  onChangeCurveMode();
  onChangeHorMode();
  onChangeVerMode();
  onChangeNormLMode();
  onChangeNormRMode();
  onChangeLegendMode();
}

/*!
  \brief Create toolbar for the view window.
*/
void Plot2d_ViewWindow::createToolBar()
{
  QtxActionToolMgr* mgr = toolMgr();
  myToolBar = mgr->createToolBar( tr( "LBL_TOOLBAR_LABEL" ),          // title (language-dependant)
				  QString( "Plot2dViewOperations" ),  // name (language-independant)
				  false );                            // disable floatable toolbar
  mgr->append( DumpId, myToolBar );
  mgr->append( ScaleOpId, myToolBar );
  mgr->append( MoveOpId, myToolBar );
  mgr->append( toolMgr()->separator(), myToolBar );
  mgr->append( CurvPointsId, myToolBar );
  mgr->append( CurvLinesId, myToolBar );
  mgr->append( CurvSplinesId, myToolBar );
  mgr->append( toolMgr()->separator(), myToolBar );
  mgr->append( PModeNormLMaxId, myToolBar );
  mgr->append( PModeNormLMinId, myToolBar );
  mgr->append( toolMgr()->separator(), myToolBar );
  mgr->append( PModeNormRMaxId, myToolBar );
  mgr->append( PModeNormRMinId, myToolBar );
  mgr->append( toolMgr()->separator(), myToolBar );
  mgr->append( PModeXLinearId, myToolBar );
  mgr->append( PModeXLogarithmicId, myToolBar );
  mgr->append( toolMgr()->separator(), myToolBar );
  mgr->append( PModeYLinearId, myToolBar );
  mgr->append( PModeYLogarithmicId, myToolBar );
  mgr->append( toolMgr()->separator(), myToolBar );
  mgr->append( LegendId, myToolBar );
  mgr->append( CurvSettingsId, myToolBar );
  mgr->append( AnalyticalCurveId, myToolBar );
  mgr->append( CloneId, myToolBar );
  mgr->append( PrintId, myToolBar );
}

/*!
  \brief Get the visual parameters of the view window.
  \return visual parameters of this view window formatted to the string
*/
QString Plot2d_ViewWindow::getVisualParameters()
{
  return myViewFrame->getVisualParameters();
}

/*!
  \brief Restore visual parameters of the view window from the formated string
  \param parameters view window visual parameters
*/
void Plot2d_ViewWindow::setVisualParameters( const QString& parameters )
{
  myViewFrame->setVisualParameters( parameters );
}

/*!
  \brief Grab the view window to the internal image.
*/
void Plot2d_ViewWindow::RefreshDumpImage()
{
  QPixmap px = QPixmap::grabWindow( myViewFrame->winId() );
  myDumpImage = px.toImage();
}

/*!
  \brief Called when the scale mode for the horizontal axis is changed.
*/
void Plot2d_ViewWindow::onChangeHorMode()
{
  bool aHorLinear = myViewFrame->isModeHorLinear();
  bool aVerLinear = myViewFrame->isModeVerLinear();

  if ( aHorLinear )
    toolMgr()->action( PModeXLinearId )->setChecked( true );
  else
    toolMgr()->action( PModeXLogarithmicId )->setChecked( true );

  toolMgr()->action( GlobalPanId )->setEnabled( aHorLinear && aVerLinear );
}

/*!
  \brief Called when the scale mode for the vertical axis is changed.
*/
void Plot2d_ViewWindow::onChangeVerMode()
{
  bool aHorLinear = myViewFrame->isModeHorLinear();
  bool aVerLinear = myViewFrame->isModeVerLinear();

  if ( aVerLinear )
    toolMgr()->action( PModeYLinearId )->setChecked( true );
  else
    toolMgr()->action( PModeYLogarithmicId )->setChecked( true );

  toolMgr()->action( GlobalPanId )->setEnabled( aHorLinear && aVerLinear );
}

/*!
  \brief Called when the normalization mode (by left Y axis) for curves is changed.
*/
void Plot2d_ViewWindow::onChangeNormLMode()
{
  bool aNormMax = myViewFrame->isNormLMaxMode();
  bool aNormMin = myViewFrame->isNormLMinMode();

  if ( aNormMax )
    toolMgr()->action( PModeNormLMaxId )->setChecked( true );
  else
    toolMgr()->action( PModeNormLMaxId )->setChecked( false );
  if ( aNormMin )
    toolMgr()->action( PModeNormLMinId )->setChecked( true );
  else
    toolMgr()->action( PModeNormLMinId )->setChecked( false );
}

/*!
  \brief Called when the normalization mode (by left Y axis) for curves is changed.
*/
void Plot2d_ViewWindow::onChangeNormRMode()
{
  bool aNormMax = myViewFrame->isNormRMaxMode();
  bool aNormMin = myViewFrame->isNormRMinMode();

  if ( aNormMax )
    toolMgr()->action( PModeNormRMaxId )->setChecked( true );
  else
    toolMgr()->action( PModeNormRMaxId )->setChecked( false );
  if ( aNormMin )
    toolMgr()->action( PModeNormRMinId )->setChecked( true );
  else
    toolMgr()->action( PModeNormRMinId )->setChecked( false );
}

/*!
  \brief Called when the curve type is changed.
*/
void Plot2d_ViewWindow::onChangeCurveMode()
{
  switch ( myViewFrame->getCurveType() ) {
  case 0:
    toolMgr()->action( CurvPointsId )->setChecked( true );
    break;
  case 1:
    toolMgr()->action( CurvLinesId )->setChecked( true );
    break;
  case 2:
    toolMgr()->action( CurvSplinesId )->setChecked( true );
    break;
  default:
    break;
  }
}

/*!
  \brief Called when the legend mode is changed.
*/
void Plot2d_ViewWindow::onChangeLegendMode()
{
  toolMgr()->action( LegendId )->setChecked( myViewFrame->isLegendShow() );
}

/*!
  \brief Called when the "Fit all" action is activated.
*/
void Plot2d_ViewWindow::onFitAll()
{
  myViewFrame->onViewFitAll();
}

/*!
  \brief Called when the "Fit rect" action is activated.
*/
void Plot2d_ViewWindow::onFitRect()
{
  myViewFrame->onViewFitArea();
}

/*!
  \brief Called when the "Zoom" action is activated.
*/
void Plot2d_ViewWindow::onZoom()
{
  myViewFrame->onViewZoom();
}

/*!
  \brief Called when the "Panning" action is activated.
*/
void Plot2d_ViewWindow::onPanning()
{
  myViewFrame->onViewPan();
}

/*!
  \brief Called when the "Global panning" action is activated.
*/
void Plot2d_ViewWindow::onGlobalPanning()
{
  myViewFrame->onViewGlobalPan();
}

/*!
  \brief Called when horizontal axis scale mode action is activated.
*/
void Plot2d_ViewWindow::onViewHorMode()
{
  myViewFrame->setHorScaleMode( toolMgr()->action( PModeXLinearId )->isChecked() ? 0 : 1 );
}

/*!
  \brief Called when vertical axis scale mode action is activated.
*/
void Plot2d_ViewWindow::onViewVerMode()
{
  myViewFrame->setVerScaleMode( toolMgr()->action( PModeYLinearId )->isChecked() ? 0 : 1 );
}

/*!
  \brief Called when normalization mode action (by left Y axis) is activated.
*/

void Plot2d_ViewWindow::onViewNormLMode()
{
  myViewFrame->setNormLMaxMode( toolMgr()->action( PModeNormLMaxId )->isChecked() ? true : false );
  myViewFrame->setNormLMinMode( toolMgr()->action( PModeNormLMinId )->isChecked() ? true : false );
}

/*!
  \brief Called when normalization mode action (by right Y axis) is activated.
*/

void Plot2d_ViewWindow::onViewNormRMode()
{
  myViewFrame->setNormRMaxMode( toolMgr()->action( PModeNormRMaxId )->isChecked() ? true : false );
  myViewFrame->setNormRMinMode( toolMgr()->action( PModeNormRMinId )->isChecked() ? true : false );
}

/*!
  \brief Called when the "Show legend" action is activated.
*/
void Plot2d_ViewWindow::onLegend()
{
  myViewFrame->showLegend( !myViewFrame->isLegendShow() );
  onChangeLegendMode();
}

/*!
  \brief Called when the "Change curve type" action is activated.
*/
void Plot2d_ViewWindow::onCurves()
{
  if( toolMgr()->action( CurvPointsId )->isChecked() )
    myViewFrame->setCurveType( 0 );
  else if ( toolMgr()->action( CurvLinesId )->isChecked() )
    myViewFrame->setCurveType( 1 );
  else if ( toolMgr()->action( CurvSplinesId )->isChecked() )
    myViewFrame->setCurveType( 2 );
}
 
/*!
  \brief Called when the "Dump view" action is activated.
*/
void Plot2d_ViewWindow::onDumpView()
{
  qApp->postEvent( myViewFrame, new QPaintEvent( QRect( 0, 0, myViewFrame->width(), myViewFrame->height() ) ) );
  SUIT_ViewWindow::onDumpView();
}

/*!
  \brief Dump the contents of the view window to the image.
  \return image, containing all scene rendered in the window
*/
QImage Plot2d_ViewWindow::dumpView()
{
  if ( getToolBar()->underMouse() || myDumpImage.isNull() ) {
    QPixmap px = QPixmap::grabWindow( myViewFrame->winId() );
    return px.toImage();
  }
  
  return myDumpImage;
}

/*!
  \brief Dump scene rendered in the view window to the file.
  \param img image
  \param fileName name of file
  \param format image format ("BMP" [default], "JPEG", "JPG", "PNG")
*/
bool Plot2d_ViewWindow::dumpViewToFormat( const QImage&  img,
                                          const QString& fileName, 
                                          const QString& format )
{
  bool res = myViewFrame ? myViewFrame->print( fileName, format ) : false;
  if( !res )
    res = SUIT_ViewWindow::dumpViewToFormat( img, fileName, format );

  return res;
}

/*!
  \brief Get supported image files wildcards.
  \return image files wildcards (list of wildcards, separated by ";;")
*/
QString Plot2d_ViewWindow::filter() const
{
  QStringList filters = SUIT_ViewWindow::filter().split( ";;", QString::SkipEmptyParts );
  filters << tr( "PDF_FILES" );
  filters << tr( "POSTSCRIPT_FILES" );
  filters << tr( "ENCAPSULATED_POSTSCRIPT_FILES" );
  return filters.join( ";;" );
}

/*!
  \brief Called when the "Print view" action is activated.
*/
void Plot2d_ViewWindow::onPrintView()
{
  if ( !myViewFrame )
    return;

#if !defined(WIN32) && !defined(QT_NO_CUPS)
#if QT_VERSION < 0x040303
  if ( !Qtx::hasAnyPrinters() ) {
    SUIT_MessageBox::warning( this, tr( "WRN_WARNING" ),
                              tr( "WRN_NO_PRINTERS" ) );
    return;
  }
#endif
#endif

  // stored settings for further starts
  static QString aPrinterName;
  static int aColorMode = -1;
  static int anOrientation = -1;

  QPrinter aPrinter;

  // restore settinds from previous launching

  // printer name
  if ( !aPrinterName.isEmpty() )
    aPrinter.setPrinterName( aPrinterName );
  else 
  {
    // Nothing to do for the first printing. aPrinter contains default printer name by default
  }

  // color mode
  if ( aColorMode >= 0 )
    aPrinter.setColorMode( (QPrinter::ColorMode)aColorMode );
  else 
  {
    // Black-and-wight printers are often used
    aPrinter.setColorMode( QPrinter::GrayScale );
  }

  if ( anOrientation >= 0 )
    aPrinter.setOrientation( (QPrinter::Orientation)anOrientation );
  else
    aPrinter.setOrientation( QPrinter::Landscape );

  QPrintDialog printDlg( &aPrinter, this );
  printDlg.setPrintRange( QAbstractPrintDialog::AllPages );
  if ( printDlg.exec() != QDialog::Accepted ) 
    return;

  // store printer settings for further starts
  aPrinterName = aPrinter.printerName();
  aColorMode = aPrinter.colorMode();
  anOrientation = aPrinter.orientation();
  
  int W, H;
  QPainter aPainter;

  bool needColorCorrection = aPrinter.colorMode() == QPrinter::GrayScale;

  // work arround for printing on real printer
  if ( aPrinter.outputFileName().isEmpty() && aPrinter.orientation() == QPrinter::Landscape )
  {
    aPrinter.setFullPage( false );
    // set paper orientation and rotate painter
    aPrinter.setOrientation( QPrinter::Portrait );

    W = aPrinter.height();
    H = aPrinter.width();

    aPainter.begin( &aPrinter );
    aPainter.translate( QPoint( H, 0 ) );
    aPainter.rotate( 90 );
  }
  else 
  {
    aPrinter.setFullPage( false );
    aPainter.begin( &aPrinter );
    W = aPrinter.width();
    H = aPrinter.height();
  }

  QMap< QwtPlotCurve*, QPen > aCurvToPen;
  QMap< QwtPlotCurve*, QwtSymbol* > aCurvToSymbol;

  if ( needColorCorrection )
  {
    // Iterate through, store temporary their parameters and assign 
    // parameters proper for printing

    CurveDict aCurveDict = myViewFrame->getCurves();
    CurveDict::iterator it;
    for ( it = aCurveDict.begin(); it != aCurveDict.end(); it++ ) 
    {
      QwtPlotCurve* aCurve = it.key();
      if ( !aCurve )
        continue;

      // pen
      QPen aPen = aCurve->pen();
      aCurvToPen[ aCurve ] = aPen;

      aPen.setColor( QColor( 0, 0, 0 ) );
      aPen.setWidthF( 1.5 );

      aCurve->setPen( aPen );

      // symbol
      QwtSymbol* aSymbol = const_cast<QwtSymbol*>( aCurve->symbol() );
      aCurvToSymbol[ aCurve ] = aSymbol;
      aPen = aSymbol->pen();
      aPen.setColor( QColor( 0, 0, 0 ) );
      aPen.setWidthF( 1.5 );
      aSymbol->setPen( aPen );

      aCurve->setSymbol( aSymbol );
    }
  }

  myViewFrame->printPlot( &aPainter, QRect( 0, 0, W, H ) );
  aPainter.end();

  // restore old pens and symbols
  if ( needColorCorrection && !aCurvToPen.isEmpty() )
  {
    CurveDict aCurveDict = myViewFrame->getCurves();
    CurveDict::iterator it;
    for ( it = aCurveDict.begin(); it != aCurveDict.end(); it++ ) 
    {
      QwtPlotCurve* aCurve = it.key();
      if ( !aCurve || 
           !aCurvToPen.contains( aCurve ) ||
           !aCurvToSymbol.contains( aCurve ) )
        continue;

      aCurve->setPen( aCurvToPen[ aCurve ] );
      aCurve->setSymbol( aCurvToSymbol[ aCurve ] );
    }
  }
}

/*!
  \fn void Plot2d_ViewWindow::cloneView();
  \brief Emitted when the "Clone View" action is activated.
*/
