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
//  File   : SVTK_SpaceMouse.h
//  Author : Alexander SLADKOV
//  Module : SALOME
//  $Header$

#ifndef SVTK_SpaceMouse_h
#define SVTK_SpaceMouse_h

#ifndef WIN32
extern "C"
{
#include <X11/X.h>
#include <X11/Xlib.h>
}
#endif

class SVTK_SpaceMouse 
{
 public:

  // access to SpaceMouse utility class
  static SVTK_SpaceMouse* getInstance();

  enum MoveData { x, y, z, a, b, c };
  enum EventType { SpaceMouseMove = 1, SpaceButtonPress, SpaceButtonRelease };

  typedef struct  {
    int type;
    int button;
    double data[6];
    int period;
  } MoveEvent;

  bool isSpaceMouseOn() const { return spaceMouseOn != 0; }

#ifndef WIN32
  int initialize     ( Display*, Window );
  int setWindow      ( Display*, Window );
  int translateEvent ( Display*, XEvent*, MoveEvent*, double, double );
  int close          ( Display* );
#endif

 private:
  SVTK_SpaceMouse();
  static SVTK_SpaceMouse* myInstance;

#ifndef WIN32
  Atom XMotionEvent;
  Atom XButtonPressEvent;
  Atom XButtonReleaseEvent;
  Atom XCommandEvent;

  Window win;
#endif
  int spaceMouseOn;
};

#endif
