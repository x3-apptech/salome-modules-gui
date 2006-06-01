// Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
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
#ifndef SVTK_MAINWINDOW_H
#define SVTK_MAINWINDOW_H

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include "SVTK.h"
#include "SVTK_Selection.h"

#include <vtkSmartPointer.h>

#include <qmainwindow.h>

class QtxAction;

class vtkObject;
class vtkRenderer;
class vtkRenderWindow;
class vtkInteractorStyle;
class vtkRenderWindowInteractor;

class SUIT_ResourceMgr;
class SUIT_ViewWindow;

class SVTK_RenderWindowInteractor;
class SVTK_NonIsometricDlg;
class SVTK_UpdateRateDlg;
class SVTK_CubeAxesActor2D;
class SVTK_CubeAxesDlg;

class VTKViewer_Trihedron;
class VTKViewer_Transform;
class VTKViewer_Actor;

class SVTK_Renderer;
class SVTK_Selector;


//! The class is a container for #SVTK_RenderWindowInteractor.
/*!
  The class contains #SVTK_RenderWindowInteractor instance and
  adds predefined viewer actions and toolbar for user interaction.
*/
class SVTK_EXPORT SVTK_MainWindow: public QMainWindow
{
  Q_OBJECT;

public:
  SVTK_MainWindow(QWidget* theParent, 
		  const char* theName,
		  SUIT_ResourceMgr* theResourceMgr,
		  SUIT_ViewWindow* theViewWindow);
  
  //! To initialize the class
  virtual
  void
  Initialize(SVTK_RenderWindowInteractor* theInteractor);

  virtual
  ~SVTK_MainWindow();

  //----------------------------------------------------------------------------
  //! Get used #SVTK_RenderWindowInteractor
  SVTK_RenderWindowInteractor*
  GetInteractor();

  //! Get used #vtkRenderWindowInteractor (obsolete)
  vtkRenderWindowInteractor*
  getInteractor();

  //! Get used #vtkRenderWindow (obsolete)
  vtkRenderWindow*
  getRenderWindow();

  //! To repaint the view
  void
  Repaint(bool theUpdateTrihedron = true);

  //! To invoke a VTK event on #SVTK_RenderWindowInteractor instance
  void
  InvokeEvent(unsigned long theEvent, void* theCallData);

  //----------------------------------------------------------------------------
  //! Redirect the request to #SVTK_RenderWindowInteractor::GetInteractorStyle
  vtkInteractorStyle* 
  GetInteractorStyle();

  //! Redirect the request to #SVTK_RenderWindowInteractor::PushInteractorStyle
  void
  PushInteractorStyle(vtkInteractorStyle* theStyle);

  //! Redirect the request to #SVTK_RenderWindowInteractor::PopInteractorStyle
  void
  PopInteractorStyle();

  //----------------------------------------------------------------------------
  //! Redirect the request to #SVTK_RenderWindowInteractor::GetSelector
  SVTK_Selector* 
  GetSelector();

  //! Redirect the request to #SVTK_RenderWindowInteractor::SelectionMode
  Selection_Mode
  SelectionMode();

  //! Redirect the request to #SVTK_RenderWindowInteractor::SetSelectionMode
  void 
  SetSelectionMode(Selection_Mode theMode);

  //----------------------------------------------------------------------------
  //! Redirect the request to #SVTK_RenderWindowInteractor::GetRenderer
  SVTK_Renderer* 
  GetRenderer();

  //! Redirect the request to #SVTK_RenderWindowInteractor::getRenderer
  vtkRenderer* 
  getRenderer();

  //! Set background color to the view
  void
  SetBackgroundColor(const QColor& theColor);

  //! Get background color of the view
  QColor 
  BackgroundColor();

  //! Redirect the request to #SVTK_Renderer::SetScale
  void
  SetScale(double theScale[3]);

  //! Redirect the request to #SVTK_Renderer::GetScale
  void
  GetScale(double theScale[3]);

  //! Redirect the request to #SVTK_Renderer::AddActor
  virtual
  void 
  AddActor(VTKViewer_Actor* theActor, 
	   bool theIsUpdate = false);

  //! Redirect the request to #SVTK_Renderer::RemoveActor
  virtual
  void 
  RemoveActor(VTKViewer_Actor* theActor, 
	      bool theIsUpdate = false);

  //! Redirect the request to #SVTK_Renderer::GetTrihedronSize
  int  
  GetTrihedronSize();

  //! Redirect the request to #SVTK_Renderer::SetTrihedronSize
  void
  SetTrihedronSize(const int theSize, const bool theRelative = true);

  //! Redirect the request to #SVTK_Renderer::AdjustActors
  void 
  AdjustActors();

  //! Redirect the request to #SVTK_Renderer::IsTrihedronDisplayed
  bool
  IsTrihedronDisplayed();
 
  //! Redirect the request to #SVTK_Renderer::IsCubeAxesDisplayed
  bool
  IsCubeAxesDisplayed();

  //! Redirect the request to #SVTK_Renderer::GetTrihedron
  VTKViewer_Trihedron* 
  GetTrihedron();

  //! Redirect the request to #SVTK_Renderer::GetCubeAxes
  SVTK_CubeAxesActor2D*
  GetCubeAxes();

  //----------------------------------------------------------------------------
  QToolBar* getToolBar();

 signals:
  void Show( QShowEvent * );
  void Hide( QHideEvent * );
  
 public slots:
  virtual void showEvent( QShowEvent * );
  virtual void hideEvent( QHideEvent * );

  void activateZoom();
  void activateWindowFit();
  void activateRotation();
  void activatePanning(); 
  void activateGlobalPanning(); 

  void onFrontView(); 
  void onBackView(); 
  void onTopView();
  void onBottomView();
  void onRightView(); 
  void onLeftView();     

  void onResetView();     
  void onFitAll();

  void onViewTrihedron(); 
  void onViewCubeAxes();

  void onUpdateRate(bool theIsActivate);
  void onNonIsometric(bool theIsActivate);
  void onGraduatedAxes(bool theIsActivate);

  void onAdjustTrihedron();
  void onAdjustCubeAxes();

 public:
  QImage dumpView();

 protected:  
  void
  createActions(SUIT_ResourceMgr* theResourceMgr);

  void
  createToolBar();

  void
  SetEventDispatcher(vtkObject* theDispatcher);

  enum { DumpId, FitAllId, FitRectId, ZoomId, PanId, GlobalPanId, RotationId,
         FrontId, BackId, TopId, BottomId, LeftId, RightId, ResetId, 
	 ViewTrihedronId, NonIsometric, GraduatedAxes, UpdateRate};
  typedef QMap<int, QtxAction*> TActionsMap;

  SUIT_ViewWindow* myViewWindow;

  SVTK_NonIsometricDlg* myNonIsometricDlg;
  SVTK_UpdateRateDlg* myUpdateRateDlg;
  SVTK_CubeAxesDlg* myCubeAxesDlg;

  vtkSmartPointer<vtkObject> myEventDispatcher;
  TActionsMap myActionsMap;  
  QToolBar* myToolBar;

  SVTK_RenderWindowInteractor* myInteractor;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
