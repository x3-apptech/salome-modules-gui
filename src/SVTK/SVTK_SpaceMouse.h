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
//  File   : SVTK_SpaceMouse.h
//  Author : Alexander SLADKOV

#ifndef SVTK_SpaceMouse_h
#define SVTK_SpaceMouse_h

#include <QtGlobal>

#ifndef WIN32
extern "C"
{
#include <X11/X.h>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <X11/Xlib.h>
#else
#include <xcb/xcb.h>
#endif
}

#endif

class SVTK_SpaceMouse 
{
 public:

  enum MoveData { x, y, z, a, b, c };
  enum EventType { SpaceMouseMove = 1, SpaceButtonPress, SpaceButtonRelease };

  typedef struct  {
    int type;
    int button;
    double data[6];
    int period;
  } MoveEvent;

  bool isSpaceMouseOn() const { return spaceMouseOn != 0; }

  SVTK_SpaceMouse();

 protected:

  int spaceMouseOn;
};

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)

class SVTK_SpaceMouseX: public SVTK_SpaceMouse
{

public:
  SVTK_SpaceMouseX();

  // access to SpaceMouse utility class
  static SVTK_SpaceMouseX* getInstance();

#ifndef WIN32
  int initialize     ( Display*, Window );
  int setWindow      ( Display*, Window );
  int translateEvent ( Display*, XEvent*, MoveEvent*, double, double );
  int close          ( Display* );

private:
  Atom XMotionEvent;
  Atom XButtonPressEvent;
  Atom XButtonReleaseEvent;
  Atom XCommandEvent;
  Window win;
#endif

  static SVTK_SpaceMouseX* myInstance;

};
#else
class SVTK_SpaceMouseXCB: public SVTK_SpaceMouse
{

public:
  SVTK_SpaceMouseXCB();

  // access to SpaceMouse utility class
  static SVTK_SpaceMouseXCB* getInstance();

#ifndef WIN32
  int initialize     ( xcb_connection_t*, xcb_window_t );
  int setWindow      ( xcb_connection_t*, xcb_window_t );
  int translateEvent ( xcb_connection_t*, xcb_client_message_event_t*, MoveEvent*, double, double );
  int close          ( xcb_connection_t* );

private:
  xcb_atom_t XCBMotionEvent;
  xcb_atom_t XCBButtonPressEvent;
  xcb_atom_t XCBButtonReleaseEvent;
  xcb_window_t win;
#endif

  static SVTK_SpaceMouseXCB* myInstance;

};
#endif
#endif
