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

#ifndef SVTK_Functor_H
#define SVTK_Functor_H

#include <functional>

#include <string>

#include <VTKViewer_Functor.h>

#include <SALOME_InteractiveObject.hxx>
#include <SALOME_ListIO.hxx>
/*!
  \file SVTK_Functor.h
  This file contains numbers of functors that allows user to perform corresponding operations with existing presentations.
  Combination with algorithms it gives powerful, flexible and simple to extend way to introduce new type of operation.
*/

namespace SVTK
{
  using namespace VTK;

  //! This functor check, if the actor have pointed entry
  template<class TActor> 
  struct TIsSameEntry
  {
    std::string myEntry;
    //! To construct the functor
    TIsSameEntry(const char* theEntry): 
      myEntry(theEntry) 
    {}
    //! To calculate the functor
    bool operator()(TActor* theActor)
    {
      if ( theActor->hasIO() )
      {
        Handle(SALOME_InteractiveObject) anIO = theActor->getIO();
        if ( anIO->hasEntry() )
          return myEntry == anIO->getEntry();
      }
      return false;
    }
  };


  //----------------------------------------------------------------
  //! This functor check, if the actor point to the same #SALOME_InteractiveObject
  template<class TActor> 
  struct TIsSameIObject
  {
    Handle(SALOME_InteractiveObject) myIObject;
    //! To construct the functor
    TIsSameIObject(const Handle(SALOME_InteractiveObject)& theIObject):
      myIObject(theIObject)
    {}
    //! To calculate the functor
    bool operator()(TActor* theActor)
    {
      if(theActor->hasIO())
      {
        Handle(SALOME_InteractiveObject) anIO = theActor->getIO();
        return myIObject->isSame(anIO);
      }
      return false;
    }
  };


  //----------------------------------------------------------------
  /*!
    This highlight every input actor
  */
  template<class TActor> 
  struct THighlight
  {
    bool myIsHighlight;
    //! To construct the functor
    THighlight(bool theIsHighlight):
      myIsHighlight( theIsHighlight ) 
    {}
    //! To calculate the functor
    void operator()(TActor* theActor) 
    {
      if(theActor->GetVisibility() && theActor->GetMapper())
        theActor->highlight( myIsHighlight );
    }
  };


  //----------------------------------------------------------------
  /*!
    This collect visible IO in list
  */
  template<class TActor> 
    struct TCollectIfVisible
    {
      SALOME_ListIO& myList;
      //! To construct the functor
      TCollectIfVisible (SALOME_ListIO& theList) : myList(theList)
      {}
      //! To calculate the functor
      void operator()(TActor* theActor) 
      {
	if(theActor->GetVisibility() && theActor->hasIO())
	  myList.Append( theActor->getIO() );
      }
    };
}


#endif
