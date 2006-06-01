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
// OCCViewer_ViewWindow.cxx: implementation of the OCCViewer_ViewWindow class.


#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_CreateRestoreViewDlg.h"
#include "OCCViewer_ClippingDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SUIT_ToolButton.h"

#include "SUIT_Tools.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_MessageBox.h"

#include <qptrlist.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qdatetime.h>
#include <qimage.h>

#include <V3d_Plane.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>

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

const char* imageRotateCursor[] = { 
"32 32 3 1",
". c None",
"a c #000000",
"# c #ffffff",
"................................",
"................................",
"................................",
"................................",
"........#.......................",
".......#.a......................",
"......#######...................",
".......#aaaaa#####..............",
"........#..##.a#aa##........##..",
".........a#.aa..#..a#.....##.aa.",
".........#.a.....#...#..##.aa...",
".........#a.......#..###.aa.....",
"........#.a.......#a..#aa.......",
"........#a.........#..#a........",
"........#a.........#a.#a........",
"........#a.........#a.#a........",
"........#a.........#a.#a........",
".........#.........#a#.a........",
"........##a........#a#a.........",
"......##.a#.......#.#.a.........",
"....##.aa..##.....##.a..........",
"..##.aa.....a#####.aa...........",
"...aa.........aaa#a.............",
"................#.a.............",
"...............#.a..............",
"..............#.a...............",
"...............a................",
"................................",
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


/*!
  Constructor
  \param theDesktop - main window of application
  \param theModel - OCC 3D viewer
*/
OCCViewer_ViewWindow::OCCViewer_ViewWindow(SUIT_Desktop* theDesktop, OCCViewer_Viewer* theModel)
: SUIT_ViewWindow(theDesktop)
{
  myModel = theModel;
  myRestoreFlag = 0;
  myEnableDrawMode = false;
  updateEnabledDrawMode();
  myClippingDlg = 0;
}

/*!
  Initialization of view window
*/
void OCCViewer_ViewWindow::initLayout()
{
  myViewPort = new OCCViewer_ViewPort3d( this, myModel->getViewer3d(), V3d_ORTHOGRAPHIC );
  myViewPort->setBackgroundColor(black);
  myViewPort->installEventFilter(this);
	setCentralWidget(myViewPort);
  myOperation = NOTHING;

  setTransformRequested ( NOTHING );
  setTransformInProcess ( false );

  myToolBar = new QToolBar(this);
  myToolBar->setCloseMode(QDockWindow::Undocked);
  myToolBar->setLabel(tr("LBL_TOOLBAR_LABEL"));

  createActions();
  createToolBar();
}

/*!
  \return type of operation by states of mouse and keyboard buttons
  \param theEvent - mouse event
*/
OCCViewer_ViewWindow::OperationType OCCViewer_ViewWindow::getButtonState(QMouseEvent* theEvent)
{
  OperationType aOp = NOTHING;
  if( (theEvent->state() == SUIT_ViewModel::myStateMap[SUIT_ViewModel::ZOOM]) &&
      (theEvent->button() == SUIT_ViewModel::myButtonMap[SUIT_ViewModel::ZOOM]) )
    aOp = ZOOMVIEW;
  else if( (theEvent->state() == SUIT_ViewModel::myStateMap[SUIT_ViewModel::PAN]) && 
           (theEvent->button() == SUIT_ViewModel::myButtonMap[SUIT_ViewModel::PAN]) )
    aOp = PANVIEW;
  else if( (theEvent->state()  == SUIT_ViewModel::myStateMap[SUIT_ViewModel::ROTATE]) &&
           (theEvent->button() == SUIT_ViewModel::myButtonMap[SUIT_ViewModel::ROTATE]) )
    aOp = ROTATE;

  return aOp;
}

/*!
  Custom event handler
*/
bool OCCViewer_ViewWindow::eventFilter(QObject* watched, QEvent* e)
{
  if ( watched == myViewPort ) {
    int aType = e->type();
    switch(aType) {
    case QEvent::MouseButtonPress:
      vpMousePressEvent((QMouseEvent*) e);
      return true;

    case QEvent::MouseButtonRelease:
      vpMouseReleaseEvent((QMouseEvent*) e);
      return true;

    case QEvent::MouseMove:
      vpMouseMoveEvent((QMouseEvent*) e);
      return true;

    case QEvent::MouseButtonDblClick:
      emit mouseDoubleClicked(this, (QMouseEvent*)e);
      return true;

    case QEvent::Wheel:
      {
        QWheelEvent* aEvent = (QWheelEvent*) e;
        double aDelta = aEvent->delta();
        double aScale = (aDelta < 0) ? 100./(-aDelta) : aDelta/100.; 
        myViewPort->getView()->SetZoom(aScale);
      }
      return true;

    case QEvent::ContextMenu:
      {
        QContextMenuEvent * aEvent = (QContextMenuEvent*)e;
        if ( aEvent->reason() != QContextMenuEvent::Mouse )
          emit contextMenuRequested( aEvent );
      }
      return true;

    default:
      break;
    }
  }
  return SUIT_ViewWindow::eventFilter(watched, e);
}

/*!
  Updates state of enable draw mode state
*/
void OCCViewer_ViewWindow::updateEnabledDrawMode()
{
  if ( myModel )
    myEnableDrawMode = myModel->isSelectionEnabled() && myModel->isMultiSelectionEnabled();
}

/*!
  Handler of mouse press event
*/
void OCCViewer_ViewWindow::vpMousePressEvent(QMouseEvent* theEvent)
{
  myStartX = theEvent->x();
  myStartY = theEvent->y();
  switch ( myOperation ) {
  case WINDOWFIT:
    if ( theEvent->button() == Qt::LeftButton )
      emit vpTransformationStarted ( WINDOWFIT );
    break;    
   
  case PANGLOBAL:
    if ( theEvent->button() == Qt::LeftButton )
      emit vpTransformationStarted ( PANGLOBAL );
    break;    
    
  case ZOOMVIEW:
    if ( theEvent->button() == Qt::LeftButton )
      emit vpTransformationStarted ( ZOOMVIEW );
    break;
    
  case PANVIEW:
    if ( theEvent->button() == Qt::LeftButton )
      emit vpTransformationStarted ( PANVIEW );
    break;

  case ROTATE:
    if ( theEvent->button() == Qt::LeftButton ) {
	    myViewPort->startRotation(myStartX, myStartY);
	    emit vpTransformationStarted ( ROTATE );
	  }
    break;
      
  default:
  /*  Try to activate a transformation */
    switch ( getButtonState(theEvent) ) {
    case ZOOMVIEW:
	    activateZoom();
      break;
    case PANVIEW:
	    activatePanning();
      break;
    case ROTATE:
	    activateRotation();
	    myViewPort->startRotation(myStartX, myStartY);
      break;
    default:
      emit mousePressed(this, theEvent);
      break;
    }
    /* notify that we start a transformation */
    if ( transformRequested() ) 
	    emit vpTransformationStarted ( myOperation );
  }
  if ( transformRequested() ) 
    setTransformInProcess( true );		 
}


/*!
  Starts zoom operation, sets corresponding cursor
*/
void OCCViewer_ViewWindow::activateZoom()
{
  if ( !transformRequested() && !myCursorIsHand )
    myCursor = cursor();	        /* save old cursor */
  
  if ( myOperation != ZOOMVIEW ) {
    QPixmap zoomPixmap (imageZoomCursor);
    QCursor zoomCursor (zoomPixmap);
    setTransformRequested ( ZOOMVIEW );		
    setCursor( zoomCursor );
  }
}


/*!
  Starts panning operation, sets corresponding cursor
*/
void OCCViewer_ViewWindow::activatePanning()
{
  if ( !transformRequested() && !myCursorIsHand )
    myCursor = cursor();	        // save old cursor 
  
  if ( myOperation != PANVIEW ) {
    QCursor panCursor (Qt::SizeAllCursor);
    setTransformRequested ( PANVIEW );
    setCursor( panCursor );
  }
}

/*!
  Starts rotation operation, sets corresponding cursor
*/
void OCCViewer_ViewWindow::activateRotation()
{
  if ( !transformRequested() && !myCursorIsHand )
    myCursor = cursor();	        // save old cursor 
  
  if ( myOperation != ROTATE ) {
    QPixmap rotatePixmap (imageRotateCursor);
    QCursor rotCursor (rotatePixmap);
    setTransformRequested ( ROTATE );
    setCursor( rotCursor );	
  }
}

/*!
  Starts global panning operation, sets corresponding cursor
*/
void OCCViewer_ViewWindow::activateGlobalPanning()
{
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
    QPixmap globalPanPixmap (imageCrossCursor);
    QCursor glPanCursor (globalPanPixmap);
    myCurScale = aView3d->Scale();
    aView3d->FitAll(0.01, false);
    myCursor = cursor();	        // save old cursor 
    myViewPort->fitAll(); // fits view before selecting a new scene center 
    setTransformRequested( PANGLOBAL );
    setCursor( glPanCursor );
  }
}

/*!
  Starts fit operation, sets corresponding cursor
*/
void OCCViewer_ViewWindow::activateWindowFit()
{
  if ( !transformRequested() && !myCursorIsHand )
    myCursor = cursor();	        /* save old cursor */

  if ( myOperation != WINDOWFIT ) {
    QCursor handCursor (Qt::PointingHandCursor);
    setTransformRequested ( WINDOWFIT );		
    setCursor ( handCursor );
    myCursorIsHand = true;
  }
}

/*!
  Stores which viewer operation is requesting
*/
void OCCViewer_ViewWindow::setTransformRequested ( OperationType op )
{    
  myOperation = op;
  myViewPort->setMouseTracking( myOperation == NOTHING );  
}


/*!
  Handler of mouse move event
*/
void OCCViewer_ViewWindow::vpMouseMoveEvent(QMouseEvent* theEvent)
{
  myCurrX = theEvent->x();
  myCurrY = theEvent->y();
  switch (myOperation) {
  case ROTATE:
    myViewPort->rotate(myCurrX, myCurrY);
    break;
    
  case ZOOMVIEW:
    myViewPort->zoom(myStartX, myStartY, myCurrX, myCurrY);
    myStartX = myCurrX;
    myStartY = myCurrY;
    break;
    
  case PANVIEW:
    myViewPort->pan(myCurrX - myStartX, myStartY - myCurrY);
    myStartX = myCurrX;
    myStartY = myCurrY;
    break;
    
/*    case WINDOWFIT:
    myDrawRect = true;
    repaint();
    break;
*/      
  case PANGLOBAL:
    break;
    
  default:
    int aState = theEvent->state();
    //int aButton = theEvent->button();
    if ( aState == Qt::LeftButton ||
	aState == ( Qt::LeftButton | Qt::ShiftButton) )	{
      myDrawRect = myEnableDrawMode;
      if ( myDrawRect ) {
        drawRect();
	if ( !myCursorIsHand )	{   // we are going to sketch a rectangle
          QCursor handCursor (Qt::PointingHandCursor);
	  myCursorIsHand = true;		
	  myCursor = cursor();
	  setCursor( handCursor );
	}
      }
    } 
    else {
      emit mouseMoving( this, theEvent ); 
    }		
  }
}

/*!
  Handler of mouse release event
*/
void OCCViewer_ViewWindow::vpMouseReleaseEvent(QMouseEvent* theEvent)
{
  switch ( myOperation ) {
  case NOTHING:
    {
      emit mouseReleased(this, theEvent);
      if(theEvent->button() == RightButton)
      {
        QContextMenuEvent aEvent( QContextMenuEvent::Mouse,
                                  theEvent->pos(), theEvent->globalPos(),
                                  theEvent->state() );
        emit contextMenuRequested( &aEvent );
      }
    }
    break;
  case ROTATE:
    myViewPort->endRotation();
    resetState();
    break;
    
  case PANVIEW:
  case ZOOMVIEW:
    resetState();
    break;
    
  case PANGLOBAL:
    if ( theEvent->button() == Qt::LeftButton ) {
	    myViewPort->setCenter( theEvent->x(), theEvent->y() );
      myViewPort->getView()->SetScale(myCurScale);
	    resetState();
	  }
    break;
      
  case WINDOWFIT:
    if ( theEvent->state() == Qt::LeftButton ) {
	    myCurrX = theEvent->x();
	    myCurrY = theEvent->y();
	    QRect rect = SUIT_Tools::makeRect(myStartX, myStartY, myCurrX, myCurrY);
	    if ( !rect.isEmpty() ) myViewPort->fitRect(rect);
	    resetState();
	  }
    break;
  }
  
  // NOTE: viewer 3D detects a rectangle of selection using this event
  // so we must emit it BEFORE resetting the selection rectangle
  
  if ( theEvent->button() == Qt::LeftButton && myDrawRect ) {
    myDrawRect = false;
    drawRect();
    resetState(); 
    myViewPort->update();
  }
}

/*!
  Sets the viewport to its initial state
  ( no transformations in process etc. )
*/
void OCCViewer_ViewWindow::resetState()
{
  myDrawRect = false;
  
  /* make rectangle empty (left > right) */
  myRect.setLeft(2);
  myRect.setRight(0);
  
  if ( transformRequested() || myCursorIsHand ) 
    setCursor( myCursor );
  myCursorIsHand = false;
  
  if ( transformRequested() ) 
    emit vpTransformationFinished (myOperation);
  
  setTransformInProcess( false );		
  setTransformRequested( NOTHING );	
}


/*!
  Draws rectangle by starting and current points
*/
void OCCViewer_ViewWindow::drawRect()
{
  QPainter aPainter(myViewPort);
  aPainter.setRasterOp(Qt::XorROP);
  aPainter.setPen(Qt::white);
  QRect aRect = SUIT_Tools::makeRect(myStartX, myStartY, myCurrX, myCurrY);
  if ( !myRect.isEmpty() )
	  aPainter.drawRect( myRect );
  aPainter.drawRect(aRect);
  myRect = aRect;
}

/*!
  Creates actions of OCC view window
*/
void OCCViewer_ViewWindow::createActions()
{
  if (!myActionsMap.isEmpty()) return;
  
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  
  QtxAction* aAction;

  // Dump view
  aAction = new QtxAction(tr("MNU_DUMP_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_DUMP" ) ),
                           tr( "MNU_DUMP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onDumpView()));
	myActionsMap[ DumpId ] = aAction;

  // FitAll
  aAction = new QtxAction(tr("MNU_FITALL"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_FITALL" ) ),
                           tr( "MNU_FITALL" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITALL"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onFitAll()));
	myActionsMap[ FitAllId ] = aAction;

  // FitRect
  aAction = new QtxAction(tr("MNU_FITRECT"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_FITAREA" ) ),
                           tr( "MNU_FITRECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITRECT"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateWindowFit()));
	myActionsMap[ FitRectId ] = aAction;

  // Zoom
  aAction = new QtxAction(tr("MNU_ZOOM_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateZoom()));
	myActionsMap[ ZoomId ] = aAction;

  // Panning
  aAction = new QtxAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activatePanning()));
	myActionsMap[ PanId ] = aAction;

  // Global Panning
  aAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateGlobalPanning()));
	myActionsMap[ GlobalPanId ] = aAction;

  // Rotation
  aAction = new QtxAction(tr("MNU_ROTATE_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_ROTATE" ) ),
                           tr( "MNU_ROTATE_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ROTATE_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateRotation()));
	myActionsMap[ RotationId ] = aAction;

  // Projections
  aAction = new QtxAction(tr("MNU_FRONT_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_FRONT" ) ),
                           tr( "MNU_FRONT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_FRONT_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onFrontView()));
	myActionsMap[ FrontId ] = aAction;

  aAction = new QtxAction(tr("MNU_BACK_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_BACK" ) ),
                           tr( "MNU_BACK_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_BACK_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onBackView()));
	myActionsMap[ BackId ] = aAction;

  aAction = new QtxAction(tr("MNU_TOP_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_TOP" ) ),
                           tr( "MNU_TOP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_TOP_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onTopView()));
	myActionsMap[ TopId ] = aAction;

  aAction = new QtxAction(tr("MNU_BOTTOM_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_BOTTOM" ) ),
                           tr( "MNU_BOTTOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_BOTTOM_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onBottomView()));
	myActionsMap[ BottomId ] = aAction;

  aAction = new QtxAction(tr("MNU_LEFT_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_LEFT" ) ),
                           tr( "MNU_LEFT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_LEFT_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onLeftView()));
	myActionsMap[ LeftId ] = aAction;

  aAction = new QtxAction(tr("MNU_RIGHT_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_RIGHT" ) ),
                           tr( "MNU_RIGHT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RIGHT_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onRightView()));
	myActionsMap[ RightId ] = aAction;

  // Reset
  aAction = new QtxAction(tr("MNU_RESET_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_RESET" ) ),
                           tr( "MNU_RESET_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onResetView()));
	myActionsMap[ ResetId ] = aAction;

  // Reset
  aAction = new QtxAction(tr("MNU_CLONE_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_CLONE_VIEW" ) ),
                           tr( "MNU_CLONE_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_CLONE_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onCloneView()));
	myActionsMap[ CloneId ] = aAction;

  myClippingAction = new QtxAction(tr("MNU_CLIPPING"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_CLIPPING" ) ),
                           tr( "MNU_CLIPPING" ), 0, this);
  myClippingAction->setStatusTip(tr("DSC_CLIPPING"));
  myClippingAction->setToggleAction( true );
  connect(myClippingAction, SIGNAL(toggled( bool )), this, SLOT(onClipping( bool )));
	myActionsMap[ ClippingId ] = myClippingAction;

  aAction = new QtxAction(tr("MNU_SHOOT_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_SHOOT_VIEW" ) ),
                           tr( "MNU_SHOOT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_SHOOT_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onMemorizeView()));
	myActionsMap[ MemId ] = aAction;

  aAction = new QtxAction(tr("MNU_PRESETS_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_PRESETS_VIEW" ) ),
                           tr( "MNU_PRESETS_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PRESETS_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onRestoreView()));
	myActionsMap[ RestoreId ] = aAction;

  if (myModel->trihedronActivated()) {
    aAction = new QtxAction(tr("MNU_SHOW_TRIHEDRE"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_TRIHEDRON" ) ),
                             tr( "MNU_SHOW_TRIHEDRE" ), 0, this);
    aAction->setStatusTip(tr("DSC_SHOW_TRIHEDRE"));
    connect(aAction, SIGNAL(activated()), this, SLOT(onTrihedronShow()));
	  myActionsMap[ TrihedronShowId ] = aAction;
  }
}

/*!
  Creates toolbar of OCC view window
*/
void OCCViewer_ViewWindow::createToolBar()
{
  myActionsMap[DumpId]->addTo(myToolBar);  
  if ( myModel->trihedronActivated() ) 
    myActionsMap[TrihedronShowId]->addTo(myToolBar);

  SUIT_ToolButton* aScaleBtn = new SUIT_ToolButton(myToolBar, "scale");
  aScaleBtn->AddAction(myActionsMap[FitAllId]);
  aScaleBtn->AddAction(myActionsMap[FitRectId]);
  aScaleBtn->AddAction(myActionsMap[ZoomId]);

  SUIT_ToolButton* aPanningBtn = new SUIT_ToolButton(myToolBar, "pan");
  aPanningBtn->AddAction(myActionsMap[PanId]);
  aPanningBtn->AddAction(myActionsMap[GlobalPanId]);

  myActionsMap[RotationId]->addTo(myToolBar);

  SUIT_ToolButton* aViewsBtn = new SUIT_ToolButton(myToolBar, "projection");
  aViewsBtn->AddAction(myActionsMap[FrontId]);
  aViewsBtn->AddAction(myActionsMap[BackId]);
  aViewsBtn->AddAction(myActionsMap[TopId]);
  aViewsBtn->AddAction(myActionsMap[BottomId]);
  aViewsBtn->AddAction(myActionsMap[LeftId]);
  aViewsBtn->AddAction(myActionsMap[RightId]);

  myActionsMap[ResetId]->addTo(myToolBar);

  SUIT_ToolButton* aMemBtn = new SUIT_ToolButton(myToolBar, "view");
  aMemBtn->AddAction(myActionsMap[MemId]);
  aMemBtn->AddAction(myActionsMap[RestoreId]);

  myToolBar->addSeparator();
  myActionsMap[CloneId]->addTo(myToolBar);
  
  myToolBar->addSeparator();
  myActionsMap[ClippingId]->addTo(myToolBar);
}

/*!
  Processes operation fit all
*/
void OCCViewer_ViewWindow::onViewFitAll()
{
  myViewPort->fitAll();
}

/*!
  Processes transformation "front view"
*/
void OCCViewer_ViewWindow::onFrontView()
{
  emit vpTransformationStarted ( FRONTVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Xpos);
  onViewFitAll();
}

/*!
  Processes transformation "back view"
*/
void OCCViewer_ViewWindow::onBackView()
{
  emit vpTransformationStarted ( BACKVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Xneg);
  onViewFitAll();
}

/*!
  Processes transformation "top view"
*/
void OCCViewer_ViewWindow::onTopView()
{
  emit vpTransformationStarted ( TOPVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Zpos);
  onViewFitAll();
}

/*!
  Processes transformation "bottom view"
*/
void OCCViewer_ViewWindow::onBottomView()
{
  emit vpTransformationStarted ( BOTTOMVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Zneg);
  onViewFitAll();
}

/*!
  Processes transformation "left view"
*/
void OCCViewer_ViewWindow::onLeftView()
{
  emit vpTransformationStarted ( LEFTVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Yneg);
  onViewFitAll();
}

/*!
  Processes transformation "right view"
*/
void OCCViewer_ViewWindow::onRightView()
{
  emit vpTransformationStarted ( RIGHTVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Ypos);
  onViewFitAll();
}

/*!
  Processes transformation "reset view": sets default orientation of viewport camera
*/
void OCCViewer_ViewWindow::onResetView()
{
  emit vpTransformationStarted( RESETVIEW );
  bool upd = myViewPort->getView()->SetImmediateUpdate( false );
  myViewPort->getView()->Reset( false );
  myViewPort->fitAll( false, true, false );
  myViewPort->getView()->SetImmediateUpdate( upd );
  myViewPort->getView()->Update();
}

/*!
  Processes transformation "fit all"
*/
void OCCViewer_ViewWindow::onFitAll()
{
  emit vpTransformationStarted( FITALLVIEW );
  myViewPort->fitAll();
}

/*!
  Creates one more window with same content
*/
void OCCViewer_ViewWindow::onCloneView()
{
  SUIT_ViewWindow* vw = myManager->createViewWindow();
  vw->show();
}

/*!
  SLOT: called if clipping operation is activated, enables/disables of clipping plane
*/
void OCCViewer_ViewWindow::onClipping( bool on )
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  if ( on )
    myActionsMap[ ClippingId ]->setIconSet(aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_CLIPPING_PRESSED" )));
  else
    myActionsMap[ ClippingId ]->setIconSet(aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_CLIPPING" )));
  
  if ( on )
    {
      if ( !myClippingDlg )
	{
	  myClippingDlg = new OCCViewer_ClippingDlg( this, myDesktop );
	  myClippingDlg->SetAction( myClippingAction );
	}

      if ( !myClippingDlg->isShown() )
	myClippingDlg->show();
    }
  else
    {
      if ( myClippingDlg->isShown() )
	myClippingDlg->hide();
      setCuttingPlane(false);
    }
}

/*!
  Stores view parameters
*/
void OCCViewer_ViewWindow::onMemorizeView()
{
  myModel->appendViewAspect( getViewParams() );
}

/*!
  Restores view parameters
*/
void OCCViewer_ViewWindow::onRestoreView()
{
	OCCViewer_CreateRestoreViewDlg* aDlg = new OCCViewer_CreateRestoreViewDlg( centralWidget(), myModel );
	connect( aDlg, SIGNAL( dlgOk() ), this, SLOT( setRestoreFlag() ) );
	aDlg->exec();
	myModel->updateViewAspects( aDlg->parameters() );
	if( myRestoreFlag && aDlg->parameters().count() )
		performRestoring( aDlg->currentItem() );
}

/*!
  Restores view parameters from structure viewAspect
*/
void OCCViewer_ViewWindow::performRestoring( const viewAspect& anItem )
{
	Handle(V3d_View) aView3d = myViewPort->getView();

	Standard_Boolean prev = aView3d->SetImmediateUpdate( Standard_False );
	aView3d->SetScale( anItem.scale );
	aView3d->SetCenter( anItem.centerX, anItem.centerY );
	aView3d->SetTwist( anItem.twist );
	aView3d->SetAt( anItem.atX, anItem.atY, anItem.atZ );
	aView3d->SetImmediateUpdate( prev );
	aView3d->SetEye( anItem.eyeX, anItem.eyeY, anItem.eyeZ );
	aView3d->SetProj( anItem.projX, anItem.projY, anItem.projZ );
		
	myRestoreFlag = 0;
}

/*!
  Sets restore flag
*/
void OCCViewer_ViewWindow::setRestoreFlag()
{
	myRestoreFlag = 1;
}

/*!
  SLOT: called when action "show/hide" trihedron is activated
*/
void OCCViewer_ViewWindow::onTrihedronShow()
{
  myModel->toggleTrihedron();
}

/*!
  \return QImage, containing all scene rendering in window
*/
QImage OCCViewer_ViewWindow::dumpView()
{
  QPixmap px = QPixmap::grabWindow( myViewPort->winId() );
  return px.convertToImage();
}

/*!
  Sets parameters of cutting plane
  \param on - is cutting plane enabled
  \param x - x-position of plane point 
  \param y - y-position of plane point 
  \param z - z-position of plane point 
  \param dx - x-coordinate of plane normal
  \param dy - y-coordinate of plane normal
  \param dz - z-coordinate of plane normal
*/
void  OCCViewer_ViewWindow::setCuttingPlane( bool on, const double x,  const double y,  const double z,
                				      const double dx, const double dy, const double dz )
{
  Handle(V3d_View) view = myViewPort->getView();
  if ( view.IsNull() )
    return;

  if ( on ) {
    Handle(V3d_Viewer) viewer = myViewPort->getViewer();
    
    // try to use already existing plane or create a new one
    Handle(V3d_Plane) clipPlane;
    view->InitActivePlanes();
    if ( view->MoreActivePlanes() )
      clipPlane = view->ActivePlane();
    else
      clipPlane = new V3d_Plane( viewer );
    
    // set new a,b,c,d values for the plane
    gp_Pln pln( gp_Pnt( x, y, z ), gp_Dir( dx, dy, dz ) );
    double a, b, c, d;
    pln.Coefficients( a, b, c, d );
    clipPlane->SetPlane( a, b, c, d );
    
    view->SetPlaneOn( clipPlane );
  } 
  else
    view->SetPlaneOff();
  
  view->Update();
  view->Redraw();
}

/*!
  \return true if there is at least one cutting plane
*/
bool OCCViewer_ViewWindow::isCuttingPlane()
{
  Handle(V3d_View) view = myViewPort->getView();
  view->InitActivePlanes();
  return (view->MoreActivePlanes());
}

/*!
  The method returns the visual parameters of this view as a viewAspect object
*/
viewAspect OCCViewer_ViewWindow::getViewParams() const
{
  double centerX, centerY, projX, projY, projZ, twist;
  double atX, atY, atZ, eyeX, eyeY, eyeZ;

  Handle(V3d_View) aView3d = myViewPort->getView();

  aView3d->Center( centerX, centerY );
  aView3d->Proj( projX, projY, projZ );
  aView3d->At( atX, atY, atZ );
  aView3d->Eye( eyeX, eyeY, eyeZ );
  twist = aView3d->Twist();

  QString aName = QTime::currentTime().toString() + QString::fromLatin1( " h:m:s" );

  viewAspect params;
  params.scale    = aView3d->Scale();
  params.centerX  = centerX;
  params.centerY  = centerY;
  params.projX    = projX;
  params.projY    = projY;
  params.projZ    = projZ;
  params.twist    = twist;
  params.atX      = atX;
  params.atY      = atY;
  params.atZ      = atZ;
  params.eyeX     = eyeX;
  params.eyeY     = eyeY;
  params.eyeZ     = eyeZ;
  params.name	  = aName;

  return params;
}


/*!
  The method returns the visual parameters of this view as a formated string
*/
QString OCCViewer_ViewWindow::getVisualParameters()
{
  viewAspect params = getViewParams();
  QString retStr;
  retStr.sprintf( "%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e", params.scale,
		  params.centerX, params.centerY, params.projX, params.projY, params.projZ, params.twist,
		  params.atX, params.atY, params.atZ, params.eyeX, params.eyeY, params.eyeZ );
  return retStr;
}

/*!
  The method restors visual parameters of this view from a formated string
*/
void OCCViewer_ViewWindow::setVisualParameters( const QString& parameters )
{
  QStringList paramsLst = QStringList::split( '*', parameters, true );
  if ( paramsLst.size() == 13 ) {
    viewAspect params;
    params.scale    = paramsLst[0].toDouble();
    params.centerX  = paramsLst[1].toDouble();
    params.centerY  = paramsLst[2].toDouble();
    params.projX    = paramsLst[3].toDouble();
    params.projY    = paramsLst[4].toDouble();
    params.projZ    = paramsLst[5].toDouble();
    params.twist    = paramsLst[6].toDouble();
    params.atX      = paramsLst[7].toDouble();
    params.atY      = paramsLst[8].toDouble();
    params.atZ      = paramsLst[9].toDouble();
    params.eyeX     = paramsLst[10].toDouble();
    params.eyeY     = paramsLst[11].toDouble();
    params.eyeZ     = paramsLst[12].toDouble();

    performRestoring( params );
  }
}
