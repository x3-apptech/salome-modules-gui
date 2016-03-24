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

#include "SVTK_Trihedron.h"
#include "SALOME_Actor.h"

#include "VTKViewer_Algorithm.h"

#include <vtkObjectFactory.h>
#include <vtkActorCollection.h>
#include <vtkRenderer.h>

vtkStandardNewMacro(SVTK_Trihedron);

/*!
  Constructor
*/
SVTK_Trihedron
::SVTK_Trihedron()
{
}

/*!
  \return count of visible actors
  \param theRenderer - renderer to be checked
*/
int 
SVTK_Trihedron
::GetVisibleActorCount(vtkRenderer* theRenderer)
{
  VTK::ActorCollectionCopy aCopy(theRenderer->GetActors());
  vtkActorCollection* aCollection = aCopy.GetActors();
  aCollection->InitTraversal();
  int aCount = 0;
  while(vtkActor* aProp = aCollection->GetNextActor()) {
    if(aProp->GetVisibility())
    {
      if(SALOME_Actor* anActor = SALOME_Actor::SafeDownCast(aProp)) {
        if(!anActor->IsInfinitive()) 
          aCount++;
      }
      else if ( !OwnActor( aProp ) ) {
        aCount++;
      }
    }
  }
  return aCount;
}
