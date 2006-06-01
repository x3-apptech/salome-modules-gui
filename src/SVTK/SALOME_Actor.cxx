//  SALOME OBJECT : implementation of interactive object visualization for OCC and VTK viewers
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : SALOME_Actor.cxx
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$

/*!
  \class SALOME_Actor SALOME_Actor.h
  \brief Abstract class of SALOME Objects in VTK.
*/


#include "SALOME_Actor.h"

#include "VTKViewer_Transform.h"
#include "VTKViewer_TransformFilter.h"
#include "VTKViewer_PassThroughFilter.h"
#include "VTKViewer_GeometryFilter.h"
#include "SVTK_RectPicker.h"

#include "SVTK_Actor.h"

// VTK Includes
#include <vtkCell.h>
#include <vtkLine.h>
#include <vtkPicker.h>
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkOutlineSource.h>

#include <vtkInteractorStyle.h>
#include <vtkRenderWindowInteractor.h>

#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

using namespace std;

#if defined __GNUC__
  #if __GNUC__ == 2
    #define __GNUC_2__
  #endif
#endif

int SALOME_POINT_SIZE = 5;
int SALOME_LINE_WIDTH = 3;

namespace
{
  int
  GetEdgeId(SALOME_Actor* theActor,
	    vtkPicker* thePicker, 
	    int theObjId)
  {
    int anEdgeId = 0;
    if (vtkCell* aPickedCell = theActor->GetElemCell(theObjId)) {
      vtkFloatingPointType aPickPosition[3];
      thePicker->GetPickPosition(aPickPosition);
      vtkFloatingPointType aMinDist = 1000000.0, aDist = 0;
      for (int i = 0, iEnd = aPickedCell->GetNumberOfEdges(); i < iEnd; i++){
	if(vtkLine* aLine = vtkLine::SafeDownCast(aPickedCell->GetEdge(i))){
	  int subId;  
	  vtkFloatingPointType pcoords[3], closestPoint[3], weights[3];
	  aLine->EvaluatePosition(aPickPosition,closestPoint,subId,pcoords,aDist,weights);
	  if (aDist < aMinDist) {
	    aMinDist = aDist;
	    anEdgeId = -1 - i;
	  }
	}
      }
    }
    return anEdgeId;
  }

  inline
  bool
  CheckDimensionId(Selection_Mode theMode, 
		   SALOME_Actor *theActor, 
		   vtkIdType theObjId)
  {
    switch(theMode){
    case CellSelection:
      return true;
    case EdgeSelection:
      return ( theActor->GetObjDimension( theObjId ) == 1 );
    case FaceSelection:
      return ( theActor->GetObjDimension( theObjId ) == 2 );
    case VolumeSelection:
      return ( theActor->GetObjDimension( theObjId ) == 3 );
    };
    return false;
  }

}


vtkStandardNewMacro(SALOME_Actor);

/*!
  Constructor
*/
SALOME_Actor
::SALOME_Actor():
  myRenderer(NULL),
  myInteractor(NULL),
  mySelectionMode(ActorSelection),
  myPreHighlightActor(SVTK_Actor::New()),
  myHighlightActor(SVTK_Actor::New()),
  myOutline(vtkOutlineSource::New()),
  myOutlineActor(VTKViewer_Actor::New())
{
  myPreHighlightActor->Delete();
  myPreHighlightActor->Initialize();
  myPreHighlightActor->PickableOff();
  myPreHighlightActor->SetVisibility( false );

  myHighlightActor->Delete();
  myHighlightActor->Initialize();
  myHighlightActor->PickableOff();
  myHighlightActor->SetVisibility( false );

  myOutline->Delete();

  vtkPolyDataMapper* anOutlineMapper = vtkPolyDataMapper::New();
  anOutlineMapper->SetInput(myOutline->GetOutput());

  myOutlineActor->Delete();
  myOutlineActor->SetMapper( anOutlineMapper );
  anOutlineMapper->Delete();

  myOutlineActor->PickableOff();
  myOutlineActor->DragableOff();
  myOutlineActor->GetProperty()->SetColor(1.0,0.0,0.0);
  myOutlineActor->GetProperty()->SetAmbient(1.0);
  myOutlineActor->GetProperty()->SetDiffuse(0.0);
  myOutlineActor->SetVisibility( false );
}

/*!
  Destructor
*/
SALOME_Actor
::~SALOME_Actor()
{}


/*!
  \return true if the SALOME_Actor has a reference to SALOME_InteractiveObject
*/
Standard_Boolean 
SALOME_Actor
::hasIO() 
{ 
  return !myIO.IsNull(); 
}

/*!
  \return correspoinding reference to SALOME_InteractiveObject
*/
const Handle(SALOME_InteractiveObject)& 
SALOME_Actor
::getIO()
{ 
  return myIO; 
}

/*!
  Sets reference to SALOME_InteractiveObject
  \param theIO - new SALOME_InteractiveObject
*/
void
SALOME_Actor
::setIO(const Handle(SALOME_InteractiveObject)& theIO) 
{ 
  myIO = theIO; 
}

/*!
  Sets name the SALOME_Actor
  \param theName - new name
*/
void
SALOME_Actor
::setName(const char* theName)
{
  if(hasIO())	
    myIO->setName(theName);
  Superclass::setName(theName);
}


/*!
  Publishes the actor in all its internal devices
*/
void
SALOME_Actor
::AddToRender(vtkRenderer* theRenderer)
{
  Superclass::AddToRender(theRenderer);

  myRenderer = theRenderer;

  theRenderer->AddActor( myPreHighlightActor.GetPointer() );
  theRenderer->AddActor( myHighlightActor.GetPointer() );
  theRenderer->AddActor( myOutlineActor.GetPointer() );
}

/*!
  Removes the actor from all its internal devices
*/
void 
SALOME_Actor
::RemoveFromRender(vtkRenderer* theRenderer)
{
  Superclass::RemoveFromRender(theRenderer);

  theRenderer->RemoveActor( myPreHighlightActor.GetPointer() );
  theRenderer->RemoveActor( myHighlightActor.GetPointer() );
  theRenderer->RemoveActor( myOutlineActor.GetPointer() );
}

/*!
  \return reference on renderer where it is published
*/
vtkRenderer*
SALOME_Actor
::GetRenderer()
{
  return myRenderer;
}

/*!
  Sets interactor in order to use vtkInteractorObserver devices
  \param theInteractor - new interactor
*/
void
SALOME_Actor
::SetInteractor(vtkRenderWindowInteractor* theInteractor)
{
  myInteractor = theInteractor;
}

/*!
  Put a request to redraw the view 
*/
void
SALOME_Actor
::Update()
{
  myInteractor->CreateTimer(VTKI_TIMER_UPDATE);    
}

/*!
  Apply view transformation
  \param theTransform - transformation
*/
void
SALOME_Actor
::SetTransform(VTKViewer_Transform* theTransform)
{
  Superclass::SetTransform(theTransform);

  myPreHighlightActor->SetTransform(theTransform);
  myHighlightActor->SetTransform(theTransform);
  myOutlineActor->SetTransform(theTransform);
}

/*!
  Apply additional position
*/
void
SALOME_Actor
::SetPosition(vtkFloatingPointType _arg1, 
	      vtkFloatingPointType _arg2, 
	      vtkFloatingPointType _arg3)
{
  Superclass::SetPosition(_arg1,_arg2,_arg3);

  myPreHighlightActor->SetPosition(_arg1,_arg2,_arg3);
  myHighlightActor->SetPosition(_arg1,_arg2,_arg3);
  myOutlineActor->SetPosition(_arg1,_arg2,_arg3);
}

/*!
  Apply additional position
*/
void
SALOME_Actor
::SetPosition(vtkFloatingPointType _arg[3])
{
  SetPosition(_arg[0],_arg[1],_arg[2]);
}

/*!
  Shows/hides actor
  \param theVisibility - new visibility state
*/
void
SALOME_Actor
::SetVisibility( int theVisibility )
{
  Superclass::SetVisibility( theVisibility );

  myOutlineActor->SetVisibility( theVisibility && isHighlighted() && !hasHighlight() );

  myPreHighlightActor->SetVisibility( theVisibility && myIsPreselected );

  if(mySelector.GetPointer() && hasIO()){
    if(mySelector->SelectionMode() != ActorSelection){
      int aHasIndex = mySelector->HasIndex( getIO() );
      myHighlightActor->SetVisibility( theVisibility && isHighlighted() && aHasIndex);
    }
  }
}

/*!
  Set selector in order to the actor at any time can restore current selection
  \param theSelector - new selector
*/
void
SALOME_Actor
::SetSelector(SVTK_Selector* theSelector)
{
  mySelector = theSelector;
}

/*!
  To map current selection to VTK representation
*/
void
SALOME_Actor
::Highlight(bool theIsHighlight)
{
  mySelectionMode = mySelector->SelectionMode();
  myHighlightActor->SetVisibility( false );
  myOutlineActor->SetVisibility( false );

  if(mySelector.GetPointer()){
    if(mySelectionMode != ActorSelection){
      TColStd_IndexedMapOfInteger aMapIndex;
      mySelector->GetIndex( getIO(), aMapIndex );
      switch( mySelectionMode ){
      case NodeSelection:
	myHighlightActor->GetProperty()->SetRepresentationToPoints();
	myHighlightActor->MapPoints( this, aMapIndex );
	break;
      case EdgeOfCellSelection:
	myHighlightActor->GetProperty()->SetRepresentationToWireframe();
	myHighlightActor->MapEdge( this, aMapIndex );
	break;
      case CellSelection:
      case EdgeSelection:
      case FaceSelection:
      case VolumeSelection:
	myHighlightActor->GetProperty()->SetRepresentationToSurface();
	myHighlightActor->MapCells( this, aMapIndex );
	break;
      }
      myHighlightActor->SetVisibility( GetVisibility() && theIsHighlight );
    }
  }

  highlight(theIsHighlight);
}

/*!
  Updates visibility of the highlight devices  
*/
void
SALOME_Actor
::highlight(bool theIsHighlight)
{
  vtkFloatingPointType aBounds[6];
  GetInput()->GetBounds(aBounds);
  myOutline->SetBounds(aBounds);
  myOutlineActor->SetVisibility( GetVisibility() && theIsHighlight );

  Superclass::highlight(theIsHighlight);
}


/*!
  To process prehighlight (called from SVTK_InteractorStyle)
*/
bool
SALOME_Actor
::PreHighlight(vtkInteractorStyle *theInteractorStyle, 
	       SVTK_SelectionEvent* theSelectionEvent,
	       bool theIsHighlight)
{
  if ( !GetPickable() )
    return false;

  vtkRenderer *aRenderer = theInteractorStyle->GetCurrentRenderer();
  //
  myPreHighlightActor->SetVisibility( false );
  bool anIsPreselected = myIsPreselected;
  
  Selection_Mode aSelectionMode = theSelectionEvent->mySelectionMode;
  bool anIsChanged = (mySelectionMode != aSelectionMode);

  vtkFloatingPointType x = theSelectionEvent->myX;
  vtkFloatingPointType y = theSelectionEvent->myY;
  vtkFloatingPointType z = 0.0;

  if( !theIsHighlight ) {
    SetPreSelected( false );
    vtkActorCollection* theActors = aRenderer->GetActors();
    theActors->InitTraversal();
    while( vtkActor *ac = theActors->GetNextActor() )
      if( SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( ac ) )
	if( anActor->hasIO() && myIO->isSame( anActor->getIO() ) )
	  anActor->SetPreSelected( false );

  }else{
    switch(aSelectionMode){
    case NodeSelection: 
    {
      myPointPicker->Pick( x, y, z, aRenderer );
      
      int aVtkId = myPointPicker->GetPointId();
      if( aVtkId >= 0 && mySelector->IsValid( this, aVtkId, true ) ) {
	int anObjId = GetNodeObjId( aVtkId );
	myIsPreselected = (anObjId >= 0);
	if(myIsPreselected){
	  const TColStd_IndexedMapOfInteger& aMapIndex = myPreHighlightActor->GetMapIndex();
	  int anExtent = aMapIndex.Extent();
	  anIsChanged |= (anExtent == 0 || anExtent > 0 && anObjId != aMapIndex(1));
	  if(anIsChanged){
	    TColStd_IndexedMapOfInteger aMapIndex;
	    aMapIndex.Add( anObjId );
	    
	    myPreHighlightActor->GetProperty()->SetRepresentationToPoints();
	    myPreHighlightActor->MapPoints( this, aMapIndex );
	  }
	  myPreHighlightActor->SetVisibility( true );
	}
      }
      break;
    }
    case CellSelection: 
    case EdgeSelection:
    case FaceSelection:
    case VolumeSelection: 
    {
      myCellPicker->Pick( x, y, z, aRenderer );
      
      int aVtkId = myCellPicker->GetCellId();
      if ( aVtkId >= 0 && mySelector->IsValid( this, aVtkId ) && hasIO() ) {
	int anObjId = GetElemObjId (aVtkId );
	if ( anObjId >= 0 ) {
	  myIsPreselected = CheckDimensionId(aSelectionMode,this,anObjId);
	  if(myIsPreselected){
	    const TColStd_IndexedMapOfInteger& aMapIndex = myPreHighlightActor->GetMapIndex();
	    int anExtent = aMapIndex.Extent();
	    anIsChanged |= (anExtent == 0 || anExtent > 0 && anObjId != aMapIndex(1));
	    if(anIsChanged){
	      TColStd_IndexedMapOfInteger aMapIndex;
	      aMapIndex.Add( anObjId );
	      
	      myPreHighlightActor->GetProperty()->SetRepresentationToSurface();
	      myPreHighlightActor->MapCells( this, aMapIndex );
	    }
	    myPreHighlightActor->SetVisibility( true );
	  }
	}
      }
      break;
    }
    case EdgeOfCellSelection:
    {
      myCellPicker->Pick( x, y, z, aRenderer );
      
      int aVtkId = myCellPicker->GetCellId();
      if ( aVtkId >= 0 && mySelector->IsValid( this, aVtkId )) {
	int anObjId = GetElemObjId( aVtkId );
	if ( anObjId >= 0 ) {
	  int anEdgeId = GetEdgeId(this,myCellPicker.GetPointer(),anObjId);
	  myIsPreselected = anEdgeId < 0;
	  if(myIsPreselected){
	    const TColStd_IndexedMapOfInteger& aMapIndex = myPreHighlightActor->GetMapIndex();
	    int anExtent = aMapIndex.Extent();
	    anIsChanged |= (anExtent == 0);
	    anIsChanged |= (anExtent == 2 && (anObjId != aMapIndex(1) || anEdgeId != aMapIndex(2)));
	    if(anIsChanged){
	      TColStd_IndexedMapOfInteger aMapIndex;
	      aMapIndex.Add( anObjId );
	      aMapIndex.Add( anEdgeId );

	      myPreHighlightActor->GetProperty()->SetRepresentationToWireframe();
	      myPreHighlightActor->MapEdge( this, aMapIndex );
	    }
	    myPreHighlightActor->SetVisibility( true );
	  }
	}
      }
      break;
    }
    case ActorSelection : 
    {
      if( !mySelector->IsSelected( myIO ) ) {
	SetPreSelected( true );

	vtkActorCollection* theActors = aRenderer->GetActors();
	theActors->InitTraversal();
	while( vtkActor *anAct = theActors->GetNextActor() ) {
	  if( anAct != this )
	    if( SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( anAct ) )
	      if( anActor->hasIO() && myIO->isSame( anActor->getIO() ) )
		anActor->SetPreSelected( true );
	}
      }
    }
    default:
      break;
    }
  }

  mySelectionMode = aSelectionMode;
  anIsChanged |= (anIsPreselected != myIsPreselected);

  return anIsChanged;
}

/*!
  To process highlight (called from SVTK_InteractorStyle)
*/
bool
SALOME_Actor
::Highlight(vtkInteractorStyle *theInteractorStyle, 
	    SVTK_SelectionEvent* theSelectionEvent,
	    bool theIsHighlight)
{
  if ( !GetPickable() )
    return false;

  myOutlineActor->SetVisibility( false );
  myHighlightActor->SetVisibility( false );

  vtkRenderer *aRenderer = theInteractorStyle->GetCurrentRenderer();
  //
  Selection_Mode aSelectionMode = theSelectionEvent->mySelectionMode;
  bool anIsShift = theSelectionEvent->myIsShift;
  if( !anIsShift || !theIsHighlight ) {
    mySelector->RemoveIObject( this );
  }

  if ( !theIsHighlight )
    return true;

  vtkFloatingPointType x = theSelectionEvent->myX;
  vtkFloatingPointType y = theSelectionEvent->myY;
  vtkFloatingPointType z = 0.0;

  if( !theSelectionEvent->myIsRectangle ) {
    switch(aSelectionMode){
    case NodeSelection: {
      myPointPicker->Pick( x, y, z, aRenderer );

      int aVtkId = myPointPicker->GetPointId();
      if( aVtkId >= 0 && mySelector->IsValid( this, aVtkId, true ) ) {
	int anObjId = GetNodeObjId( aVtkId );
	if( anObjId >= 0 ) {
	  mySelector->AddOrRemoveIndex( myIO, anObjId, anIsShift );
	  mySelector->AddIObject( this );
	}
      }
      break;
    }
    case CellSelection: 
    case EdgeSelection:
    case FaceSelection:
    case VolumeSelection: 
    {
      myCellPicker->Pick( x, y, z, aRenderer );
    
      int aVtkId = myCellPicker->GetCellId();
      if( aVtkId >= 0 && mySelector->IsValid( this, aVtkId ) ) {
	int anObjId = GetElemObjId( aVtkId );
	if( anObjId >= 0 ) {
	  if ( CheckDimensionId(aSelectionMode,this,anObjId) ) {
	    mySelector->AddOrRemoveIndex( myIO, anObjId, anIsShift );
	    mySelector->AddIObject( this );
	  }
	}
      }
      break;
    }
    case EdgeOfCellSelection: 
    {
      myCellPicker->Pick( x, y, z, aRenderer );
    
      int aVtkId = myCellPicker->GetCellId();
      if( aVtkId >= 0 && mySelector->IsValid( this, aVtkId ) ) {
	int anObjId = GetElemObjId( aVtkId );
	if( anObjId >= 0 ) {
	  int anEdgeId = GetEdgeId(this,myCellPicker.GetPointer(),anObjId);
	  if( anEdgeId < 0 ) {
	    mySelector->AddOrRemoveIndex( myIO, anObjId, false );
	    mySelector->AddOrRemoveIndex( myIO, anEdgeId, true );
	    mySelector->AddIObject( this );
	  } 
	}
      }
      break;
    }
    case ActorSelection : 
    {
      if( mySelector->IsSelected( myIO ) && anIsShift )
	mySelector->RemoveIObject( this );
      else {
	mySelector->AddIObject( this );
      }
      break;
    }
    default:
      break;
    }
  }else{
    vtkFloatingPointType xLast = theSelectionEvent->myLastX;
    vtkFloatingPointType yLast = theSelectionEvent->myLastY;
    vtkFloatingPointType zLast = 0.0;

    vtkFloatingPointType x1 = x < xLast ? x : xLast;
    vtkFloatingPointType y1 = y < yLast ? y : yLast;
    vtkFloatingPointType z1 = z < zLast ? z : zLast;
    vtkFloatingPointType x2 = x > xLast ? x : xLast;
    vtkFloatingPointType y2 = y > yLast ? y : yLast;
    vtkFloatingPointType z2 = z > zLast ? z : zLast;

    switch(aSelectionMode){
    case NodeSelection: {
      myPointRectPicker->InitializePickList();
      myPointRectPicker->AddPickList(this);
      myPointRectPicker->Pick( x1, y1, z1, x2, y2, z2, aRenderer );

      const SVTK_RectPicker::TVectorIdsMap& aVectorIdsMap = myPointRectPicker->GetPointIdsMap();
      SVTK_RectPicker::TVectorIdsMap::const_iterator aMapIter = aVectorIdsMap.find(this);
      TColStd_MapOfInteger anIndexes;
      if(aMapIter != aVectorIdsMap.end()){
	const SVTK_RectPicker::TVectorIds& aVectorIds = aMapIter->second;
	vtkIdType anEnd = aVectorIds.size();
	for(vtkIdType anId = 0; anId < anEnd; anId++ ) {
	  int aPointId = aVectorIds[anId];
	  if( aPointId >= 0 && mySelector->IsValid( this, aPointId, true ) ) {
	    int anObjId = GetNodeObjId( aPointId );
	    anIndexes.Add( anObjId );
	  }
	}
      }
      
      if( !anIndexes.IsEmpty() ) {
	mySelector->AddOrRemoveIndex( myIO, anIndexes, anIsShift );
	mySelector->AddIObject( this );
	anIndexes.Clear();
      }
      else
	mySelector->RemoveIObject( this );

      break;
    }
    case ActorSelection :
    {
      vtkFloatingPointType aPnt[3];
      vtkFloatingPointType* aBounds = GetBounds();

      bool anIsPicked = true;
      for( int i = 0; i <= 1; i++ ) {
	for( int j = 2; j <= 3; j++ ) {
	  for( int k = 4; k <= 5; k++ ) {
	    aRenderer->SetWorldPoint( aBounds[ i ], aBounds[ j ], aBounds[ k ], 1.0 );
	    aRenderer->WorldToDisplay();
	    aRenderer->GetDisplayPoint( aPnt );

	    if( aPnt[0] < x1 || aPnt[0] > x2 || aPnt[1] < y1 || aPnt[1] > y2 ) {
	      anIsPicked = false;
	      break;
	    }
	  }
	}
      }

      if( anIsPicked )
	mySelector->AddIObject(this);

      break;
    }
    case CellSelection: 
    case EdgeSelection:
    case FaceSelection:
    case VolumeSelection: 
    {
      myCellRectPicker->InitializePickList();
      myCellRectPicker->AddPickList(this);
      myCellRectPicker->Pick( x1, y1, z1, x2, y2, z2, aRenderer );

      const SVTK_RectPicker::TVectorIdsMap& aVectorIdsMap = myCellRectPicker->GetCellIdsMap();
      SVTK_RectPicker::TVectorIdsMap::const_iterator aMapIter = aVectorIdsMap.find(this);
      TColStd_MapOfInteger anIndexes;
      if(aMapIter != aVectorIdsMap.end()){
	const SVTK_RectPicker::TVectorIds& aVectorIds = aMapIter->second;
	vtkIdType anEnd = aVectorIds.size();
	for(vtkIdType anId = 0; anId < anEnd; anId++ ) {
	  int aCellId = aVectorIds[anId];
	  if ( !mySelector->IsValid( this, aCellId ) )
	    continue;

	  int anObjId = GetElemObjId( aCellId );
	  if( anObjId != -1 )
	    if ( CheckDimensionId(aSelectionMode,this,anObjId) ) {
	      anIndexes.Add(anObjId);
	    }
	}
      }
      mySelector->AddOrRemoveIndex( myIO, anIndexes, anIsShift );
      mySelector->AddIObject( this );
    }
    default:
      break;
    }
  }

  mySelectionMode = aSelectionMode;

  return true;
}

/*!
  To set up a picker for nodal selection (initialized by SVTK_Renderer::AddActor)
  \param thePointPicker - new picker
*/
void
SALOME_Actor
::SetPointPicker(vtkPointPicker* thePointPicker) 
{
  myPointPicker = thePointPicker;
}

/*!
  To set up a picker for cell selection (initialized by SVTK_Renderer::AddActor)
  \param theCellPicker - new picker
*/
void
SALOME_Actor
::SetCellPicker(vtkCellPicker* theCellPicker) 
{
  myCellPicker = theCellPicker;
}

/*!
  To set up a picker for point rectangle selection (initialized by SVTK_Renderer::AddActor)
  \param theRectPicker - new picker
*/
void
SALOME_Actor
::SetPointRectPicker(SVTK_RectPicker* theRectPicker) 
{
  myPointRectPicker = theRectPicker;
}

/*!
  To set up a picker for cell rectangle selection (initialized by SVTK_Renderer::AddActor)
  \param theRectPicker - new picker
*/
void
SALOME_Actor
::SetCellRectPicker(SVTK_RectPicker* theRectPicker) 
{
  myCellRectPicker = theRectPicker;
}

/*!
  To set up a prehighlight property (initialized by SVTK_Renderer::AddActor)
*/
void
SALOME_Actor
::SetPreHighlightProperty(vtkProperty* theProperty) 
{
  myPreHighlightActor->SetProperty(theProperty);
}

/*!
  To set up a highlight property (initialized by SVTK_Renderer::AddActor)
*/
void
SALOME_Actor
::SetHighlightProperty(vtkProperty* theProperty) 
{
  myHighlightActor->SetProperty(theProperty);
}
