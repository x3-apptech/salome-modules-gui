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
//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : 
//  Author : 
//  Module : SALOME
//  $Header$
//
#include "SVTK_RenderWindowInteractor.h"
//#include "SVTK_GenericRenderWindowInteractor.h"

#include "SVTK_InteractorStyle.h"
#include "SVTK_Renderer.h"
#include "SVTK_Functor.h"
#include "SALOME_Actor.h"

// QT Includes
// Put Qt includes before the X11 includes which #define the symbol None
// (see SVTK_SpaceMouse.h) to avoid the compilation error.
#ifndef WIN32
# include <QX11Info>
#endif
#include <QMouseEvent>

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

using namespace std;

static bool GENERATE_SUIT_EVENTS = false;
static bool FOCUS_UNDER_MOUSE = false;


/*!
  Constructor
*/
QVTK_RenderWindowInteractor
::QVTK_RenderWindowInteractor(QWidget* theParent, 
			      const char* theName):
  QWidget(theParent),
  myRenderWindow(vtkRenderWindow::New())
{
  setAttribute( Qt::WA_PaintOnScreen );
  setAttribute( Qt::WA_NoSystemBackground );

  setObjectName(theName);

  setMouseTracking(true);

  myRenderWindow->Delete();
  myRenderWindow->DoubleBufferOn();

#ifndef WIN32
  myRenderWindow->SetDisplayId((void*)QX11Info::display());
#endif
  myRenderWindow->SetWindowId((void*)winId());
}

/*!
  To initialize by vtkGenericRenderWindowInteractor instance
*/
void 
QVTK_RenderWindowInteractor
::Initialize(vtkGenericRenderWindowInteractor* theDevice)
{
  if ( GetDevice() )
    myDevice->SetRenderWindow( NULL );

  myDevice = theDevice;

  if ( theDevice )
    theDevice->SetRenderWindow( getRenderWindow() );
}

/*!
  Destructor
*/
QVTK_RenderWindowInteractor
::~QVTK_RenderWindowInteractor() 
{
#ifndef WIN32
  SVTK_SpaceMouse* aSpaceMouse = SVTK_SpaceMouse::getInstance();
  if ( aSpaceMouse && aSpaceMouse->isSpaceMouseOn() )
    aSpaceMouse->close( QX11Info::display() );
#endif
}


/*!
  \return corresponding render window interactor
*/
vtkGenericRenderWindowInteractor* 
QVTK_RenderWindowInteractor
::GetDevice()
{
  return myDevice.GetPointer();
}

/*!
  \return corresponding render window
*/
vtkRenderWindow*
QVTK_RenderWindowInteractor
::getRenderWindow()
{
  return myRenderWindow.GetPointer();
}

/*!
  Just to simplify usage of its device (vtkGenericRenderWindowInteractor)
*/
void
QVTK_RenderWindowInteractor
::InvokeEvent(unsigned long theEvent, void* theCallData)
{
  GetDevice()->InvokeEvent(theEvent,theCallData);
}

/*!
  Need for initial contents display on Win32
*/
void
QVTK_RenderWindowInteractor
::show()
{
  QWidget::show();
  update(); // needed for initial contents display on Win32
}

/*!
  To implement final initialization, just before the widget is displayed
*/
void
QVTK_RenderWindowInteractor
::polish()
{
  // Final initialization just before the widget is displayed
  GetDevice()->SetSize(width(),height());
  if(!GetDevice()->GetInitialized() && GetDevice()->GetRenderWindow()){
    GetDevice()->Initialize();
    GetDevice()->ConfigureEvent();
  }
}

/*!
  To adjust widget and vtkRenderWindow size
*/
void
QVTK_RenderWindowInteractor
::resize(int w, int h) 
{
  GetDevice()->UpdateSize(w,h);
}

/*!
  Custom paint event handler
*/
void
QVTK_RenderWindowInteractor
::paintEvent( QPaintEvent* theEvent ) 
{
  GetDevice()->CreateTimer(VTKI_TIMER_FIRST);
}


/*!
  Custom resize event handler
*/
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



/*!
  Custom context menu event handler
*/
void
QVTK_RenderWindowInteractor
::contextMenuEvent( QContextMenuEvent* event )
{}

/*!
  Custom mouse move event handler
*/
void
QVTK_RenderWindowInteractor
::mouseMoveEvent( QMouseEvent* event ) 
{
  GetDevice()->SetEventInformationFlipY(event->x(), 
					event->y(),
					event->modifiers() & Qt::ControlModifier,
					event->modifiers() & Qt::ShiftModifier);
  GetDevice()->MouseMoveEvent();
}


/*!
  Custom mouse press event handler
*/
void
QVTK_RenderWindowInteractor
::mousePressEvent( QMouseEvent* event ) 
{
  GetDevice()->SetEventInformationFlipY(event->x(), 
					event->y(),
					event->modifiers() & Qt::ControlModifier,
					event->modifiers() & Qt::ShiftModifier);
  if( event->button() & Qt::LeftButton )
    GetDevice()->LeftButtonPressEvent();
  else if( event->button() & Qt::MidButton )
    GetDevice()->MiddleButtonPressEvent();
  else if( event->button() & Qt::RightButton )
    GetDevice()->RightButtonPressEvent();
}


/*!
  Custom mouse release event handler
*/
void
QVTK_RenderWindowInteractor
::mouseReleaseEvent( QMouseEvent *event )
{
  GetDevice()->SetEventInformationFlipY(event->x(), 
					event->y(),
					event->modifiers() & Qt::ControlModifier,
					event->modifiers() & Qt::ShiftModifier);

  if( event->button() & Qt::LeftButton )
    GetDevice()->LeftButtonReleaseEvent();
  else if( event->button() & Qt::MidButton )
    GetDevice()->MiddleButtonReleaseEvent();
  else if( event->button() & Qt::RightButton )
    GetDevice()->RightButtonReleaseEvent();
}


/*!
  Custom mouse double click event handler
*/
void
QVTK_RenderWindowInteractor
::mouseDoubleClickEvent( QMouseEvent* event )
{}


/*!
  Custom mouse wheel event handler
*/
void
QVTK_RenderWindowInteractor
::wheelEvent( QWheelEvent* event )
{
  activateWindow();
  setFocus();
}


/*!
  Custom key press event handler
*/
void
QVTK_RenderWindowInteractor
::keyPressEvent( QKeyEvent* event ) 
{
  GetDevice()->SetKeyEventInformation(event->modifiers() & Qt::ControlModifier,
				      event->modifiers() & Qt::ShiftModifier,
				      event->key());
  GetDevice()->KeyPressEvent();
  GetDevice()->CharEvent();
}

/*!
  Custom key release event handler
*/
void
QVTK_RenderWindowInteractor
::keyReleaseEvent( QKeyEvent * event ) 
{
  GetDevice()->SetKeyEventInformation(event->modifiers() & Qt::ControlModifier,
				      event->modifiers() & Qt::ShiftModifier,
				      event->key());
  GetDevice()->KeyReleaseEvent();
}


/*!
  Custom enter event handler
*/
void  
QVTK_RenderWindowInteractor
::enterEvent( QEvent* event )
{
  if(FOCUS_UNDER_MOUSE){
    activateWindow();
    setFocus();
  }
  GetDevice()->EnterEvent();
}

/*!
  Custom leave event handler
*/
void  
QVTK_RenderWindowInteractor
::leaveEvent( QEvent * )
{
  GetDevice()->LeaveEvent();
}

/*!
  Reimplemented from QWidget in order to set window - receiver
  of space mouse events. 
*/
void  
QVTK_RenderWindowInteractor
::focusInEvent( QFocusEvent* event )
{
  QWidget::focusInEvent( event );

#ifndef WIN32
  // register set space mouse events receiver
  SVTK_SpaceMouse* aSpaceMouse = SVTK_SpaceMouse::getInstance();

  if ( aSpaceMouse )
  {
    if ( !aSpaceMouse->isSpaceMouseOn() )
      // initialize 3D space mouse driver 
      aSpaceMouse->initialize( QX11Info::display(), winId() );
    else
      aSpaceMouse->setWindow( QX11Info::display(), winId() );
  }
#endif
}

/*!
  Reimplemented from QWidget in order to set window - receiver
  of space mouse events. 
*/
void  
QVTK_RenderWindowInteractor
::focusOutEvent ( QFocusEvent* event )
{
  QWidget::focusOutEvent( event );

#ifndef WIN32
  // unregister set space mouse events receiver
  SVTK_SpaceMouse* aSpaceMouse = SVTK_SpaceMouse::getInstance();
  if ( aSpaceMouse && aSpaceMouse->isSpaceMouseOn() )
    aSpaceMouse->setWindow( QX11Info::display(), 0 );
#endif
}


#ifdef WIN32

/*!
  To handle native Win32 events (from such devices as SpaceMouse)
*/
bool QVTK_RenderWindowInteractor::winEvent( MSG* msg, long* result )
{
  // TODO: Implement event handling for SpaceMouse
  return QWidget::winEvent( msg, result);
}

#else

/*!
  To handle native X11 events (from such devices as SpaceMouse)
*/
bool 
QVTK_RenderWindowInteractor
::x11Event( XEvent *xEvent )
{
  // handle 3d space mouse events
  if ( SVTK_SpaceMouse* aSpaceMouse = SVTK_SpaceMouse::getInstance() )
  {
    if ( aSpaceMouse->isSpaceMouseOn() && xEvent->type == ClientMessage )
    {
      SVTK_SpaceMouse::MoveEvent anEvent;
      int type = aSpaceMouse->translateEvent( QX11Info::display(), xEvent, &anEvent, 1.0, 1.0 );
      switch ( type )
      {
      case SVTK_SpaceMouse::SpaceMouseMove:
	      GetDevice()->InvokeEvent( SVTK::SpaceMouseMoveEvent, anEvent.data );
	      break;
      case SVTK_SpaceMouse::SpaceButtonPress:
	      GetDevice()->InvokeEvent( SVTK::SpaceMouseButtonEvent, &anEvent.button );
	      break;
      case SVTK_SpaceMouse::SpaceButtonRelease:
	      break;
      }
      return true; // stop handling the event
    }
  }

  return QWidget::x11Event( xEvent );
}

#endif

/*!
  Constructor
*/
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

/*!
  To initialize properly the class
*/
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

/*!
  Destructor
*/
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

/*!
  To get corresponding SVTK_Renderer instance
*/
SVTK_Renderer* 
SVTK_RenderWindowInteractor
::GetRenderer()
{
  return myRenderer.GetPointer();
}

/*!
  To get corresponding SVTK_Renderer device (just to simplify collobaration with SVTK_Renderer)
*/
vtkRenderer* 
SVTK_RenderWindowInteractor
::getRenderer()
{
  return GetRenderer()->GetDevice();
}

/*!
  Changes renderer
  \param theRenderer - new renderer
*/
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


/*!
  Changes interactor style
  \param theStyle - new interactor style
*/
void
SVTK_RenderWindowInteractor
::InitInteractorStyle(vtkInteractorStyle* theStyle)
{
  GetDevice()->SetInteractorStyle(theStyle); 
}

/*!
  To change current interactor style by pushing the new one into the container
*/
void
SVTK_RenderWindowInteractor
::PushInteractorStyle(vtkInteractorStyle* theStyle)
{
  myInteractorStyles.push(PInteractorStyle(theStyle));
  InitInteractorStyle(theStyle);
}

/*!
  To restore previous interactor style
*/
void
SVTK_RenderWindowInteractor
::PopInteractorStyle()
{
  if(GetInteractorStyle())
    myInteractorStyles.pop();
  
  if(GetInteractorStyle()) 
    InitInteractorStyle(GetInteractorStyle());
}

/*!
  To get current interactor style
*/
vtkInteractorStyle* 
SVTK_RenderWindowInteractor
::GetInteractorStyle()
{
  return myInteractorStyles.empty() ? 0 : myInteractorStyles.top().GetPointer();
}


/*!
  To get current selector
*/
SVTK_Selector* 
SVTK_RenderWindowInteractor
::GetSelector() 
{ 
  return mySelector.GetPointer(); 
}


/*!
  Changes selector
  \param theSelector - new selector
*/
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

/*!
  Main process VTK event method
*/
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

/*!
  To change selection mode (just to simplify collobaration with SVTK_Selector)
*/
void
SVTK_RenderWindowInteractor
::SetSelectionMode(Selection_Mode theMode)
{
  mySelector->SetSelectionMode(theMode);
}

/*!
  To get current selection mode (just to simplify collobaration with SVTK_Selector)
*/
Selection_Mode
SVTK_RenderWindowInteractor
::SelectionMode() const
{
  return mySelector->SelectionMode();
}


/*!
  Emits signal selectionChanged()
*/
void
SVTK_RenderWindowInteractor
::onEmitSelectionChanged()
{
  return emit selectionChanged();
}


/*!
  Custom mouse move event handler
*/
void
SVTK_RenderWindowInteractor
::mouseMoveEvent( QMouseEvent* event ) 
{
  QVTK_RenderWindowInteractor::mouseMoveEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit MouseMove( event );
}


/*!
  Custom mouse press event handler
*/
void
SVTK_RenderWindowInteractor
::mousePressEvent( QMouseEvent* event ) 
{
  QVTK_RenderWindowInteractor::mousePressEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit MouseButtonPressed( event );
}


/*!
  Custom mouse release event handler
*/
void
SVTK_RenderWindowInteractor
::mouseReleaseEvent( QMouseEvent *event )
{
  bool aRightBtn = event->button() == Qt::RightButton;
  bool isOperation = false;
  if( aRightBtn && GetInteractorStyle()) {
    SVTK_InteractorStyle* style = dynamic_cast<SVTK_InteractorStyle*>( GetInteractorStyle() );
    if ( style )
      isOperation = style->CurrentState() != VTK_INTERACTOR_STYLE_CAMERA_NONE;
  }

  QVTK_RenderWindowInteractor::mouseReleaseEvent(event);

  if ( aRightBtn && !isOperation && !( event->modifiers() & Qt::ControlModifier ) &&
       !( event->modifiers() & Qt::ShiftModifier ) ) {
    QContextMenuEvent aEvent( QContextMenuEvent::Mouse,
                              event->pos(), event->globalPos() );
    emit contextMenuRequested( &aEvent );
  }
  if(GENERATE_SUIT_EVENTS)
    emit MouseButtonReleased( event );
}


/*!
  Custom mouse double click event handler
*/
void
SVTK_RenderWindowInteractor
::mouseDoubleClickEvent( QMouseEvent* event )
{
  QVTK_RenderWindowInteractor::mouseDoubleClickEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit MouseDoubleClicked( event );
}


/*!
  Custom mouse wheel event handler
*/
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

/*!
  Custom key press event handler
*/
void
SVTK_RenderWindowInteractor
::keyPressEvent( QKeyEvent* event ) 
{
  QVTK_RenderWindowInteractor::keyPressEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit KeyPressed( event );
}

/*!
  Custom key release event handler
*/
void
SVTK_RenderWindowInteractor
::keyReleaseEvent( QKeyEvent * event ) 
{
  QVTK_RenderWindowInteractor::keyReleaseEvent(event);

  if(GENERATE_SUIT_EVENTS)
    emit KeyReleased( event );
}

