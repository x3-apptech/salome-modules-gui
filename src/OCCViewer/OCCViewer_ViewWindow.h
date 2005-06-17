#ifndef OCCVIEWER_VIEWWINDOW_H
#define OCCVIEWER_VIEWWINDOW_H

#include "OCCViewer_ViewModel.h"

#include "SUIT_ViewWindow.h"

#include "QtxAction.h"

#include <qcursor.h>
#include <qvaluelist.h>

class SUIT_Desktop;
class OCCViewer_ViewPort3d;

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
  void onDumpView();
  void activateZoom();
  void activateWindowFit();
  void activateRotation();
  void activatePanning();
  void activateGlobalPanning();
  void onCloneView();
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
         FrontId, BackId, TopId, BottomId, LeftId, RightId, ResetId, CloneId, MemId, RestoreId,
         TrihedronShowId };

  typedef QMap<int, QtxAction*> ActionsMap;


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
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
