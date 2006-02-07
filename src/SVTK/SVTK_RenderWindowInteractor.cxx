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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : 
//  Author : 
//  Module : SALOME
//  $Header$

#include "SVTK_RenderWindowInteractor.h"
#include "SVTK_GenericRenderWindowInteractor.h"

#include "SVTK_InteractorStyle.h"
#include "SVTK_Renderer.h"
#include "SVTK_Functor.h"
#include "SALOME_Actor.h"

#include "SVTK_SpaceMouse.h" 
#include "SVTK_Event.h" 

#include "VTKViewer_Algorithm.h"

// VTK Includes
#include <vtkObjectFactory.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkPicker.h>
#include <vtkCamera.h>

// QT Includes
#include <qtimer.h>
#include <qapplication.h>
#include <qcolordialog.h>
#include <qpaintdevice.h>

using namespace std;

static bool GENERATE_SUIT_EVENTS = false;
static bool FOCUS_UNDER_MOUSE = false;


//----------------------------------------------------------------------------
QVTK_RenderWindowInteractor
::QVTK_RenderWindowInteractor(QWidget* theParent, 
			      const char* theName):
  QWidget(theParent,theName,Qt::WNoAutoErase),
  myRenderWindow(vtkRenderWindow::New())
{
  setMouseTracking(true);

  myRenderWindow->Delete();
  myRenderWindow->DoubleBufferOn();

#ifndef WNT
  myRenderWindow->SetDisplayId((void*)x11Display());
#endif
  myRenderWindow->SetWindowId((void*)winId());
}


void 
QVTK_RenderWindowInteractor
::Initialize(vtkGenericRenderWindowInteractor* theDevice)
{
  if(GetDevice())
    myDevice->SetRenderWindow(NULL);

  myDevice = theDevice;

  if(theDevice)
    theDevice->SetRenderWindow(getRenderWindow());
}

//----------------------------------------------------------------------------
QVTK_RenderWindowInteractor
::~QVTK_RenderWindowInteractor() 
{
  if(SVTK_SpaceMouse* aSpaceMouse = SVTK_SpaceMouse::getInstance())
    if(aSpaceMouse->isSpaceMouseOn())
      aSpaceMouse->close(x11Display());
}


//----------------------------------------------------------------------------
vtkGenericRenderWindowInteractor* 
QVTK_RenderWindowInteractor
::GetDevice()
{
  return myDevice.GetPointer();
}

//----------------------------------------------------------------------------
vtkRenderWindow*
QVTK_RenderWindowInteractor
::getRenderWindow()
{
  return myRenderWindow.GetPointer();
}

//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::InvokeEvent(unsigned long theEvent, void* theCallData)
{
  GetDevice()->InvokeEvent(theEvent,theCallData);
}

//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::show()
{
  QWidget::show();
  update(); // needed for initial contents display on Win32
}

//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::polish()
{
  // Final initialization just before the widget is displayed
  GetDevice()->SetSize(width(),height());
  if(!GetDevice()->GetInitialized()){
    GetDevice()->Initialize();
    GetDevice()->ConfigureEvent();
  }
}

//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::resize(int w, int h) 
{
  GetDevice()->UpdateSize(w,h);
}

//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::paintEvent( QPaintEvent* theEvent ) 
{
  GetDevice()->Render();
}


//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::resizeEvent( QResizeEvent* theEvent )
{
  int* aSize = getRenderWindow()->GetSize();
  int aWidth = aSize[0];
  int aHeight = aSize[1];

  GetDevice()->UpdateSize(width(),height());

  if(isVisible() && aWidth && aHeight){
    if( aWidth != width() || aHeight != height() ) {
      vtkRendererCollection * aRenderers = getRenderWindow()->GetRenderers();
      aRenderers->InitTraversal();
      double aCoeff = 1.0;
      if(vtkRenderer *aRenderer = aRenderers->GetNextItem()) {
	vtkCamera *aCamera = aRenderer->GetActiveCamera();
	double aScale = aCamera->GetParallelScale();
	if((aWidth - width())*(aHeight - height()) > 0)
	  aCoeff = sqrt(double(aWidth)/double(width())*double(height())/double(aHeight));
	else
	  aCoeff = double(aWidth)/double(width());
	aCamera->SetParallelScale(aScale*aCoeff);
      }
    }
  }

  update(); 
}


//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::contextMenuEvent( QContextMenuEvent* event )
{}


//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::mouseMoveEvent( QMouseEvent* event ) 
{
  GetDevice()->SetEventInformationFlipY(event->x(), 
					event->y(),
					event->state() & ControlButton,
					event->state() & ShiftButton);
  GetDevice()->MouseMoveEvent();
}


//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::mousePressEvent( QMouseEvent* event ) 
{
  GetDevice()->SetEventInformationFlipY(event->x(), 
					event->y(),
					event->state() & ControlButton,
					event->state() & ShiftButton);
  if( event->button() & LeftButton )
    GetDevice()->LeftButtonPressEvent();
  else if( event->button() & MidButton )
    GetDevice()->MiddleButtonPressEvent();
  else if( event->button() & RightButton )
    GetDevice()->RightButtonPressEvent();
}


//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::mouseReleaseEvent( QMouseEvent *event )
{
  GetDevice()->SetEventInformationFlipY(event->x(), 
					event->y(),
					event->state() & ControlButton,
					event->state() & ShiftButton);

  if( event->button() & LeftButton )
    GetDevice()->LeftButtonReleaseEvent();
  else if( event->button() & MidButton )
    GetDevice()->MiddleButtonReleaseEvent();
  else if( event->button() & RightButton )
    GetDevice()->RightButtonReleaseEvent();
}


//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::mouseDoubleClickEvent( QMouseEvent* event )
{}


//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::wheelEvent( QWheelEvent* event )
{
  setActiveWindow();
  setFocus();
}


//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::keyPressEvent( QKeyEvent* event ) 
{
  GetDevice()->SetKeyEventInformation(event->state() & ControlButton,
				      event->state() & ShiftButton,
				      event->key());
  GetDevice()->KeyPressEvent();
  GetDevice()->CharEvent();
}

//----------------------------------------------------------------------------
void
QVTK_RenderWindowInteractor
::keyReleaseEvent( QKeyEvent * event ) 
{
  GetDevice()->SetKeyEventInformation(event->state() & ControlButton,
				      event->state() & ShiftButton,
				      event->key());
  GetDevice()->KeyReleaseEvent();
}


//----------------------------------------------------------------------------
void  
QVTK_RenderWindowInteractor
::enterEvent( QEvent* event )
{
  if(FOCUS_UNDER_MOUSE){
    setActiveWindow();
    setFocus();
  }
  GetDevice()->EnterEvent();
}

//----------------------------------------------------------------------------
void  
QVTK_RenderWindowInteractor
::leaveEvent( QEvent * )
{
  GetDevice()->LeaveEvent();
}


//----------------------------------------------------------------------------
void  
QVTK_RenderWindowInteractor
::focusInEvent( QFocusEvent* event )
{
  QWidget::focusInEvent( event );

  // register set space mouse events receiver
  if(SVTK_SpaceMouse* aSpaceMouse = SVTK_SpaceMouse::getInstance()){
    if(!aSpaceMouse->isSpaceMouseOn()) {// initialize 3D space mouse driver 
      aSpaceMouse->initialize(x11Display(),winId());
    }else{
      aSpaceMouse->setWindow(x11Display(),winId());
    }
  }
}

//----------------------------------------------------------------------------
void  
QVTK_RenderWindowInteractor
::focusOutEvent ( QFocusEvent* event )
{
  QWidget::focusOutEvent( event );

  // unregister set space mouse events receiver
  if(SVTK_SpaceMouse* aSpaceMouse = SVTK_SpaceMouse::getInstance()){
    if(aSpaceMouse->isSpaceMouseOn())
      aSpaceMouse->setWindow(x11Display(),0);
  }
}


//----------------------------------------------------------------------------
bool 
QVTK_RenderWindowInteractor
::x11Event( XEvent *xEvent )
{
  // handle 3d space mouse events
  if(SVTK_SpaceMouse* aSpaceMouse = SVTK_SpaceMouse::getInstance()){
    if(aSpaceMouse->isSpaceMouseOn() && xEvent->type == ClientMessage){
      SVTK_SpaceMouse::MoveEvent anEvent;
      int type = aSpaceMouse->translateEvent( x11Display(), xEvent, &anEvent, 1.0, 1.0 );
      switch( type ){
      case SVTK_SpaceMouse::SpaceMouseMove : 
	GetDevice()->InvokeEvent(SVTK::SpaceMouseMoveEvent, anEvent.data );
	break;
      case SVTK_SpaceMouse::SpaceButtonPress :
	GetDevice()->InvokeEvent( SVTK::SpaceMouseButtonEvent, &anEvent.button );
	break;
      case SVTK_SpaceMouse::SpaceButtonRelease :
	break;
      }
      return true; // stop handling the event
    }
  }

  return QWidget::x11Event( xEvent );
}


//----------------------------------------------------------------------------
SVTK_RenderWindowInteractor
::SVTK_RenderWindowInteractor(QWidget* theParent, 
			       const char* theName):
  QVTK_RenderWindowInteractor(theParent,theName),
  myEventCallbackCommand(vtkCallbackCommand::New())
{
  myEventCallbackCommand->Delete();

  myEventCallbackCommand->SetClientData(this); 
  myPriority = 0.0;

  myEventCallbackCommand->SetCallback(SVTK_RenderWindowInteractor::ProcessEvents);
}

void
SVTK_RenderWindowInteractor
::Initialize(vtkGenericRenderWindowInteractor* theDevice,
	     SVTK_Renderer* theRenderer,
	     SVTK_Selector* theSelector)
{
  QVTK_RenderWindowInteractor::Initialize(theDevice);
  SetRenderer(theRenderer);
  SetSelector(theSelector);
}

//----------------------------------------------------------------------------
SVTK_RenderWindowInteractor
::~SVTK_RenderWindowInteractor() 
{
  // Sequence of the destruction call are fixed and should be changed.
  // vtkRenderWindow instance should be destroyed after all vtkRenderer's
  GetDevice()->SetInteractorStyle(NULL); 
  while(!myInteractorStyles.empty()){
    const PInteractorStyle& aStyle = myInteractorStyles.top();
    aStyle->SetInteractor(NULL);
    myInteractorStyles.pop();
  }

  SetRenderer(NULL);

  GetDevice()->SetRenderWindow(NULL);
}

//----------------------------------------------------------------------------
SVTK_Renderer* 
SVTK_RenderWindowInteractor
::GetRenderer()
{
  return myRenderer.GetPointer();
}

vtkRenderer* 
SVTK_RenderWindowInteractor
::getRenderer()
{
  return GetRenderer()->GetDevice();
}

void
SVTK_RenderWindowInteractor
::SetRenderer(SVTK_Renderer* theRenderer)
{
  if(theRenderer == myRenderer.GetPointer())
    return;

  if(GetRenderer())
    myRenderWindow->RemoveRenderer(getRenderer());

  myRenderer = theRenderer;

  if(GetRenderer())
    myRenderWindow->AddRenderer(getRenderer());
}


//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::InitInteractorStyle(vtkInteractorStyle* theStyle)
{
  GetDevice()->SetInteractorStyle(theStyle); 
}


//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::PushInteractorStyle(vtkInteractorStyle* theStyle)
{
  myInteractorStyles.push(PInteractorStyle(theStyle));
  InitInteractorStyle(theStyle);
}


//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::PopInteractorStyle()
{
  if(GetInteractorStyle())
    myInteractorStyles.pop();
  
  if(GetInteractorStyle()) 
    InitInteractorStyle(GetInteractorStyle());
}


//----------------------------------------------------------------------------
vtkInteractorStyle* 
SVTK_RenderWindowInteractor
::GetInteractorStyle()
{
  return myInteractorStyles.empty() ? 0 : myInteractorStyles.top().GetPointer();
}


//----------------------------------------------------------------------------
SVTK_Selector* 
SVTK_RenderWindowInteractor
::GetSelector() 
{ 
  return mySelector.GetPointer(); 
}


void
SVTK_RenderWindowInteractor
::SetSelector(SVTK_Selector* theSelector)
{ 
  if(mySelector.GetPointer())
    mySelector->RemoveObserver(myEventCallbackCommand.GetPointer());

  mySelector = theSelector; 

  if(mySelector.GetPointer())
    mySelector->AddObserver(vtkCommand::EndPickEvent, 
			    myEventCallbackCommand.GetPointer(), 
			    myPriority);
}


//----------------------------------------------------------------------------
void 
SVTK_RenderWindowInteractor
::ProcessEvents(vtkObject* vtkNotUsed(theObject), 
		unsigned long theEvent,
		void* theClientData, 
		void* vtkNotUsed(theCallData))
{
  SVTK_RenderWindowInteractor* self = reinterpret_cast<SVTK_RenderWindowInteractor*>(theClientData);

  switch(theEvent){
  case vtkCommand::EndPickEvent:
    self->onEmitSelectionChanged();
    break;
  }
}


//----------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::SetSelectionMode(Selection_Mode theMode)
{
  mySelector->SetSelectionMode(theMode);
}


//----------------------------------------------------------------
Selection_Mode
SVTK_RenderWindowInteractor
::SelectionMode() const
{
  return mySelector->SelectionMode();
}


//----------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::onEmitSelectionChanged()
{
  return emit selectionChanged();
}


//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::mouseMoveEvent( QMouseEvent* event ) 
{
  QVTK_RenderWindowInteractor::mouseMoveEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit MouseMove( event );
}


//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::mousePressEvent( QMouseEvent* event ) 
{
  QVTK_RenderWindowInteractor::mousePressEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit MouseButtonPressed( event );
}


//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::mouseReleaseEvent( QMouseEvent *event )
{
  QVTK_RenderWindowInteractor::mouseReleaseEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit MouseButtonReleased( event );
}


//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::mouseDoubleClickEvent( QMouseEvent* event )
{
  QVTK_RenderWindowInteractor::mouseDoubleClickEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit MouseDoubleClicked( event );
}


//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::wheelEvent( QWheelEvent* event )
{
  QVTK_RenderWindowInteractor::wheelEvent(event);

  if(event->delta() > 0)
    GetDevice()->InvokeEvent(SVTK::ZoomInEvent,NULL);
  else
    GetDevice()->InvokeEvent(SVTK::ZoomOutEvent,NULL);

  if(GENERATE_SUIT_EVENTS)
    emit WheelMoved( event );
}


//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::keyPressEvent( QKeyEvent* event ) 
{
  QVTK_RenderWindowInteractor::keyPressEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit KeyPressed( event );
}

//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::keyReleaseEvent( QKeyEvent * event ) 
{
  QVTK_RenderWindowInteractor::keyReleaseEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit KeyReleased( event );
}

//----------------------------------------------------------------------------
void
SVTK_RenderWindowInteractor
::contextMenuEvent( QContextMenuEvent* event )
{
  if( !( event->state() & KeyButtonMask ) )
    emit contextMenuRequested( event );
}
