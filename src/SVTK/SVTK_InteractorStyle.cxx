//  SALOME VTKViewer : build VTK viewer into Salome desktop
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : 
//  Author : 
//  Module : SALOME
//  $Header$


#include "SVTK_InteractorStyle.h"

#include "VTKViewer_Utilities.h"
#include "SVTK_GenericRenderWindowInteractor.h"

#include "SVTK_Selection.h"
#include "SVTK_Event.h" 
#include "SVTK_Selector.h"
#include "SVTK_Functor.h"
#include "SVTK_Actor.h"

#include "VTKViewer_Algorithm.h"
#include "SVTK_Functor.h"

#include "SALOME_Actor.h"

#include <vtkObjectFactory.h>
#include <vtkMath.h>
#include <vtkCommand.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkPointPicker.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkRendererCollection.h>
#include <vtkDataSet.h>
#include <vtkPerspectiveTransform.h> 
#include <vtkMatrix4x4.h>

#include <qapplication.h>
#include <qpixmap.h>
//VRV: porting on Qt 3.0.5
#if QT_VERSION >= 0x030005
#include <qpainter.h>
#endif
//VRV: porting on Qt 3.0.5
#include <algorithm>

#include <iostream>

using namespace std;

namespace
{
  inline 
  void
  GetEventPosition(vtkRenderWindowInteractor* theInteractor,
		   int& theX, 
		   int& theY)
  {
    theInteractor->GetEventPosition(theX,theY);
    theY = theInteractor->GetSize()[1] - theY - 1;
  }
}


vtkStandardNewMacro(SVTK_InteractorStyle);


/*!
  Constructor
*/
SVTK_InteractorStyle
::SVTK_InteractorStyle():
  mySelectionEvent(new SVTK_SelectionEvent()),
  myPointPicker(vtkPointPicker::New()),
  myLastHighlitedActor(NULL),
  myLastPreHighlitedActor(NULL),
  myControllerIncrement(SVTK_ControllerIncrement::New()),
  myControllerOnKeyDown(SVTK_ControllerOnKeyDown::New()),
  myHighlightRotationPointActor(SVTK_Actor::New())
{
  myPointPicker->Delete();

  myPointPicker->SetTolerance(0.025);

  this->MotionFactor = 10.0;
  this->State = VTK_INTERACTOR_STYLE_CAMERA_NONE;
  this->RadianToDegree = 180.0 / vtkMath::Pi();
  this->ForcedState = VTK_INTERACTOR_STYLE_CAMERA_NONE;

  loadCursors();

  // set custom event handling function (to handle 3d space mouse events)
  EventCallbackCommand->SetCallback( SVTK_InteractorStyle::ProcessEvents );

  // set default values of properties.  user may edit them in preferences.
  mySMDecreaseSpeedBtn = 1;
  mySMIncreaseSpeedBtn = 2;
  mySMDominantCombinedSwitchBtn = 9;
  //
  myControllerIncrement->Delete();
  myControllerOnKeyDown->Delete();

  myCurrRotationPointType = SVTK::SetRotateGravity;
  myPrevRotationPointType = myCurrRotationPointType;

  myHighlightRotationPointActor->Delete();
  myHighlightRotationPointActor->Initialize();
  myHighlightRotationPointActor->PickableOff();
  myHighlightRotationPointActor->SetVisibility( false );
  
  myHighlightRotationPointActor->GetProperty()->SetPointSize(SALOME_POINT_SIZE+2);
  myHighlightRotationPointActor->GetProperty()->SetLineWidth(SALOME_LINE_WIDTH+2);
  myHighlightRotationPointActor->GetProperty()->SetRepresentationToPoints();

  myBBFirstCheck = true;
}

/*!
  Destructor
*/
SVTK_InteractorStyle
::~SVTK_InteractorStyle() 
{
}

/*!
  \return widget for rendering
*/
QWidget*
SVTK_InteractorStyle
::GetRenderWidget()
{
  return myInteractor->GetRenderWidget();
}

/*!
  \return selector
*/
SVTK_Selector*
SVTK_InteractorStyle
::GetSelector() 
{
  return myInteractor->GetSelector();
}

/*!
  Generate special SVTK_SelectionEvent
*/
SVTK_SelectionEvent*
SVTK_InteractorStyle
::GetSelectionEvent()
{
  mySelectionEvent->mySelectionMode = GetSelector()->SelectionMode();

  mySelectionEvent->myIsCtrl = Interactor->GetControlKey();
  mySelectionEvent->myIsShift = Interactor->GetShiftKey();

  mySelectionEvent->myLastX = mySelectionEvent->myX;
  mySelectionEvent->myLastY = mySelectionEvent->myY;

  GetEventPosition( this->Interactor, mySelectionEvent->myX, mySelectionEvent->myY );

  return mySelectionEvent.get();
}

/*!
  Generate special SVTK_SelectionEvent with flipped Y coordinate
*/
SVTK_SelectionEvent*
SVTK_InteractorStyle
::GetSelectionEventFlipY()
{
  mySelectionEvent->mySelectionMode = GetSelector()->SelectionMode();

  mySelectionEvent->myIsCtrl = Interactor->GetControlKey();
  mySelectionEvent->myIsShift = Interactor->GetShiftKey();

  mySelectionEvent->myLastX = mySelectionEvent->myX;
  mySelectionEvent->myLastY = mySelectionEvent->myY;

  this->Interactor->GetEventPosition(mySelectionEvent->myX, mySelectionEvent->myY);

  return mySelectionEvent.get();
}

void
SVTK_InteractorStyle
::RotateXY(int dx, int dy)
{
  /*   if(GetCurrentRenderer() == NULL)
    return;
  
  int *size = GetCurrentRenderer()->GetRenderWindow()->GetSize();
  double aDeltaElevation = -20.0 / size[1];
  double aDeltaAzimuth = -20.0 / size[0];
  
  double rxf = double(dx) * aDeltaAzimuth * this->MotionFactor;
  double ryf = double(dy) * aDeltaElevation * this->MotionFactor;
  
  vtkCamera *cam = GetCurrentRenderer()->GetActiveCamera();
  cam->Azimuth(rxf);
  cam->Elevation(ryf);
  cam->OrthogonalizeViewUp();

  GetCurrentRenderer()->ResetCameraClippingRange(); 

  this->Render();*/

  if(GetCurrentRenderer() == NULL)
    return;
  
  vtkCamera *cam = GetCurrentRenderer()->GetActiveCamera();

  double viewFP[3], viewPos[3];
  cam->GetFocalPoint(viewFP);
  cam->GetPosition(viewPos);

  if ( myCurrRotationPointType == SVTK::SetRotateGravity )
  {
    vtkFloatingPointType aCenter[3];
    if ( ComputeBBCenter(GetCurrentRenderer(),aCenter) ) 
    {
      myRotationPointX = aCenter[0];
      myRotationPointY = aCenter[1];
      myRotationPointZ = aCenter[2];
    }
  }

  // Calculate corresponding transformation
  vtkPerspectiveTransform* aTransform = vtkPerspectiveTransform::New();
  aTransform->Identity();
  aTransform->Translate(+myRotationPointX, +myRotationPointY, +myRotationPointZ);

  // Azimuth transformation
  int *size = GetCurrentRenderer()->GetRenderWindow()->GetSize();
  double aDeltaAzimuth = -20.0 / size[0];
  
  double rxf = double(dx) * aDeltaAzimuth * this->MotionFactor;
  aTransform->RotateWXYZ(rxf, cam->GetViewUp());

  // Elevation transformation
  double aDeltaElevation = -20.0 / size[1];

  double ryf = double(dy) * aDeltaElevation * this->MotionFactor;
  vtkMatrix4x4* aMatrix = cam->GetViewTransformMatrix();
  const double anAxis[3] = {-aMatrix->GetElement(0,0), // mkr : 27.11.2006 : PAL14011 - Strange behaviour in rotation in VTK Viewer.
			    -aMatrix->GetElement(0,1), 
			    -aMatrix->GetElement(0,2)};
  
  aTransform->RotateWXYZ(ryf, anAxis);
            
  aTransform->Translate(-myRotationPointX, -myRotationPointY, -myRotationPointZ);

  // To apply the transformation
  cam->SetPosition(aTransform->TransformPoint(viewPos));
  cam->SetFocalPoint(aTransform->TransformPoint(viewFP));

  cam->OrthogonalizeViewUp();

  GetCurrentRenderer()->ResetCameraClippingRange(); 

  this->Render();
}

void
SVTK_InteractorStyle
::PanXY(int x, int y, int oldX, int oldY)
{
  TranslateView(x, y, oldX, oldY);   
  this->Render();
}


void 
SVTK_InteractorStyle
::DollyXY(int dx, int dy)
{
  if (GetCurrentRenderer() == NULL) 
    return;

  double dxf = this->MotionFactor * (double)(dx) / (double)(GetCurrentRenderer()->GetCenter()[1]);
  double dyf = this->MotionFactor * (double)(dy) / (double)(GetCurrentRenderer()->GetCenter()[1]);

  double zoomFactor = pow((double)1.1, dxf + dyf);
  
  vtkCamera *aCam = GetCurrentRenderer()->GetActiveCamera();
  if (aCam->GetParallelProjection())
    aCam->SetParallelScale(aCam->GetParallelScale()/zoomFactor);
  else{
    aCam->Dolly(zoomFactor);
    GetCurrentRenderer()->ResetCameraClippingRange(); 
  }

  this->Render();
}

void 
SVTK_InteractorStyle
::SpinXY(int x, int y, int oldX, int oldY)
{
  vtkCamera *cam;

  if (GetCurrentRenderer() == NULL)
    return;

  double newAngle = atan2((double)(y - GetCurrentRenderer()->GetCenter()[1]),
			  (double)(x - GetCurrentRenderer()->GetCenter()[0]));
  double oldAngle = atan2((double)(oldY -GetCurrentRenderer()->GetCenter()[1]),
			  (double)(oldX - GetCurrentRenderer()->GetCenter()[0]));
  
  newAngle *= this->RadianToDegree;
  oldAngle *= this->RadianToDegree;

  cam = GetCurrentRenderer()->GetActiveCamera();
  cam->Roll(newAngle - oldAngle);
  cam->OrthogonalizeViewUp();
      
  this->Render();
}


/*!
  To reset reset view
*/
void
SVTK_InteractorStyle
::OnConfigure() 
{
  this->FindPokedRenderer(0,0);
  this->GetCurrentRenderer()->InvokeEvent(vtkCommand::ConfigureEvent,NULL);
}

/*!
  To handle mouse move event
*/
void
SVTK_InteractorStyle
::OnMouseMove() 
{
  int x, y;
  GetEventPosition( this->Interactor, x, y );
  this->OnMouseMove( this->Interactor->GetControlKey(),
		     this->Interactor->GetShiftKey(),
		     x, y );
}

/*!
  To handle left mouse button down event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnLeftButtonDown()
{
  int x, y;
  GetEventPosition( this->Interactor, x, y );
  this->OnLeftButtonDown( this->Interactor->GetControlKey(),
			  this->Interactor->GetShiftKey(),
			  x, y );
}

/*!
  To handle left mouse button up event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnLeftButtonUp()
{
  int x, y;
  GetEventPosition( this->Interactor, x, y );
  this->OnLeftButtonUp( this->Interactor->GetControlKey(),
			this->Interactor->GetShiftKey(),
			x, y );
}

/*!
  To handle middle mouse button down event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnMiddleButtonDown() 
{
  int x, y;
  GetEventPosition( this->Interactor, x, y );
  this->OnMiddleButtonDown( this->Interactor->GetControlKey(),
			    this->Interactor->GetShiftKey(),
			    x, y );
}

/*!
  To handle middle mouse button up event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnMiddleButtonUp()
{
  int x, y;
  GetEventPosition( this->Interactor, x, y );
  this->OnMiddleButtonUp( this->Interactor->GetControlKey(),
			  this->Interactor->GetShiftKey(),
			  x, y );
}

/*!
  To handle right mouse button down event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnRightButtonDown() 
{
  int x, y;
  GetEventPosition( this->Interactor, x, y );
  this->OnRightButtonDown( this->Interactor->GetControlKey(),
			   this->Interactor->GetShiftKey(),
			   x, y );
}

/*!
  To handle right mouse button up event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnRightButtonUp()
{
  int x, y;
  GetEventPosition( this->Interactor, x, y );
  this->OnRightButtonUp( this->Interactor->GetControlKey(),
			 this->Interactor->GetShiftKey(),
			 x, y );
}

/*!
  To handle mouse move event
*/
void
SVTK_InteractorStyle
::OnMouseMove(int vtkNotUsed(ctrl), 
	      int shift,
	      int x, int y) 
{
  myShiftState = shift;
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
    onOperation(QPoint(x, y));
  else if (ForcedState == VTK_INTERACTOR_STYLE_CAMERA_NONE)
    onCursorMove(QPoint(x, y));
}

/*!
  To handle left mouse button down event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnLeftButtonDown(int ctrl, int shift, 
		   int x, int y) 
{
  this->FindPokedRenderer(x, y);
  if(GetCurrentRenderer() == NULL)
    return;

  myShiftState = shift;
  // finishing current viewer operation
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  myOtherPoint = myPoint = QPoint(x, y);
  if (ForcedState != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    startOperation(ForcedState);
  } else {
    if (ctrl)
      startOperation(VTK_INTERACTOR_STYLE_CAMERA_ZOOM);
    else if ( myCurrRotationPointType == SVTK::StartPointSelection )
    {
      SVTK_SelectionEvent* aSelectionEvent = GetSelectionEventFlipY();

      SALOME_Actor* anActor = GetSelector()->Pick(aSelectionEvent, GetCurrentRenderer());
      
      if ( anActor )
      {
	myPointPicker->Pick( aSelectionEvent->myX,
			     aSelectionEvent->myY, 
			     0.0, 
			     GetCurrentRenderer() );
	int aVtkId = myPointPicker->GetPointId();
	if ( aVtkId >= 0 )
	{
	  int anObjId = anActor->GetNodeObjId( aVtkId );
	  vtkFloatingPointType* aCoords = anActor->GetNodeCoord(anObjId);
	  
	  myCurrRotationPointType = SVTK::SetRotateSelected;
	  
	  // invoke event for update coordinates in SVTK_SetRotationPointDlg
	  InvokeEvent(SVTK::RotationPointChanged,(void*)aCoords);
	}
	else
	{
	  // invoke event with no data (for SVTK_SetRotationPointDlg)
	  InvokeEvent(SVTK::RotationPointChanged,0);
	  myCurrRotationPointType = myPrevRotationPointType;
	}
      }
      else
      {
	// invoke event with no data (for SVTK_SetRotationPointDlg)
	InvokeEvent(SVTK::RotationPointChanged,0);
	myCurrRotationPointType = myPrevRotationPointType;
      }
    
      myHighlightRotationPointActor->SetVisibility( false );
      if(GetCurrentRenderer() != NULL)
	GetCurrentRenderer()->RemoveActor( myHighlightRotationPointActor.GetPointer() );

      GetRenderWidget()->setCursor(myDefCursor); 
    }
    else
      startOperation(VTK_INTERACTOR_STYLE_CAMERA_SELECT);
  }
  
  return;
}

/*!
  To handle left mouse button up event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnLeftButtonUp(int vtkNotUsed(ctrl),
		 int shift, 
		 int vtkNotUsed(x),
		 int vtkNotUsed(y))
{
  myShiftState = shift;
  // finishing current viewer operation
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
}

/*!
  To handle middle mouse button down event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnMiddleButtonDown(int ctrl,
		     int shift, 
		     int x, int y) 
{
  this->FindPokedRenderer(x, y);
  if(GetCurrentRenderer() == NULL)
    return;

  myShiftState = shift;
  // finishing current viewer operation
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  myOtherPoint = myPoint = QPoint(x, y);
  if (ForcedState != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    startOperation(ForcedState);
  }
  else {
    if (ctrl)
      startOperation(VTK_INTERACTOR_STYLE_CAMERA_PAN);
  }
}


/*!
  To handle middle mouse button up event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnMiddleButtonUp(int vtkNotUsed(ctrl),
		   int shift, 
		   int vtkNotUsed(x),
		   int vtkNotUsed(y))
{
  myShiftState = shift;
  // finishing current viewer operation
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
}


/*!
  To handle right mouse button down event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnRightButtonDown(int ctrl,
		    int shift, 
		    int x, int y) 
{
  this->FindPokedRenderer(x, y);
  if(GetCurrentRenderer() == NULL)
    return;

  myShiftState = shift;
  // finishing current viewer operation
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  myOtherPoint = myPoint = QPoint(x, y);
  if (ForcedState != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    startOperation(ForcedState);
  }
  else {
    if (ctrl)
      startOperation(VTK_INTERACTOR_STYLE_CAMERA_ROTATE);  
  }
}

/*!
  To handle right mouse button up event (reimplemented from vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::OnRightButtonUp(int vtkNotUsed(ctrl),
		  int shift, 
		  int vtkNotUsed(x),
		  int vtkNotUsed(y))
{
  myShiftState = shift;
  // finishing current viewer operation
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
}

/* XPM */
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


/*!
  loads cursors for viewer operations - zoom, pan, etc...
*/
void
SVTK_InteractorStyle
::loadCursors()
{
  myDefCursor       = QCursor(Qt::ArrowCursor);
  myHandCursor      = QCursor(Qt::PointingHandCursor);
  myPanCursor       = QCursor(Qt::SizeAllCursor);
  myZoomCursor      = QCursor(QPixmap(imageZoomCursor));
  myRotateCursor    = QCursor(QPixmap(imageRotateCursor));
  mySpinCursor      = QCursor(QPixmap(imageRotateCursor)); // temporarly !!!!!!
  myGlobalPanCursor = QCursor(Qt::CrossCursor);
  myCursorState     = false;
}


/*!
  Starts Zoom operation (e.g. through menu command)
*/
void
SVTK_InteractorStyle
::startZoom()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_ZOOM);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_ZOOM;
}


/*!
  Starts Pan operation (e.g. through menu command)
*/
void
SVTK_InteractorStyle
::startPan()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_PAN);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_PAN;
}

/*!
  Starts Rotate operation (e.g. through menu command)
*/
void 
SVTK_InteractorStyle
::startRotate()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_ROTATE);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_ROTATE;
}

/*!
  Set rotation point selected by user
*/
void 
SVTK_InteractorStyle
::startPointSelection()
{
  myCurrRotationPointType = SVTK::StartPointSelection;

  if(GetCurrentRenderer() != NULL) {
    GetCurrentRenderer()->AddActor( myHighlightRotationPointActor.GetPointer() );
    vtkFloatingPointType aColor[3];
    GetCurrentRenderer()->GetBackground( aColor );
    myHighlightRotationPointActor->GetProperty()->SetColor(1. - aColor[0],
							   1. - aColor[1],
							   1. - aColor[2]);
  }

  setCursor(VTK_INTERACTOR_STYLE_CAMERA_NONE);
}

/*! 
  Starts Spin operation (e.g. through menu command)
*/
void
SVTK_InteractorStyle
::startSpin()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_SPIN);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_SPIN;
}



/*!
  Starts Fit Area operation (e.g. through menu command)
*/
void
SVTK_InteractorStyle
::startFitArea()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_FIT);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_FIT;
}


/*!
  Starts Global Panning operation (e.g. through menu command)
*/
void
SVTK_InteractorStyle
::startGlobalPan()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN;

  // store current zoom scale
  myScale = GetCurrentRenderer()->GetActiveCamera()->GetParallelScale();

  GetCurrentRenderer()->ResetCamera();

  this->Render();
}


/*!
  Fits viewer contents to rect
*/
void
SVTK_InteractorStyle
::fitRect(const int left, 
	  const int top, 
	  const int right, 
	  const int bottom)
{
  if (GetCurrentRenderer() == NULL) 
    return;
 
  // move camera
  int x = (left + right)/2;
  int y = (top + bottom)/2;
  int *aSize = GetCurrentRenderer()->GetRenderWindow()->GetSize();
  int oldX = aSize[0]/2;
  int oldY = aSize[1]/2;
  TranslateView(oldX, oldY, x, y);

  // zoom camera
  double dxf = right == left ? 1.0 : (double)(aSize[0]) / (double)(abs(right - left));
  double dyf = bottom == top ? 1.0 : (double)(aSize[1]) / (double)(abs(bottom - top));
  double zoomFactor = (dxf + dyf)/2 ;

  vtkCamera *aCam = GetCurrentRenderer()->GetActiveCamera();
  if(aCam->GetParallelProjection())
    aCam->SetParallelScale(aCam->GetParallelScale()/zoomFactor);
  else{
    aCam->Dolly(zoomFactor);
    GetCurrentRenderer()->ResetCameraClippingRange();
  }
  
  this->Render();
}


/*!
  Starts viewer operation (!internal usage!)
*/
void
SVTK_InteractorStyle
::startOperation(int operation)
{
  switch(operation)
  { 
  case VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN:
  case VTK_INTERACTOR_STYLE_CAMERA_ZOOM:
  case VTK_INTERACTOR_STYLE_CAMERA_PAN:
  case VTK_INTERACTOR_STYLE_CAMERA_ROTATE:
  case VTK_INTERACTOR_STYLE_CAMERA_SPIN:
  case VTK_INTERACTOR_STYLE_CAMERA_FIT:
  case VTK_INTERACTOR_STYLE_CAMERA_SELECT:
    if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
      startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
    State = operation;
    if (State != VTK_INTERACTOR_STYLE_CAMERA_SELECT)
      setCursor(operation);
    onStartOperation();
    break;
  case VTK_INTERACTOR_STYLE_CAMERA_NONE:
  default:
    setCursor(VTK_INTERACTOR_STYLE_CAMERA_NONE);
    State = ForcedState = VTK_INTERACTOR_STYLE_CAMERA_NONE;
    break;
  }
}


/*!
  Sets proper cursor for window when viewer operation is activated
*/
void
SVTK_InteractorStyle
::setCursor(const int operation)
{
  if (!GetRenderWidget()) return;
  switch (operation)
  {
    case VTK_INTERACTOR_STYLE_CAMERA_ZOOM:
      GetRenderWidget()->setCursor(myZoomCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_PAN:
      GetRenderWidget()->setCursor(myPanCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_ROTATE:
      GetRenderWidget()->setCursor(myRotateCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_SPIN:
      GetRenderWidget()->setCursor(mySpinCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN:
      GetRenderWidget()->setCursor(myGlobalPanCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_FIT:
    case VTK_INTERACTOR_STYLE_CAMERA_SELECT:
      GetRenderWidget()->setCursor(myHandCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_NONE:
    default:
      if ( myCurrRotationPointType == SVTK::StartPointSelection )
	GetRenderWidget()->setCursor(myHandCursor);
      else
	GetRenderWidget()->setCursor(myDefCursor); 
      myCursorState = false;
      break;
  }
}


/*!
  Called when viewer operation started (!put necessary initialization here!)
*/
void
SVTK_InteractorStyle
::onStartOperation()
{
  if (!GetRenderWidget()) 
    return;

  vtkRenderWindowInteractor *aRWI = this->Interactor;
  vtkRenderWindow *aRenWin = aRWI->GetRenderWindow();
  aRenWin->SetDesiredUpdateRate(aRWI->GetDesiredUpdateRate());

  switch (State) {
    case VTK_INTERACTOR_STYLE_CAMERA_SELECT:
    case VTK_INTERACTOR_STYLE_CAMERA_FIT:
    {
      QPainter p(GetRenderWidget());
      p.setPen(Qt::lightGray);
      p.setRasterOp(Qt::XorROP);
      p.drawRect(QRect(myPoint, myOtherPoint));
      break;
    }
    case VTK_INTERACTOR_STYLE_CAMERA_ZOOM:
    case VTK_INTERACTOR_STYLE_CAMERA_PAN:
    case VTK_INTERACTOR_STYLE_CAMERA_ROTATE:
    case VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN:
    case VTK_INTERACTOR_STYLE_CAMERA_SPIN:
      break;
  }
}


/*!
  Called when viewer operation finished (!put necessary post-processing here!)
*/
void
SVTK_InteractorStyle
::onFinishOperation() 
{
  if (!GetRenderWidget()) 
    return;

  vtkRenderWindowInteractor *aRWI = this->Interactor;
  vtkRenderWindow *aRenWin = aRWI->GetRenderWindow();
  aRenWin->SetDesiredUpdateRate(aRWI->GetStillUpdateRate());

  SVTK_SelectionEvent* aSelectionEvent = GetSelectionEventFlipY();

  switch (State) {
    case VTK_INTERACTOR_STYLE_CAMERA_SELECT:
    case VTK_INTERACTOR_STYLE_CAMERA_FIT:
    {
      QPainter aPainter(GetRenderWidget());
      aPainter.setPen(Qt::lightGray);
      aPainter.setRasterOp(Qt::XorROP);
      QRect aRect(myPoint, myOtherPoint);
      aPainter.drawRect(aRect);
      aRect = aRect.normalize();

      if (State == VTK_INTERACTOR_STYLE_CAMERA_FIT) {
        // making fit rect opeation 
        int w, h;
        Interactor->GetSize(w, h);
        int x1 = aRect.left(); 
        int y1 = h - aRect.top() - 1;
        int x2 = aRect.right(); 
        int y2 = h - aRect.bottom() - 1;
        fitRect(x1, y1, x2, y2);
      }
      else {
        if (myPoint == myOtherPoint)
	  {
	    // process point selection
	    this->FindPokedRenderer(aSelectionEvent->myX, aSelectionEvent->myY);
	    Interactor->StartPickCallback();
	    
	    SALOME_Actor* anActor = GetSelector()->Pick(aSelectionEvent, GetCurrentRenderer());
	    
	    aSelectionEvent->myIsRectangle = false;

	    if(!myShiftState)
	      GetSelector()->ClearIObjects();

	    if(anActor)
	      {
		anActor->Highlight( this, aSelectionEvent, true );
	      }
	    else
	      {
		if(myLastHighlitedActor.GetPointer() && myLastHighlitedActor.GetPointer() != anActor)
		  myLastHighlitedActor->Highlight( this, aSelectionEvent, false );
		myLastHighlitedActor = anActor;
	      }
	  } 
	else 
	  {
	    //processing rectangle selection
	    Interactor->StartPickCallback();
	    GetSelector()->StartPickCallback();
	    aSelectionEvent->myIsRectangle = true;

	    if(!myShiftState)
	      GetSelector()->ClearIObjects();

	    vtkActorCollection* aListActors = GetCurrentRenderer()->GetActors();
	    aListActors->InitTraversal();
	    while(vtkActor* aActor = aListActors->GetNextActor())
	      {
		if(aActor->GetVisibility())
		  {
		    if(SALOME_Actor* aSActor = SALOME_Actor::SafeDownCast(aActor))
		      {
			if(aSActor->hasIO())
			  aSActor->Highlight( this, aSelectionEvent, true );
		      }
		  }
	      }
	  }
	Interactor->EndPickCallback();
	GetSelector()->EndPickCallback();
      } 
    } 
    break;
  case VTK_INTERACTOR_STYLE_CAMERA_ZOOM:
  case VTK_INTERACTOR_STYLE_CAMERA_PAN:
  case VTK_INTERACTOR_STYLE_CAMERA_ROTATE:
  case VTK_INTERACTOR_STYLE_CAMERA_SPIN:
    break;
  case VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN: 
    {
      int w, h, x, y;
      Interactor->GetSize(w, h);
      x = myPoint.x(); 
      y = h - myPoint.y() - 1;
      Place(x, y);
    }
    break;
  }

  this->Render();
}


/*!
  Called during viewer operation when user moves mouse (!put necessary processing here!)
*/
void
SVTK_InteractorStyle
::onOperation(QPoint mousePos) 
{
  if (!GetRenderWidget()) 
    return;

  switch (State) {
  case VTK_INTERACTOR_STYLE_CAMERA_PAN: 
    {
      this->PanXY(mousePos.x(), myPoint.y(), myPoint.x(), mousePos.y());
      myPoint = mousePos;
      break;
    }
  case VTK_INTERACTOR_STYLE_CAMERA_ZOOM: 
    {    
      this->DollyXY(mousePos.x() - myPoint.x(), mousePos.y() - myPoint.y());
      myPoint = mousePos;
      break;
    }
  case VTK_INTERACTOR_STYLE_CAMERA_ROTATE: 
    {
      this->RotateXY(mousePos.x() - myPoint.x(), myPoint.y() - mousePos.y());
      myPoint = mousePos;
      break;
    }
  case VTK_INTERACTOR_STYLE_CAMERA_SPIN: 
    {
      this->SpinXY(mousePos.x(), mousePos.y(), myPoint.x(), myPoint.y());
      myPoint = mousePos;
      break;
    }
  case VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN: 
    {    
      break;
    }
  case VTK_INTERACTOR_STYLE_CAMERA_SELECT:
    {
      if (!myCursorState)
        setCursor(VTK_INTERACTOR_STYLE_CAMERA_SELECT);
    }
  case VTK_INTERACTOR_STYLE_CAMERA_FIT:
    {
      QPainter p(GetRenderWidget());
      p.setPen(Qt::lightGray);
      p.setRasterOp(Qt::XorROP);
      p.drawRect(QRect(myPoint, myOtherPoint));
      myOtherPoint = mousePos;
      p.drawRect(QRect(myPoint, myOtherPoint));
      break;
    }
  }
}

/*!
  Called when user moves mouse inside viewer window and there is no active viewer operation 
  (!put necessary processing here!)
*/
void
SVTK_InteractorStyle
::onCursorMove(QPoint mousePos) 
{
  // processing highlighting
   SVTK_SelectionEvent* aSelectionEvent = GetSelectionEventFlipY();
  this->FindPokedRenderer(aSelectionEvent->myX,aSelectionEvent->myY);

  bool anIsChanged = false;

  SALOME_Actor *anActor = GetSelector()->Pick(aSelectionEvent, GetCurrentRenderer());
  
  if ( myCurrRotationPointType == SVTK::StartPointSelection )
  {
    myHighlightRotationPointActor->SetVisibility( false );

    if ( anActor )
    {
      myPointPicker->Pick( aSelectionEvent->myX, aSelectionEvent->myY, 0.0, GetCurrentRenderer() );
      int aVtkId = myPointPicker->GetPointId();
      if ( aVtkId >= 0 ) {
	int anObjId = anActor->GetNodeObjId( aVtkId );

	TColStd_IndexedMapOfInteger aMapIndex;
	aMapIndex.Add( anObjId );
	myHighlightRotationPointActor->MapPoints( anActor, aMapIndex );

	myHighlightRotationPointActor->SetVisibility( true );
	anIsChanged = true;
      }
    }
  }
  else {
    if (anActor){
      anIsChanged |= anActor->PreHighlight( this, aSelectionEvent, true );
    }

    if(myLastPreHighlitedActor.GetPointer() && myLastPreHighlitedActor.GetPointer() != anActor)
      anIsChanged |= myLastPreHighlitedActor->PreHighlight( this, aSelectionEvent, false );   

  }
  
  myLastPreHighlitedActor = anActor;

  if(anIsChanged)
    this->Render();
}

/*!
  Called on finsh GlobalPan operation 
*/
void
SVTK_InteractorStyle
::Place(const int theX, const int theY) 
{
  if (GetCurrentRenderer() == NULL)
    return;

  //translate view
  int *aSize = GetCurrentRenderer()->GetRenderWindow()->GetSize();
  int centerX = aSize[0]/2;
  int centerY = aSize[1]/2;

  TranslateView(centerX, centerY, theX, theY);

  // restore zoom scale
  vtkCamera *cam = GetCurrentRenderer()->GetActiveCamera();
  cam->SetParallelScale(myScale);
  GetCurrentRenderer()->ResetCameraClippingRange();

  this->Render();
}



/*!
  Translates view from Point to Point
*/
void
SVTK_InteractorStyle
::TranslateView(int toX, int toY, int fromX, int fromY)
{
  vtkCamera *cam = GetCurrentRenderer()->GetActiveCamera();
  double viewFocus[4], focalDepth, viewPoint[3];
  vtkFloatingPointType newPickPoint[4], oldPickPoint[4], motionVector[3];
  cam->GetFocalPoint(viewFocus);

  this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1],
			      viewFocus[2], viewFocus);
  focalDepth = viewFocus[2];

  this->ComputeDisplayToWorld(double(toX), double(toY),
			      focalDepth, newPickPoint);
  this->ComputeDisplayToWorld(double(fromX),double(fromY),
			      focalDepth, oldPickPoint);
  
  // camera motion is reversed
  motionVector[0] = oldPickPoint[0] - newPickPoint[0];
  motionVector[1] = oldPickPoint[1] - newPickPoint[1];
  motionVector[2] = oldPickPoint[2] - newPickPoint[2];
  
  cam->GetFocalPoint(viewFocus);
  cam->GetPosition(viewPoint);
  cam->SetFocalPoint(motionVector[0] + viewFocus[0],
		     motionVector[1] + viewFocus[1],
		     motionVector[2] + viewFocus[2]);
  cam->SetPosition(motionVector[0] + viewPoint[0],
		   motionVector[1] + viewPoint[1],
		   motionVector[2] + viewPoint[2]);
}

void
SVTK_InteractorStyle
::IncrementalPan( const int incrX, const int incrY )
{
  this->PanXY( incrX, incrY, 0, 0 );
}

void
SVTK_InteractorStyle
::IncrementalZoom( const int incr )
{
  this->DollyXY( incr, incr );
}

void
SVTK_InteractorStyle
::IncrementalRotate( const int incrX, const int incrY )
{
  this->RotateXY( incrX, -incrY );
}

/*!
  Redefined in order to add an observer (callback) for custorm event (space mouse event)
*/
void
SVTK_InteractorStyle
::SetInteractor( vtkRenderWindowInteractor* theInteractor )
{
  // register EventCallbackCommand as observer of standard events (keypress, mousemove, etc)
  Superclass::SetInteractor( theInteractor );
 
  myInteractor = dynamic_cast<SVTK_GenericRenderWindowInteractor*>(theInteractor);

  if(theInteractor) { 
    // register EventCallbackCommand as observer of custorm event (3d space mouse event)
    theInteractor->AddObserver( SVTK::SpaceMouseMoveEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::SpaceMouseButtonEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::PanLeftEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::PanRightEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::PanUpEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::PanDownEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::ZoomInEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::ZoomOutEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::RotateLeftEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::RotateRightEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::RotateUpEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::RotateDownEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::PlusSpeedIncrementEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::MinusSpeedIncrementEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::SetSpeedIncrementEvent, EventCallbackCommand, Priority );

    theInteractor->AddObserver( SVTK::SetSMDecreaseSpeedEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::SetSMIncreaseSpeedEvent, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::SetSMDominantCombinedSwitchEvent, EventCallbackCommand, Priority );

    theInteractor->AddObserver( SVTK::StartZoom, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::StartPan, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::StartRotate, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::StartGlobalPan, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::StartFitArea, EventCallbackCommand, Priority );

    theInteractor->AddObserver( SVTK::SetRotateGravity, EventCallbackCommand, Priority );
    theInteractor->AddObserver( SVTK::StartPointSelection, EventCallbackCommand, Priority );

    theInteractor->AddObserver( SVTK::ChangeRotationPoint, EventCallbackCommand, Priority );
  }
}

/*!
  To implement cached rendering
*/
void
SVTK_InteractorStyle
::OnTimer() 
{
  //vtkInteractorStyle::OnTimer();
  this->Interactor->Render();
  // check if bounding box was changed
  if ( GetCurrentRenderer() )
  {
    vtkFloatingPointType aCurrBBCenter[3];
    if ( ComputeBBCenter(GetCurrentRenderer(),aCurrBBCenter) )
    {
      if ( !myBBFirstCheck )
      {
	if ( fabs(aCurrBBCenter[0]-myBBCenter[0]) > 1e-38 ||
	     fabs(aCurrBBCenter[1]-myBBCenter[1]) > 1e-38 ||
	     fabs(aCurrBBCenter[2]-myBBCenter[2]) > 1e-38 ) {
	  // bounding box was changed => send SVTK::RotationPointChanged event
	  // invoke event for update coordinates in SVTK_SetRotationPointDlg
	  InvokeEvent(SVTK::BBCenterChanged,(void*)aCurrBBCenter);
	  for ( int i =0; i < 3; i++) myBBCenter[i] = aCurrBBCenter[i];
	}
      }
      else 
      {
	for ( int i =0; i < 3; i++) myBBCenter[i] = aCurrBBCenter[i];
	myBBFirstCheck = false;
      }
    }
  }
}

/*!
  To invoke #vtkRenderWindowInteractor::CreateTimer
*/
void
SVTK_InteractorStyle
::Render() 
{
  this->Interactor->CreateTimer(VTKI_TIMER_FIRST);
}

void
SVTK_InteractorStyle
::onSpaceMouseMove( double* data )
{
  // general things, do SetCurrentRenderer() within FindPokedRenderer() 
  int x, y;
  GetEventPosition( this->Interactor, x, y ); // current mouse position (from last mouse move event or any other event)
  FindPokedRenderer( x, y ); // calls SetCurrentRenderer
  
  IncrementalZoom( (int)data[2] );        // 1. push toward / pull backward = zoom out / zoom in
  IncrementalPan(  (int)data[0],  (int)data[1] );// 2. pull up / push down = pan up / down, 3. move left / right = pan left / right
  IncrementalRotate( 0,  (int)data[4] );   // 4. twist the control = rotate around Y axis
  IncrementalRotate( (int)data[3], 0  );   // 5. tilt the control forward/backward = rotate around X axis (Z axis of local coordinate system of space mouse)
}

void
SVTK_InteractorStyle
::onSpaceMouseButton( int button )
{
  if( mySMDecreaseSpeedBtn == button ) {   
    ControllerIncrement()->Decrease();
  }
  if( mySMIncreaseSpeedBtn == button ) {    
    ControllerIncrement()->Increase();
  }
  if( mySMDominantCombinedSwitchBtn == button )    
    DominantCombinedSwitch();
}

void
SVTK_InteractorStyle
::DominantCombinedSwitch()
{
  printf( "\n--DominantCombinedSwitch() NOT IMPLEMENTED--\n" );
}

/*!
  Main process event method (reimplemented from #vtkInteractorStyle)
*/
void
SVTK_InteractorStyle
::ProcessEvents( vtkObject* object,
		 unsigned long event,
		 void* clientData, 
		 void* callData )
{
  if ( clientData ) {
    vtkObject* anObject = reinterpret_cast<vtkObject*>( clientData );
    SVTK_InteractorStyle* self = dynamic_cast<SVTK_InteractorStyle*>( anObject );
    int aSpeedIncrement=self->ControllerIncrement()->Current();
    vtkFloatingPointType aCenter[3];
    vtkFloatingPointType* aSelectedPoint;
    if ( self ) {
      switch ( event ) {
      case SVTK::SpaceMouseMoveEvent : 
	self->onSpaceMouseMove( (double*)callData ); 
	return;
      case SVTK::SpaceMouseButtonEvent : 
	self->onSpaceMouseButton( *((int*)callData) ); 
	return;
      case SVTK::PanLeftEvent: 
	self->IncrementalPan(-aSpeedIncrement, 0);
	return;
      case SVTK::PanRightEvent:
	self->IncrementalPan(aSpeedIncrement, 0);
	return;
      case SVTK::PanUpEvent:
	self->IncrementalPan(0, aSpeedIncrement);
	return;
      case SVTK::PanDownEvent:
	self->IncrementalPan(0, -aSpeedIncrement);
	return;
      case SVTK::ZoomInEvent:
	self->IncrementalZoom(aSpeedIncrement);
	return;
      case SVTK::ZoomOutEvent:
	self->IncrementalZoom(-aSpeedIncrement);
	return;
      case SVTK::RotateLeftEvent: 
	self->IncrementalRotate(-aSpeedIncrement, 0);
	return;
      case SVTK::RotateRightEvent:
	self->IncrementalRotate(aSpeedIncrement, 0);
	return;
      case SVTK::RotateUpEvent:
	self->IncrementalRotate(0, -aSpeedIncrement);
	return;
      case SVTK::RotateDownEvent:
	self->IncrementalRotate(0, aSpeedIncrement);
	return;
      case SVTK::PlusSpeedIncrementEvent:
	self->ControllerIncrement()->Increase();
	return;
      case SVTK::MinusSpeedIncrementEvent:
	self->ControllerIncrement()->Decrease();
	return;
      case SVTK::SetSpeedIncrementEvent:
	self->ControllerIncrement()->SetStartValue(*((int*)callData));
	return;

      case SVTK::SetSMDecreaseSpeedEvent:
	self->mySMDecreaseSpeedBtn = *((int*)callData);
	return;
      case SVTK::SetSMIncreaseSpeedEvent:
	self->mySMIncreaseSpeedBtn = *((int*)callData);
	return;
      case SVTK::SetSMDominantCombinedSwitchEvent:
	self->mySMDominantCombinedSwitchBtn = *((int*)callData);
	return;

      case SVTK::StartZoom:
	self->startZoom();
	return;
      case SVTK::StartPan:
	self->startPan();
	return;
      case SVTK::StartRotate:
	self->startRotate();
	return;
      case SVTK::StartGlobalPan:
	self->startGlobalPan();
	return;
      case SVTK::StartFitArea:
	self->startFitArea();
	return;

      case SVTK::SetRotateGravity:
	if ( self->myCurrRotationPointType == SVTK::StartPointSelection )
	{
	  self->myHighlightRotationPointActor->SetVisibility( false );
	  if( self->GetCurrentRenderer() != NULL )
	    self->GetCurrentRenderer()->RemoveActor( self->myHighlightRotationPointActor.GetPointer() );
	  self->GetRenderWidget()->setCursor(self->myDefCursor); 
	}
	self->myPrevRotationPointType = self->myCurrRotationPointType;
	self->myCurrRotationPointType = SVTK::SetRotateGravity;
	if ( ComputeBBCenter(self->GetCurrentRenderer(),aCenter) ) 
	  // invoke event for update coordinates in SVTK_SetRotationPointDlg
	  self->InvokeEvent(SVTK::BBCenterChanged,(void*)aCenter);
	return;
      case SVTK::StartPointSelection:
	self->startPointSelection();
	return;

      case SVTK::ChangeRotationPoint:
	if ( self->myCurrRotationPointType == SVTK::StartPointSelection )
	{
	  self->myHighlightRotationPointActor->SetVisibility( false );
	  if( self->GetCurrentRenderer() != NULL )
	    self->GetCurrentRenderer()->RemoveActor( self->myHighlightRotationPointActor.GetPointer() );
	  self->GetRenderWidget()->setCursor(self->myDefCursor); 
	}
	self->myPrevRotationPointType = self->myCurrRotationPointType;
	self->myCurrRotationPointType = SVTK::SetRotateSelected;
	aSelectedPoint = (vtkFloatingPointType*)callData;
	self->myRotationPointX = aSelectedPoint[0];
	self->myRotationPointY = aSelectedPoint[1];
	self->myRotationPointZ = aSelectedPoint[2];
	return;
      }
    }
  }

  Superclass::ProcessEvents( object, event, clientData, callData );
}

/*!
  To handle keyboard event (reimplemented from #vtkInteractorStyle)
*/
void SVTK_InteractorStyle::OnChar()
{
}

/*!
  Redefined vtkInteractorStyle::OnKeyDown
*/
void SVTK_InteractorStyle::OnKeyDown()
{
  bool bInvokeSuperclass=myControllerOnKeyDown->OnKeyDown(this);
  if (bInvokeSuperclass){
    Superclass::OnKeyDown();
  }
}

/*!
  Provide instructions for Picking
*/
void SVTK_InteractorStyle::ActionPicking()
{
  int x, y;
  Interactor->GetEventPosition( x, y ); 
  FindPokedRenderer( x, y ); 
  
  myOtherPoint = myPoint = QPoint(x, y);
  
  startOperation(VTK_INTERACTOR_STYLE_CAMERA_SELECT);
  onFinishOperation();
  startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
}

/*!
  To set current increment controller 
*/
void SVTK_InteractorStyle::SetControllerOnKeyDown(SVTK_ControllerOnKeyDown* theController)
{
  myControllerOnKeyDown=theController;
}

/*!
  To get current OnKeyDown controller 
*/
SVTK_ControllerOnKeyDown* SVTK_InteractorStyle::ControllerOnKeyDown()
{
  return myControllerOnKeyDown.GetPointer();
}

/*!
  To get current increment controller
*/
void SVTK_InteractorStyle::SetControllerIncrement(SVTK_ControllerIncrement* theController)
{
  myControllerIncrement=theController;
}

/*!
  To get current increment controller 
*/
SVTK_ControllerIncrement* SVTK_InteractorStyle::ControllerIncrement()
{
  return myControllerIncrement.GetPointer();
}

vtkStandardNewMacro(SVTK_ControllerIncrement);
SVTK_ControllerIncrement::SVTK_ControllerIncrement()
{
  myIncrement=10;
}
SVTK_ControllerIncrement::~SVTK_ControllerIncrement()
{
}
void SVTK_ControllerIncrement::SetStartValue(const int theValue)
{
  myIncrement=theValue;
}
int SVTK_ControllerIncrement::Current()const
{
  return myIncrement;
}
int SVTK_ControllerIncrement::Increase()
{
  ++myIncrement;
  return myIncrement;
}
int SVTK_ControllerIncrement::Decrease()
{
  if (myIncrement>1){
    --myIncrement;
  }
  return myIncrement;
}

vtkStandardNewMacro(SVTK_ControllerOnKeyDown);

/*!
  Constructor
*/
SVTK_ControllerOnKeyDown::SVTK_ControllerOnKeyDown()
{
}

/*!
  Destructor
*/
SVTK_ControllerOnKeyDown::~SVTK_ControllerOnKeyDown()
{
}

bool SVTK_ControllerOnKeyDown::OnKeyDown(vtkInteractorStyle* theIS)
{
  return true;
}
