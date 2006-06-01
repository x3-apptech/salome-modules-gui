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
#ifndef OCCVIEWER_VIEWWINDOW_H
#define OCCVIEWER_VIEWWINDOW_H

#include "OCCViewer_ViewModel.h"

#include "SUIT_ViewWindow.h"

#include "QtxAction.h"

#include <qcursor.h>
#include <qvaluelist.h>

class SUIT_Desktop;
class OCCViewer_ViewPort3d;

class OCCViewer_ClippingDlg;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_ViewWindow : public SUIT_ViewWindow  
{
  Q_OBJECT

public:
  enum OperationType{ NOTHING, PANVIEW, ZOOMVIEW, ROTATE, PANGLOBAL, WINDOWFIT, FITALLVIEW, RESETVIEW,
                      FRONTVIEW, BACKVIEW, TOPVIEW, BOTTOMVIEW, LEFTVIEW, RIGHTVIEW };

  OCCViewer_ViewWindow(SUIT_Desktop* theDesktop, OCCViewer_Viewer* theModel);
	virtual ~OCCViewer_ViewWindow() {};

  OCCViewer_ViewPort3d* getViewPort() { return myViewPort; }

  bool eventFilter(QObject* watched, QEvent* e);

  QToolBar* getToolBar() { return myToolBar; }

  void performRestoring( const viewAspect& );
  
  virtual void initLayout();

  void updateEnabledDrawMode();

  void setCuttingPlane( bool on, const double x = 0 , const double y = 0 , const double z = 0,
                                 const double dx = 0, const double dy = 0, const double dz = 1);

  bool isCuttingPlane();

  virtual QString   getVisualParameters();
  virtual void      setVisualParameters( const QString& parameters );
 
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
  void onCloneView();
  void onClipping( bool on );
  void onMemorizeView();
  void onRestoreView();
  void onTrihedronShow();
  void setRestoreFlag();
  
signals:
  void vpTransformationStarted(OCCViewer_ViewWindow::OperationType type);
  void vpTransformationFinished(OCCViewer_ViewWindow::OperationType type);
  void cloneView();

protected:
  enum { DumpId, FitAllId, FitRectId, ZoomId, PanId, GlobalPanId, RotationId,
         FrontId, BackId, TopId, BottomId, LeftId, RightId, ResetId, CloneId, ClippingId, MemId, RestoreId,
         TrihedronShowId };

  typedef QMap<int, QtxAction*> ActionsMap;

  QImage dumpView();

  /* Transformation selected but not started yet */
  bool transformRequested() const { return ( myOperation != NOTHING ); }
  void setTransformRequested ( OperationType op );

  /* Transformation is selected and already started */
  bool		transformInProcess() const { return myEventStarted; }
  void		setTransformInProcess( bool bOn ) { myEventStarted = bOn; }

  void vpMousePressEvent(QMouseEvent* theEvent);
  void vpMouseReleaseEvent(QMouseEvent* theEvent);
  void vpMouseMoveEvent(QMouseEvent* theEvent);

  void resetState();
  void drawRect();

  void createActions();
  void createToolBar();
 
  virtual OperationType getButtonState(QMouseEvent* theEvent);

  viewAspect getViewParams() const;

  OperationType         myOperation;
  OCCViewer_Viewer*     myModel;
  OCCViewer_ViewPort3d* myViewPort;

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
 
  QRect		        myRect;				
  QCursor	        myCursor;

  QToolBar*  myToolBar;
  ActionsMap myActionsMap;

  double myCurScale;

private:
  OCCViewer_ClippingDlg* myClippingDlg;
  QtxAction* myClippingAction;
  
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
