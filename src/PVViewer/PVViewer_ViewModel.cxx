// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author: Adrien Bruneton (CEA)

#include "PVViewer_ViewModel.h"
#include "PVViewer_ViewWindow.h"

#include <utilities.h>
#include <SUIT_Desktop.h>

//----------------------------------------
PVViewer_Viewer::PVViewer_Viewer()
:SUIT_ViewModel() 
{
  MESSAGE("PVViewer_Viewer: creating view model ...");
}

/*!
  Create new instance of view window on desktop \a theDesktop.
  \retval SUIT_ViewWindow* - created view window pointer.
*/
SUIT_ViewWindow* PVViewer_Viewer::createView(SUIT_Desktop* theDesktop)
{
  PVViewer_ViewWindow* aPVView = new PVViewer_ViewWindow(theDesktop, this);
  return aPVView;
}

