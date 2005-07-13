#include "VTKViewer_RenderWindowInteractor.h"
#include "VTKViewer_RenderWindow.h"
#include "VTKViewer_InteractorStyle.h"
#include "SUIT_ViewModel.h"
#include "VTKViewer_ViewWindow.h"

//#include "SUIT_Application.h"
//#include "SUIT_Desktop.h"

//#include "SALOME_Selection.h"
#include "VTKViewer_Actor.h"
#include "VTKViewer_Algorithm.h"
#include "VTKViewer_Functor.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <math.h>

// VTK Includes
#include <vtkAssemblyNode.h>
#include <vtkActor.h>
#include <vtkInteractorStyle.h>
#include <vtkObjectFactory.h>
#include <vtkPicker.h>
#include <vtkCellPicker.h>
#include <vtkPointPicker.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkDataSet.h>
#include <vtkMaskPoints.h>
#include <vtkVertex.h>
#include <vtkRendererCollection.h>
#include <vtkPolyDataWriter.h>

// QT Includes
#include <qkeycode.h>

//****************************************************************
VTKViewer_RenderWindowInteractor* VTKViewer_RenderWindowInteractor::New() 
{
  vtkObject *ret = vtkObjectFactory::CreateInstance("VTKViewer_RenderWindowInteractor") ;
  if( ret ) {
    return dynamic_cast<VTKViewer_RenderWindowInteractor *>(ret) ;
  }
  return new VTKViewer_RenderWindowInteractor;
}

//****************************************************************
VTKViewer_RenderWindowInteractor::VTKViewer_RenderWindowInteractor() 
{
  this->Enabled = 0 ;
  this->mTimer = new QTimer( this ) ;
  myDisplayMode = 0;

  myBasicPicker = vtkPicker::New();
  myCellPicker = vtkCellPicker::New();
  myPointPicker = vtkPointPicker::New();

  myCellActor = VTKViewer_Actor::New(); 
  myCellActor->PickableOff();
  myCellActor->GetProperty()->SetColor(1,1,0);
  myCellActor->GetProperty()->SetLineWidth(5);
  myCellActor->GetProperty()->SetRepresentationToSurface();

  myEdgeActor = VTKViewer_Actor::New(); 
  myEdgeActor->PickableOff();
  myEdgeActor->GetProperty()->SetColor(1,0,0);
  myEdgeActor->GetProperty()->SetLineWidth(5);
  myEdgeActor->GetProperty()->SetRepresentationToWireframe();

  myPointActor = VTKViewer_Actor::New(); 
  myPointActor->PickableOff();
  myPointActor->GetProperty()->SetColor(1,1,0);
  myPointActor->GetProperty()->SetPointSize(5);
  myPointActor->GetProperty()->SetRepresentationToPoints();

  connect(mTimer, SIGNAL(timeout()), this, SLOT(TimerFunc())) ;
}

//****************************************************************
VTKViewer_RenderWindowInteractor::~VTKViewer_RenderWindowInteractor() 
{
  delete mTimer ;

  if ( GetRenderWindow() ) {
    myViewWnd->RemoveActor(myCellActor);
    myViewWnd->RemoveActor(myEdgeActor);
    myViewWnd->RemoveActor(myPointActor);
  }

  myCellActor->Delete();
  myEdgeActor->Delete();
  myPointActor->Delete();

  myBasicPicker->Delete();
  myCellPicker->Delete();
  myPointPicker->Delete();
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::PrintSelf(ostream& os, vtkIndent indent) 
{
  vtkRenderWindowInteractor::PrintSelf(os, indent) ;
  //
  // :NOTE: Fri Apr 21 21:51:05 2000 Pagey
  // QGL specific stuff goes here. One should add output 
  // lines here if any protected members are added to
  // the class. 
  //
}

//****************************************************************
// We never allow the VTKViewer_RenderWindowInteractor to control 
// the event loop. The application always has the control. 
//
void VTKViewer_RenderWindowInteractor::Initialize()
{
  //
  // We cannot do much unless there is a render window 
  // associated with this interactor. 
  //
  if( ! RenderWindow ) {
    vtkErrorMacro(<< "VTKViewer_RenderWindowInteractor::Initialize(): No render window attached!") ;
    return ;
  }
  
  //
  // We cannot hand a render window which is not a VTKViewer_RenderWindow. 
  // One way to force this is to use dynamic_cast and hope that 
  // it works. If the dynamic_cast does not work, we flag an error
  // and get the hell out.
  //
  vtkRenderWindow *my_render_win = dynamic_cast<vtkRenderWindow *>(RenderWindow) ;
  if( !my_render_win ) {
    vtkErrorMacro(<< "VTKViewer_RenderWindowInteractor::Initialize() can only handle VTKViewer_RenderWindow.") ;
    return ;
  }
 
  //
  // If the render window has zero size, then set it to a default 
  // value of 300x300.
  // 
  int* aSize = my_render_win->GetSize();
  this->Size[0] = ((aSize[0] > 0) ? aSize[0] : 300);
  this->Size[1] = ((aSize[1] > 0) ? aSize[1] : 300);

  this->SetPicker(myBasicPicker);

  SetSelectionTolerance();

  //
  // Enable the interactor. 
  //
  this->Enable() ;
  
  //
  // Start the rendering of the window. 
  //
  my_render_win->Start() ;
  
  //
  // The interactor has been initialized.
  //
  this->Initialized = 1 ;

  return ;
}


//----------------------------------------------------------------------------
void VTKViewer_RenderWindowInteractor::setViewWindow(VTKViewer_ViewWindow* theViewWnd){
  myViewWnd = theViewWnd;

  if ( myViewWnd ) {
    myViewWnd->InsertActor(myCellActor);
    myViewWnd->InsertActor(myEdgeActor);
    myViewWnd->InsertActor(myPointActor);
  }
}

//----------------------------------------------------------------------------
void VTKViewer_RenderWindowInteractor::MoveInternalActors()
{
  myViewWnd->MoveActor(myCellActor);
  myViewWnd->MoveActor(myEdgeActor);
  myViewWnd->MoveActor(myPointActor);
}

//----------------------------------------------------------------------------
void VTKViewer_RenderWindowInteractor::SetInteractorStyle(vtkInteractorObserver *theInteractor){
  myInteractorStyle = dynamic_cast<VTKViewer_InteractorStyle*>(theInteractor);
  vtkRenderWindowInteractor::SetInteractorStyle(theInteractor);
}


/*void VTKViewer_RenderWindowInteractor::SetSelectionMode(Selection_Mode theMode)
{
  myCellActor->SetVisibility(false);
  myEdgeActor->SetVisibility(false);
  myPointActor->SetVisibility(false);

  switch(theMode){
  case ActorSelection:
    this->SetPicker(myBasicPicker);
    break;
  case NodeSelection:
    this->SetPicker(myPointPicker);
    break;
  case CellSelection:
  case EdgeSelection:
  case FaceSelection:
  case VolumeSelection:
  case EdgeOfCellSelection:
    this->SetPicker(myCellPicker);
    break;
  }

  myInteractorStyle->OnSelectionModeChanged();
}*/

//****************************************************************
void VTKViewer_RenderWindowInteractor::SetSelectionProp(const double& theRed, const double& theGreen, 
                                                        const double& theBlue, const int& theWidth) 
{
  myCellActor->GetProperty()->SetColor(theRed, theGreen, theBlue);
  myCellActor->GetProperty()->SetLineWidth(theWidth);

  myPointActor->GetProperty()->SetColor(theRed, theGreen, theBlue);
  myPointActor->GetProperty()->SetPointSize(theWidth);
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::SetSelectionTolerance(const double& theTolNodes, const double& theTolItems)
{
  myTolNodes = theTolNodes;
  myTolItems = theTolItems;

  myBasicPicker->SetTolerance(myTolItems);
  myCellPicker->SetTolerance(myTolItems);
  myPointPicker->SetTolerance(myTolNodes);

}

//****************************************************************
void VTKViewer_RenderWindowInteractor::Enable()
{
  //
  // Do not need to do anything if already enabled.
  //
  if( this->Enabled ) {
    return ;
  }
  
  this->Enabled = 1 ;
  this->Modified() ;
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::Disable()
{
  if( ! this->Enabled ) {
    return ;
  }
  
  this->Enabled = 0 ;
  this->Modified() ;
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::Start()
{
  //
  // We do not allow this interactor to control the 
  // event loop. Only the QtApplication objects are
  // allowed to do that. 
  //
  vtkErrorMacro(<<"VTKViewer_RenderWindowInteractor::Start() not allowed to start event loop.") ;
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::UpdateSize(int w, int h)
{
  // if the size changed send this on to the RenderWindow
  if ((w != this->Size[0])||(h != this->Size[1])) {
    this->Size[0] = w;
    this->Size[1] = h;
    this->RenderWindow->SetSize(w,h);
  }
}

//****************************************************************
int VTKViewer_RenderWindowInteractor::CreateTimer(int vtkNotUsed(timertype))
{
  //
  // Start a one-shot timer for 10ms. 
  //
  mTimer->start(10, TRUE) ;
  return 1 ;
}

//****************************************************************
int VTKViewer_RenderWindowInteractor::DestroyTimer(void)
{
  //
  // :TRICKY: Tue May  2 00:17:32 2000 Pagey
  //
  // QTimer will automatically expire after 10ms. So 
  // we do not need to do anything here. In fact, we 
  // should not even Stop() the QTimer here because doing 
  // this will skip some of the processing that the TimerFunc()
  // does and will result in undesirable effects. For 
  // example, this will result in vtkLODActor to leave
  // the models in low-res mode after the mouse stops
  // moving. 
  //
  return 1 ;
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::TimerFunc()
{
  if( ! this->Enabled ) {
    return ;
  }
  
  ((vtkInteractorStyle*)this->InteractorStyle)->OnTimer() ;
  emit RenderWindowModified() ;
}

void VTKViewer_RenderWindowInteractor::MouseMove(QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnMouseMove(0, 0, event->x(), event->y()/*this->Size[1] - event->y() - 1*/) ;
  if (myInteractorStyle->needsRedrawing() )
    emit RenderWindowModified() ; 
}

void VTKViewer_RenderWindowInteractor::LeftButtonPressed(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnLeftButtonDown((event->state() & ControlButton), 
				      (event->state() & ShiftButton), 
				      event->x(), event->y());
}

void VTKViewer_RenderWindowInteractor::LeftButtonReleased(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnLeftButtonUp( (event->state() & ControlButton), 
				     (event->state() & ShiftButton), 
				     event->x(), event->y() ) ;
}

void VTKViewer_RenderWindowInteractor::MiddleButtonPressed(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnMiddleButtonDown((event->state() & ControlButton), 
					(event->state() & ShiftButton), 
					event->x(), event->y() ) ;
}

void VTKViewer_RenderWindowInteractor::MiddleButtonReleased(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnMiddleButtonUp( (event->state() & ControlButton), 
				       (event->state() & ShiftButton), 
				       event->x(), event->y() ) ;
}

void VTKViewer_RenderWindowInteractor::RightButtonPressed(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnRightButtonDown( (event->state() & ControlButton), 
					(event->state() & ShiftButton), 
					event->x(), event->y() ) ;
}

void VTKViewer_RenderWindowInteractor::RightButtonReleased(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  bool isOperation = myInteractorStyle->CurrentState() != VTK_INTERACTOR_STYLE_CAMERA_NONE;
  myInteractorStyle->OnRightButtonUp( (event->state() & ControlButton),
				      (event->state() & ShiftButton),
				      event->x(), event->y() );
  if ( !isOperation )
  {
    QContextMenuEvent aEvent( QContextMenuEvent::Mouse,
                              event->pos(), event->globalPos(),
                              event->state() );
    emit contextMenuRequested( &aEvent );
  }
}

void VTKViewer_RenderWindowInteractor::ButtonPressed(const QMouseEvent *event) {
  return ;
}

void VTKViewer_RenderWindowInteractor::ButtonReleased(const QMouseEvent *event) {
  return ;
}


int VTKViewer_RenderWindowInteractor::GetDisplayMode() {
  return myDisplayMode;
}

void VTKViewer_RenderWindowInteractor::SetDisplayMode(int theMode) {
  if(theMode == 0)
    ChangeRepresentationToWireframe();
  else
    ChangeRepresentationToSurface();
  myDisplayMode = theMode;
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::ChangeRepresentationToWireframe()
// change all actors to wireframe
{
  ChangeRepresentationToWireframe(GetRenderer()->GetActors());
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::ChangeRepresentationToSurface()
{
  ChangeRepresentationToSurface(GetRenderer()->GetActors());
}


void VTKViewer_RenderWindowInteractor::ChangeRepresentationToWireframe(vtkActorCollection* theCollection)
{
  using namespace VTK;
  ForEach<VTKViewer_Actor>(theCollection,
			TSetFunction<VTKViewer_Actor,int>
			(&VTKViewer_Actor::setDisplayMode,0));
  emit RenderWindowModified();
}

void VTKViewer_RenderWindowInteractor::ChangeRepresentationToSurface(vtkActorCollection* theCollection)
{
  using namespace VTK;
  ForEach<VTKViewer_Actor>(theCollection,
			TSetFunction<VTKViewer_Actor,int>
			(&VTKViewer_Actor::setDisplayMode,1));
  emit RenderWindowModified();
}

//****************************************************************
vtkRenderer* VTKViewer_RenderWindowInteractor::GetRenderer()
{
  vtkRendererCollection * theRenderers =  this->RenderWindow->GetRenderers();
  theRenderers->InitTraversal();
  return theRenderers->GetNextItem();
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::EraseAll()
{
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::DisplayAll()
{
  using namespace VTK;
  vtkActorCollection* aCollection = GetRenderer()->GetActors();
  ForEach<VTKViewer_Actor>(aCollection,TSetVisibility<VTKViewer_Actor>(true));

  emit RenderWindowModified() ;
}

//****************************************************************
void VTKViewer_RenderWindowInteractor::Erase( VTKViewer_Actor* SActor, bool update)
{
}

void VTKViewer_RenderWindowInteractor::Remove( VTKViewer_Actor* SActor, bool updateViewer )
{
  if ( SActor != 0 )
  {
    GetRenderer()->RemoveProp( SActor );
    if ( updateViewer )
      emit RenderWindowModified();
  }
}

void VTKViewer_RenderWindowInteractor::RemoveAll( const bool updateViewer )
{
  vtkRenderer* aRenderer = GetRenderer();
  vtkActorCollection* anActors = aRenderer->GetActors();
  if ( anActors )
  {
    anActors->InitTraversal();
    while ( vtkActor *anAct = anActors->GetNextActor() )
    {
      if ( anAct->IsA( "VTKViewer_Actor" ) )
      {
      }
    }

    if ( updateViewer )
      emit RenderWindowModified();
  }
}




void VTKViewer_RenderWindowInteractor::Display( VTKViewer_Actor* theActor, bool update)
{
  GetRenderer()->AddActor(theActor);
  theActor->SetVisibility(true);

  if(update)
    emit RenderWindowModified();
}

void VTKViewer_RenderWindowInteractor::KeyPressed(QKeyEvent *event)
{
  // NOT_IMPLEMENTED
}


struct TUpdateAction{
  void operator()(vtkActor* theActor){
    theActor->ApplyProperties();
  }
};

void VTKViewer_RenderWindowInteractor::Update() {
  using namespace VTK;
  vtkRenderer* aRen = GetRenderer();
  ForEach<vtkActor>(aRen->GetActors(),TUpdateAction());

  aRen->ResetCamera();

  emit RenderWindowModified();  
}


void VTKViewer_RenderWindowInteractor::unHighlightSubSelection(){
  myPointActor->SetVisibility(false);
  myEdgeActor->SetVisibility(false);
  myCellActor->SetVisibility(false);
}

bool VTKViewer_RenderWindowInteractor::unHighlightAll(){
  unHighlightSubSelection();

  emit RenderWindowModified() ;
  return false;
}


//----------------------------------------------------------------------------
bool VTKViewer_RenderWindowInteractor::highlight(const TColStd_IndexedMapOfInteger& theMapIndex,
						 VTKViewer_Actor* theMapActor, VTKViewer_Actor* theActor,
						 TUpdateActor theFun, bool hilight, bool update)
{
  if(theMapIndex.Extent() == 0) return false;

  if (hilight) {
    setActorData(theMapIndex,theMapActor,theActor,theFun);
    theActor->SetVisibility(true);
  }
  else {
    theActor->SetVisibility(false);
  }

  if(update){
    this->RenderWindow->Render();
    emit RenderWindowModified() ;
  }

  return false;
}

void VTKViewer_RenderWindowInteractor::setActorData(const TColStd_IndexedMapOfInteger& theMapIndex,
						    VTKViewer_Actor * theMapActor,
						    VTKViewer_Actor * theActor,
						    TUpdateActor theFun)
{
  (*theFun)(theMapIndex,theMapActor,theActor);
  float aPos[3];
  theMapActor->GetPosition(aPos);
  theActor->SetPosition(aPos);
}
