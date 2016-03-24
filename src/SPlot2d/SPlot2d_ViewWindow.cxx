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

#include "SPlot2d_ViewWindow.h"

#include "Plot2d_ViewFrame.h"

#include "SUIT_Accel.h"

/*!
  Constructor
*/
SPlot2d_ViewWindow
::SPlot2d_ViewWindow( SUIT_Desktop* theDesktop, 
                      Plot2d_Viewer* theModel)
  : Plot2d_ViewWindow( theDesktop, theModel )
{
}

/*!
  Destructor
*/
SPlot2d_ViewWindow
::~SPlot2d_ViewWindow()
{
}

/*!
  Performs action
  \param theAction - type of action
*/
bool 
SPlot2d_ViewWindow
::action( const int theAction  )
{
  switch ( theAction ) {
  case SUIT_Accel::PanLeft: 
    getViewFrame()->onPanLeft();
    break;
  case SUIT_Accel::PanRight: 
    getViewFrame()->onPanRight();
    break;
  case SUIT_Accel::PanUp: 
    getViewFrame()->onPanUp();
    break;
  case SUIT_Accel::PanDown: 
    getViewFrame()->onPanDown();
    break;
  case SUIT_Accel::ZoomIn: 
    getViewFrame()->onZoomIn();
    break;
  case SUIT_Accel::ZoomOut: 
    getViewFrame()->onZoomOut();
    break;
  case SUIT_Accel::ZoomFit:
    getViewFrame()->fitAll();
    break;
  }
  return true;
}
