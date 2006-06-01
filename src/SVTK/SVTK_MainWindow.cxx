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

#include "SALOME_Actor.h"

#include <vtkGenericRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include "QtxAction.h"

#include "SUIT_ToolButton.h"
#include "SUIT_MessageBox.h"
#include "SUIT_ViewWindow.h"

#include "SUIT_Tools.h"
#include "SUIT_ResourceMgr.h"
#include "SVTK_NonIsometricDlg.h"
#include "SVTK_UpdateRateDlg.h"
#include "SVTK_CubeAxesDlg.h"

#include "SVTK_MainWindow.h"
#include "SVTK_Event.h"
#include "SVTK_Renderer.h"
#include "SVTK_RenderWindowInteractor.h"

#include "SVTK_Selector.h"

#include <qimage.h>

/*!
  Constructor
*/
SVTK_MainWindow
::SVTK_MainWindow(QWidget* theParent, 
		  const char* theName,
		  SUIT_ResourceMgr* theResourceMgr,
		  SUIT_ViewWindow* theViewWindow) :
  QMainWindow(theParent,theName,0),
  myViewWindow(theViewWindow)
{
  myToolBar = new QToolBar(this);
  myToolBar->setCloseMode(QDockWindow::Undocked);
  myToolBar->setLabel(tr("LBL_TOOLBAR_LABEL"));

  createActions(theResourceMgr);
  createToolBar();
}

/*!
  To initialize the class
*/
void
SVTK_MainWindow
::Initialize(SVTK_RenderWindowInteractor* theInteractor)
{
  myInteractor = theInteractor;
  SetEventDispatcher(myInteractor->GetDevice());

  setCentralWidget(myInteractor);
  myInteractor->setBackgroundMode(Qt::NoBackground);

  myInteractor->setFocusPolicy(StrongFocus);
  myInteractor->setFocus();
  setFocusProxy(myInteractor);

  myUpdateRateDlg = new SVTK_UpdateRateDlg(myActionsMap[UpdateRate],this,"SVTK_UpdateRateDlg");
  myNonIsometricDlg = new SVTK_NonIsometricDlg(myActionsMap[NonIsometric],this,"SVTK_NonIsometricDlg");
  myCubeAxesDlg = new SVTK_CubeAxesDlg(myActionsMap[GraduatedAxes],this,"SVTK_CubeAxesDlg");
}

/*!
  Destructor
*/
SVTK_MainWindow
::~SVTK_MainWindow()
{
}

/*!
  \return used SVTK_RenderWindowInteractor
*/
SVTK_RenderWindowInteractor*
SVTK_MainWindow
::GetInteractor()
{
  return myInteractor;
}

/*!
  \return used #vtkRenderWindowInteractor (obsolete)
*/
vtkRenderWindowInteractor*
SVTK_MainWindow
::getInteractor()
{
  return GetInteractor()->GetDevice();
}

/*!
  \return used vtkRenderWindow (obsolete)
*/
vtkRenderWindow*
SVTK_MainWindow
::getRenderWindow()
{
  return GetInteractor()->getRenderWindow();
}

/*!
  To repaint the view
  \param theUpdateTrihedron - adjust trihedron
*/
void
SVTK_MainWindow
::Repaint(bool theUpdateTrihedron)
{
  if(theUpdateTrihedron) 
    GetRenderer()->OnAdjustTrihedron();

  GetInteractor()->update();
}

/*!
  To invoke a VTK event on SVTK_RenderWindowInteractor instance
*/
void
SVTK_MainWindow
::InvokeEvent(unsigned long theEvent, void* theCallData)
{
  GetInteractor()->InvokeEvent(theEvent,theCallData);
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::GetInteractorStyle
*/
vtkInteractorStyle*
SVTK_MainWindow
::GetInteractorStyle()
{
  return GetInteractor()->GetInteractorStyle();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::PushInteractorStyle
*/
void
SVTK_MainWindow
::PushInteractorStyle(vtkInteractorStyle* theStyle)
{
  GetInteractor()->PushInteractorStyle(theStyle);
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::PopInteractorStyle
*/
void
SVTK_MainWindow
::PopInteractorStyle()
{
  GetInteractor()->PopInteractorStyle();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::GetSelector
*/
SVTK_Selector*
SVTK_MainWindow
::GetSelector()
{
  return GetInteractor()->GetSelector();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::SelectionMode
*/
Selection_Mode
SVTK_MainWindow
::SelectionMode()
{
  return GetSelector()->SelectionMode();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::SetSelectionMode
*/
void
SVTK_MainWindow
::SetSelectionMode(Selection_Mode theMode)
{
  GetSelector()->SetSelectionMode(theMode);
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::GetRenderer
*/
SVTK_Renderer* 
SVTK_MainWindow
::GetRenderer()
{
  return GetInteractor()->GetRenderer();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::getRenderer
*/
vtkRenderer* 
SVTK_MainWindow
::getRenderer()
{
  return GetInteractor()->getRenderer();
}

/*!
  Sets background color of the view
  \param theColor - new background color
*/
void
SVTK_MainWindow
::SetBackgroundColor(const QColor& theColor)
{
  getRenderer()->SetBackground(theColor.red()/255.0, 
			       theColor.green()/255.0,
			       theColor.blue()/255.0);
}

/*!
  \return background color of the view
*/
QColor
SVTK_MainWindow
::BackgroundColor()
{
  vtkFloatingPointType aBackgroundColor[3];
  getRenderer()->GetBackground(aBackgroundColor);
  return QColor(int(aBackgroundColor[0]*255), 
		int(aBackgroundColor[1]*255), 
		int(aBackgroundColor[2]*255));
}

/*!
  Redirect the request to SVTK_Renderer::GetScale
*/
void
SVTK_MainWindow
::GetScale( double theScale[3] ) 
{
  GetRenderer()->GetScale( theScale );
}

/*!
  Redirect the request to SVTK_Renderer::SetScale
*/
void
SVTK_MainWindow
::SetScale( double theScale[3] ) 
{
  GetRenderer()->SetScale( theScale );
  Repaint();
}

/*!
  Redirect the request to SVTK_Renderer::AddActor
*/
void
SVTK_MainWindow
::AddActor(VTKViewer_Actor* theActor, 
	   bool theIsUpdate)
{
  GetRenderer()->AddActor(theActor);
  if(theIsUpdate) 
    Repaint();
}

/*!
  Redirect the request to SVTK_Renderer::RemoveActor
*/
void
SVTK_MainWindow
::RemoveActor(VTKViewer_Actor* theActor, 
	      bool theIsUpdate)
{
  GetRenderer()->RemoveActor(theActor);
  if(theIsUpdate) 
    Repaint();
}

/*!
  Redirect the request to SVTK_Renderer::GetTrihedronSize
*/
int
SVTK_MainWindow
::GetTrihedronSize()
{
  return GetRenderer()->GetTrihedronSize();
}

/*!
  Redirect the request to SVTK_Renderer::SetTrihedronSize
*/
void 
SVTK_MainWindow
::SetTrihedronSize( const int theSize, const bool theRelative )
{
  GetRenderer()->SetTrihedronSize(theSize, theRelative);
  Repaint();
}


/*! If parameter theIsForcedUpdate is true, recalculate parameters for
 *  trihedron and cube axes, even if trihedron and cube axes is invisible.
 */
void
SVTK_MainWindow
::AdjustActors()
{
  GetRenderer()->AdjustActors();
  Repaint();
}

/*!
  Redirect the request to SVTK_Renderer::IsTrihedronDisplayed
*/
bool
SVTK_MainWindow
::IsTrihedronDisplayed()
{
  return GetRenderer()->IsTrihedronDisplayed();
}

/*!
  Redirect the request to SVTK_Renderer::IsCubeAxesDisplayed
*/
bool
SVTK_MainWindow
::IsCubeAxesDisplayed()
{
  return GetRenderer()->IsCubeAxesDisplayed();
}

/*!
  Redirect the request to SVTK_Renderer::GetTrihedron
*/
VTKViewer_Trihedron*  
SVTK_MainWindow
::GetTrihedron() 
{ 
  return GetRenderer()->GetTrihedron(); 
}

/*!
  Redirect the request to SVTK_Renderer::GetCubeAxes
*/
SVTK_CubeAxesActor2D* 
SVTK_MainWindow
::GetCubeAxes() 
{ 
  return GetRenderer()->GetCubeAxes(); 
}

/*!
  \return toolbar of svtk main window
*/
QToolBar* 
SVTK_MainWindow
::getToolBar()
{
  return myToolBar;
}

void
SVTK_MainWindow
::SetEventDispatcher(vtkObject* theDispatcher)
{
  myEventDispatcher = theDispatcher;
}

#if defined(WIN32) && !defined(_DEBUG)
#pragma optimize( "", off )
#endif

/*!
  Creates all actions of svtk main window
*/
void
SVTK_MainWindow
::createActions(SUIT_ResourceMgr* theResourceMgr)
{
  if(!myActionsMap.isEmpty()) 
    return;
  
  QtxAction* anAction;

  // Dump view
  anAction = new QtxAction(tr("MNU_DUMP_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_DUMP" ) ),
			   tr( "MNU_DUMP_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(anAction, SIGNAL(activated()), myViewWindow, SLOT(onDumpView()));
  myActionsMap[ DumpId ] = anAction;

  // FitAll
  anAction = new QtxAction(tr("MNU_FITALL"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITALL" ) ),
			   tr( "MNU_FITALL" ), 0, this);
  anAction->setStatusTip(tr("DSC_FITALL"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onFitAll()));
  myActionsMap[ FitAllId ] = anAction;

  // FitRect
  anAction = new QtxAction(tr("MNU_FITRECT"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITAREA" ) ),
			   tr( "MNU_FITRECT" ), 0, this);
  anAction->setStatusTip(tr("DSC_FITRECT"));
  connect(anAction, SIGNAL(activated()), this, SLOT(activateWindowFit()));
  myActionsMap[ FitRectId ] = anAction;

  // Zoom
  anAction = new QtxAction(tr("MNU_ZOOM_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ZOOM" ) ),
			   tr( "MNU_ZOOM_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(activateZoom()));
  myActionsMap[ ZoomId ] = anAction;

  // Panning
  anAction = new QtxAction(tr("MNU_PAN_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_PAN" ) ),
			   tr( "MNU_PAN_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(activatePanning()));
  myActionsMap[ PanId ] = anAction;

  // Global Panning
  anAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_GLOBALPAN" ) ),
			   tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(activateGlobalPanning()));
  myActionsMap[ GlobalPanId ] = anAction;

  // Rotation
  anAction = new QtxAction(tr("MNU_ROTATE_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ROTATE" ) ),
			   tr( "MNU_ROTATE_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_ROTATE_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(activateRotation()));
  myActionsMap[ RotationId ] = anAction;

  // Projections
  anAction = new QtxAction(tr("MNU_FRONT_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FRONT" ) ),
			   tr( "MNU_FRONT_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_FRONT_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onFrontView()));
  myActionsMap[ FrontId ] = anAction;

  anAction = new QtxAction(tr("MNU_BACK_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BACK" ) ),
			   tr( "MNU_BACK_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_BACK_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onBackView()));
  myActionsMap[ BackId ] = anAction;

  anAction = new QtxAction(tr("MNU_TOP_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TOP" ) ),
			   tr( "MNU_TOP_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_TOP_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onTopView()));
  myActionsMap[ TopId ] = anAction;

  anAction = new QtxAction(tr("MNU_BOTTOM_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BOTTOM" ) ),
			   tr( "MNU_BOTTOM_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_BOTTOM_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onBottomView()));
  myActionsMap[ BottomId ] = anAction;

  anAction = new QtxAction(tr("MNU_LEFT_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_LEFT" ) ),
			   tr( "MNU_LEFT_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_LEFT_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onLeftView()));
  myActionsMap[ LeftId ] = anAction;

  anAction = new QtxAction(tr("MNU_RIGHT_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RIGHT" ) ),
			   tr( "MNU_RIGHT_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_RIGHT_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onRightView()));
  myActionsMap[ RightId ] = anAction;

  // Reset
  anAction = new QtxAction(tr("MNU_RESET_VIEW"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RESET" ) ),
			   tr( "MNU_RESET_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onResetView()));
  myActionsMap[ ResetId ] = anAction;

  // onViewTrihedron: Shows - Hides Trihedron
  anAction = new QtxAction(tr("MNU_SHOW_TRIHEDRON"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TRIHEDRON" ) ),
			   tr( "MNU_SHOW_TRIHEDRON" ), 0, this);
  anAction->setStatusTip(tr("DSC_SHOW_TRIHEDRON"));
  connect(anAction, SIGNAL(activated()), this, SLOT(onViewTrihedron()));
  myActionsMap[ ViewTrihedronId ] = anAction;

  // onNonIsometric: Manage non-isometric params
  anAction = new QtxAction(tr("MNU_SVTK_SCALING"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_SCALING" ) ),
			   tr( "MNU_SVTK_SCALING" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_SCALING"));
  anAction->setToggleAction(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onNonIsometric(bool)));
  myActionsMap[ NonIsometric ] = anAction;

  // onGraduatedAxes: Manage graduated axes params
  anAction = new QtxAction(tr("MNU_SVTK_GRADUATED_AXES"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_GRADUATED_AXES" ) ),
			   tr( "MNU_SVTK_GRADUATED_AXES" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_GRADUATED_AXES"));
  anAction->setToggleAction(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onGraduatedAxes(bool)));
  myActionsMap[ GraduatedAxes ] = anAction;

  // onGraduatedAxes: Manage graduated axes params
  anAction = new QtxAction(tr("MNU_SVTK_UPDATE_RATE"), 
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_UPDATE_RATE" ) ),
			   tr( "MNU_SVTK_UPDATE_RATE" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_UPDATE_RATE"));
  anAction->setToggleAction(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onUpdateRate(bool)));
  myActionsMap[ UpdateRate ] = anAction;
}

#if defined(WIN32) && !defined(_DEBUG)
#pragma optimize( "", on )
#endif

/*!
  Creates toolbar of svtk main window
*/
void
SVTK_MainWindow
::createToolBar()
{
  myActionsMap[DumpId]->addTo(myToolBar);
  myActionsMap[ViewTrihedronId]->addTo(myToolBar);

  SUIT_ToolButton* aScaleBtn = new SUIT_ToolButton(myToolBar);
  aScaleBtn->AddAction(myActionsMap[FitAllId]);
  aScaleBtn->AddAction(myActionsMap[FitRectId]);
  aScaleBtn->AddAction(myActionsMap[ZoomId]);

  SUIT_ToolButton* aPanningBtn = new SUIT_ToolButton(myToolBar);
  aPanningBtn->AddAction(myActionsMap[PanId]);
  aPanningBtn->AddAction(myActionsMap[GlobalPanId]);

  myActionsMap[RotationId]->addTo(myToolBar);

  SUIT_ToolButton* aViewsBtn = new SUIT_ToolButton(myToolBar);
  aViewsBtn->AddAction(myActionsMap[FrontId]);
  aViewsBtn->AddAction(myActionsMap[BackId]);
  aViewsBtn->AddAction(myActionsMap[TopId]);
  aViewsBtn->AddAction(myActionsMap[BottomId]);
  aViewsBtn->AddAction(myActionsMap[LeftId]);
  aViewsBtn->AddAction(myActionsMap[RightId]);

  myActionsMap[ResetId]->addTo(myToolBar);

  myActionsMap[UpdateRate]->addTo(myToolBar);
  myActionsMap[NonIsometric]->addTo(myToolBar);
  myActionsMap[GraduatedAxes]->addTo(myToolBar);
}

/*!
  Custom show event handler
*/
void
SVTK_MainWindow
::showEvent( QShowEvent * theEvent ) 
{
  emit Show( theEvent );
}

/*!
  Custom hide event handler
*/
void
SVTK_MainWindow
::hideEvent( QHideEvent * theEvent ) 
{
  emit Hide( theEvent );
}

/*!
  Starts zoom transformation
*/
void
SVTK_MainWindow
::activateZoom()
{
  myEventDispatcher->InvokeEvent(SVTK::StartZoom,0);
}

/*!
  Starts panning transformation
*/
void
SVTK_MainWindow
::activatePanning()
{
  myEventDispatcher->InvokeEvent(SVTK::StartPan,0);
}

/*!
  Starts rotation transformation
*/
void
SVTK_MainWindow
::activateRotation()
{
  myEventDispatcher->InvokeEvent(SVTK::StartRotate,0);
}

/*!
  Starts global panning transformation
*/
void
SVTK_MainWindow
::activateGlobalPanning()
{
  myEventDispatcher->InvokeEvent(SVTK::StartGlobalPan,0);
}

/*!
  Starts window fit transformation
*/
void
SVTK_MainWindow
::activateWindowFit()
{
  myEventDispatcher->InvokeEvent(SVTK::StartFitArea,0);
}

/*!
  Processes transformation "front view"
*/
void
SVTK_MainWindow
::onFrontView()
{
  GetRenderer()->OnFrontView();
  Repaint();
}

/*!
  Processes transformation "back view"
*/
void
SVTK_MainWindow
::onBackView()
{
  GetRenderer()->OnBackView();
  Repaint();
}

/*!
  Processes transformation "top view"
*/
void
SVTK_MainWindow
::onTopView()
{
  GetRenderer()->OnTopView();
  Repaint();
}

/*!
  Processes transformation "bottom view"
*/
void
SVTK_MainWindow
::onBottomView()
{
  GetRenderer()->OnBottomView();
  Repaint();
}

/*!
  Processes transformation "left view"
*/
void
SVTK_MainWindow
::onLeftView()
{
  GetRenderer()->OnLeftView();
  Repaint();
}

/*!
  Processes transformation "right view"
*/
void
SVTK_MainWindow
::onRightView()
{
  GetRenderer()->OnRightView();
  Repaint();
}

/*!
  Processes transformation "reset view": sets default orientation of viewport camera
*/
void
SVTK_MainWindow
::onResetView()
{
  GetRenderer()->OnResetView();
  Repaint();
}

/*!
  Processes transformation "fit all"
*/
void
SVTK_MainWindow
::onFitAll()
{
  GetRenderer()->OnFitAll();
  Repaint();
}

/*!
  Shows trihedron
*/
void 
SVTK_MainWindow
::onViewTrihedron()
{
  GetRenderer()->OnViewTrihedron();
  Repaint();
}

/*!
  Shows cube axes
*/
void
SVTK_MainWindow
::onViewCubeAxes()
{
  GetRenderer()->OnViewCubeAxes();
  Repaint();
}

void
SVTK_MainWindow
::onUpdateRate(bool theIsActivate)
{
  if(theIsActivate){
    myUpdateRateDlg->Update();
    myUpdateRateDlg->show();
  }else
    myUpdateRateDlg->hide();
}

void
SVTK_MainWindow
::onNonIsometric(bool theIsActivate)
{
  if(theIsActivate){
    myNonIsometricDlg->Update();
    myNonIsometricDlg->show();
  }else
    myNonIsometricDlg->hide();
}

void
SVTK_MainWindow
::onGraduatedAxes(bool theIsActivate)
{
  if(theIsActivate){
    myCubeAxesDlg->Update();
    myCubeAxesDlg->show();
  }else
    myCubeAxesDlg->hide();
}

void
SVTK_MainWindow
::onAdjustTrihedron()
{   
  GetRenderer()->OnAdjustTrihedron();
}

void
SVTK_MainWindow
::onAdjustCubeAxes()
{   
  GetRenderer()->OnAdjustCubeAxes();
}

/*!
  \return QImage, containing all scene rendering in window
*/
QImage
SVTK_MainWindow
::dumpView()
{
  QPixmap px = QPixmap::grabWindow( GetInteractor()->winId() );
  return px.convertToImage();
}
