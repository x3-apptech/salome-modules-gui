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

//  SALOME OBJECT : implementation of interactive object visualization for OCC and VTK viewers
//  File   : SALOME_Actor.cxx
//  Author : Nicolas REJNERI

/*!
  \class SALOME_Actor SALOME_Actor.h
  \brief Abstract class of SALOME Objects in VTK.
*/


#include "SALOME_Actor.h"
#include "SALOME_InteractiveObject.hxx"

#include "VTKViewer_Algorithm.h"
#include "VTKViewer_Transform.h"
#include "VTKViewer_TransformFilter.h"
#include "VTKViewer_GeometryFilter.h"
#include "VTKViewer_FramedTextActor.h"
#include "SVTK_AreaPicker.h"

#include "SVTK_Actor.h"

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

// VTK Includes
#include <vtkCell.h>
#include <vtkLine.h>
#include <vtkQuadraticEdge.h>
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
#include <vtkPolygon.h>

#include <vtkInteractorStyle.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPassThroughFilter.h>

#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <math.h>
#include <QPoint>
#include <QVector>

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
      double aPickPosition[3];
      thePicker->GetPickPosition(aPickPosition);
      double aMinDist = 1000000.0, aDist = 0;
      vtkCell* aSelEdge;
      for (int i = 0, iEnd = aPickedCell->GetNumberOfEdges(); i < iEnd; i++){
	aSelEdge = aPickedCell->GetEdge(i);
        if(vtkLine::SafeDownCast(aPickedCell->GetEdge(i)) || 
	   vtkQuadraticEdge::SafeDownCast(aPickedCell->GetEdge(i))){
          int subId;  
          double pcoords[3], closestPoint[3], weights[3];
          aSelEdge->EvaluatePosition(aPickPosition,closestPoint,subId,pcoords,aDist,weights);
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
    switch(theMode) {
    case CellSelection:
      return true;
    case EdgeSelection:
      return ( theActor->GetObjDimension( theObjId ) == 1 );
    case FaceSelection:
      return ( theActor->GetObjDimension( theObjId ) == 2 );
    case VolumeSelection:
      return ( theActor->GetObjDimension( theObjId ) == 3 );
    case Elem0DSelection:        
      return ((theActor->GetObjDimension( theObjId ) == 0) &&
               theActor->GetElemCell(theObjId) && 
              (theActor->GetElemCell(theObjId)->GetCellType() == VTK_VERTEX));
    case BallSelection:
      return ((theActor->GetObjDimension( theObjId ) == 0) &&
               theActor->GetElemCell(theObjId) && 
              (theActor->GetElemCell(theObjId)->GetCellType() == VTK_POLY_VERTEX));

    };
    return false;
  }
}

namespace SVTK
{
  /*!
    Make picker work with this actor only
  */
  TPickLimiter::TPickLimiter(vtkAbstractPicker* picker, SALOME_Actor* actor):myPicker(picker)
  {
    myPicker->InitializePickList();
    myPicker->AddPickList( actor );
    myPicker->SetPickFromList( true );
  }
  /*!
    Unlimit picking
  */
  TPickLimiter::~TPickLimiter()
  {
    myPicker->SetPickFromList( false );
    myPicker->InitializePickList();
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
  myOutlineActor(VTKViewer_Actor::New()),
  myIsDisplayNameActor(false),
  myNameActor(VTKViewer_FramedTextActor::New())
{
  myPreHighlightActor->Delete();
  myPreHighlightActor->Initialize();
  myPreHighlightActor->PickableOff();
  myPreHighlightActor->SetVisibility( false );
  myPreHighlightActor->SetCoincident3DAllowed(true);

  myHighlightActor->Delete();
  myHighlightActor->Initialize();
  myHighlightActor->PickableOff();
  myHighlightActor->SetVisibility( false );
  myHighlightActor->SetCoincident3DAllowed(true);

  myOutline->Delete();

  vtkPolyDataMapper* anOutlineMapper = vtkPolyDataMapper::New();
  anOutlineMapper->SetInputConnection(myOutline->GetOutputPort());

  myOutlineActor->Delete();
  myOutlineActor->SetMapper( anOutlineMapper );
  anOutlineMapper->Delete();

  myOutlineActor->PickableOff();
  myOutlineActor->DragableOff();
  myOutlineActor->GetProperty()->SetColor(1.0,0.0,0.0);
  myOutlineActor->GetProperty()->SetAmbient(1.0);
  myOutlineActor->GetProperty()->SetDiffuse(0.0);
  myOutlineActor->SetVisibility( false );

  // Name actor
  myNameActor->Delete();
  myNameActor->SetVisibility(false);
  myNameActor->SetPickable(false);
  myNameActor->SetModePosition(VTKViewer_FramedTextActor::TopRight);
  myNameActor->SetLayoutType(VTKViewer_FramedTextActor::Vertical);

  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();

  QColor aForegroundColor = aResourceMgr->colorValue( "VTKViewer", "group_names_text_color", Qt::white );
  myNameActor->SetForegroundColor(aForegroundColor.redF(),
                                  aForegroundColor.greenF(),
                                  aForegroundColor.blueF());

  double aGroupNamesTransparency = 0.5;
  aGroupNamesTransparency = aResourceMgr->doubleValue( "VTKViewer", "group_names_transparency", aGroupNamesTransparency );
  myNameActor->SetTransparency(aGroupNamesTransparency);
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
  myNameActor->SetText(theName);
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

  myHighlightActor->AddToRender(theRenderer);
  myPreHighlightActor->AddToRender(theRenderer);
  theRenderer->AddActor( myOutlineActor.GetPointer() );
  theRenderer->AddActor( myNameActor.GetPointer() );
}

/*!
  Removes the actor from all its internal devices
*/
void 
SALOME_Actor
::RemoveFromRender(vtkRenderer* theRenderer)
{
  Superclass::RemoveFromRender(theRenderer);

  myHighlightActor->RemoveFromRender(theRenderer);
  myPreHighlightActor->RemoveFromRender(theRenderer);

  theRenderer->RemoveActor( myPreHighlightActor.GetPointer() );
  theRenderer->RemoveActor( myHighlightActor.GetPointer() );
  theRenderer->RemoveActor( myOutlineActor.GetPointer() );
  theRenderer->RemoveActor( myNameActor.GetPointer() );
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
::SetPosition(double _arg1, 
              double _arg2, 
              double _arg3)
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
::SetPosition(double _arg[3])
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

  UpdateNameActors();
}

/*!
  Gets know whether the actor should be displayed or not
*/
bool 
SALOME_Actor
::ShouldBeDisplayed()
{
  return true;
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
      case Elem0DSelection: 
      case BallSelection: 
        myHighlightActor->GetProperty()->SetRepresentationToSurface();
        myHighlightActor->MapCells( this, aMapIndex );
        break;
      }
      myHighlightActor->SetVisibility( GetVisibility() && theIsHighlight );
    }
    myHighlightActor->SetMarkerEnabled( mySelectionMode == NodeSelection );
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
  double aBounds[6];
  vtkDataSet * aDataSet = GetHighlightedDataSet();
  aDataSet->GetBounds(aBounds);
  myOutline->SetBounds(aBounds);
  myOutline->Update();
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
  if ( !GetPickable() || !mySelector || !mySelector->IsPreSelectionEnabled() )
    return false;
      
  vtkRenderer *aRenderer = theInteractorStyle->GetCurrentRenderer();
  //
  myPreHighlightActor->SetVisibility( false );
  bool anIsPreselected = myIsPreselected;
  SetPreSelected( false );

  Selection_Mode aSelectionMode = theSelectionEvent->mySelectionMode;
  bool anIsChanged = (mySelectionMode != aSelectionMode);

  myPreHighlightActor->SetMarkerEnabled( aSelectionMode == NodeSelection );

  double x = theSelectionEvent->myX;
  double y = theSelectionEvent->myY;
  double z = 0.0;

  if( !theIsHighlight ) {
    if ( hasIO() ) {
      VTK::ActorCollectionCopy aCopy(aRenderer->GetActors());
      vtkActorCollection* theActors = aCopy.GetActors();
      theActors->InitTraversal();
      while( vtkActor *ac = theActors->GetNextActor() )
        if( SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( ac ) )
          if( anActor->hasIO() && myIO->isSame( anActor->getIO() ) )
            anActor->SetPreSelected( false );
    }
  }else{
    switch(aSelectionMode) {
    case NodeSelection: 
    {
      SVTK::TPickLimiter aPickLimiter( myPointPicker, this );
      myPointPicker->Pick( x, y, z, aRenderer );
      
      int aVtkId = myPointPicker->GetPointId();
      if( aVtkId >= 0 && mySelector->IsValid( this, aVtkId, true ) ) {
        int anObjId = GetNodeObjId( aVtkId );
        myIsPreselected = (anObjId >= 0);
        if(myIsPreselected){
          const TColStd_IndexedMapOfInteger& aMapIndex = myPreHighlightActor->GetMapIndex();
          int anExtent = aMapIndex.Extent();
          anIsChanged |= (anExtent == 0 || (anExtent > 0 && anObjId != aMapIndex(1)));
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
    case Elem0DSelection:        
    case BallSelection: 
    {
      SVTK::TPickLimiter aPickLimiter( myCellPicker, this );
      myCellPicker->Pick( x, y, z, aRenderer );
      
      int aVtkId = myCellPicker->GetCellId();
      if ( aVtkId >= 0 && mySelector->IsValid( this, aVtkId ) && hasIO() ) {
        int anObjId = GetElemObjId (aVtkId );
        if ( anObjId >= 0 ) {
          myIsPreselected = CheckDimensionId(aSelectionMode,this,anObjId);
          if(myIsPreselected){
            const TColStd_IndexedMapOfInteger& aMapIndex = myPreHighlightActor->GetMapIndex();
            int anExtent = aMapIndex.Extent();
            anIsChanged |= (anExtent == 0 || (anExtent > 0 && anObjId != aMapIndex(1)));
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
      SVTK::TPickLimiter aPickLimiter( myCellPicker, this );
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
            anIsChanged |= (anExtent == 0 || anExtent == 1);
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

        if ( hasIO() ) {
          VTK::ActorCollectionCopy aCopy(aRenderer->GetActors());
          vtkActorCollection* theActors = aCopy.GetActors();
          theActors->InitTraversal();
          while( vtkActor *anAct = theActors->GetNextActor() ) {
            if( anAct != this )
              if( SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( anAct ) )
                if( anActor->hasIO() && myIO->isSame( anActor->getIO() ) )
                  anActor->SetPreSelected( true );
          }
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
  if ( !GetPickable() || !mySelector || !mySelector->IsSelectionEnabled() )
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

  double x = theSelectionEvent->myX;
  double y = theSelectionEvent->myY;

  if( theSelectionEvent->myIsRectangle || theSelectionEvent->myIsPolygon ) {
    double xLast = theSelectionEvent->myLastX;
    double yLast = theSelectionEvent->myLastY;

    double x1 = x < xLast ? x : xLast;
    double y1 = y < yLast ? y : yLast;
    double x2 = x > xLast ? x : xLast;
    double y2 = y > yLast ? y : yLast;

    switch(aSelectionMode){
    case NodeSelection: {

      SVTK::TPickLimiter aPickLimiter( myPointAreaPicker, this );
      if ( theSelectionEvent->myIsRectangle )
        myPointAreaPicker->Pick( x1, y1, x2, y2, aRenderer, SVTK_AreaPicker::RectangleMode );
      else if( theSelectionEvent->myIsPolygon )
        myPointAreaPicker->Pick( theSelectionEvent->myPolygonPoints, aRenderer, SVTK_AreaPicker::PolygonMode );

      const SVTK_AreaPicker::TVectorIdsMap& aVectorIdsMap = myPointAreaPicker->GetPointIdsMap();
      SVTK_AreaPicker::TVectorIdsMap::const_iterator aMapIter = aVectorIdsMap.find(this);
      TColStd_MapOfInteger anIndexes;
      if(aMapIter != aVectorIdsMap.end()){
        const SVTK_AreaPicker::TVectorIds& aVectorIds = aMapIter->second;
        vtkIdType anEnd = aVectorIds.size();
        for(vtkIdType anId = 0; anId < anEnd; anId++ ) {
          int aPointId = aVectorIds[anId];
          if( aPointId >= 0 && mySelector->IsValid( this, aPointId, true ) ) {
            int anObjId = GetNodeObjId( aPointId );
            anIndexes.Add( anObjId );
          }
        }
      }
      
      if ( hasIO() ) {
        if( !anIndexes.IsEmpty() ) {
          mySelector->AddOrRemoveIndex( myIO, anIndexes, anIsShift );
          mySelector->AddIObject( this );
          anIndexes.Clear();
        }
        else if ( !anIsShift )
          mySelector->RemoveIObject( this );
      }
      break;
    }
    case ActorSelection :
    {
      double aPnt[3];
      double* aBounds = GetBounds();

      bool anIsPicked = true;
      for( int i = 0; i <= 1; i++ ) {
        for( int j = 2; j <= 3; j++ ) {
          for( int k = 4; k <= 5; k++ ) {
            aRenderer->SetWorldPoint( aBounds[ i ], aBounds[ j ], aBounds[ k ], 1.0 );
            aRenderer->WorldToDisplay();
            aRenderer->GetDisplayPoint( aPnt );
            bool anIsPointInSelection;
            if( theSelectionEvent->myIsRectangle )
              anIsPointInSelection =  aPnt[0] > x1 && aPnt[0] < x2 && aPnt[1] > y1 && aPnt[1] < y2;
            else if( theSelectionEvent->myIsPolygon )
              anIsPointInSelection = myPointAreaPicker->isPointInPolygon( QPoint( aPnt[0], aPnt[1] ),
                                                                          theSelectionEvent->myPolygonPoints );

            if( !anIsPointInSelection ) {
                anIsPicked = false;
                break;
            }
          }
        }
      }

      if ( hasIO() ) {
        if( anIsPicked && mySelector->IsSelected( myIO ) && anIsShift )
          mySelector->RemoveIObject( this );
        else if ( anIsPicked ){
          mySelector->AddIObject( this );
        }
      }

      break;
    }
    case CellSelection: 
    case EdgeSelection:
    case FaceSelection:
    case VolumeSelection: 
    case Elem0DSelection:        
    case BallSelection: 
    {
      SVTK::TPickLimiter aPickLimiter( myCellAreaPicker, this );
      if( theSelectionEvent->myIsRectangle )
        myCellAreaPicker->Pick( x1, y1, x2, y2, aRenderer, SVTK_AreaPicker::RectangleMode );
      else if( theSelectionEvent->myIsPolygon )
        myCellAreaPicker->Pick( theSelectionEvent->myPolygonPoints, aRenderer, SVTK_AreaPicker::PolygonMode );

      const SVTK_AreaPicker::TVectorIdsMap& aVectorIdsMap = myCellAreaPicker->GetCellIdsMap();
      SVTK_AreaPicker::TVectorIdsMap::const_iterator aMapIter = aVectorIdsMap.find(this);
      TColStd_MapOfInteger anIndexes;
      if(aMapIter != aVectorIdsMap.end()){
        const SVTK_AreaPicker::TVectorIds& aVectorIds = aMapIter->second;
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
      
      if ( hasIO() ) {
        if( !anIndexes.IsEmpty() ) {
          mySelector->AddOrRemoveIndex( myIO, anIndexes, anIsShift );
          mySelector->AddIObject( this );
          anIndexes.Clear();
        }
        else if ( !anIsShift )
          mySelector->RemoveIObject( this );
      }
    }
    default:
      break;
    }
  }
  else {
    switch(aSelectionMode){
    case NodeSelection: {
      SVTK::TPickLimiter aPickLimiter( myPointPicker, this );
      myPointPicker->Pick( x, y, 0.0, aRenderer );

      int aVtkId = myPointPicker->GetPointId();
      if( aVtkId >= 0 && mySelector->IsValid( this, aVtkId, true ) ) {
        int anObjId = GetNodeObjId( aVtkId );
        if( hasIO() && anObjId >= 0 ) {
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
    case Elem0DSelection:
    case BallSelection:
    {
      SVTK::TPickLimiter aPickLimiter( myCellPicker, this );
      myCellPicker->Pick( x, y, 0.0, aRenderer );

      int aVtkId = myCellPicker->GetCellId();
      if( aVtkId >= 0 && mySelector->IsValid( this, aVtkId ) ) {
        int anObjId = GetElemObjId( aVtkId );
        if( anObjId >= 0 ) {
          if ( hasIO() && CheckDimensionId(aSelectionMode,this,anObjId) ) {
            mySelector->AddOrRemoveIndex( myIO, anObjId, anIsShift );
            mySelector->AddIObject( this );
          }
        }
      }
      break;
    }
    case EdgeOfCellSelection:
    {
      SVTK::TPickLimiter aPickLimiter( myCellPicker, this );
      myCellPicker->Pick( x, y, 0.0, aRenderer );

      int aVtkId = myCellPicker->GetCellId();
      if( aVtkId >= 0 && mySelector->IsValid( this, aVtkId ) ) {
        int anObjId = GetElemObjId( aVtkId );
        if( anObjId >= 0 ) {
          int anEdgeId = GetEdgeId(this,myCellPicker.GetPointer(),anObjId);
          if( hasIO() && anEdgeId < 0 ) {
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
      if ( hasIO() ) {
        if( mySelector->IsSelected( myIO ) && anIsShift )
          mySelector->RemoveIObject( this );
        else {
          mySelector->AddIObject( this );
        }
      }
      break;
    }
    default:
      break;
    }
  }

  mySelectionMode = aSelectionMode;

  return true;
}

/*!
  To get flag of displaying of name actor
  \return flag to display or not to display name actor
*/
bool
SALOME_Actor
::IsDisplayNameActor() const
{
  return myIsDisplayNameActor;
}

/*!
  To set flag of displaying of name actor
  \param theIsDisplayNameActor flag to display or not to display name actor
*/
void
SALOME_Actor
::SetIsDisplayNameActor(bool theIsDisplayNameActor)
{
  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
  bool isShowGroupNames = aResourceMgr->booleanValue("VTKViewer", "show_group_names", false);
  myIsDisplayNameActor = theIsDisplayNameActor && isShowGroupNames;
  UpdateNameActors();
}

/*!
  To set text of name actor
  \param theText - text of name actor
*/
void
SALOME_Actor
::SetNameActorText(const char* theText)
{
  myNameActor->SetText(theText);
}

/*!
  To set offset of name actor
  \param theOffset - offset of name actor
*/
void
SALOME_Actor
::SetNameActorOffset(double theOffset[2])
{
  myNameActor->SetOffset(theOffset);
}

/*!
  To get size of name actor
  \param theRenderer - renderer
  \param theSize - size of name actor
*/
void
SALOME_Actor
::GetNameActorSize(vtkRenderer* theRenderer, double theSize[2]) const
{
  myNameActor->GetSize(theRenderer, theSize);
}

/*!
  Update visibility of name actors
*/
void
SALOME_Actor
::UpdateNameActors()
{
  if( vtkRenderer* aRenderer = GetRenderer() )
  {
    double anOffset[2] = { 0, 0 };
    VTK::ActorCollectionCopy aCopy(aRenderer->GetActors());
    vtkActorCollection* aCollection = aCopy.GetActors();
    for( int anIndex = 0, aNbItems = aCollection->GetNumberOfItems(); anIndex < aNbItems; anIndex++ )
    {
      if( SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>( aCollection->GetItemAsObject( anIndex ) ) )
      {
        if( anActor->IsDisplayNameActor() )
        {
          anActor->SetNameActorOffset( anOffset );
          if( anActor->GetVisibility() )
          {
            double aSize[2];
            anActor->GetNameActorSize( aRenderer, aSize );
            anOffset[0] = anOffset[0] + aSize[0];
            anOffset[1] = anOffset[1] + aSize[1];
          }
        }
      }
    }
  }
  myNameActor->SetVisibility( GetVisibility() && IsDisplayNameActor() );
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
  To set up a picker for point rectangle or polygonal selection (initialized by SVTK_Renderer::AddActor)
  \param theAreaPicker - new picker
*/
void
SALOME_Actor
::SetPointAreaPicker(SVTK_AreaPicker* theAreaPicker)
{
  myPointAreaPicker = theAreaPicker;
}

/*!
  To set up a picker for cell rectangle of polygonal selection (initialized by SVTK_Renderer::AddActor)
  \param theAreaPicker - new picker
*/
void
SALOME_Actor
::SetCellAreaPicker(SVTK_AreaPicker* theAreaPicker)
{
  myCellAreaPicker = theAreaPicker;
}

/*!
  To find a gravity center of object
  \param theObjId - identification of object
*/
double*
SALOME_Actor
::GetGravityCenter( int theObjId )
{
  double* result = new double[3];
  for( int i = 0; i < 3; i++ )
    result[i]= 0.0;

  vtkPoints* points = GetElemCell( theObjId )->GetPoints();
  int nbPoints = points->GetNumberOfPoints();

  if( nbPoints <= 0 )
    return NULL;

  for( int i = 0; i < nbPoints; i++ )
  {
    double* aPoint = points->GetPoint(i);
    result[0] += aPoint[0];
    result[1] += aPoint[1];
    result[2] += aPoint[2];
  }
  result[0] = result[0] / nbPoints;
  result[1] = result[1] / nbPoints;
  result[2] = result[2] / nbPoints;

  return result;
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

/*!
  Set standard point marker
  \param theMarkerType type of the marker
  \param theMarkerScale scale of the marker
*/
void
SALOME_Actor
::SetMarkerStd( VTK::MarkerType theMarkerType, VTK::MarkerScale theMarkerScale )
{
  myPreHighlightActor->SetMarkerStd( theMarkerType, theMarkerScale );
  myHighlightActor->SetMarkerStd( theMarkerType, theMarkerScale );
}

/*!
  Set custom point marker
  \param theMarkerId id of the marker texture
  \param theMarkerTexture marker texture
*/
void
SALOME_Actor
::SetMarkerTexture( int theMarkerId, VTK::MarkerTexture theMarkerTexture )
{
  myPreHighlightActor->SetMarkerTexture( theMarkerId, theMarkerTexture );
  myHighlightActor->SetMarkerTexture( theMarkerId, theMarkerTexture );
}

/*!
  Get type of the point marker
  \return type of the point marker
*/
VTK::MarkerType
SALOME_Actor
::GetMarkerType()
{
  return myPreHighlightActor->GetMarkerType();
}

/*!
  Get scale of the point marker
  \return scale of the point marker
*/
VTK::MarkerScale
SALOME_Actor
::GetMarkerScale()
{
  return myPreHighlightActor->GetMarkerScale();
}

/*!
  Get texture identifier of the point marker
  \return texture identifier of the point marker
 */
int
SALOME_Actor
::GetMarkerTexture()
{
  return myPreHighlightActor->GetMarkerTexture();
}
