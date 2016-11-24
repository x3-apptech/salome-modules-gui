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

#include "VTKViewer_Actor.h"

#include "VTKViewer_Transform.h"
#include "VTKViewer_TransformFilter.h"
#include "VTKViewer_GeometryFilter.h"

// VTK Includes
#include <vtkCell.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkPassThroughFilter.h>

#if defined __GNUC__
  #if __GNUC__ == 2
    #define __GNUC_2__
  #endif
#endif

int VTKViewer_POINT_SIZE = 5;
int VTKViewer_LINE_WIDTH = 3;


vtkStandardNewMacro(VTKViewer_Actor);

/*!
  Constructor
*/
VTKViewer_Actor
::VTKViewer_Actor():
  myIsResolveCoincidentTopology(true),
  myOpacity(1.0),
  myDisplayMode(1),
  myIsInfinite(false),
  myStoreMapping(false),
  myGeomFilter(VTKViewer_GeometryFilter::New()),
  myTransformFilter(VTKViewer_TransformFilter::New()),
  myRepresentation(VTKViewer::Representation::Surface),
  myProperty(vtkProperty::New()),
  PreviewProperty(NULL),
  myIsPreselected(false),
  myIsHighlighted(false)
{
  vtkMapper::GetResolveCoincidentTopologyPolygonOffsetParameters(myPolygonOffsetFactor,
                                                                 myPolygonOffsetUnits);

  for(int i = 0; i < 6; i++)
    myPassFilter.push_back(vtkPassThroughFilter::New());
}

/*!
  Destructor
*/
VTKViewer_Actor
::~VTKViewer_Actor()
{
  SetPreviewProperty(NULL);

  myGeomFilter->Delete();

  myTransformFilter->Delete();

  for(int i = 0, iEnd = myPassFilter.size(); i < iEnd; i++)
    if(myPassFilter[i])
      myPassFilter[i]->Delete();
  
  myProperty->Delete();
}

/*!
  \return name
*/
const char* 
VTKViewer_Actor
::getName() 
{ 
  return myName.c_str(); 
}

/*!
  Sets name
  \param theName - new name
*/
void
VTKViewer_Actor
::setName(const char* theName)
{
  myName = theName;
}

/*!
  To publish the actor an all its internal devices
*/
void
VTKViewer_Actor
::AddToRender(vtkRenderer* theRenderer)
{
  theRenderer->AddActor(this);
}

/*!
  To remove the actor an all its internal devices
*/
void 
VTKViewer_Actor
::RemoveFromRender(vtkRenderer* theRenderer)
{
  theRenderer->RemoveActor(this);
}

/*!
  Used to obtain all dependent actors
*/
void
VTKViewer_Actor
::GetChildActors(vtkActorCollection*) 
{}

/*!
  Apply view transformation
  \param theTransform - view transformation
*/
void
VTKViewer_Actor
::SetTransform(VTKViewer_Transform* theTransform)
{
  myTransformFilter->SetTransform(theTransform);
}


/*!
  To insert some additional filters and then sets the given #vtkMapper
*/
void
VTKViewer_Actor
::SetMapper(vtkMapper* theMapper)
{
  InitPipeLine(theMapper);
}

/*!
  Initialization
*/
void
VTKViewer_Actor
::InitPipeLine(vtkMapper* theMapper)
{
  if(theMapper){
    int anId = 0;
    myPassFilter[ anId ]->SetInputData( theMapper->GetInput() );
    myPassFilter[ anId + 1]->SetInputConnection( myPassFilter[ anId ]->GetOutputPort() );
    
    anId++; // 1
    myGeomFilter->SetStoreMapping( myStoreMapping );
    myGeomFilter->SetInputConnection( myPassFilter[ anId ]->GetOutputPort() );

    anId++; // 2
    myPassFilter[ anId ]->SetInputConnection( myGeomFilter->GetOutputPort() ); 
    myPassFilter[ anId + 1 ]->SetInputConnection( myPassFilter[ anId ]->GetOutputPort() );

    anId++; // 3
    myTransformFilter->SetInputConnection( myPassFilter[ anId ]->GetOutputPort() );

    anId++; // 4
    myPassFilter[ anId ]->SetInputConnection( myTransformFilter->GetOutputPort() );
    myPassFilter[ anId + 1 ]->SetInputConnection( myPassFilter[ anId ]->GetOutputPort() );

    anId++; // 5
    theMapper->SetInputConnection(myPassFilter[anId]->GetOutputPort());
  }
  Superclass::SetMapper(theMapper);
}

/*!
  Renders actor
*/
void
VTKViewer_Actor
::Render(vtkRenderer *ren, vtkMapper* m)
{
  if(vtkDataSet* aDataSet = GetInput()){
    static double PERCENTS_OF_DETAILS = 0.50;
    vtkIdType aNbOfPoints = vtkIdType(aDataSet->GetNumberOfPoints()*PERCENTS_OF_DETAILS);
    if(aNbOfPoints > 0)
      SetNumberOfCloudPoints(aNbOfPoints);
  }

  int aResolveCoincidentTopology = vtkMapper::GetResolveCoincidentTopology();
  if(myIsResolveCoincidentTopology){
    double aFactor, aUnit; 
    vtkMapper::GetResolveCoincidentTopologyPolygonOffsetParameters(aFactor,aUnit);
    
    vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
    vtkMapper::SetResolveCoincidentTopologyPolygonOffsetParameters(myPolygonOffsetFactor,
                                                                   myPolygonOffsetUnits);
    Superclass::Render(ren,m);
    
    vtkMapper::SetResolveCoincidentTopologyPolygonOffsetParameters(aFactor,aUnit);
  }else{
    vtkMapper::SetResolveCoincidentTopologyToOff();
    Superclass::Render(ren,m);
  }
  vtkMapper::SetResolveCoincidentTopology(aResolveCoincidentTopology);
}

/*!
  Set ResolveCoincidentTopology flag
  \param theIsResolve - new flag value
*/
void
VTKViewer_Actor
::SetResolveCoincidentTopology(bool theIsResolve) 
{
  myIsResolveCoincidentTopology = theIsResolve;
}

/*!
  Set polygon offset parameters
  \param factor, units  - Opengl polygon offset parameters
*/
void
VTKViewer_Actor
::SetPolygonOffsetParameters(double factor, 
                             double units)
{
  myPolygonOffsetFactor = factor;
  myPolygonOffsetUnits = units;
}

/*!
  Get polygon offset parameters
  \param factor, units  - Opengl polygon offset parameters
*/
void
VTKViewer_Actor
::GetPolygonOffsetParameters(double& factor, 
                             double& units)
{
  factor = myPolygonOffsetFactor;
  units = myPolygonOffsetUnits;
}

/*!
  \return shrink factor
*/
double
VTKViewer_Actor
::GetShrinkFactor() 
{ 
  return 1.0;
}

/*!
  \return true if the actor is shrunkable
*/
bool
VTKViewer_Actor
::IsShrunkable() 
{ 
  return false;
}

/*!
  \return true if the actor is shrunk
*/
bool
VTKViewer_Actor
::IsShrunk() 
{ 
  return false;
}

/*!
  Insert shrink filter into pipeline
*/
void
VTKViewer_Actor
::SetShrink() 
{} 

/*!
  Remove shrink filter from pipeline
*/
void
VTKViewer_Actor
::UnShrink() 
{}

/*!
  Allows to get initial #vtkDataSet
*/
vtkDataSet* 
VTKViewer_Actor
::GetInput()
{
  return myPassFilter.front()->GetOutput();
}

/*!
  To calculatate last modified time
*/
vtkMTimeType
VTKViewer_Actor
::GetMTime()
{
  vtkMTimeType mTime = this->Superclass::GetMTime();
  vtkMTimeType time = myTransformFilter->GetMTime();
  mTime = ( time > mTime ? time : mTime );
  if(vtkDataSet *aDataSet = dynamic_cast<vtkDataSet*>(myPassFilter[0]->GetInput())){ // bad usage of GetInput
    time = aDataSet->GetMTime();
    mTime = ( time > mTime ? time : mTime );
  }
  return mTime;
}

/*!
  Set representation (VTK_SURFACE, VTK_POINTS, VTK_WIREFRAME and so on)
  param theMode - new mode
*/
void
VTKViewer_Actor
::SetRepresentation(int theMode) 
{ 
  using namespace VTKViewer::Representation;
  switch(myRepresentation){
  case Points : 
  case Surface : 
  case SurfaceWithEdges :
    myProperty->SetAmbient(GetProperty()->GetAmbient());
    myProperty->SetDiffuse(GetProperty()->GetDiffuse());
    myProperty->SetSpecular(GetProperty()->GetSpecular());
    break;
  }    

  switch(theMode){
  case Points : 
  case Surface : 
  case SurfaceWithEdges :
    GetProperty()->SetAmbient(myProperty->GetAmbient());
    GetProperty()->SetDiffuse(myProperty->GetDiffuse());
    GetProperty()->SetSpecular(myProperty->GetSpecular());
    break;
  default:
    GetProperty()->SetAmbient(1.0);
    GetProperty()->SetDiffuse(0.0);
    GetProperty()->SetSpecular(0.0);
  }

  switch(theMode){
  case Insideframe : 
    myGeomFilter->SetInside(true);
    myGeomFilter->SetWireframeMode(true);
    GetProperty()->SetRepresentation(VTK_WIREFRAME);
    break;
  case Points : 
    GetProperty()->SetPointSize(VTKViewer_POINT_SIZE);  
    GetProperty()->SetRepresentation(theMode);
    myGeomFilter->SetWireframeMode(false);
    myGeomFilter->SetInside(false);
    break;
  case Wireframe : 
    GetProperty()->SetRepresentation(theMode);
    myGeomFilter->SetWireframeMode(true);
    myGeomFilter->SetInside(false);
    break;
  case Surface : 
  case SurfaceWithEdges :
    GetProperty()->SetRepresentation(theMode);
    myGeomFilter->SetWireframeMode(false);
    myGeomFilter->SetInside(false);
    break;
  }

  myRepresentation = theMode;
}

/*!
  \return current representation mode
*/
int
VTKViewer_Actor
::GetRepresentation()
{ 
  return myRepresentation;
}

/*!
  Maps VTK index of a node to corresponding object index
*/
int 
VTKViewer_Actor
::GetNodeObjId(int theVtkID)
{ 
  return theVtkID;
}

/*!
  Get coordinates of a node for given object index
*/
double* 
VTKViewer_Actor
::GetNodeCoord(int theObjID)
{
  return GetInput()->GetPoint(theObjID);
}

/*!
  Get corresponding #vtkCell for given object index
*/
vtkCell* 
VTKViewer_Actor
::GetElemCell(int theObjID)
{
  return GetInput()->GetCell(theObjID);
}

/*!
  Maps VTK index of a cell to corresponding object index
*/
int
VTKViewer_Actor
::GetElemObjId(int theVtkID) 
{ 
  return theVtkID;
}


/*!
  \return object dimension. Virtual method should be redifined by derived classes
*/
int
VTKViewer_Actor
::GetObjDimension( const int theObjId )
{
  if ( vtkCell* aCell = GetElemCell(theObjId) )
    return aCell->GetCellDimension();
  return 0;
}

/*!
  Infinitive means actor without size (point for example),
  which is not taken into account in calculation of boundaries of the scene
*/
void
VTKViewer_Actor
::SetInfinitive(bool theIsInfinite)
{ 
  myIsInfinite = theIsInfinite;
}

/*!
  \return infinive flag
*/
bool
VTKViewer_Actor
::IsInfinitive()
{ 
  if(myIsInfinite)
    return true;

  static double MAX_DISTANCE = 0.9*VTK_FLOAT_MAX;
  double aBounds[6];
  GetBounds(aBounds);
  for(int i = 0; i < 6; i++)
    if(fabs(aBounds[i]) > MAX_DISTANCE)
      return true;
  
  static double MIN_DISTANCE = 1.0/VTK_FLOAT_MAX;
  if(GetLength() < MIN_DISTANCE)
    return true;
  
  return false;
}

/*!
  \return current bounding box
*/
double* 
VTKViewer_Actor
::GetBounds()
{
  return Superclass::GetBounds();
}


/*!
  \return current bounding box
*/
void
VTKViewer_Actor
::GetBounds(double theBounds[6])
{
  Superclass::GetBounds(theBounds);
}


bool
VTKViewer_Actor
::IsSetCamera() const 
{ 
  return false; 
}

bool
VTKViewer_Actor
::IsResizable() const 
{ 
  return false; 
}

void
VTKViewer_Actor
::SetSize( const double ) 
{}


void 
VTKViewer_Actor
::SetCamera( vtkCamera* ) 
{}


void
VTKViewer_Actor
::SetOpacity(double theOpacity)
{ 
  myOpacity = theOpacity;
  GetProperty()->SetOpacity(theOpacity);
}

double
VTKViewer_Actor
::GetOpacity()
{
  return myOpacity;
}


/*!
  Change color
*/
void
VTKViewer_Actor
::SetColor(double r,
           double g,
           double b)
{
  GetProperty()->SetColor(r,g,b);
}

/*!
  Change color
*/
void
VTKViewer_Actor
::SetColor(const double theRGB[3])
{ 
  SetColor(theRGB[0],theRGB[1],theRGB[2]);
}

/*!
  Get color
*/
void
VTKViewer_Actor
::GetColor(double& r,
           double& g,
           double& b)
{
  double aColor[3];
  GetProperty()->GetColor(aColor);
  r = aColor[0];
  g = aColor[1];
  b = aColor[2];
}


/*!
  Change material
*/
void
VTKViewer_Actor
::SetMaterial(std::vector<vtkProperty*> theProps)
{
}

/*!
  Get current front material
*/
vtkProperty* 
VTKViewer_Actor
::GetFrontMaterial()
{
  return NULL;
}

/*!
  Get current back material
*/
vtkProperty* 
VTKViewer_Actor
::GetBackMaterial()
{
  return NULL;
}

/*!
  \return display mode
*/
int
VTKViewer_Actor
::getDisplayMode()
{ 
  return myDisplayMode; 
}

/*!
  Change display mode
*/
void
VTKViewer_Actor
::setDisplayMode(int theMode)
{ 
  SetRepresentation(theMode + 1); 
  myDisplayMode = GetRepresentation() - 1;
}


/*!
  \return true if the descendant of the VTKViewer_Actor will implement its own highlight or not
*/
bool
VTKViewer_Actor
::hasHighlight() 
{ 
  return false; 
} 

/*!
  \return true if the VTKViewer_Actor is already highlighted
*/
bool
VTKViewer_Actor
::isHighlighted() 
{ 
  return myIsHighlighted; 
}

/*!
  \return true if the VTKViewer_Actor is already preselected
*/
bool
VTKViewer_Actor
::isPreselected() 
{ 
  return myIsPreselected; 
}

/*!
  Set preselection mode
*/
void
VTKViewer_Actor
::SetPreSelected(bool thePreselect) 
{ 
  myIsPreselected = thePreselect;
}

/*!
  Just to update visibility of the highlight devices
*/
void
VTKViewer_Actor
::highlight(bool theIsHighlight)
{
  myIsHighlighted = theIsHighlight; 
}

/*!
 * On/Off representation 2D quadratic element as arked polygon
 */
void VTKViewer_Actor::SetQuadraticArcMode(bool theFlag){
  myGeomFilter->SetQuadraticArcMode(theFlag);
}

/*!
 * Return true if 2D quadratic element displayed as arked polygon
 */
bool VTKViewer_Actor::GetQuadraticArcMode() const{
  return myGeomFilter->GetQuadraticArcMode();
}
/*!
 * Set Max angle for representation 2D quadratic element as arked polygon
 */
void VTKViewer_Actor::SetQuadraticArcAngle(double theMaxAngle){
  myGeomFilter->SetQuadraticArcAngle(theMaxAngle);
}

/*!
 * Return Max angle of the representation 2D quadratic element as arked polygon
 */
double VTKViewer_Actor::GetQuadraticArcAngle() const{
  return myGeomFilter->GetQuadraticArcAngle();
}

/*!
 * Return pointer to the dataset, which used to calculation of the bounding box of the actor.
 * By default it is the input dataset.
 */
vtkDataSet* VTKViewer_Actor::GetHighlightedDataSet() {
  return GetInput();
}



vtkCxxSetObjectMacro(VTKViewer_Actor,PreviewProperty,vtkProperty);
