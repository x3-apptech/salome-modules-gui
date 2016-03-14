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

#include "VTKViewer_ViewWindow.h"
#include "VTKViewer_ViewModel.h"
#include "VTKViewer_RenderWindow.h"
#include "VTKViewer_RenderWindowInteractor.h"
#include "VTKViewer_InteractorStyle.h"
#include "VTKViewer_Trihedron.h"
#include "VTKViewer_Transform.h"
#include "VTKViewer_Utilities.h"
#include "VTKViewer_Texture.h"
#include "VTKViewer_OpenGLRenderer.h"

#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Tools.h>
#include <SUIT_ResourceMgr.h>

#include <QFileInfo>
#include <QImage>

#include <vtkCamera.h>
#include <vtkJPEGReader.h>
#include <vtkBMPReader.h>
#include <vtkTIFFReader.h>
#include <vtkPNGReader.h>
#include <vtkMetaImageReader.h>
#include <vtkImageMapToColors.h>
#include <vtkTexture.h>

#include <QtxToolBar.h>
#include <QtxMultiAction.h>

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
  myRenderer  = VTKViewer_OpenGLRenderer::New() ;

  myTrihedron->AddToRender( myRenderer );

  myRenderWindow = new VTKViewer_RenderWindow( this, "RenderWindow" );
  setCentralWidget(myRenderWindow);
  myRenderWindow->setFocusPolicy( Qt::StrongFocus );
  myRenderWindow->setFocus();

  myRenderWindow->getRenderWindow()->AddRenderer( myRenderer );

  myRenderer->GetActiveCamera()->ParallelProjectionOn();
  myRenderer->LightFollowCameraOn();
  myRenderer->TwoSidedLightingOn();

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

  myToolBar = new QtxToolBar( true, tr("LBL_TOOLBAR_LABEL"), this );
  myToolBar->setObjectName( "VTKViewerViewOperations" );
  myToolBar->setFloatable( false );

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


  // set default background
  setBackground( Qtx::BackgroundData( Qt::black ) );
  // reset view
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
  connect(aAction, SIGNAL(triggered()), this, SLOT(onDumpView()));
  myActionsMap[ DumpId ] = aAction;

  //! \li FitAll
  aAction = new QtxAction(tr("MNU_FITALL"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITALL" ) ),
                           tr( "MNU_FITALL" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITALL"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onFitAll()));
  myActionsMap[ FitAllId ] = aAction;

  //! \li FitRect
  aAction = new QtxAction(tr("MNU_FITRECT"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITAREA" ) ),
                           tr( "MNU_FITRECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITRECT"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(activateWindowFit()));
  myActionsMap[ FitRectId ] = aAction;

  //! \li FitSelection
  aAction = new QtxAction(tr("MNU_FITSELECTION"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITSELECTION" ) ),
                           tr( "MNU_FITSELECTION" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITSELECTION"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onFitSelection()));
  myActionsMap[ FitSelectionId ] = aAction;

  //! \li Zoom
  aAction = new QtxAction(tr("MNU_ZOOM_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(activateZoom()));
  myActionsMap[ ZoomId ] = aAction;

  //! \li Panning
  aAction = new QtxAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(activatePanning()));
  myActionsMap[ PanId ] = aAction;

  //! \li Global Panning
  aAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(activateGlobalPanning()));
  myActionsMap[ GlobalPanId ] = aAction;

  //! \li Rotation
  aAction = new QtxAction(tr("MNU_ROTATE_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ROTATE" ) ),
                           tr( "MNU_ROTATE_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ROTATE_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(activateRotation()));
  myActionsMap[ RotationId ] = aAction;

  //! \li Projections
  aAction = new QtxAction(tr("MNU_FRONT_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FRONT" ) ),
                           tr( "MNU_FRONT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_FRONT_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onFrontView()));
  myActionsMap[ FrontId ] = aAction;

  //! \li Back view
  aAction = new QtxAction(tr("MNU_BACK_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BACK" ) ),
                           tr( "MNU_BACK_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_BACK_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onBackView()));
  myActionsMap[ BackId ] = aAction;

  //! \li Top view
  aAction = new QtxAction(tr("MNU_TOP_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TOP" ) ),
                           tr( "MNU_TOP_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_TOP_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onTopView()));
  myActionsMap[ TopId ] = aAction;

  //! \li Bottom view
  aAction = new QtxAction(tr("MNU_BOTTOM_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BOTTOM" ) ),
                           tr( "MNU_BOTTOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_BOTTOM_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onBottomView()));
  myActionsMap[ BottomId ] = aAction;

  //! \li Left view
  aAction = new QtxAction(tr("MNU_LEFT_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_LEFT" ) ),
                           tr( "MNU_LEFT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_LEFT_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onLeftView()));
  myActionsMap[ LeftId ] = aAction;

  //! \li Right view
  aAction = new QtxAction(tr("MNU_RIGHT_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RIGHT" ) ),
                           tr( "MNU_RIGHT_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RIGHT_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onRightView()));
  myActionsMap[ RightId ] = aAction;

  // \li Rotate anticlockwise
  aAction = new QtxAction(tr("MNU_ANTICLOCKWISE_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ANTICLOCKWISE" ) ),
			  tr( "MNU_ANTICLOCKWISE_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ANTICLOCKWISE_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onAntiClockWiseView()));
  myActionsMap[ AntiClockWiseId ] = aAction;

  // \li Rotate clockwise
  aAction = new QtxAction(tr("MNU_CLOCKWISE_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_CLOCKWISE" ) ),
			  tr( "MNU_CLOCKWISE_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_CLOCKWISE_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onClockWiseView()));
  myActionsMap[ ClockWiseId ] = aAction;

  //! \li Reset
  aAction = new QtxAction(tr("MNU_RESET_VIEW"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RESET" ) ),
                           tr( "MNU_RESET_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onResetView()));
  myActionsMap[ ResetId ] = aAction;

  //! \li Trihedron shown
  aAction = new QtxAction(tr("MNU_SHOW_TRIHEDRON"), aResMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TRIHEDRON" ) ),
                           tr( "MNU_SHOW_TRIHEDRON" ), 0, this);
  aAction->setStatusTip(tr("DSC_SHOW_TRIHEDRON"));
  connect(aAction, SIGNAL(triggered()), this, SLOT(onTrihedronShow()));
  myActionsMap[ TrihedronShowId ] = aAction;
}

/*!Create tool bar.*/
void VTKViewer_ViewWindow::createToolBar()
{
  myToolBar->addAction( myActionsMap[DumpId] );
  myToolBar->addAction( myActionsMap[TrihedronShowId] );

  QtxMultiAction* aScaleAction = new QtxMultiAction( this );
  aScaleAction->insertAction( myActionsMap[FitAllId] );
  aScaleAction->insertAction( myActionsMap[FitRectId] );
  aScaleAction->insertAction( myActionsMap[FitSelectionId] );
  aScaleAction->insertAction( myActionsMap[ZoomId] );
  myToolBar->addAction( aScaleAction );

  QtxMultiAction* aPanningAction = new QtxMultiAction( this );
  aPanningAction->insertAction( myActionsMap[PanId] );
  aPanningAction->insertAction( myActionsMap[GlobalPanId] );
  myToolBar->addAction( aPanningAction );

  myToolBar->addAction( myActionsMap[RotationId] );

  QtxMultiAction* aViewsAction = new QtxMultiAction(myToolBar);
  aViewsAction->insertAction( myActionsMap[FrontId] );
  aViewsAction->insertAction( myActionsMap[BackId] );
  aViewsAction->insertAction( myActionsMap[TopId] );
  aViewsAction->insertAction( myActionsMap[BottomId] );
  aViewsAction->insertAction( myActionsMap[LeftId] );
  aViewsAction->insertAction( myActionsMap[RightId] );
  myToolBar->addAction( aViewsAction );

  myToolBar->addAction( myActionsMap[AntiClockWiseId] );
  myToolBar->addAction( myActionsMap[ClockWiseId] );

  myToolBar->addAction( myActionsMap[ResetId] );
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

/*!
  \brief Rotate view 90 degrees clockwise
*/
void VTKViewer_ViewWindow::onClockWiseView()
{
  vtkCamera* aCamera = myRenderer->GetActiveCamera(); 
  aCamera->Roll(-90);
  aCamera->OrthogonalizeViewUp();
  Repaint();
}

/*!
  \brief Rotate view 90 degrees conterclockwise
*/
void VTKViewer_ViewWindow::onAntiClockWiseView()
{
  vtkCamera* aCamera = myRenderer->GetActiveCamera(); 
  aCamera->Roll(90);
  aCamera->OrthogonalizeViewUp();
  Repaint();
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
  static double aCoeff = 3.0;
  aCamera->SetParallelScale(aCoeff*aCamera->GetParallelScale());
  Repaint();
}

/*!On fit all slot.*/
void VTKViewer_ViewWindow::onFitAll()
{
  myRWInteractor->GetInteractorStyle()->ViewFitAll();
  Repaint();
}

/*!On fit selection slot.*/
void VTKViewer_ViewWindow::onFitSelection()
{
  myRWInteractor->GetInteractorStyle()->ViewFitSelection();
  Repaint();
}

/*!Set background color of the viewport [obsolete]*/
void VTKViewer_ViewWindow::setBackgroundColor( const QColor& c )
{
  Qtx::BackgroundData bg = background();
  bg.setColor( c );
  setBackground( bg );
}

/*!Returns background color of the viewport [obsolete]*/
QColor VTKViewer_ViewWindow::backgroundColor() const
{
  return background().color();
}

/*!Set background of the viewport*/
void VTKViewer_ViewWindow::setBackground( const Qtx::BackgroundData& bgData )
{
  bool ok = false;
 
  if ( bgData.isValid() ) {
    switch ( bgData.mode() ) {
    case Qtx::ColorBackground:
      {
	QColor c = bgData.color();
	if ( c.isValid() ) {
	  // show solid-colored background
	  getRenderer()->SetTexturedBackground( false );  // cancel texture mode
	  getRenderer()->SetGradientBackground( false );  // cancel gradient mode
	  getRenderer()->SetBackground( c.red()/255.0,
					c.green()/255.0,
					c.blue()/255.0 ); // set background color
	  ok = true;
	}
	break;
      }
    case Qtx::SimpleGradientBackground:
      {
	QColor c1, c2;
	int type = bgData.gradient( c1, c2 );
        if ( c1.isValid() )
        {
          if ( !c2.isValid() )
            c2 = c1;

          // show two-color gradient background
          getRenderer()->SetTexturedBackground( false );    // cancel texture mode
          getRenderer()->SetGradientBackground( true );     // switch to gradient mode

          VTKViewer_OpenGLRenderer* aRenderer =
            VTKViewer_OpenGLRenderer::SafeDownCast( getRenderer() );
          if( aRenderer )
          {
            aRenderer->SetGradientType( type );
            aRenderer->SetBackground( c1.redF(), c1.greenF(), c1.blueF() );
            aRenderer->SetBackground2( c2.redF(), c2.greenF(), c2.blueF() );
            ok = true;
          }
        }
	break;
      }
    case Qtx::CustomGradientBackground:
      {
	// NOT IMPLEMENTED YET
	getRenderer()->SetTexturedBackground( false );  // cancel texture mode
	getRenderer()->SetGradientBackground( false );  // cancel gradient mode
	// .........
	break;
      }
    default:
      break;
    }
    if ( bgData.isTextureShown() ) {
      QString fileName;
      int textureMode = bgData.texture( fileName );
      QFileInfo fi( fileName );
      if ( !fileName.isEmpty() && fi.exists() ) {
	// read texture from file
	QString extension = fi.suffix().toLower();
	vtkImageReader2* aReader = 0;
	if ( extension == "jpg" || extension == "jpeg" )
	  aReader = vtkJPEGReader::New();
	else if ( extension == "bmp" )
	  aReader = vtkBMPReader::New();
	else if ( extension == "tif" || extension == "tiff" )
	  aReader = vtkTIFFReader::New();
	else if ( extension == "png" )
	  aReader = vtkPNGReader::New();
	else if ( extension == "mhd" || extension == "mha" )
	  aReader = vtkMetaImageReader::New();           
	if ( aReader ) {
	  // create texture
	  aReader->SetFileName( fi.absoluteFilePath().toLatin1().constData() );
	  aReader->Update();
	  
	  VTKViewer_Texture* aTexture = VTKViewer_Texture::New();
	  vtkImageMapToColors* aMap = 0;
	  vtkAlgorithmOutput* anOutput;
	  /*
	  // special processing for BMP reader
	  vtkBMPReader* aBMPReader = (vtkBMPReader*)aReader;
	  if ( aBMPReader ) {
	    // Special processing for BMP file
	    aBMPReader->SetAllow8BitBMP(1);
	    
	    aMap = vtkImageMapToColors::New();
	    aMap->SetInputConnection( aBMPReader->GetOutputPort() );
	    aMap->SetLookupTable( (vtkScalarsToColors*)aBMPReader->GetLookupTable() );
	    aMap->SetOutputFormatToRGB();
	    
	    anOutput = aMap->GetOutputPort();
	  }
	  else {
          }
	  */
	  anOutput = aReader->GetOutputPort( 0 );
	  aTexture->SetInputConnection( anOutput );
	  // set texture mode
	  // VSR: Currently, VTK only supports Stretch mode, so below code will give
	  // the same results for all modes
	  switch ( textureMode ) {
	  case Qtx::TileTexture:
	    aTexture->RepeatOn();
	    aTexture->EdgeClampOff();
	    aTexture->InterpolateOff();
	    break;
	  case Qtx::StretchTexture:
	    aTexture->RepeatOff();
	    aTexture->EdgeClampOff();
	    aTexture->InterpolateOn();
	    break;
	  case Qtx::CenterTexture:
	  default:
	    aTexture->RepeatOff();
	    aTexture->EdgeClampOn();
	    aTexture->InterpolateOff();
	    break;
	  }
	  // show textured background
	  getRenderer()->SetTexturedBackground( true );     // switch to texture mode
	  getRenderer()->SetBackgroundTexture( aTexture );  // set texture image
	  
	  // clean-up resources
	  if ( aMap )
	    aMap->Delete();
	  aReader->Delete();
	  aTexture->Delete();
	  ok = true;
	}
      }
    }
  }

  if ( ok )
    myBackground = bgData;
}

/*!Returns background of the viewport*/
Qtx::BackgroundData VTKViewer_ViewWindow::background() const
{
  return myBackground;
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
    double bnd[6];
    myTrihedron->VisibilityOff();
    ::ComputeVisiblePropBounds(myRenderer,bnd);
    myTrihedron->VisibilityOn();
    double aLength = 0;
    static bool CalcByDiag = false;
    if(CalcByDiag){
      aLength = sqrt((bnd[1]-bnd[0])*(bnd[1]-bnd[0])+
                     (bnd[3]-bnd[2])*(bnd[3]-bnd[2])+
                     (bnd[5]-bnd[4])*(bnd[5]-bnd[4]));
    }else{
      aLength = bnd[1]-bnd[0];
      aLength = qMax((bnd[3]-bnd[2]),aLength);
      aLength = qMax((bnd[5]-bnd[4]),aLength);
    }
   
    static double aSizeInPercents = 105;
    QString aSetting;// = SUIT_CONFIG->getSetting("Viewer:TrihedronSize");
    if(!aSetting.isEmpty()) aSizeInPercents = aSetting.toFloat();

    static double EPS_SIZE = 5.0E-3;
    double aSize = myTrihedron->GetSize();
    double aNewSize = aLength*aSizeInPercents/100.0;
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
  return px.toImage();
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
  QStringList paramsLst = parameters.split( '*' );
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
