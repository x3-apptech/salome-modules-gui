// Plot2d_ViewWindow.cxx: implementation of the Plot2d_ViewWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "Plot2d_ViewWindow.h"

#include "SUIT_ViewManager.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_Session.h"
#include "SUIT_ToolButton.h"

#include <qstatusbar.h>
#include <qlayout.h>
#include <qapplication.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Plot2d_ViewWindow::Plot2d_ViewWindow(SUIT_Desktop* theDesktop, Plot2d_Viewer* theModel)
: SUIT_ViewWindow(theDesktop)
{
  myModel = theModel;

  myViewFrame = new Plot2d_ViewFrame(this, "plotView");
  setCentralWidget(myViewFrame);

  myToolBar = new QToolBar(this);
  myToolBar->setCloseMode(QDockWindow::Undocked);
  myToolBar->setLabel(tr("LBL_TOOLBAR_LABEL"));
  createActions();
  createToolBar();

  connect(myViewFrame, SIGNAL(vpModeHorChanged()), this, SLOT(onChangeHorMode()));
  connect(myViewFrame, SIGNAL(vpModeVerChanged()), this, SLOT(onChangeVerMode()));
  connect(myViewFrame, SIGNAL(vpCurveChanged()),   this, SLOT(onChangeCurveMode()));
  connect(myViewFrame, SIGNAL(contextMenuRequested( QContextMenuEvent * )),
          this,        SIGNAL(contextMenuRequested( QContextMenuEvent * )) );

}

Plot2d_ViewWindow::~Plot2d_ViewWindow()
{
}

//****************************************************************
void Plot2d_ViewWindow::putInfo(QString theMsg)
{
  QStatusBar*	aStatusBar = myDesktop->statusBar();
  aStatusBar->message(theMsg, 3000);
}

//****************************************************************
void Plot2d_ViewWindow::contextMenuPopup( QPopupMenu* thePopup )
{
  // scaling
  QPopupMenu* scalingPopup = new QPopupMenu( thePopup );
  myActionsMap[ PModeXLinearId ]->addTo( scalingPopup );
  myActionsMap[ PModeXLogarithmicId ]->addTo( scalingPopup );
  onChangeHorMode();
  scalingPopup->insertSeparator();
  myActionsMap[ PModeYLinearId ]->addTo( scalingPopup );
  myActionsMap[ PModeYLogarithmicId ]->addTo( scalingPopup );
  thePopup->insertItem( tr( "SCALING_POPUP" ), scalingPopup );
  onChangeVerMode();

  thePopup->insertItem(tr("TOT_PLOT2D_FITDATA"), myViewFrame, SLOT(onFitData()));
  // curve type
  QPopupMenu* curTypePopup = new QPopupMenu( thePopup );
  myActionsMap[ CurvPointsId ]->addTo( curTypePopup );
  myActionsMap[ CurvLinesId ]->addTo( curTypePopup );
  myActionsMap[ CurvSplinesId ]->addTo( curTypePopup );
  thePopup->insertItem( tr( "CURVE_TYPE_POPUP" ), curTypePopup );

  // legend
  myActionsMap[ LegendId ]->addTo(thePopup);
  // settings
  myActionsMap[ CurvSettingsId ]->addTo(thePopup);
}

//****************************************************************
bool Plot2d_ViewWindow::eventFilter(QObject* watched, QEvent* e)
{
  if (watched == myViewFrame) {
    int aType = e->type();
    switch(aType) {
    case QEvent::MouseButtonPress:
      emit mousePressed(this, (QMouseEvent*) e);
      return true;

    case QEvent::MouseButtonRelease:
      emit mouseReleased(this, (QMouseEvent*) e);
      return true;

    case QEvent::MouseMove:
      emit mouseMoving(this, (QMouseEvent*) e);
      return true;

    default:
      break;
    }
  }
  return SUIT_ViewWindow::eventFilter(watched, e);
}

//****************************************************************
void Plot2d_ViewWindow::createActions()
{
  if ( !myActionsMap.isEmpty() )
    return;

  QtxAction* aAction;
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  // Dump view
  aAction = new QtxAction(tr("MNU_DUMP_VIEW"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_DUMP" ) ),
                           tr( "MNU_DUMP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onDumpView()));
  myActionsMap[ DumpId ] = aAction;

  // FitAll
  aAction = new QtxAction(tr("MNU_FITALL"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_FIT_ALL" ) ),
                           tr( "MNU_FITALL" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITALL"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onFitAll()));
  myActionsMap[ FitAllId ] = aAction;

  // FitRect
  aAction = new QtxAction(tr("MNU_FITRECT"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_FIT_AREA" ) ),
                           tr( "MNU_FITRECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITRECT"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onFitRect()));
  myActionsMap[ FitRectId ] = aAction;

  // Zoom
  aAction = new QtxAction(tr("MNU_ZOOM_VIEW"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onZoom()));
  myActionsMap[ ZoomId ] = aAction;

  // Panning
  aAction = new QtxAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onPanning()));
  myActionsMap[ PanId ] = aAction;

  // Global Panning
  aAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onGlobalPanning()));
  myActionsMap[ GlobalPanId ] = aAction;

  // Curve type - points
  aAction = new QtxAction(tr("TOT_PLOT2D_CURVES_POINTS"),
                aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_CURVES_POINTS")),
                tr("MEN_PLOT2D_CURVES_POINTS"), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_CURVES_POINTS"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onCurves()));
  aAction->setToggleAction(true);
  myActionsMap[ CurvPointsId ] = aAction;

  // Curve type - lines
  aAction = new QtxAction(tr("TOT_PLOT2D_CURVES_LINES"),
               aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_CURVES_LINES")),
               tr("MEN_PLOT2D_CURVES_LINES"), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_CURVES_LINES"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onCurves()));
  aAction->setToggleAction(true);
  myActionsMap[ CurvLinesId ] = aAction;

  // Curve type - splines
  aAction = new QtxAction(tr("TOT_PLOT2D_CURVES_SPLINES"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_CURVES_SPLINES")),
                 tr("MEN_PLOT2D_CURVES_SPLINES"), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_CURVES_SPLINES"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onCurves()));
  aAction->setToggleAction(true);
  myActionsMap[ CurvSplinesId ] = aAction;

  // Mode for X (linear or logarithmic)
  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LINEAR_HOR"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LINEAR_HOR")),
                 tr("MEN_PLOT2D_MODE_LINEAR_HOR"), 0, this);
  aAction->setStatusTip (tr("PRP_PLOT2D_MODE_LINEAR_HOR"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewHorMode()));
  myActionsMap[ HorId ] = aAction;

  // Mode for Y (linear or logarithmic)
  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LINEAR_VER"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LINEAR_VER")),
                 tr("MEN_PLOT2D_MODE_LINEAR_VER" ), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_MODE_LINEAR_VER"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewVerMode()));
  myActionsMap[ VerId ] = aAction;

  // Legend
  aAction = new QtxAction(tr("TOT_PLOT2D_SHOW_LEGEND"),
                aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_SHOW_LEGEND")),
                tr("MEN_PLOT2D_SHOW_LEGEND"), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_SHOW_LEGEND"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onLegend()));
  aAction->setToggleAction(true);
  myActionsMap[ LegendId ] = aAction;

  // Settings
  aAction = new QtxAction(tr( "TOT_PLOT2D_SETTINGS"),
                aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_SETTINGS")),
                tr("MEN_PLOT2D_SETTINGS"), 0, this);
  aAction->setStatusTip(tr( "PRP_PLOT2D_SETTINGS"));
  connect(aAction, SIGNAL(activated()), myViewFrame, SLOT(onSettings()));
  myActionsMap[ CurvSettingsId ] = aAction;

  // Clone
  aAction = new QtxAction(tr("MNU_CLONE_VIEW"), aResMgr->loadPixmap( "Plot2d", tr( "ICON_PLOT2D_CLONE_VIEW" ) ),
                           tr( "MNU_CLONE_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_CLONE_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SIGNAL(cloneView()));
  myActionsMap[ CloneId ] = aAction;

  /* Popup Actions */
  /* Linear/logarithmic mode */
  // Horizontal axis
  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LINEAR_HOR"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LINEAR_HOR")),
                 tr("MEN_PLOT2D_MODE_LINEAR_HOR"), 0, this);
  aAction->setStatusTip (tr("PRP_PLOT2D_MODE_LINEAR_HOR"));
  aAction->setToggleAction(true);
  myActionsMap[PModeXLinearId] = aAction;
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewHorMode()));

  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LOGARITHMIC_HOR"),
              aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LOGARITHMIC_HOR")),
              tr("MEN_PLOT2D_MODE_LOGARITHMIC_HOR"), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_MODE_LOGARITHMIC_HOR"));
  aAction->setToggleAction(true);
  myActionsMap[PModeXLogarithmicId] = aAction;
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewHorMode()));

  // Vertical axis
  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LINEAR_VER"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LINEAR_VER")),
                 tr("MEN_PLOT2D_MODE_LINEAR_VER" ), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_MODE_LINEAR_VER"));
  aAction->setToggleAction(true);
  myActionsMap[PModeYLinearId] = aAction;
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewVerMode()));

  aAction = new QtxAction(tr("TOT_PLOT2D_MODE_LOGARITHMIC_VER"),
                 aResMgr->loadPixmap("Plot2d", tr("ICON_PLOT2D_MODE_LOGARITHMIC_VER")),
                 tr("MEN_PLOT2D_MODE_LOGARITHMIC_VER" ), 0, this);
  aAction->setStatusTip(tr("PRP_PLOT2D_MODE_LOGARITHMIC_VER"));
  aAction->setToggleAction(true);
  myActionsMap[PModeYLogarithmicId] = aAction;
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewVerMode()));

}

//****************************************************************
void Plot2d_ViewWindow::createToolBar()
{
  myActionsMap[DumpId]->addTo(myToolBar);

  SUIT_ToolButton* aScaleBtn = new SUIT_ToolButton(myToolBar);
  aScaleBtn->AddAction(myActionsMap[FitAllId]);
  aScaleBtn->AddAction(myActionsMap[FitRectId]);
  aScaleBtn->AddAction(myActionsMap[ZoomId]);

  SUIT_ToolButton* aPanBtn = new SUIT_ToolButton(myToolBar);
  aPanBtn->AddAction(myActionsMap[PanId]);
  aPanBtn->AddAction(myActionsMap[GlobalPanId]);

  myCurveBtn = new SUIT_ToolButton(myToolBar);
  myCurveBtn->AddAction(myActionsMap[CurvPointsId]);
  myCurveBtn->AddAction(myActionsMap[CurvLinesId]);
  myCurveBtn->AddAction(myActionsMap[CurvSplinesId]);
  myActionsMap[CurvLinesId]->setOn(true);
  onChangeCurveMode();

  myActionsMap[HorId]->addTo(myToolBar);
  onChangeHorMode();
  myActionsMap[VerId]->addTo(myToolBar);
  onChangeVerMode();

  myActionsMap[LegendId]->addTo(myToolBar);
  myActionsMap[CurvSettingsId]->addTo(myToolBar);
  myActionsMap[CloneId]->addTo(myToolBar);
  onChangeLegendMode();
}

//****************************************************************
void Plot2d_ViewWindow::onChangeHorMode()
{
  bool aLinear = myViewFrame->isModeHorLinear();
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  myActionsMap[PModeXLinearId]->setOn( aLinear );
  myActionsMap[PModeXLogarithmicId]->setOn( !aLinear );

  QPixmap pix = aResMgr->loadPixmap( "Plot2d", tr( aLinear ? "ICON_PLOT2D_MODE_LOGARITHMIC_HOR" :
                                                             "ICON_PLOT2D_MODE_LINEAR_HOR" ) );
  myActionsMap[HorId]->setIconSet( pix );
  myActionsMap[HorId]->setToolTip( tr( aLinear ? "TOT_PLOT2D_MODE_LOGARITHMIC_HOR" :
                                                 "TOT_PLOT2D_MODE_LINEAR_HOR" ) );
  myActionsMap[HorId]->setStatusTip( tr( aLinear ? "PRP_PLOT2D_MODE_LOGARITHMIC_HOR" :
                                                   "PRP_PLOT2D_MODE_LINEAR_HOR" ) );

  myActionsMap[GlobalPanId]->setEnabled( myViewFrame->isModeVerLinear() && myViewFrame->isModeHorLinear() );
}

//****************************************************************
void Plot2d_ViewWindow::onChangeVerMode()
{
  bool aLinear = myViewFrame->isModeVerLinear();
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  myActionsMap[PModeYLinearId]->setOn( aLinear );
  myActionsMap[PModeYLogarithmicId]->setOn( !aLinear );

  QPixmap pix = aResMgr->loadPixmap( "Plot2d", tr( aLinear ? "ICON_PLOT2D_MODE_LOGARITHMIC_VER" :
                                                             "ICON_PLOT2D_MODE_LINEAR_VER" ) );
  myActionsMap[VerId]->setIconSet( pix );
  myActionsMap[VerId]->setToolTip( tr( aLinear ? "TOT_PLOT2D_MODE_LOGARITHMIC_VER" :
                                                 "TOT_PLOT2D_MODE_LINEAR_VER" ) );
  myActionsMap[VerId]->setStatusTip( tr( aLinear ? "PRP_PLOT2D_MODE_LOGARITHMIC_VER" :
                                                   "PRP_PLOT2D_MODE_LINEAR_VER" ) );

  myActionsMap[GlobalPanId]->setEnabled( myViewFrame->isModeVerLinear() && myViewFrame->isModeHorLinear() );
}

//****************************************************************
void Plot2d_ViewWindow::onChangeCurveMode()
{
  myCurveBtn->SetItem(myViewFrame->getCurveType());
}

//****************************************************************
void Plot2d_ViewWindow::onChangeLegendMode()
{
  myActionsMap[ LegendId ]->setOn(myViewFrame->isLegendShow());
}

//****************************************************************
void Plot2d_ViewWindow::onFitAll()
{
  myViewFrame->onViewFitAll();
}

//****************************************************************
void Plot2d_ViewWindow::onFitRect()
{
  myViewFrame->onViewFitArea();
}

//****************************************************************
void Plot2d_ViewWindow::onZoom()
{
  myViewFrame->onViewZoom();
}

//****************************************************************
void Plot2d_ViewWindow::onPanning()
{
  myViewFrame->onViewPan();
}

//****************************************************************
void Plot2d_ViewWindow::onGlobalPanning()
{
  myViewFrame->onViewGlobalPan();
}

//****************************************************************
void Plot2d_ViewWindow::onViewHorMode()
{
  if (myViewFrame->isModeHorLinear())
    myViewFrame->setHorScaleMode(1);
  else
    myViewFrame->setHorScaleMode(0);
}

//****************************************************************
void Plot2d_ViewWindow::onViewVerMode()
{
  if (myViewFrame->isModeVerLinear())
    myViewFrame->setVerScaleMode(1);
  else
    myViewFrame->setVerScaleMode(0);
}

//****************************************************************
void Plot2d_ViewWindow::onLegend()
{
  myViewFrame->showLegend(!myViewFrame->isLegendShow());
  onChangeLegendMode();
}

//****************************************************************
void Plot2d_ViewWindow::onCurves()
{
  QtxAction* aSender = (QtxAction*) sender();
  if(aSender == myActionsMap[CurvPointsId]) {
    myActionsMap[CurvPointsId]->setOn(true);
    myActionsMap[CurvLinesId]->setOn(false);
    myActionsMap[CurvSplinesId]->setOn(false);
    myViewFrame->setCurveType(0);
  }
  else if(aSender == myActionsMap[CurvLinesId]) {
    myActionsMap[CurvLinesId]->setOn(true);
    myActionsMap[CurvPointsId]->setOn(false);
    myActionsMap[CurvSplinesId]->setOn(false);
    myViewFrame->setCurveType(1);
  }
  else if(aSender == myActionsMap[CurvSplinesId]) {
    myActionsMap[CurvSplinesId]->setOn( true );
    myActionsMap[CurvPointsId]->setOn( false );
    myActionsMap[CurvLinesId]->setOn( false );
    myViewFrame->setCurveType(2);
  }
}

//****************************************************************
void Plot2d_ViewWindow::onDumpView()
{
  qApp->postEvent( myViewFrame, new QPaintEvent( QRect( 0, 0, myViewFrame->width(), myViewFrame->height() ), TRUE ) );
  SUIT_ViewWindow::onDumpView();
}

//****************************************************************
QImage Plot2d_ViewWindow::dumpView()
{
  QPixmap px = QPixmap::grabWindow( myViewFrame->winId() );
  return px.convertToImage();
}
