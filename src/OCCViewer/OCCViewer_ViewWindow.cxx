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

// File   : OCCViewer_ViewWindow.cxx
// Author :

#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewManager.h"
#include "OCCViewer_ViewSketcher.h"
#include "OCCViewer_CreateRestoreViewDlg.h"
#include "OCCViewer_ClipPlane.h"
#include "OCCViewer_SetRotationPointDlg.h"
#include "OCCViewer_AxialScaleDlg.h"
#include "OCCViewer_CubeAxesDlg.h"
#include "OCCViewer_ClippingDlg.h"
#include "OCCViewer_RayTracingDlg.h"
#include "OCCViewer_EnvTextureDlg.h"
#include "OCCViewer_LightSourceDlg.h"
#include "OCCViewer_Utilities.h"

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

#include <Basics_OCCTVersion.hxx>

#include <QPainter>
#include <QTime>
#include <QImage>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QActionGroup>
#include <QMenu>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Shape.hxx>

#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS.hxx>

#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGLUtils_FrameBuffer.h>

#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_ExportFormat.hxx>
#if OCC_VERSION_LARGE > 0x06090000
#include <Graphic3d_StereoMode.hxx>
#include <Graphic3d_RenderingParams.hxx>
#endif

#if OCC_VERSION_MAJOR < 7
  #include <Visual3d_View.hxx>
#endif

#include <V3d_Plane.hxx>
#include <V3d_Light.hxx>

#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_GTrsf.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <Image_PixMap.hxx>

#include <Standard_Version.hxx>

#include "utilities.h"

// // OpenCV includes
// #include <cv.h>
// #include <highgui.h>

static QEvent* l_mbPressEvent = 0;

//#ifdef WIN32
//# include <QWindowsStyle>
//#endif

#include <GL/gl.h>

// To avoid conflict between KeyPress from the X.h (define KeyPress 2)
// and QEvent::KeyPress (qevent.h)
#ifdef KeyPress
#undef KeyPress
#endif

// Enable ray tracing features
#define ENABLE_RAY_TRACING

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
  myDrawRectEnabled = true;
  myDrawRect=false;
  updateEnabledDrawMode();
  myScalingDlg = 0;
  mySetRotationPointDlg = 0;
  myRectBand = 0;

  IsSketcherStyle = false;
  myIsKeyFree = false;

  mypSketcher = 0;
  myCurSketch = -1;
  my2dMode = No2dMode;

  myInteractionStyle = SUIT_ViewModel::STANDARD;
  myPreselectionEnabled = true;
  mySelectionEnabled = true;

  myCursorIsHand = false;

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

  myCurrPointType = BBCENTER;
  myPrevPointType = BBCENTER;
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

        if ( aEvent->modifiers().testFlag(Qt::ControlModifier) ) {
          Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
          if ( isPreselectionEnabled() && ic->HasOpenedContext() ) {
            if ( aEvent->delta() > 0 ) {
              ic->HilightNextDetected( myViewPort->getView() );
            } else {
              ic->HilightPreviousDetected( myViewPort->getView() );
            }
          }
        }
        else {
          emit vpTransformationStarted ( ZOOMVIEW );
          myViewPort->startZoomAtPoint( aEvent->x(), aEvent->y() );
          double delta = (double)( aEvent->delta() ) / ( 15 * 8 );
          int x  = aEvent->x();
          int y  = aEvent->y();
          int x1 = (int)( aEvent->x() + width()*delta/100 );
          int y1 = (int)( aEvent->y() + height()*delta/100 );
          myViewPort->zoom( x, y, x1, y1 );
          myViewPort->getView()->ZFitAll();
          emit vpTransformationFinished ( ZOOMVIEW );
        }
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
  \brief Enable / disable draw rect (rubber band) mode
*/
bool OCCViewer_ViewWindow::enableDrawMode( bool on )
{
  bool prev = myDrawRectEnabled;
  myDrawRectEnabled = on;
  updateEnabledDrawMode();
  return prev;
}

/*!
  \brief Update state of enable draw mode state.
*/
void OCCViewer_ViewWindow::updateEnabledDrawMode()
{
  myEnableDrawMode = myDrawRectEnabled;
  if ( myModel )
    myEnableDrawMode = myEnableDrawMode && myModel->isSelectionEnabled() && myModel->isMultiSelectionEnabled();
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
    OperationType aState;
    if ( interactionStyle() == SUIT_ViewModel::STANDARD )
      aState = getButtonState(theEvent, anInteractionStyle);
    else {
      aState = OCCViewer_ViewWindow::NOTHING;
      myIsKeyFree = true;
    }
    switch ( aState ) {
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
            GProp_GProps aSystem;
            gp_Pnt aPnt;
            if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
            {
              aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
            }
            else if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_EDGE )
            {
              BRepGProp::LinearProperties( aShape, aSystem );
              aPnt = aSystem.CentreOfMass();
            }
            else if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_FACE )
            {
              BRepGProp::SurfaceProperties( aShape, aSystem );
              aPnt = aSystem.CentreOfMass();
            }
            else if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_SOLID )
            {
              BRepGProp::VolumeProperties( aShape, aSystem );
              aPnt = aSystem.CentreOfMass();
            }
            else
            {
              myCurrPointType = myPrevPointType;
              break;
            }

            if ( mySetRotationPointDlg )
            {
              myRotationPointSelection = false;
              mySetRotationPointDlg->setCoords(aPnt.X(), aPnt.Y(), aPnt.Z());
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
    saveCursor();                /* save old cursor */

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
    saveCursor();                // save old cursor

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
    saveCursor();                // save old cursor

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
  Handle(V3d_View) aView3d = myViewPort->getView();

  // Project boundaries points and add to avergae gravity
  // the ones which lie within the screen limits
  Standard_Real aScreenLimits[4] = { 0.0, 0.0, 0.0, 0.0 };

#if OCC_VERSION_LARGE > 0x06070100
  // NDC space screen limits
  aScreenLimits[0] = -1.0;
  aScreenLimits[1] =  1.0;
  aScreenLimits[2] = -1.0;
  aScreenLimits[3] =  1.0;
#else
  aView3d->View()->ViewMapping().WindowLimit( aScreenLimits[0],
                                              aScreenLimits[1],
                                              aScreenLimits[2],
                                              aScreenLimits[3] );
#endif

  Standard_Integer aPointsNb = 0;

  Standard_Real aXmin = 0.0;
  Standard_Real aYmin = 0.0;
  Standard_Real aZmin = 0.0;
  Standard_Real aXmax = 0.0;
  Standard_Real aYmax = 0.0;
  Standard_Real aZmax = 0.0;

  Graphic3d_MapOfStructure aSetOfStructures;
  aView3d->View()->DisplayedStructures( aSetOfStructures );
  Graphic3d_MapIteratorOfMapOfStructure aStructureIt( aSetOfStructures );

  for( ; aStructureIt.More(); aStructureIt.Next() ) {
    const Handle(Graphic3d_Structure)& aStructure = aStructureIt.Key();
    if ( aStructure->IsEmpty() || !aStructure->IsVisible() || aStructure->CStructure()->IsForHighlight )
      continue;

#if OCC_VERSION_LARGE > 0x06070100
    Bnd_Box aBox = aStructure->MinMaxValues();
    aXmin = aBox.IsVoid() ? RealFirst() : aBox.CornerMin().X();
    aYmin = aBox.IsVoid() ? RealFirst() : aBox.CornerMin().Y();
    aZmin = aBox.IsVoid() ? RealFirst() : aBox.CornerMin().Z();
    aXmax = aBox.IsVoid() ? RealLast()  : aBox.CornerMax().X();
    aYmax = aBox.IsVoid() ? RealLast()  : aBox.CornerMax().Y();
    aZmax = aBox.IsVoid() ? RealLast()  : aBox.CornerMax().Z();
#else
    aStructure->MinMaxValues( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );
#endif

    // Infinite structures are skipped
    Standard_Real aLIM = ShortRealLast() - 1.0;
    if ( Abs( aXmin ) > aLIM || Abs( aYmin ) > aLIM || Abs( aZmin ) > aLIM
      || Abs( aXmax ) > aLIM || Abs( aYmax ) > aLIM || Abs( aZmax ) > aLIM ) {
      continue;
    }

    gp_Pnt aPoints[8] = {
      gp_Pnt( aXmin, aYmin, aZmin ), gp_Pnt( aXmin, aYmin, aZmax ),
      gp_Pnt( aXmin, aYmax, aZmin ), gp_Pnt( aXmin, aYmax, aZmax ),
      gp_Pnt( aXmax, aYmin, aZmin ), gp_Pnt( aXmax, aYmin, aZmax ),
      gp_Pnt( aXmax, aYmax, aZmin ), gp_Pnt( aXmax, aYmax, aZmax )
    };

    for ( Standard_Integer aPointIt = 0; aPointIt < 8; ++aPointIt ) {
      const gp_Pnt& aBBPoint = aPoints[aPointIt];

#if OCC_VERSION_LARGE > 0x06070100
      gp_Pnt aProjected = aView3d->Camera()->Project( aBBPoint );
      const Standard_Real& U = aProjected.X();
      const Standard_Real& V = aProjected.Y();
#else
      Standard_Real U = 0.0;
      Standard_Real V = 0.0;
      Standard_Real W = 0.0;
      aView3d->View()->Projects( aBBPoint.X(), aBBPoint.Y(), aBBPoint.Z(), U, V, W );
#endif

      if (U >= aScreenLimits[0]
       && U <= aScreenLimits[1]
       && V >= aScreenLimits[2]
       && V <= aScreenLimits[3])
      {
        aPointsNb++;
        theX += aBBPoint.X();
        theY += aBBPoint.Y();
        theZ += aBBPoint.Z();
      }
    }
  }

  if ( aPointsNb > 0 )
  {
    theX /= aPointsNb;
    theY /= aPointsNb;
    theZ /= aPointsNb;
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
  myCurrPointType = BBCENTER;

  Standard_Real Xcenter, Ycenter, Zcenter;
  if ( OCCViewer_Utilities::computeVisibleBBCenter( myViewPort->getView(), Xcenter, Ycenter, Zcenter ) )
    mySetRotationPointDlg->setCoords( Xcenter, Ycenter, Zcenter );
}

/*!
  \brief Update gravity center in the "Set Rotation Point" dialog box.
  \sa OCCViewer_SetRotationPointDlg class
*/
void OCCViewer_ViewWindow::updateGravityCoords()
{
  if ( mySetRotationPointDlg && mySetRotationPointDlg->isVisible() && myCurrPointType == BBCENTER )
  {
    Standard_Real Xcenter, Ycenter, Zcenter;
    if ( OCCViewer_Utilities::computeVisibleBBCenter( myViewPort->getView(), Xcenter, Ycenter, Zcenter ) )
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
  \brief Start the shape selection process.
*/
void OCCViewer_ViewWindow::activateStartPointSelection( TopAbs_ShapeEnum theShapeType )
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
      ic->Activate(anObj,AIS_Shape::SelectionMode(theShapeType));
     }
  }
  // activate selection <------

  if ( !myCursorIsHand )
  {
    QCursor handCursor (Qt::PointingHandCursor);
    myCursorIsHand = true;
    saveCursor();
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
    saveCursor();                // save old cursor
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
    saveCursor();                /* save old cursor */

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
  if ( myIsKeyFree && interactionStyle() == SUIT_ViewModel::KEY_FREE ) {
    myIsKeyFree = false;
    switch ( getButtonState( theEvent, interactionStyle() ) ) {
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
      break;
    }
  }

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
      if ( ( anInteractionStyle == SUIT_ViewModel::STANDARD &&
           aButton == Qt::LeftButton && ( aState == Qt::NoModifier || Qt::ShiftModifier ) ) ||
         ( anInteractionStyle == SUIT_ViewModel::KEY_FREE &&
         aButton == Qt::LeftButton && ( aState == Qt::ControlModifier || aState == ( Qt::ControlModifier|Qt::ShiftModifier ) ) ) ) {
        myDrawRect = myEnableDrawMode;
        if ( myDrawRect ) {
          drawRect();
          if ( !myCursorIsHand )        {   // we are going to sketch a rectangle
            QCursor handCursor (Qt::PointingHandCursor);
            myCursorIsHand = true;
            saveCursor();
            myViewPort->setCursor( handCursor );
          }
        }
        emit mouseMoving( this, theEvent );
      }
      else if ( ( anInteractionStyle == SUIT_ViewModel::STANDARD &&
                aButton == Qt::RightButton && ( aState == Qt::NoModifier || Qt::ShiftModifier ) ) ||
                ( anInteractionStyle == SUIT_ViewModel::KEY_FREE &&
                aButton == Qt::RightButton && ( aState == Qt::ControlModifier || aState == ( Qt::ControlModifier|Qt::ShiftModifier ) ) ) ) {
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
    myViewPort->getView()->ZFitAll();
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
  if( !aResMgr )
    return;

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

  // FitSelection
  aAction = new QtxAction(tr("MNU_FITSELECTION"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_VIEW_FITSELECTION" ) ),
                           tr( "MNU_FITSELECTION" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITSELECTION"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onFitSelection()));
  toolMgr()->registerAction( aAction, FitSelectionId );

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

  // Projection mode group

  // - orthographic projection
  aAction = new QtxAction(tr("MNU_ORTHOGRAPHIC_MODE"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_ORTHOGRAPHIC" ) ),
                          tr( "MNU_ORTHOGRAPHIC_MODE" ), 0, this);
  aAction->setStatusTip(tr("DSC_ORTHOGRAPHIC_MODE"));
  aAction->setCheckable(true);
  toolMgr()->registerAction( aAction, OrthographicId );

  // - perspective projection
  aAction = new QtxAction(tr("MNU_PERSPECTIVE_MODE"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_PERSPECTIVE" ) ),
                          tr( "MNU_PERSPECTIVE_MODE" ), 0, this);
  aAction->setStatusTip(tr("DSC_PERSPECTIVE_MODE"));
  aAction->setCheckable(true);
  toolMgr()->registerAction( aAction, PerspectiveId );
#if OCC_VERSION_LARGE > 0x06090000
  // - stereo projection
  aAction = new QtxAction(tr("MNU_STEREO_MODE"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_STEREO" ) ),
                          tr( "MNU_STEREO_MODE" ), 0, this);
  aAction->setStatusTip(tr("DSC_STEREO_MODE"));
  aAction->setCheckable(true);
  toolMgr()->registerAction( aAction, StereoId );
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onStereoType(bool)));
#endif
  // - add exclusive action group
  QActionGroup* aProjectionGroup = new QActionGroup( this );
  aProjectionGroup->addAction( toolMgr()->action( OrthographicId ) );
  aProjectionGroup->addAction( toolMgr()->action( PerspectiveId ) );
  connect(aProjectionGroup, SIGNAL(triggered(QAction*)), this, SLOT(onProjectionType(QAction*)));
  
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

  aAction = new QtxAction (tr("MNU_CLIPPING"), aResMgr->loadPixmap ("OCCViewer", tr("ICON_OCCVIEWER_CLIPPING")),
                                      tr("MNU_CLIPPING"), 0, this);
  aAction->setStatusTip (tr("DSC_CLIPPING"));
  aAction->setCheckable (true);
  connect (aAction, SIGNAL (toggled (bool)), this, SLOT (onClipping (bool)));
  toolMgr()->registerAction (aAction, ClippingId);

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
    aAction->setCheckable( true );
    aAction->setChecked( true );
    aAction->setStatusTip(tr("DSC_SHOW_TRIHEDRE"));
    connect(aAction, SIGNAL(toggled(bool)), this, SLOT(onTrihedronShow(bool)));
    toolMgr()->registerAction( aAction, TrihedronShowId );
  }

  // Scale
  aAction = new QtxAction(tr("MNU_SCALING"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_SCALING" ) ),
                           tr( "MNU_SCALING" ), 0, this);
  aAction->setStatusTip(tr("DSC_SCALING"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onAxialScale()));
  toolMgr()->registerAction( aAction, AxialScaleId );

  // Enable/disable preselection
  aAction = new QtxAction(tr("MNU_ENABLE_PRESELECTION"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_PRESELECTION" ) ),
                          tr( "MNU_ENABLE_PRESELECTION" ), 0, this);
  aAction->setStatusTip(tr("DSC_ENABLE_PRESELECTION"));
  aAction->setCheckable(true);
  connect(aAction, SIGNAL(toggled(bool)), this, SLOT(onSwitchPreselection(bool)));
  toolMgr()->registerAction( aAction, SwitchPreselectionId );

  // Enable/disable selection
  aAction = new QtxAction(tr("MNU_ENABLE_SELECTION"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_SELECTION" ) ),
                          tr( "MNU_ENABLE_SELECTION" ), 0, this);
  aAction->setStatusTip(tr("DSC_ENABLE_SELECTION"));
  aAction->setCheckable(true);
  connect(aAction, SIGNAL(toggled(bool)), this, SLOT(onSwitchSelection(bool)));
  toolMgr()->registerAction( aAction, SwitchSelectionId );

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

  // Return to 3d view
  if (my2dMode!=No2dMode){
    aAction = new QtxAction(tr("MNU_RETURN_3D_VIEW"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_RETURN_3D_VIEW" ) ),
                            tr( "MNU_RETURN_3D_VIEW" ), 0, this );
    aAction->setStatusTip(tr("DSC_RETURN_3D_VIEW"));
    connect(aAction, SIGNAL(triggered()), this, SLOT(returnTo3dView()));
    toolMgr()->registerAction( aAction, ReturnTo3dViewId );
  }

  // Synchronize View
  toolMgr()->registerAction( synchronizeAction(), SynchronizeId );
#ifdef ENABLE_RAY_TRACING
  // Ray tracing
  aAction = new QtxAction( tr("MNU_RAY_TRACING"), aResMgr->loadPixmap( "OCCViewer", tr("ICON_OCCVIEWER_RAY_TRACING") ),
                           tr("MNU_RAY_TRACING"), 0, this );
  aAction->setStatusTip( tr("DSC_RAY_TRACING") );
  connect( aAction, SIGNAL( triggered() ), this, SLOT( onRayTracing() ) );
  toolMgr()->registerAction( aAction, RayTracingId );

  // Environment texture
  aAction = new QtxAction( tr("MNU_ENV_TEXTURE"), aResMgr->loadPixmap( "OCCViewer", tr("ICON_OCCVIEWER_ENV_TEXTURE") ),
                           tr("MNU_ENV_TEXTURE"), 0, this );
  aAction->setStatusTip( tr("DSC_ENV_TEXTURE") );
  connect( aAction, SIGNAL( triggered() ), this, SLOT( onEnvTexture() ) );
  toolMgr()->registerAction( aAction, EnvTextureId );

  // Light source
  aAction = new QtxAction( tr("MNU_LIGHT_SOURCE"), aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_LIGHT_SOURCE" ) ),
                           tr( "MNU_LIGHT_SOURCE" ), 0, this );
  aAction->setStatusTip( tr("DSC_LIGHT_SOURCE") );
  connect( aAction, SIGNAL( triggered() ), this, SLOT( onLightSource() ) );
  toolMgr()->registerAction( aAction, LightSourceId );
#endif
}

/*!
  \brief Create toolbar.
*/
void OCCViewer_ViewWindow::createToolBar()
{
  static const char* titles[] = {
    "LBL_3DTOOLBAR_LABEL",
    "LBL_XYTOOLBAR_LABEL",
    "LBL_XZTOOLBAR_LABEL",
    "LBL_YZTOOLBAR_LABEL",
  };
  static const char* names[] = {
    "OCCViewer3DViewOperations",
    "OCCViewerXYViewOperations",
    "OCCViewerXZViewOperations",
    "OCCViewerYZViewOperations",
  };
  int tid = toolMgr()->createToolBar( tr( titles[my2dMode] ),        // title (language-dependant)
				      QString( names[my2dMode] ),    // name (language-independant)
				      false );                       // disable floatable toolbar
  if ( my2dMode != No2dMode ){
    toolMgr()->append( ReturnTo3dViewId, tid );
    toolMgr()->append( toolMgr()->separator(), tid );
  }
  toolMgr()->append( DumpId, tid );
  toolMgr()->append( SwitchInteractionStyleId, tid );
  toolMgr()->append( SwitchZoomingStyleId, tid );
  toolMgr()->append( SwitchPreselectionId, tid );
  toolMgr()->append( SwitchSelectionId, tid );
  if( myModel->trihedronActivated() )
    toolMgr()->append( TrihedronShowId, tid );

  QtxMultiAction* aScaleAction = new QtxMultiAction( this );
  aScaleAction->insertAction( toolMgr()->action( FitAllId ) );
  aScaleAction->insertAction( toolMgr()->action( FitRectId ) );
#if OCC_VERSION_LARGE > 0x06090000
  aScaleAction->insertAction( toolMgr()->action( FitSelectionId ) );
#endif
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

    toolMgr()->append( OrthographicId, tid );
    toolMgr()->append( PerspectiveId, tid );
#if OCC_VERSION_LARGE > 0x06090000
    toolMgr()->append( StereoId, tid );
#endif

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
  toolMgr()->append( GraduatedAxesId, tid );
  toolMgr()->append( AmbientId, tid );

  toolMgr()->append( MaximizedId, tid );
  toolMgr()->append( SynchronizeId, tid );
#ifdef ENABLE_RAY_TRACING
  toolMgr()->append( RayTracingId, tid );
  toolMgr()->append( EnvTextureId, tid );
  toolMgr()->append( LightSourceId, tid );
#endif
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
  onProjectionType(); // needed to apply projection type properly after reset
  myViewPort->getView()->Update();
  emit vpTransformationFinished( RESETVIEW );
}

/*!
  \brief Set the given projection mode.

  Set the given projection mode: Orthographic or Perspective.
*/
void OCCViewer_ViewWindow::onProjectionType( QAction* theAction )
{
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
    Handle(Graphic3d_Camera) aCamera = aView3d->Camera();
    if (theAction == toolMgr()->action( OrthographicId )) {
      myModel->setProjectionType(Orthographic);
      aCamera->SetProjectionType ( Graphic3d_Camera::Projection_Orthographic );
      aCamera->SetFOVy(45.0);
    }
    else if (theAction == toolMgr()->action( PerspectiveId )) {
      myModel->setProjectionType(Perspective);
      aCamera->SetProjectionType ( Graphic3d_Camera::Projection_Perspective );
      aCamera->SetFOVy(30.0);
    }
#if OCC_VERSION_LARGE > 0x06090000
    if (toolMgr()->action( StereoId )->isChecked()) {
      aCamera->SetProjectionType ( Graphic3d_Camera::Projection_Stereo );
      aCamera->SetFOVy(30.0);
    }
#endif
    aView3d->Redraw();
    onViewFitAll();
  }
}

/*!
  \brief Sets Stereo projection mode.

  Sets Stereo projection mode.
*/
void OCCViewer_ViewWindow::onStereoType( bool activate )
{
#if OCC_VERSION_LARGE > 0x06090000
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
    Handle(Graphic3d_Camera) aCamera = aView3d->Camera();
    if (activate) {
      toolMgr()->action( PerspectiveId )->setChecked(true);
      aCamera->SetProjectionType(Graphic3d_Camera::Projection_Perspective);
      toolMgr()->action( PerspectiveId )->actionGroup()->setEnabled(false);

      aCamera->SetProjectionType ( Graphic3d_Camera::Projection_Stereo );
      SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
      setStereoType( aResMgr->integerValue( "OCCViewer", "stereo_type", 0 ) );
      setAnaglyphFilter( aResMgr->integerValue( "OCCViewer", "anaglyph_filter", 0 ) );
      setReverseStereo( aResMgr->booleanValue( "OCCViewer", "reverse_stereo", false ) );
      setVSync( aResMgr->booleanValue( "OCCViewer", "enable_vsync", true ) );
      setQuadBufferSupport( aResMgr->booleanValue( "OCCViewer", "enable_quad_buffer_support", false ) );
    }
    else {
      toolMgr()->action( PerspectiveId )->actionGroup()->setEnabled(true);
      if (myModel->projectionType() == Orthographic) {
        toolMgr()->action( OrthographicId )->setChecked(true);
        aCamera->SetProjectionType(Graphic3d_Camera::Projection_Orthographic);
      }
      else if (myModel->projectionType() == Perspective) {
        toolMgr()->action( PerspectiveId )->setChecked(true);
        aCamera->SetProjectionType(Graphic3d_Camera::Projection_Perspective);
      }
    }
    aView3d->Redraw();
    onViewFitAll();
  }

  if ( isQuadBufferSupport() && !isOpenGlStereoSupport() && stereoType() == QuadBuffer &&
       toolMgr()->action( StereoId )->isChecked() )
    SUIT_MessageBox::warning( 0, tr( "WRN_WARNING" ),  tr( "WRN_SUPPORT_QUAD_BUFFER" ) );
#endif
}

/*!
  \brief Restore the view.

  Restore the projection mode based on tool-buttons states.
*/
void OCCViewer_ViewWindow::onProjectionType()
{
  emit vpTransformationStarted( PROJECTION );
  if (toolMgr()->action( OrthographicId )->isChecked())
    setProjectionType( Orthographic );
  if (toolMgr()->action( PerspectiveId )->isChecked())
    setProjectionType( Perspective );
#if OCC_VERSION_LARGE > 0x06090000
  if (toolMgr()->action( StereoId )->isChecked())
    setProjectionType( Stereo );
#endif
  emit vpTransformationFinished( PROJECTION );
}

void OCCViewer_ViewWindow::setProjectionType( int mode )
{
  QtxAction* anOrthographicAction = dynamic_cast<QtxAction*>( toolMgr()->action( OrthographicId ) );
  QtxAction* aPerspectiveAction = dynamic_cast<QtxAction*>( toolMgr()->action( PerspectiveId ) );
#if OCC_VERSION_LARGE > 0x06090000
  QtxAction* aStereoAction = dynamic_cast<QtxAction*>( toolMgr()->action( StereoId ) );
#endif
  switch ( mode ) {
    case Orthographic:
      onProjectionType( anOrthographicAction );
      break;
    case Perspective:
      onProjectionType( aPerspectiveAction );
      break;
    case Stereo:
      onStereoType( true );
      break;
  }
  // update action state if method is called outside
  if ( mode == Orthographic && !anOrthographicAction->isChecked() ) {
	  anOrthographicAction->setChecked( true );
    #if OCC_VERSION_LARGE > 0x06090000
	  aStereoAction->setChecked( false );
    #endif
  }
  if ( mode == Perspective && !aPerspectiveAction->isChecked() ) {
	  aPerspectiveAction->setChecked( true );
    #if OCC_VERSION_LARGE > 0x06090000
	  aStereoAction->setChecked( false );
    #endif
  }
#if OCC_VERSION_LARGE > 0x06090000
  if ( mode == Stereo ) {
    aStereoAction->setChecked( true );
    if ( anOrthographicAction->isEnabled() ) {
      anOrthographicAction->setEnabled( false );
      anOrthographicAction->setChecked( false );
      aStereoAction->setChecked( false );
    }
    else {
      anOrthographicAction->setEnabled( true );
      aStereoAction->setChecked( false );
      anOrthographicAction->setChecked(myModel->projectionType() == Orthographic);
    }
    if ( aPerspectiveAction->isEnabled() ) {
      aPerspectiveAction->setEnabled( false );
      aPerspectiveAction->setChecked( true );
      if ( isQuadBufferSupport() && !isOpenGlStereoSupport() && stereoType() == QuadBuffer &&
           toolMgr()->action( StereoId )->isChecked() )
        SUIT_MessageBox::warning( 0, tr( "WRN_WARNING" ),  tr( "WRN_SUPPORT_QUAD_BUFFER" ) );
    }
    else {
      aPerspectiveAction->setEnabled( true );
      aStereoAction->setChecked( false );
      aPerspectiveAction->setChecked(myModel->projectionType() == Perspective);
      onProjectionType();
    }
  }
  else {
    if ( !anOrthographicAction->isEnabled() )
      anOrthographicAction->setEnabled( true );
    if ( !aPerspectiveAction->isEnabled() )
      aPerspectiveAction->setEnabled( true );
  }
#endif
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
  \brief Perform "fit selection" transformation.
*/
void OCCViewer_ViewWindow::onFitSelection()
{
  emit vpTransformationStarted( FITSELECTION );
#if OCC_VERSION_LARGE > 0x06090000
  myModel->getAISContext()->FitSelected( getViewPort()->getView() );
#endif
  emit vpTransformationFinished( FITSELECTION );
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
      if (myCurrPointType == BBCENTER)
      {
        Standard_Real Xcenter, Ycenter, Zcenter;
        if (OCCViewer_Utilities::computeVisibleBBCenter(myViewPort->getView(), Xcenter, Ycenter, Zcenter))
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
  OCCViewer_ViewWindow* occVw = dynamic_cast<OCCViewer_ViewWindow*> (vw);
  if(occVw && occVw->getView(OCCViewer_ViewFrame::MAIN_VIEW)) {
    occVw->getView(OCCViewer_ViewFrame::MAIN_VIEW)->synchronize(this);
  }
}

/*!
  Creates one more window with same content
*/
void OCCViewer_ViewWindow::onAxialScale()
{
  if ( !myScalingDlg )
    myScalingDlg = new OCCViewer_AxialScaleDlg( this , myModel );

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
  aView3d->SetTwist( anItem.twist );
  aView3d->SetAt( anItem.atX, anItem.atY, anItem.atZ );
  aView3d->SetImmediateUpdate( prev );
  aView3d->SetEye( anItem.eyeX, anItem.eyeY, anItem.eyeZ );
  aView3d->SetProj( anItem.projX, anItem.projY, anItem.projZ );
  aView3d->SetAxialScale( anItem.scaleX, anItem.scaleY, anItem.scaleZ );

#if OCC_VERSION_LARGE > 0x06070100
  if ( anItem.centerX != 0.0 || anItem.centerY != 0.0 )
  {
    double anUpX = 0.0, anUpY = 0.0, anUpZ = 0.0;

    // "eye" and "at" require conversion to represent center panning
    // up direction is only available after setting angle of twist and
    // other view parameters
    aView3d->Up( anUpX, anUpY, anUpZ );

    gp_Dir aProj( -anItem.projX, -anItem.projY, -anItem.projZ );
    gp_Dir anUp( anUpX, anUpY, anUpZ );
    gp_Pnt anAt( anItem.atX, anItem.atY, anItem.atZ );
    gp_Pnt anEye( anItem.eyeX, anItem.eyeY, anItem.eyeZ );
    gp_Dir aSide = aProj ^ anUp;

    anAt.Translate( gp_Vec( aSide ) * anItem.centerX );
    anAt.Translate( gp_Vec( anUp  ) * anItem.centerY );

    aView3d->SetAt( anAt.X(), anAt.Y(), anAt.Z() );
    aView3d->SetProj( anItem.projX, anItem.projY, anItem.projZ );
  }
#else
  aView3d->SetCenter( anItem.centerX, anItem.centerY );
#endif

  if ( !baseParamsOnly ) {

    myModel->setTrihedronShown( anItem.isVisible );
    myModel->setTrihedronSize( anItem.size );

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
void OCCViewer_ViewWindow::onTrihedronShow(bool show)
{
  myModel->setTrihedronShown(show);
}

/*!
  \brief Toggles preselection (highlighting) on/off
*/
void OCCViewer_ViewWindow::onSwitchPreselection( bool on )
{
  myPreselectionEnabled = on;
  myModel->setSelectionOptions( isPreselectionEnabled(), myModel->isSelectionEnabled() );

  // unhighlight all highlighted objects
  /*if ( !on ) {
    myModel->unHighlightAll( true, false );
  }*/

  // update action state if method is called outside
  QtxAction* a = dynamic_cast<QtxAction*>( toolMgr()->action( SwitchPreselectionId ) );
  if ( a && a->isChecked() != on ) {
    a->setChecked( on );
  }
}

/*!
  \brief Toggles selection on/off
*/
void OCCViewer_ViewWindow::onSwitchSelection( bool on )
{
  mySelectionEnabled = on;
  myModel->setSelectionOptions( myModel->isPreselectionEnabled(), isSelectionEnabled() );

  // update action state if method is called outside

  // preselection
  QtxAction* a = dynamic_cast<QtxAction*>( toolMgr()->action( SwitchPreselectionId ) );
  if ( a ) {
    a->setEnabled( on );
  }

  // selection
  a = dynamic_cast<QtxAction*>( toolMgr()->action( SwitchSelectionId ) );
  if ( a && a->isChecked() != on ) {
    a->setChecked( on );
  }
}

/*!
  \brief Switches "keyboard free" interaction style on/off
*/
void OCCViewer_ViewWindow::onSwitchInteractionStyle( bool on )
{
  myInteractionStyle = on ? (int)SUIT_ViewModel::KEY_FREE : (int)SUIT_ViewModel::STANDARD;

  // update action state if method is called outside
  QtxAction* a = dynamic_cast<QtxAction*>( toolMgr()->action( SwitchInteractionStyleId ) );
  if ( a && a->isChecked() != on )
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
  if ( a && a->isChecked() != on )
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

  // rnv: An old approach to dump the OCCViewer content
  //      Now used OCCT built-in procedure.
  /*
  QApplication::syncX();
  view->Redraw(); // In order to reactivate GL context
  //view->Update();

#ifndef DISABLE_GLVIEWER
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
#endif
  */

  Image_PixMap aPix;
  view->ToPixMap(aPix,aWidth, aHeight,Graphic3d_BT_RGBA);

  QImage anImage( aPix.Data(), aWidth, aHeight, QImage::Format_ARGB32 );
  anImage = anImage.mirrored();
  return anImage;
}

bool OCCViewer_ViewWindow::dumpViewToFormat( const QImage& img,
                                             const QString& fileName,
                                             const QString& format )
{
  bool res = false;
  QApplication::setOverrideCursor( Qt::WaitCursor );
  if ( format != "PS" && format != "EPS")
   res = myViewPort->getView()->Dump( fileName.toStdString().c_str() );

#if OCC_VERSION_MAJOR < 7
  Handle(Visual3d_View) a3dView = myViewPort->getView()->View();
#else
  Handle(Graphic3d_CView) a3dView = myViewPort->getView()->View();
#endif

  if (format == "PS")
    res = a3dView->Export(strdup(qPrintable(fileName)), Graphic3d_EF_PostScript);
  else if (format == "EPS")
    res = a3dView->Export(strdup(qPrintable(fileName)), Graphic3d_EF_EnhPostScript);

  QApplication::restoreOverrideCursor();
  return res;
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

    // calculate new a,b,c,d values for the plane
    gp_Pln pln (gp_Pnt(x, y, z), gp_Dir(dx, dy, dz));
    double a, b, c, d;
    pln.Coefficients(a, b, c, d);
#if OCC_VERSION_LARGE > 0x07000000 
    Handle(Graphic3d_SequenceOfHClipPlane) aPlanes = view->ClipPlanes();
    Graphic3d_SequenceOfHClipPlane::Iterator anIter (*aPlanes);
    if(aPlanes->Size() > 0 ) {
#else
    Graphic3d_SequenceOfHClipPlane aPlanes = view->GetClipPlanes();
    Graphic3d_SequenceOfHClipPlane::Iterator anIter (aPlanes);
    if(aPlanes.Size() > 0 ) {
#endif
      Handle(Graphic3d_ClipPlane) aClipPlane = anIter.Value();
      aClipPlane->SetEquation(pln);
      aClipPlane->SetOn(Standard_True);
    } else {
      view->AddClipPlane( myModel->createClipPlane( pln, Standard_True ) );
    }
  }
  else {
#if OCC_VERSION_LARGE > 0x07000000 
    Handle(Graphic3d_SequenceOfHClipPlane) aPlanes = view->ClipPlanes();
    Graphic3d_SequenceOfHClipPlane::Iterator anIter (*aPlanes);
#else
    Graphic3d_SequenceOfHClipPlane aPlanes = view->GetClipPlanes();
    Graphic3d_SequenceOfHClipPlane::Iterator anIter (aPlanes);
#endif
    for( ;anIter.More();anIter.Next() ){
      Handle(Graphic3d_ClipPlane) aClipPlane = anIter.Value();
      aClipPlane->SetOn(Standard_False);
    }
  }

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
  bool res = false;
#if OCC_VERSION_LARGE > 0x07000000 
  Handle(Graphic3d_SequenceOfHClipPlane) aPlanes = view->ClipPlanes();
  Graphic3d_SequenceOfHClipPlane::Iterator anIter (*aPlanes);
#else
    Graphic3d_SequenceOfHClipPlane aPlanes = view->GetClipPlanes();
    Graphic3d_SequenceOfHClipPlane::Iterator anIter (aPlanes);
#endif
  for( ;anIter.More();anIter.Next() ) {
    Handle(Graphic3d_ClipPlane) aClipPlane = anIter.Value();
    if(aClipPlane->IsOn()) {
      res = true;
      break;
    }
  }
  return res;
}

/*!
  \brief Get the visual parameters of the view window.
  \return visual parameters of view window
*/
viewAspect OCCViewer_ViewWindow::getViewParams() const
{
  double projX, projY, projZ, twist;
  double atX, atY, atZ, eyeX, eyeY, eyeZ;
  double aScaleX, aScaleY, aScaleZ;

  Handle(V3d_View) aView3d = myViewPort->getView();

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

#if OCC_VERSION_LARGE <= 0x06070100 // the property is deprecated after OCCT 6.7.1
  aView3d->Center( params.centerX, params.centerY );
#endif

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
#if OCC_VERSION_LARGE <= 0x06070100 // the property is deprecated after OCCT 6.7.1
  data << QString( "centerX=%1" )  .arg( params.centerX, 0, 'e', 12 );
  data << QString( "centerY=%1" )  .arg( params.centerY, 0, 'e', 12 );
#endif
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

  ClipPlanesList aPlanes =  myModel->getClipPlanes();
  for ( int i=0; i < aPlanes.size(); i++ )
  {
    OCCViewer_ClipPlane& aPlane = aPlanes[i];
    QString ClippingPlane = QString( "ClippingPlane%1=").arg( i+1 );
    ClippingPlane +=  QString( "Mode~%1;").arg( (int)aPlane.Mode );
    ClippingPlane +=  QString( "IsActive~%1;").arg( aPlane.IsOn );
    switch ( aPlane.Mode )
    {
      case OCCViewer_ClipPlane::Absolute :
      {
        ClippingPlane += QString( "AbsoluteOrientation~%1;" ).arg( aPlane.OrientationType );

        if ( aPlane.OrientationType == OCCViewer_ClipPlane::AbsoluteCustom )
        {
          ClippingPlane += QString( "Dx~%1;" ).arg( aPlane.AbsoluteOrientation.Dx );
          ClippingPlane += QString( "Dy~%1;" ).arg( aPlane.AbsoluteOrientation.Dy );
          ClippingPlane += QString( "Dz~%1;" ).arg( aPlane.AbsoluteOrientation.Dz );
        }
        else
        {
          ClippingPlane += QString( "IsInvert~%1;" ).arg( aPlane.AbsoluteOrientation.IsInvert );
        }
      }
      break;

      case OCCViewer_ClipPlane::Relative :
      {
        ClippingPlane += QString( "RelativeOrientation~%1;" ).arg( aPlane.OrientationType );
        ClippingPlane += QString( "Rotation1~%1;" ).arg( aPlane.RelativeOrientation.Rotation1 );
        ClippingPlane += QString( "Rotation2~%1" ).arg( aPlane.RelativeOrientation.Rotation2 );
      }
      break;
    }

    ClippingPlane +=  QString( "X~%1;" ).arg( aPlane.X );
    ClippingPlane +=  QString( "Y~%1;" ).arg( aPlane.Y );
    ClippingPlane +=  QString( "Z~%1;" ).arg( aPlane.Z );
    data << ClippingPlane;
  }

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

  // ray tracing parameters
  Graphic3d_RenderingParams rendParams = this->getViewPort()->getView()->RenderingParams();
  if ( rendParams.Method == Graphic3d_RM_RAYTRACING ) {
    QString RayTracing = "rayTracing=";
    RayTracing += QString( "rtDepth~%1;" ).arg( rendParams.RaytracingDepth );
    RayTracing += QString( "rtReflection~%1;" ).arg( rendParams.IsReflectionEnabled );
    RayTracing += QString( "rtAntialiasing~%1;" ).arg( rendParams.IsAntialiasingEnabled );
    RayTracing += QString( "rtShadow~%1;" ).arg( rendParams.IsShadowEnabled );
    RayTracing += QString( "rtTransShadow~%1;" ).arg( rendParams.IsTransparentShadowEnabled );
    data << RayTracing;
  }

  // environment texture parameters
  Handle(Graphic3d_TextureEnv) aTexture = this->getViewPort()->getView()->TextureEnv();
  if ( !aTexture.IsNull() ) {
    QString EnvTexture = "envTexture=";
    if ( aTexture->Name() == Graphic3d_NOT_ENV_UNKNOWN ) {
      TCollection_AsciiString aFileName;
      aTexture->Path().SystemName( aFileName );
      EnvTexture += QString( "etFile~%1;" ).arg( aFileName.ToCString() );
    }
    else
      EnvTexture += QString( "etNumber~%1;" ).arg( aTexture->Name() );
    data << EnvTexture;
  }

  // light source parameters
  myModel->getViewer3d()->InitDefinedLights();
  while ( myModel->getViewer3d()->MoreDefinedLights() )
  {
    Handle(V3d_Light) aLight = myModel->getViewer3d()->DefinedLight();
    if ( aLight->Type() != V3d_AMBIENT ) {
      QString LightSource = QString( "lightSource=" );
      LightSource += QString( "lightType~%1;" ).arg( aLight->Type() );
      double aX, aY, aZ;
      if ( aLight->Type() == V3d_DIRECTIONAL )
        Handle(V3d_DirectionalLight)::DownCast( aLight )->Direction( aX, aY, aZ );
      else if ( aLight->Type() == V3d_POSITIONAL )
        Handle(V3d_PositionalLight)::DownCast( aLight )->Position( aX, aY, aZ );
      LightSource += QString( "lightX~%1;" ).arg( aX );
      LightSource += QString( "lightY~%1;" ).arg( aY );
      LightSource += QString( "lightZ~%1;" ).arg( aZ );
      LightSource += QString( "lightColorR~%1;" ).arg( aLight->Color().Red() );
      LightSource += QString( "lightColorG~%1;" ).arg( aLight->Color().Green() );
      LightSource += QString( "lightColorB~%1;" ).arg( aLight->Color().Blue() );
      LightSource += QString( "lightHeadlight~%1;" ).arg( aLight->Headlight() );
      data << LightSource;
    }
    myModel->getViewer3d()->NextDefinedLights();
  }

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
  ClipPlanesList aClipPlanes;
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
      else if ( paramName.contains( "ClippingPlane" ) )
      {
        QStringList ClipPlaneData = paramValue.split( ';' );
        OCCViewer_ClipPlane aPlane;
        foreach( QString ClipPlaneParam, ClipPlaneData )
        {
          QString ClipPlane_paramName  = ClipPlaneParam.section( '~', 0, 0 ).trimmed();
          QString ClipPlane_paramValue = ClipPlaneParam.section( '~', 1, 1 ).trimmed();
          if ( ClipPlane_paramName == "Mode" )
          {
            aPlane.Mode = ( OCCViewer_ClipPlane::PlaneMode ) ClipPlane_paramValue.toInt();
          }
          else if ( ClipPlane_paramName == "IsActive" ) aPlane.IsOn = ClipPlane_paramValue.toInt();
          else if ( ClipPlane_paramName == "X" )        aPlane.X    = ClipPlane_paramValue.toDouble();
          else if ( ClipPlane_paramName == "Y" )        aPlane.Y    = ClipPlane_paramValue.toDouble();
          else if ( ClipPlane_paramName == "Z" )        aPlane.Z    = ClipPlane_paramValue.toDouble();
          else
          {
            switch ( aPlane.Mode )
            {
              case OCCViewer_ClipPlane::Absolute :
                if      ( ClipPlane_paramName == "Dx" ) aPlane.AbsoluteOrientation.Dx = ClipPlane_paramValue.toDouble();
                else if ( ClipPlane_paramName == "Dy" ) aPlane.AbsoluteOrientation.Dy = ClipPlane_paramValue.toDouble();
                else if ( ClipPlane_paramName == "Dz" ) aPlane.AbsoluteOrientation.Dz = ClipPlane_paramValue.toDouble();
                else if ( ClipPlane_paramName == "IsInvert" ) aPlane.AbsoluteOrientation.IsInvert = ClipPlane_paramValue.toInt();
                else if ( ClipPlane_paramName == "AbsoluteOrientation" ) aPlane.OrientationType = ClipPlane_paramValue.toInt();
                break;

              case OCCViewer_ClipPlane::Relative :
                if      ( ClipPlane_paramName == "RelativeOrientation" ) aPlane.OrientationType = ClipPlane_paramValue.toInt();
                else if ( ClipPlane_paramName == "Rotation1" )           aPlane.RelativeOrientation.Rotation1 = ClipPlane_paramValue.toDouble();
                else if ( ClipPlane_paramName == "Rotation2" )           aPlane.RelativeOrientation.Rotation2 = ClipPlane_paramValue.toDouble();
                break;
            }
          }
        }
        aClipPlanes.push_back(aPlane);
      }
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
      else if ( paramName == "rayTracing" )
      {
        Graphic3d_RenderingParams& rendParams = this->getViewPort()->getView()->ChangeRenderingParams();
        rendParams.Method = Graphic3d_RM_RAYTRACING;
        QStringList rtData = paramValue.split( ';' );
        foreach( QString rtParam, rtData )
        {
          QString rt_paramName  = rtParam.section( '~', 0, 0 ).trimmed();
          QString rt_paramValue = rtParam.section( '~', 1, 1 ).trimmed();
          if ( rt_paramName == "rtDepth" ) rendParams.RaytracingDepth = rt_paramValue.toInt();
          else if ( rt_paramName == "rtReflection" ) rendParams.IsReflectionEnabled = rt_paramValue.toInt();
          else if ( rt_paramName == "rtAntialiasing" ) rendParams.IsAntialiasingEnabled = rt_paramValue.toInt();
          else if ( rt_paramName == "rtShadow" ) rendParams.IsShadowEnabled = rt_paramValue.toInt();
          else if ( rt_paramName == "rtTransShadow" ) rendParams.IsTransparentShadowEnabled = rt_paramValue.toInt();
        }
      }
      else if ( paramName == "envTexture" )
      {
        Handle(Graphic3d_TextureEnv) aTexture;
        QStringList etData = paramValue.split( ';' );
        foreach( QString etParam, etData )
        {
          QString et_paramName  = etParam.section( '~', 0, 0 ).trimmed();
          QString et_paramValue = etParam.section( '~', 1, 1 ).trimmed();
          if ( et_paramName == "etNumber" )
            aTexture = new Graphic3d_TextureEnv( Graphic3d_NameOfTextureEnv( et_paramValue.toInt() ) );
          else if ( et_paramName == "etFile" )
            aTexture = new Graphic3d_TextureEnv( TCollection_AsciiString( et_paramValue.toStdString().c_str() ) );
          Handle(V3d_View) aView = this->getViewPort()->getView();
          aView->SetTextureEnv( aTexture );
#if OCC_VERSION_LARGE <= 0x07000000
          aView->SetSurfaceDetail( V3d_TEX_ENVIRONMENT );
#endif
        }
      }
      else if ( paramName == "lightSource" )
      {
        myModel->getViewer3d()->InitDefinedLights();
        while ( myModel->getViewer3d()->MoreDefinedLights() )
        {
          Handle(V3d_Light) aLight = myModel->getViewer3d()->DefinedLight();
          if( aLight->Type() != V3d_AMBIENT ) {
            myModel->getViewer3d()->DelLight( aLight );
	    myModel->getViewer3d()->InitDefinedLights();
	  } else {
	    myModel->getViewer3d()->NextDefinedLights();
	  }
        }
        double aX, aY, aZ;
        double cR, cG, cB;
        V3d_TypeOfLight aType;
        bool isHeadlight;
        QStringList lsData = paramValue.split( ';' );
        foreach( QString lsParam, lsData )
        {
          QString ls_paramName  = lsParam.section( '~', 0, 0 ).trimmed();
          QString ls_paramValue = lsParam.section( '~', 1, 1 ).trimmed();
          if ( ls_paramName == "lightType" ) aType = V3d_TypeOfLight( ls_paramValue.toInt() );
          else if ( ls_paramName == "lightX" ) aX = ls_paramValue.toDouble();
          else if ( ls_paramName == "lightY" ) aY = ls_paramValue.toDouble();
          else if ( ls_paramName == "lightZ" ) aZ = ls_paramValue.toDouble();
          else if ( ls_paramName == "lightColorR" ) cR = ls_paramValue.toDouble();
          else if ( ls_paramName == "lightColorG" ) cG = ls_paramValue.toDouble();
          else if ( ls_paramName == "lightColorB" ) cB = ls_paramValue.toDouble();
          else if ( ls_paramName == "lightHeadlight" ) isHeadlight = ls_paramValue.toInt();
        }
        Quantity_Color aColor = Quantity_Color( cR, cG, cB, Quantity_TOC_RGB );
        if( aType == V3d_DIRECTIONAL ) {
          Handle(V3d_DirectionalLight) aLight = new V3d_DirectionalLight( myModel->getViewer3d() );
          aLight->SetDirection( aX, aY, aZ );
          aLight->SetColor( aColor );
          aLight->SetHeadlight( isHeadlight );
          myModel->getViewer3d()->SetLightOn( aLight );
        }
        else if( aType == V3d_POSITIONAL ) {
          Handle(V3d_PositionalLight) aLight = new V3d_PositionalLight( myModel->getViewer3d(), aX, aY, aZ, aColor.Name() );
          aLight->SetHeadlight( isHeadlight );
          myModel->getViewer3d()->SetLightOn( aLight );
        }
      }
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
  myModel->setClipPlanes(aClipPlanes);
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
    Save old cursor. [ protected ]
*/
void OCCViewer_ViewWindow::saveCursor()
{
  QCursor* aCursor = NULL;
  if ( myViewPort )
    aCursor = myViewPort->getDefaultCursor();
  myCursor = ( aCursor ? *aCursor : cursor() );
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
    bool append = bool( mypSketcher->buttonState() && mypSketcher->isHasShift() );
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

void OCCViewer_ViewWindow::returnTo3dView()
{
  setReturnedTo3dView( true );
}

void OCCViewer_ViewWindow::setReturnedTo3dView(bool isVisible3dView)
{
  if ( !toolMgr()->action( ReturnTo3dViewId ) ||
    toolMgr()->isShown(ReturnTo3dViewId) != isVisible3dView ) return;
  if ( !isVisible3dView )
    toolMgr()->show( ReturnTo3dViewId );
  else
    toolMgr()->hide( ReturnTo3dViewId );
  if ( isVisible3dView ) emit returnedTo3d( );
}


void OCCViewer_ViewWindow::setMaximized(bool toMaximize, bool toSendSignal)
{
  QAction* anAction =  toolMgr()->action( MaximizedId );
  QAction* anAction2 =  toolMgr()->action( ReturnTo3dViewId );
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  if ( toMaximize ) {
    anAction->setText( tr( "MNU_MINIMIZE_VIEW" ) );
    anAction->setToolTip( tr( "MNU_MINIMIZE_VIEW" ) );
    anAction->setIcon( aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_MINIMIZE" ) ) );
    anAction->setStatusTip( tr( "DSC_MINIMIZE_VIEW" ) );
    if ( anAction2 && my2dMode != No2dMode ) toolMgr()->show( ReturnTo3dViewId );
    if (toSendSignal) {
      emit maximized( this, true );
    }
  }
  else {
    anAction->setText( tr( "MNU_MAXIMIZE_VIEW" ) );
    anAction->setToolTip( tr( "MNU_MAXIMIZE_VIEW" ) );
    anAction->setIcon( aResMgr->loadPixmap( "OCCViewer", tr( "ICON_OCCVIEWER_MAXIMIZE" ) ) );
    anAction->setStatusTip( tr( "DSC_MAXIMIZE_VIEW" ) );
    if ( anAction2 && my2dMode != No2dMode ) toolMgr()->hide( ReturnTo3dViewId );
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

int OCCViewer_ViewWindow::projectionType() const
{
  int mode = Orthographic;
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
    Handle(Graphic3d_Camera) aCamera = aView3d->Camera();
    if (aCamera->ProjectionType() == Graphic3d_Camera::Projection_Perspective)
      mode = Perspective;
    if (aCamera->ProjectionType() == Graphic3d_Camera::Projection_Orthographic)
      mode = Orthographic;
  #if OCC_VERSION_LARGE > 0x06090000
    if (aCamera->ProjectionType() == Graphic3d_Camera::Projection_Stereo)
      mode = Stereo;
  #endif
  }
  return mode;
}

void OCCViewer_ViewWindow::setStereoType( int type )
{
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Graphic3d_RenderingParams* aParams = &aView3d->ChangeRenderingParams();
    aParams->StereoMode = (Graphic3d_StereoMode)type;
  #endif
  }
}

int OCCViewer_ViewWindow::stereoType() const
{
  int type = QuadBuffer;
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Graphic3d_RenderingParams* aParams = &aView3d->ChangeRenderingParams();
    type = (OCCViewer_ViewWindow::StereoType)aParams->StereoMode;
  #endif
  }
  return type;
}

void OCCViewer_ViewWindow::setAnaglyphFilter( int type )
{
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Graphic3d_RenderingParams* aParams = &aView3d->ChangeRenderingParams();
    if (type == RedCyan)
      aParams->AnaglyphFilter = Graphic3d_RenderingParams::Anaglyph_RedCyan_Optimized;
    if (type == YellowBlue)
      aParams->AnaglyphFilter = Graphic3d_RenderingParams::Anaglyph_YellowBlue_Optimized;
    if (type == GreenMagenta)
      aParams->AnaglyphFilter = Graphic3d_RenderingParams::Anaglyph_GreenMagenta_Simple;
  #endif
  }
}

int OCCViewer_ViewWindow::anaglyphFilter() const
{
  int type = RedCyan;
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Graphic3d_RenderingParams* aParams = &aView3d->ChangeRenderingParams();
    if (aParams->AnaglyphFilter == Graphic3d_RenderingParams::Anaglyph_RedCyan_Optimized)
      type = RedCyan;
    if (aParams->AnaglyphFilter == Graphic3d_RenderingParams::Anaglyph_YellowBlue_Optimized)
      type = YellowBlue;
    if (aParams->AnaglyphFilter == Graphic3d_RenderingParams::Anaglyph_GreenMagenta_Simple)
      type = GreenMagenta;
  #endif
  }
  return type;
}

void OCCViewer_ViewWindow::setStereographicFocus( int type, double value )
{
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Handle(Graphic3d_Camera) aCamera = aView3d->Camera();
    aCamera->SetZFocus( (Graphic3d_Camera::FocusType) type, value );
  #endif
  }
}

int OCCViewer_ViewWindow::stereographicFocusType() const
{
  int type = Relative;
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Handle(Graphic3d_Camera) aCamera = aView3d->Camera();
    type = (OCCViewer_ViewWindow::FocusIODType)aCamera->ZFocusType();
  #endif
  }
  return type;
}

double OCCViewer_ViewWindow::stereographicFocusValue() const
{
  double value = 1.0;
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Handle(Graphic3d_Camera) aCamera = aView3d->Camera();
    value = aCamera->ZFocus();
  #endif
  }
  return value;
}

void OCCViewer_ViewWindow::setInterocularDistance( int type, double value )
{
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Handle(Graphic3d_Camera) aCamera = aView3d->Camera();
    aCamera->SetIOD( (Graphic3d_Camera::IODType) type, value );
  #endif
  }
}

int OCCViewer_ViewWindow::interocularDistanceType() const
{
  int type = Relative;
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Handle(Graphic3d_Camera) aCamera = aView3d->Camera();
    type = (OCCViewer_ViewWindow::FocusIODType)aCamera->GetIODType();
  #endif
  }
  return type;
}

double OCCViewer_ViewWindow::interocularDistanceValue() const
{
  double value = 0.05;
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Handle(Graphic3d_Camera) aCamera = aView3d->Camera();
    value = aCamera->IOD();
  #endif
  }
  return value;
}

void OCCViewer_ViewWindow::setReverseStereo( bool reverse )
{
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Graphic3d_RenderingParams* aParams = &aView3d->ChangeRenderingParams();
    aParams->ToReverseStereo = reverse;
  #endif
  }
}

bool OCCViewer_ViewWindow::isReverseStereo() const
{
  int reverse = false;
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Graphic3d_RenderingParams* aParams = &aView3d->ChangeRenderingParams();
    reverse = aParams->ToReverseStereo;
  #endif
  }
  return reverse;
}

void OCCViewer_ViewWindow::setVSync( bool enable )
{
  Handle(AIS_InteractiveContext) anIntCont = myModel->getAISContext();
  if ( !anIntCont.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast(anIntCont->CurrentViewer()->Driver());
    OpenGl_Caps* aCaps = &aDriver->ChangeOptions();
    aCaps->swapInterval = enable;
  #endif
  }
}

bool OCCViewer_ViewWindow::isVSync() const
{
  int enable = true;
  Handle(AIS_InteractiveContext) anIntCont = myModel->getAISContext();
  if ( !anIntCont.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast(anIntCont->CurrentViewer()->Driver());
    OpenGl_Caps* aCaps = &aDriver->ChangeOptions();
    enable = aCaps->swapInterval;
  #endif
  }
  return enable;
}

void OCCViewer_ViewWindow::setQuadBufferSupport( bool enable )
{
  Handle(AIS_InteractiveContext) anIntCont = myModel->getAISContext();
  if ( !anIntCont.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast(anIntCont->CurrentViewer()->Driver());
    OpenGl_Caps* aCaps = &aDriver->ChangeOptions();
    aCaps->contextStereo = enable;
  #endif
  }
}

bool OCCViewer_ViewWindow::isQuadBufferSupport() const
{
  int enable = true;
  Handle(AIS_InteractiveContext) anIntCont = myModel->getAISContext();
  if ( !anIntCont.IsNull() ) {
  #if OCC_VERSION_LARGE > 0x06090000
    Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast(anIntCont->CurrentViewer()->Driver());
    OpenGl_Caps* aCaps = &aDriver->ChangeOptions();
    enable = aCaps->contextStereo;
  #endif
  }
  return enable;
}


bool OCCViewer_ViewWindow::isOpenGlStereoSupport() const
{
  GLboolean support[1];
  glGetBooleanv (GL_STEREO, support);
  if ( support[0] )
    return true;
  return false;
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

void OCCViewer_ViewWindow::showStaticTrihedron( bool on )
{
  if ( myViewPort ) myViewPort->showStaticTrihedron( on );
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
  Standard_Real anUp[3];
  Standard_Real anAt[3];
  Standard_Real anEye[3];
  Standard_Real aProj[3];
  Standard_Real anAxialScale[3];

  aSourceView->Up( anUp[0], anUp[1], anUp[2] );
  aSourceView->At( anAt[0], anAt[1], anAt[2] );
  aSourceView->Proj( aProj[0], aProj[1], aProj[2] );
  getViewPort()->getAxialScale( anAxialScale[0], anAxialScale[1], anAxialScale[2] );

  aProps.setAxialScale( anAxialScale[0], anAxialScale[1], anAxialScale[2] );
  aProps.setViewUp( anUp[0], anUp[1], anUp[2] );

#if OCC_VERSION_LARGE > 0x06070100
  aSourceView->Eye( anEye[0], anEye[1], anEye[2] );

  // store camera properties "as is": it is up to synchronized
  // view classes to provide necessary property conversion.
  aProps.setPosition( anEye[0], anEye[1], anEye[2] );
  aProps.setFocalPoint( anAt[0], anAt[1], anAt[2] );

  if ( aSourceView->Camera()->IsOrthographic() )
  {
    aProps.setProjection( SUIT_CameraProperties::PrjOrthogonal );
    aProps.setViewAngle( 0.0 );
  }
  else
  {
    aProps.setProjection( SUIT_CameraProperties::PrjPerspective );
    aProps.setViewAngle( aSourceView->Camera()->FOVy() );
  }
  aProps.setMappingScale( aSourceView->Camera()->Scale() );
#else
  Standard_Real aCameraDepth = aSourceView->Depth() + aSourceView->ZSize() * 0.5;

  // generate view orientation matrix for transforming OCC projection reference point
  // into a camera (eye) position.
  gp_Dir aLeftDir = gp_Dir( anUp[0], anUp[1], anUp[2] ) ^ gp_Dir( aProj[0], aProj[1], aProj[2] );

  gp_GTrsf aTrsf;
  aTrsf.SetValue( 1, 1, aLeftDir.X() );
  aTrsf.SetValue( 2, 1, aLeftDir.Y() );
  aTrsf.SetValue( 3, 1, aLeftDir.Z() );

  aTrsf.SetValue( 1, 2, anUp[0] );
  aTrsf.SetValue( 2, 2, anUp[1] );
  aTrsf.SetValue( 3, 2, anUp[2] );

  aTrsf.SetValue( 1, 3, aProj[0] );
  aTrsf.SetValue( 2, 3, aProj[1] );
  aTrsf.SetValue( 3, 3, aProj[2] );

  aTrsf.SetValue( 1, 4, anAt[0] );
  aTrsf.SetValue( 2, 4, anAt[1] );
  aTrsf.SetValue( 3, 4, anAt[2] );

  Graphic3d_Vertex aProjRef = aSourceView->ViewMapping().ProjectionReferencePoint();

  // transform to world-space coordinate system
  gp_XYZ aPosition( aProjRef.X(), aProjRef.Y(), aCameraDepth );
  aTrsf.Transforms( aPosition );

  // compute focal point
  double aFocalPoint[3];

  aFocalPoint[0] = aPosition.X() - aProj[0] * aCameraDepth;
  aFocalPoint[1] = aPosition.Y() - aProj[1] * aCameraDepth;
  aFocalPoint[2] = aPosition.Z() - aProj[2] * aCameraDepth;

  aProps.setFocalPoint( aFocalPoint[0], aFocalPoint[1], aFocalPoint[2] );
  aProps.setPosition( aPosition.X(), aPosition.Y(), aPosition.Z() );

  Standard_Real aViewScale[2];
  aSourceView->Size( aViewScale[0], aViewScale[1] );
  aProps.setMappingScale( aViewScale[1] );
#endif

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
  double anAxialScale[3];

  // get common properties
  aProps.getFocalPoint( aFocalPoint[0], aFocalPoint[1], aFocalPoint[2] );
  aProps.getPosition( aPosition[0], aPosition[1], aPosition[2] );
  aProps.getViewUp( anUpDir[0], anUpDir[1], anUpDir[2] );
  aProps.getAxialScale( anAxialScale[0], anAxialScale[1], anAxialScale[2] );

#if OCC_VERSION_LARGE > 0x06070100
  aDestView->SetAt( aFocalPoint[0], aFocalPoint[1], aFocalPoint[2] );
  aDestView->SetEye( aPosition[0], aPosition[1], aPosition[2] );
  aDestView->SetUp( anUpDir[0], anUpDir[1], anUpDir[2] );
  aDestView->Camera()->SetScale( aProps.getMappingScale() );
#else
  gp_Dir aProjDir( aPosition[0] - aFocalPoint[0],
                   aPosition[1] - aFocalPoint[1],
                   aPosition[2] - aFocalPoint[2] );

  // get custom view translation
  Standard_Real aTranslation[3];
  aDestView->At( aTranslation[0], aTranslation[1], aTranslation[2] );

  gp_Dir aLeftDir = gp_Dir( anUpDir[0], anUpDir[1], anUpDir[2] )
                  ^ gp_Dir( aProjDir.X(), aProjDir.Y(), aProjDir.Z() );

  gp_GTrsf aTrsf;
  aTrsf.SetValue( 1, 1, aLeftDir.X() );
  aTrsf.SetValue( 2, 1, aLeftDir.Y() );
  aTrsf.SetValue( 3, 1, aLeftDir.Z() );

  aTrsf.SetValue( 1, 2, anUpDir[0] );
  aTrsf.SetValue( 2, 2, anUpDir[1] );
  aTrsf.SetValue( 3, 2, anUpDir[2] );

  aTrsf.SetValue( 1, 3, aProjDir.X() );
  aTrsf.SetValue( 2, 3, aProjDir.Y() );
  aTrsf.SetValue( 3, 3, aProjDir.Z() );

  aTrsf.SetValue( 1, 4, aTranslation[0] );
  aTrsf.SetValue( 2, 4, aTranslation[1] );
  aTrsf.SetValue( 3, 4, aTranslation[2] );
  aTrsf.Invert();

  // transform to view-space coordinate system
  gp_XYZ aProjRef( aPosition[0], aPosition[1], aPosition[2] );
  aTrsf.Transforms( aProjRef );

  // set view camera properties using low-level approach. this is done
  // in order to avoid interference with static variables in v3d view used
  // when rotation is in process in another view.
  Visual3d_ViewMapping aMapping = aDestView->View()->ViewMapping();
  Visual3d_ViewOrientation anOrientation = aDestView->View()->ViewOrientation();

  Graphic3d_Vector aMappingProj( aProjDir.X(), aProjDir.Y(), aProjDir.Z() );
  Graphic3d_Vector aMappingUp( anUpDir[0], anUpDir[1], anUpDir[2] );

  aMappingProj.Normalize();
  aMappingUp.Normalize();

  anOrientation.SetViewReferencePlane( aMappingProj );
  anOrientation.SetViewReferenceUp( aMappingUp );

  aDestView->SetViewMapping( aMapping );
  aDestView->SetViewOrientation( anOrientation );

  // set panning
  aDestView->SetCenter( aProjRef.X(), aProjRef.Y() );

  // set mapping scale
  double aMapScaling = aProps.getMappingScale();
  Standard_Real aWidth, aHeight;
  aDestView->Size( aWidth, aHeight );
  aDestView->SetSize ( aWidth > aHeight ? aMapScaling * (aWidth / aHeight) : aMapScaling );
#endif

  getViewPort()->setAxialScale( anAxialScale[0], anAxialScale[1], anAxialScale[2] );

  aDestView->ZFitAll();
  aDestView->SetImmediateUpdate( Standard_True );
  aDestView->Redraw();

  blockSignals( blocked );
}

/*!
  \brief Indicates whether preselection is enabled
  \return true if preselection is enabled
*/
bool OCCViewer_ViewWindow::isPreselectionEnabled() const
{
  return myPreselectionEnabled;
}

/*!
  \brief Enables/disables preselection
  \param theIsToEnable if true - preselection will be enabled
*/
void OCCViewer_ViewWindow::enablePreselection( bool theIsToEnable )
{
  onSwitchPreselection( theIsToEnable );
}

/*!
  \brief Indicates whether selection is enabled
  \return true if selection is enabled
*/
bool OCCViewer_ViewWindow::isSelectionEnabled() const
{
  return mySelectionEnabled;
}

/*!
  \brief Enables/disables selection
  \param theIsToEnable if true - selection will be enabled
*/
void OCCViewer_ViewWindow::enableSelection( bool theIsToEnable )
{
  onSwitchSelection( theIsToEnable );
}


/*!
  \brief called if clipping operation is activated / deactivated.

  Enables/disables clipping plane displaying.

  \parma on action state
*/
void OCCViewer_ViewWindow::onClipping (bool theIsOn)
{
  if(!myModel) return;
  OCCViewer_ClippingDlg* aClippingDlg = myModel->getClippingDlg();

  if (theIsOn) {
    if (!aClippingDlg) {
      aClippingDlg = new OCCViewer_ClippingDlg (this, myModel);
      myModel->setClippingDlg(aClippingDlg);
    }
    if (!aClippingDlg->isVisible())
      aClippingDlg->show();
  } else {
    if ( aClippingDlg ) {
      aClippingDlg->close();
      myModel->setClippingDlg(0);
    }
  }

  SUIT_ViewManager* mgr = getViewManager();
  if( mgr ) {
    QVector<SUIT_ViewWindow*> aViews = mgr->getViews();
    for(int i = 0, iEnd = aViews.size(); i < iEnd; i++) {
      if(SUIT_ViewWindow* aViewWindow = aViews.at(i)) {
	QtxActionToolMgr* mgr = aViewWindow->toolMgr();
	if(!mgr) continue;
	QAction* a = toolMgr()->action( ClippingId );
	if(!a) continue;
	if(theIsOn != a->isChecked()){
	  disconnect (a, SIGNAL (toggled (bool)), aViewWindow, SLOT (onClipping (bool)));
	  a->setChecked(theIsOn);
	  connect (a, SIGNAL (toggled (bool)), aViewWindow, SLOT (onClipping (bool)));
	}
      }
    }
  }
}

void OCCViewer_ViewWindow::onRayTracing()
{
  if( !OCCViewer_Utilities::isDialogOpened( this, OCCViewer_RayTracingDlg::getName() ) ) {
    QDialog* aDlg = new OCCViewer_RayTracingDlg( this );
    if ( aDlg != NULL )
      aDlg->show();
  }
}

void OCCViewer_ViewWindow::onEnvTexture()
{
  if( !OCCViewer_Utilities::isDialogOpened( this, OCCViewer_EnvTextureDlg::getName() ) ) {
    QDialog* aDlg = new OCCViewer_EnvTextureDlg( this );
    if ( aDlg != NULL )
      aDlg->show();
  }
}

void OCCViewer_ViewWindow::onLightSource()
{
  if( !OCCViewer_Utilities::isDialogOpened( this, OCCViewer_LightSourceDlg::getName() ) ) {
    QDialog* aDlg = new OCCViewer_LightSourceDlg( this, myModel );
    if ( aDlg != NULL )
      aDlg->show();
  }
}
