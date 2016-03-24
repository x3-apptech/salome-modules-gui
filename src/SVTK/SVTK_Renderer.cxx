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
//  File   :
//  Author :

#include "SVTK_Renderer.h"

#include "SVTK_Trihedron.h"
#include "SVTK_CubeAxesActor2D.h"
#include "SVTK_AreaPicker.h"

#include "SALOME_Actor.h"
#include "VTKViewer_Actor.h"
#include "VTKViewer_Algorithm.h"
#include "VTKViewer_Transform.h"
#include "VTKViewer_Utilities.h"
#include "VTKViewer_OpenGLRenderer.h"

#include <vtkCamera.h>
#include <vtkTextProperty.h>
#include <vtkObjectFactory.h>
#include <vtkCallbackCommand.h>

#include <vtkPicker.h>
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>

#include <vtkProperty.h>
#include <vtkProp3DCollection.h>

// undefining min and max because CASCADE's defines them and
// it clashes with std::min(), std::max() included in utilities.h
#undef min
#undef max


vtkStandardNewMacro(SVTK_Renderer);

/*!
  Constructor
*/
SVTK_Renderer
::SVTK_Renderer():
  myDevice(VTKViewer_OpenGLRenderer::New()),
  myInteractor(NULL),
  myPriority(0.0),
  myEventCallbackCommand(vtkCallbackCommand::New()),
  myPointPicker(vtkPointPicker::New()),
  myCellPicker(vtkCellPicker::New()),
  myPointAreaPicker(SVTK_AreaPicker::New()),
  myCellAreaPicker(SVTK_AreaPicker::New()),
  myPreHighlightProperty(vtkProperty::New()),
  myHighlightProperty(vtkProperty::New()),
  myTransform(VTKViewer_Transform::New()),
  myCubeAxes(SVTK_CubeAxesActor2D::New()),
  myTrihedron(SVTK_Trihedron::New()),
  myTrihedronSize(105),
  myIsTrihedronRelative(true)
{
  myDevice->Delete();
  myTransform->Delete();

  myPointPicker->Delete();
  myCellPicker->Delete();

  myPointAreaPicker->Delete();
  myPointAreaPicker->PickFromListOn();

  myCellAreaPicker->Delete();
  myCellAreaPicker->PickFromListOn();
  myCellAreaPicker->PickPointsOff();

  //SetPreselectionProp();
  myPreHighlightProperty->Delete();
  myPreHighlightProperty->SetColor(0,1,1);
  myPreHighlightProperty->SetPointSize(SALOME_POINT_SIZE+2);
  myPreHighlightProperty->SetLineWidth(SALOME_LINE_WIDTH+2);
  myPreHighlightProperty->SetRepresentationToPoints();

  //SetSelectionProp();
  myHighlightProperty->Delete();
  myHighlightProperty->SetColor(1,1,0);
  myHighlightProperty->SetPointSize(SALOME_POINT_SIZE+2);
  myHighlightProperty->SetLineWidth(SALOME_LINE_WIDTH+2);
  myHighlightProperty->SetRepresentationToPoints();

  // Bug 0020123, note 0005217 - Problem with zoom
  GetDevice()->SetNearClippingPlaneTolerance( 0.00001 );

  myTrihedron->Delete();
  myCubeAxes->Delete();
  myEventCallbackCommand->Delete();

  myTrihedron->AddToRender(GetDevice());
  GetDevice()->AddViewProp(GetCubeAxes());

  myBndBox[0] = myBndBox[2] = myBndBox[4] = 0;
  myBndBox[1] = myBndBox[3] = myBndBox[5] = myTrihedron->GetSize();

  myCubeAxes->SetBounds(myBndBox);
  myCubeAxes->SetCamera(GetDevice()->GetActiveCamera());

  myCubeAxes->SetLabelFormat("%6.4g");
  myCubeAxes->SetFlyModeToOuterEdges(); // ENK remarks: it must bee
  myCubeAxes->SetFontFactor(0.8);
  myCubeAxes->SetCornerOffset(0);
  myCubeAxes->SetScaling(0);
  myCubeAxes->SetNumberOfLabels(5);
  myCubeAxes->VisibilityOff();
  myCubeAxes->SetTransform(GetTransform());

  vtkTextProperty* aTextProp = vtkTextProperty::New();
  aTextProp->SetColor(1, 1, 1);
  aTextProp->ShadowOn();
  myCubeAxes->SetAxisTitleTextProperty(aTextProp);
  myCubeAxes->SetAxisLabelTextProperty(aTextProp);
  aTextProp->Delete();

  GetDevice()->GetActiveCamera()->ParallelProjectionOn();
  GetDevice()->LightFollowCameraOn();
  GetDevice()->TwoSidedLightingOn();

  myEventCallbackCommand->SetClientData(this);
  myEventCallbackCommand->SetCallback(SVTK_Renderer::ProcessEvents);
  GetDevice()->AddObserver(vtkCommand::ConfigureEvent,
                           myEventCallbackCommand.GetPointer(), 
                           myPriority);
  GetDevice()->AddObserver(vtkCommand::ResetCameraEvent,
                           myEventCallbackCommand.GetPointer(), 
                           myPriority);
  GetDevice()->AddObserver(vtkCommand::ResetCameraClippingRangeEvent,
                           myEventCallbackCommand.GetPointer(), 
                           myPriority);
}

/*!
  Destructor
*/
SVTK_Renderer
::~SVTK_Renderer()
{
  VTK::ActorCollectionCopy aCopy(GetDevice()->GetActors());
  vtkActorCollection* anActors = aCopy.GetActors();
  vtkActorCollection* anActors2 = vtkActorCollection::New();

  anActors->InitTraversal();
  while(vtkActor* anAct = anActors->GetNextActor()){
    if(SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>(anAct)){
      anActors2->AddItem(anActor);
    }
  }

  anActors2->InitTraversal();
  while(vtkActor* anAct = anActors2->GetNextActor()){
    if(SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>(anAct)){
      RemoveActor(anActor);
    }
  }

  anActors2->Delete();
}


/*!
  Main process event method
*/
void 
SVTK_Renderer
::ProcessEvents(vtkObject* vtkNotUsed(theObject), 
                unsigned long theEvent,
                void* theClientData, 
                void* vtkNotUsed(theCallData))
{
  SVTK_Renderer* self = reinterpret_cast<SVTK_Renderer*>(theClientData);

  switch(theEvent){
  case vtkCommand::ConfigureEvent:
    self->OnResetView();
    break;
  case vtkCommand::ResetCameraEvent:
    self->OnFitAll();
    break;
  case vtkCommand::ResetCameraClippingRangeEvent:
    self->OnResetClippingRange();
    break;
  }
}

/*!
  \return renderer's device
*/
vtkRenderer* 
SVTK_Renderer
::GetDevice()
{
  return myDevice.GetPointer();
}

/*!
  Initialize renderer
*/
void 
SVTK_Renderer
::Initialize(vtkRenderWindowInteractor* theInteractor,
             SVTK_Selector* theSelector)
{
  myInteractor = theInteractor;
  mySelector = theSelector;
  SetSelectionTolerance();
}

/*!
  Publishes pointed actor into the renderer
*/
void
SVTK_Renderer
::AddActor(VTKViewer_Actor* theActor, bool theIsAdjustActors)
{
  if(SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>(theActor)){
    anActor->SetInteractor(myInteractor);
    anActor->SetTransform(GetTransform());
    anActor->SetSelector(mySelector.GetPointer());

    anActor->SetPointPicker(myPointPicker.GetPointer());
    anActor->SetCellPicker(myCellPicker.GetPointer());

    anActor->SetPointAreaPicker(myPointAreaPicker.GetPointer());
    anActor->SetCellAreaPicker(myCellAreaPicker.GetPointer());

    anActor->SetPreHighlightProperty(myPreHighlightProperty.GetPointer());
    anActor->SetHighlightProperty(myHighlightProperty.GetPointer());

    anActor->AddToRender(GetDevice());
    anActor->UpdateNameActors();

    if(theIsAdjustActors)
      AdjustActors();
  }
}

/*!
  Removes pointed actor from the renderer
*/
void
SVTK_Renderer
::RemoveActor(VTKViewer_Actor* theActor, bool theIsAdjustActors)
{
  if(SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>(theActor)){
    // Order of the calls are important because VTKViewer_Actor::RemoveFromRender
    //   can leads do destruction of the actor
    anActor->SetInteractor(NULL);
    anActor->SetTransform(NULL);
    anActor->SetSelector(NULL);

    anActor->SetPointPicker(NULL);
    anActor->SetCellPicker(NULL);

    anActor->SetPointAreaPicker(NULL);
    anActor->SetCellAreaPicker(NULL);

    anActor->SetPreHighlightProperty(NULL);
    anActor->SetHighlightProperty(NULL);

    anActor->RemoveFromRender(GetDevice());

    while ( int i = myPointPicker->GetProp3Ds()->IsItemPresent( theActor ))
      myPointPicker->GetProp3Ds()->RemoveItem( i-1 );
    while ( int i = myCellPicker->GetProp3Ds()->IsItemPresent( theActor ))
      myCellPicker->GetProp3Ds()->RemoveItem( i-1 );

    while ( int i = myPointPicker->GetActors()->IsItemPresent( theActor ))
      myPointPicker->GetActors()->RemoveItem( i-1 );
    while ( int i = myCellPicker->GetActors()->IsItemPresent( theActor ))
      myCellPicker->GetActors()->RemoveItem( i-1 );

    if(theIsAdjustActors)
      AdjustActors();
  }
}

/*!
  Get special container that keeps scaling of the scene
*/
VTKViewer_Transform* 
SVTK_Renderer
::GetTransform()
{
  return myTransform.GetPointer();
}

/*!
  Allows to get a scale that is applied on the whole scene
*/
void
SVTK_Renderer
::GetScale( double theScale[3] ) 
{
  myTransform->GetMatrixScale( theScale );
}

/*!
  Allows to apply a scale on the whole scene
*/
void
SVTK_Renderer
::SetScale( double theScale[3] ) 
{
  myTransform->SetMatrixScale( theScale[0], theScale[1], theScale[2] );
  AdjustActors();

  VTK::ActorCollectionCopy aCopy(GetDevice()->GetActors());
  vtkActorCollection* anActors = aCopy.GetActors();
  anActors->InitTraversal();
  while(vtkActor* anAct = anActors->GetNextActor())
    if(SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>(anAct))
      if(anActor->isHighlighted() && !anActor->IsInfinitive())
        anActor->highlight(true);
}

/*!
  Applies color and size (PointSize and LineWidth) of primitives in selection mode
*/
void
SVTK_Renderer
::SetSelectionProp(const double& theRed, 
                   const double& theGreen, 
                   const double& theBlue, 
                   const int& theWidth) 
{
  myHighlightProperty->SetColor( theRed, theGreen, theBlue );
  myHighlightProperty->SetLineWidth( theWidth );
  myHighlightProperty->SetPointSize( theWidth );
}

/*!
  Applies color and size (PointSize and LineWidth) of primitives in preselection mode
*/
void
SVTK_Renderer
::SetPreselectionProp(const double& theRed, 
                      const double& theGreen, 
                      const double& theBlue, 
                      const int& theWidth) 
{
  myPreHighlightProperty->SetColor( theRed, theGreen, theBlue );
  myPreHighlightProperty->SetLineWidth( theWidth );
  myPreHighlightProperty->SetPointSize( theWidth );
}

/*!
  Setup requested tolerance for the picking
*/
void
SVTK_Renderer
::SetSelectionTolerance(const double& theTolNodes, 
                        const double& theTolCell,
                        const double& theTolObjects)
{
  myPointPicker->SetTolerance( theTolNodes );
  myCellPicker->SetTolerance( theTolCell );

  myPointAreaPicker->SetTolerance( theTolNodes );
  myCellAreaPicker->SetTolerance( theTolCell );

  mySelector->SetTolerance( theTolObjects );
}


/*! If parameter theIsForcedUpdate is true, recalculate parameters for
 *  trihedron and cube axes, even if trihedron and cube axes is invisible.
 */

inline
bool
CheckBndBox(const double theBounds[6])
{
  if(theBounds[0] > -VTK_FLOAT_MAX && theBounds[1] < VTK_FLOAT_MAX &&
     theBounds[2] > -VTK_FLOAT_MAX && theBounds[3] < VTK_FLOAT_MAX &&
     theBounds[4] > -VTK_FLOAT_MAX && theBounds[5] < VTK_FLOAT_MAX)
    return true;
  return false;
}

/*!
  Adjusts size of actors
*/
bool
SVTK_Renderer
::OnAdjustActors()
{
  bool aTDisplayed = IsTrihedronDisplayed();
  bool aCDisplayed = IsCubeAxesDisplayed();

  double aNewBndBox[6];
  aNewBndBox[ 0 ] = aNewBndBox[ 2 ] = aNewBndBox[ 4 ] = VTK_FLOAT_MAX;
  aNewBndBox[ 1 ] = aNewBndBox[ 3 ] = aNewBndBox[ 5 ] = -VTK_FLOAT_MAX;

  int aVisibleNum = myTrihedron->GetVisibleActorCount(GetDevice());
  if(aVisibleNum){
    if(aTDisplayed)
      myTrihedron->VisibilityOff();

    if(aCDisplayed) 
      myCubeAxes->VisibilityOff();

    // if the new trihedron size have sufficient difference, then apply the value
    double aSize = myTrihedron->GetSize();
    if ( IsTrihedronRelative() )
      {
        ComputeTrihedronSize(GetDevice(),aSize,aSize,myTrihedronSize);
        myTrihedron->SetSize(aSize);
      }
    else
      myTrihedron->SetSize( myTrihedronSize );

    // iterate through displayed objects and set size if necessary
    VTK::ActorCollectionCopy aCopy(GetDevice()->GetActors());
    vtkActorCollection* anActors = aCopy.GetActors();
    anActors->InitTraversal();
    while(vtkActor* anAct = anActors->GetNextActor()){
      if(SALOME_Actor* anActor = dynamic_cast<SALOME_Actor*>(anAct)){
        if(anActor->IsResizable())
          anActor->SetSize(0.5*aSize);
        if(anActor->GetVisibility() && !anActor->IsInfinitive()){
          double *aBounds = anActor->GetBounds();
          if(CheckBndBox(aBounds))
            for(int i = 0; i < 5; i = i + 2){
              if(aBounds[i] < aNewBndBox[i]) 
                aNewBndBox[i] = aBounds[i];
              if(aBounds[i+1] > aNewBndBox[i+1]) 
                aNewBndBox[i+1] = aBounds[i+1];
            }
        }
      }
    }

    if(aTDisplayed) 
      myTrihedron->VisibilityOn();

    if(aCDisplayed) 
      myCubeAxes->VisibilityOn();
    
  }else{
    double aSize = myTrihedron->GetSize();
    aNewBndBox[0] = aNewBndBox[2] = aNewBndBox[4] = 0;
    aNewBndBox[1] = aNewBndBox[3] = aNewBndBox[5] = aSize;
  }
  
  if(CheckBndBox(aNewBndBox)){
    for(int i = 0; i < 6; i++)
      myBndBox[i] = aNewBndBox[i];
    myCubeAxes->SetBounds(myBndBox);
    return true;
  }

  return false;
}

/*!
  Adjusts size of actors
*/
void
SVTK_Renderer
::AdjustActors()
{
  if(OnAdjustActors())
    ::ResetCameraClippingRange(GetDevice());
}

/*!
  Set size of the trihedron
  \param theSize - new size
  \param theRelative - if it is true, then size is mesured in percents from bounding box of the scene,
  otherwise - in viewer units
*/
void
SVTK_Renderer
::SetTrihedronSize(double theSize, const bool theRelative)
{
  if(myTrihedronSize != theSize || myIsTrihedronRelative != theRelative){
    myTrihedronSize = theSize;
    myIsTrihedronRelative = theRelative;
    AdjustActors();
  }
}

/*!
  \return size of the trihedron in percents from bounding box of the scene
*/
double
SVTK_Renderer
::GetTrihedronSize() const
{
  return myTrihedronSize;
}

/*!
  \return true if the size of the trihedron is relative
*/
bool 
SVTK_Renderer
::IsTrihedronRelative() const
{
  return myIsTrihedronRelative;
}

/*!
  \return trihedron control
*/
VTKViewer_Trihedron* 
SVTK_Renderer
::GetTrihedron()
{
  return myTrihedron.GetPointer();
}

/*!
  \return true if trihedron is displayed
*/
bool
SVTK_Renderer
::IsTrihedronDisplayed()
{
  return myTrihedron->GetVisibility() == VTKViewer_Trihedron::eOn;
}

/*!
  Toggle trihedron visibility
*/
void 
SVTK_Renderer
::OnViewTrihedron()
{
  if(IsTrihedronDisplayed())
    myTrihedron->VisibilityOff();
  else
    myTrihedron->VisibilityOn();
}

/*!
  Set trihedron visibility
*/
void 
SVTK_Renderer
::SetTrihedronVisibility( const bool show ) {
  if(show)
    myTrihedron->VisibilityOn();
  else
    myTrihedron->VisibilityOff();  
}

/*!
  Adjust size of the trihedron to the bounding box of the scene
*/
void
SVTK_Renderer
::OnAdjustTrihedron()
{   
  AdjustActors();
}

/*!
  \return graduated rules control
*/
SVTK_CubeAxesActor2D* 
SVTK_Renderer
::GetCubeAxes()
{
  return myCubeAxes.GetPointer();
}

/*!
  \return true if graduated rules displayed
*/
bool
SVTK_Renderer
::IsCubeAxesDisplayed()
{
  return myCubeAxes->GetVisibility() == 1;
}

/*!
  Toggle graduated rules visibility
*/
void
SVTK_Renderer
::OnViewCubeAxes()
{
  if(IsCubeAxesDisplayed())
    myCubeAxes->VisibilityOff();
  else
    myCubeAxes->VisibilityOn();
}

/*!
  Adjust size of the graduated rules to the bounding box of the scene
*/
void
SVTK_Renderer
::OnAdjustCubeAxes()
{   
  AdjustActors();
}

/*!
  Sets camera into predefined state
*/
void
SVTK_Renderer
::OnResetView()
{
  int aTrihedronIsVisible = IsTrihedronDisplayed();
  int aCubeAxesIsVisible = IsCubeAxesDisplayed();

  myTrihedron->SetVisibility( VTKViewer_Trihedron::eOnlyLineOn );
  myCubeAxes->SetVisibility(0);

  ::ResetCamera(GetDevice(),true);  
  vtkCamera* aCamera = GetDevice()->GetActiveCamera();
  aCamera->SetPosition(1,-1,1);
  aCamera->SetViewUp(0,0,1);
  ::ResetCamera(GetDevice(),true);  

  if(aTrihedronIsVisible) 
    myTrihedron->VisibilityOn();
  else
    myTrihedron->VisibilityOff();

  if(aCubeAxesIsVisible) 
    myCubeAxes->VisibilityOn();
  else
    myCubeAxes->VisibilityOff();

  static double aCoeff = 3.0;
  aCamera->SetParallelScale(aCoeff*aCamera->GetParallelScale());
}

/*!
  Fit all presentation in the scene into the window
*/
void
SVTK_Renderer
::OnFitAll()
{
  int aTrihedronWasVisible = false;
  int aCubeAxesWasVisible = false;

  aTrihedronWasVisible = IsTrihedronDisplayed();
  if(aTrihedronWasVisible)
    myTrihedron->VisibilityOff();

  aCubeAxesWasVisible = IsCubeAxesDisplayed();
  if(aCubeAxesWasVisible)
    myCubeAxes->VisibilityOff();

  if(myTrihedron->GetVisibleActorCount(GetDevice())){
    myTrihedron->VisibilityOff();
    myCubeAxes->VisibilityOff();
    ::ResetCamera(GetDevice());
  }else{
    myTrihedron->SetVisibility(VTKViewer_Trihedron::eOnlyLineOn);
    myCubeAxes->SetVisibility(2);
    ::ResetCamera(GetDevice(),true);
  }

  if(aTrihedronWasVisible)
    myTrihedron->VisibilityOn();
  else
    myTrihedron->VisibilityOff();
  
  if(aCubeAxesWasVisible)
    myCubeAxes->VisibilityOn();
  else
    myCubeAxes->VisibilityOff();

  ::ResetCameraClippingRange(GetDevice());
}

/*!
  Fit all selected presentation in the scene
*/
void SVTK_Renderer::onFitSelection()
{
  vtkActorCollection* aSelectedCollection = vtkActorCollection::New();

  VTK::ActorCollectionCopy aCopy( GetDevice()->GetActors() );
  vtkActorCollection* aCollection = aCopy.GetActors();
  aCollection->InitTraversal();
  while ( vtkActor* aProp = aCollection->GetNextActor() )
    if ( SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( aProp ) ) {
      const Handle(SALOME_InteractiveObject)& io = anActor->getIO();
      if ( !io.IsNull() && mySelector->IsSelected( io ) )
        aSelectedCollection->AddItem( aProp );
    }
  
  if( aSelectedCollection->GetNumberOfItems() == 0 )
    return; // if collection is empty
  
  double bounds[6];
  ::ComputeBounds( aSelectedCollection, bounds );

  if ( aSelectedCollection->GetNumberOfItems() && ::isBoundValid( bounds ) ) {
    GetDevice()->ResetCamera( bounds );
    GetDevice()->ResetCameraClippingRange( bounds );
  }
}

/*!
  Reset camera clipping range to adjust the range to the bounding box of the scene
*/
void
SVTK_Renderer
::OnResetClippingRange()
{
  return;
  ::ResetCameraClippingRange(GetDevice());
}

/*!
  To reset direction of the camera to front view
*/
void
SVTK_Renderer
::OnFrontView()
{
  vtkCamera* aCamera = GetDevice()->GetActiveCamera();
  aCamera->SetPosition(1,0,0);
  aCamera->SetViewUp(0,0,1);
  aCamera->SetFocalPoint(0,0,0);
  this->OnFitAll();
}

/*!
  To reset direction of the camera to back view
*/
void
SVTK_Renderer
::OnBackView()
{
  vtkCamera* aCamera = GetDevice()->GetActiveCamera();
  aCamera->SetPosition(-1,0,0);
  aCamera->SetViewUp(0,0,1);
  aCamera->SetFocalPoint(0,0,0);
  this->OnFitAll();
}

/*!
  To reset direction of the camera to top view
*/
void
SVTK_Renderer
::OnTopView()
{
  vtkCamera* aCamera = GetDevice()->GetActiveCamera();
  aCamera->SetPosition(0,0,1);
  aCamera->SetViewUp(0,1,0);
  aCamera->SetFocalPoint(0,0,0);
  this->OnFitAll();
}

/*!
  To reset direction of the camera to bottom view
*/
void
SVTK_Renderer
::OnBottomView()
{
  vtkCamera* aCamera = GetDevice()->GetActiveCamera();
  aCamera->SetPosition(0,0,-1);
  aCamera->SetViewUp(0,1,0);
  aCamera->SetFocalPoint(0,0,0);
  this->OnFitAll();
}

/*!
  To reset direction of the camera to left view
*/
void
SVTK_Renderer
::OnLeftView()
{
  vtkCamera* aCamera = GetDevice()->GetActiveCamera(); 
  aCamera->SetPosition(0,-1,0);
  aCamera->SetViewUp(0,0,1);
  aCamera->SetFocalPoint(0,0,0);
  this->OnFitAll();
}


/*!
  To rotate view 90 degrees clockwise
*/
void
SVTK_Renderer
::onClockWiseView()
{
  vtkCamera* aCamera = GetDevice()->GetActiveCamera(); 
  aCamera->Roll(-90);
  aCamera->OrthogonalizeViewUp();
}

/*!
  To rotate view 90 degrees counterclockwise
*/
void
SVTK_Renderer
::onAntiClockWiseView()
{
  vtkCamera* aCamera = GetDevice()->GetActiveCamera(); 
  aCamera->Roll(90);
  aCamera->OrthogonalizeViewUp();
}

/*!
  To reset direction of the camera to right view
*/
void
SVTK_Renderer
::OnRightView()
{
  vtkCamera* aCamera = GetDevice()->GetActiveCamera();
  aCamera->SetPosition(0,1,0);
  aCamera->SetViewUp(0,0,1);
  aCamera->SetFocalPoint(0,0,0);
  this->OnFitAll();
}
