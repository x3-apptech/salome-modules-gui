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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
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


#include "VTKViewer_Actor.h"

#include "VTKViewer_Transform.h"
#include "VTKViewer_TransformFilter.h"
#include "VTKViewer_PassThroughFilter.h"
#include "VTKViewer_GeometryFilter.h"

// VTK Includes
#include <vtkCell.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

using namespace std;

#if defined __GNUC__
  #if __GNUC__ == 2
    #define __GNUC_2__
  #endif
#endif

int VTKViewer_POINT_SIZE = 5;
int VTKViewer_LINE_WIDTH = 3;


//----------------------------------------------------------------------------
vtkStandardNewMacro(VTKViewer_Actor);


//----------------------------------------------------------------------------
VTKViewer_Actor
::VTKViewer_Actor():
  myIsHighlighted(false),
  myIsPreselected(false),
  myRepresentation(VTK_SURFACE),
  myDisplayMode(1),
  myProperty(vtkProperty::New()),
  PreviewProperty(NULL),
  myIsInfinite(false),
  myIsResolveCoincidentTopology(true),
  myStoreMapping(false),
  myGeomFilter(VTKViewer_GeometryFilter::New()),
  myTransformFilter(VTKViewer_TransformFilter::New())
{
  vtkMapper::GetResolveCoincidentTopologyPolygonOffsetParameters(myPolygonOffsetFactor,
								 myPolygonOffsetUnits);

  for(int i = 0; i < 6; i++)
    myPassFilter.push_back(VTKViewer_PassThroughFilter::New());
}


//----------------------------------------------------------------------------
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


//----------------------------------------------------------------------------
const char* 
VTKViewer_Actor
::getName() 
{ 
  return myName.c_str(); 
}

void
VTKViewer_Actor
::setName(const char* theName)
{
  myName = theName;
}


//----------------------------------------------------------------------------
void
VTKViewer_Actor
::AddToRender(vtkRenderer* theRenderer)
{
  theRenderer->AddActor(this);
}

void 
VTKViewer_Actor
::RemoveFromRender(vtkRenderer* theRenderer)
{
  theRenderer->RemoveActor(this);
}

void
VTKViewer_Actor
::GetChildActors(vtkActorCollection*) 
{}


//----------------------------------------------------------------------------
void
VTKViewer_Actor
::SetTransform(VTKViewer_Transform* theTransform)
{
  myTransformFilter->SetTransform(theTransform);
}


void
VTKViewer_Actor
::SetMapper(vtkMapper* theMapper)
{
  InitPipeLine(theMapper);
}

void
VTKViewer_Actor
::InitPipeLine(vtkMapper* theMapper)
{
  if(theMapper){
    int anId = 0;
    myPassFilter[ anId ]->SetInput( theMapper->GetInput() );
    myPassFilter[ anId + 1]->SetInput( myPassFilter[ anId ]->GetOutput() );
    
    anId++; // 1
    myGeomFilter->SetStoreMapping( myStoreMapping );
    myGeomFilter->SetInput( myPassFilter[ anId ]->GetOutput() );

    anId++; // 2
    myPassFilter[ anId ]->SetInput( myGeomFilter->GetOutput() ); 
    myPassFilter[ anId + 1 ]->SetInput( myPassFilter[ anId ]->GetOutput() );

    anId++; // 3
    myTransformFilter->SetInput( myPassFilter[ anId ]->GetPolyDataOutput() );

    anId++; // 4
    myPassFilter[ anId ]->SetInput( myTransformFilter->GetOutput() );
    myPassFilter[ anId + 1 ]->SetInput( myPassFilter[ anId ]->GetOutput() );

    anId++; // 5
    if(vtkDataSetMapper* aMapper = dynamic_cast<vtkDataSetMapper*>(theMapper)){
      aMapper->SetInput(myPassFilter[anId]->GetOutput());
    }else if(vtkPolyDataMapper* aMapper = dynamic_cast<vtkPolyDataMapper*>(theMapper)){
      aMapper->SetInput(myPassFilter[anId]->GetPolyDataOutput());
    }
  }
  Superclass::SetMapper(theMapper);
}


//----------------------------------------------------------------------------
void
VTKViewer_Actor
::Render(vtkRenderer *ren, vtkMapper* m)
{
  if(myIsResolveCoincidentTopology){
    int aResolveCoincidentTopology = vtkMapper::GetResolveCoincidentTopology();
    float aFactor, aUnit; 
    vtkMapper::GetResolveCoincidentTopologyPolygonOffsetParameters(aFactor,aUnit);
    
    vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
    vtkMapper::SetResolveCoincidentTopologyPolygonOffsetParameters(myPolygonOffsetFactor,
								   myPolygonOffsetUnits);
    Superclass::Render(ren,m);
    
    vtkMapper::SetResolveCoincidentTopologyPolygonOffsetParameters(aFactor,aUnit);
    vtkMapper::SetResolveCoincidentTopology(aResolveCoincidentTopology);
  }else{
    Superclass::Render(ren,m);
  }
}


void
VTKViewer_Actor
::SetResolveCoincidentTopology(bool theIsResolve) 
{
  myIsResolveCoincidentTopology = theIsResolve;
}

void
VTKViewer_Actor
::SetPolygonOffsetParameters(float factor, float units)
{
  myPolygonOffsetFactor = factor;
  myPolygonOffsetUnits = units;
}

void
VTKViewer_Actor
::GetPolygonOffsetParameters(float& factor, float& units)
{
  factor = myPolygonOffsetFactor;
  units = myPolygonOffsetUnits;
}


//----------------------------------------------------------------------------
float
VTKViewer_Actor
::GetShrinkFactor() 
{ 
  return 1.0;
}

bool
VTKViewer_Actor
::IsShrunkable() 
{ 
  return false;
}

bool
VTKViewer_Actor
::IsShrunk() 
{ 
  return false;
}

void
VTKViewer_Actor
::SetShrink() 
{} 

void
VTKViewer_Actor
::UnShrink() 
{}


//----------------------------------------------------------------------------
vtkDataSet* 
VTKViewer_Actor
::GetInput()
{
  return myPassFilter.front()->GetOutput();
}


unsigned long int
VTKViewer_Actor
::GetMTime()
{
  unsigned long mTime = this->Superclass::GetMTime();
  unsigned long time = myTransformFilter->GetMTime();
  mTime = ( time > mTime ? time : mTime );
  if(vtkDataSet *aDataSet = myPassFilter[0]->GetInput()){
    time = aDataSet->GetMTime();
    mTime = ( time > mTime ? time : mTime );
  }
  return mTime;
}


//----------------------------------------------------------------------------
void
VTKViewer_Actor
::SetRepresentation(int theMode) 
{ 
  switch(myRepresentation){
  case VTK_POINTS : 
  case VTK_SURFACE : 
    myProperty->DeepCopy(GetProperty());
  }    
  switch(theMode){
  case VTK_POINTS : 
  case VTK_SURFACE : 
    GetProperty()->DeepCopy(myProperty);
    break;
  default:
    GetProperty()->SetAmbient(1.0);
    GetProperty()->SetDiffuse(0.0);
    GetProperty()->SetSpecular(0.0);
  }
  switch(theMode){
  case 3 : 
    myGeomFilter->SetInside(true);
    myGeomFilter->SetWireframeMode(true);
    GetProperty()->SetRepresentation(VTK_WIREFRAME);
    break;
  case VTK_POINTS : 
    GetProperty()->SetPointSize(VTKViewer_POINT_SIZE);  
    GetProperty()->SetRepresentation(theMode);
    myGeomFilter->SetWireframeMode(false);
    myGeomFilter->SetInside(false);
    break;
  case VTK_WIREFRAME : 
    GetProperty()->SetRepresentation(theMode);
    myGeomFilter->SetWireframeMode(true);
    myGeomFilter->SetInside(false);
    break;
  case VTK_SURFACE : 
    GetProperty()->SetRepresentation(theMode);
    myGeomFilter->SetWireframeMode(false);
    myGeomFilter->SetInside(false);
    break;
  }
  myRepresentation = theMode;
}

int
VTKViewer_Actor
::GetRepresentation()
{ 
  return myRepresentation;
}


//----------------------------------------------------------------------------
int 
VTKViewer_Actor
::GetNodeObjId(int theVtkID)
{ 
  return theVtkID;
}

float* 
VTKViewer_Actor
::GetNodeCoord(int theObjID)
{
  return GetInput()->GetPoint(theObjID);
}

vtkCell* 
VTKViewer_Actor
::GetElemCell(int theObjID)
{
  return GetInput()->GetCell(theObjID);
}

int
VTKViewer_Actor
::GetElemObjId(int theVtkID) 
{ 
  return theVtkID;
}


//=================================================================================
// function : GetObjDimension
// purpose  : Return object dimension.
//            Virtual method shoulb be redifined by derived classes
//=================================================================================
int
VTKViewer_Actor
::GetObjDimension( const int theObjId )
{
  if ( vtkCell* aCell = GetElemCell(theObjId) )
    return aCell->GetCellDimension();
  return 0;
}


void
VTKViewer_Actor
::SetInfinitive(bool theIsInfinite)
{ 
  myIsInfinite = theIsInfinite;
}


bool
VTKViewer_Actor
::IsInfinitive()
{ 
  return myIsInfinite; 
}


float* 
VTKViewer_Actor
::GetBounds()
{
  return Superclass::GetBounds();
}


void
VTKViewer_Actor
::GetBounds(float theBounds[6])
{
  Superclass::GetBounds(theBounds);
}


//----------------------------------------------------------------------------
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
::SetSize( const float ) 
{}


void 
VTKViewer_Actor
::SetCamera( vtkCamera* ) 
{}

//----------------------------------------------------------------------------
void
VTKViewer_Actor
::SetOpacity(float theOpacity)
{ 
  myOpacity = theOpacity;
  GetProperty()->SetOpacity(theOpacity);
}

float
VTKViewer_Actor
::GetOpacity()
{
  return myOpacity;
}


void
VTKViewer_Actor
::SetColor(float r,float g,float b)
{
  GetProperty()->SetColor(r,g,b);
}

void
VTKViewer_Actor
::SetColor(const float theRGB[3])
{ 
  SetColor(theRGB[0],theRGB[1],theRGB[2]);
}

void
VTKViewer_Actor
::GetColor(float& r,float& g,float& b)
{
  float aColor[3];
  GetProperty()->GetColor(aColor);
  r = aColor[0];
  g = aColor[1];
  b = aColor[2];
}


//----------------------------------------------------------------------------
int
VTKViewer_Actor
::getDisplayMode()
{ 
  return myDisplayMode; 
}

void
VTKViewer_Actor
::setDisplayMode(int theMode)
{ 
  SetRepresentation(theMode + 1); 
  myDisplayMode = GetRepresentation() - 1;
}


//----------------------------------------------------------------------------
bool
VTKViewer_Actor
::hasHighlight() 
{ 
  return false; 
} 

bool
VTKViewer_Actor
::isHighlighted() 
{ 
  return myIsHighlighted; 
}

void
VTKViewer_Actor
::SetPreSelected(bool thePreselect) 
{ 
  myIsPreselected = thePreselect;
}


//----------------------------------------------------------------
void
VTKViewer_Actor
::highlight(bool theIsHighlight)
{
  myIsHighlighted = theIsHighlight; 
}

vtkCxxSetObjectMacro(VTKViewer_Actor,PreviewProperty,vtkProperty);
