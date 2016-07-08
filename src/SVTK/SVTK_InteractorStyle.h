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
//  File   : SVTK_InteractorStyle.h
//  Author : Christophe ATTANASIO

#ifndef __SVTK_InteractorStyle_h
#define __SVTK_InteractorStyle_h

#include "SVTK.h"

#include "SVTK_SelectionEvent.h"

#include <boost/shared_ptr.hpp>

#include <vtkInteractorStyle.h>
#include <vtkSmartPointer.h>

#include <QCursor>
#include <QtxRubberBand.h>

#include <map>

#include <vtkObject.h>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

//
//! Control the value of increment  in arithmetic progression mode.
/*!
  This class controls of value of increment,
  for pan/rotate/zoom operations in arithmetic progression mode
*/
class SVTK_EXPORT SVTK_ControllerIncrement : public vtkObject{
 public:
  vtkTypeMacro(SVTK_ControllerIncrement, vtkObject);
  static SVTK_ControllerIncrement* New();

  //! Set start value of increment
  void SetStartValue(const int );

  //! Get current value of increment
  int Current()const;

  //! Increace the increment value by add 1
  virtual int Increase();

  //! Decreace the increment value by subtract 1
  virtual int Decrease();
 protected:
  SVTK_ControllerIncrement();
  virtual ~SVTK_ControllerIncrement();
 protected:
  int  myIncrement;
 private:
  SVTK_ControllerIncrement(const SVTK_ControllerIncrement&);//Not implemented
  void operator=(const SVTK_ControllerIncrement&);          //Not implemented
};

//
//! Control the value of increment  in geometric progression mode.
/*!
  This class controls of value of increment,
  for pan/rotate/zoom operations in geometric progression mode.
*/
class SVTK_EXPORT SVTK_GeomControllerIncrement : public SVTK_ControllerIncrement{
 public:
  vtkTypeMacro(SVTK_GeomControllerIncrement, SVTK_ControllerIncrement);
  static SVTK_GeomControllerIncrement* New();

  //! Increace the increment value by add 1
  virtual int Increase();

  //! Decreace the increment value by subtract 1
  virtual int Decrease();
 protected:
  SVTK_GeomControllerIncrement();
  virtual ~SVTK_GeomControllerIncrement();
 private:
  SVTK_GeomControllerIncrement(const SVTK_GeomControllerIncrement&);//Not implemented
  void operator=(const SVTK_GeomControllerIncrement&);              //Not implemented
};
//
//! Control the behaviour of KeyDown event in SALOME way.
/*!
  This class controls the behaviour of KeyDown event
  in SALOME way
*/
class SVTK_EXPORT SVTK_ControllerOnKeyDown : public vtkObject{
 public:
  vtkTypeMacro(SVTK_ControllerOnKeyDown, vtkObject);
  static SVTK_ControllerOnKeyDown* New();

  //! Provides the action on event 
  virtual bool OnKeyDown(vtkInteractorStyle* );

 protected:
  SVTK_ControllerOnKeyDown();
  virtual ~SVTK_ControllerOnKeyDown();
  
 private:
  SVTK_ControllerOnKeyDown(const SVTK_ControllerOnKeyDown&);//Not implemented
  void operator=(const SVTK_ControllerOnKeyDown&);          //Not implemented
};

class vtkPointPicker;
class vtkTDxInteractorStyle;

class SALOME_Actor;

class SVTK_Selector;
class SVTK_GenericRenderWindowInteractor;
class SVTK_Actor;

class QtxRectRubberBand;

#define VTK_INTERACTOR_STYLE_CAMERA_NONE    0
#define VTK_INTERACTOR_STYLE_CAMERA_ROTATE  1
#define VTK_INTERACTOR_STYLE_CAMERA_PAN     2
#define VTK_INTERACTOR_STYLE_CAMERA_ZOOM    3
#define VTK_INTERACTOR_STYLE_CAMERA_SPIN    4
#define VTK_INTERACTOR_STYLE_CAMERA_FIT        5
#define VTK_INTERACTOR_STYLE_CAMERA_SELECT     6
#define VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN 7
#define VTK_INTERACTOR_STYLE_CAMERA_SELECT_ROTATION_POINT 8

enum PolygonState { Disable, Start, InProcess, Finished, Closed, NotValid };

//! Introduce SALOME way of user interaction
/*!
  This class defines SALOME way of user interaction for VTK viewer, as well, 
  as introduce a new selection mechanism
*/
class SVTK_EXPORT SVTK_InteractorStyle: public vtkInteractorStyle
{
 public:
  static SVTK_InteractorStyle *New();
  vtkTypeMacro(SVTK_InteractorStyle, vtkInteractorStyle);

  typedef boost::shared_ptr<SVTK_SelectionEvent> PSelectionEvent;

  void SetTDxStyle(vtkTDxInteractorStyle*){} 

  //! Generate special #SVTK_SelectionEvent
  virtual SVTK_SelectionEvent* GetSelectionEvent();

  //! Generate special #SVTK_SelectionEvent with flipped Y coordinate
  virtual SVTK_SelectionEvent* GetSelectionEventFlipY();

  //! Redefined in order to add an observer (callback) for custom event (space mouse event)
  virtual void SetInteractor( vtkRenderWindowInteractor* );

  //! To invoke #vtkRenderWindowInteractor::CreateTimer
  virtual void Render();

  //! To implement cached rendering
  virtual void OnTimer();

  //! To reset reset view
  virtual void OnConfigure();

  //! To handle mouse move event
  virtual void OnMouseMove();

  //! To handle left mouse button down event (reimplemented from #vtkInteractorStyle)
  virtual void OnLeftButtonDown();

  //! To handle left mouse button up event (reimplemented from #vtkInteractorStyle)
  virtual void OnLeftButtonUp();

  //! To handle middle mouse button down event (reimplemented from #vtkInteractorStyle)
  virtual void OnMiddleButtonDown();

  //! To handle middle mouse button up event (reimplemented from #vtkInteractorStyle)
  virtual void OnMiddleButtonUp();

  //! To handle right mouse button down event (reimplemented from #vtkInteractorStyle)
  virtual void OnRightButtonDown();

  //! To handle right mouse button up event (reimplemented from #vtkInteractorStyle)
  virtual void OnRightButtonUp();

  //! To handle mouse wheel forward event (reimplemented from #vtkInteractorStyle)
  virtual void OnMouseWheelForward();

  //! To handle mouse wheel backward event (reimplemented from #vtkInteractorStyle)
  virtual void OnMouseWheelBackward();

  //! To handle mouse button double click event
  virtual void OnMouseButtonDoubleClick();

  //! To handle keyboard event (reimplemented from #vtkInteractorStyle)
  virtual void OnChar();

  //! To set current increment controller 
  void SetControllerIncrement(SVTK_ControllerIncrement*);

  //! To modify current increment controller 
  void SetIncrementSpeed(const int, const int = 0);

  //! To get current increment controller 
  SVTK_ControllerIncrement* ControllerIncrement();
 
  //! Redefine vtkInteractorStyle::OnKeyDown
  virtual void OnKeyDown();

  //! Provide instructions for Picking
  void ActionPicking();

  //! To set current OnKeyDown controller 
  void SetControllerOnKeyDown(SVTK_ControllerOnKeyDown*);

  //! To get current OnKeyDown controller 
  SVTK_ControllerOnKeyDown* ControllerOnKeyDown();
  
  SVTK_Selector* GetSelector();

  int          CurrentState() const { return State; }
  PolygonState GetPolygonState() const { return myPoligonState; }
  void         SetPolygonState( const PolygonState& thePolygonState )
                              { myPoligonState = thePolygonState; }

  void SetAdvancedZoomingEnabled( const bool theState ) { myIsAdvancedZoomingEnabled = theState; }
  bool IsAdvancedZoomingEnabled() const { return myIsAdvancedZoomingEnabled; }

  //! releaze myLastHighlitedActor and myLastPreHighlitedActor
  void FreeActors();

  protected:
  SVTK_InteractorStyle();
  ~SVTK_InteractorStyle();

  QWidget*
  GetRenderWidget();
  
  // Generic event bindings must be overridden in subclasses
  virtual void OnMouseMove  (int ctrl, int shift, int x, int y);
  virtual void OnLeftButtonDown(int ctrl, int shift, int x, int y);
  virtual void OnLeftButtonUp  (int ctrl, int shift, int x, int y);
  virtual void OnMiddleButtonDown(int ctrl, int shift, int x, int y);
  virtual void OnMiddleButtonUp  (int ctrl, int shift, int x, int y);
  virtual void OnRightButtonDown(int ctrl, int shift, int x, int y);
  virtual void OnRightButtonUp  (int ctrl, int shift, int x, int y);

  void RotateXY(int dx, int dy);
  void PanXY(int x, int y, int oldX, int oldY);
  void DollyXY(int dx, int dy);
  void SpinXY(int dx, int dy, int oldX, int oldY);
  void fitRect(const int left, const int top, const int right, const int bottom);
  void Place(const int theX, const int theY);
  void TranslateView(int toX, int toY, int fromX, int fromY);

  void
  IncrementalPan( const int incrX, const int incrY );

  void
  IncrementalZoom( const int incr );

  void
  IncrementalRotate( const int incrX, const int incrY );

  // Main process event method (reimplemented from #vtkInteractorStyle)
  static 
  void
  ProcessEvents(vtkObject* object, 
                unsigned long event,
                void* clientData, 
                void* callData );

  float MotionFactor;
  float RadianToDegree;                 // constant: for conv from deg to rad
  double myScale;

 protected:
  void startZoom();
  void startPan();
  void startGlobalPan();
  void startRotate();
  void startFitArea();
  void startSpin();

  void startPointSelection();
  void startFocalPointSelection();

 protected:
  void loadCursors();
  void startOperation(int operation);
  void onStartOperation();
  virtual void onFinishOperation();
  void onOperation(QPoint mousePos);
  void onCursorMove(QPoint mousePos);
  void setCursor(const int operation);

  void onSpaceMouseMove( double* data );
  virtual void onSpaceMouseButton( int button );

  void DominantCombinedSwitch();
  
  void drawRect();
  void endDrawRect();
  void drawPolygon();
  void endDrawPolygon();

 protected:
  QCursor                   myDefCursor;
  QCursor                   myPanCursor;
  QCursor                   myZoomCursor;
  QCursor                   myRotateCursor;
  QCursor                   mySpinCursor;
  QCursor                   myHandCursor;
  QCursor                   myGlobalPanCursor;
  QPoint                    myPoint;
  QPoint                    myOtherPoint;
  bool                      myCursorState;
  bool                      myShiftState;
  int                       ForcedState;

  vtkSmartPointer<SALOME_Actor> myLastHighlitedActor;
  vtkSmartPointer<SALOME_Actor> myLastPreHighlitedActor;

  //! "Increment" controller for pan/rotate/zoom operations
  vtkSmartPointer<SVTK_ControllerIncrement> myControllerIncrement;

  //!controls the behaviour of KeyDown event in SALOME way
  vtkSmartPointer<SVTK_ControllerOnKeyDown> myControllerOnKeyDown;

  // SpaceMouse short cuts
  int                       mySMDecreaseSpeedBtn;
  int                       mySMIncreaseSpeedBtn;
  int                       mySMDominantCombinedSwitchBtn;
  
  vtkSmartPointer<SVTK_GenericRenderWindowInteractor> myInteractor;

  PSelectionEvent mySelectionEvent;

  unsigned long                   myCurrRotationPointType;
  unsigned long                   myPrevRotationPointType;

  unsigned long                   myCurrFocalPointType;
  unsigned long                   myPrevFocalPointType;

  double                          myRotationPointX;
  double                          myRotationPointY;
  double                          myRotationPointZ;

  vtkSmartPointer<SVTK_Actor>     myHighlightSelectionPointActor;
  vtkSmartPointer<vtkPointPicker> myPointPicker;
  
  double                          myBBCenter[3];
  bool                            myBBFirstCheck;

  QtxRectRubberBand*              myRectBand; //!< selection rectangle rubber band
  QtxPolyRubberBand*              myPolygonBand; //!< polygonal selection
  QVector<QPoint>                 myPolygonPoints;
  PolygonState                    myPoligonState;

  bool                            myIsAdvancedZoomingEnabled;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
