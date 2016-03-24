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
//  File   : VTKViewer_InteractorStyle.h
//  Author : Christophe ATTANASIO
//  Module : SALOME
//
#ifndef __VTKViewer_InteractorStyle_h
#define __VTKViewer_InteractorStyle_h

#include <vtkInteractorStyle.h>

class vtkCell;
class vtkRenderWindowInteractor;
class vtkTDxInteractorStyle;

#include <QObject>
#include <QCursor>

class QRubberBand;

#include <map>

#include "VTKViewer.h"

#include "VTKViewer_Filter.h"

class VTKViewer_Actor;
class VTKViewer_Trihedron;
class VTKViewer_ViewWindow;
class VTKViewer_RenderWindowInteractor;

#define VTK_INTERACTOR_STYLE_CAMERA_NONE    0
#define VTK_INTERACTOR_STYLE_CAMERA_ROTATE  1
#define VTK_INTERACTOR_STYLE_CAMERA_PAN     2
#define VTK_INTERACTOR_STYLE_CAMERA_ZOOM    3
#define VTK_INTERACTOR_STYLE_CAMERA_SPIN    4
#define VTK_INTERACTOR_STYLE_CAMERA_FIT        5
#define VTK_INTERACTOR_STYLE_CAMERA_SELECT     6
#define VTK_INTERACTOR_STYLE_CAMERA_GLOBAL_PAN 7

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*! Description:\n
 * This class must be supplied with a vtkRenderWindowInteractor wrapper or\n
 * parent. This class should not normally be instantiated by application\n
 * programmers.
 */
class VTKVIEWER_EXPORT VTKViewer_InteractorStyle : public QObject, public vtkInteractorStyle
{
 public:
  static VTKViewer_InteractorStyle *New();
  vtkTypeMacro(VTKViewer_InteractorStyle, vtkInteractorStyle);

  virtual void SetInteractor(vtkRenderWindowInteractor *theInteractor);
  void setViewWnd(VTKViewer_ViewWindow* theViewWnd);
  void setGUIWindow(QWidget* theWindow);

  void setTriedron(VTKViewer_Trihedron* theTrihedron);
  void setPreselectionProp(const double& theRed = 0, const double& theGreen = 1,
                           const double& theBlue = 1, const int& theWidth = 5);

  // Generic event bindings must be overridden in subclasses
  void OnMouseMove  (int ctrl, int shift, int x, int y);
  void OnLeftButtonDown(int ctrl, int shift, int x, int y);
  void OnLeftButtonUp  (int ctrl, int shift, int x, int y);
  void OnMiddleButtonDown(int ctrl, int shift, int x, int y);
  void OnMiddleButtonUp  (int ctrl, int shift, int x, int y);
  void OnRightButtonDown(int ctrl, int shift, int x, int y);
  void OnRightButtonUp  (int ctrl, int shift, int x, int y);

  void OnSelectionModeChanged();

  void ViewFitAll();
  void ViewFitSelection();

  void                     SetFilter( const Handle( VTKViewer_Filter)& );
  Handle(VTKViewer_Filter) GetFilter( const int );  
  bool                     IsFilterPresent( const int );
  void                     RemoveFilter( const int );
  bool                     IsValid( VTKViewer_Actor* theActor,
                                    const int     theId,
                                    const bool    theIsNode = false );

  void                     IncrementalPan   ( const int incrX, const int incrY );
  void                     IncrementalZoom  ( const int incr );
  void                     IncrementalRotate( const int incrX, const int incrY );

  int                      CurrentState() const { return State; }

 protected:
  VTKViewer_InteractorStyle();
  ~VTKViewer_InteractorStyle();
  VTKViewer_InteractorStyle(const VTKViewer_InteractorStyle&) {};
  void operator=(const VTKViewer_InteractorStyle&) {};

  void RotateXY(int dx, int dy);
  void PanXY(int x, int y, int oldX, int oldY);
  void DollyXY(int dx, int dy);
  void SpinXY(int dx, int dy, int oldX, int oldY);
  void fitRect(const int left, const int top, const int right, const int bottom);
  void Place(const int theX, const int theY);
  void TranslateView(int toX, int toY, int fromX, int fromY);
  bool IsInRect(vtkActor* theActor, 
                const int left, const int top, 
                const int right, const int bottom);
  bool IsInRect(vtkCell* theCell, 
                const int left, const int top, 
                const int right, const int bottom);
  bool IsInRect(double* thePoint, 
                const int left, const int top, 
                const int right, const int bottom);

  int State;
  double MotionFactor;
  double RadianToDegree;                 // constant: for conv from deg to rad
  double myScale;

  VTKViewer_Actor *myPreViewActor, *myPreSelectionActor, *mySelectedActor;

  int myElemId;
  int myEdgeId;
  int myNodeId;

 public:
  bool eventFilter(QObject* object, QEvent* event);
  void startZoom();
  void startPan();
  void startGlobalPan();
  void startRotate();
  void startFitArea();
  void startSpin();
  void SetTDxStyle(vtkTDxInteractorStyle *tdxStyle) {}
  bool needsRedrawing();

 protected:
  void loadCursors();
  void startOperation(int operation);
  virtual void onStartOperation();
  virtual void onFinishOperation();
  virtual void onOperation(QPoint mousePos);
  virtual void onCursorMove(QPoint mousePos);
  virtual void setCursor(const int operation);

  void drawRect();
  void endDrawRect();

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
  
  VTKViewer_RenderWindowInteractor* m_Interactor;
  VTKViewer_ViewWindow*     m_ViewWnd;
  VTKViewer_Trihedron*      m_Trihedron;
  QWidget*                  myGUIWindow;
  
  std::map<int, Handle(VTKViewer_Filter) > myFilters;

  QRubberBand* myRectBand; //!< selection rectangle rubber band

  /**  @name members from old version*/
  //@{
  double                    DeltaElevation;
  double                    DeltaAzimuth;
  int                       LastPos[2];
  //@}
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
