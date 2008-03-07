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
#include "OCCViewer_ViewManager.h"
#include "OCCViewer_ViewSketcher.h"
#include "OCCViewer_CreateRestoreViewDlg.h"
#include "OCCViewer_ClippingDlg.h"
#include "OCCViewer_SetRotationPointDlg.h"

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
#include <TColgp_Array1OfPnt2d.hxx>

#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Shape.hxx>

#include <BRep_Tool.hxx>
#include <TopoDS.hxx>

#include <BRepBndLib.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Visual3d_View.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_Structure.hxx>

static QEvent* l_mbPressEvent = 0;

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
  mySetRotationPointDlg = 0;

  mypSketcher = 0;
  myCurSketch = -1;
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

  myCurrPointType = GRAVITY;
  myPrevPointType = GRAVITY;
  mySelectedPoint = gp_Pnt(0.,0.,0.);
  myRotationPointSelection = false;

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
	    myViewPort->startRotation(myStartX, myStartY, myCurrPointType, mySelectedPoint);
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
	    myViewPort->startRotation(myStartX, myStartY, myCurrPointType, mySelectedPoint);
      break;
    default:
      if ( myRotationPointSelection )
      {
	if ( theEvent->button() == Qt::LeftButton )
	{
	  Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
	  ic->Select();
	  for ( ic->InitSelected(); ic->MoreSelected(); ic->NextSelected() )
	  {
	    TopoDS_Shape aShape = ic->SelectedShape();
	    if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
	    {
	      gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( ic->SelectedShape() ) ); 
	      if ( mySetRotationPointDlg )
	      {
		myRotationPointSelection = false;
		mySetRotationPointDlg->setCoords(aPnt.X(), aPnt.Y(), aPnt.Z());
	      }
	    }	 
	    else 
	    {
	      myCurrPointType = myPrevPointType;
	      break;
	    }
	  }
	  if ( ic->NbSelected() == 0 ) myCurrPointType = myPrevPointType;
	  if ( mySetRotationPointDlg ) mySetRotationPointDlg->toggleChange();
	  ic->CloseAllContexts();
	  myOperation = NOTHING; 
	  setCursor( myCursor );
	  myCursorIsHand = false;
	  myRotationPointSelection = false;
	}
      }
      else
	emit mousePressed(this, theEvent);
      break;
    }
    /* notify that we start a transformation */
    if ( transformRequested() ) 
	    emit vpTransformationStarted ( myOperation );
  }
  if ( transformRequested() ) 
    setTransformInProcess( true );		 

  /* we may need it for sketching... */
  if ( l_mbPressEvent )
    delete l_mbPressEvent;
  l_mbPressEvent = new QMouseEvent( *theEvent );
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
  Compute the gravity center
*/
bool OCCViewer_ViewWindow::computeGravityCenter( double& theX, double& theY, double& theZ )
{
  Handle(Visual3d_View) aView = myViewPort->getView()->View();

  Standard_Real Xmin,Ymin,Zmin,Xmax,Ymax,Zmax,U,V,W ;
  Standard_Real Umin,Vmin,Umax,Vmax ;
  Standard_Integer Nstruct,Npoint ;
  Graphic3d_MapOfStructure MySetOfStructures;
  
  aView->DisplayedStructures (MySetOfStructures);
  Nstruct = MySetOfStructures.Extent() ;
  
  Graphic3d_MapIteratorOfMapOfStructure MyIterator(MySetOfStructures) ;
  aView->ViewMapping().WindowLimit(Umin,Vmin,Umax,Vmax) ;
  Npoint = 0 ; theX = theY = theZ = 0. ;
  for( ; MyIterator.More(); MyIterator.Next()) {
    if (!(MyIterator.Key())->IsEmpty()) {
      (MyIterator.Key())->MinMaxValues(Xmin,Ymin,Zmin,
                                         Xmax,Ymax,Zmax) ;
    
      Standard_Real LIM = ShortRealLast() -1.;
      if (!    (fabs(Xmin) > LIM || fabs(Ymin) > LIM || fabs(Zmin) > LIM 
                ||  fabs(Xmax) > LIM || fabs(Ymax) > LIM || fabs(Zmax) > LIM )) {
        
        aView->Projects(Xmin,Ymin,Zmin,U,V,W) ;
        if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
          Npoint++ ; theX += Xmin ; theY += Ymin ; theZ += Zmin ;
        }
        aView->Projects(Xmax,Ymin,Zmin,U,V,W) ;
        if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
          Npoint++ ; theX += Xmax ; theY += Ymin ; theZ += Zmin ;
        }
        aView->Projects(Xmin,Ymax,Zmin,U,V,W) ;
        if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
          Npoint++ ; theX += Xmin ; theY += Ymax ; theZ += Zmin ;
        }
        aView->Projects(Xmax,Ymax,Zmin,U,V,W) ;
        if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
          Npoint++ ; theX += Xmax ; theY += Ymax ; theZ += Zmin ;
        }
        aView->Projects(Xmin,Ymin,Zmax,U,V,W) ;
        if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
          Npoint++ ; theX += Xmin ; theY += Ymin ; theZ += Zmax ;
        }
        aView->Projects(Xmax,Ymin,Zmax,U,V,W) ;
        if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
          Npoint++ ; theX += Xmax ; theY += Ymin ; theZ += Zmax ;
        }
        aView->Projects(Xmin,Ymax,Zmax,U,V,W) ;
        if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
          Npoint++ ; theX += Xmin ; theY += Ymax ; theZ += Zmax ;
        }
        aView->Projects(Xmax,Ymax,Zmax,U,V,W) ;
        if( U >= Umin && U <= Umax && V >= Vmin && V <= Vmax ) {
          Npoint++ ; theX += Xmax ; theY += Ymax ; theZ += Zmax ;
        }
      }
    }
  }
  if( Npoint > 0 ) {
    theX /= Npoint ; theY /= Npoint ; theZ /= Npoint ;
  }
  return true;
}

/*!
  Set the gravity center as a rotation point
*/
void OCCViewer_ViewWindow::activateSetRotationGravity()
{
  if ( myRotationPointSelection )
  {
    Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
    ic->CloseAllContexts();
    myOperation = NOTHING; 
    setCursor( myCursor );
    myCursorIsHand = false;
    myRotationPointSelection = false;
  }

  myPrevPointType = myCurrPointType;
  myCurrPointType = GRAVITY;

  Standard_Real Xcenter, Ycenter, Zcenter;
  if ( computeGravityCenter( Xcenter, Ycenter, Zcenter ) )
    mySetRotationPointDlg->setCoords( Xcenter, Ycenter, Zcenter );
}

/*!
  Update gravity center in the SetRotationPointDlg
*/
void OCCViewer_ViewWindow::updateGravityCoords()
{
  if ( mySetRotationPointDlg && mySetRotationPointDlg->isShown() && myCurrPointType == GRAVITY )
  {
    Standard_Real Xcenter, Ycenter, Zcenter;
    if ( computeGravityCenter( Xcenter, Ycenter, Zcenter ) )
      mySetRotationPointDlg->setCoords( Xcenter, Ycenter, Zcenter );
  }
}

/*!
  Set the point selected by user as a rotation point
*/
void OCCViewer_ViewWindow::activateSetRotationSelected(double theX, double theY, double theZ)
{
  if ( myRotationPointSelection )
  {
    Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
    ic->CloseAllContexts();
    myOperation = NOTHING; 
    setCursor( myCursor );
    myCursorIsHand = false;
    myRotationPointSelection = false;
  }

  myPrevPointType = myCurrPointType;
  myCurrPointType = SELECTED;
  mySelectedPoint.SetCoord(theX,theY,theZ);
}

/*!
  Start the point selection process
*/
void OCCViewer_ViewWindow::activateStartPointSelection()
{
  myPrevPointType = myCurrPointType;
  myCurrPointType = SELECTED;

  // activate selection ------>
  Handle(AIS_InteractiveContext) ic = myModel->getAISContext();

  ic->OpenLocalContext();

  AIS_ListOfInteractive aList;
  ic->DisplayedObjects( aList );
  for ( AIS_ListIteratorOfListOfInteractive it( aList ); it.More(); it.Next() ) 
  {
    Handle(AIS_InteractiveObject) anObj = it.Value();
    if ( !anObj.IsNull() && anObj->HasPresentation() &&
         anObj->IsKind( STANDARD_TYPE(AIS_Shape) ) )
    {
      ic->Load(anObj,-1);
      ic->Activate(anObj,AIS_Shape::SelectionMode(TopAbs_VERTEX));
     }
  }  
  // activate selection <------

  if ( !myCursorIsHand )
  {
    QCursor handCursor (Qt::PointingHandCursor);
    myCursorIsHand = true;		
    myCursor = cursor();
    setCursor( handCursor );
  }
  myRotationPointSelection = true;
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
    myViewPort->rotate(myCurrX, myCurrY, myCurrPointType, mySelectedPoint);
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
    if ( myRotationPointSelection )
      emit mouseMoving( this, theEvent ); 
    else
    {
      int aState = theEvent->state();
      if ( aState == Qt::LeftButton ||
	   aState == ( Qt::LeftButton | Qt::ShiftButton) ) {
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
      else if ( aState == Qt::RightButton || 
		aState == ( Qt::RightButton | Qt::ShiftButton ) ) {
	OCCViewer_ViewSketcher* sketcher = 0;
	for ( OCCViewer_ViewSketcher* sk = mySketchers.first();
	      sk && !sketcher; sk = mySketchers.next() )
	{
	  if( sk->isDefault() && sk->sketchButton() & ( aState & Qt::MouseButtonMask ) )
	    sketcher = sk;
	}
	if ( sketcher && myCurSketch == -1 )
	{
	  activateSketching( sketcher->type() );
	  if ( mypSketcher )
	  {
	    myCurSketch = mypSketcher->sketchButton();

	    if ( l_mbPressEvent )
	    {
	      QApplication::sendEvent( getViewPort(), l_mbPressEvent );
	      delete l_mbPressEvent;
	      l_mbPressEvent = 0;
	    }
	    QApplication::sendEvent( getViewPort(), theEvent );
	  }
	}
      }
      else {
	emit mouseMoving( this, theEvent ); 
      }	
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
      int prevState = myCurSketch;
      if(theEvent->state() == RightButton)
      {
	for ( OCCViewer_ViewSketcher* sk = mySketchers.first();
	      sk && myCurSketch != -1; sk = mySketchers.next() )
	{
	  if( ( sk->sketchButton() & theEvent->state() ) && sk->sketchButton() == myCurSketch )
	    myCurSketch = -1;
	}
      }

      emit mouseReleased(this, theEvent);
      if(theEvent->button() == RightButton && prevState == -1)
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

  if ( l_mbPressEvent )
  {
    delete l_mbPressEvent;
    l_mbPressEvent = 0;
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
  
  if ( myRotationPointSelection )
  {
    QCursor handCursor (Qt::PointingHandCursor);
    setCursor( handCursor );
  }
  else
  { 
    if ( transformRequested() || myCursorIsHand ) 
      setCursor( myCursor );
    myCursorIsHand = false;
  }
  
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

  // Rotation Point
  mySetRotationPointAction = new QtxAction(tr("MNU_CHANGINGROTATIONPOINT_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_ROTATION_POINT" ) ),
                           tr( "MNU_CHANGINGROTATIONPOINT_VIEW" ), 0, this);
  mySetRotationPointAction->setStatusTip(tr("DSC_CHANGINGROTATIONPOINT_VIEW"));
  mySetRotationPointAction->setToggleAction( true );
  connect(mySetRotationPointAction, SIGNAL(toggled( bool )), this, SLOT(onSetRotationPoint( bool )));
  myActionsMap[ ChangeRotationPointId ] = mySetRotationPointAction;

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

  myActionsMap[ChangeRotationPointId]->addTo(myToolBar);

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
  SLOT: called if change rotation point operation is activated
*/
void OCCViewer_ViewWindow::onSetRotationPoint( bool on )
{
  if ( on )
    {
      if ( !mySetRotationPointDlg )
	{
	  mySetRotationPointDlg = new OCCViewer_SetRotationPointDlg( this, myDesktop );
	  mySetRotationPointDlg->SetAction( mySetRotationPointAction );
	}

      if ( !mySetRotationPointDlg->isShown() )
      {
	if ( mySetRotationPointDlg->IsFirstShown() )
	{ 
	  Standard_Real Xcenter, Ycenter, Zcenter;
	  if ( computeGravityCenter( Xcenter, Ycenter, Zcenter ) )
	    mySetRotationPointDlg->setCoords( Xcenter, Ycenter, Zcenter );
	}
	mySetRotationPointDlg->show();
      }
    }
  else
    {
      if ( mySetRotationPointDlg->isShown() )
	mySetRotationPointDlg->hide();
    }
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

/*!
  Custom show event handler
*/
void OCCViewer_ViewWindow::showEvent( QShowEvent * theEvent ) 
{
  emit Show( theEvent );
}

/*!
  Custom hide event handler
*/
void OCCViewer_ViewWindow::hideEvent( QHideEvent * theEvent ) 
{
  emit Hide( theEvent );
}


/*!
    Creates default sketcher. [ virtual protected ]
*/
OCCViewer_ViewSketcher* OCCViewer_ViewWindow::createSketcher( int type )
{
  if ( type == Rect )
    return new OCCViewer_RectSketcher( this, type );
  if ( type == Polygon )
    return new OCCViewer_PolygonSketcher( this, type );
  return 0;
}

void OCCViewer_ViewWindow::initSketchers()
{
  if ( mySketchers.isEmpty() )
  {
    mySketchers.append( createSketcher( Rect ) );
    mySketchers.append( createSketcher( Polygon ) );
  }
}

OCCViewer_ViewSketcher* OCCViewer_ViewWindow::getSketcher( const int typ )
{
  OCCViewer_ViewSketcher* sketcher = 0;
  for ( OCCViewer_ViewSketcher* sk = mySketchers.first();
        sk && !sketcher; sk = mySketchers.next() )
  {
    if ( sk->type() == typ )
      sketcher = sk;
  }
  return sketcher;
}

/*!
    Handles requests for sketching in the active view. [ virtual public ]
*/
void OCCViewer_ViewWindow::activateSketching( int type )
{
  OCCViewer_ViewPort3d* vp = getViewPort();
  if ( !vp )
    return;

  if ( !vp->isSketchingEnabled() )
    return;

  /* Finish current sketching */
  if ( type == NoSketching )
  {
    if ( mypSketcher )
    {
      onSketchingFinished();
      mypSketcher->deactivate();
      mypSketcher = 0;
    }
  }
  /* Activate new sketching */
  else
  {
    activateSketching( NoSketching );  /* concurrency not suported */
    mypSketcher = getSketcher( type );
    if ( mypSketcher )
    {
      mypSketcher->activate();
      onSketchingStarted();
    }
  }
}

/*!
    Unhilights detected entities. [ virtual protected ]
*/
void OCCViewer_ViewWindow::onSketchingStarted()
{
}

/*!
    Selection by rectangle or polygon. [ virtual protected ]
*/
void OCCViewer_ViewWindow::onSketchingFinished()
{
  if ( mypSketcher && mypSketcher->result() == OCCViewer_ViewSketcher::Accept )
  {
    Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
    bool append = bool( mypSketcher->buttonState() & Qt::ShiftButton );
    switch( mypSketcher->type() )
    {
    case Rect:
      {
	QRect* aRect = (QRect*)mypSketcher->data();
	if( aRect )
	{
	  int aLeft = aRect->left();
	  int aRight = aRect->right();
	  int aTop = aRect->top();
	  int aBottom = aRect->bottom();

	  if( append )
	    ic->ShiftSelect( aLeft, aBottom, aRight, aTop, getViewPort()->getView(), Standard_False );
	  else
	    ic->Select( aLeft, aBottom, aRight, aTop, getViewPort()->getView(), Standard_False );
	}
      }
      break;
    case Polygon:
      {
        QPointArray* aPolygon = (QPointArray*)mypSketcher->data();
        if( aPolygon )
        {
	  int size = aPolygon->size();
	  TColgp_Array1OfPnt2d anArray( 1, size );

	  QPointArray::Iterator it = aPolygon->begin();
	  QPointArray::Iterator itEnd = aPolygon->end();
	  for( int index = 1; it != itEnd; ++it, index++ )
	  {
	    QPoint aPoint = *it;
	    anArray.SetValue( index, gp_Pnt2d( aPoint.x(), aPoint.y() ) );
	  }

	  if( append )
	    ic->ShiftSelect( anArray, getViewPort()->getView(), Standard_False );
	  else
	    ic->Select( anArray, getViewPort()->getView(), Standard_False );
        }
      }
      break;
    default:
      break;
    }

    OCCViewer_ViewManager* aViewMgr = ( OCCViewer_ViewManager* )getViewManager();
    aViewMgr->getOCCViewer()->performSelectionChanged();
  }
}
