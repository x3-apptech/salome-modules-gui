//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : SVTK_RectPicker.cxx
//  Author : 
//  Module : SALOME
//
#include "SVTK_RectPicker.h"

#include <set>

#include <vtkObjectFactory.h>
#include <vtkCommand.h>

#include <vtkAbstractMapper3D.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

#include <vtkAssemblyPath.h>
#include <vtkAssemblyNode.h>

#include <vtkRenderWindow.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>
#include <vtkDataSet.h>
#include <vtkPoints.h>
#include <vtkCamera.h>
#include <vtkCell.h>

namespace
{
  //----------------------------------------------------------------------------
  inline
  vtkFloatingPointType
  GetZ(float* theZPtr,
       int theSelection[4],
       int theDX,
       int theDY)
  {
    return theZPtr[theDX - theSelection[0] + (theDY - theSelection[1])*(theSelection[2] - theSelection[0] + 1)];
  }


  //----------------------------------------------------------------------------
  inline
  int
  Check(float* theZPtr,
	int theSelection[4],
	vtkFloatingPointType theTolerance,
	vtkFloatingPointType theDZ,
	int theDX,
	int theDY)
  {
    int aRet = 0;
    vtkFloatingPointType aZ = -1.0;
    if(theDX >= theSelection[0] && theDX <= theSelection[2] &&
       theDY >= theSelection[1] && theDY <= theSelection[3])
    {
      // Access the value from the captured zbuffer.  Note, we only
      // captured a portion of the zbuffer, so we need to offset dx by
      // the selection window.
      aZ = GetZ(theZPtr,theSelection,theDX,theDY);
      if(aZ > theTolerance && aZ < 1.0 - theTolerance){
	aRet = fabs(aZ - theDZ) <= theTolerance;
      }
    }

    //cout<<"\tCheck = {"<<theDX<<", "<<theDY<<", "<<theDZ<<", "<<aZ<<"} = "<<aRet<<"\n";
    return aRet;
  }


  //----------------------------------------------------------------------------
  void
  SelectVisiblePoints(int theSelection[4],
		      vtkRenderer *theRenderer,
		      vtkDataSet *theInput,
		      SVTK_RectPicker::TVectorIds& theVisibleIds,
		      SVTK_RectPicker::TVectorIds& theInVisibleIds,
		      vtkFloatingPointType theTolerance)
  {
    theVisibleIds.clear();
    theInVisibleIds.clear();

    vtkIdType aNumPts = theInput->GetNumberOfPoints();
    if(aNumPts < 1)
      return;
    
    theVisibleIds.reserve(aNumPts/2 + 1);
    theInVisibleIds.reserve(aNumPts/2 + 1);

    // Grab the composite perspective transform.  This matrix is used to convert
    // each point to view coordinates.  vtkRenderer provides a WorldToView()
    // method but it computes the composite perspective transform each time
    // WorldToView() is called.  This is expensive, so we get the matrix once
    // and handle the transformation ourselves.
    vtkMatrix4x4 *aMatrix = vtkMatrix4x4::New();
    aMatrix->DeepCopy( theRenderer->GetActiveCamera()->
		       GetCompositePerspectiveTransformMatrix( theRenderer->GetTiledAspectRatio(), 0, 1 ) );

    // We grab the z-buffer for the selection region all at once and probe the resulting array.
    float *aZPtr = theRenderer->GetRenderWindow()->
      GetZbufferData(theSelection[0], theSelection[1], theSelection[2], theSelection[3]);

    //cout<<"theSelection = {"<<theSelection[0]<<", "<<theSelection[1]<<", "<<theSelection[2]<<", "<<theSelection[3]<<"}\n";

    //cout<<"\t";
    for(int iX = theSelection[0]; iX <= theSelection[2];  iX++){
      //cout<<iX<<"\t";
    }
    //cout<<endl;

    for(int iY = theSelection[1]; iY <= theSelection[3];  iY++){
      //cout<<iY<<"\t";
      for(int iX = theSelection[0]; iX <= theSelection[2];  iX++){
	//cout<<std::setprecision(4)<<GetZ(aZPtr,theSelection,iX,iY)<<"\t";
      }
      //cout<<endl;
    }

    for(vtkIdType aPntId = 0; aPntId < aNumPts; aPntId++){
      // perform conversion
      vtkFloatingPointType aX[4] = {1.0, 1.0, 1.0, 1.0};
      theInput->GetPoint(aPntId,aX);

      vtkFloatingPointType aView[4];
      aMatrix->MultiplyPoint(aX,aView);
      if(aView[3] == 0.0)
	continue;
      theRenderer->SetViewPoint(aView[0]/aView[3], 
				aView[1]/aView[3],
				aView[2]/aView[3]);
      theRenderer->ViewToDisplay();

      vtkFloatingPointType aDX[3];
      theRenderer->GetDisplayPoint(aDX);
      
      // check whether visible and in selection window 
      if(aDX[0] >= theSelection[0] && aDX[0] <= theSelection[2] &&
         aDX[1] >= theSelection[1] && aDX[1] <= theSelection[3])
      {
	//cout<<"aPntId "<<aPntId<<"; aDX = {"<<aDX[0]<<", "<<aDX[1]<<", "<<aDX[2]<<"}\n";
	int aDX0 = int(aDX[0]);
	int aDX1 = int(aDX[1]);

	int aRet = Check(aZPtr,theSelection,theTolerance,aDX[2],aDX0,aDX1);
	if(aRet > 0)
	  goto ADD_VISIBLE;
	if(aRet < 0)
	  goto ADD_INVISIBLE;

	static int aMaxRadius = 5;
	for(int aRadius = 1; aRadius < aMaxRadius; aRadius++){
	  int aStartDX[2] = {aDX0 - aRadius, aDX1 - aRadius};
	  for(int i = 0; i <= aRadius; i++){
	    int aRet = Check(aZPtr,theSelection,theTolerance,aDX[2],aStartDX[0]++,aStartDX[1]);
	    if(aRet > 0)
	      goto ADD_VISIBLE;
	    if(aRet < 0)
	      goto ADD_INVISIBLE;
	  }
	  for(int i = 0; i <= aRadius; i++){
	    int aRet = Check(aZPtr,theSelection,theTolerance,aDX[2],aStartDX[0],aStartDX[1]++);
	    if(aRet > 0)
	      goto ADD_VISIBLE;
	    if(aRet < 0)
	      goto ADD_INVISIBLE;
	  }
	  for(int i = 0; i <= aRadius; i++){
	    int aRet = Check(aZPtr,theSelection,theTolerance,aDX[2],aStartDX[0]--,aStartDX[1]);
	    if(aRet > 0)
	      goto ADD_VISIBLE;
	    if(aRet < 0)
	      goto ADD_INVISIBLE;
	  }
	  for(int i = 0; i <= aRadius; i++){
	    int aRet = Check(aZPtr,theSelection,theTolerance,aDX[2],aStartDX[0],aStartDX[1]--);
	    if(aRet > 0)
	      goto ADD_VISIBLE;
	    if(aRet < 0)
	      goto ADD_INVISIBLE;
	  }
	}
	if(false)
	  ADD_VISIBLE : theVisibleIds.push_back(aPntId);
	if(false)
	  ADD_INVISIBLE : theInVisibleIds.push_back(aPntId);
      }
    }//for all points

    aMatrix->Delete();

    if(aZPtr)
      delete [] aZPtr;
  }


  //----------------------------------------------------------------------------
  inline
  void
  GetCenter(const vtkFloatingPointType theBounds[6],
	    vtkFloatingPointType theCenter[3])
  {
    theCenter[0] = (theBounds[1] + theBounds[0]) / 2.0;
    theCenter[1] = (theBounds[3] + theBounds[2]) / 2.0;
    theCenter[2] = (theBounds[5] + theBounds[4]) / 2.0;
  }

  void
  SelectVisibleCells(int theSelection[4],
		     vtkRenderer *theRenderer,
		     vtkDataSet *theInput,
		     SVTK_RectPicker::TVectorIds& theVectorIds,
		     vtkFloatingPointType theTolerance)
  {
    theVectorIds.clear();

    vtkIdType aNumCells = theInput->GetNumberOfCells();
    if(aNumCells < 1)
      return;
    
    theVectorIds.reserve(aNumCells/2 + 1);

    SVTK_RectPicker::TVectorIds aVisiblePntIds;
    SVTK_RectPicker::TVectorIds anInVisiblePntIds;
    SelectVisiblePoints(theSelection,
			theRenderer,
			theInput,
			aVisiblePntIds,
			anInVisiblePntIds,
			theTolerance);

    typedef std::set<vtkIdType> TIdsSet;
    TIdsSet aVisibleIds(aVisiblePntIds.begin(),aVisiblePntIds.end());
    TIdsSet anInVisibleIds(anInVisiblePntIds.begin(),anInVisiblePntIds.end());

    // Grab the composite perspective transform.  This matrix is used to convert
    // each point to view coordinates.  vtkRenderer provides a WorldToView()
    // method but it computes the composite perspective transform each time
    // WorldToView() is called.  This is expensive, so we get the matrix once
    // and handle the transformation ourselves.
    vtkMatrix4x4 *aMatrix = vtkMatrix4x4::New();
    aMatrix->DeepCopy(theRenderer->GetActiveCamera()->
		      GetCompositePerspectiveTransformMatrix( theRenderer->GetTiledAspectRatio(), 0, 1 ) );

    for(vtkIdType aCellId = 0; aCellId < aNumCells; aCellId++){
      vtkCell* aCell = theInput->GetCell(aCellId);

      vtkFloatingPointType aBounds[6];
      aCell->GetBounds(aBounds);

      vtkFloatingPointType aCenter[3];
      GetCenter(aBounds,aCenter);

      vtkFloatingPointType aView[4];
      vtkFloatingPointType aX[4] = {aCenter[0], aCenter[1], aCenter[2], 1.0};
      aMatrix->MultiplyPoint(aX,aView);

      if(aView[3] == 0.0)
	continue;

      theRenderer->SetViewPoint(aView[0]/aView[3], 
				aView[1]/aView[3],
				aView[2]/aView[3]);
      theRenderer->ViewToDisplay();

      vtkFloatingPointType aDX[3];
      theRenderer->GetDisplayPoint(aDX);
      
      // check whether visible and in selection window 
      if(aDX[0] >= theSelection[0] && aDX[0] <= theSelection[2] &&
         aDX[1] >= theSelection[1] && aDX[1] <= theSelection[3])
      {

	//cout<<"aCellId = "<<aCellId<<": ";
	vtkIdType aNumPts = aCell->GetNumberOfPoints();
	bool anIsVisible = true;
	for(vtkIdType anId = 0; anId < aNumPts; anId++){
	  vtkIdType aPntId = aCell->GetPointId(anId);
	  //cout<<aPntId<<"; ";
	  anIsVisible = aVisibleIds.find(aPntId) != aVisibleIds.end();
	  if(!anIsVisible)
	    break;
	}
	//cout<<"\t"<<anIsVisible<<"\n";
	if(anIsVisible)
	  theVectorIds.push_back(aCellId);
      }
    }//for all parts
  }

  //----------------------------------------------------------------------------
  void
  CalculatePickPosition(vtkRenderer *theRenderer,
			vtkFloatingPointType theSelectionX, 
			vtkFloatingPointType theSelectionY, 
			vtkFloatingPointType theSelectionZ,
			vtkFloatingPointType thePickPosition[3])
  {
    // Convert the selection point into world coordinates.
    //
    theRenderer->SetDisplayPoint(theSelectionX, theSelectionY, theSelectionZ);
    theRenderer->DisplayToWorld();
    vtkFloatingPointType* aWorldCoords = theRenderer->GetWorldPoint();
    if ( aWorldCoords[3] != 0.0 ) {
      for (int i=0; i < 3; i++) {
	thePickPosition[i] = aWorldCoords[i] / aWorldCoords[3];
      }
    }
  }
}

vtkStandardNewMacro(SVTK_RectPicker);

SVTK_RectPicker
::SVTK_RectPicker()
{
  this->Tolerance = 0.005;
  this->PickPoints = 1;
}

SVTK_RectPicker
::~SVTK_RectPicker()
{}

int
SVTK_RectPicker
::Pick(vtkFloatingPointType, 
       vtkFloatingPointType, 
       vtkFloatingPointType, 
       vtkRenderer*)
{
  return 0;
}

int
SVTK_RectPicker
::Pick(vtkFloatingPointType theSelection[3], 
       vtkFloatingPointType theSelection2[3], 
       vtkRenderer *theRenderer)
{
  return Pick(theSelection[0], theSelection[1], theSelection[2], 
	      theSelection2[0], theSelection2[1], theSelection2[2],
	      theRenderer);
}

int 
SVTK_RectPicker
::Pick(vtkFloatingPointType theSelectionX, 
       vtkFloatingPointType theSelectionY, 
       vtkFloatingPointType theSelectionZ, 
       vtkFloatingPointType theSelectionX2, 
       vtkFloatingPointType theSelectionY2, 
       vtkFloatingPointType theSelectionZ2,
       vtkRenderer *theRenderer)
{
  //  Initialize picking process
  this->Initialize();
  myCellIdsMap.clear();
  myPointIdsMap.clear();
  this->Renderer = theRenderer;

  // Get camera focal point and position. Convert to display (screen) 
  // coordinates. We need a depth value for z-buffer.
  //
  vtkCamera* aCamera = theRenderer->GetActiveCamera();

  vtkFloatingPointType aCameraFP[4];
  aCamera->GetFocalPoint(aCameraFP); 
  aCameraFP[3] = 1.0;

  theRenderer->SetWorldPoint(aCameraFP);
  theRenderer->WorldToDisplay();
  vtkFloatingPointType* aDisplayCoords = theRenderer->GetDisplayPoint();
  vtkFloatingPointType aSelectionZ = aDisplayCoords[2];

  this->SelectionPoint[0] = theSelectionX;
  this->SelectionPoint[1] = theSelectionY;
  this->SelectionPoint[2] = theSelectionZ;

  // Convert the selection point into world coordinates.
  //
  CalculatePickPosition(theRenderer,
			theSelectionX,
			theSelectionY,
			aSelectionZ,
			this->PickPosition);

  this->SelectionPoint2[0] = theSelectionX2;
  this->SelectionPoint2[1] = theSelectionY2;
  this->SelectionPoint2[2] = theSelectionZ2;

  // Convert the selection point into world coordinates.
  //
  CalculatePickPosition(theRenderer,
			theSelectionX2,
			theSelectionY2,
			aSelectionZ,
			this->PickPosition2);

  // Invoke start pick method if defined
  this->InvokeEvent(vtkCommand::StartPickEvent,NULL);

  vtkPropCollection *aProps;
  if ( this->PickFromList ) 
    aProps = this->GetPickList();
  else 
    aProps = theRenderer->GetProps();

  aProps->InitTraversal();
  while ( vtkProp* aProp = aProps->GetNextProp() ) {
    aProp->InitPathTraversal();
    while ( vtkAssemblyPath* aPath = aProp->GetNextPath() ) {
      vtkMapper *aMapper = NULL;
      bool anIsPickable = false;
      vtkActor* anActor = NULL;
      vtkProp *aPropCandidate = aPath->GetLastNode()->GetProp();
      if ( aPropCandidate->GetPickable() && aPropCandidate->GetVisibility() ) {
        anIsPickable = true;
	anActor = vtkActor::SafeDownCast(aPropCandidate);
	if ( anActor ) {
          aMapper = anActor->GetMapper();
          if ( anActor->GetProperty()->GetOpacity() <= 0.0 )
	    anIsPickable = false;
	}
      }
      if ( anIsPickable  &&  aMapper && aMapper->GetInput()) {
	int aSelectionPoint[4] = {int(theSelectionX),
				  int(theSelectionY),
				  int(theSelectionX2),
				  int(theSelectionY2)};
	if ( this->PickPoints ) {
	  TVectorIds& aVisibleIds = myPointIdsMap[anActor];
	  TVectorIds anInVisibleIds;
	  SelectVisiblePoints(aSelectionPoint,
			      theRenderer,
			      aMapper->GetInput(),
			      aVisibleIds,
			      anInVisibleIds,
			      this->Tolerance);
	  if ( aVisibleIds.empty() ) {
	    myPointIdsMap.erase(myPointIdsMap.find(anActor));
	  }
	} else {
	  TVectorIds& aVectorIds = myCellIdsMap[anActor];
	  SelectVisibleCells(aSelectionPoint,
			     theRenderer,
			     aMapper->GetInput(),
			     aVectorIds,
			     this->Tolerance);
	  if ( aVectorIds.empty() ) {
	    myCellIdsMap.erase(myCellIdsMap.find(anActor));
	  }
	}
      }
    }
  }

  // Invoke end pick method if defined
  this->InvokeEvent(vtkCommand::EndPickEvent,NULL);

  return myPointIdsMap.empty() || myCellIdsMap.empty();
}


const SVTK_RectPicker::TVectorIdsMap& 
SVTK_RectPicker
::GetPointIdsMap() const
{
  return myPointIdsMap;
}

const SVTK_RectPicker::TVectorIdsMap& 
SVTK_RectPicker
::GetCellIdsMap() const
{
  return myCellIdsMap;
}

