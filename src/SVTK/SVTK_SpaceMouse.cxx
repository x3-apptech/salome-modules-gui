// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : SVTK_SpaceMouse.cxx
//  Author : Alexander SLADKOV

#include <string.h>
#include <math.h>
#include <stdio.h>

#ifndef WIN32
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#endif

#include "SVTK_SpaceMouse.h"

SVTK_SpaceMouse* SVTK_SpaceMouse::myInstance = 0;

/*!
  \return shared instance of object (creates if there is no one)
*/
SVTK_SpaceMouse* SVTK_SpaceMouse::getInstance()
{
  if ( !myInstance )
    myInstance = new SVTK_SpaceMouse();
  return myInstance;
}

/*!
  Constructor
*/
SVTK_SpaceMouse::SVTK_SpaceMouse()
{
#ifndef WIN32
  win = InputFocus;
#endif
  spaceMouseOn = 0;
}

#ifndef WIN32

/*!
  Initialization
*/
int SVTK_SpaceMouse::initialize( Display *display, Window window )
{
 XMotionEvent        = XInternAtom( display, "MotionEvent",        1 );
 XButtonPressEvent   = XInternAtom( display, "ButtonPressEvent",   1 );
 XButtonReleaseEvent = XInternAtom( display, "ButtonReleaseEvent", 1 );
 XCommandEvent       = XInternAtom( display, "CommandEvent",       1 );

 spaceMouseOn = (XMotionEvent        != 0) && 
                (XButtonPressEvent   != 0) && 
                (XButtonReleaseEvent != 0) && 
                (XCommandEvent       != 0);
 if ( !spaceMouseOn )
  return 0;

 spaceMouseOn = setWindow( display, window );
 if ( !spaceMouseOn )
  return 0;
 
 return spaceMouseOn; 
}

static int errorCallback( Display *display, XErrorEvent *Error )
{
  char msg[ 128 ];
  if ( Error->error_code != BadWindow ) {
    XGetErrorText( display,Error->error_code,msg,sizeof( msg ) );
    fprintf( stderr, "SpaceMouse reported error = %s. Exit ... \n", msg );
  }
  return 0;
}

/*!
  Initialize by window
*/
int SVTK_SpaceMouse::setWindow( Display *display, Window window )
{
  XTextProperty winName;
  XEvent xEvent;
  Atom type;
  int format;
  unsigned long NItems, BytesReturn;
  unsigned char *PropReturn;
  Window root;
  int result;
  int (*errorHandler)(Display *,XErrorEvent *);

  result = 1;
  errorHandler = XSetErrorHandler( errorCallback );
 
  root = RootWindow( display, DefaultScreen(display) );

  PropReturn = NULL;
  XGetWindowProperty( display, root, XCommandEvent, 0,1, 0,
                      AnyPropertyType, &type, &format, &NItems,
                      &BytesReturn, &PropReturn );

  win = InputFocus;
  if ( PropReturn != NULL ) {
    win = *(Window *) PropReturn;
    XFree( PropReturn );
  }
  else
    return result = 0;

  if ( XGetWMName( display, win, &winName ) == 0 )
    return result = 0;

  if ( strcmp( (char *) "Magellan Window", (char *) winName.value) != 0 )
    return result = 0;

  xEvent.type = ClientMessage;
  xEvent.xclient.format = 16;
  xEvent.xclient.send_event = 0;
  xEvent.xclient.display = display;
  xEvent.xclient.window = win;
  xEvent.xclient.message_type = XCommandEvent;
  
  xEvent.xclient.data.s[0] = (short) ((window>>16)&0x0000FFFF);
  xEvent.xclient.data.s[1] = (short)  (window&0x0000FFFF);
  xEvent.xclient.data.s[2] = 27695;

  if ( XSendEvent( display, win, 0, 0x0000, &xEvent ) == 0 )
    return result = 0;

  XFlush( display );

  XSetErrorHandler( errorHandler );
  return result;
}

/*!
  Close
*/
int SVTK_SpaceMouse::close(Display *display)
{
  initialize( display, (Window)InputFocus );
  spaceMouseOn = 0;
  
  return 1;
}

/*!
  Custom event handler
*/
int SVTK_SpaceMouse::translateEvent( Display* display, XEvent* xEvent, MoveEvent* spaceMouseEvent,
                    double scale, double rScale )
{
  if ( !spaceMouseOn )
    return 0;

  if ( xEvent->type == ClientMessage ) {
    if ( xEvent->xclient.message_type == XMotionEvent ) {
      spaceMouseEvent->type = SpaceMouseMove;
      spaceMouseEvent->data[ x ] =
        xEvent->xclient.data.s[2] * scale;
      spaceMouseEvent->data[ y ] =
        xEvent->xclient.data.s[3] * scale;
      spaceMouseEvent->data[ z ] =
        xEvent->xclient.data.s[4] * scale;
      spaceMouseEvent->data[ a ] =
        xEvent->xclient.data.s[5] * rScale;
      spaceMouseEvent->data[ b ] =
        xEvent->xclient.data.s[6] * rScale;
      spaceMouseEvent->data[ c ] =
        xEvent->xclient.data.s[7] * rScale;
      spaceMouseEvent->period = xEvent->xclient.data.s[8];
      return 1;
    }
    else if ( xEvent->xclient.message_type == XButtonPressEvent ) {
      spaceMouseEvent->type = SpaceButtonPress;
      spaceMouseEvent->button = xEvent->xclient.data.s[2];
      return 2;
    }
    else if ( xEvent->xclient.message_type == XButtonReleaseEvent ) {
      spaceMouseEvent->type = SpaceButtonRelease;
      spaceMouseEvent->button = xEvent->xclient.data.s[2];
      return 3;
    }
  }
  return (!display);
}

#endif
