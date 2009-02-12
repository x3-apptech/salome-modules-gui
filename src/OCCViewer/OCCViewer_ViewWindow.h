//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef OCCVIEWER_VIEWWINDOW_H
#define OCCVIEWER_VIEWWINDOW_H

#include "OCCViewer.h"
#include "SUIT_ViewWindow.h"
#include <gp_Pnt.hxx>

class QtxRectRubberBand;
class SUIT_Desktop;
class OCCViewer_ViewPort3d;
class OCCViewer_ViewSketcher;
class OCCViewer_ClippingDlg;
class OCCViewer_AxialScaleDlg;
class OCCViewer_SetRotationPointDlg;
class OCCViewer_Viewer;
class viewAspect;
class QtxAction;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_ViewWindow : public SUIT_ViewWindow  
{
  Q_OBJECT

public:
  enum { DumpId, FitAllId, FitRectId, ZoomId, PanId, GlobalPanId,
	 ChangeRotationPointId, RotationId,
         FrontId, BackId, TopId, BottomId, LeftId, RightId, ResetId, CloneId, ClippingId, MemId, RestoreId,
         TrihedronShowId, AxialScaleId };

  enum OperationType{ NOTHING, PANVIEW, ZOOMVIEW, ROTATE, 
		      PANGLOBAL, WINDOWFIT, FITALLVIEW, RESETVIEW,
                      FRONTVIEW, BACKVIEW, TOPVIEW, BOTTOMVIEW, LEFTVIEW, RIGHTVIEW };

  enum RotationPointType{ GRAVITY, SELECTED };

  enum SketchingType { NoSketching, Rect, Polygon };

  OCCViewer_ViewWindow(SUIT_Desktop* theDesktop, OCCViewer_Viewer* theModel);
  virtual ~OCCViewer_ViewWindow();

  OCCViewer_ViewPort3d* getViewPort();

  bool eventFilter(QObject* watched, QEvent* e);

  void performRestoring( const viewAspect& );
  
  virtual void initLayout();

  void updateEnabledDrawMode();

  void setCuttingPlane( bool on, const double x = 0 , const double y = 0 , const double z = 0,
                                 const double dx = 0, const double dy = 0, const double dz = 1);

  bool isCuttingPlane();

  virtual QString   getVisualParameters();
  virtual void      setVisualParameters( const QString& parameters );

  virtual void            initSketchers();
  OCCViewer_ViewSketcher* getSketcher( const int );

  void                    activateSketching( int );
 
public slots:
  void onFrontView();
  void onViewFitAll();
  void onBackView();
  void onTopView();
  void onBottomView();
  void onLeftView();
  void onRightView();
  void onResetView();
  void onFitAll();
  void activateZoom();
  void activateWindowFit();
  void activateRotation();
  void activatePanning();
  void activateGlobalPanning();
  void onSetRotationPoint( bool on );
  void onCloneView();
  void onClipping( bool on );
  void onAxialScale();
  void onMemorizeView();
  void onRestoreView();
  void onTrihedronShow();
  void setRestoreFlag();

  void activateSetRotationGravity();
  void activateSetRotationSelected( double theX, double theY, double theZ );
  void activateStartPointSelection();
  void updateGravityCoords();
   
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );


signals:
  void vpTransformationStarted(OCCViewer_ViewWindow::OperationType type);
  void vpTransformationFinished(OCCViewer_ViewWindow::OperationType type);
  void viewCloned( SUIT_ViewWindow* );

  void Show( QShowEvent * );
  void Hide( QHideEvent * );

protected:
  virtual QImage dumpView();
  virtual bool   dumpViewToFormat( const QImage&, const QString& fileName, const QString& format );
  virtual QString  filter() const;

  /* Transformation selected but not started yet */
  bool transformRequested() const;
  void setTransformRequested ( OperationType );

  /* Transformation is selected and already started */
  bool		transformInProcess() const;
  void		setTransformInProcess( bool );

  void vpMousePressEvent(QMouseEvent* theEvent);
  void vpMouseReleaseEvent(QMouseEvent* theEvent);
  void vpMouseMoveEvent(QMouseEvent* theEvent);

  void resetState();
  void drawRect();
  void endDrawRect();

  void createActions();
  void createToolBar();
 
  virtual OperationType getButtonState(QMouseEvent* theEvent);

  viewAspect getViewParams() const;

  bool computeGravityCenter( double& theX, double& theY, double& theZ );

  virtual void                          onSketchingStarted();
  virtual void                          onSketchingFinished();

  virtual OCCViewer_ViewSketcher*       createSketcher( int );

  OCCViewer_ViewSketcher*               mypSketcher;
  QList<OCCViewer_ViewSketcher*>        mySketchers;

  int                                   myCurSketch;

  OperationType         myOperation;
  OCCViewer_Viewer*     myModel;
  OCCViewer_ViewPort3d* myViewPort;

  RotationPointType     myCurrPointType;
  RotationPointType     myPrevPointType;
  gp_Pnt                mySelectedPoint;
  bool                  myRotationPointSelection;

  int					myRestoreFlag;

  int					myStartX;
  int					myStartY;
  int					myCurrX;
  int					myCurrY;

  bool		        myEventStarted;       // set when transformation is in process 
  bool		        myCursorIsHand;			
  bool		        myDrawRect;           // set when a rect is used for selection or magnify 
  bool		        myEnableDrawMode;
  bool		        myPaintersRedrawing;  // set to draw with external painters 
 
  QCursor	        myCursor;

  double myCurScale;

private:
  OCCViewer_ClippingDlg* myClippingDlg;
  QtxAction* myClippingAction;

  OCCViewer_AxialScaleDlg* myScalingDlg;

  OCCViewer_SetRotationPointDlg* mySetRotationPointDlg;
  QtxAction* mySetRotationPointAction;

  QtxRectRubberBand* myRectBand; //!< selection rectangle rubber band
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
