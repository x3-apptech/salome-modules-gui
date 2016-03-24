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
//  File   : VTKViewer_ViewFrame.h
//  Author : Nicolas REJNERI

#ifndef VTKViewer_Functor_H
#define VTKViewer_Functor_H

#include <functional>

#include <string>

namespace VTK
{
  template<class TActor, class TArg, class TStoreArg = TArg> struct TSetFunction
  {
    typedef void (TActor::* TAction)(TArg);
    TAction myAction;
    TStoreArg myArg;
    TSetFunction(TAction theAction, TArg theArg) : myAction(theAction), myArg(theArg)
    {}
    void operator()(TActor* theActor)
    {
            (theActor->*myAction)(myArg);
    }
  };

  template<class TActor, class TArg = int> struct TSetVisibility: TSetFunction<TActor,TArg>
  {
    TSetVisibility(TArg theArg): 
            TSetFunction<TActor,TArg>(&TActor::SetVisibility,theArg)
    {}
  };
}

#endif
