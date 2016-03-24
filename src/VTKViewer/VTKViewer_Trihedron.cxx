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

#include "VTKViewer_Trihedron.h"
#include "VTKViewer_Actor.h"
#include "VTKViewer_Algorithm.h"

// VTK Includes
#include <vtkConfigure.h>
#include <vtkMath.h>
#include <vtkMapper.h>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkObjectFactory.h>

#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkVectorText.h>
#include <vtkTextActor.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

// QT includes
#include <QtGlobal>

vtkStandardNewMacro(VTKViewer_UnScaledActor);

/*!Constructor*/
VTKViewer_UnScaledActor::VTKViewer_UnScaledActor() 
{
  Bounds[0] = Bounds[2] = Bounds[4] = VTK_FLOAT_MAX;
  Bounds[1] = Bounds[3] = Bounds[5] = -VTK_FLOAT_MAX;
}

/*!
  \return bounding box
*/
double* 
VTKViewer_UnScaledActor
::GetBounds()
{
  return Bounds;
}

/*! Sets \a mySize= \a theSize variable.
 * \param  theSize - integer size
 */
void VTKViewer_UnScaledActor::SetSize(int theSize)
{
  mySize = theSize;
}

/*!This causes the actor to be rendered.
 * Set new scale for actor.
 */
void VTKViewer_UnScaledActor::Render(vtkRenderer *theRenderer)
{
  if(theRenderer){
    double P[2][3] = {{-1.0, -1.0, 0.0},{+1.0, +1.0, 0.0}};
    theRenderer->ViewToWorld(P[0][0],P[0][1],P[0][2]);
    theRenderer->ViewToWorld(P[1][0],P[1][1],P[1][2]);
    double aWorldDiag = sqrt((P[1][0]-P[0][0])*(P[1][0]-P[0][0])+
                                           (P[1][1]-P[0][1])*(P[1][1]-P[0][1])+
                                           (P[1][2]-P[0][2])*(P[1][2]-P[0][2]));
    int* aSize = theRenderer->GetRenderWindow()->GetSize();
    double aWinDiag = sqrt(double(aSize[0]*aSize[0]+aSize[1]*aSize[1]));
    vtkDataSet* aDataSet = GetMapper()->GetInput();
    GetMapper()->Update();
    double aLength = aDataSet->GetLength();
    double aPrecision = 1.0E-3;
    double aZeroTol   = 1.0E-12;
    double anOldScale = GetScale()[0];
    double aScale = anOldScale;
    double aMaxSize = (double)qMax(aSize[1],aSize[0]);
    if (qAbs(aWinDiag) > aZeroTol && qAbs(aLength) > aZeroTol && qAbs(aMaxSize) > aZeroTol)
      aScale = mySize*aWorldDiag/aWinDiag/aLength*sqrt(double(qMin(aSize[1],aSize[0]))/aMaxSize);
    if(qAbs(aScale) > aZeroTol && qAbs(aScale - anOldScale)/aScale > aPrecision){
      SetScale(aScale);
    }
  }
  vtkFollower::Render(theRenderer);
}

vtkStandardNewMacro(VTKViewer_LineActor);

#ifdef IPAL21440
vtkCxxSetObjectMacro(VTKViewer_LineActor,LabelActor,vtkTextActor);
#else
vtkCxxSetObjectMacro(VTKViewer_LineActor,LabelActor,VTKViewer_UnScaledActor);
#endif
vtkCxxSetObjectMacro(VTKViewer_LineActor,ArrowActor,vtkFollower);

/*!Adds Label and Arrow actors to \a theRenderer.*/
void VTKViewer_LineActor::Render(vtkRenderer *theRenderer)
{
#ifndef IPAL21440
  if(LabelActor && LabelActor->GetVisibility()){
    LabelActor->Modified();
    LabelActor->Render(theRenderer);
  }
#endif
  if(ArrowActor && ArrowActor->GetVisibility()){
    ArrowActor->Modified();
    ArrowActor->Render(theRenderer);
  }
  vtkFollower::Render(theRenderer);
}

/*!
  Constructor
*/
VTKViewer_Axis::VTKViewer_Axis()
{
  /*! \li Initialize the Line pipe-line representation*/
  myLineSource = vtkLineSource::New();
  myLineSource->SetPoint1(0.0,0.0,0.0);
  
  myMapper[0] = vtkPolyDataMapper::New();
  myMapper[0]->SetInputConnection(myLineSource->GetOutputPort());
  
  myLineActor = VTKViewer_LineActor::New();
  myLineActor->SetMapper(myMapper[0]);
  myLineActor->PickableOff();
  
  /*! \li Initialize the Arrow pipe-line representation*/
  myConeSource =  vtkConeSource::New();
  myConeSource->SetResolution(16);
  myConeSource->SetAngle(10);
  myConeSource->SetCenter(-0.5,0.0,0.0);
  
  myMapper[1] = vtkPolyDataMapper::New();
  myMapper[1]->SetInputConnection(myConeSource->GetOutputPort());
  
  myArrowActor = vtkFollower::New();
  myArrowActor->SetMapper(myMapper[1]);
  static int aArrowActorSize = 16;
  myArrowActor->SetScale(aArrowActorSize);
  myArrowActor->PickableOff();
  
  myLineActor->SetArrowActor(myArrowActor);
  
  /*! \li Initialize the Label pipe-line representation */
#ifdef IPAL21440
  myTextMapper = vtkTextMapper::New();
  
  myLabelActor = vtkTextActor::New();
  myLabelActor->SetMapper(myTextMapper);
  myLabelActor->SetTextScaleModeToNone();
  myLabelActor->PickableOff();
  
  vtkCoordinate* aCoord = vtkCoordinate::New();
  myLabelActor->GetPositionCoordinate()->SetReferenceCoordinate( aCoord );
  aCoord->Delete();
#else
  myVectorText = vtkVectorText::New();
  
  myMapper[2] = vtkPolyDataMapper::New();
  myMapper[2]->SetInputConnection(myVectorText->GetOutputPort());
  
  myLabelActor = VTKViewer_UnScaledActor::New();
  myLabelActor->SetMapper(myMapper[2]);
  static int aLabelActorSize = 12;
  myLabelActor->SetSize(aLabelActorSize);
  myLabelActor->PickableOff();
  //myLabelActor->DebugOn();
#endif
  
  myLineActor->SetLabelActor(myLabelActor);
  
  /*! \li Initialise visibility param.*/
  myVisibility = VTKViewer_Trihedron::eOn;
}

/*!
  Destructor
*/
VTKViewer_Axis::~VTKViewer_Axis()
{
  /*! \li Destroy of the Label pipe-line representation */
  myLabelActor->Delete();
  
  myMapper[0]->RemoveAllInputs();
  myMapper[0]->Delete();
  
  /*! \li Destroy of the Arrow pipe-line representation */
  myArrowActor->Delete();
  
  myMapper[1]->RemoveAllInputs();
  myMapper[1]->Delete();
  
  myConeSource->Delete();
  
  /*! \li Destroy of the Line pipe-line representation */
  myLineActor->Delete();
  
#ifdef IPAL21440
  myTextMapper->RemoveAllInputs();
  myTextMapper->Delete();
#else
  myVectorText->Delete();
  
  myMapper[2]->RemoveAllInputs();
  myMapper[2]->Delete();
#endif
  
  myLineSource->Delete();
}

/*! Add to renderer
 * \param theRenderer - vtkRenderer pointer
 */
void VTKViewer_Axis::AddToRender(vtkRenderer* theRenderer){
  /*! \li Order of the calls are important*/
  theRenderer->AddActor(myLineActor);
  theRenderer->AddActor(myLabelActor);
  theRenderer->AddActor(myArrowActor);
}

/*! Remove actor of acis from \a theRenderer which are in myPresent.
 * \param theRenderer - vtkRenderer pointer
 */
void VTKViewer_Axis::RemoveFromRender(vtkRenderer* theRenderer){
  /*! \li Order of the calls are important*/
  theRenderer->RemoveActor(myLineActor);
  theRenderer->RemoveActor(myLabelActor);
  theRenderer->RemoveActor(myArrowActor);
}

/*! Sets visibility for all Axis to \a theVis*/
void VTKViewer_Axis::SetVisibility(VTKViewer_Trihedron::TVisibility theVis)
{
  switch(theVis){
  case VTKViewer_Trihedron::eOff:
  case VTKViewer_Trihedron::eOn:
    myLabelActor->SetVisibility(theVis);
    myArrowActor->SetVisibility(theVis);
    myLineActor->SetVisibility(theVis);
    break;
  case VTKViewer_Trihedron::eOnlyLineOn:
    myLabelActor->VisibilityOff();
    myArrowActor->VisibilityOff();
    myLineActor->VisibilityOn();
    break;
  default:
    return;
  }
  myVisibility = theVis;
}

/*! Set camera for myLabelActor
 */
void VTKViewer_Axis::SetCamera(vtkCamera* theCamera){
#ifndef IPAL21440
  myLabelActor->SetCamera(theCamera);
#endif
}

/*! Sets color for actors: myLineActor,myLabelActor,myArrowActor
 */
void VTKViewer_Axis::SetColor(double theRed, double theGreen, double theBlue)
{
  // Set color property for arrow and line actors
  vtkProperty* aProperty = vtkProperty::New();
  aProperty->SetColor(theRed, theGreen, theBlue);

  myArrowActor->SetProperty(aProperty);
  myLineActor->SetProperty(aProperty);
#ifndef IPAL21440
  myLabelActor->SetProperty(aProperty);
#endif

  aProperty->Delete();
  
  // Set color property for label actor
#ifdef IPAL21440
  vtkTextProperty* aTextProperty = vtkTextProperty::New();
  aTextProperty->SetColor(theRed, theGreen, theBlue);

  myLabelActor->SetTextProperty(aTextProperty);

  aTextProperty->Delete();
#endif
}

/*! Set size of VTKViewer_Axis
 */
void VTKViewer_Axis::SetSize(double theSize)
{
  double aPosition[3] = {myDir[0]*theSize, myDir[1]*theSize, myDir[2]*theSize};

  double aCoef = 0.99;
  double aLinePosition[3] = {aPosition[0]*aCoef, aPosition[1]*aCoef, aPosition[2]*aCoef};
  myLineSource->SetPoint2(aLinePosition);
  
  myArrowActor->SetPosition(0.0,0.0,0.0);
  myArrowActor->AddPosition(aPosition);
  myArrowActor->SetOrientation(myRot);
  myArrowActor->SetScale(theSize / 10.);
  
#ifdef IPAL21440
  if( vtkCoordinate* aCoord = myLabelActor->GetPositionCoordinate()->GetReferenceCoordinate() )
    aCoord->SetValue( aPosition );
#else
  myLabelActor->SetPosition(0.0,0.0,0.0);
  myLabelActor->AddPosition(aPosition);
#endif
}

/*! Check if actor belongs to the axis object
 * \param theActor - vtkActor pointer
 * \retval Return true if the actor belongs to the axis object
 */
bool VTKViewer_Axis::OwnActor(const vtkActor* theActor)
{
  return theActor == myLineActor  || 
         theActor == myArrowActor ||
#ifdef IPAL21440
         false;
#else
         theActor == myLabelActor;
#endif
}

/*! \class VTKViewer_XAxis
 * \brief X Axis actor
 */
class VTKViewer_XAxis : public VTKViewer_Axis
{
protected:
  VTKViewer_XAxis();
  VTKViewer_XAxis(const VTKViewer_XAxis&);
public:
  vtkTypeMacro(VTKViewer_XAxis,VTKViewer_Axis);
  static VTKViewer_XAxis *New();
};

vtkStandardNewMacro(VTKViewer_XAxis);

/*!Initialize X Axis*/
VTKViewer_XAxis::VTKViewer_XAxis(){ 
  myDir[0] = 1.0; myDir[1] = 0.0; myDir[2] = 0.0;
  myRot[0] = 0.0; myRot[1] = 0.0; myRot[2] = 0.0;
#ifdef IPAL21440
  myTextMapper->SetInput("X");
#else
  myVectorText->SetText("X");
#endif
  SetColor(1.0,0.0,0.0);
}

/*! \class VTKViewer_YAxis
 * \brief Y Axis actor
 */
class VTKViewer_YAxis : public VTKViewer_Axis{
protected:
  VTKViewer_YAxis();
  VTKViewer_YAxis(const VTKViewer_YAxis&);
public:
  vtkTypeMacro(VTKViewer_YAxis,VTKViewer_Axis);
  static VTKViewer_YAxis *New();
};

vtkStandardNewMacro(VTKViewer_YAxis);

/*!Initialize Y Axis*/
VTKViewer_YAxis::VTKViewer_YAxis()
{ 
  myDir[0] = 0.0; myDir[1] = 1.0; myDir[2] = 0.0;
  myRot[0] = 0.0; myRot[1] = 0.0; myRot[2] = 90.;
#ifdef IPAL21440
  myTextMapper->SetInput("Y");
#else
  myVectorText->SetText("Y");
#endif
  SetColor(0.0,1.0,0.0);
}

/*! \class VTKViewer_ZAxis
 * \brief Z Axis actor
 */
class VTKViewer_ZAxis : public VTKViewer_Axis
{
protected:
  VTKViewer_ZAxis();
  VTKViewer_ZAxis(const VTKViewer_ZAxis&);
public:
  vtkTypeMacro(VTKViewer_ZAxis,VTKViewer_Axis);
  static VTKViewer_ZAxis *New();
};

vtkStandardNewMacro(VTKViewer_ZAxis);

/*!Initialize Z Axis*/
VTKViewer_ZAxis::VTKViewer_ZAxis()
{
  myDir[0] = 0.0; myDir[1] = 0.0; myDir[2] = 1.0;
  myRot[0] = 0.0; myRot[1] = -90; myRot[2] = 0.0;
#ifdef IPAL21440
  myTextMapper->SetInput("Z");
#else
  myVectorText->SetText("Z");
#endif
  SetColor(0.0,0.0,1.0);
}

vtkStandardNewMacro(VTKViewer_Trihedron);

/*!
  Constructor
*/
VTKViewer_Trihedron::VTKViewer_Trihedron()
{
  myPresent = vtkActorCollection::New();
  myAxis[0] = VTKViewer_XAxis::New();
  myAxis[1] = VTKViewer_YAxis::New();
  myAxis[2] = VTKViewer_ZAxis::New();
  static double aSize = 100;
  SetSize(aSize);
}

/*!
  Destructor
*/
VTKViewer_Trihedron::~VTKViewer_Trihedron()
{
  myPresent->RemoveAllItems();
  myPresent->Delete();
  for(int i = 0; i < 3; i++)
    myAxis[i]->Delete();
}

/*! Set size of axes
 */
void VTKViewer_Trihedron::SetSize(double theSize)
{
  mySize = theSize;
  for(int i = 0; i < 3; i++)
    myAxis[i]->SetSize(theSize);
}

/*! Set visibility of axes
 */
void VTKViewer_Trihedron::SetVisibility(TVisibility theVis)
{
  for(int i = 0; i < 3; i++)
    myAxis[i]->SetVisibility(theVis);
}

/*!
  \return visibility of first axis
*/
VTKViewer_Trihedron::TVisibility VTKViewer_Trihedron::GetVisibility()
{
  return myAxis[0]->GetVisibility();
}

/*! Add to render all Axis
 * \param theRenderer - vtkRenderer pointer
 */
void VTKViewer_Trihedron::AddToRender(vtkRenderer* theRenderer)
{
  vtkCamera* aCamera = theRenderer->GetActiveCamera();
  for(int i = 0; i < 3; i++){
    myAxis[i]->AddToRender(theRenderer);
    myAxis[i]->SetCamera(aCamera);
  }
}

/*! Remove all actors from \a theRenderer which are in myPresent.
 * \param theRenderer - vtkRenderer pointer
 */
void VTKViewer_Trihedron::RemoveFromRender(vtkRenderer* theRenderer)
{
  myPresent->InitTraversal();
  while(vtkActor* anActor = myPresent->GetNextActor())
    theRenderer->RemoveActor(anActor);
  for(int i = 0; i < 3; i++)
    myAxis[i]->RemoveFromRender(theRenderer);
}

/*! Return count of visible actors.
 * \param theRenderer - vtkRenderer pointer
 */
int VTKViewer_Trihedron::GetVisibleActorCount(vtkRenderer* theRenderer)
{
  //TVisibility aVis = GetVisibility();
  //SetVisibility(eOff);
  VTK::ActorCollectionCopy aCopy(theRenderer->GetActors());
  vtkActorCollection* aCollection = aCopy.GetActors();
  aCollection->InitTraversal();
  int aCount = 0;
  while(vtkActor* prop = aCollection->GetNextActor()) {
    if( prop->GetVisibility()) {
      if(VTKViewer_Actor* anActor = VTKViewer_Actor::SafeDownCast(prop)) {
        if(!anActor->IsInfinitive()) 
          aCount++;
      }
      else if ( !OwnActor( anActor ) ) {
        aCount++;
      }
        //int aCount = theRenderer->VisibleActorCount();
        //SetVisibility(aVis);
    }
  }
  return aCount;
}

/*! Check if actor belongs to the axis object
 * \param theActor - vtkActor pointer
 * \retval Return true if the actor belongs to the axis object
 */
bool VTKViewer_Trihedron::OwnActor(const vtkActor* theActor)
{
  myPresent->InitTraversal();
  while(vtkActor* anActor = myPresent->GetNextActor()) {
    if ( anActor == theActor ) return true;
  }
  for(int i = 0; i < 3; i++) {
    if ( myAxis[i]->OwnActor(theActor) ) return true;
  }
  return false;
}
