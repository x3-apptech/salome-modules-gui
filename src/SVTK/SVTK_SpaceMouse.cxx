// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#include <QtGlobal>

#ifndef WIN32
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <X11/Xlib.h>
#else
#include <xcb/xcb.h>
#endif
#endif

#include "SVTK_SpaceMouse.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
SVTK_SpaceMouseX* SVTK_SpaceMouseX::myInstance = 0;
#else
SVTK_SpaceMouseXCB* SVTK_SpaceMouseXCB::myInstance = 0;
#endif

/*!
  Constructor
*/
SVTK_SpaceMouse::SVTK_SpaceMouse()
{
  spaceMouseOn = 0;
}
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
/*!
  Constructor
*/
SVTK_SpaceMouseX::SVTK_SpaceMouseX()
: SVTK_SpaceMouse()
{
#ifndef WIN32
  win = InputFocus;
#endif
}

/*!
  \return shared instance of object (creates if there is no one)
*/
SVTK_SpaceMouseX* SVTK_SpaceMouseX::getInstance()
{
  if ( !myInstance )
    myInstance = new SVTK_SpaceMouseX();
  return myInstance;
}

#ifndef WIN32

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
  Initialization
*/
int SVTK_SpaceMouseX::initialize( Display *display, Window window )
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

/*!
  Initialize by window
*/
int SVTK_SpaceMouseX::setWindow( Display *display, Window window )
{
  XTextProperty winName;
  XEvent xEvent;
  Atom type;
  int format;
  unsigned long NItems, BytesReturn;
  unsigned char *PropReturn;
  Window root;
  int (*errorHandler)(Display *,XErrorEvent *);

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
    return 0;

  if ( XGetWMName( display, win, &winName ) == 0 )
    return 0;

  if ( strcmp( (char *) "Magellan Window", (char *) winName.value) != 0 )
    return 0;

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
    return 0;

  XFlush( display );

  XSetErrorHandler( errorHandler );
  return 1;
}

/*!
  Close
*/
int SVTK_SpaceMouseX::close(Display *display)
{
  initialize( display, (Window)InputFocus );
  spaceMouseOn = 0;
  
  return 1;
}

/*!
  Custom event handler
*/
int SVTK_SpaceMouseX::translateEvent( Display* display, XEvent* xEvent, MoveEvent* spaceMouseEvent,
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
#else

/*!
  Constructor
*/
SVTK_SpaceMouseXCB::SVTK_SpaceMouseXCB()
: SVTK_SpaceMouse()
{
#ifndef WIN32
  win = InputFocus;
#endif
}

/*!
  \return shared instance of object (creates if there is no one)
*/
SVTK_SpaceMouseXCB* SVTK_SpaceMouseXCB::getInstance()
{
  if ( !myInstance )
    myInstance = new SVTK_SpaceMouseXCB();
  return myInstance;
}
/*!
  Initialization
*/
#ifndef WIN32
int SVTK_SpaceMouseXCB::initialize( xcb_connection_t *connection, xcb_window_t window )
{
  // make request
  xcb_intern_atom_cookie_t cookie = xcb_intern_atom (connection, 0, strlen("XCB_MOTION_NOTIFY"), "XCB_MOTION_NOTIFY" );
  // get response
  xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply ( connection, cookie, NULL );
  XCBMotionEvent = reply->atom;
  cookie = xcb_intern_atom (connection, 0, strlen("XCB_BUTTON_PRESS"), "XCB_BUTTON_PRESS" );
  reply = xcb_intern_atom_reply ( connection, cookie, NULL );
  XCBButtonPressEvent = reply->atom;
  cookie = xcb_intern_atom (connection, 0, strlen("XCB_BUTTON_RELEASE"), "XCB_BUTTON_RELEASE" );
  reply = xcb_intern_atom_reply ( connection, cookie, NULL );
  XCBButtonReleaseEvent = reply->atom;

  free (reply);

  spaceMouseOn = (XCBMotionEvent        != 0) &&
                 (XCBButtonPressEvent   != 0) &&
                 (XCBButtonReleaseEvent != 0);
  if ( !spaceMouseOn )
    return 0;

  spaceMouseOn = setWindow( connection, window );
  if ( !spaceMouseOn )
    return 0;

  return spaceMouseOn;
}

/*!
  Initialize by window
*/
int SVTK_SpaceMouseXCB::setWindow( xcb_connection_t *connection, xcb_window_t window )
{
  xcb_client_message_event_t xcbEvent;
  xcb_window_t root_window;
  xcb_screen_t* screen;
  xcb_generic_error_t* error;
  xcb_void_cookie_t cookie;

  screen = xcb_setup_roots_iterator ( xcb_get_setup ( connection ) ).data;

  /* root window */
  if ( screen )
    root_window = screen->root;

  xcb_get_property_cookie_t prop_cookie;
  xcb_get_property_reply_t* prop_reply;
  prop_cookie = xcb_get_property (connection, 0, root_window, XCB_ATOM_WM_NAME,
                                  XCB_ATOM_STRING, 0, 0);

  win = InputFocus;

  if (( prop_reply = xcb_get_property_reply ( connection, prop_cookie, NULL ) ))
  {
    int len = xcb_get_property_value_length( prop_reply );

    if ( len == 0 )
    {
      free(prop_reply);
      return 0;
    }

    if ( strcmp( (char *) "Magellan Window", (char *) xcb_get_property_value( prop_reply ) ) != 0 )
      return 0;

    win = * ( xcb_window_t * ) xcb_get_property_value( prop_reply );
  }

  free(prop_reply);

  xcbEvent.response_type = XCB_CLIENT_MESSAGE;
  xcbEvent.format = 16;
  xcbEvent.sequence = 0;
  xcbEvent.window = win;

  xcbEvent.data.data16[0] = (short) ((window>>16)&0x0000FFFF);
  xcbEvent.data.data16[1] = (short)  (window&0x0000FFFF);
  xcbEvent.data.data16[2] = 27695;

  cookie = xcb_send_event( connection, 0, win, 0x0000, (const char *)&xcbEvent );


  if (( error = xcb_request_check( connection, cookie )))
  {
    if ( error->error_code != BadWindow )
    {
      fprintf ( stderr, "SpaceMouse reported error = %d. Exit ... \n", error->error_code);
    }
    return 0;
  }

  xcb_flush(connection);

  return 1;
}

/*!
  Close
*/
int SVTK_SpaceMouseXCB::close(xcb_connection_t *connection)
{
  initialize( connection, (xcb_window_t)InputFocus );
  spaceMouseOn = 0;

  return 1;
}

/*!
  Custom event handler
*/
int SVTK_SpaceMouseXCB::translateEvent( xcb_connection_t* connection, xcb_client_message_event_t* xcbEvent, MoveEvent* spaceMouseEvent,
                    double scale, double rScale )
{
  if ( !spaceMouseOn )
    return 0;

  if ( xcbEvent->response_type == XCB_CLIENT_MESSAGE ) {
    if ( xcbEvent->type == XCBMotionEvent ) {
      spaceMouseEvent->type = SpaceMouseMove;
      spaceMouseEvent->data[ x ] =
        xcbEvent->data.data16[2] * scale;
      spaceMouseEvent->data[ y ] =
        xcbEvent->data.data16[3] * scale;
      spaceMouseEvent->data[ z ] =
        xcbEvent->data.data16[4] * scale;
      spaceMouseEvent->data[ a ] =
        xcbEvent->data.data16[5] * rScale;
      spaceMouseEvent->data[ b ] =
        xcbEvent->data.data16[6] * rScale;
      spaceMouseEvent->data[ c ] =
        xcbEvent->data.data16[7] * rScale;
      spaceMouseEvent->period = xcbEvent->data.data16[8];
      return 1;
    }
    else if ( xcbEvent->type == XCBButtonPressEvent ) {
      spaceMouseEvent->type = SpaceButtonPress;
      spaceMouseEvent->button = xcbEvent->data.data16[2];
      return 2;
    }
    else if ( xcbEvent->type == XCBButtonReleaseEvent ) {
      spaceMouseEvent->type = SpaceButtonRelease;
      spaceMouseEvent->button = xcbEvent->data.data16[2];
      return 3;
    }
  }
  return (!connection);
}
#endif
#endif
