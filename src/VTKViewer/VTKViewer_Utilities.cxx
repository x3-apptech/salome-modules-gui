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
#include "VTKViewer_Actor.h"

// VTK Includes
#include <vtkMath.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "VTKViewer_Utilities.h"


/*!@see vtkRenderer::ResetCamera(float bounds[6]) method*/
void ResetCamera(vtkRenderer* theRenderer, int theUsingZeroFocalPoint)
{  
  if(!theRenderer)
    return;

  vtkCamera* aCamera = theRenderer->GetActiveCamera();
  if(!aCamera) 
    return;

  float aBounds[6];
  int aCount = ComputeVisiblePropBounds(theRenderer,aBounds);

  if(theUsingZeroFocalPoint || aCount){
    static float MIN_DISTANCE = 1.0 / VTK_LARGE_FLOAT;

    float aLength = aBounds[1]-aBounds[0];
    aLength = max((aBounds[3]-aBounds[2]),aLength);
    aLength = max((aBounds[5]-aBounds[4]),aLength);
    
    if(aLength < MIN_DISTANCE)
      return;

    float aWidth = 
      sqrt((aBounds[1]-aBounds[0])*(aBounds[1]-aBounds[0]) +
	   (aBounds[3]-aBounds[2])*(aBounds[3]-aBounds[2]) +
	   (aBounds[5]-aBounds[4])*(aBounds[5]-aBounds[4]));
    
    if(aWidth < MIN_DISTANCE)
      return;

    double aViewPlaneNormal[3];
    aCamera->GetViewPlaneNormal(aViewPlaneNormal);
    
    float aCenter[3] = {0.0, 0.0, 0.0};
    if(!theUsingZeroFocalPoint){
      aCenter[0] = (aBounds[0] + aBounds[1])/2.0;
      aCenter[1] = (aBounds[2] + aBounds[3])/2.0;
      aCenter[2] = (aBounds[4] + aBounds[5])/2.0;
    }
    aCamera->SetFocalPoint(aCenter[0],aCenter[1],aCenter[2]);
    
    double aViewAngle = aCamera->GetViewAngle();
    float aDistance = 2.0*aWidth/tan(aViewAngle*vtkMath::Pi()/360.0);
    
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
      aWidth *= float(aWinSize[1])/float(aWinSize[0]);
    
    if(theUsingZeroFocalPoint) 
      aWidth *= sqrt(2.0);
    
    aCamera->SetParallelScale(aWidth/2.0);
  }

  ResetCameraClippingRange(theRenderer);
}

/*! Compute the bounds of the visible props*/
int ComputeVisiblePropBounds(vtkRenderer* theRenderer, float theBounds[6])
{
  float      *bounds;
  int        aCount=0;
  
  theBounds[0] = theBounds[2] = theBounds[4] = VTK_LARGE_FLOAT;
  theBounds[1] = theBounds[3] = theBounds[5] = -VTK_LARGE_FLOAT;
  
  // loop through all props
  vtkActorCollection* aCollection = theRenderer->GetActors();
  aCollection->InitTraversal();
  while (vtkActor* prop = aCollection->GetNextActor()) {
    // if it's invisible, or has no geometry, we can skip the rest 
    if ( prop->GetVisibility() )
    {
      if(VTKViewer_Actor* anActor = VTKViewer_Actor::SafeDownCast(prop))
        if(anActor->IsInfinitive()) continue;
        bounds = prop->GetBounds();
        // make sure we haven't got bogus bounds
        if ( bounds != NULL &&
          bounds[0] > -VTK_LARGE_FLOAT && bounds[1] < VTK_LARGE_FLOAT &&
          bounds[2] > -VTK_LARGE_FLOAT && bounds[3] < VTK_LARGE_FLOAT &&
          bounds[4] > -VTK_LARGE_FLOAT && bounds[5] < VTK_LARGE_FLOAT )
        {
          aCount++;
          
          if (bounds[0] < theBounds[0])
          {
            theBounds[0] = bounds[0]; 
          }
          if (bounds[1] > theBounds[1])
          {
            theBounds[1] = bounds[1]; 
          }
          if (bounds[2] < theBounds[2])
          {
            theBounds[2] = bounds[2]; 
          }
          if (bounds[3] > theBounds[3])
          {
            theBounds[3] = bounds[3]; 
          }
          if (bounds[4] < theBounds[4])
          {
            theBounds[4] = bounds[4]; 
          }
          if (bounds[5] > theBounds[5])
          {
            theBounds[5] = bounds[5]; 
          }
        }//not bogus
    }
  }
  return aCount;
}

/*!@see vtkRenderer::ResetCameraClippingRange(float bounds[6]) method*/
void ResetCameraClippingRange(vtkRenderer* theRenderer)
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
  
  float bounds[6];
  theRenderer->ComputeVisiblePropBounds(bounds);
  
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
  
  float range[2] = {distance - width/2.0, distance + width/2.0};
  
  // Do not let the range behind the camera throw off the calculation.
  if (range[0] < 0.0) range[0] = 0.0;
  
  anActiveCamera->SetClippingRange( range );
}

/*!Compute trihedron size.*/
bool ComputeTrihedronSize( vtkRenderer* theRenderer,double& theNewSize,
			   const double theSize, const float theSizeInPercents )
{
  // calculating diagonal of visible props of the renderer
  float bnd[ 6 ];
  if ( ComputeVisiblePropBounds( theRenderer, bnd ) == 0 )
  {
    bnd[ 1 ] = bnd[ 3 ] = bnd[ 5 ] = 100;
    bnd[ 0 ] = bnd[ 2 ] = bnd[ 4 ] = 0;
  }
  float aLength = 0;

  aLength = bnd[ 1 ]-bnd[ 0 ];
  aLength = max( ( bnd[ 3 ] - bnd[ 2 ] ),aLength );
  aLength = max( ( bnd[ 5 ] - bnd[ 4 ] ),aLength );

  static float EPS_SIZE = 5.0E-3;
  theNewSize = aLength * theSizeInPercents / 100.0;

  // if the new trihedron size have sufficient difference, then apply the value
  return fabs( theNewSize - theSize) > theSize * EPS_SIZE ||
         fabs( theNewSize-theSize ) > theNewSize * EPS_SIZE;
}
