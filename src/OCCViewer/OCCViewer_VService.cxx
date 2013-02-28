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

#include "OCCViewer_VService.h"

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#include <Basics_OCCTVersion.hxx>

#if OCC_VERSION_LARGE > 0x06050500

//
// new code, changed in OCCT v6.6.0
//

#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#ifdef WNT
#include <WNT_Window.hxx>
#else
#include <Xw_Window.hxx>
#endif

#else // #if OCC_VERSION_LARGE > 0x06050500

//
// obsolete code (before OCCT v6.6.0); to be removed
//

#ifdef WNT
#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#else
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#endif

#endif // #if OCC_VERSION_LARGE > 0x06050500

/*!
    Create native view window for CasCade view [ static ]
*/
Handle(Aspect_Window) OCCViewer_VService::CreateWindow( const Handle(V3d_View)& view,
							WId winId )
{
#if OCC_VERSION_LARGE > 0x06050500

//
// new code, changed in OCCT v6.6.0
//
  Aspect_Handle aWindowHandle = (Aspect_Handle)winId;
#ifdef WNT
  Handle(WNT_Window) viewWindow = new WNT_Window( aWindowHandle );
#else
  Handle(Aspect_DisplayConnection) aDispConnection = view->Viewer()->Driver()->GetDisplayConnection();
  Handle(Xw_Window) viewWindow = new Xw_Window( aDispConnection, aWindowHandle );
#endif
  return viewWindow;

#else // #if OCC_VERSION_LARGE > 0x06050500

//
// obsolete code (before OCCT v6.6.0); to be removed
//
  int hwnd = (int)winId;
  short lowin = (short)hwnd;
  short hiwin = (short)( hwnd >> 16 );

#ifdef WNT
  Handle(WNT_Window) viewWindow = new WNT_Window( Handle(Graphic3d_WNTGraphicDevice)::DownCast(view->Viewer()->Device()), hiwin, lowin );
  // Prevent flickering
  viewWindow->SetFlags( WDF_NOERASEBKGRND );
#else
  Handle(Xw_Window) viewWindow = new Xw_Window( Handle(Graphic3d_GraphicDevice)::DownCast(view->Viewer()->Device()), hiwin, lowin, Xw_WQ_SAMEQUALITY );
#endif
  return viewWindow;

#endif // #if OCC_VERSION_LARGE > 0x06050500
}

/*!
    Maps CasCade view to the window [ static ]
*/
// void OCCViewer_VService::SetWindow( const Handle(V3d_View)& view,
//                                     const Standard_Integer hiwin,
//                                     const Standard_Integer lowin,
//                                     const Xw_WindowQuality quality )
// {
//   view->SetWindow( OCCViewer_VService::CreateWindow( view, hiwin, lowin, quality ) );
// }

/*!
    Magnifies 'view' based on previous view [ static ]
*/
// void OCCViewer_VService::SetMagnify( const Handle(V3d_View)& view,
// 				     const Standard_Integer hiwin,
// 				     const Standard_Integer lowin,
// 				     const Handle(V3d_View)& prevView,
// 				     const Standard_Integer x1,
// 				     const Standard_Integer y1,
// 				     const Standard_Integer x2,
// 				     const Standard_Integer y2,
// 				     const Xw_WindowQuality aQuality )
// {
// #ifdef WNT
//   Handle(WNT_Window) w =
//     new WNT_Window( Handle(Graphic3d_WNTGraphicDevice)::DownCast(view->Viewer()->Device()), hiwin, lowin );
// #else
//   Handle(Xw_Window) w =
//     new Xw_Window( Handle(Graphic3d_GraphicDevice)::DownCast(view->Viewer()->Device()), hiwin, lowin, aQuality );
// #endif
//   view->SetMagnify( w, prevView, x1, y1, x2, y2 );
// }

/*!
    Creates viewer 3d [ static ]
*/
Handle(V3d_Viewer) OCCViewer_VService::Viewer3d( const Standard_CString aDisplay,
						 const Standard_ExtString aName,
						 const Standard_CString aDomain,
						 const Standard_Real ViewSize ,
						 const V3d_TypeOfOrientation ViewProj,
						 const Standard_Boolean ComputedMode,
						 const Standard_Boolean aDefaultComputedMode )
{
#if OCC_VERSION_LARGE > 0x06050500

//
// new code, changed in OCCT v6.6.0
//

  static Handle(Graphic3d_GraphicDriver) aGraphicDriver;
  if (aGraphicDriver.IsNull())
  {
    Handle(Aspect_DisplayConnection) aDisplayConnection;
#ifndef WNT
    aDisplayConnection = new Aspect_DisplayConnection( aDisplay );
#endif
    aGraphicDriver = Graphic3d::InitGraphicDriver( aDisplayConnection );
  }

  return new V3d_Viewer( aGraphicDriver, aName, aDomain, ViewSize, ViewProj,
			 Quantity_NOC_GRAY30, V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT,
			 ComputedMode, aDefaultComputedMode, V3d_TEX_NONE );
  
#else // #if OCC_VERSION_LARGE > 0x06050500

//
// obsolete code (before OCCT v6.6.0); to be removed
//

#ifndef WNT
  static Handle(Graphic3d_GraphicDevice) defaultdevice;
  if ( defaultdevice.IsNull() )
    defaultdevice = new Graphic3d_GraphicDevice( aDisplay );
  return new V3d_Viewer( defaultdevice, aName, aDomain, ViewSize, ViewProj,
			 Quantity_NOC_GRAY30, V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT,
			 ComputedMode, aDefaultComputedMode, V3d_TEX_NONE );
#else
  static Handle(Graphic3d_WNTGraphicDevice) defaultdevice;
  if ( defaultdevice.IsNull() )
    defaultdevice = new Graphic3d_WNTGraphicDevice();
  return new V3d_Viewer( defaultdevice, aName, aDomain, ViewSize, ViewProj,
			 Quantity_NOC_GRAY30, V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT,
			 ComputedMode, aDefaultComputedMode, V3d_TEX_NONE);
#endif  // WNT

#endif // #if OCC_VERSION_LARGE > 0x06050500
}
