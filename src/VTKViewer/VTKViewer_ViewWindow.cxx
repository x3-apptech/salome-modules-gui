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
#include "SUIT_FileDlg.h"

#include <qapplication.h>

#include <vtkRenderer.h>
#include <vtkCamera.h>

//****************************************************************
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
VTKViewer_ViewWindow::VTKViewer_ViewWindow( SUIT_Desktop* theDesktop, 
                                            VTKViewer_Viewer* theModel,
					    VTKViewer_InteractorStyle* iStyle,
					    VTKViewer_RenderWindowInteractor* rw )
: SUIT_ViewWindow(theDesktop)
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

//****************************************************************
VTKViewer_ViewWindow::~VTKViewer_ViewWindow()
{
  myTransform->Delete();
  // In order to ensure that the interactor unregisters
  // this RenderWindow, we assign a NULL RenderWindow to 
  // it before deleting it.
  myRWInteractor->SetRenderWindow( NULL );
  myRWInteractor->Delete();
  
  //m_RW->Delete() ;
  myRenderer->RemoveAllProps();
  //m_Renderer->Delete() ;
  myTrihedron->Delete();
}

//****************************************************************
bool VTKViewer_ViewWindow::isTrihedronDisplayed(){
  return myTrihedron->GetVisibility() == VTKViewer_Trihedron::eOn;
}

//****************************************************************
/*!
    Activates 'zooming' transformation
*/
void VTKViewer_ViewWindow::activateZoom()
{
  myRWInteractor->GetInteractorStyle()->startZoom();
}

//****************************************************************
/*!
    Activates 'panning' transformation
*/
void VTKViewer_ViewWindow::activatePanning()
{
  myRWInteractor->GetInteractorStyle()->startPan();
}

//****************************************************************
/*!
    Activates 'rotation' transformation
*/
void VTKViewer_ViewWindow::activateRotation()
{
  myRWInteractor->GetInteractorStyle()->startRotate();
}

//****************************************************************
void VTKViewer_ViewWindow::activateGlobalPanning()
{
  //if(myTrihedron->GetVisibleActorCount(myRenderer))
  myRWInteractor->GetInteractorStyle()->startGlobalPan();
}

//****************************************************************
/*!
    Activates 'fit area' transformation
*/
void VTKViewer_ViewWindow::activateWindowFit()
{
  myRWInteractor->GetInteractorStyle()->startFitArea();
}

//****************************************************************
void VTKViewer_ViewWindow::createActions()
{
  if (!myActionsMap.isEmpty()) return;
  
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  
  QtxAction* aAction;

  // Dump view
  aAction = new QtxAction(tr("MNU_DUMP_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_DUMP" ) ),
                           tr( "MNU_DUMP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onDumpView()));
  myActionsMap[ DumpId ] = aAction;

  // FitAll
  aAction = new QtxAction(tr("MNU_FITALL"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITALL" ) ),
                           tr( "MNU_FITALL" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITALL"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onFitAll()));
  myActionsMap[ FitAllId ] = aAction;

  // FitRect
  aAction = new QtxAction(tr("MNU_FITRECT"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITAREA" ) ),
                           tr( "MNU_FITRECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITRECT"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateWindowFit()));
  myActionsMap[ FitRectId ] = aAction;

  // Zoom
  aAction = new QtxAction(tr("MNU_ZOOM_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateZoom()));
  myActionsMap[ ZoomId ] = aAction;

  // Panning
  aAction = new QtxAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activatePanning()));
  myActionsMap[ PanId ] = aAction;

  // Global Panning
  aAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateGlobalPanning()));
  myActionsMap[ GlobalPanId ] = aAction;

  // Rotation
  aAction = new QtxAction(tr("MNU_ROTATE_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ROTATE" ) ),
                           tr( "MNU_ROTATE_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ROTATE_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(activateRotation()));
  myActionsMap[ RotationId ] = aAction;

  // Projections
  aAction = new QtxAction(tr("MNU_FRONT_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FRONT" ) ),
                           tr( "MNU_FRONT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_FRONT_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onFrontView()));
  myActionsMap[ FrontId ] = aAction;

  aAction = new QtxAction(tr("MNU_BACK_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BACK" ) ),
                           tr( "MNU_BACK_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_BACK_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onBackView()));
  myActionsMap[ BackId ] = aAction;

  aAction = new QtxAction(tr("MNU_TOP_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TOP" ) ),
                           tr( "MNU_TOP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_TOP_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onTopView()));
  myActionsMap[ TopId ] = aAction;

  aAction = new QtxAction(tr("MNU_BOTTOM_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BOTTOM" ) ),
                           tr( "MNU_BOTTOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_BOTTOM_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onBottomView()));
  myActionsMap[ BottomId ] = aAction;

  aAction = new QtxAction(tr("MNU_LEFT_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_LEFT" ) ),
                           tr( "MNU_LEFT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_LEFT_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onLeftView()));
  myActionsMap[ LeftId ] = aAction;

  aAction = new QtxAction(tr("MNU_RIGHT_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RIGHT" ) ),
                           tr( "MNU_RIGHT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RIGHT_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onRightView()));
  myActionsMap[ RightId ] = aAction;

  // Reset
  aAction = new QtxAction(tr("MNU_RESET_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RESET" ) ),
                           tr( "MNU_RESET_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onResetView()));
  myActionsMap[ ResetId ] = aAction;
}

//****************************************************************
void VTKViewer_ViewWindow::createToolBar()
{
  myActionsMap[DumpId]->addTo(myToolBar);

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

//****************************************************************
void VTKViewer_ViewWindow::onFrontView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(1,0,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//****************************************************************
void VTKViewer_ViewWindow::onBackView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(-1,0,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//****************************************************************
void VTKViewer_ViewWindow::onTopView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(0,0,1);
  camera->SetViewUp(0,1,0);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//****************************************************************
void VTKViewer_ViewWindow::onBottomView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(0,0,-1);
  camera->SetViewUp(0,1,0);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//****************************************************************
void VTKViewer_ViewWindow::onLeftView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera(); 
  camera->SetPosition(0,-1,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//****************************************************************
void VTKViewer_ViewWindow::onRightView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(0,1,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//****************************************************************
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
  static float aCoeff = 3.0;
  aCamera->SetParallelScale(aCoeff*aCamera->GetParallelScale());
  Repaint();
}

//****************************************************************
void VTKViewer_ViewWindow::onFitAll()
{
  myRWInteractor->GetInteractorStyle()->ViewFitAll();
  Repaint();
}

//****************************************************************
/*
   Dumps 3d-Viewer contents into image file
   File format is defined by file's extension; supported formats : PNG, BMP, GIF, JPG
*/
void VTKViewer_ViewWindow::onDumpView()
{
  QApplication::setOverrideCursor( Qt::waitCursor );
  QPixmap px = QPixmap::grabWindow(myRenderWindow->winId());
  QApplication::restoreOverrideCursor();
  
  QString fileName = SUIT_FileDlg::getFileName(this,
                                              QString::null,
                                              tr("VTK_IMAGE_FILES"),
                                              tr("INF_APP_DUMP_VIEW"),
                                              false);
  if (!fileName.isNull()) {
    QApplication::setOverrideCursor( Qt::waitCursor );
    QString fmt = SUIT_Tools::extension( fileName ).upper();
    if (fmt.isEmpty())
      fmt = QString("BMP"); // default format
    if (fmt == "JPG")
      fmt = "JPEG";
    bool bOk = px.save(fileName, fmt.latin1());
    QApplication::restoreOverrideCursor();
    if (!bOk) {
      SUIT_MessageBox::error1(this, tr("ERROR"), tr("ERR_DOC_CANT_SAVE_FILE"), tr("BUT_OK"));
    }
  }
}

//****************************************************************
/*!
    Set background of the viewport
*/
void VTKViewer_ViewWindow::setBackgroundColor( const QColor& color )
{
  if ( myRenderer )
    myRenderer->SetBackground( color.red()/255., color.green()/255., color.blue()/255. );
}

//****************************************************************
/*!
    Returns background of the viewport
*/
QColor VTKViewer_ViewWindow::backgroundColor() const
{
  float backint[3];
  if ( myRenderer ) {
    myRenderer->GetBackground( backint );
    return QColor(int(backint[0]*255), int(backint[1]*255), int(backint[2]*255));
  }
  return SUIT_ViewWindow::backgroundColor();
}

//****************************************************************
void VTKViewer_ViewWindow::Repaint(bool theUpdateTrihedron)
{
  if (theUpdateTrihedron) onAdjustTrihedron();
  myRenderWindow->update();
}

//****************************************************************
void VTKViewer_ViewWindow::GetScale( double theScale[3] ) {
  myTransform->GetScale( theScale );
}

//****************************************************************
void VTKViewer_ViewWindow::SetScale( double theScale[3] ) {
  myTransform->SetScale( theScale[0], theScale[1], theScale[2] );
  myRWInteractor->Render();
  Repaint();
}

//****************************************************************
void VTKViewer_ViewWindow::onAdjustTrihedron(){   
  if( !isTrihedronDisplayed() ) 
    return;
  int aVisibleNum = myTrihedron->GetVisibleActorCount(myRenderer);
  if(aVisibleNum){
    // calculating diagonal of visible props of the renderer
    float bnd[6];
    myTrihedron->VisibilityOff();
    ::ComputeVisiblePropBounds(myRenderer,bnd);
    myTrihedron->VisibilityOn();
    float aLength = 0;
    static bool CalcByDiag = false;
    if(CalcByDiag){
      aLength = sqrt((bnd[1]-bnd[0])*(bnd[1]-bnd[0])+
                     (bnd[3]-bnd[2])*(bnd[3]-bnd[2])+
                     (bnd[5]-bnd[4])*(bnd[5]-bnd[4]));
    }else{
      aLength = bnd[1]-bnd[0];
      aLength = max((bnd[3]-bnd[2]),aLength);
      aLength = max((bnd[5]-bnd[4]),aLength);
    }
   
    static float aSizeInPercents = 105;
    QString aSetting;// = SUIT_CONFIG->getSetting("Viewer:TrihedronSize");
    if(!aSetting.isEmpty()) aSizeInPercents = aSetting.toFloat();

    static float EPS_SIZE = 5.0E-3;
    float aSize = myTrihedron->GetSize();
    float aNewSize = aLength*aSizeInPercents/100.0;
    // if the new trihedron size have sufficient difference, then apply the value
    if(fabs(aNewSize-aSize) > aSize*EPS_SIZE || fabs(aNewSize-aSize) > aNewSize*EPS_SIZE){
      myTrihedron->SetSize(aNewSize);
    }
  }
  ::ResetCameraClippingRange(myRenderer);
}

//****************************************************************
void VTKViewer_ViewWindow::onKeyPressed(QKeyEvent* event)
{
  emit keyPressed( this, event );
}

//****************************************************************
void VTKViewer_ViewWindow::onKeyReleased(QKeyEvent* event)
{
  emit keyReleased( this, event );
}

//****************************************************************
void VTKViewer_ViewWindow::onMousePressed(QMouseEvent* event)
{
  emit mousePressed(this, event);
}

//****************************************************************
void VTKViewer_ViewWindow::onMouseReleased(QMouseEvent* event)
{
  emit mouseReleased( this, event );
}

//****************************************************************
void VTKViewer_ViewWindow::onMouseMoving(QMouseEvent* event)
{
  emit mouseMoving( this, event );
}

//****************************************************************
void VTKViewer_ViewWindow::onMouseDoubleClicked( QMouseEvent* event )
{
  emit mouseDoubleClicked( this, event );
}

void VTKViewer_ViewWindow::InsertActor( VTKViewer_Actor* theActor, bool theMoveInternalActors ){
  theActor->AddToRender(myRenderer);
  theActor->SetTransform(myTransform);
  if(theMoveInternalActors) 
    myRWInteractor->MoveInternalActors();
}

void VTKViewer_ViewWindow::AddActor( VTKViewer_Actor* theActor, bool theUpdate /*=false*/ ){
  InsertActor(theActor);
  if(theUpdate) 
    Repaint();
}

void VTKViewer_ViewWindow::RemoveActor( VTKViewer_Actor* theActor, bool theUpdate /*=false*/ ){
  theActor->RemoveFromRender(myRenderer);
  if(theUpdate) 
    Repaint();
}

void VTKViewer_ViewWindow::MoveActor( VTKViewer_Actor* theActor)
{
  RemoveActor(theActor);
  InsertActor(theActor,true);
}
