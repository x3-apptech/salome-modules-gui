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

#include <Handle_Aspect_Window.hxx>
#include <Handle_V3d_View.hxx>
#include <Handle_V3d_Viewer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_Integer.hxx>
#include <Quantity_Length.hxx>
#include <Xw_WindowQuality.hxx>
#include <V3d_TypeOfOrientation.hxx>

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
};

#endif
