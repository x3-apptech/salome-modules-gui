#include "SALOME_Actor.h"

#include <qapplication.h>

#include <vtkActorCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

#include "QtxAction.h"

#include "SUIT_Session.h"
#include "SUIT_ToolButton.h"
#include "SUIT_MessageBox.h"

#include "SUIT_Tools.h"
#include "SUIT_ResourceMgr.h"

#include "VTKViewer_Transform.h"
#include "VTKViewer_Utilities.h"

#include "SVTK_Trihedron.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_ViewModel.h"
#include "SVTK_RenderWindow.h"
#include "SVTK_RenderWindowInteractor.h"
#include "SVTK_InteractorStyle.h"

#include "SALOME_ListIteratorOfListIO.hxx"

#include "SVTK_SelectorDef.h"

#include "VTKViewer_Algorithm.h"
#include "SVTK_Functor.h"

//----------------------------------------------------------------------------
SVTK_ViewWindow
::SVTK_ViewWindow( SUIT_Desktop* theDesktop, 
		   SVTK_Viewer* theModel)
  : SUIT_ViewWindow(theDesktop)
{
  myModel = theModel;
  mySelector = new SVTK_SelectorDef();
  connect(this,SIGNAL(selectionChanged()),theModel,SLOT(onSelectionChanged()));

  myTransform = VTKViewer_Transform::New();
  myTrihedron = SVTK_Trihedron::New();
  myRenderer  = vtkRenderer::New() ;

  myTrihedron->AddToRender( myRenderer );

  myRenderWindow = new SVTK_RenderWindow( this, "RenderWindow" );
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
  myRWInteractor = SVTK_RenderWindowInteractor::New();
  myRWInteractor->SetRenderWindow( myRenderWindow->getRenderWindow() );
  myRWInteractor->setViewWindow( this );

  SVTK_InteractorStyle* RWS = SVTK_InteractorStyle::New();
  RWS->setGUIWindow( myRenderWindow );
  RWS->setViewWindow( this );

  myRWInteractor->SetInteractorStyle( RWS ); 
  myRWInteractor->Initialize();

  RWS->setTriedron( myTrihedron );
  RWS->FindPokedRenderer( 0, 0 );

  SetSelectionMode(ActorSelection);

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
  connect( myRWInteractor, SIGNAL(contextMenuRequested( QContextMenuEvent * )),
           this,           SIGNAL(contextMenuRequested( QContextMenuEvent * )) );

  onResetView();
}

//----------------------------------------------------------------------------
SVTK_ViewWindow
::~SVTK_ViewWindow()
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

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::activateZoom()
{
  myRWInteractor->GetSInteractorStyle()->startZoom();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::activatePanning()
{
  myRWInteractor->GetSInteractorStyle()->startPan();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::activateRotation()
{
  myRWInteractor->GetSInteractorStyle()->startRotate();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::activateGlobalPanning()
{
  if(myTrihedron->GetVisibleActorCount(myRenderer))
    myRWInteractor->GetSInteractorStyle()->startGlobalPan();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::activateWindowFit()
{
  myRWInteractor->GetSInteractorStyle()->startFitArea();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::createActions()
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

  // onViewTrihedron: Shows - Hides Trihedron
  aAction = new QtxAction(tr("MNU_SHOW_TRIHEDRON"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TRIHEDRON" ) ),
                           tr( "MNU_SHOW_TRIHEDRON" ), 0, this);
  aAction->setStatusTip(tr("DSC_SHOW_TRIHEDRON"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewTrihedron()));
  myActionsMap[ ViewTrihedronId ] = aAction;
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::createToolBar()
{
  myActionsMap[DumpId]->addTo(myToolBar);
  myActionsMap[ViewTrihedronId]->addTo(myToolBar);

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

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onFrontView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(1,0,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onBackView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(-1,0,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onTopView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(0,0,1);
  camera->SetViewUp(0,1,0);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onBottomView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(0,0,-1);
  camera->SetViewUp(0,1,0);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onLeftView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera(); 
  camera->SetPosition(0,-1,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onRightView()
{
  vtkCamera* camera = myRenderer->GetActiveCamera();
  camera->SetPosition(0,1,0);
  camera->SetViewUp(0,0,1);
  camera->SetFocalPoint(0,0,0);
  onFitAll();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onResetView()
{
  int aTrihedronIsVisible = isTrihedronDisplayed();
  myTrihedron->SetVisibility( VTKViewer_Trihedron::eOnlyLineOn );
  ::ResetCamera(myRenderer,true);  
  vtkCamera* aCamera = myRenderer->GetActiveCamera();
  aCamera->SetPosition(1,-1,1);
  aCamera->SetViewUp(0,0,1);
  ::ResetCamera(myRenderer,true);  
  if(aTrihedronIsVisible) myTrihedron->VisibilityOn();
  else myTrihedron->VisibilityOff();
  static float aCoeff = 3.0;
  aCamera->SetParallelScale(aCoeff*aCamera->GetParallelScale());
  Repaint();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onFitAll()
{
  myRWInteractor->GetSInteractorStyle()->ViewFitAll();
  Repaint();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onDumpView()
{
  QApplication::setOverrideCursor( Qt::waitCursor );
  QPixmap px = QPixmap::grabWindow(myRenderWindow->winId());
  QApplication::restoreOverrideCursor();
  
  SUIT_Application* app = getViewManager()->study()->application();

  QString aFileName = app->getFileName( false, QString::null, tr("VTK_IMAGE_FILES"), tr("INF_APP_DUMP_VIEW"), 0 );

  if ( !aFileName.isNull() ) {
    QApplication::setOverrideCursor( Qt::waitCursor );
    QString fmt = SUIT_Tools::extension( aFileName ).upper();
    if (fmt.isEmpty())
      fmt = QString("BMP"); // default format
    if (fmt == "JPG")
      fmt = "JPEG";
    bool bOk = px.save(aFileName, fmt.latin1());
    QApplication::restoreOverrideCursor();
    if (!bOk) {
      SUIT_MessageBox::error1(this, tr("ERROR"), tr("ERR_DOC_CANT_SAVE_FILE"), tr("BUT_OK"));
    }
  }
}

//----------------------------------------------------------------
void
SVTK_ViewWindow
::onSelectionChanged()
{
  unHighlightAll();

  const SALOME_ListIO& aListIO = mySelector->StoredIObjects();
  SALOME_ListIteratorOfListIO anIter(aListIO);
  for(; anIter.More(); anIter.Next()){
    highlight(anIter.Value(),true,!anIter.More());
  }

  emit selectionChanged();
}

//----------------------------------------------------------------
void
SVTK_ViewWindow
::SetSelectionMode(Selection_Mode theMode)
{
  mySelector->SetSelectionMode(theMode);
  myRWInteractor->SetSelectionMode(theMode);
}

//----------------------------------------------------------------
Selection_Mode
SVTK_ViewWindow
::SelectionMode() const
{
  return mySelector->SelectionMode();
}

//----------------------------------------------------------------
void 
SVTK_ViewWindow
::unHighlightAll() 
{
  myRWInteractor->unHighlightAll();
}

//----------------------------------------------------------------
void
SVTK_ViewWindow
::highlight( const Handle(SALOME_InteractiveObject)& theIO, 
	     bool theIsHighlight, 
	     bool theIsUpdate ) 
{
  myRWInteractor->highlight(theIO, theIsHighlight, theIsUpdate);

  if(mySelector->HasIndex(theIO) && theIO->hasEntry()){
    TColStd_IndexedMapOfInteger aMapIndex;
    mySelector->GetIndex(theIO,aMapIndex);
    using namespace VTK;
    const char* anEntry = theIO->getEntry();
    vtkActorCollection* aCollection = myRenderer->GetActors();
    if(SALOME_Actor* anActor = Find<SALOME_Actor>(aCollection,TIsSameEntry<SALOME_Actor>(anEntry))){
      switch (mySelector->SelectionMode()) {
      case NodeSelection:
	myRWInteractor->highlightPoint(aMapIndex,anActor,theIsHighlight,theIsUpdate);
	break;
      case EdgeOfCellSelection:
	myRWInteractor->highlightEdge(aMapIndex,anActor,theIsHighlight,theIsUpdate);
	break;
      case CellSelection:
      case EdgeSelection:
      case FaceSelection:
      case VolumeSelection:
	myRWInteractor->highlightCell(aMapIndex,anActor,theIsHighlight,theIsUpdate);
	break;
      }
    }
  }else{
    myRWInteractor->unHighlightSubSelection();
  }
}

//----------------------------------------------------------------
bool
SVTK_ViewWindow
::isInViewer( const Handle(SALOME_InteractiveObject)& theIO ) 
{
  return myRWInteractor->isInViewer( theIO );
}

//----------------------------------------------------------------
bool
SVTK_ViewWindow
::isVisible( const Handle(SALOME_InteractiveObject)& theIO ) 
{
  return myRWInteractor->isVisible( theIO );
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::setBackgroundColor( const QColor& color )
{
  if ( myRenderer )
    myRenderer->SetBackground( color.red()/255., color.green()/255., color.blue()/255. );
}

//----------------------------------------------------------------------------
QColor
SVTK_ViewWindow
::backgroundColor() const
{
  float backint[3];
  if ( myRenderer ) {
    myRenderer->GetBackground( backint );
    return QColor(int(backint[0]*255), int(backint[1]*255), int(backint[2]*255));
  }
  return SUIT_ViewWindow::backgroundColor();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::Repaint(bool theUpdateTrihedron)
{
  if (theUpdateTrihedron) 
    onAdjustTrihedron();
  myRenderWindow->update();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::GetScale( double theScale[3] ) 
{
  myTransform->GetScale( theScale );
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::SetScale( double theScale[3] ) 
{
  myTransform->SetScale( theScale[0], theScale[1], theScale[2] );
  myRWInteractor->Render();
  Repaint();
}

//----------------------------------------------------------------------------
bool
SVTK_ViewWindow
::isTrihedronDisplayed()
{
  return myTrihedron->GetVisibility() == VTKViewer_Trihedron::eOn;
}

//----------------------------------------------------------------------------
void 
SVTK_ViewWindow
::onViewTrihedron()
{
  if(!myTrihedron) 
    return;

  if(isTrihedronDisplayed())
    myTrihedron->VisibilityOff();
  else
    myTrihedron->VisibilityOn();

  Repaint();
}

//----------------------------------------------------------------------------
bool
SVTK_ViewWindow
::ComputeTrihedronSize( double& theNewSize, double& theSize )
{
  // calculating diagonal of visible props of the renderer
  float aBndBox[ 6 ];
  myTrihedron->VisibilityOff();

  if ( ::ComputeVisiblePropBounds( myRenderer, aBndBox ) == 0 ) {
    aBndBox[ 1 ] = aBndBox[ 3 ] = aBndBox[ 5 ] = 100;
    aBndBox[ 0 ] = aBndBox[ 2 ] = aBndBox[ 4 ] = 0;
  }

  myTrihedron->VisibilityOn();
  float aLength = 0;
  static bool aCalcByDiag = false;
  if ( aCalcByDiag ) {
    aLength = sqrt( ( aBndBox[1]-aBndBox[0])*(aBndBox[1]-aBndBox[0] )+
                    ( aBndBox[3]-aBndBox[2])*(aBndBox[3]-aBndBox[2] )+
                    ( aBndBox[5]-aBndBox[4])*(aBndBox[5]-aBndBox[4] ) );
  } else {
    aLength = aBndBox[ 1 ]-aBndBox[ 0 ];
    aLength = max( ( aBndBox[ 3 ] - aBndBox[ 2 ] ),aLength );
    aLength = max( ( aBndBox[ 5 ] - aBndBox[ 4 ] ),aLength );
  }

  static float aSizeInPercents = 105;
  //QString aSetting = QAD_CONFIG->getSetting( "Viewer:TrihedronSize" );
  //if ( !aSetting.isEmpty() )
  //  aSizeInPercents = aSetting.toFloat();

  static float EPS_SIZE = 5.0E-3;
  theSize = myTrihedron->GetSize();
  theNewSize = aLength * aSizeInPercents / 100.0;

  // if the new trihedron size have sufficient difference, then apply the value
  return fabs( theNewSize - theSize) > theSize * EPS_SIZE ||
         fabs( theNewSize-theSize ) > theNewSize * EPS_SIZE;
}

//----------------------------------------------------------------------------
double
SVTK_ViewWindow
::GetTrihedronSize() const
{
  return myTrihedron->GetSize();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::AdjustTrihedrons( const bool theIsForcedUpdate )
{
  if ( !isTrihedronDisplayed() && !theIsForcedUpdate )
    return;

  int aVisibleNum = myTrihedron->GetVisibleActorCount( myRenderer );
  if ( aVisibleNum || theIsForcedUpdate ) {
    // if the new trihedron size have sufficient difference, then apply the value
    double aNewSize = 100, anOldSize;
    if ( ComputeTrihedronSize( aNewSize, anOldSize ) || theIsForcedUpdate ) {
      myTrihedron->SetSize( aNewSize );
      // itearte throuh displayed objects and set size if necessary
      
      vtkActorCollection* anActors = getRenderer()->GetActors();
      anActors->InitTraversal();
      while( vtkActor* anActor = anActors->GetNextActor() ) {
        if( SALOME_Actor* aSActor = SALOME_Actor::SafeDownCast( anActor ) ) {
          if ( aSActor->IsResizable() )
            aSActor->SetSize( 0.5 * aNewSize );
        }
      }
    }
  }

  ::ResetCameraClippingRange(myRenderer);
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::onAdjustTrihedron()
{   
  AdjustTrihedrons( false );
}

#define INCREMENT_FOR_OP 10

//=======================================================================
// name    : onPanLeft
// Purpose : Performs incremental panning to the left
//=======================================================================
void
SVTK_ViewWindow
::onPanLeft()
{
  myRWInteractor->GetSInteractorStyle()->IncrementalPan( -INCREMENT_FOR_OP, 0 );
}

//=======================================================================
// name    : onPanRight
// Purpose : Performs incremental panning to the right
//=======================================================================
void
SVTK_ViewWindow
::onPanRight()
{
  myRWInteractor->GetSInteractorStyle()->IncrementalPan( INCREMENT_FOR_OP, 0 );
}

//=======================================================================
// name    : onPanUp
// Purpose : Performs incremental panning to the top
//=======================================================================
void
SVTK_ViewWindow
::onPanUp()
{
  myRWInteractor->GetSInteractorStyle()->IncrementalPan( 0, INCREMENT_FOR_OP );
}

//=======================================================================
// name    : onPanDown
// Purpose : Performs incremental panning to the bottom
//=======================================================================
void
SVTK_ViewWindow
::onPanDown()
{
  myRWInteractor->GetSInteractorStyle()->IncrementalPan( 0, -INCREMENT_FOR_OP );
}

//=======================================================================
// name    : onZoomIn
// Purpose : Performs incremental zooming in
//=======================================================================
void
SVTK_ViewWindow
::onZoomIn()
{
  myRWInteractor->GetSInteractorStyle()->IncrementalZoom( INCREMENT_FOR_OP );
}

//=======================================================================
// name    : onZoomOut
// Purpose : Performs incremental zooming out
//=======================================================================
void
SVTK_ViewWindow
::onZoomOut()
{
  myRWInteractor->GetSInteractorStyle()->IncrementalZoom( -INCREMENT_FOR_OP );
}

//=======================================================================
// name    : onRotateLeft
// Purpose : Performs incremental rotating to the left
//=======================================================================
void
SVTK_ViewWindow
::onRotateLeft()
{
  myRWInteractor->GetSInteractorStyle()->IncrementalRotate( -INCREMENT_FOR_OP, 0 );
}

//=======================================================================
// name    : onRotateRight
// Purpose : Performs incremental rotating to the right
//=======================================================================
void
SVTK_ViewWindow
::onRotateRight()
{
  myRWInteractor->GetSInteractorStyle()->IncrementalRotate( INCREMENT_FOR_OP, 0 );
}

//=======================================================================
// name    : onRotateUp
// Purpose : Performs incremental rotating to the top
//=======================================================================
void
SVTK_ViewWindow
::onRotateUp()
{
  myRWInteractor->GetSInteractorStyle()->IncrementalRotate( 0, -INCREMENT_FOR_OP );
}

//=======================================================================
void
SVTK_ViewWindow
::onKeyPressed(QKeyEvent* event)
{
  emit keyPressed( this, event );
}

//=======================================================================
void
SVTK_ViewWindow
::onKeyReleased(QKeyEvent* event)
{
  emit keyReleased( this, event );
}

//=======================================================================
void
SVTK_ViewWindow
::onMousePressed(QMouseEvent* event)
{
  emit mousePressed(this, event);
}

//=======================================================================
void
SVTK_ViewWindow
::onMouseReleased(QMouseEvent* event)
{
  emit mouseReleased( this, event );
}

//=======================================================================
void
SVTK_ViewWindow
::onMouseMoving(QMouseEvent* event)
{
  emit mouseMoving( this, event );
}

//=======================================================================
void
SVTK_ViewWindow
::onMouseDoubleClicked( QMouseEvent* event )
{
  emit mouseDoubleClicked( this, event );
}

//=======================================================================
// name    : onRotateDown
// Purpose : Performs incremental rotating to the bottom
//=======================================================================
void
SVTK_ViewWindow
::onRotateDown()
{
  myRWInteractor->GetSInteractorStyle()->IncrementalRotate( 0, INCREMENT_FOR_OP );
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::InsertActor( SALOME_Actor* theActor, bool theMoveInternalActors )
{
  theActor->AddToRender(myRenderer);
  theActor->SetTransform(myTransform);
  if(theMoveInternalActors) 
    myRWInteractor->MoveInternalActors();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::AddActor( SALOME_Actor* theActor, bool theUpdate /*=false*/ )
{
  InsertActor(theActor);
  if(theUpdate) 
    Repaint();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::RemoveActor( SALOME_Actor* theActor, bool theUpdate /*=false*/ )
{
  theActor->RemoveFromRender(myRenderer);
  if(theUpdate) 
    Repaint();
}

//----------------------------------------------------------------------------
void
SVTK_ViewWindow
::MoveActor( SALOME_Actor* theActor)
{
  RemoveActor(theActor);
  InsertActor(theActor,true);
}
