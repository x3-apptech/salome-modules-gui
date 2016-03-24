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

#ifndef VTKViewer_Algorithm_H
#define VTKViewer_Algorithm_H

#include "VTKViewer.h"

#include <vtkActorCollection.h>

class vtkActor;

namespace VTK
{
  /*!
   * This object should be used to avoid problems with recurring calls of GetActors() method of the vtkRenderer class.
   *
   * Instead of the following instructions:
   *
   * vtkRenderer* aRenderer = ...;
   * vtkActorCollection* anActorCollection = aRenderer->GetActors();
   * DoSomething( anActorCollection ); // where GetActors() could be called again
   *
   * A code like the following should be used:
   *
   * vtkRenderer* aRenderer = ...;
   * vtkActorCollection* anActorCollection = aRenderer->GetActors();
   * ActorCollectionCopy aCopy( anActorCollection );
   * DoSomething( aCopy.GetActors() );
   */
  struct VTKVIEWER_EXPORT ActorCollectionCopy
  {
    vtkActorCollection* myActorCollection;

    ActorCollectionCopy( vtkActorCollection* theActorCollection );
    ~ActorCollectionCopy();

    vtkActorCollection* GetActors() const;
  };

  /*!For each actor(for ex: someActor) from \a theCollection(that can be dynamic cast to type TActor)\n
   * Call method \a theFun(someActor)
   */
    template<typename TActor, typename TFunction>
      TFunction ForEach(vtkActorCollection *theCollection, TFunction theFun)
      {
        if(theCollection){
          theCollection->InitTraversal();
          while(vtkActor *anAct = theCollection->GetNextActor())
            if(TActor *anActor = dynamic_cast<TActor*>(anAct))
              theFun(anActor);
        }
        return theFun;
      }
  
    /*!For each actor(for ex: someActor) from \a theCollection(that can be dynamic cast to type TActor and \n
     * method \a thePredicate(someActor) return true) \n
     * Call method \a theFun(someActor)
     */
    template<typename TActor, typename TPredicate, typename TFunction>
      TFunction ForEachIf(vtkActorCollection *theCollection, 
                          TPredicate thePredicate,
                          TFunction theFun)
      {
        if(theCollection){
          theCollection->InitTraversal();
          while(vtkActor *anAct = theCollection->GetNextActor())
            if(TActor *anActor = dynamic_cast<TActor*>(anAct))
              if(thePredicate(anActor))
                theFun(anActor);
        }
        return theFun;
      }
  
    /*!Find actor from collection, that can be dynamicaly cast to \a TActor, \n
     *and method \a thePredicate(someActor) return true) \n
     *\retval someActor
     */
    template<typename TActor, typename TPredicate>
      TActor* Find(vtkActorCollection *theCollection, TPredicate thePredicate)
      {
        if(theCollection){
          theCollection->InitTraversal();
          while(vtkActor *anAct = theCollection->GetNextActor())
            if(TActor *anActor = dynamic_cast<TActor*>(anAct))
              if(thePredicate(anActor))
                return anActor;
        }
        return NULL;
      }

}

#endif
