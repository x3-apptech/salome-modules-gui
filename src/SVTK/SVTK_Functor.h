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
