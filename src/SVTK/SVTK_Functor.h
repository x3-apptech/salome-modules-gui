// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
#ifndef SVTK_Functor_H
#define SVTK_Functor_H

#include <functional>

#include <string>

#include <VTKViewer_Functor.h>

#include "SALOME_InteractiveObject.hxx"

namespace VTK
{
  template<class TActor> 
  struct TIsSameEntry
  {
    std::string myEntry;
    TIsSameEntry(const char* theEntry): myEntry(theEntry) {}
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
  template<class TActor> 
  struct TIsSameIObject
  {
    Handle(SALOME_InteractiveObject) myIObject;
    TIsSameIObject(const Handle(SALOME_InteractiveObject)& theIObject):
      myIObject(theIObject)
    {}
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
  template<class TActor> 
  struct THighlight
  {
    bool myIsHighlight;
    THighlight(bool theIsHighlight): myIsHighlight( theIsHighlight ) {}
    void operator()(TActor* theActor) 
    {
      if(theActor->GetVisibility() && theActor->GetMapper())
	theActor->highlight( myIsHighlight );
    }
  };

}


#endif
