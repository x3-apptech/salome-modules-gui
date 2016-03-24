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

#include "VTKViewer_RenderWindowInteractor.h"
#include "VTKViewer_RenderWindow.h"
#include "VTKViewer_InteractorStyle.h"
#include "SUIT_ViewModel.h"
#include "VTKViewer_ViewWindow.h"

#include "VTKViewer_Actor.h"
#include "VTKViewer_Algorithm.h"
#include "VTKViewer_Functor.h"

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
#include <vtkProperty.h>

// QT Includes
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>

/*! Create new instance of VTKViewer_RenderWindowInteractor*/
VTKViewer_RenderWindowInteractor* VTKViewer_RenderWindowInteractor::New() 
{
  vtkObject *ret = vtkObjectFactory::CreateInstance("VTKViewer_RenderWindowInteractor") ;
  if( ret ) {
    return dynamic_cast<VTKViewer_RenderWindowInteractor *>(ret) ;
  }
  return new VTKViewer_RenderWindowInteractor;
}

/*!Constructor.*/
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

/*!Destructor.*/
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

/*!
  Print interactor to stream
  \param os - stream
  \param indent
*/
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

/*!Description:\n
 * Initializes the event handlers without an XtAppContext.  This is \n
 * good for when you don`t have a user interface, but you still \n
 * want to have mouse interaction.\n
 * We never allow the VTKViewer_RenderWindowInteractor to control \n
 * the event loop. The application always has the control.
 */
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

/*!Sets view window and add to it selection actors.*/
void VTKViewer_RenderWindowInteractor::setViewWindow(VTKViewer_ViewWindow* theViewWnd){
  myViewWnd = theViewWnd;

  if ( myViewWnd ) {
    myViewWnd->InsertActor(myCellActor);
    myViewWnd->InsertActor(myEdgeActor);
    myViewWnd->InsertActor(myPointActor);
  }
}

/*!Move selection actors to view window.*/
void VTKViewer_RenderWindowInteractor::MoveInternalActors()
{
  myViewWnd->MoveActor(myCellActor);
  myViewWnd->MoveActor(myEdgeActor);
  myViewWnd->MoveActor(myPointActor);
}

/*!Sets interactor style.*/
void VTKViewer_RenderWindowInteractor::SetInteractorStyle(vtkInteractorObserver *theInteractor){
  myInteractorStyle = dynamic_cast<VTKViewer_InteractorStyle*>(theInteractor);
  vtkRenderWindowInteractor::SetInteractorStyle(theInteractor);
}

/*!Sets selection properties.
 *\param theRed - red component of color
 *\param theGreen - green component of color
 *\param theBlue - blue component of color
 *\param theWidth - point size and line width
 */
void VTKViewer_RenderWindowInteractor::SetSelectionProp(const double& theRed, const double& theGreen, 
                                                        const double& theBlue, const int& theWidth) 
{
  myCellActor->GetProperty()->SetColor(theRed, theGreen, theBlue);
  myCellActor->GetProperty()->SetLineWidth(theWidth);

  myPointActor->GetProperty()->SetColor(theRed, theGreen, theBlue);
  myPointActor->GetProperty()->SetPointSize(theWidth);
}

/*!Sets selection tolerance
 *\param theTolNodes - nodes selection tolerance
 *\param theTolItems - selection tolerance for basic and cell pickers.
 */
void VTKViewer_RenderWindowInteractor::SetSelectionTolerance(const double& theTolNodes, const double& theTolItems)
{
  myTolNodes = theTolNodes;
  myTolItems = theTolItems;

  myBasicPicker->SetTolerance(myTolItems);
  myCellPicker->SetTolerance(myTolItems);
  myPointPicker->SetTolerance(myTolNodes);

}

/*! Description:\n
 * Enable/Disable interactions.  By default interactors are enabled when \n
 * initialized.  Initialize() must be called prior to enabling/disabling \n
 * interaction. These methods are used when a window/widget is being \n
 * shared by multiple renderers and interactors.  This allows a "modal" \n
 * display where one interactor is active when its data is to be displayed \n
 * and all other interactors associated with the widget are disabled \n
 * when their data is not displayed.
 */
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

/*!See Enable().*/
void VTKViewer_RenderWindowInteractor::Disable()
{
  if( ! this->Enabled ) {
    return ;
  }
  
  this->Enabled = 0 ;
  this->Modified() ;
}

/*!Description:\n
 * This will start up the X event loop and never return. If you \n
 * call this method it will loop processing X events until the \n
 * application is exited.
 */
void VTKViewer_RenderWindowInteractor::Start()
{
  //
  // We do not allow this interactor to control the 
  // event loop. Only the QtApplication objects are
  // allowed to do that. 
  //
  vtkErrorMacro(<<"VTKViewer_RenderWindowInteractor::Start() not allowed to start event loop.") ;
}

/*! Description:\n
 * Event loop notification member for Window size change
 */
void VTKViewer_RenderWindowInteractor::UpdateSize(int w, int h)
{
  // if the size changed send this on to the RenderWindow
  if ((w != this->Size[0])||(h != this->Size[1])) {
    this->Size[0] = w;
    this->Size[1] = h;
    this->RenderWindow->SetSize(w,h);
  }
}

/*! Description: 
 * Timer methods must be overridden by platform dependent subclasses.
 * flag is passed to indicate if this is first timer set or an update 
 * as Win32 uses repeating timers, whereas X uses One shot more timer 
 * if flag == VTKXI_TIMER_FIRST Win32 and X should createtimer 
 * otherwise Win32 should exit and X should perform AddTimeOut
 * \retval 1
 */
int VTKViewer_RenderWindowInteractor::CreateTimer(int vtkNotUsed(timertype))
{
  ///
  /// Start a one-shot timer for 10ms.
  ///
  mTimer->setSingleShot(true) ;
  mTimer->start(10) ;
  return 1 ;
}

/*! 
  \sa CreateTimer(int )
  \retval 1
 */
int VTKViewer_RenderWindowInteractor::DestroyTimer(void)
{
  //
  // :TRICKY: Tue May  2 00:17:32 2000 Pagey
  //
  /*! QTimer will automatically expire after 10ms. So 
   * we do not need to do anything here. In fact, we 
   * should not even Stop() the QTimer here because doing 
   * this will skip some of the processing that the TimerFunc()
   * does and will result in undesirable effects. For 
   * example, this will result in vtkLODActor to leave
   * the models in low-res mode after the mouse stops
   * moving. 
   */
  return 1 ;
}

/*! Not all of these slots are needed in VTK_MAJOR_VERSION=3,\n
 * but moc does not understand "#if VTK_MAJOR_VERSION". Hence, \n
 * we have to include all of these for the time being. Once,\n
 * this bug in MOC is fixed, we can separate these. 
 */
void VTKViewer_RenderWindowInteractor::TimerFunc()
{
  if( ! this->Enabled ) {
    return ;
  }
  
  ((vtkInteractorStyle*)this->InteractorStyle)->OnTimer() ;
  emit RenderWindowModified() ;
}

/*!Emit render window modified on mouse move,\n
 *if interactor style needs redrawing and render window enabled.*/
void VTKViewer_RenderWindowInteractor::MouseMove(QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnMouseMove(0, 0, event->x(), event->y()/*this->Size[1] - event->y() - 1*/) ;
  if (myInteractorStyle->needsRedrawing() )
    emit RenderWindowModified() ; 
}

/*!Reaction on left button pressed.\n
 *Same as left button down for interactor style.\n
 *If render window enabled.
 */
void VTKViewer_RenderWindowInteractor::LeftButtonPressed(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnLeftButtonDown((event->modifiers() & Qt::ControlModifier), 
                                      (event->modifiers() & Qt::ShiftModifier), 
                                      event->x(), event->y());
}

/*!Reaction on left button releases.\n
 *Same as left button up for interactor style.\n
 *If render window enabled.
 */
void VTKViewer_RenderWindowInteractor::LeftButtonReleased(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnLeftButtonUp( (event->modifiers() & Qt::ControlModifier), 
                                     (event->modifiers() & Qt::ShiftModifier), 
                                     event->x(), event->y() ) ;
}

/*!Reaction on middle button pressed.\n
 *Same as middle button down for interactor style.\n
 *If render window enabled.
 */
void VTKViewer_RenderWindowInteractor::MiddleButtonPressed(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnMiddleButtonDown((event->modifiers() & Qt::ControlModifier), 
                                        (event->modifiers() & Qt::ShiftModifier), 
                                        event->x(), event->y() ) ;
}

/*!Reaction on middle button released.\n
 *Same as middle button up for interactor style.\n
 *If render window enabled.
 */
void VTKViewer_RenderWindowInteractor::MiddleButtonReleased(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnMiddleButtonUp( (event->modifiers() & Qt::ControlModifier), 
                                       (event->modifiers() & Qt::ShiftModifier), 
                                       event->x(), event->y() ) ;
}

/*!Reaction on right button pressed.\n
 *Same as right button down for interactor style.\n
 *If render window enabled.
 */
void VTKViewer_RenderWindowInteractor::RightButtonPressed(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  myInteractorStyle->OnRightButtonDown( (event->modifiers() & Qt::ControlModifier), 
                                        (event->modifiers() & Qt::ShiftModifier), 
                                        event->x(), event->y() ) ;
}

/*!Reaction on right button released.\n
 *Same as right button up for interactor style.If render window enabled.\n
 *Emit context menu requested, if interactor style state equal VTK_INTERACTOR_STYLE_CAMERA_NONE.
 */
void VTKViewer_RenderWindowInteractor::RightButtonReleased(const QMouseEvent *event) {
  if( ! this->Enabled ) {
    return ;
  }
  bool isOperation = myInteractorStyle->CurrentState() != VTK_INTERACTOR_STYLE_CAMERA_NONE;
  myInteractorStyle->OnRightButtonUp( (event->modifiers() & Qt::ControlModifier),
                                      (event->modifiers() & Qt::ShiftModifier),
                                      event->x(), event->y() );
  if ( !isOperation )
  {
    QContextMenuEvent aEvent( QContextMenuEvent::Mouse,
                              event->pos(), event->globalPos() );
    emit contextMenuRequested( &aEvent );
  }
}

/*!Reaction on button pressed.
 *\warning Do nothing.
 */
void VTKViewer_RenderWindowInteractor::ButtonPressed(const QMouseEvent *event) {
  return ;
}

/*!Reaction on button released..
 *\warning Do nothing.
 */
void VTKViewer_RenderWindowInteractor::ButtonReleased(const QMouseEvent *event) {
  return ;
}

/*!Gets display mode.*/
int VTKViewer_RenderWindowInteractor::GetDisplayMode() {
  return myDisplayMode;
}

/*!Sets display mode.*/
void VTKViewer_RenderWindowInteractor::SetDisplayMode(int theMode) {
  if(theMode == 0)
    ChangeRepresentationToWireframe();
  else if (theMode == 1)
    ChangeRepresentationToSurface();
  else if (theMode == 2) {
    ChangeRepresentationToSurfaceWithEdges();
    theMode++;
  }
  myDisplayMode = theMode;
}

/*!Change all actors to wireframe*/
void VTKViewer_RenderWindowInteractor::ChangeRepresentationToWireframe()
{
  using namespace VTK;
  ActorCollectionCopy aCopy(GetRenderer()->GetActors());
  ChangeRepresentationToWireframe(aCopy.GetActors());
}

/*!Change all actors to surface*/
void VTKViewer_RenderWindowInteractor::ChangeRepresentationToSurface()
{
  using namespace VTK;
  ActorCollectionCopy aCopy(GetRenderer()->GetActors());
  ChangeRepresentationToSurface(aCopy.GetActors());
}

/*!Change all actors to surface with edges*/
void VTKViewer_RenderWindowInteractor::ChangeRepresentationToSurfaceWithEdges()
{
  using namespace VTK;
  ActorCollectionCopy aCopy(GetRenderer()->GetActors());
  ChangeRepresentationToSurfaceWithEdges(aCopy.GetActors());
}

/*!Change all actors from \a theCollection to wireframe and
 * emit render window modified.
 */
void VTKViewer_RenderWindowInteractor::ChangeRepresentationToWireframe(vtkActorCollection* theCollection)
{
  using namespace VTK;
  ForEach<VTKViewer_Actor>(theCollection,
                        TSetFunction<VTKViewer_Actor,int>
                        (&VTKViewer_Actor::setDisplayMode,0));
  emit RenderWindowModified();
}

/*!Change all actors from \a theCollection to surface and
 * emit render window modified.
 */
void VTKViewer_RenderWindowInteractor::ChangeRepresentationToSurface(vtkActorCollection* theCollection)
{
  using namespace VTK;
  ForEach<VTKViewer_Actor>(theCollection,
                        TSetFunction<VTKViewer_Actor,int>
                        (&VTKViewer_Actor::setDisplayMode,1));
  emit RenderWindowModified();
}

/*!Change all actors from \a theCollection to surface with edges and
 * emit render window modified.
 */
void VTKViewer_RenderWindowInteractor::ChangeRepresentationToSurfaceWithEdges(vtkActorCollection* theCollection)
{
  using namespace VTK;
  ForEach<VTKViewer_Actor>(theCollection,
                        TSetFunction<VTKViewer_Actor,int>
                        (&VTKViewer_Actor::setDisplayMode,3));
  emit RenderWindowModified();
}

/*!Gets renderer.*/
vtkRenderer* VTKViewer_RenderWindowInteractor::GetRenderer()
{
  vtkRendererCollection * theRenderers =  this->RenderWindow->GetRenderers();
  theRenderers->InitTraversal();
  return theRenderers->GetNextItem();
}

/*!Do nothing*/
void VTKViewer_RenderWindowInteractor::EraseAll()
{
}

/*!Display all actors.
 *Sets visible for all actors from renderer collection and emit render window modified.
 */
void VTKViewer_RenderWindowInteractor::DisplayAll()
{
  using namespace VTK;
  ActorCollectionCopy aCopy(GetRenderer()->GetActors());
  ForEach<VTKViewer_Actor>(aCopy.GetActors(),TSetVisibility<VTKViewer_Actor>(true));

  emit RenderWindowModified() ;
}

/*!Do nothing*/
void VTKViewer_RenderWindowInteractor::Erase( VTKViewer_Actor* SActor, bool update)
{
}

/*!Remove \a SActor from renderer and emit update window, if \a updateViewer - true*/
void VTKViewer_RenderWindowInteractor::Remove( VTKViewer_Actor* SActor, bool updateViewer )
{
  if ( SActor != 0 )
  {
    GetRenderer()->RemoveViewProp( SActor );
    if ( updateViewer )
      emit RenderWindowModified();
  }
}

/*!Remove actors from render window collection(not implemented).
 *Emit render window modified, if \a updateViewer - true.
 */
void VTKViewer_RenderWindowInteractor::RemoveAll( const bool updateViewer )
{
  using namespace VTK;
  vtkRenderer* aRenderer = GetRenderer();
  ActorCollectionCopy aCopy(aRenderer->GetActors());
  vtkActorCollection* anActors = aCopy.GetActors();
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

/*!\brief Display the \a theActor.*/
/*! Add actor to renderer and set visibility to true.
 * Emit render window modified, if \a update - true.
 */
void VTKViewer_RenderWindowInteractor::Display( VTKViewer_Actor* theActor, bool update)
{
  GetRenderer()->AddActor(theActor);
  theActor->SetVisibility(true);

  if(update)
    emit RenderWindowModified();
}

/*!
  default key press event (empty implementation)
*/
void VTKViewer_RenderWindowInteractor::KeyPressed(QKeyEvent *event)
{
  /// NOT_IMPLEMENTED
}

/*!Structure with one function "operator()", which call apply properties for actor.*/
struct TUpdateAction{
  /*!Apply properties for \a theActor.*/
  void operator()(vtkActor* theActor){
    theActor->ApplyProperties();
  }
};

/*!Update all actors from renderer and emit render window modified.*/
void VTKViewer_RenderWindowInteractor::Update() {
  using namespace VTK;
  vtkRenderer* aRen = GetRenderer();
  ActorCollectionCopy aCopy(aRen->GetActors());
  ForEach<vtkActor>(aCopy.GetActors(),TUpdateAction());

  aRen->ResetCamera();

  emit RenderWindowModified();  
}

/*!Unhighlight all selection actors.*/
void VTKViewer_RenderWindowInteractor::unHighlightSubSelection(){
  myPointActor->SetVisibility(false);
  myEdgeActor->SetVisibility(false);
  myCellActor->SetVisibility(false);
}

/*!@see unHighlightSubSelection()
 * Also emit render window modified.
 */
bool VTKViewer_RenderWindowInteractor::unHighlightAll(){
  unHighlightSubSelection();

  emit RenderWindowModified() ;
  return false;
}


/*! \li Sets actors data and sets visibility to true, if flag \a hilight - true, 
 * else sets visibility to false.
 * \li Emit render window modified, if flag \a update - true.
 */
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

/*!Sets actors data.*/
void VTKViewer_RenderWindowInteractor::setActorData(const TColStd_IndexedMapOfInteger& theMapIndex,
                                                    VTKViewer_Actor * theMapActor,
                                                    VTKViewer_Actor * theActor,
                                                    TUpdateActor theFun)
{
  (*theFun)(theMapIndex,theMapActor,theActor);
  double aPos[3];
  theMapActor->GetPosition(aPos);
  theActor->SetPosition(aPos);
}
