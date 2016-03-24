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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : VTKViewer_InteractorStyle.cxx
//  Author : Christophe ATTANASIO

#include "VTKViewer_InteractorStyle.h"

#include "VTKViewer_Actor.h"
#include "VTKViewer_Utilities.h"
#include "VTKViewer_Trihedron.h"
#include "VTKViewer_ViewWindow.h"
#include "VTKViewer_RenderWindow.h"
#include "VTKViewer_RenderWindowInteractor.h"

#include <vtkObjectFactory.h>
#include <vtkMath.h>
#include <vtkCommand.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkPicker.h>
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>
#include <vtkLine.h> 
#include <vtkMapper.h>
#include <vtkDataSet.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>

#include <QApplication>
#include <QRubberBand>

#include <algorithm>

/*
static int GetEdgeId(vtkPicker *thePicker, SALOME_Actor *theActor, int theObjId){
  int anEdgeId = -1;
  if (vtkCell* aPickedCell = theActor->GetElemCell(theObjId)) {
    float aPickPosition[3];
    thePicker->GetPickPosition(aPickPosition);
    float aMinDist = 1000000.0, aDist = 0;
    for (int i = 0, iEnd = aPickedCell->GetNumberOfEdges(); i < iEnd; i++){
      if(vtkLine* aLine = vtkLine::SafeDownCast(aPickedCell->GetEdge(i))){
        int subId;  float pcoords[3], closestPoint[3], weights[3];
        aLine->EvaluatePosition(aPickPosition,closestPoint,subId,pcoords,aDist,weights);
        if (aDist < aMinDist) {
          aMinDist = aDist;
          anEdgeId = i;
        }
      }
    }
  }
  return anEdgeId;
}
*/

vtkStandardNewMacro(VTKViewer_InteractorStyle);


/*!Constructor.*/
VTKViewer_InteractorStyle::VTKViewer_InteractorStyle()
{
  m_Trihedron = 0;
  this->MotionFactor = 10.0;
  this->State = VTK_INTERACTOR_STYLE_CAMERA_NONE;
  this->RadianToDegree = 180.0 / vtkMath::Pi();
  this->ForcedState = VTK_INTERACTOR_STYLE_CAMERA_NONE;
  loadCursors();

  myPreSelectionActor = VTKViewer_Actor::New();
  myPreSelectionActor->GetProperty()->SetColor(0,1,1);
  myPreSelectionActor->GetProperty()->SetLineWidth(5);
  myPreSelectionActor->GetProperty()->SetPointSize(5);

  myRectBand = 0;

  OnSelectionModeChanged();
}


/*!Destructor.*/
VTKViewer_InteractorStyle::~VTKViewer_InteractorStyle() 
{
  m_ViewWnd->RemoveActor(myPreSelectionActor);
  endDrawRect();
}


/*!Set preselection properties.
 *\param theRed   - red color.
 *\param theGreen - green color.
 *\param theBlue  - blue color.
 *\param theWidth - width..
 */
void VTKViewer_InteractorStyle::setPreselectionProp(const double& theRed, const double& theGreen, 
                                                          const double& theBlue, const int& theWidth) 
{
  if ( myPreSelectionActor->GetProperty() == 0 )
    return;
  myPreSelectionActor->GetProperty()->SetColor(theRed, theGreen, theBlue);
  myPreSelectionActor->GetProperty()->SetLineWidth(theWidth);
  myPreSelectionActor->GetProperty()->SetPointSize(theWidth);
}


/*!Set render window interactor
 *\param theInteractor - interactor.
 */
void VTKViewer_InteractorStyle::SetInteractor(vtkRenderWindowInteractor *theInteractor){
  m_Interactor = dynamic_cast<VTKViewer_RenderWindowInteractor*>(theInteractor);
  Superclass::SetInteractor(theInteractor);
}


/*!Set view window.
 *\param theViewWnd - SALOME VTKViewer_ViewWindow
 */
void VTKViewer_InteractorStyle::setViewWnd(VTKViewer_ViewWindow* theViewWnd ){
  m_ViewWnd = theViewWnd;
  m_ViewWnd->AddActor(myPreSelectionActor);
  myPreSelectionActor->Delete();
}


/*!Set GUI window.
 *\param theWindow - QWidget window.
 */
void VTKViewer_InteractorStyle::setGUIWindow(QWidget* theWindow){
  myGUIWindow = theWindow;
}


/*!Set trihedron.
 *\param theTrihedron - SALOME VTKViewer_Trihedron
 */
void VTKViewer_InteractorStyle::setTriedron(VTKViewer_Trihedron* theTrihedron){
  m_Trihedron = theTrihedron;
}

/*!Rotate camera.
 *\param dx - 
 *\param dy - 
 */
void VTKViewer_InteractorStyle::RotateXY(int dx, int dy)
{
  double rxf;
  double ryf;
  vtkCamera *cam;
  
  if (this->CurrentRenderer == NULL)
    {
      return;
    }
  
  int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();
  this->DeltaElevation = -20.0 / size[1];
  this->DeltaAzimuth = -20.0 / size[0];
  
  rxf = (double)dx * this->DeltaAzimuth *  this->MotionFactor;
  ryf = (double)dy * this->DeltaElevation * this->MotionFactor;
  
  cam = this->CurrentRenderer->GetActiveCamera();
  cam->Azimuth(rxf);
  cam->Elevation(ryf);
  cam->OrthogonalizeViewUp();
  ::ResetCameraClippingRange(this->CurrentRenderer); 
  //this->Interactor->Render();
  myGUIWindow->update();
}

void VTKViewer_InteractorStyle::PanXY(int x, int y, int oldX, int oldY)
{
  TranslateView(x, y, oldX, oldY);   
  //this->Interactor->Render();
  myGUIWindow->update();
}


/*! Move the position of the camera along the direction of projection. (dx,dy)*/
void VTKViewer_InteractorStyle::DollyXY(int dx, int dy)
{
  if (this->CurrentRenderer == NULL) return;

  double dxf = this->MotionFactor * (double)(dx) / (double)(this->CurrentRenderer->GetCenter()[1]);
  double dyf = this->MotionFactor * (double)(dy) / (double)(this->CurrentRenderer->GetCenter()[1]);

  double zoomFactor = pow((double)1.1, dxf + dyf);
  
  vtkCamera *aCam = this->CurrentRenderer->GetActiveCamera();
  if (aCam->GetParallelProjection())
    aCam->SetParallelScale(aCam->GetParallelScale()/zoomFactor);
  else{
    aCam->Dolly(zoomFactor);
    ::ResetCameraClippingRange(this->CurrentRenderer);
  }

  //this->Interactor->Render();
  myGUIWindow->update();
}

void VTKViewer_InteractorStyle::SpinXY(int x, int y, int oldX, int oldY)
{
  vtkCamera *cam;

  if (this->CurrentRenderer == NULL)
    {
      return;
    }

  double newAngle = atan2((double)(y - this->CurrentRenderer->GetCenter()[1]),
                          (double)(x - this->CurrentRenderer->GetCenter()[0]));
  double oldAngle = atan2((double)(oldY -this->CurrentRenderer->GetCenter()[1]),
                          (double)(oldX - this->CurrentRenderer->GetCenter()[0]));
  
  newAngle *= this->RadianToDegree;
  oldAngle *= this->RadianToDegree;

  cam = this->CurrentRenderer->GetActiveCamera();
  cam->Roll(newAngle - oldAngle);
  cam->OrthogonalizeViewUp();
      
  //this->Interactor->Render();
  myGUIWindow->update();
}


/*!On mouse move event.
 *\param ctrl  - CTRL (not used)
 *\param shift - SHIFT (on/off - integer 0/1)
 *\param x - x coordinate
 *\param y - y coordinate
 */
void VTKViewer_InteractorStyle::OnMouseMove(int vtkNotUsed(ctrl), 
                                                  int shift,
                                                  int x, int y) 
{
  myShiftState = shift;
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
    onOperation(QPoint(x, y));
  else if (ForcedState == VTK_INTERACTOR_STYLE_CAMERA_NONE)
    onCursorMove(QPoint(x, y));
}


/*!On Left button down event.
 *\param ctrl  - CTRL  (on/off - integer 0/1)
 *\param shift - SHIFT (on/off - integer 0/1)
 *\param x - x coordinate
 *\param y - y coordinate
 */
void VTKViewer_InteractorStyle::OnLeftButtonDown(int ctrl, int shift, 
                                                       int x, int y) 
{
  if (this->HasObserver(vtkCommand::LeftButtonPressEvent)) {
    this->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL);
    return;
  }
  this->FindPokedRenderer(x, y);
  if (this->CurrentRenderer == NULL) {
    return;
  }
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
    else
      startOperation(VTK_INTERACTOR_STYLE_CAMERA_SELECT);
  }
  return;
}


/*!On left button up event.
 *\param ctrl  - CTRL  (not used)
 *\param shift - SHIFT (on/off - integer 0/1)
 *\param x - x coordinate (not used)
 *\param y - y coordinate (not used)
 */
void VTKViewer_InteractorStyle::OnLeftButtonUp(int vtkNotUsed(ctrl),
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


/*!On left button up event.
 *\param ctrl  - CTRL  (on/off - integer 0/1)
 *\param shift - SHIFT (on/off - integer 0/1)
 *\param x - x coordinate
 *\param y - y coordinate
 */
void VTKViewer_InteractorStyle::OnMiddleButtonDown(int ctrl,
                                                         int shift, 
                                                         int x, int y) 
{
  if (this->HasObserver(vtkCommand::MiddleButtonPressEvent)) 
    {
      this->InvokeEvent(vtkCommand::MiddleButtonPressEvent,NULL);
      return;
    }
  this->FindPokedRenderer(x, y);
  if (this->CurrentRenderer == NULL)
    {
      return;
    }
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


/*!On middle button up event.
 *\param ctrl  - CTRL  (not used)
 *\param shift - SHIFT (on/off - integer 0/1)
 *\param x - x coordinate (not used)
 *\param y - y coordinate (not used)
 */
void VTKViewer_InteractorStyle::OnMiddleButtonUp(int vtkNotUsed(ctrl),
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


/*!On right button down event.
 *\param ctrl  - CTRL  (on/off - integer 0/1)
 *\param shift - SHIFT (on/off - integer 0/1)
 *\param x - x coordinate
 *\param y - y coordinate
 */
void VTKViewer_InteractorStyle::OnRightButtonDown(int ctrl,
                                                        int shift, 
                                                        int x, int y) 
{
  if (this->HasObserver(vtkCommand::RightButtonPressEvent)) 
    {
      this->InvokeEvent(vtkCommand::RightButtonPressEvent,NULL);
      return;
    }
  this->FindPokedRenderer(x, y);
  if (this->CurrentRenderer == NULL)
    {
      return;
    }
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

/*!On right button up event.
 *\param ctrl  - CTRL  (not used)
 *\param shift - SHIFT (on/off - integer 0/1)
 *\param x - x coordinate (not used)
 *\param y - y coordinate (not used)
 */
void VTKViewer_InteractorStyle::OnRightButtonUp(int vtkNotUsed(ctrl),
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

/*! @name XPM - x pixmaps. */
//@{
/*!Image Zoom cursor*/
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

/*!Image rotate cursor*/
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
//@}

/*! Loads cursors for viewer operations - zoom, pan, etc...*/
void VTKViewer_InteractorStyle::loadCursors()
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


/*! event filter - controls mouse and keyboard events during viewer operations*/
bool VTKViewer_InteractorStyle::eventFilter(QObject* object, QEvent* event)
{
  if (!myGUIWindow) return false;
  if ( (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::KeyPress) && object != myGUIWindow)
  {
    qApp->removeEventFilter(this);
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  return QObject::eventFilter(object, event);
}


/*! starts Zoom operation (e.g. through menu command)*/
void VTKViewer_InteractorStyle::startZoom()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_ZOOM);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_ZOOM;
  qApp->installEventFilter(this);
}


/*! starts Pan operation (e.g. through menu command)*/
void VTKViewer_InteractorStyle::startPan()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_PAN);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_PAN;
  qApp->installEventFilter(this);
}

/*! starts Rotate operation (e.g. through menu command)*/
void VTKViewer_InteractorStyle::startRotate()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_ROTATE);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_ROTATE;
  qApp->installEventFilter(this);
}


/*! starts Spin operation (e.g. through menu command)*/
void VTKViewer_InteractorStyle::startSpin()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_SPIN);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_SPIN;
  qApp->installEventFilter(this);
}



/*! starts Fit Area operation (e.g. through menu command)*/
void VTKViewer_InteractorStyle::startFitArea()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_FIT);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_FIT;
  qApp->installEventFilter(this);
}


/*!View fit all.*/
void  VTKViewer_InteractorStyle::ViewFitAll() {
  int aTriedronWasVisible = false;
  if(m_Trihedron){
    aTriedronWasVisible = m_Trihedron->GetVisibility() == VTKViewer_Trihedron::eOn;
    if(aTriedronWasVisible) m_Trihedron->VisibilityOff();
  }

  if(m_Trihedron->GetVisibleActorCount(CurrentRenderer)){
    m_Trihedron->VisibilityOff();
    ::ResetCamera(CurrentRenderer);
  }else{
    m_Trihedron->SetVisibility(VTKViewer_Trihedron::eOnlyLineOn);
    ::ResetCamera(CurrentRenderer,true);
  }
  if(aTriedronWasVisible) m_Trihedron->VisibilityOn();
  else m_Trihedron->VisibilityOff();
  ::ResetCameraClippingRange(CurrentRenderer);
}

/*!View fit selection.*/
void  VTKViewer_InteractorStyle::ViewFitSelection() {

  vtkActorCollection* aSelectedCollection = vtkActorCollection::New();

  VTK::ActorCollectionCopy aCopy( CurrentRenderer->GetActors() );
  vtkActorCollection* aCollection = aCopy.GetActors();
  aCollection->InitTraversal();
  while ( vtkActor* aProp = aCollection->GetNextActor() )
    if ( VTKViewer_Actor* anActor = VTKViewer_Actor::SafeDownCast( aProp ) )
      if ( anActor->isPreselected() )
        aSelectedCollection->AddItem( aProp );

  double bounds[6];
  ::ComputeBounds( aSelectedCollection, bounds );

  if ( aSelectedCollection->GetNumberOfItems() && ::isBoundValid( bounds ) ) {
    CurrentRenderer->ResetCamera( bounds );
    CurrentRenderer->ResetCameraClippingRange( bounds );
  }
}


/*! starts Global Panning operation (e.g. through menu command)*/
void VTKViewer_InteractorStyle::startGlobalPan()
{
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE)
  {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  setCursor(VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN);
  ForcedState = VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN;

  // store current zoom scale
  vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();
  myScale = cam->GetParallelScale();

  ViewFitAll();

  if (myGUIWindow) myGUIWindow->update();
  
  qApp->installEventFilter(this);
}


/*!\retval \c true if needs redrawing*/
bool VTKViewer_InteractorStyle::needsRedrawing()
{
  return State == VTK_INTERACTOR_STYLE_CAMERA_ZOOM   ||
         State == VTK_INTERACTOR_STYLE_CAMERA_PAN    ||
         State == VTK_INTERACTOR_STYLE_CAMERA_ROTATE ||
         State == VTK_INTERACTOR_STYLE_CAMERA_SPIN   ||
         State == VTK_INTERACTOR_STYLE_CAMERA_NONE;
}


/*! fits viewer contents to rectangle
 *\param left - left side
 *\param top  - top side
 *\param right  - right side
 *\param bottom  - bottom side 
 */
void VTKViewer_InteractorStyle::fitRect(const int left, 
                                       const int top, 
                                       const int right, 
                                       const int bottom)
{
  if (this->CurrentRenderer == NULL) return;
 
  // move camera
  int x = (left + right)/2;
  int y = (top + bottom)/2;
  int *aSize = this->CurrentRenderer->GetRenderWindow()->GetSize();
  int oldX = aSize[0]/2;
  int oldY = aSize[1]/2;
  TranslateView(oldX, oldY, x, y);

  // zoom camera
  double dxf = (double)(aSize[0]) / (double)(abs(right - left));
  double dyf = (double)(aSize[1]) / (double)(abs(bottom - top));
  double zoomFactor = (dxf + dyf)/2 ;

  vtkCamera *aCam = this->CurrentRenderer->GetActiveCamera();
  if(aCam->GetParallelProjection())
    aCam->SetParallelScale(aCam->GetParallelScale()/zoomFactor);
  else{
    aCam->Dolly(zoomFactor);
    ::ResetCameraClippingRange(this->CurrentRenderer);
  }
  
  myGUIWindow->update();
}


/*! starts viewer operation (!internal usage!)*/
void VTKViewer_InteractorStyle::startOperation(int operation)
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


/*! sets proper cursor for window when viewer operation is activated*/
void VTKViewer_InteractorStyle::setCursor(const int operation)
{
  if (!myGUIWindow) return;
  switch (operation)
  {
    case VTK_INTERACTOR_STYLE_CAMERA_ZOOM:
      myGUIWindow->setCursor(myZoomCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_PAN:
      myGUIWindow->setCursor(myPanCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_ROTATE:
      myGUIWindow->setCursor(myRotateCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_SPIN:
      myGUIWindow->setCursor(mySpinCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN:
      myGUIWindow->setCursor(myGlobalPanCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_FIT:
    case VTK_INTERACTOR_STYLE_CAMERA_SELECT:
      myGUIWindow->setCursor(myHandCursor); 
      myCursorState = true;
      break;
    case VTK_INTERACTOR_STYLE_CAMERA_NONE:
    default:
      myGUIWindow->setCursor(myDefCursor); 
      myCursorState = false;
      break;
  }
}

/*!
  Draws rectangle by starting and current points
*/
void VTKViewer_InteractorStyle::drawRect()
{
  if ( !myRectBand ) {
    myRectBand = new QRubberBand( QRubberBand::Rectangle, myGUIWindow );
    QPalette palette;
    palette.setColor(myRectBand->foregroundRole(), Qt::white);
    myRectBand->setPalette(palette);
  }
  myRectBand->hide();

  QRect aRect(myPoint, myOtherPoint);
  myRectBand->setGeometry( aRect );
  myRectBand->setVisible( aRect.isValid() );
}

/*!
  \brief Delete rubber band on the end on the dragging operation.
*/
void VTKViewer_InteractorStyle::endDrawRect()
{
  delete myRectBand;
  myRectBand = 0;
}

/*! called when viewer operation started (!put necessary initialization here!)*/
void VTKViewer_InteractorStyle::onStartOperation()
{
  if (!myGUIWindow) return;
  // VSV: LOD actor activisation
  //  this->Interactor->GetRenderWindow()->SetDesiredUpdateRate(this->Interactor->GetDesiredUpdateRate());
  switch (State) {
    case VTK_INTERACTOR_STYLE_CAMERA_SELECT:
    case VTK_INTERACTOR_STYLE_CAMERA_FIT:
    {
      drawRect();
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


/*! called when viewer operation finished (!put necessary post-processing here!)*/
void VTKViewer_InteractorStyle::onFinishOperation() 
{
  if (!myGUIWindow) return;


//  SUIT_Study* aActiveStudy = SUIT_Application::getDesktop()->getActiveStudy();
//  SALOME_Selection* aSel    = SALOME_Selection::Selection( aActiveStudy->getSelection() );

  // VSV: LOD actor activisation
  //  rwi->GetRenderWindow()->SetDesiredUpdateRate(rwi->GetStillUpdateRate());

//  Selection_Mode aSelectionMode = aSel->SelectionMode();
//  bool aSelActiveCompOnly = aSel->IsSelectActiveCompOnly();

/*  switch (State) {
    case VTK_INTERACTOR_STYLE_CAMERA_SELECT:
    case VTK_INTERACTOR_STYLE_CAMERA_FIT:
    {
      QPainter p(myGUIWindow);
      p.setPen(Qt::lightGray);
      p.setRasterOp(Qt::XorROP);
      QRect rect(myPoint, myOtherPoint);
      p.drawRect(rect);
      rect = rect.normalize();
      if (State == VTK_INTERACTOR_STYLE_CAMERA_FIT) {
        // making fit rect opeation 
        int w, h;
        m_Interactor->GetSize(w, h);
        int x1, y1, x2, y2;
        x1 = rect.left(); 
        y1 = h - rect.top() - 1;
        x2 = rect.right(); 
        y2 = h - rect.bottom() - 1;
        fitRect(x1, y1, x2, y2);
      }
      else {
        if (myPoint == myOtherPoint) {
          // process point selection
          int w, h, x, y;
          m_Interactor->GetSize(w, h);
          x = myPoint.x(); 
          y = h - myPoint.y() - 1;

          this->FindPokedRenderer(x, y);
          m_Interactor->StartPickCallback();

          vtkPicker* aPicker = vtkPicker::SafeDownCast(m_Interactor->GetPicker());
          aPicker->Pick(x, y, 0.0, this->CurrentRenderer);
    
          SALOME_Actor* SActor = SALOME_Actor::SafeDownCast(aPicker->GetActor());

          if (vtkCellPicker* picker = vtkCellPicker::SafeDownCast(aPicker)) {
            int aVtkId = picker->GetCellId();
            if ( aVtkId >= 0 && SActor && SActor->hasIO() && IsValid( SActor, aVtkId ) ) {
              int anObjId = SActor->GetElemObjId(aVtkId);
              if(anObjId >= 0){
                Handle(SALOME_InteractiveObject) IO = SActor->getIO();
                if(aSelectionMode != EdgeOfCellSelection) {
                  if(CheckDimensionId(aSelectionMode,SActor,anObjId)){
                    if (IsSelected(IO,aSel)) {
                      // This IO is already in the selection
                      aSel->AddOrRemoveIndex( IO, anObjId, myShiftState, false );
                    } else {
                      if (!myShiftState) {
                        this->HighlightProp( NULL );
                        aSel->ClearIObjects();
                      }
                      aSel->AddOrRemoveIndex( IO, anObjId, myShiftState, false );
                      aSel->AddIObject( IO, false );
                    }
                  }
                }else{
                  if (!myShiftState) {
                    this->HighlightProp( NULL );
                    aSel->ClearIObjects();
                  }
                  int anEdgeId = GetEdgeId(picker,SActor,anObjId);
                  if (anEdgeId >= 0) {
                    aSel->AddOrRemoveIndex( IO, anObjId, true, false);
                    aSel->AddOrRemoveIndex( IO, -anEdgeId-1, true, true );
                    aSel->AddIObject( IO, false );
                  } 
                }
              }
            } else {
              this->HighlightProp( NULL );
              aSel->ClearIObjects();
            }
          } else if ( vtkPointPicker* picker = vtkPointPicker::SafeDownCast(aPicker) ) {
            int aVtkId = picker->GetPointId();
            if ( aVtkId >= 0 && IsValid( SActor, aVtkId, true ) ) {
              if ( SActor && SActor->hasIO() ) {
                int anObjId = SActor->GetNodeObjId(aVtkId);
                if(anObjId >= 0){
                  Handle(SALOME_InteractiveObject) IO = SActor->getIO();
                  if(IsSelected(IO,aSel)) {
                    // This IO is already in the selection
                    aSel->AddOrRemoveIndex( IO, anObjId, myShiftState, false );
                  } else {
                    if(!myShiftState) {
                      this->HighlightProp( NULL );
                      aSel->ClearIObjects();
                    }
                    aSel->AddOrRemoveIndex( IO, anObjId, myShiftState, false );
                    aSel->AddIObject( IO, false );
                  }
                }
              }
            } else {
              this->HighlightProp( NULL );
              aSel->ClearIObjects();
            } 
          } else {
            if ( SActor && SActor->hasIO() ) {
              this->PropPicked++;
              Handle(SALOME_InteractiveObject) IO = SActor->getIO();
              if(IsSelected(IO,aSel)) {
                // This IO is already in the selection
                if(myShiftState) {
                  aSel->RemoveIObject(IO);
                }
              }
              else {
                if(!myShiftState) {
                  this->HighlightProp( NULL );
                  aSel->ClearIObjects();
                }
                aSel->AddIObject( IO, false );
              }
            }else{
              // No selection clear all
              this->PropPicked = 0;
              this->HighlightProp( NULL );
              aSel->ClearIObjects();
            }
          }
          m_Interactor->EndPickCallback();
        } else {
          //processing rectangle selection
          QString aComponentDataType = SUIT_Application::getDesktop()->getComponentDataType();
          if(aSelActiveCompOnly && aComponentDataType.isEmpty()) return;
          m_Interactor->StartPickCallback();

          if (!myShiftState) {
            this->PropPicked = 0;
            this->HighlightProp( NULL );
            aSel->ClearIObjects();
          }

          // Compute bounds
          //      vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();
          QRect rect(myPoint, myOtherPoint);
          rect = rect.normalize();
          int w, h;
          m_Interactor->GetSize(w, h);
          int x1, y1, x2, y2;
          x1 = rect.left(); 
          y1 = h - rect.top() - 1;
          x2 = rect.right(); 
          y2 = h - rect.bottom() - 1;

          switch (aSelectionMode) {
          case NodeSelection: {
            if ( vtkPointPicker* aPointPicker = vtkPointPicker::SafeDownCast(m_Interactor->GetPicker()) ) {
              vtkActorCollection* aListActors = this->CurrentRenderer->GetActors();
              aListActors->InitTraversal();
              while (vtkActor* aActor = aListActors->GetNextActor()) {
                if (!aActor->GetVisibility()) 
                  continue;
                if(SALOME_Actor* SActor = SALOME_Actor::SafeDownCast(aActor)) {
                  if (SActor->hasIO()) {
                    Handle(SALOME_InteractiveObject) IO = SActor->getIO();
                    if (IO.IsNull()) 
                      continue;
                    if (aSelActiveCompOnly && aComponentDataType != IO->getComponentDataType())
                      continue;
                    if (vtkDataSet* aDataSet = SActor->GetInput()) {
                      SALOME_Selection::TContainerOfId anIndices;
                      for(int i = 0; i < aDataSet->GetNumberOfPoints(); i++) {
                        float aPoint[3];
                        aDataSet->GetPoint(i,aPoint);
                        if (IsInRect(aPoint,x1,y1,x2,y2)){
                          float aDisp[3];
                          ComputeWorldToDisplay(aPoint[0],aPoint[1],aPoint[2],aDisp);
                          if(aPointPicker->Pick(aDisp[0],aDisp[1],0.0,CurrentRenderer)){
                            if(vtkActorCollection *anActorCollection = aPointPicker->GetActors()){
                              if(anActorCollection->IsItemPresent(SActor)){
                                float aPickedPoint[3];
                                aPointPicker->GetMapperPosition(aPickedPoint);
                                vtkIdType aVtkId = aDataSet->FindPoint(aPickedPoint);
                                if ( aVtkId >= 0 && IsValid( SActor, aVtkId, true ) ){
                                  int anObjId = SActor->GetNodeObjId(aVtkId);
                                  anIndices.insert(anObjId);
                                }
                              }
                            }
                          }
                        }
                      }
                      if (!anIndices.empty()) {
                        aSel->AddOrRemoveIndex(IO, anIndices, true, false);
                        aSel->AddIObject(IO, false);
                        anIndices.clear();
                      }else{
                        aSel->RemoveIObject(IO, false);
                      }
                    }
                  }
                }
              }
            }
            break;
          }
          case CellSelection:
          case EdgeOfCellSelection:
          case EdgeSelection:
          case FaceSelection:
          case VolumeSelection: 
            {
              vtkSmartPointer<VTKViewer_CellAreaPicker> picker = VTKViewer_CellRectPicker::New();
              picker->SetTolerance(0.001);
              picker->Pick(x1, y1, 0.0, x2, y2, 0.0, this->CurrentRenderer);
              
              vtkActorCollection* aListActors = picker->GetActors();
              aListActors->InitTraversal();
              while(vtkActor* aActor = aListActors->GetNextActor()) {
                if (SALOME_Actor* aSActor = SALOME_Actor::SafeDownCast(aActor)) {
                  if (aSActor->hasIO()) {
                    Handle(SALOME_InteractiveObject) aIO = aSActor->getIO();
                    if (aSelActiveCompOnly && aComponentDataType != aIO->getComponentDataType())
                      continue;
                    VTKViewer_CellDataSet cellList = picker->GetCellData(aActor);
                    if ( !cellList.empty() ) {
                      SALOME_Selection::TContainerOfId anIndexes;
                      VTKViewer_CellDataSet::iterator it;
                      for ( it = cellList.begin(); it != cellList.end(); ++it ) {
                        int aCellId = (*it).cellId;
                        
                        if ( !IsValid( aSActor, aCellId ) )
                          continue;
                        
                        int anObjId = aSActor->GetElemObjId(aCellId);
                        if (anObjId != -1){
                          if ( CheckDimensionId(aSelectionMode,aSActor,anObjId) ) {
                            anIndexes.insert(anObjId);
                          }
                        }
                      }
                      aSel->AddOrRemoveIndex(aIO, anIndexes, true, false);
                      aSel->AddIObject(aIO, false);
                    }
                  }
                }
              }
            }
            break;          
          case ActorSelection: // objects selection
            {
              vtkSmartPointer<VTKViewer_AreaPicker> picker = VTKViewer_AreaPicker::New();
              picker->SetTolerance(0.001);
              picker->Pick(x1, y1, 0.0, x2, y2, 0.0, this->CurrentRenderer);

              vtkActorCollection* aListActors = picker->GetActors();
              SALOME_ListIO aListIO;
              aListActors->InitTraversal();
              while(vtkActor* aActor = aListActors->GetNextActor()) {
                if (SALOME_Actor* aSActor = SALOME_Actor::SafeDownCast(aActor)) {
                  if (aSActor->hasIO()) {
                    Handle(SALOME_InteractiveObject) aIO = aSActor->getIO();
                    if (!IsStored(aIO,aListIO))
                      aListIO.Append(aIO);
                  }
                }
              }
              if (!aListIO.IsEmpty()) {
                SALOME_ListIteratorOfListIO It(aListIO);
                for(;It.More();It.Next()) {
                  Handle(SALOME_InteractiveObject) IOS = It.Value();
                  this->PropPicked++;
                  aSel->AddIObject( IOS, false );
                }
              }
            } // end case 4
          } //end switch
          m_Interactor->EndPickCallback();
        }
        aActiveStudy->update3dViewers();
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
      m_Interactor->GetSize(w, h);
      x = myPoint.x(); 
      y = h - myPoint.y() - 1;
      Place(x, y);
    }
    break;
  }
  if (myGUIWindow) myGUIWindow->update();
*/
}

/*! called during viewer operation when user moves mouse (!put necessary processing here!)*/
void VTKViewer_InteractorStyle::onOperation(QPoint mousePos) 
{
  if (!myGUIWindow) return;
  int w, h;
  GetInteractor()->GetSize(w, h);
  switch (State) {
  case VTK_INTERACTOR_STYLE_CAMERA_PAN: 
    {
      // processing panning
      //this->FindPokedCamera(mousePos.x(), mousePos.y());
      this->PanXY(mousePos.x(), myPoint.y(), myPoint.x(), mousePos.y());
      myPoint = mousePos;
      break;
    }
  case VTK_INTERACTOR_STYLE_CAMERA_ZOOM: 
    {    
      // processing zooming
      //this->FindPokedCamera(mousePos.x(), mousePos.y());
      this->DollyXY(mousePos.x() - myPoint.x(), mousePos.y() - myPoint.y());
      myPoint = mousePos;
      break;
    }
  case VTK_INTERACTOR_STYLE_CAMERA_ROTATE: 
    {
      // processing rotation
      //this->FindPokedCamera(mousePos.x(), mousePos.y());
      this->RotateXY(mousePos.x() - myPoint.x(), myPoint.y() - mousePos.y());
      myPoint = mousePos;
      break;
    }
  case VTK_INTERACTOR_STYLE_CAMERA_SPIN: 
    {
      // processing spinning
      //this->FindPokedCamera(mousePos.x(), mousePos.y());
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
      myOtherPoint = mousePos;
      drawRect();
      break;
    }
  }
  this->LastPos[0] = mousePos.x();
  this->LastPos[1] = h - mousePos.y() - 1;
}

/*! called when selection mode changed (!put necessary initialization here!)*/
void VTKViewer_InteractorStyle::OnSelectionModeChanged()
{
  
  myPreSelectionActor->SetVisibility(false);
  myElemId = myEdgeId = myNodeId = -1;
  mySelectedActor = NULL;
}

/*! called when user moves mouse inside viewer window and there is no active viewer operation \n
 * (!put necessary processing here!)
 */
void VTKViewer_InteractorStyle::onCursorMove(QPoint mousePos) {
  // processing highlighting
//  SUIT_Study* anActiveStudy = SUIT_Application::getDesktop()->getActiveStudy();
//  SALOME_Selection* Sel = SALOME_Selection::Selection( anActiveStudy->getSelection() );
//  Selection_Mode aSelectionMode = Sel->SelectionMode();

/*  int w, h, x, y;
  m_Interactor->GetSize(w, h);
  x = mousePos.x(); y = h - mousePos.y() - 1;

  this->FindPokedRenderer(x,y);
  m_Interactor->StartPickCallback();
  myPreSelectionActor->SetVisibility(false);

  vtkPicker* aPicker = vtkPicker::SafeDownCast(m_Interactor->GetPicker());
  aPicker->Pick(x, y, 0.0, this->CurrentRenderer);

  SALOME_Actor* SActor = SALOME_Actor::SafeDownCast(aPicker->GetActor());

  if (vtkCellPicker* picker = vtkCellPicker::SafeDownCast(aPicker)) {
    int aVtkId = picker->GetCellId();
    if ( aVtkId >= 0 ) {
      int anObjId = SActor->GetElemObjId(aVtkId);
      if ( SActor && SActor->hasIO() && IsValid( SActor, aVtkId ) ) {
        bool anIsSameObjId = (mySelectedActor == SActor && myElemId == anObjId);
        bool aResult = anIsSameObjId;
        if(!anIsSameObjId) {
          if(aSelectionMode != EdgeOfCellSelection) {
            aResult = CheckDimensionId(aSelectionMode,SActor,anObjId);
            if(aResult){
              mySelectedActor = SActor;
              myElemId = anObjId;
              m_Interactor->setCellData(anObjId,SActor,myPreSelectionActor);
            }
          }
        }
        if(aSelectionMode == EdgeOfCellSelection){
          int anEdgeId = GetEdgeId(picker,SActor,anObjId);
          bool anIsSameEdgeId = (myEdgeId != anEdgeId) && anIsSameObjId;
          aResult = anIsSameEdgeId;
          if(!anIsSameEdgeId) {
            aResult = (anEdgeId >= 0);
            if (aResult) {
              mySelectedActor = SActor;
              myEdgeId = anEdgeId;
              myElemId = anObjId;
              m_Interactor->setEdgeData(anObjId,SActor,-anEdgeId-1,myPreSelectionActor);
            } 
          }
        }
        if(aResult) {
          myPreSelectionActor->GetProperty()->SetRepresentationToSurface();
          myPreSelectionActor->SetVisibility(true);
        }
      }
    }
  }
  else if (vtkPointPicker* picker = vtkPointPicker::SafeDownCast(aPicker)) {
    int aVtkId = picker->GetPointId();
    if ( aVtkId >= 0 && IsValid( SActor, aVtkId, true ) ) {
      if ( SActor && SActor->hasIO() ) {
        int anObjId = SActor->GetNodeObjId(aVtkId);
        bool anIsSameObjId = (mySelectedActor == SActor && myNodeId == anObjId);
        if(!anIsSameObjId) {
          mySelectedActor = SActor;
          myNodeId = anObjId;
          m_Interactor->setPointData(anObjId,SActor,myPreSelectionActor);
        }
        myPreSelectionActor->GetProperty()->SetRepresentationToSurface();
        myPreSelectionActor->SetVisibility(true);
      }
    }
  }
  else if ( vtkPicker::SafeDownCast(aPicker) ) {
    if ( SActor ) {
      if ( myPreViewActor != SActor ) {
        if ( myPreViewActor != NULL ) {
          myPreViewActor->SetPreSelected( false );
        }
        myPreViewActor = SActor;
              
        if ( SActor->hasIO() ) {
          Handle( SALOME_InteractiveObject) IO = SActor->getIO();
          if ( !IsSelected(IO,Sel) ) {
            // Find All actors with same IO
            vtkActorCollection* theActors = this->CurrentRenderer->GetActors();
            theActors->InitTraversal();
            while( vtkActor *ac = theActors->GetNextActor() ) {
              if ( SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( ac ) ) {
                if ( anActor->hasIO() ) {
                  Handle(SALOME_InteractiveObject) IOS = anActor->getIO();
                  if(IO->isSame(IOS)) {
                    anActor->SetPreSelected( true );
                  }
                }
              }
            }
          }
        }
      }
    } else {
      myPreViewActor = NULL;
      vtkActorCollection* theActors = this->CurrentRenderer->GetActors();
      theActors->InitTraversal();
      while( vtkActor *ac = theActors->GetNextActor() ) {
        if ( SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( ac ) ) {
          anActor->SetPreSelected( false );
        }
      }
    }
  }
  m_Interactor->EndPickCallback();
  //m_Interactor->Render();
  myGUIWindow->update();
  
  this->LastPos[0] = x;
  this->LastPos[1] = y;*/
}

/*! called on finsh GlobalPan operation */
void VTKViewer_InteractorStyle::Place(const int theX, const int theY) 
{
  if (this->CurrentRenderer == NULL) {
    return;
  }

  //translate view
  int *aSize = this->CurrentRenderer->GetRenderWindow()->GetSize();
  int centerX = aSize[0]/2;
  int centerY = aSize[1]/2;

  TranslateView(centerX, centerY, theX, theY);

  // restore zoom scale
  vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();
  cam->SetParallelScale(myScale);
  ::ResetCameraClippingRange(this->CurrentRenderer);

  if (myGUIWindow) myGUIWindow->update();

}



/*! Translates view from Point to Point*/
void VTKViewer_InteractorStyle::TranslateView(int toX, int toY, int fromX, int fromY)
{
  vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();
  double viewFocus[4], focalDepth, viewPoint[3];
  double newPickPoint[4], oldPickPoint[4], motionVector[3];
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


/*! Checks: is the given Actor within display coordinates?*/
bool VTKViewer_InteractorStyle::IsInRect(vtkActor* theActor, 
                                               const int left, const int top, 
                                               const int right, const int bottom)
{
  double* aBounds = theActor->GetBounds();
  double aMin[3], aMax[3];
  ComputeWorldToDisplay(aBounds[0], aBounds[2], aBounds[4], aMin);
  ComputeWorldToDisplay(aBounds[1], aBounds[3], aBounds[5], aMax);
  if (aMin[0] > aMax[0]) {
    double aBuf = aMin[0];
    aMin[0] = aMax[0];
    aMax[0] = aBuf;
  }
  if (aMin[1] > aMax[1]) {
    double aBuf = aMin[1];
    aMin[1] = aMax[1];
    aMax[1] = aBuf;    
  }

  return ((aMin[0]>left) && (aMax[0]<right) && (aMin[1]>bottom) && (aMax[1]<top));
}


/*! Checks: is the given Cell within display coordinates?*/
bool VTKViewer_InteractorStyle::IsInRect(vtkCell* theCell, 
                                               const int left, const int top, 
                                               const int right, const int bottom)
{
  double* aBounds = theCell->GetBounds();
  double aMin[3], aMax[3];
  ComputeWorldToDisplay(aBounds[0], aBounds[2], aBounds[4], aMin);
  ComputeWorldToDisplay(aBounds[1], aBounds[3], aBounds[5], aMax);
  if (aMin[0] > aMax[0]) {
    double aBuf = aMin[0];
    aMin[0] = aMax[0];
    aMax[0] = aBuf;
  }
  if (aMin[1] > aMax[1]) {
    double aBuf = aMin[1];
    aMin[1] = aMax[1];
    aMax[1] = aBuf;    
  }

  return ((aMin[0]>left) && (aMax[0]<right) && (aMin[1]>bottom) && (aMax[1]<top));
}

/*!Checks: is given point \a thePoint in rectangle*/
bool VTKViewer_InteractorStyle::IsInRect(double* thePoint, 
                                         const int left, const int top, 
                                         const int right, const int bottom)
{
  double aPnt[3];
  ComputeWorldToDisplay(thePoint[0], thePoint[1], thePoint[2], aPnt);

  return ((aPnt[0]>left) && (aPnt[0]<right) && (aPnt[1]>bottom) && (aPnt[1]<top));
}

/*!Set filter \a theFilter*/
void  VTKViewer_InteractorStyle::SetFilter( const Handle(VTKViewer_Filter)& theFilter )
{
  myFilters[ theFilter->GetId() ] = theFilter;
}

/*!Checks: is filter present (with id \a theId)
 *\param theId - filter id.
 */
bool  VTKViewer_InteractorStyle::IsFilterPresent( const int theId )
{
  return myFilters.find( theId ) != myFilters.end();
}

/*!Remove filter with id \a theId.
 *\param theId - filter id.
 */
void  VTKViewer_InteractorStyle::RemoveFilter( const int theId )
{
  if ( IsFilterPresent( theId ) )
    myFilters.erase( theId );
}

/*!Checks: is valid cell(node) with id \a theId in actor \a theActor.
 *\param theActor - VTKViewer_Actor pointer.
 *\param theId    - cell id.
 *\param theIsNode - boolean flag, if \c true - node, else - cell.
 */
bool VTKViewer_InteractorStyle::IsValid( VTKViewer_Actor* theActor,
                                               const int     theId,
                                               const bool    theIsNode )
{
  std::map<int, Handle(VTKViewer_Filter)>::const_iterator anIter;
  for ( anIter = myFilters.begin(); anIter != myFilters.end(); ++anIter )
  {
    const Handle(VTKViewer_Filter)& aFilter = anIter->second;
    if ( theIsNode == aFilter->IsNodeFilter() &&
         !aFilter->IsValid( theActor, theId ) )
      return false;
  }
  return true;
}

/*!Gets filter handle by filter id \a theId.*/
Handle(VTKViewer_Filter) VTKViewer_InteractorStyle::GetFilter( const int theId )
{
  return IsFilterPresent( theId ) ? myFilters[ theId ] : Handle(VTKViewer_Filter)();
}

/*!Increment pan.
 *\param incrX - X coordinate increment.
 *\param incrY - Y coordinate increment.
 */
void VTKViewer_InteractorStyle::IncrementalPan( const int incrX, const int incrY )
{
  this->PanXY( incrX, incrY, 0, 0 );
}

/*!Increment zoom.
 *\param incr - zoom increment.
 */
void VTKViewer_InteractorStyle::IncrementalZoom( const int incr )
{
  this->DollyXY( incr, incr );
}

/*!Increment rotate.
 *\param incrX - X coordinate increment.
 *\param incrY - Y coordinate increment.
 */
void VTKViewer_InteractorStyle::IncrementalRotate( const int incrX, const int incrY )
{
  this->RotateXY( incrX, -incrY );
}
























