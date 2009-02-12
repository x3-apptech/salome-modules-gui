//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "OCCViewer_VService.h"
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <Viewer_Viewer.hxx>

#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>
#include <Viewer_Viewer.hxx>

#ifdef WNT
#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#include <WNT_GraphicDevice.hxx>
#include <WNT_GraphicDevice.hxx>
#include <WNT_WDriver.hxx>
#include <InterfaceGraphic_WNT.hxx>
#else
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#include <Xw_Driver.hxx>
//#include <Xdps_Driver.hxx>
#include <Xw_TypeOfMapping.hxx>
#endif

// For 2d
#define LOPTIM
#ifdef WNT
#ifndef LOPTIM
static Handle(WNT_GraphicDevice) XServiceDefault2dDevice;
static Handle(WNT_GraphicDevice) XServiceImageDevice;
#else
static Handle(WNT_GraphicDevice)& _XServiceDefault2dDevice() {
static Handle(WNT_GraphicDevice) XServiceDefault2dDevice;
return XServiceDefault2dDevice;
}
#define XServiceDefault2dDevice _XServiceDefault2dDevice()

static Handle(WNT_GraphicDevice)& _XServiceImageDevice() {
static Handle(WNT_GraphicDevice) XServiceImageDevice;
return XServiceImageDevice;
}
#define XServiceImageDevice _XServiceImageDevice()
#endif // LOPTIM
#else
#ifndef LOPTIM
static Handle(Xw_GraphicDevice) XServiceDefault2dDevice;
static Handle(Xw_GraphicDevice) XServiceImageDevice;
#else
static Handle(Xw_GraphicDevice)& _XServiceDefault2dDevice() {
static Handle(Xw_GraphicDevice) XServiceDefault2dDevice;
return XServiceDefault2dDevice;
}
#define XServiceDefault2dDevice _XServiceImageDevice()

static Handle(Xw_GraphicDevice)& _XServiceImageDevice() {
static Handle(Xw_GraphicDevice) XServiceImageDevice;
return XServiceImageDevice;
}
#define XServiceImageDevice _XServiceImageDevice()
#endif // LOPTIM
#endif // WNT

/*!
    Maps CasCade view to the window [ static ]
*/
void OCCViewer_VService::SetWindow( const Handle(V3d_View)& view,
	                          const Standard_Integer hiwin,
	                          const Standard_Integer lowin,
	                          const Xw_WindowQuality quality )
{
#ifdef WNT
  Handle(WNT_Window) w =
      new WNT_Window( Handle(Graphic3d_WNTGraphicDevice)::DownCast(view->Viewer()->Device()), hiwin, lowin );
  // Prevent flicker
  w->SetFlags( WDF_NOERASEBKGRND );
#else
  Handle(Xw_Window) w =
      new Xw_Window( Handle(Graphic3d_GraphicDevice)::DownCast(view->Viewer()->Device()), hiwin, lowin, quality );
#endif
  view->SetWindow( w );
}

/*!
    Magnifies 'view' based on previous view [ static ]
*/
void OCCViewer_VService::SetMagnify( const Handle(V3d_View)& view,
	                           const Standard_Integer hiwin,
	                           const Standard_Integer lowin,
	                           const Handle(V3d_View)& prevView,
	                           const Standard_Integer x1,
	                           const Standard_Integer y1,
	                           const Standard_Integer x2,
	                           const Standard_Integer y2,
                               const Xw_WindowQuality aQuality )
{
#ifdef WNT
  Handle(WNT_Window) w =
      new WNT_Window( Handle(Graphic3d_WNTGraphicDevice)::DownCast(view->Viewer()->Device()), hiwin, lowin );
#else
  Handle(Xw_Window) w =
      new Xw_Window( Handle(Graphic3d_GraphicDevice)::DownCast(view->Viewer()->Device()), hiwin, lowin, aQuality );
#endif
  view->SetMagnify( w, prevView, x1, y1, x2, y2 );
}

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
}

/*!
    Creates view 2D and maps it to the window [ static ]
*/
/*Handle(V2d_View) OCCViewer_VService::View2d( const Handle(V2d_Viewer)& aViewer,
					                   const Standard_Integer hiwin,
					                   const Standard_Integer lowin,
					                   const Xw_WindowQuality aQuality,
					                   const Standard_Boolean Update,
					                   const Quantity_NameOfColor BackColor )
{
#ifdef WNT
    Handle(WNT_GraphicDevice) GD = Handle(WNT_GraphicDevice)::DownCast(aViewer->Device());
    Handle(WNT_Window) W = new WNT_Window( GD, hiwin, lowin, BackColor );
    Handle(WNT_WDriver) D = new WNT_WDriver( W );
#else
    Handle(Xw_GraphicDevice) GD = Handle(Xw_GraphicDevice)::DownCast(aViewer->Device());
    Handle(Xw_Window) W = new Xw_Window( GD, hiwin, lowin, aQuality, BackColor );
    Handle(Xw_Driver) D = new Xw_Driver( W );
#endif
    Handle(V2d_View)  V = new V2d_View( D, aViewer );
    if ( Update )
        V->Update();
    return V;
}*/

/*!
    Creates view 2D and maps it to the window [ static ]
*/
/*Handle(V2d_View) OCCViewer_VService::dpsView2d( const Handle(V2d_Viewer)& aViewer,
					                      const Standard_Integer hiwin,
					                      const Standard_Integer lowin,
					                      const Xw_WindowQuality aQuality,
					                      const Standard_Boolean Update,
					                      const Quantity_NameOfColor BackColor )
{
#ifdef WNT
    Handle(WNT_GraphicDevice) GD = Handle(WNT_GraphicDevice)::DownCast(aViewer->Device());
    Handle(WNT_Window) W = new WNT_Window( GD, hiwin, lowin, BackColor );
    W->SetBackground( BackColor );
    Handle(WNT_WDriver) D = new WNT_WDriver( W );
#else
    Handle(Xw_GraphicDevice) GD = Handle(Xw_GraphicDevice)::DownCast(aViewer->Device());
    Handle(Xw_Window) W = new Xw_Window( GD,  hiwin, lowin, aQuality, BackColor );
    Handle(Xdps_Driver) D = new Xdps_Driver( W );
#endif
    Handle(V2d_View)  V = new V2d_View( D, aViewer );
    if ( Update )
        V->Update();
    return V;
}*/

/*!
    Creates viewer 2D [ static ]
*/
/*Handle(V2d_Viewer) OCCViewer_VService::Viewer2d( const Standard_CString aDisplay,
				                           const Standard_ExtString aName,
				                           const Standard_CString aDomain )
{
#ifdef WNT
    if ( XServiceDefault2dDevice.IsNull() )
        XServiceDefault2dDevice = new WNT_GraphicDevice();
#else
    if ( XServiceDefault2dDevice.IsNull() )
        XServiceDefault2dDevice = new Xw_GraphicDevice( aDisplay, Xw_TOM_READONLY );
#endif
    return new V2d_Viewer( XServiceDefault2dDevice, aName, aDomain );
}*/

/*!
    Creates viewer 2D [ static ]
*/
/*Handle(V2d_Viewer) OCCViewer_VService::Viewer2d( const Standard_CString aDisplay,
				                           const Handle(Graphic2d_View)& aView,
				                           const Standard_ExtString aName,
				                           const Standard_CString aDomain )
{
#ifdef WNT
    if ( XServiceDefault2dDevice.IsNull() )
        XServiceDefault2dDevice = new WNT_GraphicDevice();
#else
    if ( XServiceDefault2dDevice.IsNull() )
        XServiceDefault2dDevice = new Xw_GraphicDevice( aDisplay, Xw_TOM_READONLY );
#endif
    return new V2d_Viewer( XServiceDefault2dDevice, aView, aName, aDomain );
}*/

/*!
    Creates window driver [ static ]
*/
Handle(Aspect_WindowDriver) OCCViewer_VService::WindowDriver( const Standard_CString aDisplay,
                                                        const Standard_Integer ahiwin,
                                                        const Standard_Integer alowin,
                                                        const Quantity_NameOfColor aColor )
{
#ifdef WNT
    if ( XServiceDefault2dDevice.IsNull() )
        XServiceDefault2dDevice = new WNT_GraphicDevice();
    Handle(WNT_Window) W = new WNT_Window( XServiceDefault2dDevice, ahiwin, alowin, aColor );
    return new WNT_WDriver( W );
#else
    if ( XServiceDefault2dDevice.IsNull() )
        XServiceDefault2dDevice = new Xw_GraphicDevice( aDisplay, Xw_TOM_READONLY );
    Handle(Xw_Window) W = new Xw_Window( XServiceDefault2dDevice, ahiwin, alowin, Xw_WQ_DRAWINGQUALITY , aColor );
    return new Xw_Driver( W );
#endif
}

/*!
    Creates Xdps window driver [ static ]
    On Win32 the same as OCCViewer_VService::WindowDriver()
*/
/*
Handle(Aspect_WindowDriver) OCCViewer_VService::XdpsDriver( const Standard_CString aDisplay,
                                                      const Standard_Integer ahiwin,
                                                      const Standard_Integer alowin,
                                                      const Quantity_NameOfColor aColor )
{
#ifdef WNT
    if ( XServiceDefault2dDevice.IsNull() )
        XServiceDefault2dDevice = new WNT_GraphicDevice();
    Handle(WNT_Window) W = new WNT_Window( XServiceDefault2dDevice, ahiwin, alowin, aColor );
    return new WNT_WDriver( W );
#else
    if ( XServiceDefault2dDevice.IsNull() )
        XServiceDefault2dDevice = new Xw_GraphicDevice( aDisplay, Xw_TOM_READONLY );
    Handle(Xw_Window) W = new Xw_Window( XServiceDefault2dDevice, ahiwin, alowin,
                                         Xw_WQ_DRAWINGQUALITY, aColor );
    return new Xdps_Driver( W );
#endif
}
*/
/*!
    Creates Xw window driver [ static ]
    On Win32 the same as OCCViewer_VService::WindowDriver()
*/
Handle(Aspect_WindowDriver) OCCViewer_VService::ImageDriver( const Standard_CString aDisplay,
                                                       const Standard_Integer ahiwin,
                                                       const Standard_Integer alowin,
                                                       const Quantity_NameOfColor aColor )
{
#ifdef WNT
    if ( XServiceImageDevice.IsNull() )
        XServiceImageDevice = new WNT_GraphicDevice();
    Handle(WNT_Window) W = new WNT_Window( XServiceImageDevice, ahiwin, alowin, aColor );
    return new WNT_WDriver( W );
#else
    if ( XServiceImageDevice.IsNull() )
        XServiceImageDevice = new Xw_GraphicDevice( aDisplay, Xw_TOM_READONLY );
    Handle(Xw_Window) W = new Xw_Window( XServiceImageDevice, ahiwin, alowin,
                                         Xw_WQ_PICTUREQUALITY, aColor );
    return new Xw_Driver( W );
#endif
}
