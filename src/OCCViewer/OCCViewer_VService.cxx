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

#include "OCCViewer_VService.h"

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#include <Basics_OCCTVersion.hxx>
#if OCC_VERSION_LARGE > 0x06070200 // for OCC-6.7.3 and higher version
#include <OpenGl_GraphicDriver.hxx>
#else
#include <Graphic3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#endif
#include <Aspect_DisplayConnection.hxx>

#ifdef WIN32
#include <WNT_Window.hxx>
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
#ifdef WIN32
  Handle(WNT_Window) viewWindow = new WNT_Window( aWindowHandle );
#else
  Handle(Aspect_DisplayConnection) aDispConnection = view->Viewer()->Driver()->GetDisplayConnection();
  Handle(Xw_Window) viewWindow = new Xw_Window( aDispConnection, aWindowHandle );
#endif
  return viewWindow;
}

/*!
    Creates viewer 3d [ static ]
*/
Handle(V3d_Viewer) OCCViewer_VService::CreateViewer( const Standard_ExtString name,
						     const Standard_CString displayName,
						     const Standard_CString domain,
						     const Standard_Real viewSize ,
						     const V3d_TypeOfOrientation viewProjection,
						     const Standard_Boolean computedMode,
						     const Standard_Boolean defaultComputedMode )
{
#if OCC_VERSION_LARGE > 0x06070200 // for OCC-6.7.3 and higher version
  static Handle(OpenGl_GraphicDriver) aGraphicDriver;
#else
  static Handle(Graphic3d_GraphicDriver) aGraphicDriver;
#endif
  if (aGraphicDriver.IsNull())
  {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
#ifndef WIN32
    aDisplayConnection = new Aspect_DisplayConnection( displayName );
#else
    aDisplayConnection = new Aspect_DisplayConnection();
#endif
#if OCC_VERSION_LARGE > 0x06070200 // for OCC-6.7.3 and higher version
    aGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);
#else
    aGraphicDriver = Graphic3d::InitGraphicDriver( aDisplayConnection );
#endif
  }

#if OCC_VERSION_LARGE > 0x07000000
  return new V3d_Viewer( aGraphicDriver, name, domain, viewSize, viewProjection,
			 Quantity_NOC_GRAY30, V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT,
			 computedMode, defaultComputedMode );
#else
  return new V3d_Viewer( aGraphicDriver, name, domain, viewSize, viewProjection,
			 Quantity_NOC_GRAY30, V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT,
			 computedMode, defaultComputedMode, V3d_TEX_NONE );
#endif
}
