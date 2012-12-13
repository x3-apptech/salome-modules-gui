// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File   : OCCViewer_ViewWindow.cxx
// Author :

#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewManager.h"
#include "OCCViewer_ViewSketcher.h"
#include "OCCViewer_CreateRestoreViewDlg.h"
#include "OCCViewer_ClippingDlg.h"
#include "OCCViewer_SetRotationPointDlg.h"
#include "OCCViewer_AxialScaleDlg.h"
#include "OCCViewer_CubeAxesDlg.h"

#include <Basics_OCCTVersion.hxx>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SUIT_Tools.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Application.h>

#include <QtxActionToolMgr.h>
#include <QtxMultiAction.h>
#include <QtxRubberBand.h>

#include <OpenGLUtils_FrameBuffer.h>

#include <QPainter>
#include <QTime>
#include <QImage>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QMenu>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Shape.hxx>

#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <TopoDS.hxx>

#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_ExportFormat.hxx>

#include <Visual3d_View.hxx>
#include <V3d_Plane.hxx>
#include <V3d_Light.hxx>

#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <Standard_Version.hxx>

#include "utilities.h"

// // OpenCV includes
// #include <cv.h>
// #include <highgui.h>

static QEvent* l_mbPressEvent = 0;

#ifdef WIN32
# include <QWindowsStyle>
#endif

#include <GL/gl.h>

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
  \brief Constructor
  \param theDesktop main window of application
  \param theModel OCC 3D viewer
*/
OCCViewer_ViewWindow::OCCViewer_ViewWindow( SUIT_Desktop*     theDesktop,
                                            OCCViewer_Viewer* theModel )
: SUIT_ViewWindow( theDesktop )
{
  myModel = theModel;
  myRestoreFlag = 0;
  myEnableDrawMode = false;
  myDrawRect=false;
  updateEnabledDrawMode();
  myClippingDlg = 0;
  myScalingDlg = 0;
  mySetRotationPointDlg = 0;
  myRectBand = 0;
  
  IsSketcherStyle = false;

  mypSketcher = 0;
  myCurSketch = -1;
  my2dMode = No2dMode;

  myInteractionStyle = SUIT_ViewModel::STANDARD;

  clearViewAspects();
  
}

/*!
  \brief Destructor.
*/
OCCViewer_ViewWindow::~OCCViewer_ViewWindow()
{
  endDrawRect();
  qDeleteAll( mySketchers );
}

/*!
  \brief Internal initialization.
*/
void OCCViewer_ViewWindow::initLayout()
{
  myViewPort = new OCCViewer_ViewPort3d( this, myModel->getViewer3d(), V3d_ORTHOGRAPHIC );
  myViewPort->installEventFilter(this);
  setCentralWidget(myViewPort);
  myOperation = NOTHING;

  myCurrPointType = GRAVITY;
  myPrevPointType = GRAVITY;
  mySelectedPoint = gp_Pnt(0.,0.,0.);
  myRotationPointSelection = false;

  setTransformRequested ( NOTHING );
  setTransformInProcess ( false );

  createActions();
  createToolBar();

  switch (my2dMode) {
  case XYPlane:
    onTopView();
    break;
  case XZPlane:
    onLeftView();
    break;
  case YZPlane:
    onFrontView();
    break;
  }

  // Graduated axes dialog
  QtxAction* anAction = dynamic_cast<QtxAction*>( toolMgr()->action( GraduatedAxesId ) );
  myCubeAxesDlg = new OCCViewer_CubeAxesDlg( anAction, this, "OCCViewer_CubeAxesDlg" );
  myCubeAxesDlg->initialize();
  
  connect( myViewPort, SIGNAL( vpTransformed( OCCViewer_ViewPort* ) ), this, SLOT( emitViewModified() ) );
}

OCCViewer_ViewWindow* OCCViewer_ViewWindow::getView( const int mode ) const
{
  return mode == get2dMode() ? const_cast<OCCViewer_ViewWindow*>( this ) : 0;
}

/*!
  \brief Detect viewer operation according the the mouse button pressed
  and key modifiers used.
  \param theEvent mouse event
  \return type of the operation
*/
OCCViewer_ViewWindow::OperationType
OCCViewer_ViewWindow::getButtonState( QMouseEvent* theEvent, int theInteractionStyle )
{
  OperationType aOp = NOTHING;
  SUIT_ViewModel::InteractionStyle aStyle = (SUIT_ViewModel::InteractionStyle)theInteractionStyle;
  if( (theEvent->modifiers() == SUIT_ViewModel::myStateMap[aStyle][SUIT_ViewModel::ZOOM]) &&
      (theEvent->buttons() == SUIT_ViewModel::myButtonMap[aStyle][SUIT_ViewModel::ZOOM]) )
    aOp = ZOOMVIEW;
  else if( (theEvent->modifiers() == SUIT_ViewModel::myStateMap[aStyle][SUIT_ViewModel::PAN]) &&
           (theEvent->buttons() == SUIT_ViewModel::myButtonMap[aStyle][SUIT_ViewModel::PAN]) )
    aOp = PANVIEW;
  else if( (theEvent->modifiers()  == SUIT_ViewModel::myStateMap[aStyle][SUIT_ViewModel::ROTATE]) &&
           (theEvent->buttons() == SUIT_ViewModel::myButtonMap[aStyle][SUIT_ViewModel::ROTATE]) &&
           (my2dMode == No2dMode))
    aOp = ROTATE;

  return aOp;
}

/*!
  \brief Customize event handling
  \param watched event receiver object
  \param e event
  \return \c true if the event processing should be stopped
*/
bool OCCViewer_ViewWindow::eventFilter( QObject* watched, QEvent* e )
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
	myViewPort->startZoomAtPoint( aEvent->x(), aEvent->y() );
	double delta = (double)( aEvent->delta() ) / ( 15 * 8 );
	int x  = aEvent->x();
	int y  = aEvent->y();
	int x1 = (int)( aEvent->x() + width()*delta/100 );
	int y1 = (int)( aEvent->y() + height()*delta/100 );
	myViewPort->zoom( x, y, x1, y1 );
      }
      return true;

    case QEvent::ContextMenu:
      {
        QContextMenuEvent * aEvent = (QContextMenuEvent*)e;
        if ( aEvent->reason() != QContextMenuEvent::Mouse )
          emit contextMenuRequested( aEvent );
      }
      return true;

    case QEvent::KeyPress:
      emit keyPressed(this, (QKeyEvent*) e);
      return true;

    default:
      break;
    }
  }
  return SUIT_ViewWindow::eventFilter(watched, e);
}

/*!
  \brief Update state of enable draw mode state.
*/
void OCCViewer_ViewWindow::updateEnabledDrawMode()
{
  if ( myModel )
    myEnableDrawMode = myModel->isSelectionEnabled() && myModel->isMultiSelectionEnabled();
}

/*!
  \brief Handle mouse press event
  \param theEvent mouse event
*/
void OCCViewer_ViewWindow::vpMousePressEvent( QMouseEvent* theEvent )
{
  myStartX = theEvent->x();
  myStartY = theEvent->y();
  int anInteractionStyle = interactionStyle();

  // in "key free" interaction style zoom operation is activated by two buttons (simultaneously pressed),
  // which are assigned for pan and rotate - these operations are activated immediately after pressing 
  // of the first button, so it is necessary to switch to zoom when the second button is pressed
  bool aSwitchToZoom = false;
  if ( anInteractionStyle == SUIT_ViewModel::KEY_FREE && 
       ( myOperation == PANVIEW || myOperation == ROTATE ) ) {
    aSwitchToZoom = getButtonState( theEvent, anInteractionStyle ) == ZOOMVIEW;
  }

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
    if ( theEvent->button() == Qt::LeftButton ) {
      myViewPort->startZoomAtPoint( myStartX, myStartY );
      emit vpTransformationStarted ( ZOOMVIEW );
    }
    break;

  case PANVIEW:
    if ( aSwitchToZoom ) {
      myViewPort->startZoomAtPoint( myStartX, myStartY );
      activateZoom();
    }
    else if ( theEvent->button() == Qt::LeftButton )
      emit vpTransformationStarted ( PANVIEW );
    break;

  case ROTATE:
    if ( aSwitchToZoom ) {
      myViewPort->startZoomAtPoint( myStartX, myStartY );
      activateZoom();
    }
    else if ( theEvent->button() == Qt::LeftButton ) {
      myViewPort->startRotation(myStartX, myStartY, myCurrPointType, mySelectedPoint);
      emit vpTransformationStarted ( ROTATE );
    }
    break;

  default:
  /*  Try to activate a transformation */
    switch ( getButtonState(theEvent, anInteractionStyle) ) {
    case ZOOMVIEW:
      myViewPort->startZoomAtPoint( myStartX, myStartY );
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
          myViewPort->setCursor( myCursor );
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
  \brief Start zooming operation.

  Sets the corresponding cursor for the widget.
*/
void OCCViewer_ViewWindow::activateZoom()
{
  if ( !transformRequested() && !myCursorIsHand )
    myCursor = cursor();                /* save old cursor */

  if ( myOperation != ZOOMVIEW ) {
    QPixmap zoomPixmap (imageZoomCursor);
    QCursor zoomCursor (zoomPixmap);
    if( setTransformRequested ( ZOOMVIEW ) )
      myViewPort->setCursor( zoomCursor );
  }
}


/*!
  \brief Start panning operation.

  Sets the corresponding cursor for the widget.
*/
void OCCViewer_ViewWindow::activatePanning()
{
  if ( !transformRequested() && !myCursorIsHand )
    myCursor = cursor();                // save old cursor

  if ( myOperation != PANVIEW ) {
    QCursor panCursor (Qt::SizeAllCursor);
    if( setTransformRequested ( PANVIEW ) )
      myViewPort->setCursor( panCursor );
  }
}

/*!
  \brief Start rotation operation

  Sets the corresponding cursor for the widget.
*/
void OCCViewer_ViewWindow::activateRotation()
{
  if ( !transformRequested() && !myCursorIsHand )
    myCursor = cursor();                // save old cursor

  if ( myOperation != ROTATE ) {
    QPixmap rotatePixmap (imageRotateCursor);
    QCursor rotCursor (rotatePixmap);
    if( setTransformRequested ( ROTATE ) )
      myViewPort->setCursor( rotCursor );
  }
}

/*!
  \brief Compute the gravity center.
  \param theX used to return X coordinate of the gravity center
  \param theY used to return Y coordinate of the gravity center
  \param theZ used to return Z coordinate of the gravity center
  \return \c true if the gravity center is computed
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
  \brief Set the gravity center as a rotation point.
*/
void OCCViewer_ViewWindow::activateSetRotationGravity()
{
  if ( myRotationPointSelection )
  {
    Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
    ic->CloseAllContexts();
    myOperation = NOTHING;
    myViewPort->setCursor( myCursor );
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
  \brief Update gravity center in the "Set Rotation Point" dialog box.
  \sa OCCViewer_SetRotationPointDlg class
*/
void OCCViewer_ViewWindow::updateGravityCoords()
{
  if ( mySetRotationPointDlg && mySetRotationPointDlg->isVisible() && myCurrPointType == GRAVITY )
  {
    Standard_Real Xcenter, Ycenter, Zcenter;
    if ( computeGravityCenter( Xcenter, Ycenter, Zcenter ) )
      mySetRotationPointDlg->setCoords( Xcenter, Ycenter, Zcenter );
  }
}

/*!
  \brief Set the point selected by the user as a rotation point.
  \param theX X coordinate of the rotation point
  \param theY Y coordinate of the rotation point
  \param theZ Z coordinate of the rotation point
*/
void OCCViewer_ViewWindow::activateSetRotationSelected( double theX, double theY, double theZ )
{
  if ( myRotationPointSelection )
  {
    Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
    ic->CloseAllContexts();
    myOperation = NOTHING;
    myViewPort->setCursor( myCursor );
    myCursorIsHand = false;
    myRotationPointSelection = false;
  }

  myPrevPointType = myCurrPointType;
  myCurrPointType = SELECTED;
  mySelectedPoint.SetCoord(theX,theY,theZ);
}

/*!
  \brief Start the point selection process.
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
    myViewPort->setCursor( handCursor );
  }
  myRotationPointSelection = true;
}

/*!
  \brief Start global panning operation

  Sets the corresponding cursor for the widget.
*/
void OCCViewer_ViewWindow::activateGlobalPanning()
{
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
    QPixmap globalPanPixmap (imageCrossCursor);
    QCursor glPanCursor (globalPanPixmap);
    myCurScale = aView3d->Scale();
    aView3d->FitAll(0.01, false);
    myCursor = cursor();                // save old cursor
    myViewPort->fitAll(); // fits view before selecting a new scene center
    if( setTransformRequested( PANGLOBAL ) )
      myViewPort->setCursor( glPanCursor );
  }
}

/*!
  \brief Starts fit operation.

  Sets the corresponding cursor for the widget.
*/
void OCCViewer_ViewWindow::activateWindowFit()
{
  if ( !transformRequested() && !myCursorIsHand )
    myCursor = cursor();                /* save old cursor */

  if ( myOperation != WINDOWFIT ) {
    QCursor handCursor (Qt::PointingHandCursor);
    if( setTransformRequested ( WINDOWFIT ) )
    {
      myViewPort->setCursor ( handCursor );
      myCursorIsHand = true;
    }
  }
}

/*!
  \brief Start delayed viewer operation.
*/
bool OCCViewer_ViewWindow::setTransformRequested( OperationType op )
{
  bool ok = transformEnabled( op );
  myOperation = ok ? op : NOTHING;
  myViewPort->setMouseTracking( myOperation == NOTHING );  
  return ok;
}

/*!
  \brief Handle mouse move event.
  \param theEvent mouse event
*/
void OCCViewer_ViewWindow::vpMouseMoveEvent( QMouseEvent* theEvent )
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
    if ( myRotationPointSelection || isSketcherStyle() )
    {
      emit mouseMoving( this, theEvent );
    }
    else
    {
      int aState = theEvent->modifiers();
      int aButton = theEvent->buttons();
      int anInteractionStyle = interactionStyle();
      if ( anInteractionStyle == SUIT_ViewModel::STANDARD && 
           aButton == Qt::LeftButton && ( aState == Qt::NoModifier || Qt::ShiftModifier ) ) {
        myDrawRect = myEnableDrawMode;
        if ( myDrawRect ) {
          drawRect();
          if ( !myCursorIsHand )        {   // we are going to sketch a rectangle
            QCursor handCursor (Qt::PointingHandCursor);
            myCursorIsHand = true;
            myCursor = cursor();
            myViewPort->setCursor( handCursor );
          }
        }
        emit mouseMoving( this, theEvent );
      }
      else if ( anInteractionStyle == SUIT_ViewModel::STANDARD && 
                aButton == Qt::RightButton && ( aState == Qt::NoModifier || Qt::ShiftModifier ) ) {
        OCCViewer_ViewSketcher* sketcher = 0;
        QList<OCCViewer_ViewSketcher*>::Iterator it;
        for ( it = mySketchers.begin(); it != mySketchers.end() && !sketcher; ++it )
        {
          OCCViewer_ViewSketcher* sk = (*it);
          if( sk->isDefault() && sk->sketchButton() == aButton )
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
      else
        emit mouseMoving( this, theEvent );
    }
  }
}

/*!
  \brief Handle mouse release event.
  \param theEvent mouse event
*/
void OCCViewer_ViewWindow::vpMouseReleaseEvent(QMouseEvent* theEvent)
{
  switch ( myOperation ) {
  case NOTHING:
    {
      int prevState = myCurSketch;
      if(theEvent->button() == Qt::RightButton)
      {
        QList<OCCViewer_ViewSketcher*>::Iterator it;
        for ( it = mySketchers.begin(); it != mySketchers.end() && myCurSketch != -1; ++it )
        {
          OCCViewer_ViewSketcher* sk = (*it);
          if( ( sk->sketchButton() & theEvent->button() ) && sk->sketchButton() == myCurSketch )
            myCurSketch = -1;
        }
      }

      emit mouseReleased(this, theEvent);
      if(theEvent->button() == Qt::RightButton && prevState == -1)
      {
        QContextMenuEvent aEvent( QContextMenuEvent::Mouse,
                                  theEvent->pos(), theEvent->globalPos() );
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
    if ( theEvent->button() == Qt::LeftButton ) {
      myCurrX = theEvent->x();
      myCurrY = theEvent->y();
      drawRect();
      QRect rect = SUIT_Tools::makeRect(myStartX, myStartY, myCurrX, myCurrY);
      if ( !rect.isEmpty() ) myViewPort->fitRect(rect);
      endDrawRect();
      resetState();
    }
    break;
  }

  // NOTE: viewer 3D detects a rectangle of selection using this event
  // so we must emit it BEFORE resetting the selection rectangle

  if ( theEvent->button() == Qt::LeftButton && myDrawRect ) {
    drawRect();
    endDrawRect();
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
  \brief Reset the viewport to its initial state
  ( no transformations in process etc. )
*/
void OCCViewer_ViewWindow::resetState()
{
  myDrawRect = false;

  if ( myRotationPointSelection )
  {
    QCursor handCursor (Qt::PointingHandCursor);
    myViewPort->setCursor( handCursor );
  }
  else
  {
    if ( transformRequested() || myCursorIsHand )
      myViewPort->setCursor( myCursor );
    myCursorIsHand = false;
  }

  if ( transformRequested() )
    emit vpTransformationFinished (myOperation);

  setTransformInProcess( false );
  setTransformRequested( NOTHING );
}


/*!
  \brief Draw rubber band rectangle.
*/
void OCCViewer_ViewWindow::drawRect()
{
  if ( !myRectBand ) {
    myRectBand = new QtxRectRubberBand( myViewPort );
    //QPalette palette;
    //palette.setColor(myRectBand->foregroundRole(), Qt::white);
    //myRectBand->setPalette(palette);
  }
  //myRectBand->hide();
  
  myRectBand->setUpdatesEnabled ( false );
  QRect aRect = SUIT_Tools::makeRect(myStartX, myStartY, myCurrX, myCurrY);
  myRectBand->initGeometry( aRect );

  if ( !myRectBand->isVisible() )
    myRectBand->show();

  myRectBand->setUpdatesEnabled ( true );
  //myRectBand->repaint();

  //myRectBand->setVisible( aRect.isValid() );
  //if ( myRectBand->isVisible() )
  //  myRectBand->repaint();
  //else
  //  myRectBand->show();
  //myRectBand->repaint();
}

/*!
  \brief Clear rubber band rectangle on the end on the dragging operation.
*/
void OCCViewer_ViewWindow::endDrawRect()
{
  //delete myRectBand;
  //myRectBand = 0;
  if ( myRectBand )
    {
      myRectBand->clearGeometry();
      myRectBand->hide();
    }
}

/*!
  \brief Create actions.
*/
void OCCViewer_ViewWindow::createActions()
{
  if( !toolMgr()->isEmpty() )
    return;
  
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  QtxAction* aAction;

  // Dump view
  aAction = new QtxAction(tr("MNU_DUMP_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_DUMP" ) ),
                           tr( "MNU_DUMP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onDumpView()));
  toolMgr()->registerAction( aAction, DumpId );

  // FitAll
  aAction = new QtxAction(tr("MNU_FITALL"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_FITALL" ) ),
                           tr( "MNU_FITALL" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITALL"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onFitAll()));
  toolMgr()->registerAction( aAction, FitAllId );

  // FitRect
  aAction = new QtxAction(tr("MNU_FITRECT"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_FITAREA" ) ),
                           tr( "MNU_FITRECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITRECT"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(activateWindowFit()));
  toolMgr()->registerAction( aAction, FitRectId );
  
  // Zoom
  aAction = new QtxAction(tr("MNU_ZOOM_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(activateZoom()));
  toolMgr()->registerAction( aAction, ZoomId );

  // Panning
  aAction = new QtxAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(activatePanning()));
  toolMgr()->registerAction( aAction, PanId );

  // Global Panning
  aAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(activateGlobalPanning()));
  toolMgr()->registerAction( aAction, GlobalPanId );

  // Rotation Point
  mySetRotationPointAction = new QtxAction(tr("MNU_CHANGINGROTATIONPOINT_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_ROTATION_POINT" ) ),
                           tr( "MNU_CHANGINGROTATIONPOINT_VIEW" ), 0, this);
  mySetRotationPointAction->setStatusTip(tr("DSC_CHANGINGROTATIONPOINT_VIEW"));
  mySetRotationPointAction->setCheckable( true );
  connect(mySetRotationPointAction, SIGNAL(toggled( bool )), this, SLOT(onSetRotationPoint( bool )));
  toolMgr()->registerAction( mySetRotationPointAction, ChangeRotationPointId );

  // Rotation
  aAction = new QtxAction(tr("MNU_ROTATE_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_ROTATE" ) ),
                           tr( "MNU_ROTATE_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ROTATE_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(activateRotation()));
  toolMgr()->registerAction( aAction, RotationId );

  // Projections
  aAction = new QtxAction(tr("MNU_FRONT_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_FRONT" ) ),
                           tr( "MNU_FRONT_VIEW" ), 0, this, false, "Viewers:Front view");
  aAction->setStatusTip(tr("DSC_FRONT_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onFrontView()));
  this->addAction(aAction);
  toolMgr()->registerAction( aAction, FrontId );

  aAction = new QtxAction(tr("MNU_BACK_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_BACK" ) ),
                           tr( "MNU_BACK_VIEW" ), 0, this, false, "Viewers:Back view");
  aAction->setStatusTip(tr("DSC_BACK_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onBackView()));
  this->addAction(aAction);
  toolMgr()->registerAction( aAction, BackId );

  aAction = new QtxAction(tr("MNU_TOP_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_TOP" ) ),
                           tr( "MNU_TOP_VIEW" ), 0, this, false, "Viewers:Top view");
  aAction->setStatusTip(tr("DSC_TOP_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onTopView()));
  this->addAction(aAction);
  toolMgr()->registerAction( aAction, TopId );

  aAction = new QtxAction(tr("MNU_BOTTOM_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_BOTTOM" ) ),
                           tr( "MNU_BOTTOM_VIEW" ), 0, this, false, "Viewers:Bottom view");
  aAction->setStatusTip(tr("DSC_BOTTOM_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onBottomView()));
  this->addAction(aAction);
  toolMgr()->registerAction( aAction, BottomId );
  
  aAction = new QtxAction(tr("MNU_LEFT_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_LEFT" ) ),
                           tr( "MNU_LEFT_VIEW" ), 0, this, false, "Viewers:Left view");
  aAction->setStatusTip(tr("DSC_LEFT_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onLeftView()));
  this->addAction(aAction);
  toolMgr()->registerAction( aAction, LeftId );

  aAction = new QtxAction(tr("MNU_RIGHT_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_RIGHT" ) ),
                           tr( "MNU_RIGHT_VIEW" ), 0, this, false, "Viewers:Right view");
  aAction->setStatusTip(tr("DSC_RIGHT_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onRightView()));
  this->addAction(aAction);
  toolMgr()->registerAction( aAction, RightId );

  // rotate anticlockwise
  aAction = new QtxAction(tr("MNU_ANTICLOCKWISE_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_ANTICLOCKWISE" ) ),
                           tr( "MNU_ANTICLOCKWISE_VIEW" ), 0, this, false, "Viewers:Rotate anticlockwise");
  aAction->setStatusTip(tr("DSC_ANTICLOCKWISE_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onAntiClockWiseView()));
  this->addAction(aAction);
  toolMgr()->registerAction( aAction, AntiClockWiseId );

  // rotate clockwise
  aAction = new QtxAction(tr("MNU_CLOCKWISE_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_CLOCKWISE" ) ),
                           tr( "MNU_CLOCKWISE_VIEW" ), 0, this, false, "Viewers:Rotate clockwise");
  aAction->setStatusTip(tr("DSC_CLOCKWISE_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onClockWiseView()));
  this->addAction(aAction);
  toolMgr()->registerAction( aAction, ClockWiseId );

  // Reset
  aAction = new QtxAction(tr("MNU_RESET_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_RESET" ) ),
                           tr( "MNU_RESET_VIEW" ), 0, this, false, "Viewers:Reset view");
  aAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onResetView()));
  this->addAction(aAction);
  toolMgr()->registerAction( aAction, ResetId );

  // Clone
  aAction = new QtxAction(tr("MNU_CLONE_VIEW"),
                          aResMgr->loadPixmap("OCCViewer", tr("ICON_OCCVIEWER_CLONE_VIEW")),
                          tr("MNU_CLONE_VIEW"), 0, this);
  aAction->setStatusTip(tr("DSC_CLONE_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onCloneView()));
  toolMgr()->registerAction( aAction, CloneId );

  myClippingAction = new QtxAction(tr("MNU_CLIPPING"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_CLIPPING" ) ),
                           tr( "MNU_CLIPPING" ), 0, this);
  myClippingAction->setStatusTip(tr("DSC_CLIPPING"));
  myClippingAction->setCheckable( true );
  connect(myClippingAction, SIGNAL(toggled( bool )), this, SLOT(onClipping( bool )));
  toolMgr()->registerAction( myClippingAction, ClippingId );

  aAction = new QtxAction(tr("MNU_SHOOT_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_SHOOT_VIEW" ) ),
                           tr( "MNU_SHOOT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_SHOOT_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onMemorizeView()));
  toolMgr()->registerAction( aAction, MemId );

  aAction = new QtxAction(tr("MNU_PRESETS_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_PRESETS_VIEW" ) ),
                           tr( "MNU_PRESETS_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PRESETS_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onRestoreView()));
  toolMgr()->registerAction( aAction, RestoreId );

  if (myModel->trihedronActivated()) {
    aAction = new QtxAction(tr("MNU_SHOW_TRIHEDRE"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_TRIHEDRON" ) ),
                             tr( "MNU_SHOW_TRIHEDRE" ), 0, this);
    aAction->setStatusTip(tr("DSC_SHOW_TRIHEDRE"));
    connect(aAction, SIGNAL(triggered()), this, SLOT(onTrihedronShow()));
    toolMgr()->registerAction( aAction, TrihedronShowId );
  }

  // Scale
  aAction = new QtxAction(tr("MNU_SCALING"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_SCALING" ) ),
                           tr( "MNU_SCALING" ), 0, this);
  aAction->setStatusTip(tr("DSC_SCALING"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onAxialScale()));
  toolMgr()->registerAction( aAction, AxialScaleId );

  // Graduated axes 
  aAction = new QtxAction(tr("MNU_GRADUATED_AXES"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_GRADUATED_AXES" ) ),
                           tr( "MNU_GRADUATED_AXES" ), 0, this);
  aAction->setStatusTip(tr("DSC_GRADUATED_AXES"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onGraduatedAxes()));
  toolMgr()->registerAction( aAction, GraduatedAxesId );

  // Active only ambient light or not
  aAction = new QtxAction(tr("MNU_AMBIENT"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_AMBIENT" ) ),
                           tr( "MNU_AMBIENT" ), 0, this);
  aAction->setStatusTip(tr("DSC_AMBIENT"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onAmbientToogle()));
  toolMgr()->registerAction( aAction, AmbientId );

  // Switch between interaction styles
  aAction = new QtxAction(tr("MNU_STYLE_SWITCH"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_STYLE_SWITCH" ) ),
                          tr( "MNU_STYLE_SWITCH" ), 0, this);
  aAction->setStatusTip(tr("DSC_STYLE_SWITCH"));
  aAction->setCheckable(true);
  connect(aAction, SIGNAL(toggled(bool)), this, SLOT(onSwitchInteractionStyle(bool)));
  toolMgr()->registerAction( aAction, SwitchInteractionStyleId );

  // Switch between zooming styles
  aAction = new QtxAction(tr("MNU_ZOOMING_STYLE_SWITCH"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_ZOOMING_STYLE_SWITCH" ) ),
                          tr( "MNU_ZOOMING_STYLE_SWITCH" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOMING_STYLE_SWITCH"));
  aAction->setCheckable(true);
  connect(aAction, SIGNAL(toggled(bool)), this, SLOT(onSwitchZoomingStyle(bool)));
  toolMgr()->registerAction( aAction, SwitchZoomingStyleId );

  // Maximized view
  aAction = new QtxAction(tr("MNU_MINIMIZE_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_MINIMIZE" ) ),
                          tr( "MNU_MINIMIZE_VIEW" ), 0, this );
  aAction->setStatusTip(tr("DSC_MINIMIZE_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onMaximizedView()));
  toolMgr()->registerAction( aAction, MaximizedId );

  // Synchronize View 
  toolMgr()->registerAction( synchronizeAction(), SynchronizeId );
}

/*!
  \brief Create toolbar.
*/
void OCCViewer_ViewWindow::createToolBar()
{
  QString aToolbarName;
  switch (my2dMode) {
  case XYPlane:
    aToolbarName = tr( "LBL_XYTOOLBAR_LABEL" );
    break;
  case XZPlane:
    aToolbarName = tr( "LBL_XZTOOLBAR_LABEL" );
    break;
  case YZPlane:
    aToolbarName = tr( "LBL_YZTOOLBAR_LABEL" );
    break;
  default:
    aToolbarName = tr( "LBL_3DTOOLBAR_LABEL" );
  }
  
  int tid = toolMgr()->createToolBar( aToolbarName, false );

  toolMgr()->append( DumpId, tid );
  toolMgr()->append( SwitchInteractionStyleId, tid );
#if OCC_VERSION_LARGE > 0x0603000A // available only with OCC-6.3-sp11 and higher version
  toolMgr()->append( SwitchZoomingStyleId, tid );
#endif
  if( myModel->trihedronActivated() )
    toolMgr()->append( TrihedronShowId, tid );

  QtxMultiAction* aScaleAction = new QtxMultiAction( this );
  aScaleAction->insertAction( toolMgr()->action( FitAllId ) );
  aScaleAction->insertAction( toolMgr()->action( FitRectId ) );
  aScaleAction->insertAction( toolMgr()->action( ZoomId ) );
  toolMgr()->append( aScaleAction, tid );

  QtxMultiAction* aPanningAction = new QtxMultiAction( this );
  aPanningAction->insertAction( toolMgr()->action( PanId ) );
  aPanningAction->insertAction( toolMgr()->action( GlobalPanId ) );
  toolMgr()->append( aPanningAction, tid );

  if (my2dMode == No2dMode) {
    toolMgr()->append( ChangeRotationPointId, tid );
    toolMgr()->append( RotationId, tid );

    QtxMultiAction* aViewsAction = new QtxMultiAction( this );
    aViewsAction->insertAction( toolMgr()->action( FrontId ) );
    aViewsAction->insertAction( toolMgr()->action( BackId ) );
    aViewsAction->insertAction( toolMgr()->action( TopId ) );
    aViewsAction->insertAction( toolMgr()->action( BottomId ) );
    aViewsAction->insertAction( toolMgr()->action( LeftId ) );
    aViewsAction->insertAction( toolMgr()->action( RightId ) );
    toolMgr()->append( aViewsAction, tid );

    toolMgr()->append( AntiClockWiseId, tid );
    toolMgr()->append( ClockWiseId, tid );

    toolMgr()->append( ResetId, tid );
  }

  QtxMultiAction* aMemAction = new QtxMultiAction( this );
  aMemAction->insertAction( toolMgr()->action( MemId ) );
  aMemAction->insertAction( toolMgr()->action( RestoreId ) );
  toolMgr()->append( aMemAction, tid );

  toolMgr()->append( toolMgr()->separator(), tid );
  toolMgr()->append( CloneId, tid );
  
  toolMgr()->append( toolMgr()->separator(), tid );
  toolMgr()->append( ClippingId, tid );
  toolMgr()->append( AxialScaleId, tid );
#if OCC_VERSION_LARGE > 0x06030009 // available only with OCC-6.3-sp10 and higher version
  toolMgr()->append( GraduatedAxesId, tid );
#endif
  toolMgr()->append( AmbientId, tid );

  toolMgr()->append( MaximizedId, tid );
  toolMgr()->append( SynchronizeId, tid );
}

/*!
  \brief Perform 'fit all' operation.
*/
void OCCViewer_ViewWindow::onViewFitAll()
{
  myViewPort->fitAll();
}

/*!
  \brief Perform "front view" transformation.
*/
void OCCViewer_ViewWindow::onFrontView()
{
  emit vpTransformationStarted ( FRONTVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Xpos);
  onViewFitAll();
  emit vpTransformationFinished ( FRONTVIEW );
}

/*!
  \brief Perform "back view" transformation.
*/
void OCCViewer_ViewWindow::onBackView()
{
  emit vpTransformationStarted ( BACKVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Xneg);
  onViewFitAll();
  emit vpTransformationFinished ( BACKVIEW );
}

/*!
  \brief Perform "top view" transformation.
*/
void OCCViewer_ViewWindow::onTopView()
{
  emit vpTransformationStarted ( TOPVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Zpos);
  onViewFitAll();
  emit vpTransformationFinished ( TOPVIEW );
}

/*!
  \brief Perform "bottom view" transformation.
*/
void OCCViewer_ViewWindow::onBottomView()
{
  emit vpTransformationStarted ( BOTTOMVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Zneg);
  onViewFitAll();
  emit vpTransformationFinished ( BOTTOMVIEW );
}

/*!
  \brief Perform "left view" transformation.
*/
void OCCViewer_ViewWindow::onLeftView()
{
  emit vpTransformationStarted ( LEFTVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Yneg);
  onViewFitAll();
  emit vpTransformationFinished ( LEFTVIEW );
}

/*!
  \brief Perform "right view" transformation.
*/
void OCCViewer_ViewWindow::onRightView()
{
  emit vpTransformationStarted ( RIGHTVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) aView3d->SetProj (V3d_Ypos);
  onViewFitAll();
  emit vpTransformationFinished ( RIGHTVIEW );
}

/*!
  \brief Rotate view 90 degrees clockwise
*/
void OCCViewer_ViewWindow::onClockWiseView()
{
  emit vpTransformationStarted ( CLOCKWISEVIEW );
  myViewPort->rotateXY( 90. );
  emit vpTransformationFinished ( CLOCKWISEVIEW );
}

/*!
  \brief Rotate view 90 degrees conterclockwise
*/
void OCCViewer_ViewWindow::onAntiClockWiseView()
{
  emit vpTransformationStarted ( ANTICLOCKWISEVIEW );
  myViewPort->rotateXY( -90. );
  emit vpTransformationFinished ( ANTICLOCKWISEVIEW );
}

/*!
  \brief Perform "reset view" transformation.

  Sets default orientation of the viewport camera.
*/
void OCCViewer_ViewWindow::onResetView()
{
  emit vpTransformationStarted( RESETVIEW );
  bool upd = myViewPort->getView()->SetImmediateUpdate( false );
  myViewPort->getView()->Reset( false );
  myViewPort->fitAll( false, true, false );
  myViewPort->getView()->SetImmediateUpdate( upd );
  myViewPort->getView()->Update();
  emit vpTransformationFinished( RESETVIEW );
}

/*!
  \brief Perform "fit all" transformation.
*/
void OCCViewer_ViewWindow::onFitAll()
{
  emit vpTransformationStarted( FITALLVIEW );
  myViewPort->fitAll();
  emit vpTransformationFinished( FITALLVIEW );
}

/*!
  \brief Called if 'change rotation point' operation is activated.
  \param on action state
*/
void OCCViewer_ViewWindow::onSetRotationPoint( bool on )
{
  if (on)
  {
    if (!mySetRotationPointDlg)
    {
      mySetRotationPointDlg = new OCCViewer_SetRotationPointDlg (this);
      mySetRotationPointDlg->SetAction(mySetRotationPointAction);
    }

    if (!mySetRotationPointDlg->isVisible())
    {
      //if (mySetRotationPointDlg->IsFirstShown())
      if (myCurrPointType == GRAVITY)
      {
        Standard_Real Xcenter, Ycenter, Zcenter;
        if (computeGravityCenter(Xcenter, Ycenter, Zcenter))
          mySetRotationPointDlg->setCoords(Xcenter, Ycenter, Zcenter);
      }
      mySetRotationPointDlg->show();
    }
  }
  else
  {
    if (mySetRotationPointDlg->isVisible())
      mySetRotationPointDlg->hide();
  }
}

/*!
   \brief Create one more window with same content.
*/
void OCCViewer_ViewWindow::onCloneView()
{
  SUIT_ViewWindow* vw = myManager->createViewWindow();
  //vw->show();
  emit viewCloned( vw );
}

/*!
  \brief called if clipping operation is activated.

  Enables/disables clipping plane displaying.

  \parma on action state
*/
void OCCViewer_ViewWindow::onClipping( bool on )
{
  /*
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  if ( on )
    myActionsMap[ ClippingId ]->setIcon(aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_CLIPPING_PRESSED" )));
  else
    myActionsMap[ ClippingId ]->setIcon(aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_CLIPPING" )));
  */
  OCCViewer_ViewWindow* aParent = dynamic_cast<OCCViewer_ViewWindow*>(parent()->parent());
  if (!aParent)
    aParent = this;
  if ( on )
    {
      if ( !myClippingDlg )
        {
          myClippingDlg = new OCCViewer_ClippingDlg( aParent );
          myClippingDlg->SetAction( myClippingAction );
        }
    
      if ( !myClippingDlg->isVisible() )
        myClippingDlg->show();
    }
  else
    {
      if ( myClippingDlg->isVisible() )
        myClippingDlg->hide();
      aParent->setCuttingPlane(false);
    }
}

/*!
  Creates one more window with same content
*/
void OCCViewer_ViewWindow::onAxialScale()
{
  if ( !myScalingDlg )
    myScalingDlg = new OCCViewer_AxialScaleDlg( this );
  
  if ( !myScalingDlg->isVisible() )
  {
    myScalingDlg->Update();
    myScalingDlg->show();
  }
}

/*!
  Shows Graduated Axes dialog
*/
void OCCViewer_ViewWindow::onGraduatedAxes()
{
  myCubeAxesDlg->Update();
  myCubeAxesDlg->show();
}

void OCCViewer_ViewWindow::onAmbientToogle()
{
  Handle(V3d_Viewer) viewer = myViewPort->getViewer();
  viewer->InitDefinedLights();
  while(viewer->MoreDefinedLights())
    {
      Handle(V3d_Light) light = viewer->DefinedLight();
      if(light->Type() != V3d_AMBIENT)
        {
          Handle(V3d_View) aView3d = myViewPort->getView();
          if( aView3d->IsActiveLight(light) ) viewer->SetLightOff(light);
          else viewer->SetLightOn(light);
        }
      viewer->NextDefinedLights();
    }
  viewer->Update();
}

/*!
  \brief Store view parameters.
*/
void OCCViewer_ViewWindow::onMemorizeView()
{
  appendViewAspect( getViewParams() );
}

/*!
  \brief Restore view parameters.
*/
void OCCViewer_ViewWindow::onRestoreView()
{
  OCCViewer_CreateRestoreViewDlg* aDlg = new OCCViewer_CreateRestoreViewDlg( centralWidget(), this );
  connect( aDlg, SIGNAL( dlgOk() ), this, SLOT( setRestoreFlag() ) );
  aDlg->exec();
  updateViewAspects( aDlg->parameters() );
  if( myRestoreFlag && aDlg->parameters().count() )
    performRestoring( aDlg->currentItem() );
}

/*!
  \brief Restore view parameters.
  \param anItem view parameters
*/
void OCCViewer_ViewWindow::performRestoring( const viewAspect& anItem, bool baseParamsOnly )
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
  aView3d->SetAxialScale( anItem.scaleX, anItem.scaleY, anItem.scaleZ );

  if ( !baseParamsOnly ) {

    myModel->setTrihedronShown( anItem.isVisible );
    myModel->setTrihedronSize( anItem.size );
        
#if OCC_VERSION_LARGE > 0x06030009 // available only with OCC-6.3-sp10 and higher version
    // graduated trihedron
    bool anIsVisible = anItem.gtIsVisible;
    OCCViewer_AxisWidget::AxisData anAxisData[3];
    anAxisData[0].DrawName = anItem.gtDrawNameX;
    anAxisData[1].DrawName = anItem.gtDrawNameZ;
    anAxisData[2].DrawName = anItem.gtDrawNameZ;
    anAxisData[0].Name = anItem.gtNameX;
    anAxisData[1].Name = anItem.gtNameZ;
    anAxisData[2].Name = anItem.gtNameZ;
    anAxisData[0].NameColor = QColor( anItem.gtNameColorRX,
				      anItem.gtNameColorGX,
				      anItem.gtNameColorBX );
    anAxisData[1].NameColor = QColor( anItem.gtNameColorRY,
				      anItem.gtNameColorGY,
				      anItem.gtNameColorBY );
    anAxisData[2].NameColor = QColor( anItem.gtNameColorRZ,
				      anItem.gtNameColorGZ,
				      anItem.gtNameColorBZ );
    anAxisData[0].DrawValues = anItem.gtDrawValuesX;
    anAxisData[1].DrawValues = anItem.gtDrawValuesY;
    anAxisData[2].DrawValues = anItem.gtDrawValuesZ;
    anAxisData[0].NbValues = anItem.gtNbValuesX;
    anAxisData[1].NbValues = anItem.gtNbValuesY;
    anAxisData[2].NbValues = anItem.gtNbValuesZ;
    anAxisData[0].Offset = anItem.gtOffsetX;
    anAxisData[1].Offset = anItem.gtOffsetY;
    anAxisData[2].Offset = anItem.gtOffsetZ;
    anAxisData[0].Color = QColor( anItem.gtColorRX,
				  anItem.gtColorGX,
				  anItem.gtColorBX );
    anAxisData[1].Color = QColor( anItem.gtColorRY,
				  anItem.gtColorGY,
				  anItem.gtColorBY );
    anAxisData[2].Color = QColor( anItem.gtColorRZ,
				  anItem.gtColorGZ,
				  anItem.gtColorBZ );
    anAxisData[0].DrawTickmarks = anItem.gtDrawTickmarksX;
    anAxisData[1].DrawTickmarks = anItem.gtDrawTickmarksY;
    anAxisData[2].DrawTickmarks = anItem.gtDrawTickmarksZ;
    anAxisData[0].TickmarkLength = anItem.gtTickmarkLengthX;
    anAxisData[1].TickmarkLength = anItem.gtTickmarkLengthY;
    anAxisData[2].TickmarkLength = anItem.gtTickmarkLengthZ;

    myCubeAxesDlg->SetData( anIsVisible, anAxisData );
    myCubeAxesDlg->ApplyData( aView3d );
#endif

  } // if ( !baseParamsOnly )

  myRestoreFlag = 0;
}

/*!
  \brief Set restore flag.
*/
void OCCViewer_ViewWindow::setRestoreFlag()
{
  myRestoreFlag = 1;
}

/*!
  \brief Called when action "show/hide trihedron" is activated.
*/
void OCCViewer_ViewWindow::onTrihedronShow()
{
  myModel->toggleTrihedron();
}

/*!
  \brief Switches "keyboard free" interaction style on/off
*/
void OCCViewer_ViewWindow::onSwitchInteractionStyle( bool on )
{
  myInteractionStyle = on ? (int)SUIT_ViewModel::KEY_FREE : (int)SUIT_ViewModel::STANDARD;

  // update action state if method is called outside
  QtxAction* a = dynamic_cast<QtxAction*>( toolMgr()->action( SwitchInteractionStyleId ) );
  if ( a->isChecked() != on )
    a->setChecked( on );
}

/*!
  \brief Toogles advanced zooming style (relatively to the cursor position) on/off
*/
void OCCViewer_ViewWindow::onSwitchZoomingStyle( bool on )
{
  myViewPort->setAdvancedZoomingEnabled( on );

  // update action state if method is called outside
  QtxAction* a = dynamic_cast<QtxAction*>( toolMgr()->action( SwitchZoomingStyleId ) );
  if ( a->isChecked() != on )
    a->setChecked( on );
}

/*!
  \brief Get current interaction style
  \return interaction style
*/
int OCCViewer_ViewWindow::interactionStyle() const
{
  return myInteractionStyle;
}

/*!
  \brief Set current interaction style
  \param theStyle interaction style
*/
void OCCViewer_ViewWindow::setInteractionStyle( const int theStyle )
{
  onSwitchInteractionStyle( theStyle == (int)SUIT_ViewModel::KEY_FREE );
}

/*!
  \brief Get current zooming style
  \return zooming style
*/
int OCCViewer_ViewWindow::zoomingStyle() const
{
  return myViewPort->isAdvancedZoomingEnabled() ? 1 : 0;
}

/*!
  \brief Set current zooming style
  \param theStyle zooming style
*/
void OCCViewer_ViewWindow::setZoomingStyle( const int theStyle )
{
  onSwitchZoomingStyle( theStyle == 1 );
}

/*!
  \brief Dump view window contents to the pixmap.
  \return pixmap containing all scene rendered in the window
*/
QImage OCCViewer_ViewWindow::dumpView()
{
  Handle(V3d_View) view = myViewPort->getView();
  if ( view.IsNull() )
    return QImage();
  
  int aWidth = myViewPort->width();
  int aHeight = myViewPort->height();
  QApplication::syncX();
  view->Redraw(); // In order to reactivate GL context
  //view->Update();

  OpenGLUtils_FrameBuffer aFrameBuffer;
  if( aFrameBuffer.init( aWidth, aHeight ) )
  {
    QImage anImage( aWidth, aHeight, QImage::Format_RGB32 );
   
    glPushAttrib( GL_VIEWPORT_BIT );
    glViewport( 0, 0, aWidth, aHeight );
    aFrameBuffer.bind();

    // draw scene
    view->Redraw();

    aFrameBuffer.unbind();
    glPopAttrib();

    aFrameBuffer.bind();
    glReadPixels( 0, 0, aWidth, aHeight, GL_RGBA, GL_UNSIGNED_BYTE, anImage.bits() );
    aFrameBuffer.unbind();

    anImage = anImage.rgbSwapped();
    anImage = anImage.mirrored();
    return anImage;
  }
  // if frame buffers are unsupported, use old functionality
  //view->Redraw();

  unsigned char* data = new unsigned char[ aWidth*aHeight*4 ];

  QPoint p = myViewPort->mapFromParent(myViewPort->geometry().topLeft());

  glReadPixels( p.x(), p.y(), aWidth, aHeight, GL_RGBA, GL_UNSIGNED_BYTE,
                data);

  QImage anImage( data, aWidth, aHeight, QImage::Format_ARGB32 );
  anImage = anImage.mirrored();
  anImage = anImage.rgbSwapped();
  return anImage;
}

bool OCCViewer_ViewWindow::dumpViewToFormat( const QImage& img, 
                                             const QString& fileName, 
                                             const QString& format )
{
  if ( format != "PS" && format != "EPS")
    return SUIT_ViewWindow::dumpViewToFormat( img, fileName, format );

  Handle(Visual3d_View) a3dView = myViewPort->getView()->View();

  if (format == "PS")
    a3dView->Export(strdup(qPrintable(fileName)), Graphic3d_EF_PostScript);
  else if (format == "EPS")
    a3dView->Export(strdup(qPrintable(fileName)), Graphic3d_EF_EnhPostScript);

  return true;
}


QString OCCViewer_ViewWindow::filter() const
{
  return tr( "OCC_IMAGE_FILES" );
}


/*!
  \brief Set parameters of the cutting plane
  \param on if \c true, cutting plane is enabled
  \param x X position of plane point
  \param y Y position of plane point
  \param z Z position of plane point
  \param dx X coordinate of plane normal
  \param dy Y coordinate of plane normal
  \param dz Z coordinate of plane normal
*/
void OCCViewer_ViewWindow::setCuttingPlane( bool on, const double x,  const double y,  const double z,
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

    // calculate new a,b,c,d values for the plane
    gp_Pln pln (gp_Pnt(x, y, z), gp_Dir(dx, dy, dz));
    double a, b, c, d;
    pln.Coefficients(a, b, c, d);

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
    if (view->MoreActivePlanes()) {
      clipPlane = view->ActivePlane();
      clipPlane->SetPlane(a, b, c, d);
    }
    else
      clipPlane = new V3d_Plane (a, b, c, d);
#else
    if (view->MoreActivePlanes())
      clipPlane = view->ActivePlane();
    else
      clipPlane = new V3d_Plane (viewer);

    clipPlane->SetPlane(a, b, c, d);
#endif

    view->SetPlaneOn(clipPlane);
  }
  else
    view->SetPlaneOff();

  view->Update();
  view->Redraw();
}

void OCCViewer_ViewWindow::setCuttingPlane( bool on, const gp_Pln pln )
{
  gp_Dir aDir = pln.Axis().Direction();
  gp_Pnt aPnt = pln.Location();
  setCuttingPlane(on, aPnt.X(), aPnt.Y(), aPnt.Z(), aDir.X(), aDir.Y(), aDir.Z());
}


/*!
  \brief Check if any cutting plane is enabled
  \return \c true if at least one cutting plane is enabled
*/
bool OCCViewer_ViewWindow::isCuttingPlane()
{
  Handle(V3d_View) view = myViewPort->getView();
  view->InitActivePlanes();
  return (view->MoreActivePlanes());
}

/*!
  \brief Get the visual parameters of the view window.
  \return visual parameters of view window
*/
viewAspect OCCViewer_ViewWindow::getViewParams() const
{
  double centerX, centerY, projX, projY, projZ, twist;
  double atX, atY, atZ, eyeX, eyeY, eyeZ;
  double aScaleX, aScaleY, aScaleZ;

  Handle(V3d_View) aView3d = myViewPort->getView();

  aView3d->Center( centerX, centerY );
  aView3d->Proj( projX, projY, projZ );
  aView3d->At( atX, atY, atZ );
  aView3d->Eye( eyeX, eyeY, eyeZ );
  twist = aView3d->Twist();

  aView3d->AxialScale(aScaleX,aScaleY,aScaleZ);

  bool isShown = myModel->isTrihedronVisible();
  double size = myModel->trihedronSize();

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
  params.scaleX   = aScaleX;
  params.scaleY   = aScaleY;
  params.scaleZ   = aScaleZ;
  params.name     = aName;
  params.isVisible= isShown;
  params.size     = size;

#if OCC_VERSION_LARGE > 0x06030009 // available only with OCC-6.3-sp10 and higher version
  // graduated trihedron
  bool anIsVisible = false;
  OCCViewer_AxisWidget::AxisData anAxisData[3];
  myCubeAxesDlg->GetData( anIsVisible, anAxisData );

  params.gtIsVisible = anIsVisible;
  params.gtDrawNameX = anAxisData[0].DrawName;
  params.gtDrawNameY = anAxisData[1].DrawName;
  params.gtDrawNameZ = anAxisData[2].DrawName;
  params.gtNameX = anAxisData[0].Name;
  params.gtNameY = anAxisData[1].Name;
  params.gtNameZ = anAxisData[2].Name;
  params.gtNameColorRX = anAxisData[0].NameColor.red();
  params.gtNameColorGX = anAxisData[0].NameColor.green();
  params.gtNameColorBX = anAxisData[0].NameColor.blue();
  params.gtNameColorRY = anAxisData[1].NameColor.red();
  params.gtNameColorGY = anAxisData[1].NameColor.green();
  params.gtNameColorBY = anAxisData[1].NameColor.blue();
  params.gtNameColorRZ = anAxisData[2].NameColor.red();
  params.gtNameColorGZ = anAxisData[2].NameColor.green();
  params.gtNameColorBZ = anAxisData[2].NameColor.blue();
  params.gtDrawValuesX = anAxisData[0].DrawValues;
  params.gtDrawValuesY = anAxisData[1].DrawValues;
  params.gtDrawValuesZ = anAxisData[2].DrawValues;
  params.gtNbValuesX = anAxisData[0].NbValues;
  params.gtNbValuesY = anAxisData[1].NbValues;
  params.gtNbValuesZ = anAxisData[2].NbValues;
  params.gtOffsetX = anAxisData[0].Offset;
  params.gtOffsetY = anAxisData[1].Offset;
  params.gtOffsetZ = anAxisData[2].Offset;
  params.gtColorRX = anAxisData[0].Color.red();
  params.gtColorGX = anAxisData[0].Color.green();
  params.gtColorBX = anAxisData[0].Color.blue();
  params.gtColorRY = anAxisData[1].Color.red();
  params.gtColorGY = anAxisData[1].Color.green();
  params.gtColorBY = anAxisData[1].Color.blue();
  params.gtColorRZ = anAxisData[2].Color.red();
  params.gtColorGZ = anAxisData[2].Color.green();
  params.gtColorBZ = anAxisData[2].Color.blue();
  params.gtDrawTickmarksX = anAxisData[0].DrawTickmarks;
  params.gtDrawTickmarksY = anAxisData[1].DrawTickmarks;
  params.gtDrawTickmarksZ = anAxisData[2].DrawTickmarks;
  params.gtTickmarkLengthX = anAxisData[0].TickmarkLength;
  params.gtTickmarkLengthY = anAxisData[1].TickmarkLength;
  params.gtTickmarkLengthZ = anAxisData[2].TickmarkLength;
#endif

  return params;
}


/*!
  \brief Get visual parameters of this view window.
  \return visual parameters of view window
*/
QString OCCViewer_ViewWindow::getVisualParameters()
{
  viewAspect params = getViewParams();

  QStringList data;

  data << QString( "scale=%1" )    .arg( params.scale,   0, 'e', 12 );
  data << QString( "centerX=%1" )  .arg( params.centerX, 0, 'e', 12 );
  data << QString( "centerY=%1" )  .arg( params.centerY, 0, 'e', 12 );
  data << QString( "projX=%1" )    .arg( params.projX,   0, 'e', 12 );
  data << QString( "projY=%1" )    .arg( params.projY,   0, 'e', 12 );
  data << QString( "projZ=%1" )    .arg( params.projZ,   0, 'e', 12 );
  data << QString( "twist=%1" )    .arg( params.twist,   0, 'e', 12 );
  data << QString( "atX=%1" )      .arg( params.atX,     0, 'e', 12 );
  data << QString( "atY=%1" )      .arg( params.atY,     0, 'e', 12 );
  data << QString( "atZ=%1" )      .arg( params.atZ,     0, 'e', 12 );
  data << QString( "eyeX=%1" )     .arg( params.eyeX,    0, 'e', 12 );
  data << QString( "eyeY=%1" )     .arg( params.eyeY,    0, 'e', 12 );
  data << QString( "eyeZ=%1" )     .arg( params.eyeZ,    0, 'e', 12 );
  data << QString( "scaleX=%1" )   .arg( params.scaleX,  0, 'e', 12 );
  data << QString( "scaleY=%1" )   .arg( params.scaleY,  0, 'e', 12 );
  data << QString( "scaleZ=%1" )   .arg( params.scaleZ,  0, 'e', 12 );
  data << QString( "isVisible=%1" ).arg( params.isVisible );
  data << QString( "size=%1" )     .arg( params.size,    0, 'f',  2 );

#if OCC_VERSION_LARGE > 0x06030009 // available only with OCC-6.3-sp10 or newer version
  // graduated trihedron
  data << QString( "gtIsVisible=%1" )      .arg( params.gtIsVisible );
  data << QString( "gtDrawNameX=%1" )      .arg( params.gtDrawNameX );
  data << QString( "gtDrawNameY=%1" )      .arg( params.gtDrawNameY );
  data << QString( "gtDrawNameZ=%1" )      .arg( params.gtDrawNameZ );
  data << QString( "gtNameX=%1" )          .arg( params.gtNameX );
  data << QString( "gtNameY=%1" )          .arg( params.gtNameY );
  data << QString( "gtNameZ=%1" )          .arg( params.gtNameZ );
  data << QString( "gtNameColorRX=%1" )    .arg( params.gtNameColorRX );
  data << QString( "gtNameColorGX=%1" )    .arg( params.gtNameColorGX );
  data << QString( "gtNameColorBX=%1" )    .arg( params.gtNameColorBX );
  data << QString( "gtNameColorRY=%1" )    .arg( params.gtNameColorRY );
  data << QString( "gtNameColorGY=%1" )    .arg( params.gtNameColorGY );
  data << QString( "gtNameColorBY=%1" )    .arg( params.gtNameColorBY );
  data << QString( "gtNameColorRZ=%1" )    .arg( params.gtNameColorRZ );
  data << QString( "gtNameColorGZ=%1" )    .arg( params.gtNameColorGZ );
  data << QString( "gtNameColorBZ=%1" )    .arg( params.gtNameColorBZ );
  data << QString( "gtDrawValuesX=%1" )    .arg( params.gtDrawValuesX );
  data << QString( "gtDrawValuesY=%1" )    .arg( params.gtDrawValuesY );
  data << QString( "gtDrawValuesZ=%1" )    .arg( params.gtDrawValuesZ );
  data << QString( "gtNbValuesX=%1" )      .arg( params.gtNbValuesX );
  data << QString( "gtNbValuesY=%1" )      .arg( params.gtNbValuesY );
  data << QString( "gtNbValuesZ=%1" )      .arg( params.gtNbValuesZ );
  data << QString( "gtOffsetX=%1" )        .arg( params.gtOffsetX );
  data << QString( "gtOffsetY=%1" )        .arg( params.gtOffsetY );
  data << QString( "gtOffsetZ=%1" )        .arg( params.gtOffsetZ );
  data << QString( "gtColorRX=%1" )        .arg( params.gtColorRX );
  data << QString( "gtColorGX=%1" )        .arg( params.gtColorGX );
  data << QString( "gtColorBX=%1" )        .arg( params.gtColorBX );
  data << QString( "gtColorRY=%1" )        .arg( params.gtColorRY );
  data << QString( "gtColorGY=%1" )        .arg( params.gtColorGY );
  data << QString( "gtColorBY=%1" )        .arg( params.gtColorBY );
  data << QString( "gtColorRZ=%1" )        .arg( params.gtColorRZ );
  data << QString( "gtColorGZ=%1" )        .arg( params.gtColorGZ );
  data << QString( "gtColorBZ=%1" )        .arg( params.gtColorBZ );
  data << QString( "gtDrawTickmarksX=%1" ) .arg( params.gtDrawTickmarksX );
  data << QString( "gtDrawTickmarksY=%1" ) .arg( params.gtDrawTickmarksY );
  data << QString( "gtDrawTickmarksZ=%1" ) .arg( params.gtDrawTickmarksZ );
  data << QString( "gtTickmarkLengthX=%1" ).arg( params.gtTickmarkLengthX );
  data << QString( "gtTickmarkLengthY=%1" ).arg( params.gtTickmarkLengthY );
  data << QString( "gtTickmarkLengthZ=%1" ).arg( params.gtTickmarkLengthZ );
#endif
  QString bg = Qtx::backgroundToString( background() ).replace( "=", "$" );
  data << QString( "background=%1" ).arg( bg );

  return data.join("*");
}

/*!
  \brief Restore visual parameters of the view window.
  \param parameters visual parameters of view window
*/
void OCCViewer_ViewWindow::setVisualParameters( const QString& parameters )
{
  viewAspect params;

  QStringList data = parameters.split( '*' );
  Qtx::BackgroundData bgData;
  if ( parameters.contains( '=' )  ) // new format - "scale=1.000e+00*centerX=0.000e+00..."
  {
    foreach( QString param, data ) {
      QString paramName  = param.section( '=', 0, 0 ).trimmed();
      QString paramValue = param.section( '=', 1, 1 ).trimmed();
      if      ( paramName == "scale" )             params.scale             = paramValue.toDouble();
      else if ( paramName == "centerX" )           params.centerX           = paramValue.toDouble();
      else if ( paramName == "centerY" )           params.centerY           = paramValue.toDouble();
      else if ( paramName == "projX" )             params.projX             = paramValue.toDouble();
      else if ( paramName == "projY" )             params.projY             = paramValue.toDouble();
      else if ( paramName == "projZ" )             params.projZ             = paramValue.toDouble();
      else if ( paramName == "twist" )             params.twist             = paramValue.toDouble();
      else if ( paramName == "atX" )               params.atX               = paramValue.toDouble();
      else if ( paramName == "atY" )               params.atY               = paramValue.toDouble();
      else if ( paramName == "atZ" )               params.atZ               = paramValue.toDouble();
      else if ( paramName == "eyeX" )              params.eyeX              = paramValue.toDouble();
      else if ( paramName == "eyeY" )              params.eyeY              = paramValue.toDouble();
      else if ( paramName == "eyeZ" )              params.eyeZ              = paramValue.toDouble();
      else if ( paramName == "scaleX" )            params.scaleX            = paramValue.toDouble();
      else if ( paramName == "scaleY" )            params.scaleY            = paramValue.toDouble();
      else if ( paramName == "scaleZ" )            params.scaleZ            = paramValue.toDouble();
      else if ( paramName == "isVisible" )         params.isVisible         = paramValue.toInt();
      else if ( paramName == "size" )              params.size              = paramValue.toDouble();
      // graduated trihedron
      else if ( paramName == "gtIsVisible" )       params.gtIsVisible       = paramValue.toInt();
      else if ( paramName == "gtDrawNameX" )       params.gtDrawNameX       = paramValue.toInt();
      else if ( paramName == "gtDrawNameY" )       params.gtDrawNameY       = paramValue.toInt();
      else if ( paramName == "gtDrawNameZ" )       params.gtDrawNameZ       = paramValue.toInt();
      else if ( paramName == "gtNameX" )           params.gtNameX           = paramValue;
      else if ( paramName == "gtNameY" )           params.gtNameY           = paramValue;
      else if ( paramName == "gtNameZ" )           params.gtNameZ           = paramValue;
      else if ( paramName == "gtNameColorRX" )     params.gtNameColorRX     = paramValue.toInt();
      else if ( paramName == "gtNameColorGX" )     params.gtNameColorGX     = paramValue.toInt();
      else if ( paramName == "gtNameColorBX" )     params.gtNameColorBX     = paramValue.toInt();
      else if ( paramName == "gtNameColorRY" )     params.gtNameColorRY     = paramValue.toInt();
      else if ( paramName == "gtNameColorGY" )     params.gtNameColorGY     = paramValue.toInt();
      else if ( paramName == "gtNameColorBY" )     params.gtNameColorBY     = paramValue.toInt();
      else if ( paramName == "gtNameColorRZ" )     params.gtNameColorRZ     = paramValue.toInt();
      else if ( paramName == "gtNameColorGZ" )     params.gtNameColorGZ     = paramValue.toInt();
      else if ( paramName == "gtNameColorBZ" )     params.gtNameColorBZ     = paramValue.toInt();
      else if ( paramName == "gtDrawValuesX" )     params.gtDrawValuesX     = paramValue.toInt();
      else if ( paramName == "gtDrawValuesY" )     params.gtDrawValuesY     = paramValue.toInt();
      else if ( paramName == "gtDrawValuesZ" )     params.gtDrawValuesZ     = paramValue.toInt();
      else if ( paramName == "gtNbValuesX" )       params.gtNbValuesX       = paramValue.toInt();
      else if ( paramName == "gtNbValuesY" )       params.gtNbValuesY       = paramValue.toInt();
      else if ( paramName == "gtNbValuesZ" )       params.gtNbValuesZ       = paramValue.toInt();
      else if ( paramName == "gtOffsetX" )         params.gtOffsetX         = paramValue.toInt();
      else if ( paramName == "gtOffsetY" )         params.gtOffsetY         = paramValue.toInt();
      else if ( paramName == "gtOffsetZ" )         params.gtOffsetZ         = paramValue.toInt();
      else if ( paramName == "gtColorRX" )         params.gtColorRX         = paramValue.toInt();
      else if ( paramName == "gtColorGX" )         params.gtColorGX         = paramValue.toInt();
      else if ( paramName == "gtColorBX" )         params.gtColorBX         = paramValue.toInt();
      else if ( paramName == "gtColorRY" )         params.gtColorRY         = paramValue.toInt();
      else if ( paramName == "gtColorGY" )         params.gtColorGY         = paramValue.toInt();
      else if ( paramName == "gtColorBY" )         params.gtColorBY         = paramValue.toInt();
      else if ( paramName == "gtColorRZ" )         params.gtColorRZ         = paramValue.toInt();
      else if ( paramName == "gtColorGZ" )         params.gtColorGZ         = paramValue.toInt();
      else if ( paramName == "gtColorBZ" )         params.gtColorBZ         = paramValue.toInt();
      else if ( paramName == "gtDrawTickmarksX" )  params.gtDrawTickmarksX  = paramValue.toInt();
      else if ( paramName == "gtDrawTickmarksY" )  params.gtDrawTickmarksY  = paramValue.toInt();
      else if ( paramName == "gtDrawTickmarksZ" )  params.gtDrawTickmarksZ  = paramValue.toInt();
      else if ( paramName == "gtTickmarkLengthX" ) params.gtTickmarkLengthX = paramValue.toInt();
      else if ( paramName == "gtTickmarkLengthY" ) params.gtTickmarkLengthY = paramValue.toInt();
      else if ( paramName == "gtTickmarkLengthZ" ) params.gtTickmarkLengthZ = paramValue.toInt();
      else if ( paramName == "background" )        {
	QString bg = paramValue.replace( "$", "=" );
	bgData = Qtx::stringToBackground( bg );
      }
    }
  }
  else // old format - "1.000e+00*0.000e+00..."
  {
    int idx = 0;
    params.scale     = data.count() > idx ? data[idx++].toDouble() : 1.0;
    params.centerX   = data.count() > idx ? data[idx++].toDouble() : 0.0;
    params.centerY   = data.count() > idx ? data[idx++].toDouble() : 0.0;
    params.projX     = data.count() > idx ? data[idx++].toDouble() : sqrt(1./3);
    params.projY     = data.count() > idx ? data[idx++].toDouble() : -sqrt(1./3);
    params.projZ     = data.count() > idx ? data[idx++].toDouble() : sqrt(1./3);
    params.twist     = data.count() > idx ? data[idx++].toDouble() : 0.0;
    params.atX       = data.count() > idx ? data[idx++].toDouble() : 0.0;
    params.atY       = data.count() > idx ? data[idx++].toDouble() : 0.0;
    params.atZ       = data.count() > idx ? data[idx++].toDouble() : 0.0;
    params.eyeX      = data.count() > idx ? data[idx++].toDouble() : sqrt(250000./3);
    params.eyeY      = data.count() > idx ? data[idx++].toDouble() : -sqrt(250000./3);
    params.eyeZ      = data.count() > idx ? data[idx++].toDouble() : sqrt(250000./3);
    params.scaleX    = data.count() > idx ? data[idx++].toDouble() : 1.0;
    params.scaleY    = data.count() > idx ? data[idx++].toDouble() : 1.0;
    params.scaleZ    = data.count() > idx ? data[idx++].toDouble() : 1.0;
    params.isVisible = data.count() > idx ? data[idx++].toInt()    : 1;
    params.size      = data.count() > idx ? data[idx++].toDouble() : 100.0;
  }
  performRestoring( params );
  setBackground( bgData );
}

/*!
  \brief Handle show event.

  Emits Show() signal.

  \param theEvent show event
*/
void OCCViewer_ViewWindow::showEvent( QShowEvent* theEvent )
{
  emit Show( theEvent );
}

/*!
  \brief Handle hide event.

  Emits Hide() signal.

  \param theEvent hide event
*/
void OCCViewer_ViewWindow::hideEvent( QHideEvent* theEvent )
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
  QList<OCCViewer_ViewSketcher*>::Iterator it;
  for ( it = mySketchers.begin(); it != mySketchers.end() && !sketcher; ++it )
  {
    OCCViewer_ViewSketcher* sk = (*it);
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
  MESSAGE("OCCViewer_ViewWindow::onSketchingFinished()")
  if ( mypSketcher && mypSketcher->result() == OCCViewer_ViewSketcher::Accept )
  {
    Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
    bool append = bool( mypSketcher->buttonState() & Qt::ShiftModifier );
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
//           myRect = aRect;

          if( append )
            ic->ShiftSelect( aLeft, aBottom, aRight, aTop, getViewPort()->getView(), Standard_False );
          else
            ic->Select( aLeft, aBottom, aRight, aTop, getViewPort()->getView(), Standard_False );
        }
      }
      break;
    case Polygon:
      {
        QPolygon* aPolygon = (QPolygon*)mypSketcher->data();
        if( aPolygon )
        {
          int size = aPolygon->size();
          TColgp_Array1OfPnt2d anArray( 1, size );

          QPolygon::Iterator it = aPolygon->begin();
          QPolygon::Iterator itEnd = aPolygon->end();
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

OCCViewer_ViewPort3d* OCCViewer_ViewWindow::getViewPort()
{
  return myViewPort;
}

bool OCCViewer_ViewWindow::transformRequested() const
{
  return ( myOperation != NOTHING );
}

bool OCCViewer_ViewWindow::transformInProcess() const
{
  return myEventStarted;
}

void OCCViewer_ViewWindow::setTransformInProcess( bool bOn )
{
  myEventStarted = bOn;
}

/*!
  Set enabled state of transformation (rotate, zoom, etc)
*/
void OCCViewer_ViewWindow::setTransformEnabled( const OperationType id, const bool on )
{
  if ( id != NOTHING ) myStatus.insert( id, on );
}

/*!
  \return enabled state of transformation (rotate, zoom, etc)
*/
bool OCCViewer_ViewWindow::transformEnabled( const OperationType id ) const
{
  return myStatus.contains( id ) ? myStatus[ id ] : true;
}

void OCCViewer_ViewWindow::onMaximizedView()
{
  setMaximized(!isMaximized());
}


void OCCViewer_ViewWindow::setMaximized(bool toMaximize, bool toSendSignal)
{
  QAction* anAction =  toolMgr()->action( MaximizedId );
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  if ( toMaximize ) {
    anAction->setText( tr( "MNU_MINIMIZE_VIEW" ) );  
    anAction->setToolTip( tr( "MNU_MINIMIZE_VIEW" ) );  
    anAction->setIcon( aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_MINIMIZE" ) ) );
    anAction->setStatusTip( tr( "DSC_MINIMIZE_VIEW" ) );
    if (toSendSignal) {
      emit maximized( this, true );
    }
  }
  else {
    anAction->setText( tr( "MNU_MAXIMIZE_VIEW" ) );  
    anAction->setToolTip( tr( "MNU_MAXIMIZE_VIEW" ) );  
    anAction->setIcon( aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_MAXIMIZE" ) ) );
    anAction->setStatusTip( tr( "DSC_MAXIMIZE_VIEW" ) );
    if (toSendSignal) {
      emit maximized( this, false );
    }
  }
}


bool OCCViewer_ViewWindow::isMaximized() const
{
  return !(toolMgr()->action( MaximizedId )->text() == tr( "MNU_MAXIMIZE_VIEW" ));
}

void OCCViewer_ViewWindow::setSketcherStyle( bool enable )
{ 
  IsSketcherStyle = enable; 
}

bool OCCViewer_ViewWindow::isSketcherStyle() const 
{ 
  return IsSketcherStyle; 
}


void OCCViewer_ViewWindow::set2dMode(Mode2dType theType)
{
  my2dMode = theType;
}

// obsolete   
QColor OCCViewer_ViewWindow::backgroundColor() const
{
  return myViewPort ? myViewPort->backgroundColor() : Qt::black;
}
   
// obsolete
void OCCViewer_ViewWindow::setBackgroundColor( const QColor& theColor )
{
  if ( myViewPort ) myViewPort->setBackgroundColor( theColor );
}

Qtx::BackgroundData OCCViewer_ViewWindow::background() const
{
  return myViewPort ? myViewPort->background() : Qtx::BackgroundData();
}
   
void OCCViewer_ViewWindow::setBackground( const Qtx::BackgroundData& theBackground )
{
  if ( myViewPort ) myViewPort->setBackground( theBackground );
}

/*!
  Clears view aspects
*/
void OCCViewer_ViewWindow::clearViewAspects()
{
  myViewAspects.clear();
}

/*!
  \return const reference to list of view aspects
*/
const viewAspectList& OCCViewer_ViewWindow::getViewAspects()
{
  return myViewAspects;
}

/*!
  Appends new view aspect
  \param aParams - new view aspects
*/
void OCCViewer_ViewWindow::appendViewAspect( const viewAspect& aParams )
{
  myViewAspects.append( aParams );
}

/*!
  Replaces old view aspects by new ones
  \param aViewList - list of new view aspects
*/
void OCCViewer_ViewWindow::updateViewAspects( const viewAspectList& aViewList )
{
  myViewAspects = aViewList;
}

/*!
  Get camera properties for the OCC view window.
  \return shared pointer on camera properties.
*/
SUIT_CameraProperties OCCViewer_ViewWindow::cameraProperties()
{
  SUIT_CameraProperties aProps;

  Handle(V3d_View) aSourceView = getViewPort()->getView();
  if ( aSourceView.IsNull() )
    return aProps;

  if ( get2dMode() == No2dMode ) {
    aProps.setDimension( SUIT_CameraProperties::Dim3D );
  }
  else {
    aProps.setDimension( SUIT_CameraProperties::Dim2D );
    aProps.setViewSide( (SUIT_CameraProperties::ViewSide)(int)get2dMode() );
  }
  
  // read common properites of the view
  Standard_Real anUpDir[3];
  Standard_Real aPrjDir[3];
  Standard_Real aMapScale[2];
  Standard_Real aTranslation[3];
  Standard_Real anAxialScale[3];
  
  aSourceView->Up(anUpDir[0], anUpDir[1], anUpDir[2]);
  aSourceView->Proj(aPrjDir[0], aPrjDir[1], aPrjDir[2]);
  aSourceView->At(aTranslation[0], aTranslation[1], aTranslation[2]);
  aSourceView->Size(aMapScale[0], aMapScale[1]);

  getViewPort()->getAxialScale(anAxialScale[0], anAxialScale[1], anAxialScale[2]);

  // we use similar depth to the one used in perspective projection 
  // to proivde a convinience synchronization with other camera views that
  // can switch between orthogonal & perspective projection. otherwise,
  // the camera will get to close when switching from orthogonal to perspective.
  Standard_Real aCameraDepth = aSourceView->Depth() + aSourceView->ZSize() * 0.5;

  // store common props
  aProps.setViewUp(anUpDir[0], anUpDir[1], anUpDir[2]);
  aProps.setMappingScale(aMapScale[1] / 2.0);
  aProps.setAxialScale(anAxialScale[0], anAxialScale[1], anAxialScale[2]);
  
  // generate view orientation matrix for transforming OCC projection reference point
  // into a camera (eye) position.
  gp_Dir aLeftDir = gp_Dir(anUpDir[0], anUpDir[1], anUpDir[2]).Crossed(
    gp_Dir(aPrjDir[0], aPrjDir[1], aPrjDir[2]));

  gp_Trsf aTrsf;
  aTrsf.SetValues( aLeftDir.X(), anUpDir[0], aPrjDir[0], aTranslation[0],
                   aLeftDir.Y(), anUpDir[1], aPrjDir[1], aTranslation[1],
                   aLeftDir.Z(), anUpDir[2], aPrjDir[2], aTranslation[2],
                   Precision::Confusion(),
                   Precision::Confusion() );

  // get projection reference point in view coordinates
  Graphic3d_Vertex aProjRef = aSourceView->ViewMapping().ProjectionReferencePoint();
  
  // transform to world-space coordinate system
  gp_Pnt aPosition = gp_Pnt(aProjRef.X(), aProjRef.Y(), aCameraDepth).Transformed(aTrsf);
  
  // compute focal point
  double aFocalPoint[3];

  aFocalPoint[0] = aPosition.X() - aPrjDir[0] * aCameraDepth;
  aFocalPoint[1] = aPosition.Y() - aPrjDir[1] * aCameraDepth;
  aFocalPoint[2] = aPosition.Z() - aPrjDir[2] * aCameraDepth;

  aProps.setFocalPoint(aFocalPoint[0], aFocalPoint[1], aFocalPoint[2]);
  aProps.setPosition(aPosition.X(), aPosition.Y(), aPosition.Z());

  return aProps;
}

/*!
  Synchronize views.
  This implementation synchronizes OCC view's camera propreties.
*/
void OCCViewer_ViewWindow::synchronize( SUIT_ViewWindow* theView )
{
  bool blocked = blockSignals( true );

  SUIT_CameraProperties aProps = theView->cameraProperties();
  if ( !cameraProperties().isCompatible( aProps ) ) {
    // other view, this one is being currently synchronized to, seems has become incompatible
    // we have to break synchronization
    updateSyncViews();
    return;
  }

  Handle(V3d_View) aDestView = getViewPort()->getView();

  aDestView->SetImmediateUpdate( Standard_False );

  double anUpDir[3];
  double aPosition[3];
  double aFocalPoint[3];
  double aMapScaling;
  double anAxialScale[3];

  // get common properties
  aProps.getFocalPoint(aFocalPoint[0], aFocalPoint[1], aFocalPoint[2]);
  aProps.getPosition(aPosition[0], aPosition[1], aPosition[2]);
  aProps.getViewUp(anUpDir[0], anUpDir[1], anUpDir[2]);
  aProps.getAxialScale(anAxialScale[0], anAxialScale[1], anAxialScale[2]);
  aMapScaling = aProps.getMappingScale() * 2.0;

  gp_Dir aProjDir(aPosition[0] - aFocalPoint[0],
                  aPosition[1] - aFocalPoint[1],
                  aPosition[2] - aFocalPoint[2]);
  
  // get custom view translation
  Standard_Real aTranslation[3];
  aDestView->At(aTranslation[0], aTranslation[1], aTranslation[2]);

  gp_Dir aLeftDir = gp_Dir(anUpDir[0], anUpDir[1], anUpDir[2]).Crossed(
    gp_Dir(aProjDir.X(), aProjDir.Y(), aProjDir.Z()));

  // convert camera position into a view reference point
  gp_Trsf aTrsf;
  aTrsf.SetValues( aLeftDir.X(), anUpDir[0], aProjDir.X(), aTranslation[0],
                   aLeftDir.Y(), anUpDir[1], aProjDir.Y(), aTranslation[1],
                   aLeftDir.Z(), anUpDir[2], aProjDir.Z(), aTranslation[2], 
                   Precision::Confusion(),
                   Precision::Confusion() );
  aTrsf.Invert();

  // transform to view-space coordinate system
  gp_Pnt aProjRef(aPosition[0], aPosition[1], aPosition[2]);
  aProjRef.Transform(aTrsf);

  // set view camera properties using low-level approach. this is done
  // in order to avoid interference with static variables in v3d view used
  // when rotation is in process in another view.
  Visual3d_ViewMapping aMapping = aDestView->View()->ViewMapping();
  Visual3d_ViewOrientation anOrientation = aDestView->View()->ViewOrientation();

  Graphic3d_Vector aMappingProj(aProjDir.X(), aProjDir.Y(), aProjDir.Z());
  Graphic3d_Vector aMappingUp(anUpDir[0], anUpDir[1], anUpDir[2]);

  aMappingProj.Normalize();
  aMappingUp.Normalize();

  anOrientation.SetViewReferencePlane(aMappingProj);
  anOrientation.SetViewReferenceUp(aMappingUp);

  aDestView->SetViewMapping(aMapping);
  aDestView->SetViewOrientation(anOrientation);

  // set panning
  aDestView->SetCenter(aProjRef.X(), aProjRef.Y());

  // set mapping scale
  Standard_Real aWidth, aHeight;
  aDestView->Size(aWidth, aHeight);
  
  if ( aWidth > aHeight )
    aDestView->SetSize (aMapScaling * (aWidth / aHeight));
  else
    aDestView->SetSize (aMapScaling);

  getViewPort()->setAxialScale(anAxialScale[0], anAxialScale[1], anAxialScale[2]);

  aDestView->ZFitAll();
  aDestView->SetImmediateUpdate( Standard_True );
  aDestView->Redraw();

  blockSignals( blocked );
}
