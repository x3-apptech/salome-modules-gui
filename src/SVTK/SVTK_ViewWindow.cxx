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
// See http://www.salome-platform.org/
//
#include "SALOME_Actor.h"

#include <qapplication.h>
#include <qimage.h>

#include <vtkTextProperty.h>
#include <vtkActorCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>

#include "QtxAction.h"

#include "SUIT_Session.h"
#include "SUIT_ToolButton.h"
#include "SUIT_MessageBox.h"
#include "SUIT_Accel.h"

#include "SUIT_Tools.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_Accel.h"

#include "VTKViewer_Utilities.h"

#include "SVTK_View.h"
#include "SVTK_MainWindow.h"
#include "SVTK_Selector.h"

#include "SVTK_Event.h"
#include "SVTK_Renderer.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_ViewModelBase.h"
#include "SVTK_InteractorStyle.h"
#include "SVTK_RenderWindowInteractor.h"
#include "SVTK_GenericRenderWindowInteractor.h"

#include "SALOME_ListIteratorOfListIO.hxx"

#include "VTKViewer_Algorithm.h"
#include "SVTK_Functor.h"

//----------------------------------------------------------------------------
SVTK_ViewWindow
::SVTK_ViewWindow(SUIT_Desktop* theDesktop):
  SUIT_ViewWindow(theDesktop),
  myMainWindow(NULL),
  myView(NULL)
{}

void
SVTK_ViewWindow
::Initialize(SVTK_ViewModelBase* theModel)
{
  if(SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr()){
    myMainWindow = new SVTK_MainWindow(this,"SVTK_MainWindow",aResourceMgr,this);

    SVTK_RenderWindowInteractor* anIteractor = 
      new SVTK_RenderWindowInteractor(myMainWindow,"SVTK_RenderWindowInteractor");

    SVTK_Selector* aSelector = SVTK_Selector::New();

    SVTK_GenericRenderWindowInteractor* aDevice = 
      SVTK_GenericRenderWindowInteractor::New();
    aDevice->SetRenderWidget(anIteractor);
    aDevice->SetSelector(aSelector);

    SVTK_Renderer* aRenderer = SVTK_Renderer::New();
    aRenderer->Initialize(aDevice,aSelector);

    anIteractor->Initialize(aDevice,aRenderer,aSelector);

    aDevice->Delete();
    aRenderer->Delete();
    aSelector->Delete();

    myMainWindow->Initialize(anIteractor);

    SVTK_InteractorStyle* aStyle = SVTK_InteractorStyle::New();
    anIteractor->PushInteractorStyle(aStyle);
    aStyle->Delete();

    setCentralWidget(myMainWindow);
    
    myView = new SVTK_View(myMainWindow);
    Initialize(myView,theModel);
  }
}

void
SVTK_ViewWindow
::Initialize(SVTK_View* theView,
	     SVTK_ViewModelBase* theModel)
{
  connect(theView,SIGNAL(KeyPressed(QKeyEvent*)),
          this,SLOT(onKeyPressed(QKeyEvent*)) );
  connect(theView,SIGNAL(KeyReleased(QKeyEvent*)),
          this,SLOT(onKeyReleased(QKeyEvent*)));
  connect(theView,SIGNAL(MouseButtonPressed(QMouseEvent*)),
          this,SLOT(onMousePressed(QMouseEvent*)));
  connect(theView,SIGNAL(MouseButtonReleased(QMouseEvent*)),
          this,SLOT(onMouseReleased(QMouseEvent*)));
  connect(theView,SIGNAL(MouseDoubleClicked(QMouseEvent*)),
          this,SLOT(onMouseDoubleClicked(QMouseEvent*)));
  connect(theView,SIGNAL(MouseMove(QMouseEvent*)),
          this,SLOT(onMouseMoving(QMouseEvent*)));
  connect(theView,SIGNAL(contextMenuRequested(QContextMenuEvent*)),
          this,SIGNAL(contextMenuRequested(QContextMenuEvent *)));
  connect(theView,SIGNAL(selectionChanged()),
	  theModel,SLOT(onSelectionChanged()));
}

SVTK_ViewWindow
::~SVTK_ViewWindow()
{}


//----------------------------------------------------------------------------
SVTK_View* 
SVTK_ViewWindow
::getView() 
{ 
  return myView; 
}

SVTK_MainWindow* 
SVTK_ViewWindow
::getMainWindow() 
{ 
  return myMainWindow; 
}

vtkRenderWindow*
SVTK_ViewWindow
::getRenderWindow()
{
  return getMainWindow()->getRenderWindow();
}

vtkRenderWindowInteractor*
SVTK_ViewWindow
::getInteractor()
{
  return getMainWindow()->getInteractor();
}

vtkRenderer*
SVTK_ViewWindow
::getRenderer()
{
  return myMainWindow->getRenderer();
}

SVTK_Selector* 
SVTK_ViewWindow
::GetSelector() 
{ 
  return myMainWindow->GetSelector(); 
}


//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onFrontView()
{
  myMainWindow->onFrontView();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onBackView()
{
  myMainWindow->onBackView();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onTopView()
{
  myMainWindow->onTopView();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onBottomView()
{
  myMainWindow->onBottomView();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onLeftView()
{
  myMainWindow->onLeftView();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onRightView()
{
  myMainWindow->onRightView();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onResetView()
{
  myMainWindow->onResetView();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onFitAll()
{
  myMainWindow->onFitAll();
}

//----------------------------------------------------------------
void
SVTK_ViewWindow
::onSelectionChanged()
{
  myView->onSelectionChanged();
}

//----------------------------------------------------------------
void
SVTK_ViewWindow
::SetSelectionMode(Selection_Mode theMode)
{
  myMainWindow->SetSelectionMode( theMode );
}

//----------------------------------------------------------------
Selection_Mode
SVTK_ViewWindow
::SelectionMode() const
{
  return myMainWindow->SelectionMode();
}

//----------------------------------------------------------------
void 
SVTK_ViewWindow
::unHighlightAll() 
{
  myView->unHighlightAll();
}

//----------------------------------------------------------------
void
SVTK_ViewWindow
::highlight(const Handle(SALOME_InteractiveObject)& theIO, 
	    bool theIsHighlight, 
	    bool theIsUpdate ) 
{
  myView->highlight( theIO, theIsHighlight, theIsUpdate );
}

//----------------------------------------------------------------
bool
SVTK_ViewWindow
::isInViewer( const Handle(SALOME_InteractiveObject)& theIO ) 
{
  return myView->isInViewer( theIO );
}

//----------------------------------------------------------------
bool
SVTK_ViewWindow
::isVisible( const Handle(SALOME_InteractiveObject)& theIO ) 
{
  return myView->isVisible( theIO );
}

//----------------------------------------------------------------
void
SVTK_ViewWindow
::Display(const Handle(SALOME_InteractiveObject)& theIO,
	  bool theImmediatly) 
{
  myView->Display(theIO,theImmediatly);
}

void
SVTK_ViewWindow
::Erase(const Handle(SALOME_InteractiveObject)& theIO,
	  bool theImmediatly) 
{
  myView->Erase(theIO,theImmediatly);
}

void
SVTK_ViewWindow
::DisplayOnly(const Handle(SALOME_InteractiveObject)& theIO) 
{
  myView->DisplayOnly(theIO);
}

void 
SVTK_ViewWindow
::DisplayAll() 
{
  myView->DisplayAll();
}

void 
SVTK_ViewWindow
::EraseAll() 
{
  myView->EraseAll();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::setBackgroundColor( const QColor& color )
{
  myMainWindow->SetBackgroundColor( color );
}

//----------------------------------------------------------------------------
QColor
SVTK_ViewWindow
::backgroundColor() const
{
  return myMainWindow->BackgroundColor();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::Repaint(bool theUpdateTrihedron)
{
  myMainWindow->Repaint( theUpdateTrihedron );
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::GetScale( double theScale[3] ) 
{
  myMainWindow->GetScale( theScale );
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::SetScale( double theScale[3] ) 
{
  myMainWindow->SetScale( theScale );
}

//----------------------------------------------------------------------------
bool
SVTK_ViewWindow
::isTrihedronDisplayed()
{
  return myMainWindow->IsTrihedronDisplayed();
}

bool
SVTK_ViewWindow
::isCubeAxesDisplayed()
{
  return myMainWindow->IsCubeAxesDisplayed();
}

//----------------------------------------------------------------------------
void 
SVTK_ViewWindow
::onViewTrihedron()
{
  myMainWindow->onViewTrihedron();
}

void
SVTK_ViewWindow
::onViewCubeAxes()
{
  myMainWindow->onViewCubeAxes();
}

//----------------------------------------------------------------------------
VTKViewer_Trihedron* 
SVTK_ViewWindow::
GetTrihedron()
{
  return myMainWindow->GetTrihedron();
}

SVTK_CubeAxesActor2D* 
SVTK_ViewWindow
::GetCubeAxes()
{
  return myMainWindow->GetCubeAxes();
}

int
SVTK_ViewWindow
::GetTrihedronSize() const
{
  return myMainWindow->GetTrihedronSize();
}

void
SVTK_ViewWindow
::SetTrihedronSize(const int theSize)
{
  myMainWindow->SetTrihedronSize(theSize);
}

/*! If parameter theIsForcedUpdate is true, recalculate parameters for
 *  trihedron and cube axes, even if trihedron and cube axes is invisible.
 */
void
SVTK_ViewWindow
::AdjustTrihedrons(const bool theIsForcedUpdate)
{
  myMainWindow->AdjustActors();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onAdjustTrihedron()
{   
  myMainWindow->onAdjustTrihedron();
}

void
SVTK_ViewWindow
::onAdjustCubeAxes()
{   
  myMainWindow->onAdjustCubeAxes();
}

//=======================================================================
void
SVTK_ViewWindow
::onKeyPressed(QKeyEvent* event)
{
  emit keyPressed( this, event );
}

//=======================================================================
void
SVTK_ViewWindow
::onKeyReleased(QKeyEvent* event)
{
  emit keyReleased( this, event );
}

//=======================================================================
void
SVTK_ViewWindow
::onMousePressed(QMouseEvent* event)
{
  emit mousePressed(this, event);
}

//=======================================================================
void
SVTK_ViewWindow
::onMouseReleased(QMouseEvent* event)
{
  emit mouseReleased( this, event );
}

//=======================================================================
void
SVTK_ViewWindow
::onMouseMoving(QMouseEvent* event)
{
  emit mouseMoving( this, event );
}

//=======================================================================
void
SVTK_ViewWindow
::onMouseDoubleClicked( QMouseEvent* event )
{
  emit mouseDoubleClicked( this, event );
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::AddActor( VTKViewer_Actor* theActor, 
	    bool theUpdate )
{
  myMainWindow->AddActor( theActor, theUpdate );
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::RemoveActor( VTKViewer_Actor* theActor, 
	       bool theUpdate )
{
  myMainWindow->RemoveActor( theActor, theUpdate );
}

//----------------------------------------------------------------------------
QImage
SVTK_ViewWindow
::dumpView()
{
  return myMainWindow->dumpView();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::SetSelectionProp(const double& theRed, 
		   const double& theGreen, 
		   const double& theBlue, 
		   const int& theWidth) 
{
  myView->SetSelectionProp(theRed,theGreen,theBlue,theWidth);
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::SetPreselectionProp(const double& theRed, 
		      const double& theGreen, 
		      const double& theBlue, 
		      const int& theWidth) 
{
  myView->SetPreselectionProp(theRed,theGreen,theBlue,theWidth);
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::SetSelectionTolerance(const double& theTolNodes, 
			const double& theTolItems)
{
  myView->SetSelectionTolerance(theTolNodes,theTolItems);
}

//----------------------------------------------------------------------------
int convertAction( const int accelAction )
{
  switch ( accelAction ) {
  case SUIT_Accel::PanLeft     : return SVTK::PanLeftEvent;
  case SUIT_Accel::PanRight    : return SVTK::PanRightEvent;
  case SUIT_Accel::PanUp       : return SVTK::PanUpEvent;
  case SUIT_Accel::PanDown     : return SVTK::PanDownEvent;
  case SUIT_Accel::ZoomIn      : return SVTK::ZoomInEvent;
  case SUIT_Accel::ZoomOut     : return SVTK::ZoomOutEvent;
  case SUIT_Accel::RotateLeft  : return SVTK::RotateLeftEvent;
  case SUIT_Accel::RotateRight : return SVTK::RotateRightEvent;
  case SUIT_Accel::RotateUp    : return SVTK::RotateUpEvent;
  case SUIT_Accel::RotateDown  : return SVTK::RotateDownEvent;  
  }
  return accelAction;
}

//----------------------------------------------------------------------------
void 
SVTK_ViewWindow
::action( const int accelAction  )
{
  if ( accelAction == SUIT_Accel::ZoomFit )
    onFitAll();
  else {
    int anEvent = convertAction( accelAction );
    myMainWindow->InvokeEvent( anEvent, 0 );
  }
}
