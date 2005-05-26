#ifndef SVTK_VIEWWINDOW_H
#define SVTK_VIEWWINDOW_H

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SVTK.h"
#include "SUIT_ViewWindow.h"

#include "SVTK_Selection.h"
#include "SALOME_InteractiveObject.hxx"

class vtkRenderer;

class QtxAction;
class SUIT_Desktop;

class VTKViewer_Trihedron;
class VTKViewer_Transform;

class SALOME_Actor;

class SVTK_Viewer;
class SVTK_Selector;

class SVTK_RenderWindow;
class SVTK_InteractorStyle;
class SVTK_RenderWindowInteractor;

class SVTK_EXPORT SVTK_ViewWindow : public SUIT_ViewWindow
{
  Q_OBJECT;

public:
  SVTK_ViewWindow( SUIT_Desktop*, SVTK_Viewer* );
  virtual ~SVTK_ViewWindow();
  
  QToolBar* getToolBar() { return myToolBar; }
  
  void setBackgroundColor( const QColor& );
  QColor backgroundColor() const;

  vtkRenderer* getRenderer() {return myRenderer;}
  SVTK_Selector* GetSelector() {return mySelector;}
  SVTK_RenderWindow* getRenderWindow() {return myRenderWindow;}
  SVTK_RenderWindowInteractor* getRWInteractor() {return myRWInteractor;}
  Selection_Mode SelectionMode() const;
  void SetSelectionMode(Selection_Mode theMode);

  bool isTrihedronDisplayed();
 
  /*  interactive object management */
  void highlight( const Handle(SALOME_InteractiveObject)& IObject, 
                  bool highlight, bool immediatly = true );
  void unHighlightAll();
  bool isInViewer( const Handle(SALOME_InteractiveObject)& IObject );
  bool isVisible( const Handle(SALOME_InteractiveObject)& IObject );

  /* selection */
  Handle(SALOME_InteractiveObject) FindIObject(const char* Entry);
  
  /* display */		
  void Display( const Handle(SALOME_InteractiveObject)& IObject,
                bool immediatly = true );
  void DisplayOnly( const Handle(SALOME_InteractiveObject)& IObject );
  void Erase( const Handle(SALOME_InteractiveObject)& IObject,
              bool immediatly = true );
  void DisplayAll();
  void EraseAll();
  void Repaint( bool theUpdateTrihedron );
  void Repaint() { Repaint(true); }

  //apply existing transformation on adding SALOME_Actor
  void SetScale( double theScale[3] );
  void GetScale( double theScale[3] );
  void AddActor( SALOME_Actor*, bool update = false );
  void RemoveActor(SALOME_Actor*, bool update = false);

  void AdjustTrihedrons( const bool forced );
  bool ComputeTrihedronSize( double& theNewSize,
			     double& theOldSize );
  double GetTrihedronSize() const;

public slots:
  void onSelectionChanged();

 signals:
 void selectionChanged();

public slots:
  void onFrontView(); 
  void onBackView(); 
  void onTopView();
  void onBottomView();
  void onRightView(); 
  void onLeftView();     

  void onResetView();     
  void onFitAll();
  void onDumpView();

  void onViewTrihedron(); 
  void onAdjustTrihedron();
 
  void onPanLeft();
  void onPanRight();
  void onPanUp();
  void onPanDown();
  void onZoomIn();
  void onZoomOut();
  void onRotateLeft();
  void onRotateRight();
  void onRotateUp();
  void onRotateDown();

  void activateZoom();
  void activateWindowFit();
  void activateRotation();
  void activatePanning(); 
  void activateGlobalPanning(); 

protected slots:
  void onKeyPressed(QKeyEvent* event);
  void onKeyReleased(QKeyEvent* event);
  void onMousePressed(QMouseEvent* event);
  void onMouseDoubleClicked(QMouseEvent* event);
  void onMouseReleased(QMouseEvent* event);
  void onMouseMoving(QMouseEvent* event);

private:
  void InitialSetup();
  void InsertActor( SALOME_Actor* theActor,
                    bool theMoveInternalActors = false );
  void MoveActor( SALOME_Actor* theActor );

private:  
  enum { DumpId, FitAllId, FitRectId, ZoomId, PanId, GlobalPanId, RotationId,
         FrontId, BackId, TopId, BottomId, LeftId, RightId, ResetId };
  typedef QMap<int, QtxAction*> ActionsMap;
  
  void createActions();
  void createToolBar();
  
  vtkRenderer* myRenderer;

  SVTK_Viewer* myModel;
  SVTK_Selector* mySelector;

  SVTK_RenderWindow* myRenderWindow;
  SVTK_RenderWindowInteractor* myRWInteractor;
  
  VTKViewer_Trihedron* myTrihedron;  
  VTKViewer_Transform* myTransform;
  
  QToolBar* myToolBar;
  ActionsMap myActionsMap;  
  
  double myCurScale;

  friend class SVTK_RenderWindowInteractor;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
