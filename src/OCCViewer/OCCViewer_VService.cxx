// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "OCCViewer_VService.h"

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#include <OpenGl_GraphicDriver.hxx>
#include <Aspect_DisplayConnection.hxx>

#if defined(WIN32)
#include <WNT_Window.hxx>
#elif defined(__APPLE__)
#include <Cocoa_Window.hxx>
#else
#include <Xw_Window.hxx>
#endif

/*!
    Create native view window for CasCade view [ static ]
*/
Handle(Aspect_Window) OCCViewer_VService::CreateWindow( const Handle(V3d_View)& view,
							WId winId )
{
  Aspect_Handle aWindowHandle = (Aspect_Handle)winId;
#if defined(WIN32)
  Handle(WNT_Window) viewWindow = new WNT_Window( aWindowHandle );
#elif defined(__APPLE__)
  Handle(Cocoa_Window) viewWindow = new Cocoa_Window( (NSView*)winId );
#else
  Handle(Aspect_DisplayConnection) aDispConnection = view->Viewer()->Driver()->GetDisplayConnection();
  Handle(Xw_Window) viewWindow = new Xw_Window( aDispConnection, aWindowHandle );
#endif
  return viewWindow;
}

/*!
    Creates viewer 3d [ static ]
*/
Handle(V3d_Viewer) OCCViewer_VService::CreateViewer()
{
  static Handle(OpenGl_GraphicDriver) aGraphicDriver;
  if (aGraphicDriver.IsNull())
    aGraphicDriver = new OpenGl_GraphicDriver(new Aspect_DisplayConnection());
  return new V3d_Viewer(aGraphicDriver);
}
