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
#ifndef SVTK_VIEWWINDOW_H
#define SVTK_VIEWWINDOW_H

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SVTK.h"
#include "SVTK_Selection.h"
#include "SUIT_ViewWindow.h"

#include "SALOME_InteractiveObject.hxx"

#include <QImage>
#include <vtkSmartPointer.h>

class SUIT_Desktop;
class SUIT_ResourceMgr;

class VTKViewer_Actor;
class VTKViewer_Trihedron;

class SVTK_ViewModelBase;
//class SVTK_MainWindow;
class SVTK_Selector;
class SVTK_View;

class SVTK_CubeAxesActor2D;

class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkInteractorStyle;
class SVTK_RenderWindowInteractor;
class SVTK_Renderer;
class SVTK_NonIsometricDlg;
class SVTK_UpdateRateDlg;
class SVTK_CubeAxesDlg;
class SVTK_SetRotationPointDlg;
class SVTK_KeyFreeInteractorStyle;
class SVTK_ViewParameterDlg;
class SVTK_Recorder;

class vtkObject;
class QtxAction;

namespace SVTK
{
  SVTK_EXPORT
    int convertAction( const int );
}

//! Define a container for SALOME VTK view window
class SVTK_EXPORT SVTK_ViewWindow : public SUIT_ViewWindow
{
  Q_OBJECT;

 public:
  //! To construct #SVTK_ViewWindow instance
  SVTK_ViewWindow(SUIT_Desktop* theDesktop);

  virtual ~SVTK_ViewWindow();
  
  virtual QImage dumpView();

  //! To initialize #SVTK_ViewWindow instance
  virtual void Initialize(SVTK_ViewModelBase* theModel);

  //! Get #SVTK_View
  SVTK_View* getView();

  //! Get #SVTK_MainWindow
  //SVTK_MainWindow* getMainWindow();

  //! Redirect the request to #SVTK_MainWindow::getRenderWindow
  vtkRenderWindow* getRenderWindow();

  //! Redirect the request to #SVTK_MainWindow::getInteractor
  vtkRenderWindowInteractor* getInteractor() const;

  //! Redirect the request to #SVTK_MainWindow::getInteractor
  SVTK_RenderWindowInteractor*  GetInteractor() const;

  vtkInteractorStyle* GetInteractorStyle() const;

  //! Redirect the request to #SVTK_RenderWindowInteractor::PushInteractorStyle
  void PushInteractorStyle(vtkInteractorStyle* theStyle);

  //! Redirect the request to #SVTK_RenderWindowInteractor::PopInteractorStyle
  void PopInteractorStyle();

  //! Redirect the request to #SVTK_MainWindow::getRenderer 
  vtkRenderer* getRenderer() const;

  SVTK_Renderer* GetRenderer() const;

  //! Redirect the request to #SVTK_MainWindow::GetSelector 
  SVTK_Selector* GetSelector() const;
  
  //! Redirect the request to #SVTK_Selector::SelectionMode
  Selection_Mode SelectionMode() const;
  
  //! Change selection mode
  virtual void SetSelectionMode(Selection_Mode theMode);

  //! Redirect the request to #SVTK_MainWindow::SetBackgroundColor 
  virtual void setBackgroundColor( const QColor& );

  //! Redirect the request to #SVTK_MainWindow::SetBackgroundColor 
  QColor backgroundColor() const;

  //! Redirect the request to #SVTK_Renderer::IsTrihedronDisplayed
  bool isTrihedronDisplayed();

  //! Redirect the request to #SVTK_Renderer::IsCubeAxesDisplayed
  bool isCubeAxesDisplayed();
 
  /*  interactive object management */
  //! Redirect the request to #SVTK_View::highlight (to support old code)
  virtual void highlight(const Handle(SALOME_InteractiveObject)& theIO, 
			 bool theIsHighlight = true, 
			 bool theIsUpdate = true);

  //! Redirect the request to #SVTK_View::unHighlightAll (to support old code)
  virtual void unHighlightAll();

  //! Redirect the request to #SVTK_View::isInViewer (to support old code)
  bool isInViewer(const Handle(SALOME_InteractiveObject)& theIObject);

  //! Redirect the request to #SVTK_View::isVisible (to support old code)
  bool isVisible(const Handle(SALOME_InteractiveObject)& theIObject);

  //! Redirect the request to #SVTK_View::FindIObject (to support old code)
  //----------------------------------------------------------------------------
  Handle(SALOME_InteractiveObject) FindIObject(const char* theEntry);
  
  /* display */		
  //----------------------------------------------------------------------------
  //! Redirect the request to #SVTK_View::Display (to support old code)
  virtual void Display(const Handle(SALOME_InteractiveObject)& theIObject,
		       bool theImmediatly = true);

  //! Redirect the request to #SVTK_View::DisplayOnly (to support old code)
  virtual void DisplayOnly(const Handle(SALOME_InteractiveObject)& theIObject);

  //! Redirect the request to #SVTK_View::Erase (to support old code)
  virtual void Erase(const Handle(SALOME_InteractiveObject)& theIObject,
		     bool theImmediatly = true);

  //! Redirect the request to #SVTK_View::DisplayAll (to support old code)
  virtual void DisplayAll();

  //! Redirect the request to #SVTK_View::EraseAll (to support old code)
  virtual void EraseAll();

  //! To repaint the viewer
  virtual void Repaint(bool theUpdateTrihedron = true);

  //----------------------------------------------------------------------------
  //! Redirect the request to #SVTK_Renderer::SetScale
  virtual void SetScale( double theScale[3] );

  //! Redirect the request to #SVTK_Renderer::GetScale
  virtual void GetScale( double theScale[3] );

  //! Redirect the request to #SVTK_Renderer::AddActor
  virtual void AddActor(VTKViewer_Actor* theActor,
			bool theIsUpdate = false);

  //! Redirect the request to #SVTK_Renderer::RemoveActor
  virtual void RemoveActor(VTKViewer_Actor* theActor,
			   bool theIsUpdate = false);

  //----------------------------------------------------------------------------
  //! Redirect the request to #SVTK_Renderer::AdjustActors
  virtual void AdjustTrihedrons(const bool theIsForced);

  //! Redirect the request to #SVTK_Renderer::GetTrihedron
  VTKViewer_Trihedron* GetTrihedron();

  //! Redirect the request to #SVTK_Renderer::GetCubeAxes
  SVTK_CubeAxesActor2D* GetCubeAxes();

  //! Redirect the request to #SVTK_Renderer::GetTrihedronSize
  vtkFloatingPointType GetTrihedronSize() const;

  //! Redirect the request to #SVTK_Renderer::SetTrihedronSize
  virtual void SetTrihedronSize( const vtkFloatingPointType, const bool = true );

  //! Redirect the request to #SVTK_MainWindow::SetIncrementalSpeed
  virtual void SetIncrementalSpeed( const int, const int = 0 );

  //! Redirect the request to #SVTK_MainWindow::SetProjectionMode
  virtual void SetProjectionMode( const int );

  //! Redirect the request to #SVTK_MainWindow::SetInteractionStyle
  virtual void SetInteractionStyle( const int );

  //! Redirect the request to #SVTK_MainWindow::SetSpacemouseButtons
  virtual void SetSpacemouseButtons( const int, const int, const int );

  //! Redirect the request to #SVTK_Renderer::SetSelectionProp
  virtual void SetSelectionProp(const double& theRed = 1, 
				const double& theGreen = 1,
				const double& theBlue = 0, 
				const int& theWidth = 5);

  //! Redirect the request to #SVTK_Renderer::SetPreselectionProp
  virtual void SetPreselectionProp(const double& theRed = 0, 
				   const double& theGreen = 1,
				   const double& theBlue = 1, 
				   const int& theWidth = 5);

  //! Redirect the request to #SVTK_Renderer::SetSelectionTolerance
  virtual void SetSelectionTolerance(const double& theTolNodes = 0.025, 
				     const double& theTolCell = 0.001,
				     const double& theTolObjects = 0.025);

  //! Methods to save/restore visual parameters of a view (pan, zoom, etc.)
  virtual QString getVisualParameters();
  
  virtual void setVisualParameters( const QString& parameters );

  virtual bool eventFilter( QObject*, QEvent* );

  virtual void RefreshDumpImage();

  //! To invoke a VTK event on #SVTK_RenderWindowInteractor instance
  void InvokeEvent(unsigned long theEvent, void* theCallData);
  
 signals:
  void Show( QShowEvent * );
  void Hide( QHideEvent * );

public slots:
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );
  virtual void onSelectionChanged();

  void onChangeRotationPoint(bool theIsActivate);

  void activateSetRotationGravity();
  void activateSetRotationSelected(void* theData);
  void activateStartPointSelection();

  void onUpdateRate(bool theIsActivate);
  void onNonIsometric(bool theIsActivate);
  void onGraduatedAxes(bool theIsActivate);

  void activateZoom();
  void activateWindowFit();
  void activateRotation();
  void activatePanning(); 
  void activateGlobalPanning(); 

  void onPerspectiveMode();

  void activateProjectionMode(int);

  void activateSetFocalPointGravity();
  void activateSetFocalPointSelected();
  void activateStartFocalPointSelection();

  void onViewParameters(bool theIsActivate);

  void onSwitchInteractionStyle(bool theOn);

  void onStartRecording();
  void onPlayRecording();
  void onPauseRecording();
  void onStopRecording();

signals:
 void selectionChanged();
 void actorAdded(VTKViewer_Actor*);
 void actorRemoved(VTKViewer_Actor*);

public slots:
  //! Redirect the request to #SVTK_Renderer::OnFrontView
  virtual void onFrontView(); 

  //! Redirect the request to #SVTK_Renderer::OnBackView
  virtual void onBackView(); 

  //! Redirect the request to #SVTK_Renderer::OnTopView
  virtual void onTopView();

  //! Redirect the request to #SVTK_Renderer::OnBottomView
  virtual void onBottomView();

  //! Redirect the request to #SVTK_Renderer::OnRightView
  virtual void onRightView(); 

  //! Redirect the request to #SVTK_Renderer::OnLeftView
  virtual void onLeftView();     

  //! Redirect the request to #SVTK_Renderer::OnResetView
  virtual void onResetView();     

  //! Redirect the request to #SVTK_Renderer::OnFitAll
  virtual void onFitAll();

  //! Redirect the request to #SVTK_Renderer::OnViewTrihedron
  virtual void onViewTrihedron(); 

  //! Redirect the request to #SVTK_Renderer::OnViewCubeAxes
  virtual void onViewCubeAxes();

  //! Redirect the request to #SVTK_Renderer::OnAdjustTrihedron
  virtual void onAdjustTrihedron();

  //! Redirect the request to #SVTK_Renderer::OnAdjustCubeAxes
  virtual void onAdjustCubeAxes();

protected slots:
  void onKeyPressed(QKeyEvent* event);
  void onKeyReleased(QKeyEvent* event);
  void onMousePressed(QMouseEvent* event);
  void onMouseDoubleClicked(QMouseEvent* event);
  void onMouseReleased(QMouseEvent* event);
  void onMouseMoving(QMouseEvent* event);

protected:
  virtual void Initialize(SVTK_View* theView,
			  SVTK_ViewModelBase* theModel);

  void doSetVisualParameters( const QString& );
  void SetEventDispatcher(vtkObject* theDispatcher);

  QImage dumpViewContent();

  virtual QString filter() const;
  virtual bool dumpViewToFormat( const QImage& img, const QString& fileName, const QString& format );
  
  virtual bool action( const int );
  
  QtxAction* getAction( int ) const;
  void createToolBar();
  void createActions(SUIT_ResourceMgr* theResourceMgr);

  enum { DumpId, FitAllId, FitRectId, ZoomId, PanId, GlobalPanId, 
	 ChangeRotationPointId, RotationId,
         FrontId, BackId, TopId, BottomId, LeftId, RightId, ResetId, 
	 ViewTrihedronId, NonIsometric, GraduatedAxes, UpdateRate,
	 ParallelModeId, ProjectionModeId, ViewParametersId, SwitchInteractionStyleId,
	 StartRecordingId, PlayRecordingId, PauseRecordingId, StopRecordingId };


  SVTK_View* myView;
  //SVTK_MainWindow* myMainWindow;
  SVTK_ViewModelBase* myModel;

  SVTK_RenderWindowInteractor* myInteractor;
  vtkSmartPointer<SVTK_KeyFreeInteractorStyle> myKeyFreeInteractorStyle;

  QString myVisualParams; // used for delayed setting of view parameters 

  vtkSmartPointer<vtkObject> myEventDispatcher;

  SVTK_NonIsometricDlg* myNonIsometricDlg;
  SVTK_UpdateRateDlg* myUpdateRateDlg;
  SVTK_CubeAxesDlg* myCubeAxesDlg;
  SVTK_SetRotationPointDlg* mySetRotationPointDlg;
  SVTK_ViewParameterDlg* myViewParameterDlg;

  QSize myPreRecordingMinSize;
  QSize myPreRecordingMaxSize;

  SVTK_Recorder* myRecorder;
  QtxAction* myStartAction;
  QtxAction* myPlayAction;
  QtxAction* myPauseAction;
  QtxAction* myStopAction;

  int myToolBar;
  int myRecordingToolBar;

private:
  QImage myDumpImage;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
