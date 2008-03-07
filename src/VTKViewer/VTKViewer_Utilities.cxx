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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "VTKViewer_Utilities.h"
#include "VTKViewer_Actor.h"

#include <algorithm>

// VTK Includes
#include <vtkMath.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

using namespace std;

/*!@see vtkRenderer::ResetCamera(vtkFloatingPointType bounds[6]) method*/
void 
ResetCamera(vtkRenderer* theRenderer, 
	    int theUsingZeroFocalPoint)
{  
  if(!theRenderer)
    return;

  vtkCamera* aCamera = theRenderer->GetActiveCamera();
  if(!aCamera) 
    return;

  vtkFloatingPointType aBounds[6];
  int aCount = ComputeVisiblePropBounds(theRenderer,aBounds);

  if(theUsingZeroFocalPoint || aCount){
    static vtkFloatingPointType MIN_DISTANCE = 1.0 / VTK_LARGE_FLOAT;

    vtkFloatingPointType aLength = aBounds[1]-aBounds[0];
    aLength = max((aBounds[3]-aBounds[2]),aLength);
    aLength = max((aBounds[5]-aBounds[4]),aLength);
    
    if(aLength < MIN_DISTANCE)
      return;

    vtkFloatingPointType aWidth = 
      sqrt((aBounds[1]-aBounds[0])*(aBounds[1]-aBounds[0]) +
	   (aBounds[3]-aBounds[2])*(aBounds[3]-aBounds[2]) +
	   (aBounds[5]-aBounds[4])*(aBounds[5]-aBounds[4]));
    
    if(aWidth < MIN_DISTANCE)
      return;

    vtkFloatingPointType aViewPlaneNormal[3];
    aCamera->GetViewPlaneNormal(aViewPlaneNormal);
    
    vtkFloatingPointType aCenter[3] = {0.0, 0.0, 0.0};
    if(!theUsingZeroFocalPoint){
      aCenter[0] = (aBounds[0] + aBounds[1])/2.0;
      aCenter[1] = (aBounds[2] + aBounds[3])/2.0;
      aCenter[2] = (aBounds[4] + aBounds[5])/2.0;
    }
    aCamera->SetFocalPoint(aCenter[0],aCenter[1],aCenter[2]);
    
    vtkFloatingPointType aViewAngle = aCamera->GetViewAngle();
    vtkFloatingPointType aDistance = 2.0*aWidth/tan(aViewAngle*vtkMath::Pi()/360.0);
    
    // check view-up vector against view plane normal
    vtkFloatingPointType aViewUp[3];
    aCamera->GetViewUp(aViewUp);
    if(fabs(vtkMath::Dot(aViewUp,aViewPlaneNormal)) > 0.999)
      aCamera->SetViewUp(-aViewUp[2], aViewUp[0], aViewUp[1]);
    
    // update the camera
    aCamera->SetPosition(aCenter[0]+aDistance*aViewPlaneNormal[0],
			 aCenter[1]+aDistance*aViewPlaneNormal[1],
			 aCenter[2]+aDistance*aViewPlaneNormal[2]);

    // find size of the window
    int* aWinSize = theRenderer->GetSize();
    if(aWinSize[0] < aWinSize[1]) 
      aWidth *= vtkFloatingPointType(aWinSize[1])/vtkFloatingPointType(aWinSize[0]);
    
    if(theUsingZeroFocalPoint) 
      aWidth *= sqrt(2.0);
    
    aCamera->SetParallelScale(aWidth/2.0);
  }

  ResetCameraClippingRange(theRenderer);
}

/*! Compute the bounds of the visible props*/
int
ComputeVisiblePropBounds(vtkRenderer* theRenderer, 
			 vtkFloatingPointType theBounds[6])
{
  int aCount = 0;
  
  theBounds[0] = theBounds[2] = theBounds[4] = VTK_LARGE_FLOAT;
  theBounds[1] = theBounds[3] = theBounds[5] = -VTK_LARGE_FLOAT;
  
  // loop through all props
  vtkActorCollection* aCollection = theRenderer->GetActors();
  aCollection->InitTraversal();
  while (vtkActor* aProp = aCollection->GetNextActor()) {
    // if it's invisible, or has no geometry, we can skip the rest 
    if(aProp->GetVisibility() && aProp->GetMapper()){
      if(VTKViewer_Actor* anActor = VTKViewer_Actor::SafeDownCast(aProp))
        if(anActor->IsInfinitive())
	  continue;
	
      vtkFloatingPointType *aBounds = aProp->GetBounds();
      static vtkFloatingPointType MAX_DISTANCE = 0.9*VTK_LARGE_FLOAT;
      // make sure we haven't got bogus bounds
      if ( aBounds != NULL &&
	   aBounds[0] > -MAX_DISTANCE && aBounds[1] < MAX_DISTANCE &&
	   aBounds[2] > -MAX_DISTANCE && aBounds[3] < MAX_DISTANCE &&
	   aBounds[4] > -MAX_DISTANCE && aBounds[5] < MAX_DISTANCE )
      {
	aCount++;

	theBounds[0] = min(aBounds[0],theBounds[0]);
	theBounds[2] = min(aBounds[2],theBounds[2]);
	theBounds[4] = min(aBounds[4],theBounds[4]);

	theBounds[1] = max(aBounds[1],theBounds[1]);
	theBounds[3] = max(aBounds[3],theBounds[3]);
	theBounds[5] = max(aBounds[5],theBounds[5]);

      }//not bogus
    }
  }
  return aCount;
}

/*!@see vtkRenderer::ResetCameraClippingRange(vtkFloatingPointType bounds[6]) method*/
void
ResetCameraClippingRange(vtkRenderer* theRenderer)
{
  if(!theRenderer || !theRenderer->VisibleActorCount()) return;
  
  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  if( anActiveCamera == NULL ){
    return;
  }
  
  // Find the plane equation for the camera view plane
  vtkFloatingPointType vn[3];
  anActiveCamera->GetViewPlaneNormal(vn);
  vtkFloatingPointType  position[3];
  anActiveCamera->GetPosition(position);
  
  vtkFloatingPointType bounds[6];
  theRenderer->ComputeVisiblePropBounds(bounds);
  
  vtkFloatingPointType center[3];
  center[0] = (bounds[0] + bounds[1])/2.0;
  center[1] = (bounds[2] + bounds[3])/2.0;
  center[2] = (bounds[4] + bounds[5])/2.0;
  
  vtkFloatingPointType width = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
    (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
    (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));
  
  vtkFloatingPointType distance = sqrt((position[0]-center[0])*(position[0]-center[0]) +
       (position[1]-center[1])*(position[1]-center[1]) +
       (position[2]-center[2])*(position[2]-center[2]));
  
  vtkFloatingPointType range[2] = {distance - width/2.0, distance + width/2.0};
  
  // Do not let the range behind the camera throw off the calculation.
  if (range[0] < 0.0) range[0] = 0.0;
  
  anActiveCamera->SetClippingRange( range );
}

/*!Compute trihedron size.*/
bool
ComputeTrihedronSize( vtkRenderer* theRenderer,
		      vtkFloatingPointType& theNewSize,
		      const vtkFloatingPointType theSize, 
		      const vtkFloatingPointType theSizeInPercents )
{
  // calculating diagonal of visible props of the renderer
  vtkFloatingPointType bnd[ 6 ];
  if ( ComputeVisiblePropBounds( theRenderer, bnd ) == 0 )
  {
    bnd[ 1 ] = bnd[ 3 ] = bnd[ 5 ] = 100;
    bnd[ 0 ] = bnd[ 2 ] = bnd[ 4 ] = 0;
  }
  vtkFloatingPointType aLength = 0;

  aLength = bnd[ 1 ]-bnd[ 0 ];
  aLength = max( ( bnd[ 3 ] - bnd[ 2 ] ),aLength );
  aLength = max( ( bnd[ 5 ] - bnd[ 4 ] ),aLength );

  static vtkFloatingPointType EPS_SIZE = 5.0E-3;
  theNewSize = aLength * theSizeInPercents / 100.0;

  // if the new trihedron size have sufficient difference, then apply the value
  return fabs( theNewSize - theSize) > theSize * EPS_SIZE ||
         fabs( theNewSize-theSize ) > theNewSize * EPS_SIZE;
}

bool IsBBEmpty(vtkRenderer* theRenderer)
{
  if(!theRenderer)
    return false;

  vtkFloatingPointType aNewBndBox[6];
  aNewBndBox[ 0 ] = aNewBndBox[ 2 ] = aNewBndBox[ 4 ] = VTK_LARGE_FLOAT;
  aNewBndBox[ 1 ] = aNewBndBox[ 3 ] = aNewBndBox[ 5 ] = -VTK_LARGE_FLOAT;
  
  // iterate through displayed objects and set size if necessary
  vtkActorCollection* anActors = theRenderer->GetActors();
  anActors->InitTraversal();
  bool isAny = false;
  while(vtkActor* anAct = anActors->GetNextActor())
    //if(SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>(anAct))
    if(VTKViewer_Actor* anActor = VTKViewer_Actor::SafeDownCast(anAct))
      if(anActor->GetVisibility() && !anActor->IsInfinitive())
      {
	vtkFloatingPointType *aBounds = anActor->GetBounds();
	if(aBounds[0] > -VTK_LARGE_FLOAT && aBounds[1] < VTK_LARGE_FLOAT &&
	   aBounds[2] > -VTK_LARGE_FLOAT && aBounds[3] < VTK_LARGE_FLOAT &&
	   aBounds[4] > -VTK_LARGE_FLOAT && aBounds[5] < VTK_LARGE_FLOAT)
	  isAny = true;
      }
  
  return !isAny;
}

bool ComputeBBCenter(vtkRenderer* theRenderer, vtkFloatingPointType theCenter[3])
{  
  theCenter[0] = theCenter[1] = theCenter[2] = 0.0;
  
  if(!theRenderer)
    return false;

  vtkFloatingPointType aNewBndBox[6];
  aNewBndBox[ 0 ] = aNewBndBox[ 2 ] = aNewBndBox[ 4 ] = VTK_LARGE_FLOAT;
  aNewBndBox[ 1 ] = aNewBndBox[ 3 ] = aNewBndBox[ 5 ] = -VTK_LARGE_FLOAT;

  // iterate through displayed objects and set size if necessary
  vtkActorCollection* anActors = theRenderer->GetActors();
  anActors->InitTraversal();
  bool isAny = false;
  while(vtkActor* anAct = anActors->GetNextActor())
  {
    //if(SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>(anAct))
    if(VTKViewer_Actor* anActor = VTKViewer_Actor::SafeDownCast(anAct))
    {
      if(anActor->GetVisibility() && !anActor->IsInfinitive())
      {
	vtkFloatingPointType *aBounds = anActor->GetBounds();
	if(aBounds[0] > -VTK_LARGE_FLOAT && aBounds[1] < VTK_LARGE_FLOAT &&
	   aBounds[2] > -VTK_LARGE_FLOAT && aBounds[3] < VTK_LARGE_FLOAT &&
	   aBounds[4] > -VTK_LARGE_FLOAT && aBounds[5] < VTK_LARGE_FLOAT)
	{
	  for(int i = 0; i < 5; i = i + 2){
	    if(aBounds[i] < aNewBndBox[i]) 
	      aNewBndBox[i] = aBounds[i];
	    if(aBounds[i+1] > aNewBndBox[i+1]) 
	      aNewBndBox[i+1] = aBounds[i+1];
	  }
	  isAny = true;
	}
      }
    }
  }
  
  if ( !isAny )
  {
    // null bounding box => the center is (0,0,0)
    return true;
  }

  if(aNewBndBox[0] > -VTK_LARGE_FLOAT && aNewBndBox[1] < VTK_LARGE_FLOAT &&
     aNewBndBox[2] > -VTK_LARGE_FLOAT && aNewBndBox[3] < VTK_LARGE_FLOAT &&
     aNewBndBox[4] > -VTK_LARGE_FLOAT && aNewBndBox[5] < VTK_LARGE_FLOAT)
  {
    static vtkFloatingPointType MIN_DISTANCE = 1.0 / VTK_LARGE_FLOAT;
    
    vtkFloatingPointType aLength = aNewBndBox[1]-aNewBndBox[0];
    aLength = max((aNewBndBox[3]-aNewBndBox[2]),aLength);
    aLength = max((aNewBndBox[5]-aNewBndBox[4]),aLength);
    
    if(aLength < MIN_DISTANCE)
      return false;

    vtkFloatingPointType aWidth = 
      sqrt((aNewBndBox[1]-aNewBndBox[0])*(aNewBndBox[1]-aNewBndBox[0]) +
	   (aNewBndBox[3]-aNewBndBox[2])*(aNewBndBox[3]-aNewBndBox[2]) +
	   (aNewBndBox[5]-aNewBndBox[4])*(aNewBndBox[5]-aNewBndBox[4]));
    
    if(aWidth < MIN_DISTANCE)
      return false;

    theCenter[0] = (aNewBndBox[0] + aNewBndBox[1])/2.0;
    theCenter[1] = (aNewBndBox[2] + aNewBndBox[3])/2.0;
    theCenter[2] = (aNewBndBox[4] + aNewBndBox[5])/2.0;
    return true;
  }

  return false;

  /*
  vtkFloatingPointType aBounds[6];
  int aCount = ComputeVisiblePropBounds(theRenderer,aBounds);
  printf("aNewBndBox[0] = %f, aNewBndBox[1] = %f,\naNewBndBox[2] = %f, aNewBndBox[3] = %f,\naNewBndBox[4] = %f, aNewBndBox[5] = %f\n",
	   aBounds[0],aBounds[1],aBounds[2],aBounds[3],aBounds[4],aBounds[5]);
  printf("aCount = %d\n",aCount);

  if(aCount){
    static vtkFloatingPointType MIN_DISTANCE = 1.0 / VTK_LARGE_FLOAT;

    vtkFloatingPointType aLength = aBounds[1]-aBounds[0];
    aLength = max((aBounds[3]-aBounds[2]),aLength);
    aLength = max((aBounds[5]-aBounds[4]),aLength);
    
    if(aLength < MIN_DISTANCE)
      return false;

    vtkFloatingPointType aWidth = 
      sqrt((aBounds[1]-aBounds[0])*(aBounds[1]-aBounds[0]) +
	   (aBounds[3]-aBounds[2])*(aBounds[3]-aBounds[2]) +
	   (aBounds[5]-aBounds[4])*(aBounds[5]-aBounds[4]));
    
    if(aWidth < MIN_DISTANCE)
      return false;

    theCenter[0] = (aBounds[0] + aBounds[1])/2.0;
    theCenter[1] = (aBounds[2] + aBounds[3])/2.0;
    theCenter[2] = (aBounds[4] + aBounds[5])/2.0;
    return true;
  }
  return false;*/
}
