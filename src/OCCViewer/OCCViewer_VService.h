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

#ifndef OCCVIEWER_VSERVICE_H
#define OCCVIEWER_VSERVICE_H

#ifndef _Handle_V3d_View_HeaderFile
#include <Handle_V3d_View.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Xw_WindowQuality_HeaderFile
#include <Xw_WindowQuality.hxx>
#endif
#ifndef _Handle_V3d_Viewer_HeaderFile
#include <Handle_V3d_Viewer.hxx>
#endif
#ifndef _Standard_CString_HeaderFile
#include <Standard_CString.hxx>
#endif
#ifndef _Standard_ExtString_HeaderFile
#include <Standard_ExtString.hxx>
#endif
#ifndef _Quantity_Length_HeaderFile
#include <Quantity_Length.hxx>
#endif
#ifndef _Quantity_NameOfColor_HeaderFile
#include <Quantity_NameOfColor.hxx>
#endif
#ifndef _V3d_TypeOfOrientation_HeaderFile
#include <V3d_TypeOfOrientation.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _Handle_Graphic2d_View_HeaderFile
#include <Handle_Graphic2d_View.hxx>
#endif
#ifndef _Handle_Aspect_WindowDriver_HeaderFile
#include <Handle_Aspect_WindowDriver.hxx>
#endif
#ifndef _Handle_Aspect_Window_HeaderFile
#include <Handle_Aspect_Window.hxx>
#endif

class V3d_View;
class V2d_View;
class V3d_Viewer;
class V2d_Viewer;
class Graphic2d_View;
class Aspect_WindowDriver;

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class Standard_EXPORT OCCViewer_VService
{
public:
    inline void* operator new(size_t,void* anAddress)
    {
        return anAddress;
    }
    inline void* operator new(size_t size)
    {
        return Standard::Allocate(size);
    }
    inline void  operator delete(void *anAddress)
    {
        if ( anAddress ) Standard::Free((Standard_Address&)anAddress);
    }

    // STATIC METHODS
    static Handle(Aspect_Window) 
                    CreateWindow( const Handle(V3d_View)& view,
                                  const Standard_Integer hiwin,
                                  const Standard_Integer lowin,
                                  const Xw_WindowQuality quality );

    static void     SetWindow( const Handle(  V3d_View )& view,
                               const Standard_Integer hiwin,
                               const Standard_Integer lowin,
                               const Xw_WindowQuality quality = Xw_WQ_3DQUALITY );

    static void     SetMagnify( const Handle( V3d_View)& view,
                                const Standard_Integer hiwin,
                                const Standard_Integer lowin,
                                const Handle( V3d_View)& aPreviousView,
                                const Standard_Integer x1,
                                const Standard_Integer y1,
                                const Standard_Integer x2,
                                const Standard_Integer y2,
                                const Xw_WindowQuality quality = Xw_WQ_3DQUALITY );
    static Handle_V3d_Viewer
                    Viewer3d( const Standard_CString display,
                              const Standard_ExtString name,
                              const Standard_CString domain = "",
                              const Quantity_Length ViewSize = 1000.0,
                              const V3d_TypeOfOrientation ViewProj = V3d_XposYnegZpos,
                              const Standard_Boolean ComputedMode = Standard_True,
                              const Standard_Boolean DefaultComputedMode = Standard_True );

    static Handle_Aspect_WindowDriver
                    WindowDriver( const Standard_CString display,
                                  const Standard_Integer hiwin,
                                  const Standard_Integer lowin,
                                  const Quantity_NameOfColor color = Quantity_NOC_GRAY69 );
    /*static Handle_Aspect_WindowDriver
                    XdpsDriver( const Standard_CString display,
                                const Standard_Integer hiwin,
                                const Standard_Integer lowin,
                                const Quantity_NameOfColor color = Quantity_NOC_GRAY69 );
    */
    static Handle_Aspect_WindowDriver
                    ImageDriver( const Standard_CString display,
                                 const Standard_Integer hiwin,
                                 const Standard_Integer lowin,
                                 const Quantity_NameOfColor color = Quantity_NOC_GRAY69 );

};

#endif
