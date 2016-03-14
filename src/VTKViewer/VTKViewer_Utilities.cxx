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

#include "VTKViewer_Utilities.h"
#include "VTKViewer_Actor.h"
#include "VTKViewer_Algorithm.h"

#include <algorithm>

// VTK Includes
#include <vtkMath.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

/*!@see vtkRenderer::ResetCamera(double bounds[6]) method*/
void 
ResetCamera(vtkRenderer* theRenderer, 
            int theUsingZeroFocalPoint)
{  
  if(!theRenderer)
    return;

  vtkCamera* aCamera = theRenderer->GetActiveCamera();
  if(!aCamera) 
    return;

  double aBounds[6];
  int aCount = ComputeVisiblePropBounds(theRenderer,aBounds);

  if(theUsingZeroFocalPoint || aCount){
    static double MIN_DISTANCE = 1.0 / VTK_FLOAT_MAX;

    double aLength = aBounds[1]-aBounds[0];
    aLength = std::max((aBounds[3]-aBounds[2]),aLength);
    aLength = std::max((aBounds[5]-aBounds[4]),aLength);
    
    if(aLength < MIN_DISTANCE)
      return;

    double aWidth = 
      sqrt((aBounds[1]-aBounds[0])*(aBounds[1]-aBounds[0]) +
           (aBounds[3]-aBounds[2])*(aBounds[3]-aBounds[2]) +
           (aBounds[5]-aBounds[4])*(aBounds[5]-aBounds[4]));
    
    if(aWidth < MIN_DISTANCE)
      return;

    double aViewPlaneNormal[3];
    aCamera->GetViewPlaneNormal(aViewPlaneNormal);
    
    double aCenter[3] = {0.0, 0.0, 0.0};
    if(!theUsingZeroFocalPoint){
      aCenter[0] = (aBounds[0] + aBounds[1])/2.0;
      aCenter[1] = (aBounds[2] + aBounds[3])/2.0;
      aCenter[2] = (aBounds[4] + aBounds[5])/2.0;
    }
    aCamera->SetFocalPoint(aCenter[0],aCenter[1],aCenter[2]);
    
    double aViewAngle = aCamera->GetViewAngle();
    double aDistance = 2.0*aWidth/tan(aViewAngle*vtkMath::Pi()/360.0);
    
    // check view-up vector against view plane normal
    double aViewUp[3];
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
      aWidth *= double(aWinSize[1])/double(aWinSize[0]);
    
    if(theUsingZeroFocalPoint) 
      aWidth *= sqrt(2.0);
    
    aCamera->SetParallelScale(aWidth/2.0);
  }

  ResetCameraClippingRange(theRenderer);
}

/*! Compute the bounds of the visible props*/
int
ComputeVisiblePropBounds(vtkRenderer* theRenderer, 
                         double theBounds[6])
{
  VTK::ActorCollectionCopy aCopy(theRenderer->GetActors());
  vtkActorCollection* aCollection = aCopy.GetActors();
  return ComputeBounds( aCollection, theBounds );
}

/*! Compute the bounds of actors*/
int
ComputeBounds(vtkActorCollection* theCollection, double theBounds[6])
{
  int aCount = 0;

  theBounds[0] = theBounds[2] = theBounds[4] = VTK_FLOAT_MAX;
  theBounds[1] = theBounds[3] = theBounds[5] = -VTK_FLOAT_MAX;

  // loop through all props
  theCollection->InitTraversal();
  while (vtkActor* aProp = theCollection->GetNextActor()) {
    // if it's invisible, or has no geometry, we can skip the rest
    if(aProp->GetVisibility() && aProp->GetMapper() && vtkMath::AreBoundsInitialized(aProp->GetBounds())){
      if(VTKViewer_Actor* anActor = VTKViewer_Actor::SafeDownCast(aProp))
        if(anActor->IsInfinitive())
          continue;
      double *aBounds = aProp->GetBounds();
      static double MIN_DISTANCE = 1./VTK_FLOAT_MAX;
      static double MAX_DISTANCE = 0.9*VTK_FLOAT_MAX;

      if(fabs(aBounds[1] - aBounds[0]) < MIN_DISTANCE) {
        aBounds[0]-=0.001;
        aBounds[1]+=0.001;
      }

      if(fabs(aBounds[3] - aBounds[2]) < MIN_DISTANCE) {
        aBounds[2]-=0.001;
        aBounds[3]+=0.001;
      }

      if(fabs(aBounds[5] - aBounds[4]) < MIN_DISTANCE) {
        aBounds[4]-=0.001;
        aBounds[5]+=0.001;
      }
      
      // make sure we haven't got bogus bounds
      if ( aBounds != NULL &&
           aBounds[0] > -MAX_DISTANCE && aBounds[1] < MAX_DISTANCE &&
           aBounds[2] > -MAX_DISTANCE && aBounds[3] < MAX_DISTANCE &&
           aBounds[4] > -MAX_DISTANCE && aBounds[5] < MAX_DISTANCE)           
      {
        aCount++;

        theBounds[0] = std::min(aBounds[0],theBounds[0]);
        theBounds[2] = std::min(aBounds[2],theBounds[2]);
        theBounds[4] = std::min(aBounds[4],theBounds[4]);

        theBounds[1] = std::max(aBounds[1],theBounds[1]);
        theBounds[3] = std::max(aBounds[3],theBounds[3]);
        theBounds[5] = std::max(aBounds[5],theBounds[5]);

      }//not bogus
    }
  }
  return aCount;
}

/*!@see vtkRenderer::ResetCameraClippingRange(double bounds[6]) method*/
void
ResetCameraClippingRange(vtkRenderer* theRenderer)
{
  if(!theRenderer || !theRenderer->VisibleActorCount()) return;
  
  vtkCamera* anActiveCamera = theRenderer->GetActiveCamera();
  if( anActiveCamera == NULL ){
    return;
  }
  
  // Find the plane equation for the camera view plane
  double vn[3];
  anActiveCamera->GetViewPlaneNormal(vn);
  double  position[3];
  anActiveCamera->GetPosition(position);
  
  double bounds[6];
  //theRenderer->ComputeVisiblePropBounds(bounds);
  ComputeVisiblePropBounds(theRenderer, bounds);
  
  double center[3];
  center[0] = (bounds[0] + bounds[1])/2.0;
  center[1] = (bounds[2] + bounds[3])/2.0;
  center[2] = (bounds[4] + bounds[5])/2.0;
  
  double width = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
    (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
    (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));
  
  double distance = sqrt((position[0]-center[0])*(position[0]-center[0]) +
       (position[1]-center[1])*(position[1]-center[1]) +
       (position[2]-center[2])*(position[2]-center[2]));
  
  double range[2] = {distance - width/2.0, distance + width/2.0};
  
  // Do not let the range behind the camera throw off the calculation.
  if (range[0] < 0.0) range[0] = 0.0;
  
  anActiveCamera->SetClippingRange( range );
}

/*!Compute trihedron size.*/
bool
ComputeTrihedronSize( vtkRenderer* theRenderer,
                      double& theNewSize,
                      const double theSize, 
                      const double theSizeInPercents )
{
  // calculating diagonal of visible props of the renderer
  double bnd[ 6 ];
  if ( ComputeVisiblePropBounds( theRenderer, bnd ) == 0 )
  {
    bnd[ 1 ] = bnd[ 3 ] = bnd[ 5 ] = 100;
    bnd[ 0 ] = bnd[ 2 ] = bnd[ 4 ] = 0;
  }
  double aLength = 0;

  aLength = bnd[ 1 ]-bnd[ 0 ];
  aLength = std::max( ( bnd[ 3 ] - bnd[ 2 ] ),aLength );
  aLength = std::max( ( bnd[ 5 ] - bnd[ 4 ] ),aLength );

  static double EPS_SIZE = 5.0E-3;
  theNewSize = aLength * theSizeInPercents / 100.0;

  // if the new trihedron size have sufficient difference, then apply the value
  return fabs( theNewSize - theSize) > theSize * EPS_SIZE ||
         fabs( theNewSize-theSize ) > theNewSize * EPS_SIZE;
}

bool IsBBEmpty(vtkRenderer* theRenderer)
{
  if(!theRenderer)
    return false;

  double aNewBndBox[6];
  aNewBndBox[ 0 ] = aNewBndBox[ 2 ] = aNewBndBox[ 4 ] = VTK_FLOAT_MAX;
  aNewBndBox[ 1 ] = aNewBndBox[ 3 ] = aNewBndBox[ 5 ] = -VTK_FLOAT_MAX;
  
  // iterate through displayed objects and set size if necessary
  VTK::ActorCollectionCopy aCopy(theRenderer->GetActors());
  vtkActorCollection* anActors = aCopy.GetActors();
  anActors->InitTraversal();
  bool isAny = false;
  while(vtkActor* anAct = anActors->GetNextActor())
    //if(SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>(anAct))
    if(VTKViewer_Actor* anActor = VTKViewer_Actor::SafeDownCast(anAct))
      if(anActor->GetVisibility() && !anActor->IsInfinitive())
      {
        double *aBounds = anActor->GetBounds();
        if(aBounds[0] > -VTK_FLOAT_MAX && aBounds[1] < VTK_FLOAT_MAX &&
           aBounds[2] > -VTK_FLOAT_MAX && aBounds[3] < VTK_FLOAT_MAX &&
           aBounds[4] > -VTK_FLOAT_MAX && aBounds[5] < VTK_FLOAT_MAX)
          isAny = true;
      }
  
  return !isAny;
}

/*!
  Check that the given bounding box is valid, i.e each min bound < each max bound
*/

bool isBoundValid(double* theBounds) {
  if(theBounds[0] > theBounds[1] ||
     theBounds[2] > theBounds[3] ||
     theBounds[4] > theBounds[5])
    return false;
  else 
    return true;
}

bool ComputeBBCenter(vtkRenderer* theRenderer, double theCenter[3])
{  
  theCenter[0] = theCenter[1] = theCenter[2] = 0.0;
  
  if(!theRenderer)
    return false;

  double aNewBndBox[6];
  aNewBndBox[ 0 ] = aNewBndBox[ 2 ] = aNewBndBox[ 4 ] = VTK_FLOAT_MAX;
  aNewBndBox[ 1 ] = aNewBndBox[ 3 ] = aNewBndBox[ 5 ] = -VTK_FLOAT_MAX;

  // iterate through displayed objects and set size if necessary
  VTK::ActorCollectionCopy aCopy(theRenderer->GetActors());
  vtkActorCollection* anActors = aCopy.GetActors();
  anActors->InitTraversal();
  bool isAny = false;
  while(vtkActor* anAct = anActors->GetNextActor())
  {
    //if(SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>(anAct))
    if(VTKViewer_Actor* anActor = VTKViewer_Actor::SafeDownCast(anAct))
    {
      if(anActor->GetVisibility() && !anActor->IsInfinitive())
      {
        double *aBounds = anActor->GetBounds();
	
	//Ignore invalid bounds
	if(!isBoundValid(aBounds)) continue;

        if(aBounds[0] > -VTK_FLOAT_MAX && aBounds[1] < VTK_FLOAT_MAX &&
           aBounds[2] > -VTK_FLOAT_MAX && aBounds[3] < VTK_FLOAT_MAX &&
           aBounds[4] > -VTK_FLOAT_MAX && aBounds[5] < VTK_FLOAT_MAX)
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

  if(aNewBndBox[0] > -VTK_FLOAT_MAX && aNewBndBox[1] < VTK_FLOAT_MAX &&
     aNewBndBox[2] > -VTK_FLOAT_MAX && aNewBndBox[3] < VTK_FLOAT_MAX &&
     aNewBndBox[4] > -VTK_FLOAT_MAX && aNewBndBox[5] < VTK_FLOAT_MAX)
  {
    static double MIN_DISTANCE = 1.0 / VTK_FLOAT_MAX;
    
    double aLength = aNewBndBox[1]-aNewBndBox[0];
    aLength = std::max((aNewBndBox[3]-aNewBndBox[2]),aLength);
    aLength = std::max((aNewBndBox[5]-aNewBndBox[4]),aLength);
    
    if(aLength < MIN_DISTANCE)
      return false;

    double aWidth = 
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
  double aBounds[6];
  int aCount = ComputeVisiblePropBounds(theRenderer,aBounds);
  printf("aNewBndBox[0] = %f, aNewBndBox[1] = %f,\naNewBndBox[2] = %f, aNewBndBox[3] = %f,\naNewBndBox[4] = %f, aNewBndBox[5] = %f\n",
           aBounds[0],aBounds[1],aBounds[2],aBounds[3],aBounds[4],aBounds[5]);
  printf("aCount = %d\n",aCount);

  if(aCount){
    static double MIN_DISTANCE = 1.0 / VTK_FLOAT_MAX;

    double aLength = aBounds[1]-aBounds[0];
    aLength = max((aBounds[3]-aBounds[2]),aLength);
    aLength = max((aBounds[5]-aBounds[4]),aLength);
    
    if(aLength < MIN_DISTANCE)
      return false;

    double aWidth = 
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
