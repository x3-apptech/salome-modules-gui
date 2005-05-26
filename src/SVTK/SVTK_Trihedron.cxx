#include "SVTK_Trihedron.h"
#include "SALOME_Actor.h"

#include <vtkObjectFactory.h>
#include <vtkActorCollection.h>
#include <vtkRenderer.h>

//****************************************************************
vtkStandardNewMacro(SVTK_Trihedron);

//****************************************************************
SVTK_Trihedron
::SVTK_Trihedron()
{
}

//****************************************************************
int 
SVTK_Trihedron
::GetVisibleActorCount(vtkRenderer* theRenderer)
{
  vtkActorCollection* aCollection = theRenderer->GetActors();
  aCollection->InitTraversal();
  int aCount = 0;
  while(vtkActor* aProp = aCollection->GetNextActor()) {
    if(aProp->GetVisibility())
      if(SALOME_Actor* anActor = SALOME_Actor::SafeDownCast(aProp))
        if(!anActor->IsInfinitive()) 
          aCount++;
  }
  return aCount;
}
