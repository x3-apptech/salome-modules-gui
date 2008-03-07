// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "VTKViewer_ViewWindow.h"
#include "VTKViewer_ViewModel.h"
#include "VTKViewer_RenderWindow.h"
#include "VTKViewer_RenderWindowInteractor.h"
#include "VTKViewer_InteractorStyle.h"
#include "VTKViewer_Trihedron.h"
#include "VTKViewer_Transform.h"
#include "VTKViewer_Utilities.h"

#include "SUIT_Session.h"
#include "SUIT_ToolButton.h"
#include "SUIT_MessageBox.h"

#include "SUIT_Tools.h"
#include "SUIT_ResourceMgr.h"

#include <qapplication.h>
#include <qimage.h>

#include <vtkRenderer.h>
#include <vtkCamera.h>

/*! Construction*/
VTKViewer_ViewWindow::VTKViewer_ViewWindow( SUIT_Desktop* theDesktop, 
                                            VTKViewer_Viewer* theModel,
					    VTKViewer_InteractorStyle* iStyle,
					    VTKViewer_RenderWindowInteractor* rw )
: SUIT_ViewWindow( theDesktop )
{
  myModel = theModel;

  myTrihedron = VTKViewer_Trihedron::New();
  myTransform = VTKViewer_Transform::New();
  myRenderer  = vtkRenderer::New() ;

  myTrihedron->AddToRender( myRenderer );

  myRenderWindow = new VTKViewer_RenderWindow( this, "RenderWindow" );
  setCentralWidget(myRenderWindow);
  myRenderWindow->setFocusPolicy( StrongFocus );
  myRenderWindow->setFocus();

  myRenderWindow->getRenderWindow()->AddRenderer( myRenderer );

  myRenderer->GetActiveCamera()->ParallelProjectionOn();
  myRenderer->LightFollowCameraOn();
  myRenderer->TwoSidedLightingOn();

  // Set BackgroundColor
  QString BgrColorRed   = "0";//SUIT_CONFIG->getSetting("VTKViewer:BackgroundColorRed");
  QString BgrColorGreen = "0";//SUIT_CONFIG->getSetting("VTKViewer:BackgroundColorGreen");
  QString BgrColorBlue  = "0";//SUIT_CONFIG->getSetting("VTKViewer:BackgroundColorBlue");

  if( !BgrColorRed.isEmpty() && !BgrColorGreen.isEmpty() && !BgrColorBlue.isEmpty() ) 
    myRenderer->SetBackground( BgrColorRed.toInt()/255., BgrColorGreen.toInt()/255., BgrColorBlue.toInt()/255. );
  else
    myRenderer->SetBackground( 0, 0, 0 );
  
  // Create an interactor.
  myRWInteractor = rw ? rw : VTKViewer_RenderWindowInteractor::New();
  myRWInteractor->SetRenderWindow( myRenderWindow->getRenderWindow() );

  VTKViewer_InteractorStyle* RWS = iStyle ? iStyle : VTKViewer_InteractorStyle::New();
  RWS->setGUIWindow( myRenderWindow );
  myRWInteractor->SetInteractorStyle( RWS ); 

  myRWInteractor->Initialize();
  RWS->setTriedron( myTrihedron );
  RWS->FindPokedRenderer( 0, 0 );

  setCentralWidget( myRenderWindow );

  myToolBar = new QToolBar(this);
  myToolBar->setCloseMode(QDockWindow::Undocked);
  myToolBar->setLabel(tr("LBL_TOOLBAR_LABEL"));

  createActions();
  createToolBar();

  connect( myRenderWindow, SIGNAL(KeyPressed( QKeyEvent* )),
           this,           SLOT(onKeyPressed( QKeyEvent* )) );
  connect( myRenderWindow, SIGNAL(KeyReleased( QKeyEvent* )),
           this,           SLOT(onKeyReleased( QKeyEvent* )) );
  connect( myRenderWindow, SIGNAL(MouseButtonPressed( QMouseEvent* )),
           this,           SLOT(onMousePressed( QMouseEvent* )) );
  connect( myRenderWindow, SIGNAL(MouseButtonReleased( QMouseEvent* )),
           this,           SLOT(onMouseReleased( QMouseEvent* )) );
  connect( myRenderWindow, SIGNAL(MouseDoubleClicked( QMouseEvent* )),
           this,           SLOT(onMouseDoubleClicked( QMouseEvent* )) );
  connect( myRenderWindow, SIGNAL(MouseMove( QMouseEvent* )),
           this,           SLOT(onMouseMoving( QMouseEvent* )) );
  connect( myRWInteractor, SIGNAL(RenderWindowModified()),
           myRenderWindow, SLOT(update()) );

  connect( myRenderWindow, SIGNAL(contextMenuRequested( QContextMenuEvent * )),
           this,           SIGNAL(contextMenuRequested( QContextMenuEvent * )) );

  connect( myRWInteractor, SIGNAL(contextMenuRequested( QContextMenuEvent * )),
           this,           SIGNAL(contextMenuRequested( QContextMenuEvent * )) );


  onResetView();
}

/*!Destructor.*/
VTKViewer_ViewWindow::~VTKViewer_ViewWindow()
{
  myTransform->Delete();
  // In order to ensure that the interactor unregisters
  // this RenderWindow, we assign a NULL RenderWindow to 
  // it before deleting it.
  myRWInteractor->SetRenderWindow( NULL );
  myRWInteractor->Delete();
  
  //m_RW->Delete() ;
  myRenderer->RemoveAllViewProps();
  //m_Renderer->Delete() ;
  myTrihedron->Delete();
}

/*!Checks: is trihedron displayed.*/
bool VTKViewer_ViewWindow::isTrihedronDisplayed(){
  return myTrihedron->GetVisibility() == VTKViewer_Trihedron::eOn;
}

/*!Activates 'zooming' transformation*/
void VTKViewer_ViewWindow::activateZoom()
{
  myRWInteractor->GetInteractorStyle()->startZoom();
}

/*!Activates 'panning' transformation*/
void VTKViewer_ViewWindow::activatePanning()
{
  myRWInteractor->GetInteractorStyle()->startPan();
}

/*!Activates 'rotation' transformation*/
void VTKViewer_ViewWindow::activateRotation()
{
  myRWInteractor->GetInteractorStyle()->startRotate();
}

/*!Activate global panning.*/
void VTKViewer_ViewWindow::activateGlobalPanning()
{
  //if(myTrihedron->GetVisibleActorCount(myRenderer))
  myRWInteractor->GetInteractorStyle()->startGlobalPan();
}

/*!Activates 'fit area' transformation*/
void VTKViewer_ViewWindow::activateWindowFit()
{
  myRWInteractor->GetInteractorStyle()->startFitArea();
}

/*!Create actions:*/
void VTKViewer_ViewWindow::createActions()
{
  if (!myActionsMap.isEmpty()) return;
  
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  
  QtxAction* aAction;

  //! \li Dump view
  aAction = new QtxAction(tr("MNU_DUMP_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_DUMP" ) ),
                           tr( "MNU_DUMP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onDumpView()));
  myActionsMap[ DumpId ] = aAction;

  //! \li FitAll
  aAction = new QtxAction(tr("MNU_FITALL"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITALL" ) ),
                           tr( "MNU_FITALL" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITALL"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onFitAll()));
  myActionsMap[ FitAllId ] = aAction;

  //! \li FitRect
  aAction = new QtxAction(tr("MNU_FITRECT"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITAREA" ) ),
                           tr( "MNU_FITRECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITRECT"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateWindowFit()));
  myActionsMap[ FitRectId ] = aAction;

  //! \li Zoom
  aAction = new QtxAction(tr("MNU_ZOOM_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateZoom()));
  myActionsMap[ ZoomId ] = aAction;

  //! \li Panning
  aAction = new QtxAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activatePanning()));
  myActionsMap[ PanId ] = aAction;

  //! \li Global Panning
  aAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateGlobalPanning()));
  myActionsMap[ GlobalPanId ] = aAction;

  //! \li Rotation
  aAction = new QtxAction(tr("MNU_ROTATE_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ROTATE" ) ),
                           tr( "MNU_ROTATE_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ROTATE_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateRotation()));
  myActionsMap[ RotationId ] = aAction;

  //! \li Projections
  aAction = new QtxAction(tr("MNU_FRONT_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FRONT" ) ),
                           tr( "MNU_FRONT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_FRONT_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onFrontView()));
  myActionsMap[ FrontId ] = aAction;

  //! \li Back view
  aAction = new QtxAction(tr("MNU_BACK_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BACK" ) ),
                           tr( "MNU_BACK_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_BACK_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onBackView()));
  myActionsMap[ BackId ] = aAction;

  //! \li Top view
  aAction = new QtxAction(tr("MNU_TOP_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TOP" ) ),
                           tr( "MNU_TOP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_TOP_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onTopView()));
  myActionsMap[ TopId ] = aAction;

  //! \li Bottom view
  aAction = new QtxAction(tr("MNU_BOTTOM_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BOTTOM" ) ),
                           tr( "MNU_BOTTOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_BOTTOM_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onBottomView()));
  myActionsMap[ BottomId ] = aAction;

  //! \li Left view
  aAction = new QtxAction(tr("MNU_LEFT_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_LEFT" ) ),
                           tr( "MNU_LEFT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_LEFT_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onLeftView()));
  myActionsMap[ LeftId ] = aAction;

  //! \li Right view
  aAction = new QtxAction(tr("MNU_RIGHT_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RIGHT" ) ),
                           tr( "MNU_RIGHT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RIGHT_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onRightView()));
  myActionsMap[ RightId ] = aAction;

  //! \li Reset
  aAction = new QtxAction(tr("MNU_RESET_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RESET" ) ),
                           tr( "MNU_RESET_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onResetView()));
  myActionsMap[ ResetId ] = aAction;

  //! \li Trihedron shown
  aAction = new QtxAction(tr("MNU_SHOW_TRIHEDRON"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TRIHEDRON" ) ),
                           tr( "MNU_SHOW_TRIHEDRON" ), 0, this);
  aAction->setStatusTip(tr("DSC_SHOW_TRIHEDRON"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onTrihedronShow()));
  myActionsMap[ TrihedronShowId ] = aAction;
}

/*!Create tool bar.*/
void VTKViewer_ViewWindow::createToolBar()
{
  myActionsMap[DumpId]->addTo(myToolBar);
  myActionsMap[TrihedronShowId]->addTo(myToolBar);

  SUIT_ToolButton* aScaleBtn = new SUIT_ToolButton(myToolBar);
  aScaleBtn->AddAction(myActionsMap[FitAllId]);
  aScaleBtn->AddAction(myActionsMap[FitRectId]);
  aScaleBtn->AddAction(myActionsMap[ZoomId]);

  SUIT_ToolButton* aPanningBtn = new SUIT_ToolButton(myToolBar);
  aPanningBtn->AddAction(myActionsMap[PanId]);
  aPanningBtn->AddAction(myActionsMap[GlobalPanId]);

  myActionsMap[RotationId]->addTo(myToolBar);

  SUIT_ToolButton* aViewsBtn = new SUIT_ToolButton(myToolBar);
  aViewsBtn->AddAction(myActionsMap[FrontId]);
  aViewsBtn->AddAction(myActionsMap[BackId]);
  aViewsBtn->AddAction(myActionsMap[TopId]);
  aViewsBtn->AddAction(myActionsMap[BottomId]);
  aViewsBtn->AddAction(myActionsMap[LeftId]);
  aViewsBtn->AddAction(myActionsMap[RightId]);

  myActionsMap[ResetId]->addTo(myToolBar);
}

/*!On front view event.*/
void VTKViewer_ViewWindow::onFrontView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(1,0,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

/*!On back view slot.*/
void VTKViewer_ViewWindow::onBackView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(-1,0,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

/*!On back view slot.*/
void VTKViewer_ViewWindow::onTopView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(0,0,1);
  camera->SetViewUp(0,1,0);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

/*!On bottom view slot.*/
void VTKViewer_ViewWindow::onBottomView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(0,0,-1);
  camera->SetViewUp(0,1,0);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

/*!On left view slot.*/
void VTKViewer_ViewWindow::onLeftView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera(); 
  camera->SetPosition(0,-1,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

/*!On right view slot.*/
void VTKViewer_ViewWindow::onRightView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(0,1,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

/*!On reset view slot.*/
void VTKViewer_ViewWindow::onResetView()
{
  int aTriedronIsVisible = isTrihedronDisplayed();
  myTrihedron->SetVisibility( VTKViewer_Trihedron::eOnlyLineOn );
  ::ResetCamera(myRenderer,true);  
  vtkCamera* aCamera = myRenderer->GetActiveCamera();
  aCamera->SetPosition(1,-1,1);
  aCamera->SetViewUp(0,0,1);
  ::ResetCamera(myRenderer,true);  
  if(aTriedronIsVisible) myTrihedron->VisibilityOn();
  else myTrihedron->VisibilityOff();
  static vtkFloatingPointType aCoeff = 3.0;
  aCamera->SetParallelScale(aCoeff*aCamera->GetParallelScale());
  Repaint();
}

/*!On fit all slot.*/
void VTKViewer_ViewWindow::onFitAll()
{
  myRWInteractor->GetInteractorStyle()->ViewFitAll();
  Repaint();
}

/*!Set background of the viewport*/
void VTKViewer_ViewWindow::setBackgroundColor( const QColor& color )
{
  if ( myRenderer )
    myRenderer->SetBackground( color.red()/255., color.green()/255., color.blue()/255. );
}

/*!Returns background of the viewport*/
QColor VTKViewer_ViewWindow::backgroundColor() const
{
  vtkFloatingPointType backint[3];
  if ( myRenderer ) {
    myRenderer->GetBackground( backint );
    return QColor(int(backint[0]*255), int(backint[1]*255), int(backint[2]*255));
  }
  return SUIT_ViewWindow::backgroundColor();
}

/*!Repaint window. If \a theUpdateTrihedron is true - recalculate trihedron.*/
void VTKViewer_ViewWindow::Repaint(bool theUpdateTrihedron)
{
  if (theUpdateTrihedron) onAdjustTrihedron();
  myRenderWindow->update();
}

/*!Get scale of transformation filter.*/
void VTKViewer_ViewWindow::GetScale( double theScale[3] ) {
  myTransform->GetScale( theScale );
}

/*!Set scale of transformation filter and repaint window.*/
void VTKViewer_ViewWindow::SetScale( double theScale[3] ) {
  myTransform->SetMatrixScale( theScale[0], theScale[1], theScale[2] );
  myRWInteractor->Render();
  Repaint();
}

/*!Calculation trihedron size.*/
void VTKViewer_ViewWindow::onAdjustTrihedron(){   
  if( !isTrihedronDisplayed() ) 
    return;
  int aVisibleNum = myTrihedron->GetVisibleActorCount(myRenderer);
  if(aVisibleNum){
    // calculating diagonal of visible props of the renderer
    vtkFloatingPointType bnd[6];
    myTrihedron->VisibilityOff();
    ::ComputeVisiblePropBounds(myRenderer,bnd);
    myTrihedron->VisibilityOn();
    vtkFloatingPointType aLength = 0;
    static bool CalcByDiag = false;
    if(CalcByDiag){
      aLength = sqrt((bnd[1]-bnd[0])*(bnd[1]-bnd[0])+
                     (bnd[3]-bnd[2])*(bnd[3]-bnd[2])+
                     (bnd[5]-bnd[4])*(bnd[5]-bnd[4]));
    }else{
      aLength = bnd[1]-bnd[0];
      aLength = std::max((bnd[3]-bnd[2]),aLength);
      aLength = std::max((bnd[5]-bnd[4]),aLength);
    }
   
    static vtkFloatingPointType aSizeInPercents = 105;
    QString aSetting;// = SUIT_CONFIG->getSetting("Viewer:TrihedronSize");
    if(!aSetting.isEmpty()) aSizeInPercents = aSetting.toFloat();

    static vtkFloatingPointType EPS_SIZE = 5.0E-3;
    vtkFloatingPointType aSize = myTrihedron->GetSize();
    vtkFloatingPointType aNewSize = aLength*aSizeInPercents/100.0;
    // if the new trihedron size have sufficient difference, then apply the value
    if(fabs(aNewSize-aSize) > aSize*EPS_SIZE || fabs(aNewSize-aSize) > aNewSize*EPS_SIZE){
      myTrihedron->SetSize(aNewSize);
    }
  }
  ::ResetCameraClippingRange(myRenderer);
}

/*!Emit key pressed.*/
void VTKViewer_ViewWindow::onKeyPressed(QKeyEvent* event)
{
  emit keyPressed( this, event );
}

/*!Emit key released.*/
void VTKViewer_ViewWindow::onKeyReleased(QKeyEvent* event)
{
  emit keyReleased( this, event );
}

/*!Emit key pressed.*/
void VTKViewer_ViewWindow::onMousePressed(QMouseEvent* event)
{
  emit mousePressed(this, event);
}

/*!Emit mouse released.*/
void VTKViewer_ViewWindow::onMouseReleased(QMouseEvent* event)
{
  emit mouseReleased( this, event );
}

/*!Emit mouse moving.*/
void VTKViewer_ViewWindow::onMouseMoving(QMouseEvent* event)
{
  emit mouseMoving( this, event );
}

/*!Emit mouse double clicked.*/
void VTKViewer_ViewWindow::onMouseDoubleClicked( QMouseEvent* event )
{
  emit mouseDoubleClicked( this, event );
}

/*!Insert actor to renderer and transformation filter.
 *Move Internal actors, if \a theMoveInternalActors is true.
 */
void VTKViewer_ViewWindow::InsertActor( VTKViewer_Actor* theActor, bool theMoveInternalActors ){
  theActor->AddToRender(myRenderer);
  theActor->SetTransform(myTransform);
  if(theMoveInternalActors) 
    myRWInteractor->MoveInternalActors();
}

/*!Add actor.Repaint window if \a theUpdate is true.
 *@see InsertActor( VTKViewer_Actor* theActor, bool theMoveInternalActors )
 */
void VTKViewer_ViewWindow::AddActor( VTKViewer_Actor* theActor, bool theUpdate /*=false*/ ){
  InsertActor(theActor);
  if(theUpdate) 
    Repaint();
}

/*!Remove \a theActor from renderer and pepaint, if \a theUpdate is true.*/
void VTKViewer_ViewWindow::RemoveActor( VTKViewer_Actor* theActor, bool theUpdate /*=false*/ ){
  theActor->RemoveFromRender(myRenderer);
  if(theUpdate) 
    Repaint();
}

/*!@see RemoveActor() and InsertActor().*/
void VTKViewer_ViewWindow::MoveActor( VTKViewer_Actor* theActor)
{
  RemoveActor(theActor);
  InsertActor(theActor,true);
}

/*!On trihedron show slot.*/
void VTKViewer_ViewWindow::onTrihedronShow()
{
  if (isTrihedronDisplayed())
    myTrihedron->VisibilityOff();
  else
    myTrihedron->VisibilityOn();
  myRenderWindow->update();
}

/*!Dump view.*/
QImage VTKViewer_ViewWindow::dumpView()
{
  QPixmap px = QPixmap::grabWindow( myRenderWindow->winId() );
  return px.convertToImage();
}

/*! The method returns the visual parameters of this view as a formated string
 */
QString VTKViewer_ViewWindow::getVisualParameters()
{
  double pos[3], focalPnt[3], viewUp[3], parScale, scale[3];

  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->GetPosition( pos );
  camera->GetFocalPoint( focalPnt );
  camera->GetViewUp( viewUp );
  parScale = camera->GetParallelScale();
  GetScale( scale );

  QString retStr;
  retStr.sprintf( "%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e*%.12e", 
		  pos[0], pos[1], pos[2], focalPnt[0], focalPnt[1], focalPnt[2], viewUp[0], viewUp[1], 
		  viewUp[2], parScale, scale[0], scale[1], scale[2] );
  return retStr;
}

/*! The method restors visual parameters of this view from a formated string
 */
void VTKViewer_ViewWindow::setVisualParameters( const QString& parameters )
{
  QStringList paramsLst = QStringList::split( '*', parameters, true );
  if ( paramsLst.size() == 13 ) {
    double pos[3], focalPnt[3], viewUp[3], parScale, scale[3];
    pos[0] = paramsLst[0].toDouble();
    pos[1] = paramsLst[1].toDouble();
    pos[2] = paramsLst[2].toDouble();
    focalPnt[0] = paramsLst[3].toDouble();
    focalPnt[1] = paramsLst[4].toDouble();
    focalPnt[2] = paramsLst[5].toDouble();
    viewUp[0] = paramsLst[6].toDouble();
    viewUp[1] = paramsLst[7].toDouble();
    viewUp[2] = paramsLst[8].toDouble();
    parScale = paramsLst[9].toDouble();
    scale[0] = paramsLst[10].toDouble();
    scale[1] = paramsLst[11].toDouble();
    scale[2] = paramsLst[12].toDouble();

    vtkCamera* camera = myRenderer->GetActiveCamera();
    camera->SetPosition( pos );
    camera->SetFocalPoint( focalPnt );
    camera->SetViewUp( viewUp );
    camera->SetParallelScale( parScale );
    myTransform->SetMatrixScale( scale[0], scale[1], scale[2] );
    myRWInteractor->Render();
  }
}
