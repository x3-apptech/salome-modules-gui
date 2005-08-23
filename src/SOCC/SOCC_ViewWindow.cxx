#include "SOCC_ViewWindow.h"

#include "OCCViewer_ViewPort3d.h"

#include "SUIT_Accel.h"

//----------------------------------------------------------------------------
SOCC_ViewWindow
::SOCC_ViewWindow( SUIT_Desktop* theDesktop, 
		   OCCViewer_Viewer* theModel)
  : OCCViewer_ViewWindow( theDesktop, theModel )
{
}

//----------------------------------------------------------------------------
SOCC_ViewWindow
::~SOCC_ViewWindow()
{
}

//----------------------------------------------------------------------------
void 
SOCC_ViewWindow
::action( const int theAction  )
{
  const int inc = 10;
  int cx, cy;
  if ( theAction == SUIT_Accel::ZoomIn || theAction == SUIT_Accel::ZoomOut ||
       theAction == SUIT_Accel::RotateLeft || theAction == SUIT_Accel::RotateRight ||
       theAction == SUIT_Accel::RotateUp || theAction == SUIT_Accel::RotateDown ) {
    cx = myViewPort->width() / 2;
    cy = myViewPort->height() / 2;
  }
  switch ( theAction ) {
  case SUIT_Accel::PanLeft     : 
    myViewPort->pan( -inc, 0 );   
    break;
  case SUIT_Accel::PanRight    : 
    myViewPort->pan(  inc, 0 );   
    break;
  case SUIT_Accel::PanUp       : 
    myViewPort->pan( 0, inc );   
    break;
  case SUIT_Accel::PanDown     : 
    myViewPort->pan( 0, -inc );   
    break;
  case SUIT_Accel::ZoomIn      : 
    myViewPort->zoom( cx, cy, cx + inc, cy + inc );
    break;
  case SUIT_Accel::ZoomOut     : 
    myViewPort->zoom( cx, cy, cx - inc, cy - inc );
    break;
  case SUIT_Accel::ZoomFit     :
    myViewPort->fitAll();
    break;
  case SUIT_Accel::RotateLeft  : 
    myViewPort->startRotation( cx, cy );
    myViewPort->rotate( cx - inc, cy );
    myViewPort->endRotation();
    break;
  case SUIT_Accel::RotateRight :  
    myViewPort->startRotation( cx, cy );
    myViewPort->rotate( cx + inc, cy );
    myViewPort->endRotation();
    break;
  case SUIT_Accel::RotateUp    :  
    myViewPort->startRotation( cx, cy );
    myViewPort->rotate( cx, cy - inc );
    myViewPort->endRotation();
    break;
  case SUIT_Accel::RotateDown  :  
    myViewPort->startRotation( cx, cy );
    myViewPort->rotate( cx, cy + inc );
    myViewPort->endRotation();
    break;
  } 
}
