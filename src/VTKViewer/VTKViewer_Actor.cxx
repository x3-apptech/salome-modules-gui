#include "VTKViewer_Actor.h"
#include "VTKViewer_Transform.h"
#include "VTKViewer_GeometryFilter.h"
#include "VTKViewer_TransformFilter.h"
#include "VTKViewer_PassThroughFilter.h"

// VTK Includes
#include <vtkObjectFactory.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkMapper.h>
#include <vtkPolyData.h>
#include <vtkCell.h>


/*
static void CopyPoints(vtkUnstructuredGrid* theGrid, vtkDataSet *theSourceDataSet){
  vtkPoints *aPoints = vtkPoints::New();
  vtkIdType iEnd = theSourceDataSet->GetNumberOfPoints();
  aPoints->SetNumberOfPoints(iEnd);
  for(vtkIdType i = 0; i < iEnd; i++){
    aPoints->SetPoint(i,theSourceDataSet->GetPoint(i));
  }
  theGrid->SetPoints(aPoints);
  aPoints->Delete();
}
*/



vtkStandardNewMacro(VTKViewer_Actor);


/*!Constructor.Initialize default parameters.*/
VTKViewer_Actor::VTKViewer_Actor(){
  myIsHighlighted = myIsPreselected = false;

  myRepresentation = 1;
  myDisplayMode = myRepresentation - 1;

  myProperty = vtkProperty::New();
  PreviewProperty = NULL;

  myIsInfinite = false;
  myIsResolveCoincidentTopology = true;

  vtkMapper::GetResolveCoincidentTopologyPolygonOffsetParameters(myPolygonOffsetFactor,
								 myPolygonOffsetUnits);
  myStoreMapping = false;
  myGeomFilter = VTKViewer_GeometryFilter::New();

  myTransformFilter = VTKViewer_TransformFilter::New();

  for(int i = 0; i < 6; i++)
    myPassFilter.push_back(VTKViewer_PassThroughFilter::New());

  Visibility = Pickable = true;
}


/*!Destructor.*/
VTKViewer_Actor::~VTKViewer_Actor(){
  SetPreviewProperty(NULL);

  myGeomFilter->UnRegisterAllOutputs(); 
  myGeomFilter->Delete();

  myTransformFilter->UnRegisterAllOutputs();
  myTransformFilter->Delete();

  for(int i = 0, iEnd = myPassFilter.size(); i < iEnd; i++){
    if(myPassFilter[i]){
      myPassFilter[i]->UnRegisterAllOutputs(); 
      myPassFilter[i]->Delete();
    }
  }
  myProperty->Delete();
}

/*!Add VTKViewer_Actor to renderer.
 *\param theRenderer - vtkRenderer
 */
void VTKViewer_Actor::AddToRender(vtkRenderer* theRenderer){
  theRenderer->AddActor(this);
}

/*!Remove VTKViewer_Actor from renderer.
 *\param theRenderer - vtkRenderer
 */
void VTKViewer_Actor::RemoveFromRender(vtkRenderer* theRenderer){
  theRenderer->RemoveActor(this);
}

/*!Add transformation to transform filter.
 *\param theTransform - transformation.
 */
void VTKViewer_Actor::SetTransform(VTKViewer_Transform* theTransform){
  myTransformFilter->SetTransform(theTransform);
}

/*!Set mapper to pipeline.\n
 *Call method for pipeline initialization.
 *\param theMapper - mapper
 */
void VTKViewer_Actor::SetMapper(vtkMapper* theMapper){
  InitPipeLine(theMapper);
}

/*!Initialize sequence of filters for mapper, if \a theMapper is not null.
 *\param theMapper - mapper
 */
void VTKViewer_Actor::InitPipeLine(vtkMapper* theMapper){
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
  vtkLODActor::SetMapper(theMapper);
}

/*!*/
void VTKViewer_Actor::Render(vtkRenderer *ren, vtkMapper* m){
  if(myIsResolveCoincidentTopology){
    int aResolveCoincidentTopology = vtkMapper::GetResolveCoincidentTopology();
    float aFactor, aUnit; 
    vtkMapper::GetResolveCoincidentTopologyPolygonOffsetParameters(aFactor,aUnit);
    
    vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
    vtkMapper::SetResolveCoincidentTopologyPolygonOffsetParameters(myPolygonOffsetFactor,
								   myPolygonOffsetUnits);
    vtkLODActor::Render(ren,m);
    
    vtkMapper::SetResolveCoincidentTopologyPolygonOffsetParameters(aFactor,aUnit);
    vtkMapper::SetResolveCoincidentTopology(aResolveCoincidentTopology);
  }else{
    vtkLODActor::Render(ren,m);
  }
}

/*!Set flag myIsResolveCoincidentTopology to \a theIsResolve.
 *\param theIsResolve - bool flag.
 */
void VTKViewer_Actor::SetResolveCoincidentTopology(bool theIsResolve) {
  myIsResolveCoincidentTopology = theIsResolve;
}

/*!Sets polygon offset factor and polygon offset units.
 *\param factor - float factor
 *\param units  - float units
 */
void VTKViewer_Actor::SetPolygonOffsetParameters(float factor, float units){
  myPolygonOffsetFactor = factor;
  myPolygonOffsetUnits = units;
}

/*!Gets polygon offset factor and polygon offset units.
 *\param factor - output float
 *\param units  - output float
 */
void VTKViewer_Actor::GetPolygonOffsetParameters(float& factor, float& units){
  factor = myPolygonOffsetFactor;
  units = myPolygonOffsetUnits;
}


/*!Get input data set.
 *\retval vtkDataSet pointer.
 */
vtkDataSet* VTKViewer_Actor::GetInput(){
  return myPassFilter.front()->GetOutput();
}

/*!Get modification time.
 *\retval time - unsigned long.
 */
unsigned long int VTKViewer_Actor::GetMTime(){
  unsigned long mTime = this->Superclass::GetMTime();
  unsigned long time = myTransformFilter->GetMTime();
  mTime = ( time > mTime ? time : mTime );
  if(vtkDataSet *aDataSet = myPassFilter[0]->GetInput()){
    time = aDataSet->GetMTime();
    mTime = ( time > mTime ? time : mTime );
  }
  return mTime;
}

/*!Set representation mode.
 *\param theMode - int.
 */
void VTKViewer_Actor::SetRepresentation(int theMode) { 
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
    break;
    GetProperty()->SetAmbient(1.0);
    GetProperty()->SetDiffuse(0.0);
    GetProperty()->SetSpecular(0.0);
  }
  switch(theMode){
  case 3 : 
    myGeomFilter->SetInside(true);
    GetProperty()->SetRepresentation(1);
    break;
  case VTK_POINTS : 
    GetProperty()->SetPointSize(VTKViewer_POINT_SIZE);  
  default :
    GetProperty()->SetRepresentation(theMode);
    myGeomFilter->SetInside(false);
  }
  myRepresentation = theMode;
}

/*!Get representation.
 *\retval representation mode.
 */
int VTKViewer_Actor::GetRepresentation(){ 
  return myRepresentation;
}

/*!Get VTK cell by object ID.
 *\param theObjID - object ID.
 *\retval vtkCell pointer.
 */
vtkCell* VTKViewer_Actor::GetElemCell(int theObjID){
  return GetInput()->GetCell(theObjID);
}

/*!Get node coordinates by node ID.
 *\param theObjID - node ID.
 *\retval float array of coordinates.
 * \li array[0] - X coordinate.
 * \li array[1] - Y coordinate.
 * \li array[2] - Z coordinate.
 */
float* VTKViewer_Actor::GetNodeCoord(int theObjID){
  return GetInput()->GetPoint(theObjID);
}


//=================================================================================
// function : GetObjDimension
/*! purpose  : Return object dimension.\n
 *            Virtual method shoulb be redifined by derived classes
 *\param theObjId - object ID.
 */
//=================================================================================
int VTKViewer_Actor::GetObjDimension( const int theObjId )
{
  if ( vtkCell* aCell = GetElemCell(theObjId) )
    return aCell->GetCellDimension();
  return 0;
}

/*!Get infinite flag*/
bool VTKViewer_Actor::IsInfinitive(){ 
  return myIsInfinite; 
}

/*!Set property - opacity.
 *\param theOpacity - new apacity
 */
void VTKViewer_Actor::SetOpacity(float theOpacity){ 
  myOpacity = theOpacity;
  GetProperty()->SetOpacity(theOpacity);
}

/*!Get property - opacity.
 *\retval float value.
 */
float VTKViewer_Actor::GetOpacity(){
  return myOpacity;
}

/*!Set property - color
 *\param r - float Red value
 *\param g - float Green value
 *\param b - float Blue value
 */
void VTKViewer_Actor::SetColor(float r,float g,float b){
  GetProperty()->SetColor(r,g,b);
}

/*!Get property - color
 *\param r - output float Red value
 *\param g - output float Green value
 *\param b - output float Blue value
 */
void VTKViewer_Actor::GetColor(float& r,float& g,float& b){
  float aColor[3];
  GetProperty()->GetColor(aColor);
  r = aColor[0];
  g = aColor[1];
  b = aColor[2];
}

/*!Get display mode.
 *\retval int value
 */
int VTKViewer_Actor::getDisplayMode(){ 
  return myDisplayMode; 
}

/*!Set display mode
 *\param theMode - integer value.
 */
void VTKViewer_Actor::setDisplayMode(int theMode){ 
  SetRepresentation(theMode+1); 
  myDisplayMode = GetRepresentation() - 1;
}

