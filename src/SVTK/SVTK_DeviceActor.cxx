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

//  SVTK OBJECT : interactive object for SVTK visualization
//  File   : SVTK_DeviceActor.cxx
//  Author : 

#include "SVTK_DeviceActor.h"

#include "VTKViewer_Transform.h"
#include "VTKViewer_TransformFilter.h"
#include "VTKViewer_GeometryFilter.h"

// VTK Includes
#include <vtkObjectFactory.h>
#include <vtkShrinkFilter.h>
#include <vtkFeatureEdges.h>

#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include <VTKViewer_PolyDataMapper.h>
#include <VTKViewer_DataSetMapper.h>

#include <vtkPassThroughFilter.h>

vtkStandardNewMacro(SVTK_DeviceActor);

/*!
  Constructor
*/
SVTK_DeviceActor
::SVTK_DeviceActor()
{
  myIsShrunk = false;
  myIsShrinkable = true;

  myIsFeatureEdgesAllowed = false;
  myIsFeatureEdgesEnabled = false;

  myIsShaded = true;
  myProperty = vtkProperty::New();
  myRepresentation = VTKViewer::Representation::Surface;

  myIsResolveCoincidentTopology = true;
  vtkMapper::GetResolveCoincidentTopologyPolygonOffsetParameters(myPolygonOffsetFactor,
                                                                 myPolygonOffsetUnits);

  myMapper = VTKViewer_DataSetMapper::New();

  myShrinkFilter = vtkShrinkFilter::New();

  myFeatureEdges = vtkFeatureEdges::New();

  myGeomFilter = VTKViewer_GeometryFilter::New();

  myTransformFilter = VTKViewer_TransformFilter::New();

  for(int i = 0; i < 6; i++)
    myPassFilter.push_back(vtkPassThroughFilter::New());
}

/*!
  Destructor
*/
SVTK_DeviceActor
::~SVTK_DeviceActor()
{
  myMapper->Delete();

  myProperty->Delete();

  myGeomFilter->Delete();

  myTransformFilter->Delete();

  myShrinkFilter->Delete();

  myFeatureEdges->Delete();

  for(int i = 0, iEnd = myPassFilter.size(); i < iEnd; i++)
    myPassFilter[i]->Delete();
}

/*!
  To insert some additional filters and then sets the given vtkMapper
*/
void
SVTK_DeviceActor
::SetMapper(vtkMapper* theMapper)
{
  InitPipeLine(theMapper);
}

/*!
  To initialize internal pipeline
*/
void
SVTK_DeviceActor
::InitPipeLine(vtkMapper* theMapper)
{
  if(theMapper){
    int anId = 0;
    myPassFilter[ anId ]->SetInputConnection( theMapper->GetInputConnection( 0, 0 ) );
    myPassFilter[ anId + 1]->SetInputConnection( myPassFilter[ anId ]->GetOutputPort() );
    
    anId++; // 1
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
  Allows to get initial vtkDataSet
*/
vtkDataSet* 
SVTK_DeviceActor
::GetInput()
{
  return myPassFilter.front()->GetOutput();
}

/*!
  Allows to set initial vtkDataSet
*/
void
SVTK_DeviceActor
::SetInputData(vtkDataSet* theDataSet)
{
  myMapper->SetInputData(theDataSet);
  InitPipeLine(myMapper);
}

/*!
  Allows to set initial vtkDataSet
*/
void
SVTK_DeviceActor
::SetInputConnection(vtkAlgorithmOutput* theAlgorithmOutput)
{
  myMapper->SetInputConnection(theAlgorithmOutput);
  InitPipeLine(myMapper);
}

/*!
  To provide VTK to Object and backward mapping
*/
void
SVTK_DeviceActor::
SetStoreMapping(bool theStoreMapping)
{
  myGeomFilter->SetStoreMapping(theStoreMapping);
}


/*!
  \return time of modification
*/
vtkMTimeType
SVTK_DeviceActor
::GetMTime()
{
  vtkMTimeType mTime = this->Superclass::GetMTime();

  mTime = std::max(mTime,myGeomFilter->GetMTime());

  mTime = std::max(mTime,myTransformFilter->GetMTime());

  if(myIsShrunk)
    mTime = std::max(mTime,myShrinkFilter->GetMTime());

  if(myIsFeatureEdgesEnabled)
    mTime = std::max(mTime,myFeatureEdges->GetMTime());

  for(int i = 0, iEnd = myPassFilter.size(); i < iEnd; i++)
    std::max(mTime,myPassFilter[i]->GetMTime());

  return mTime;
}

/*!
  Apply a view transformation
  \param theTransform - transformation
*/
void 
SVTK_DeviceActor
::SetTransform(VTKViewer_Transform* theTransform)
{
  myTransformFilter->SetTransform(theTransform);
}

/*!
  \return true if actor is shrinkable
*/
bool
SVTK_DeviceActor
::IsShrunkable() 
{ 
  return myIsShrinkable;
}

/*!
  Changes shrinkable state of actor
  theIsShrinkable - new shrinkable state
*/  
void
SVTK_DeviceActor
::SetShrinkable(bool theIsShrinkable) 
{ 
  myIsShrinkable = theIsShrinkable;
}
  
/*!
  \return true if actor is shrunkable
*/
bool
SVTK_DeviceActor
::IsShrunk() 
{ 
  return myIsShrunk;
}

/*!
  Insert shrink filter into pipeline
*/
void
SVTK_DeviceActor
::SetShrink() 
{
  if ( !myIsShrinkable ) 
    return;
  
  if ( vtkAlgorithmOutput* anOutput = myPassFilter[ 0 ]->GetOutputPort() )
  {     
    myPassFilter[ 0 ]->Update();
    if ( vtkDataSet* aDataSet = myPassFilter[ 0 ]->GetOutput() )
    {
      int numCells=aDataSet->GetNumberOfCells();
      int numPts = aDataSet->GetNumberOfPoints();
      //It's impossible to use to apply "shrink" for "empty" dataset
      if (numCells < 1 || numPts < 1)
        return;
    }
    myShrinkFilter->SetInputConnection( anOutput );
    myPassFilter[ 1 ]->SetInputConnection( myShrinkFilter->GetOutputPort() );
    myIsShrunk = true;
  }
}

/*!
  Remove shrink filter from pipeline
*/
void 
SVTK_DeviceActor
::UnShrink() 
{
  if ( !myIsShrunk ) return;
  if ( vtkAlgorithmOutput* anOutput = myPassFilter[ 0 ]->GetOutputPort() )
  {    
    myPassFilter[ 1 ]->SetInputConnection( anOutput );
    myIsShrunk = false;
  }
}

/*!
  \return shrink factor
*/
double
SVTK_DeviceActor
::GetShrinkFactor()
{
  return myShrinkFilter->GetShrinkFactor();
}

/*!
  Changes shrink factor
  \param theValue - new shrink factor
*/
void 
SVTK_DeviceActor
::SetShrinkFactor(double theValue)
{
  myShrinkFilter->SetShrinkFactor(theValue);
}

/*!
  \return true if feature edges are allowed for this actor
*/
bool
SVTK_DeviceActor
::IsFeatureEdgesAllowed() 
{ 
  return myIsFeatureEdgesAllowed;
}

/*!
  Allows feature edges for this actor on or off
  \param theIsFeatureEdgesAllowed - flag which allows feature edges for this actor on or off
*/
void
SVTK_DeviceActor
::SetFeatureEdgesAllowed(bool theIsFeatureEdgesAllowed)
{
  myIsFeatureEdgesAllowed = theIsFeatureEdgesAllowed;
}

/*!
  \return true if feature edges are enabled
*/
bool
SVTK_DeviceActor
::IsFeatureEdgesEnabled()
{
  return myIsFeatureEdgesEnabled;
}

/*!
  Enables feature edges on or off
  \param theIsFeatureEdgesEnabled - flag which enables feature edges on or off
*/
void
SVTK_DeviceActor
::SetFeatureEdgesEnabled(bool theIsFeatureEdgesEnabled)
{
  if ( !myIsFeatureEdgesAllowed || myIsFeatureEdgesEnabled == theIsFeatureEdgesEnabled ) 
    return;

  if ( vtkAlgorithmOutput* aPolyData = myPassFilter[ 2 ]->GetOutputPort() )
  {
    if( theIsFeatureEdgesEnabled )
    {
      myPassFilter[ 2 ]->Update();
      myFeatureEdges->SetInputConnection( aPolyData );
      myPassFilter[ 3 ]->SetInputConnection( myFeatureEdges->GetOutputPort() );
      myIsFeatureEdgesEnabled = true;
    }
    else
    {
      myPassFilter[3]->SetInputConnection( aPolyData );
      myIsFeatureEdgesEnabled = false;
    }
    myIsFeatureEdgesEnabled = theIsFeatureEdgesEnabled;
  }
}

/*!
  \return angle of feature edges' filter
*/
double
SVTK_DeviceActor
::GetFeatureEdgesAngle()
{
  return myFeatureEdges->GetFeatureAngle();
}

/*!
  Sets angle of feature edges' filter
  \param theAngle angle of feature edges' filter
*/
void
SVTK_DeviceActor
::SetFeatureEdgesAngle(double theAngle)
{
  myFeatureEdges->SetFeatureAngle(theAngle);
}

/*!
  Gets information about kinds of edges which are displayed by feature edges' filter
  \param theIsFeatureEdges flag which shows whether feature edges are displayed
  \param theIsBoundaryEdges flag which shows whether boundary edges are displayed
  \param theIsManifoldEdges flag which shows whether manifold edges are displayed
  \param theIsNonManifoldEdges flag which shows whether non-manifold edges are displayed
*/
void
SVTK_DeviceActor
::GetFeatureEdgesFlags(bool& theIsFeatureEdges,
                       bool& theIsBoundaryEdges,
                       bool& theIsManifoldEdges,
                       bool& theIsNonManifoldEdges)
{
  theIsFeatureEdges = myFeatureEdges->GetFeatureEdges();
  theIsBoundaryEdges = myFeatureEdges->GetBoundaryEdges();
  theIsManifoldEdges = myFeatureEdges->GetManifoldEdges();
  theIsNonManifoldEdges = myFeatureEdges->GetNonManifoldEdges();
}

/*!
  Sets different kinds of edges to be displayed by feature edges' filter
  \param theIsFeatureEdges flag which displays feature edges
  \param theIsBoundaryEdges flag which displays boundary edges
  \param theIsManifoldEdges flag which displays manifold edges
  \param theIsNonManifoldEdges flag which displays non-manifold edges
*/
void
SVTK_DeviceActor
::SetFeatureEdgesFlags(bool theIsFeatureEdges,
                       bool theIsBoundaryEdges,
                       bool theIsManifoldEdges,
                       bool theIsNonManifoldEdges)
{
  myFeatureEdges->SetFeatureEdges(theIsFeatureEdges);
  myFeatureEdges->SetBoundaryEdges(theIsBoundaryEdges);
  myFeatureEdges->SetManifoldEdges(theIsManifoldEdges);
  myFeatureEdges->SetNonManifoldEdges(theIsNonManifoldEdges);
}

/*!
  \return feature edges' coloring flag
*/
bool
SVTK_DeviceActor
::GetFeatureEdgesColoring()
{
  return myFeatureEdges->GetColoring();
}

/*!
  Sets feature edges' coloring flag
  \param theIsColoring feature edges' coloring flag
*/
void
SVTK_DeviceActor
::SetFeatureEdgesColoring(bool theIsColoring)
{
  myFeatureEdges->SetColoring(theIsColoring);
}

/*!
  Set representation (VTK_SURFACE, VTK_POINTS, VTK_WIREFRAME and so on)
  param theMode - new mode
*/
void
SVTK_DeviceActor
::SetRepresentation(VTKViewer::Representation::Type theMode)
{ 
  using namespace VTKViewer::Representation;
  if(IsShaded()){
    switch(myRepresentation){
    case Points : 
    case Surface : 
    case SurfaceWithEdges :
      myProperty->SetAmbient(GetProperty()->GetAmbient());
      myProperty->SetDiffuse(GetProperty()->GetDiffuse());
      myProperty->SetSpecular(GetProperty()->GetSpecular());
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
  }

  switch(theMode){
  case Insideframe : 
    myGeomFilter->SetInside(true);
    myGeomFilter->SetWireframeMode(true);
    GetProperty()->SetRepresentation(VTK_WIREFRAME);
    break;
  case Points : 
    GetProperty()->SetPointSize(GetDefaultPointSize());  
    GetProperty()->SetRepresentation(VTK_POINTS);
    myGeomFilter->SetWireframeMode(false);
    myGeomFilter->SetInside(false);
    break;
  case Wireframe : 
    GetProperty()->SetRepresentation(VTK_WIREFRAME);
    myGeomFilter->SetWireframeMode(true);
    myGeomFilter->SetInside(false);
    break;
  case Surface : 
  case SurfaceWithEdges :
    GetProperty()->SetRepresentation(VTK_SURFACE);
    myGeomFilter->SetWireframeMode(false);
    myGeomFilter->SetInside(false);
    break;
  }

  SetMarkerEnabled( theMode == Points );

  myRepresentation = theMode;
}

/*!
  \return current representation mode
*/
VTKViewer::Representation::Type 
SVTK_DeviceActor
::GetRepresentation()
{
  return myRepresentation;
}

/*!
  \return default point size
*/
double
SVTK_DeviceActor
::GetDefaultPointSize()
{
  return 5;
}

/*!
  \return default line width
*/
double
SVTK_DeviceActor
::GetDefaultLineWidth()
{
  return 3;
}

/*!
  \return true if actor is shaded
*/
bool
SVTK_DeviceActor
::IsShaded()
{
  return myIsShaded;
}

/*!
  Sets shaded state of actor
  \param theShaded - new shaded state
*/
void
SVTK_DeviceActor
::SetShaded(bool theShaded)
{
  myIsShaded = theShaded;
}

/*!
  Maps VTK index of a node to corresponding object index
*/
int
SVTK_DeviceActor
::GetNodeObjId(int theVtkID)
{
  return theVtkID;
}

/*!
  Get coordinates of a node for given object index
*/
double* 
SVTK_DeviceActor
::GetNodeCoord(int theObjID)
{
  return GetInput()->GetPoint(theObjID);
}


/*!
  Get corresponding #vtkCell for given object index
*/
vtkCell* 
SVTK_DeviceActor
::GetElemCell(int theObjID)
{
  return GetInput()->GetCell(theObjID);
}

/*!
  Maps VTK index of a cell to corresponding object index
*/
int
SVTK_DeviceActor
::GetElemObjId(int theVtkID) 
{ 
  return theVtkID;
}

/*!
  Renders actor
*/
void
SVTK_DeviceActor
::Render(vtkRenderer *ren, vtkMapper* m)
{
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
  Set polygon offset parameters
  \param factor, units  - Opengl polygon offset parameters
*/
void
SVTK_DeviceActor
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
SVTK_DeviceActor
::GetPolygonOffsetParameters(double& factor, 
                             double& units)
{
  factor = myPolygonOffsetFactor;
  units = myPolygonOffsetUnits;
}

VTKViewer_DataSetMapper* SVTK_DeviceActor::GetDataSetMapper()
{
  return myMapper;
}

/*!
 * On/Off representation 2D quadratic element as arked polygon
 */
void SVTK_DeviceActor::SetQuadraticArcMode(bool theFlag){
  myGeomFilter->SetQuadraticArcMode(theFlag);
}

/*!
 * Return true if 2D quadratic element displayed as arked polygon
 */
bool SVTK_DeviceActor::GetQuadraticArcMode(){
  return myGeomFilter->GetQuadraticArcMode();
}
/*!
 * Set Max angle for representation 2D quadratic element as arked polygon
 */
void SVTK_DeviceActor::SetQuadraticArcAngle(double theMaxAngle){
  myGeomFilter->SetQuadraticArcAngle(theMaxAngle);
}

/*!
 * Return Max angle of the representation 2D quadratic element as arked polygon
 */
double SVTK_DeviceActor::GetQuadraticArcAngle(){
  return myGeomFilter->GetQuadraticArcAngle();
}

/*!
 * Set ball drawing enabled
 * \param theBallEnabled flag to enable/disable balls
 */
void SVTK_DeviceActor::SetBallEnabled( bool theBallEnabled ) {
  myMapper->SetBallEnabled( theBallEnabled );
}
/*!
 * Set ball scale factor
 * \param theBallScale double value a scale factor of ball element
 */
void SVTK_DeviceActor::SetBallScale( double theBallScale ) {
  myMapper->SetBallScale( theBallScale );
}

/*!
 * Set point marker enabled
 * \param theMarkerEnabled flag to enable/disable point marker
 */
void SVTK_DeviceActor::SetMarkerEnabled( bool theMarkerEnabled )
{
  myMapper->SetMarkerEnabled( theMarkerEnabled );
}

/*!
 * Set standard point marker
 * \param theMarkerType type of the marker
 * \param theMarkerScale scale of the marker
 */
void SVTK_DeviceActor::SetMarkerStd( VTK::MarkerType theMarkerType, VTK::MarkerScale theMarkerScale )
{
  myMapper->SetMarkerStd( theMarkerType, theMarkerScale );
}

/*!
 * Set custom point marker
 * \param theMarkerId id of the marker texture
 * \param theMarkerTexture marker texture
 */
void SVTK_DeviceActor::SetMarkerTexture( int theMarkerId, VTK::MarkerTexture theMarkerTexture )
{
  myMapper->SetMarkerTexture( theMarkerId, theMarkerTexture );
}

/*!
 * Get type of the point marker
 * \return type of the point marker
 */
VTK::MarkerType SVTK_DeviceActor::GetMarkerType()
{
  return myMapper->GetMarkerType();
}

/*!
  Get scale of the point marker
  \return scale of the point marker
*/
VTK::MarkerScale SVTK_DeviceActor::GetMarkerScale()
{
  return myMapper->GetMarkerScale();
}

/*!
 * Get texture identifier of the point marker
 * \return texture identifier of the point marker
 */
int SVTK_DeviceActor::GetMarkerTexture()
{
  return myMapper->GetMarkerTexture();
}

void SVTK_DeviceActor::SetCoincident3DAllowed(bool theFlag) {
  myGeomFilter->SetAppendCoincident3D(theFlag);
}

bool SVTK_DeviceActor::IsCoincident3DAllowed() const {
  return myGeomFilter->GetAppendCoincident3D();
}

void
SVTK_DeviceActor
::SetResolveCoincidentTopology(bool theIsResolve)
{
  myIsResolveCoincidentTopology = theIsResolve;
}
