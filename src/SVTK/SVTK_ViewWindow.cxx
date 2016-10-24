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

#include "SVTK_NonIsometricDlg.h"
#include "SVTK_UpdateRateDlg.h"
#include "SVTK_CubeAxesDlg.h"
#include "SVTK_PsOptionsDlg.h"
#include "SVTK_SetRotationPointDlg.h"
#include "SVTK_ViewParameterDlg.h"
#include "SVTK_ViewModel.h"
#include "VTKViewer_Texture.h"
#include "VTKViewer_OpenGLRenderer.h"

#include "SALOME_Actor.h"

#include <QMenu>
#include <QToolBar>
#include <QEvent>
#include <QFileInfo>
#include <QSignalMapper>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

#include <vtkTextProperty.h>
#include <vtkActorCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>
#include <vtkAxisActor2D.h>
#include <vtkGL2PSExporter.h>
#include <vtkInteractorStyle.h>
#include <vtkProperty.h>
#include <vtkCallbackCommand.h>
#include <vtkJPEGReader.h>
#include <vtkBMPReader.h>
#include <vtkTIFFReader.h>
#include <vtkPNGReader.h>
#include <vtkMetaImageReader.h>
#include <vtkImageMapToColors.h>
#include <vtkTexture.h>

#include "QtxAction.h"

#include "SUIT_Session.h"
#include "SUIT_MessageBox.h"
#include "SUIT_Accel.h"
#include "SUIT_Tools.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_Accel.h"
#include "SUIT_OverrideCursor.h"
#include "SUIT_ViewManager.h"
#include "QtxActionToolMgr.h"
#include "QtxMultiAction.h"
#include "QtxActionGroup.h"

#include "VTKViewer_Utilities.h"
#include "VTKViewer_Trihedron.h"
#include "VTKViewer_Actor.h"

#include "SVTK_View.h"
#include "SVTK_Selector.h"

#include "SVTK_Event.h"
#include "SVTK_Renderer.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_InteractorStyle.h"
#include "SVTK_RenderWindowInteractor.h"
#include "SVTK_GenericRenderWindowInteractor.h"
#include "SVTK_CubeAxesActor2D.h"
#include "SVTK_ComboAction.h"
#include "SVTK_KeyFreeInteractorStyle.h"
#include "SVTK_Selector.h"
#include "SVTK_Recorder.h"
#include "SVTK_RecorderDlg.h"

#include "salomevtkPVAxesWidget.h"
#include "salomevtkPVAxesActor.h"

#include "SALOME_ListIO.hxx"

#include "VTKViewer_Algorithm.h"
#include "SVTK_Functor.h"

#include <OpenGLUtils_FrameBuffer.h>

#include <GL/gl.h>

namespace SVTK
{
  int convertAction( const int accelAction )
  {
    switch ( accelAction ) {
    case SUIT_Accel::PanLeft     : return SVTK::PanLeftEvent;
    case SUIT_Accel::PanRight    : return SVTK::PanRightEvent;
    case SUIT_Accel::PanUp       : return SVTK::PanUpEvent;
    case SUIT_Accel::PanDown     : return SVTK::PanDownEvent;
    case SUIT_Accel::ZoomIn      : return SVTK::ZoomInEvent;
    case SUIT_Accel::ZoomOut     : return SVTK::ZoomOutEvent;
    case SUIT_Accel::RotateLeft  : return SVTK::RotateLeftEvent;
    case SUIT_Accel::RotateRight : return SVTK::RotateRightEvent;
    case SUIT_Accel::RotateUp    : return SVTK::RotateUpEvent;
    case SUIT_Accel::RotateDown  : return SVTK::RotateDownEvent;  
    }
    return accelAction;
  }
}

/*!
  Constructor
*/
SVTK_ViewWindow::SVTK_ViewWindow(SUIT_Desktop* theDesktop):
  SUIT_ViewWindow(theDesktop),
  myView(NULL),
  myDumpImage(QImage()),
  myKeyFreeInteractorStyle(SVTK_KeyFreeInteractorStyle::New()),
  myEventCallbackCommand(vtkCallbackCommand::New())
{
  setWindowFlags( windowFlags() & ~Qt::Window );
  // specific of vtkSmartPointer
  myKeyFreeInteractorStyle->Delete();
}

/*!
  To initialize #SVTK_ViewWindow instance
*/
void SVTK_ViewWindow::Initialize(SVTK_ViewModelBase* theModel)
{
  myModel = theModel;
  myInteractor = new SVTK_RenderWindowInteractor(this,"SVTK_RenderWindowInteractor");
  
  SVTK_Selector* aSelector = SVTK_Selector::New();
  int aPreselectionMode =  SUIT_Session::session()->resourceMgr()->
    integerValue( "VTKViewer", "preselection", Standard_Preselection );
  aSelector->SetDynamicPreSelection( aPreselectionMode == Dynamic_Preselection );
  aSelector->SetPreSelectionEnabled( aPreselectionMode != Preselection_Disabled );
  bool isSelectionEnabled = SUIT_Session::session()->resourceMgr()->
    booleanValue( "VTKViewer", "enable_selection", true );
  aSelector->SetSelectionEnabled( isSelectionEnabled );
    
  SVTK_GenericRenderWindowInteractor* aDevice = SVTK_GenericRenderWindowInteractor::New();
  aDevice->SetRenderWidget(myInteractor);
  aDevice->SetSelector(aSelector);
  
  SVTK_Renderer* aRenderer = SVTK_Renderer::New();
  aRenderer->Initialize(aDevice,aSelector);
  
  myInteractor->Initialize(aDevice,aRenderer,aSelector);
  
  aDevice->Delete();
  aRenderer->Delete();
  aSelector->Delete();
  
  myToolBar = toolMgr()->createToolBar( tr("LBL_TOOLBAR_LABEL"),                       // title (language-dependant)
					QString( "VTKViewerViewOperations" ),          // name (language-independant)
					false );                                       // disable floatable toolbar

  myRecordingToolBar = toolMgr()->createToolBar( tr("LBL_TOOLBAR_RECORD_LABEL"),       // title (language-dependant)
						 QString( "VTKRecordingOperations" ),  // name (language-independant)
						 false );                              // disable floatable toolbar
  
  createActions( SUIT_Session::session()->resourceMgr() );
  createToolBar();
  
  SetEventDispatcher(myInteractor->GetDevice());
  myInteractor->setBackgroundRole( QPalette::NoRole );//NoBackground
  myInteractor->setFocusPolicy(Qt::StrongFocus);
  myInteractor->setFocus();
  bool isSupportQuadBuffer = SUIT_Session::session()->resourceMgr()->
    booleanValue( "VTKViewer", "enable_quad_buffer_support", false );
  myInteractor->getRenderWindow()->SetStereoCapableWindow((int)isSupportQuadBuffer);
  setFocusProxy(myInteractor);
  
  myUpdateRateDlg = new SVTK_UpdateRateDlg( getAction( UpdateRate ), this, "SVTK_UpdateRateDlg" );
  myNonIsometricDlg = new SVTK_NonIsometricDlg( getAction( NonIsometric ), this, "SVTK_NonIsometricDlg" );
  myCubeAxesDlg = new SVTK_CubeAxesDlg( getAction( GraduatedAxes ), this, "SVTK_CubeAxesDlg" );
  myCubeAxesDlg->initialize();
  mySetRotationPointDlg = new SVTK_SetRotationPointDlg
    ( getAction( ChangeRotationPointId ), this, "SVTK_SetRotationPointDlg" );
  myViewParameterDlg = new SVTK_ViewParameterDlg
    ( getAction( ViewParametersId ), this, "SVTK_ViewParameterDlg" );
  
  myDefaultInteractorStyle = SVTK_InteractorStyle::New();
  myInteractor->PushInteractorStyle(myDefaultInteractorStyle);
  myDefaultInteractorStyle->Delete();
  
  myRecorder = SVTK_Recorder::New();
  
  myRecorder->SetNbFPS( 17.3 );
  myRecorder->SetQuality( 100 );
  myRecorder->SetProgressiveMode( true );
  myRecorder->SetUseSkippedFrames( true );
  myRecorder->SetRenderWindow( myInteractor->getRenderWindow() );
  
  setCentralWidget(myInteractor);
  
  myAxesWidget = salomevtk::vtkPVAxesWidget::New();
  myAxesWidget->SetParentRenderer(aRenderer->GetDevice());
  myAxesWidget->SetViewport(0, 0, 0.25, 0.25);
  myAxesWidget->SetInteractor(myInteractor->GetDevice());
  myAxesWidget->SetEnabled(1);
  myAxesWidget->SetInteractive(0);

  salomevtk::vtkPVAxesActor* anAxesActor = myAxesWidget->GetAxesActor();
  anAxesActor->GetXAxisTipProperty()->SetColor(   1.0, 0.0, 0.0 );
  anAxesActor->GetXAxisShaftProperty()->SetColor( 1.0, 0.0, 0.0 );
  anAxesActor->GetXAxisLabelProperty()->SetColor( 1.0, 0.0, 0.0 );
  anAxesActor->GetYAxisTipProperty()->SetColor(   0.0, 1.0, 0.0 );
  anAxesActor->GetYAxisShaftProperty()->SetColor( 0.0, 1.0, 0.0 );
  anAxesActor->GetYAxisLabelProperty()->SetColor( 0.0, 1.0, 0.0 );
  anAxesActor->GetZAxisTipProperty()->SetColor(   0.0, 0.0, 1.0 );
  anAxesActor->GetZAxisShaftProperty()->SetColor( 0.0, 0.0, 1.0 );
  anAxesActor->GetZAxisLabelProperty()->SetColor( 0.0, 0.0, 1.0 );

  myView = new SVTK_View(this);
  Initialize(myView,theModel);


  myEventCallbackCommand->SetClientData(this);
  myEventCallbackCommand->SetCallback(SVTK_ViewWindow::ProcessEvents);
  myEventCallbackCommand->Delete();

  GetInteractor()->GetInteractorStyle()->AddObserver(SVTK::OperationFinished, 
						     myEventCallbackCommand.GetPointer(), 0.0);
  myKeyFreeInteractorStyle->AddObserver(SVTK::OperationFinished, 
					myEventCallbackCommand.GetPointer(), 0.0);


  
  myInteractor->getRenderWindow()->Render();
  setBackground( Qtx::BackgroundData( Qt::black ) ); // set default background
  onResetView();
}

/*!
  To initialize #SVTK_ViewWindow instance
*/
void SVTK_ViewWindow::Initialize(SVTK_View* theView,
                                 SVTK_ViewModelBase* theModel)
{
  connect(theView,SIGNAL(KeyPressed(QKeyEvent*)),
          this,SLOT(onKeyPressed(QKeyEvent*)) );
  connect(theView,SIGNAL(KeyReleased(QKeyEvent*)),
          this,SLOT(onKeyReleased(QKeyEvent*)));
  connect(theView,SIGNAL(MouseButtonPressed(QMouseEvent*)),
          this,SLOT(onMousePressed(QMouseEvent*)));
  connect(theView,SIGNAL(MouseButtonReleased(QMouseEvent*)),
          this,SLOT(onMouseReleased(QMouseEvent*)));
  connect(theView,SIGNAL(MouseDoubleClicked(QMouseEvent*)),
          this,SLOT(onMouseDoubleClicked(QMouseEvent*)));
  connect(theView,SIGNAL(MouseMove(QMouseEvent*)),
          this,SLOT(onMouseMoving(QMouseEvent*)));
  connect(theView,SIGNAL(contextMenuRequested(QContextMenuEvent*)),
          this,SIGNAL(contextMenuRequested(QContextMenuEvent *)));
  connect(theView,SIGNAL(selectionChanged()),
          theModel,SLOT(onSelectionChanged()));

  connect( this, SIGNAL( transformed( SVTK_ViewWindow* ) ), SLOT( emitViewModified() ) );
}

/*!
  Destructor
*/
SVTK_ViewWindow::~SVTK_ViewWindow()
{
  myRecorder->Delete();
  myAxesWidget->Delete();
}


/*!
  \return corresponding view
*/
SVTK_View* SVTK_ViewWindow::getView() 
{ 
  return myView; 
}

/*!
  \return corresponding vtk render window
*/
vtkRenderWindow* SVTK_ViewWindow::getRenderWindow()
{
  return GetInteractor()->getRenderWindow();
}

/*!
  \return corresponding vtk render window interactor
*/
SVTK_RenderWindowInteractor* SVTK_ViewWindow::GetInteractor() const
{
  return myInteractor;
}

/*!
  \return corresponding vtk render window interactor
*/
vtkRenderWindowInteractor* SVTK_ViewWindow::getInteractor() const
{
  return myInteractor->GetDevice();
}

/*!
  \return corresponding vtk renderer
*/
vtkRenderer* SVTK_ViewWindow::getRenderer() const
{
  return GetInteractor()->getRenderer();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::GetRenderer
*/
SVTK_Renderer* SVTK_ViewWindow::GetRenderer() const
{
  return GetInteractor()->GetRenderer();
}

/*!
  \return corresponding vtk selector
*/
SVTK_Selector* SVTK_ViewWindow::GetSelector() const
{ 
  return GetInteractor()->GetSelector(); 
}

/*!
  Processes transformation "front view"
*/
void SVTK_ViewWindow::onFrontView()
{
  GetRenderer()->OnFrontView();
  Repaint();
  emit transformed( this );
}

/*!
  Processes transformation "back view"
*/
void SVTK_ViewWindow::onBackView()
{
  GetRenderer()->OnBackView();
  Repaint();
  emit transformed( this );
}

/*!
  Processes transformation "top view"
*/
void SVTK_ViewWindow::onTopView()
{
  GetRenderer()->OnTopView();
  Repaint();
  emit transformed( this );
}

/*!
  Processes transformation "bottom view"
*/
void SVTK_ViewWindow::onBottomView()
{
  GetRenderer()->OnBottomView();
  Repaint();
  emit transformed( this );
}

/*!
  Processes transformation "left view"
*/
void SVTK_ViewWindow::onLeftView()
{
  GetRenderer()->OnLeftView();
  Repaint();
  emit transformed( this );
}

/*!
  Processes transformation "right view"
*/
void SVTK_ViewWindow::onRightView()
{
  GetRenderer()->OnRightView();
  Repaint();
  emit transformed( this );
}

/*!
  \brief Rotate view 90 degrees clockwise
*/
void SVTK_ViewWindow::onClockWiseView()
{
  GetRenderer()->onClockWiseView();
  Repaint();
  emit transformed( this );
}

/*!
  \brief Rotate view 90 degrees conterclockwise
*/
void SVTK_ViewWindow::onAntiClockWiseView()
{
  GetRenderer()->onAntiClockWiseView();
  Repaint();
  emit transformed( this );
}

/*!
  Processes transformation "reset view": sets default orientation of viewport camera
*/
void SVTK_ViewWindow::onResetView()
{
  GetRenderer()->OnResetView();
  Repaint();
  emit transformed( this );
}

/*!
  Processes transformation "fit all"
*/
void SVTK_ViewWindow::onFitAll()
{
  GetRenderer()->OnFitAll();
  Repaint();
  emit transformed( this );
}

/*!
  Processes transformation "fit selection"
*/
void SVTK_ViewWindow::onFitSelection()
{
  GetRenderer()->onFitSelection();
  Repaint();
  emit transformed( this );
}

/*!
  SLOT: called if selection is changed
*/
void SVTK_ViewWindow::onSelectionChanged()
{
  myView->onSelectionChanged();
}

/*!
  Change selection mode
  \param theMode - new selection mode
*/
void SVTK_ViewWindow::SetSelectionMode(Selection_Mode theMode)
{
  GetSelector()->SetSelectionMode(theMode);
}

/*!
  \return selection mode
*/
Selection_Mode SVTK_ViewWindow::SelectionMode() const
{
  return GetSelector()->SelectionMode();
}

/*!
  Unhilights all objects in viewer
*/
void SVTK_ViewWindow::unHighlightAll() 
{
  myView->unHighlightAll();
}

/*!
  Hilights/unhilights object in viewer
  \param theIO - object to be updated
  \param theIsHighlight - if it is true, object will be hilighted, otherwise it will be unhilighted
  \param theIsUpdate - update current viewer
*/
void SVTK_ViewWindow::highlight(const Handle(SALOME_InteractiveObject)& theIO, 
                                bool theIsHighlight, 
                                bool theIsUpdate ) 
{
  myView->highlight( theIO, theIsHighlight, theIsUpdate );
}

/*!
  \return true if object is in viewer or in collector
  \param theIO - object to be checked
*/
bool SVTK_ViewWindow::isInViewer( const Handle(SALOME_InteractiveObject)& theIO ) 
{
  return myView->isInViewer( theIO );
}

/*!
  \return true if object is displayed in viewer
  \param theIO - object to be checked
*/
bool SVTK_ViewWindow::isVisible( const Handle(SALOME_InteractiveObject)& theIO ) 
{
  return myView->isVisible( theIO );
}

/*!
  Display object
  \param theEntry - entry that corresponds to intractive objects
*/
Handle(SALOME_InteractiveObject) SVTK_ViewWindow::FindIObject(const char* theEntry) 
{
  return myView->FindIObject(theEntry);
}

/*!
  Display object
  \param theIO - object
  \param theImmediatly - update viewer
*/
void SVTK_ViewWindow::Display(const Handle(SALOME_InteractiveObject)& theIO,
                              bool theImmediatly) 
{
  myView->Display(theIO,theImmediatly);
}

/*!
  Erase object
  \param theIO - object
  \param theImmediatly - update viewer
*/
void SVTK_ViewWindow::Erase(const Handle(SALOME_InteractiveObject)& theIO,
                            bool theImmediatly) 
{
  myView->Erase(theIO,theImmediatly);
}

/*!
  Display only passed object
  \param theIO - object
*/
void SVTK_ViewWindow::DisplayOnly(const Handle(SALOME_InteractiveObject)& theIO) 
{
  myView->DisplayOnly(theIO);
}

/*!
  Display all objects in view
*/
void SVTK_ViewWindow::DisplayAll() 
{
  myView->DisplayAll();
}

/*!
  Erase all objects in view
*/
void SVTK_ViewWindow::EraseAll() 
{
  myView->EraseAll();
}

/*!
  Sets background color [obsolete]
  \param color - new background color
*/
void SVTK_ViewWindow::setBackgroundColor( const QColor& c )
{
  Qtx::BackgroundData bg = background();
  bg.setColor( c );
  setBackground( bg );
}

/*!
  \return background color of viewer [obsolete]
*/
QColor SVTK_ViewWindow::backgroundColor() const
{
  return background().color();
}

/*!
  Sets background data
  \param bgData - new background data
*/
void SVTK_ViewWindow::setBackground( const Qtx::BackgroundData& bgData )
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
	    aTexture->SetPosition((int)VTKViewer_Texture::Tiled);
	    break;
	  case Qtx::StretchTexture:
	    aTexture->SetPosition((int)VTKViewer_Texture::Stretched);
	    break;
	  case Qtx::CenterTexture:
	    aTexture->SetPosition((int)VTKViewer_Texture::Centered);
	  default:
	    break;
	  }
	  // show textured background
	  getRenderer()->SetTexturedBackground( true );
	  getRenderer()->SetBackgroundTexture( aTexture );
	  
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

/*!
  \return background data of viewer
*/
Qtx::BackgroundData SVTK_ViewWindow::background() const
{
  return myBackground;
}


/*!
  Redirect the request to SVTK_RenderWindowInteractor::GetInteractorStyle
*/
vtkInteractorStyle* SVTK_ViewWindow::GetInteractorStyle() const
{
  return GetInteractor()->GetInteractorStyle();
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::PushInteractorStyle
*/
void SVTK_ViewWindow::PushInteractorStyle(vtkInteractorStyle* theStyle)
{
  GetInteractor()->PushInteractorStyle(theStyle);
}

/*!
  Redirect the request to SVTK_RenderWindowInteractor::PopInteractorStyle
*/
void SVTK_ViewWindow::PopInteractorStyle()
{
  GetInteractor()->PopInteractorStyle();
}

/*!
  Updates current viewer
*/
void SVTK_ViewWindow::Repaint(bool theUpdateTrihedron)
{
  if(theUpdateTrihedron) 
    GetRenderer()->OnAdjustTrihedron();

  GetInteractor()->update();

  SVTK_InteractorStyle* aStyle = (SVTK_InteractorStyle*)getInteractor()->GetInteractorStyle();
  if ( aStyle )
    aStyle->OnTimer();
}

/*!
  Redirect the request to #SVTK_Renderer::GetScale
*/
void SVTK_ViewWindow::GetScale( double theScale[3] ) 
{
  GetRenderer()->GetScale( theScale );
}

/*!
  Redirect the request to #SVTK_Renderer::SetScale
*/
void SVTK_ViewWindow::SetScale( double theScale[3] ) 
{
  GetRenderer()->SetScale( theScale );
  Repaint();
  emit transformed( this );
}

/*!
  Redirect the request to #SVTK_Renderer::IsTrihedronDisplayed
*/
bool SVTK_ViewWindow::isTrihedronDisplayed()
{
  return GetRenderer()->IsTrihedronDisplayed();
}

/*!
  Redirect the request to #SVTK_Renderer::IsCubeAxesDisplayed
*/
bool SVTK_ViewWindow::isCubeAxesDisplayed()
{
  return GetRenderer()->IsCubeAxesDisplayed();
}

/*!
  Redirect the request to #SVTK_Renderer::OnViewTrihedron
*/
void SVTK_ViewWindow::onViewTrihedron(bool show)
{
  GetRenderer()->SetTrihedronVisibility(show);
  Repaint();
}

/*!
  Redirect the request to #SVTK_Renderer::OnViewCubeAxes
*/
void SVTK_ViewWindow::onViewCubeAxes()
{
  GetRenderer()->OnViewCubeAxes();
  Repaint();
}

/*!
  Redirect the request to #SVTK_Renderer::GetTrihedron
*/
VTKViewer_Trihedron* SVTK_ViewWindow::GetTrihedron()
{
  return GetRenderer()->GetTrihedron();
}

/*!
  Redirect the request to #SVTK_Renderer::GetCubeAxes
*/
SVTK_CubeAxesActor2D* SVTK_ViewWindow::GetCubeAxes()
{
  return GetRenderer()->GetCubeAxes();
}

/*!
  \return trihedron size
*/
double SVTK_ViewWindow::GetTrihedronSize() const
{
  return GetRenderer()->GetTrihedronSize();
}

/*!
  Sets projection mode
  \param theMode - projection mode ( 0 - orthogonal, 1 - perspective, 2 - stereo )
*/
void SVTK_ViewWindow::SetProjectionMode(const int theMode)
{
  QtxAction* aParallelAction = dynamic_cast<QtxAction*>( toolMgr()->action( ParallelModeId ) );
  QtxAction* aProjectionAction = dynamic_cast<QtxAction*>( toolMgr()->action( ProjectionModeId ) );
  QtxAction* aStereoAction = dynamic_cast<QtxAction*>( toolMgr()->action( StereoModeId ) );

  switch ( theMode ) {
    case Parallel:
      onProjectionMode( aParallelAction );
      break;
    case Projection:
      onProjectionMode( aProjectionAction );
      break;
    case Stereo:
      onStereoMode( true );
      break;
  }

  // update action state if method is called outside
  SVTK_Viewer* aViewer = dynamic_cast<SVTK_Viewer*>(myModel);
  QtxAction* aSwitchZoomingStyle = dynamic_cast<QtxAction*>( toolMgr()->action( SwitchZoomingStyleId ) );
  if ( theMode == Parallel && !aParallelAction->isChecked() ) {
      aParallelAction->setChecked( true );
	  aSwitchZoomingStyle->setEnabled(true);
	  aStereoAction->setChecked( false );
  }
  if ( theMode == Projection && !aProjectionAction->isChecked() ) {
      aProjectionAction->setChecked( true );
	  aSwitchZoomingStyle->setEnabled(false);
  }
  if ( theMode == Stereo ) {
    aStereoAction->setChecked( true );
    if ( aParallelAction->isEnabled() ) {
        aParallelAction->setEnabled( false );
        aParallelAction->setChecked( false );
        aStereoAction->setChecked( false );
    }
    else {
      aParallelAction->setEnabled( true );
      aStereoAction->setChecked( false );
      aParallelAction->setChecked( aViewer->projectionMode() == Parallel );
    }
    if ( aProjectionAction->isEnabled() ) {
      aProjectionAction->setEnabled( false );
      aProjectionAction->setChecked( true );
      if ( getRenderWindow()->GetStereoCapableWindow() == 1 && !isOpenGlStereoSupport() &&
           strcmp( "CrystalEyes", getRenderWindow()->GetStereoTypeAsString() ) == 0 &&
           toolMgr()->action( StereoModeId )->isChecked() ) {
        SUIT_MessageBox::warning( 0, tr( "WRN_WARNING" ),  tr( "WRN_SUPPORT_QUAD_BUFFER" ) );
      }
    }
    else {
      aProjectionAction->setEnabled( true );
      aStereoAction->setChecked( false );
      aProjectionAction->setChecked( aViewer->projectionMode() == Projection );
      onProjectionMode();
    }
  }
  else {
    if ( !aParallelAction->isEnabled() )
      aParallelAction->setEnabled( true );
    if ( !aProjectionAction->isEnabled() )
      aProjectionAction->setEnabled( true );
  }
}

/*!
  Sets stereo type
  \param theType - stereo type
*/
void SVTK_ViewWindow::SetStereoType(const int theType)
{
  vtkRenderWindow* aWindow = getRenderWindow();
  switch (theType ) {
  case CrystalEyes:
    aWindow->SetStereoTypeToCrystalEyes();
    break;
  case RedBlue:
    aWindow->SetStereoTypeToRedBlue();
    break;
  case Interlaced:
    aWindow->SetStereoTypeToInterlaced();
    break;
  case Left:
    aWindow->SetStereoTypeToLeft();
    break;
  case Right:
    aWindow->SetStereoTypeToRight();
    break;
  case Dresden:
    aWindow->SetStereoTypeToDresden();
    break;
  case Anaglyph:
    aWindow->SetStereoTypeToAnaglyph();
    break;
  case Checkerboard:
    aWindow->SetStereoTypeToCheckerboard();
    break;
  case SplitViewPortHorizontal:
    aWindow->SetStereoTypeToSplitViewportHorizontal();
    break;
  }
}

/*!
  Sets anaglyph filter
  \param theFilter - anaglyph filter
*/
void SVTK_ViewWindow::SetAnaglyphFilter(const int theFilter)
{
  vtkRenderWindow* aWindow = getRenderWindow();
  switch (theFilter ) {
  case RedCyan:
    aWindow->SetAnaglyphColorMask(4,3);
    break;
  case YellowBlue:
    aWindow->SetAnaglyphColorMask(6,1);
    break;
  case GreenMagenta:
    aWindow->SetAnaglyphColorMask(2,5);
    break;
  }
}

/*!
  Set support quad-buffered stereo
  \param theEnable - enable/disable support quad-buffered stereo
*/
void SVTK_ViewWindow::SetQuadBufferSupport(const bool theEnable)
{
  vtkRenderWindow* aWindow = getRenderWindow();
  aWindow->SetStereoCapableWindow((int)theEnable);
}

/*!
  \return OpenGl stereo support
*/
bool SVTK_ViewWindow::isOpenGlStereoSupport() const
{
  GLboolean support[1];
  glGetBooleanv (GL_STEREO, support);
  if ( support[0] )
    return true;
  return false;
}

/*!
  Set the gravity center as a focal point
*/
void SVTK_ViewWindow::activateSetFocalPointGravity()
{
  myEventDispatcher->InvokeEvent(SVTK::SetFocalPointGravity, 0);
}

/*!
  Set the selected point as a focal point
*/
void SVTK_ViewWindow::activateSetFocalPointSelected()
{
  myEventDispatcher->InvokeEvent(SVTK::SetFocalPointSelected, 0);
}

/*!
  Set the point selected by user as a focal point
*/
void SVTK_ViewWindow::activateStartFocalPointSelection()
{
  myEventDispatcher->InvokeEvent(SVTK::StartFocalPointSelection,0);
}

void SVTK_ViewWindow::activateProjectionMode(int theMode)
{
  QtxAction* aParallelAction = dynamic_cast<QtxAction*>( toolMgr()->action( ParallelModeId ) );
  QtxAction* aProjectionAction = dynamic_cast<QtxAction*>( toolMgr()->action( ProjectionModeId ) );
  if (theMode)
    aParallelAction->setChecked( true );
  else
    aProjectionAction->setChecked( true );

  if ( !aParallelAction->isEnabled() )
    aParallelAction->setEnabled( true );
  if ( !aProjectionAction->isEnabled() )
    aProjectionAction->setEnabled( true );
}

/*!
  Sets actual interaction style
  \param theStyle - type of interaction style ( 0 - standard, 1 - keyboard free )
*/
void SVTK_ViewWindow::SetInteractionStyle(const int theStyle)
{
  onSwitchInteractionStyle( theStyle==1 );
}

/*!
  Sets actual zooming style
  \param theStyle - type of zooming style ( 0 - standard, 1 - advanced (at cursor) )
*/
void SVTK_ViewWindow::SetZoomingStyle(const int theStyle)
{
  onSwitchZoomingStyle( theStyle==1 );
}

/*!
  Set preselection mode.
  \param theMode the mode to set (standard, dynamic or disabled)
*/
void SVTK_ViewWindow::SetPreSelectionMode( Preselection_Mode theMode )
{
  onSwitchPreSelectionMode( theMode );
}

/*!
  Enables/disables selection.
  \param theEnable if true - selection will be enabled
*/
void SVTK_ViewWindow::SetSelectionEnabled( bool theEnable )
{
  GetSelector()->SetSelectionEnabled( theEnable );
  QtxAction* a = getAction( EnableSelectionId );
  if ( a->isChecked() !=  theEnable)
    a->setChecked( theEnable );
  QtxActionGroup* aPreselectionGroup = 
    dynamic_cast<QtxActionGroup*>( getAction( PreselectionId ) );
  if ( aPreselectionGroup )
    aPreselectionGroup->setEnabled( theEnable );

  // notify actors
  vtkActorCollection *actors = getRenderer()->GetActors();
  for (int i = 0; i < actors->GetNumberOfItems(); ++i )
    if (VTKViewer_Actor *actor = dynamic_cast<VTKViewer_Actor*>(actors->GetItemAsObject(i)))
    {
      cout << "actor " << actor << endl;
      actor->EnableSelection( theEnable );
    }
}

/*!
  Switches "keyboard free" interaction style on/off
*/
void SVTK_ViewWindow::onSwitchInteractionStyle(bool theOn)
{
  if (theOn) {
    // check if style is already set
    if ( GetInteractorStyle() != myKeyFreeInteractorStyle.GetPointer() )
    {
      // keep the same style extensions
      SVTK_InteractorStyle* aStyle = (SVTK_InteractorStyle*)GetInteractorStyle();
      if ( aStyle ) {
        myKeyFreeInteractorStyle->SetControllerIncrement(aStyle->ControllerIncrement());
        myKeyFreeInteractorStyle->SetControllerOnKeyDown(aStyle->ControllerOnKeyDown());
      }

      PushInteractorStyle(myKeyFreeInteractorStyle.GetPointer());
    }
  }
  else {
    // pop only key free  style
    if ( GetInteractorStyle() == myKeyFreeInteractorStyle.GetPointer() )
      PopInteractorStyle();
  }

  // update action state if method is called outside
  QtxAction* a = getAction( SwitchInteractionStyleId );
  if ( a->isChecked() != theOn ) a->setChecked( theOn );
}

/*!
  Toogles advanced zooming style (relatively to the cursor position) on/off
*/
void SVTK_ViewWindow::onSwitchZoomingStyle( bool theOn )
{
  if( myDefaultInteractorStyle.GetPointer() )
    myDefaultInteractorStyle->SetAdvancedZoomingEnabled( theOn );
  if( myKeyFreeInteractorStyle.GetPointer() )
    myKeyFreeInteractorStyle->SetAdvancedZoomingEnabled( theOn );

  // update action state if method is called outside
  QtxAction* a = getAction( SwitchZoomingStyleId );
  if ( a->isChecked() != theOn )
    a->setChecked( theOn );
}

/*!
  Switch preselection mode.
  \param theMode the preselection mode
*/
void SVTK_ViewWindow::onSwitchPreSelectionMode( int theMode )
{
  GetSelector()->SetDynamicPreSelection( theMode == Dynamic_Preselection );
  GetSelector()->SetPreSelectionEnabled( theMode != Preselection_Disabled );

  // update action state if method is called outside
  QtxAction* a = getAction( StandardPreselectionId + theMode );
  if ( a && !a->isChecked() )
    a->setChecked( true );
}

/*!
  Enables/disables selection.
  \param theOn if true - selection will be enabled
*/
void SVTK_ViewWindow::onEnableSelection( bool on )
{
  SVTK_Viewer* aViewer = dynamic_cast<SVTK_Viewer*>(myModel);
  if(aViewer)
    aViewer->enableSelection(on);  
}

/*!
  Sets incremental speed
  \param theValue - new incremental speed
  \param theMode - modification mode
*/
void SVTK_ViewWindow::SetIncrementalSpeed(const int theValue, const int theMode)
{
  if ( (SVTK_InteractorStyle*)GetInteractorStyle() )
    ((SVTK_InteractorStyle*)GetInteractorStyle())->SetIncrementSpeed(theValue, theMode);
}

/*!
  Sets spacemouse buttons for the functions
  \param theBtn1 - spacemouse button for the "decrease speed increment"
  \param theBtn2 - spacemouse button for the "increase speed increment"
  \param theBtn3 - spacemouse button for the "dominant combined switch"
*/
void SVTK_ViewWindow::SetSpacemouseButtons(const int theBtn1, 
                                           const int theBtn2,
                                           const int theBtn3)
{
  int val = theBtn1;
  myEventDispatcher->InvokeEvent(SVTK::SetSMDecreaseSpeedEvent, &val);
  val = theBtn2;
  myEventDispatcher->InvokeEvent(SVTK::SetSMIncreaseSpeedEvent, &val);
  val = theBtn3;
  myEventDispatcher->InvokeEvent(SVTK::SetSMDominantCombinedSwitchEvent, &val);
}

/*!
  Sets trihedron size
  \param theSize - new trihedron size
  \param theRelative - trihedron relativeness
*/
void SVTK_ViewWindow::SetTrihedronSize(const double theSize, const bool theRelative)
{
  GetRenderer()->SetTrihedronSize(theSize, theRelative);
  Repaint();
}

/*! If parameter theIsForcedUpdate is true, recalculate parameters for
 *  trihedron and cube axes, even if trihedron and cube axes is invisible.
 */
void SVTK_ViewWindow::AdjustTrihedrons(const bool theIsForcedUpdate)
{
  GetRenderer()->AdjustActors();
  Repaint();
}

/*!
  Redirect the request to #SVTK_Renderer::OnAdjustTrihedron
*/
void SVTK_ViewWindow::onAdjustTrihedron()
{   
  GetRenderer()->OnAdjustTrihedron();
}

/*!
  Redirect the request to #SVTK_Renderer::OnAdjustCubeAxes
*/
void SVTK_ViewWindow::onAdjustCubeAxes()
{   
  GetRenderer()->OnAdjustCubeAxes();
}

void SVTK_ViewWindow::synchronize(SVTK_ViewWindow* otherViewWindow )
{
  if ( otherViewWindow ) {
    bool blocked = blockSignals( true );
    doSetVisualParameters( otherViewWindow->getVisualParameters(), true );
    blockSignals( blocked );
  }
}

/*!
  Emits key pressed
*/
void SVTK_ViewWindow::onKeyPressed(QKeyEvent* event)
{
  emit keyPressed( this, event );
}

/*!
  Emits key released
*/
void SVTK_ViewWindow::onKeyReleased(QKeyEvent* event)
{
  emit keyReleased( this, event );
}

/*!
  Emits mouse pressed
*/
void SVTK_ViewWindow::onMousePressed(QMouseEvent* event)
{
  emit mousePressed(this, event);
}

/*!
  Emits mouse released
*/
void SVTK_ViewWindow::onMouseReleased(QMouseEvent* event)
{
  emit mouseReleased( this, event );
}

/*!
  Emits mouse moving
*/
void SVTK_ViewWindow::onMouseMoving(QMouseEvent* event)
{
  emit mouseMoving( this, event );
}

/*!
  Emits mouse double clicked
*/
void SVTK_ViewWindow::onMouseDoubleClicked( QMouseEvent* event )
{
  emit mouseDoubleClicked( this, event );
}

/*!
  Redirect the request to #SVTK_Renderer::AddActor
*/
void SVTK_ViewWindow::AddActor( VTKViewer_Actor* theActor, 
                                bool theUpdate,
                                bool theIsAdjustActors )
{
  GetRenderer()->AddActor(theActor, theIsAdjustActors);
  if(theUpdate) 
    Repaint();
  emit actorAdded(theActor);
}

/*!
  Redirect the request to #SVTK_Renderer::RemoveActor
*/
void SVTK_ViewWindow::RemoveActor( VTKViewer_Actor* theActor, 
                                   bool theUpdate,
                                   bool theIsAdjustActors )
{
  GetRenderer()->RemoveActor(theActor, theIsAdjustActors);
  if ( myDefaultInteractorStyle )
    myDefaultInteractorStyle->FreeActors();
  if ( myKeyFreeInteractorStyle )
    myKeyFreeInteractorStyle->FreeActors();
  if(theUpdate) 
    Repaint();
  emit actorRemoved(theActor);
}

QImage SVTK_ViewWindow::dumpViewContent()
{
  vtkRenderWindow* aWindow = getRenderWindow();
  int* aSize = aWindow->GetSize();
  int aWidth = aSize[0];
  int aHeight = aSize[1];
  
#ifndef DISABLE_GLVIEWER
  OpenGLUtils_FrameBuffer aFrameBuffer;
  if( aFrameBuffer.init( aWidth, aHeight ) )
  {
    glPushAttrib( GL_VIEWPORT_BIT );
    glViewport( 0, 0, aWidth, aHeight );
    aFrameBuffer.bind();

    // draw scene
    aWindow->Render();

    aFrameBuffer.unbind();
    glPopAttrib();

    QImage anImage( aWidth, aHeight, QImage::Format_RGB32 );

    aFrameBuffer.bind();
    glReadPixels( 0, 0, aWidth, aHeight, GL_RGBA, GL_UNSIGNED_BYTE, anImage.bits() );
    aFrameBuffer.unbind();

    anImage = anImage.rgbSwapped();
    anImage = anImage.mirrored();
    return anImage;
  }
#endif

  // if frame buffers are unsupported, use old functionality
  unsigned char *aData = 
    aWindow->GetRGBACharPixelData( 0, 0, aWidth-1, aHeight-1, 0 );
  
  QImage anImage( aData, aWidth, aHeight, QImage::Format_ARGB32 );

  anImage = anImage.rgbSwapped();
  anImage = anImage.mirrored();
  return anImage;
}

/*!
  \return QImage, containing all scene rendering in window
*/
QImage SVTK_ViewWindow::dumpView()
{
  if( myDumpImage.isNull() )
    return dumpViewContent();
  
  RefreshDumpImage();
  return myDumpImage;
}

QString SVTK_ViewWindow::filter() const
{
  return tr( "SVTK_IMAGE_FILES" );
}

bool SVTK_ViewWindow::dumpViewToFormat( const QImage& img, const QString& fileName, const QString& format )
{
  if ( format != "PS" && format != "EPS" && format != "PDF" )
    return SUIT_ViewWindow::dumpViewToFormat( img, fileName, format );

  SVTK_PsOptionsDlg* optionsDlg = new SVTK_PsOptionsDlg(this);
  if ( optionsDlg->exec() == QDialog::Accepted ) {
    SUIT_OverrideCursor wc;

    vtkGL2PSExporter *anExporter = vtkGL2PSExporter::New();
    anExporter->SetRenderWindow(getRenderWindow());

    // Set options
    anExporter->SetLineWidthFactor(optionsDlg->getLineFactor());
    anExporter->SetPointSizeFactor(optionsDlg->getPointFactor());
    anExporter->SetSort((vtkGL2PSExporter::SortScheme)optionsDlg->getSortType());
    anExporter->SetWrite3DPropsAsRasterImage((int)optionsDlg->isRasterize3D());
    anExporter->SetPS3Shading((int)optionsDlg->isPs3Shading());
    
    if ( format == "PS" ) {
      anExporter->SetFileFormatToPS();
      anExporter->CompressOff();
    }
    
    if ( format == "EPS" ) {
      anExporter->SetFileFormatToEPS();
      anExporter->CompressOff();
    }

    if ( format == "PDF" ) {
      anExporter->SetFileFormatToPDF();
    }
    
    QString aFilePrefix(fileName);
    QString anExtension(SUIT_Tools::extension(fileName));
    aFilePrefix.truncate(aFilePrefix.length() - 1 - anExtension.length());
    anExporter->SetFilePrefix(aFilePrefix.toLatin1().data());
    anExporter->Write();
    anExporter->Delete();
  }
  delete optionsDlg;
  return true;  
}

/*!
  \refresh QImage, containing all scene rendering in window
*/
void SVTK_ViewWindow::RefreshDumpImage()
{
  myDumpImage = dumpViewContent();
}

/*!
  Redirect the request to #SVTK_Renderer::SetSelectionProp
*/
void SVTK_ViewWindow::SetSelectionProp(const double& theRed, 
                                       const double& theGreen, 
                                       const double& theBlue, 
                                       const int& theWidth) 
{
  myView->SetSelectionProp(theRed,theGreen,theBlue,theWidth);
}

/*!
  Redirect the request to #SVTK_Renderer::SetSelectionProp
*/
void SVTK_ViewWindow::SetPreselectionProp(const double& theRed, 
                                          const double& theGreen, 
                                          const double& theBlue, 
                                          const int& theWidth) 
{
  myView->SetPreselectionProp(theRed,theGreen,theBlue,theWidth);
}

/*!
  Redirect the request to #SVTK_Renderer::SetSelectionTolerance
*/
void SVTK_ViewWindow::SetSelectionTolerance(const double& theTolNodes, 
                                            const double& theTolItems,
                                            const double& theTolObjects)
{
  myView->SetSelectionTolerance(theTolNodes, theTolItems, theTolObjects);
}

/*!
  Get visibility status of the static trihedron
*/
bool SVTK_ViewWindow::IsStaticTrihedronVisible() const
{
  return (bool)myAxesWidget->GetEnabled();
}

/*!
  Set visibility status of the static trihedron
*/
void SVTK_ViewWindow::SetStaticTrihedronVisible( const bool theIsVisible )
{
  myAxesWidget->SetEnabled( (int)theIsVisible );
}

/*!
  Performs action
  \param accelAction - action
*/
bool SVTK_ViewWindow::action( const int accelAction  )
{
  if ( accelAction == SUIT_Accel::ZoomFit )
    onFitAll();
  else {
    int anEvent = SVTK::convertAction( accelAction );
    GetInteractor()->InvokeEvent(anEvent, 0);
  }
  return true;
}

/*!
  \return action by it's id
*/
QtxAction* SVTK_ViewWindow::getAction( int id ) const
{
  return dynamic_cast<QtxAction*>( toolMgr()->action( id ) );
}


// old visual parameters had 13 values.  New format added additional 
// 76 values for graduated axes, so both numbers are processed.
const int nNormalParams = 13;   // number of view windows parameters excluding graduated axes params
const int nGradAxisParams = 25; // number of parameters of ONE graduated axis (X, Y, or Z)
const int nTrihedronParams = 3; // number of parameters for Trihedron
const int nAllParams = nNormalParams + 3*nGradAxisParams + nTrihedronParams + 1; // number of all visual parameters

/*! The method returns visual parameters of a graduated axis actor (x,y,z axis of graduated axes)
 */
void getGradAxisVisualParams( QXmlStreamWriter& writer, vtkAxisActor2D* actor, QString theAxis )
{
  //QString params;
  if ( !actor )
    return ;//params;

  // Name
  bool isVisible = actor->GetTitleVisibility();
  QString title ( actor->GetTitle() );
  double color[ 3 ];
  int font = VTK_ARIAL;
  int bold = 0;
  int italic = 0;
  int shadow = 0;

  vtkTextProperty* txtProp = actor->GetTitleTextProperty();
  if ( txtProp )
  {
    txtProp->GetColor( color );
    font = txtProp->GetFontFamily();
    bold = txtProp->GetBold();
    italic = txtProp->GetItalic();
    shadow = txtProp->GetShadow();
  }
  writer.writeStartElement("GraduatedAxis");
  writer.writeAttribute("Axis", theAxis);

  writer.writeStartElement("Title");
  writer.writeAttribute("isVisible", QString("%1").arg(isVisible));
  writer.writeAttribute("Text", title);
  writer.writeAttribute("Font", QString("%1").arg(font));
  writer.writeAttribute("Bold", QString("%1").arg(bold));
  writer.writeAttribute("Italic", QString("%1").arg(italic));
  writer.writeAttribute("Shadow", QString("%1").arg(shadow));

  writer.writeStartElement("Color");
  writer.writeAttribute("R", QString("%1").arg(color[0]));
  writer.writeAttribute("G", QString("%1").arg(color[1]));
  writer.writeAttribute("B", QString("%1").arg(color[2]));
  writer.writeEndElement();
  writer.writeEndElement();

  //params.sprintf( "* Graduated Axis: * Name *%u*%s*%.2f*%.2f*%.2f*%u*%u*%u*%u", isVisible, 
  //              title.toLatin1().data(), color[0], color[1], color[2], font, bold, italic, shadow );

  // Labels
  isVisible = actor->GetLabelVisibility();
  int labels = actor->GetNumberOfLabels();
  int offset = actor->GetTickOffset();
  font = VTK_ARIAL;
  bold = false;
  italic = false;
  shadow = false;

  txtProp = actor->GetLabelTextProperty();
  if ( txtProp )
  {
    txtProp->GetColor( color );
    font = txtProp->GetFontFamily();
    bold = txtProp->GetBold();
    italic = txtProp->GetItalic();
    shadow = txtProp->GetShadow();
  }

  writer.writeStartElement("Labels");
  writer.writeAttribute("isVisible", QString("%1").arg(isVisible));
  writer.writeAttribute("Number", QString("%1").arg(labels));
  writer.writeAttribute("Offset", QString("%1").arg(offset));
  writer.writeAttribute("Font", QString("%1").arg(font));
  writer.writeAttribute("Bold", QString("%1").arg(bold));
  writer.writeAttribute("Italic", QString("%1").arg(italic));
  writer.writeAttribute("Shadow", QString("%1").arg(shadow));

  writer.writeStartElement("Color");
  writer.writeAttribute("R", QString("%1").arg(color[0]));
  writer.writeAttribute("G", QString("%1").arg(color[1]));
  writer.writeAttribute("B", QString("%1").arg(color[2]));
  writer.writeEndElement();
  writer.writeEndElement();
  //  params += QString().sprintf( "* Labels *%u*%u*%u*%.2f*%.2f*%.2f*%u*%u*%u*%u", isVisible, labels, offset,  
  //                           color[0], color[1], color[2], font, bold, italic, shadow );

  // Tick marks
  isVisible = actor->GetTickVisibility();
  int length = actor->GetTickLength();
  writer.writeStartElement("TickMarks");
  writer.writeAttribute("isVisible", QString("%1").arg(isVisible));
  writer.writeAttribute("Length", QString("%1").arg(length));
  writer.writeEndElement();
  
  //params += QString().sprintf( "* Tick marks *%u*%u", isVisible, length );
  
  writer.writeEndElement();
  //return params;
}

void setGradAxisVisualParams(QXmlStreamReader& reader, vtkAxisActor2D* actor)
{
  if ( !actor )
    return;

  do {
    reader.readNext();
  } while (!reader.isStartElement());

  // Read title params
  QXmlStreamAttributes aAttr = reader.attributes();
  bool isVisible = aAttr.value("isVisible").toString().toUShort();
  QString title = aAttr.value("Text").toString();
  int font = aAttr.value("Font").toString().toInt();
  int bold = aAttr.value("Bold").toString().toInt();
  int italic = aAttr.value("Italic").toString().toInt();
  int shadow = aAttr.value("Shadow").toString().toInt();

  //printf("#### TITLE: %i, %s, %i, %i, %i, %i\n", isVisible, qPrintable(title), font, bold, italic, shadow);

  do {
    reader.readNext();
  } while (!reader.isStartElement());
  
  // Read title color
  aAttr = reader.attributes();

  double color[3];
  color[0] = aAttr.value("R").toString().toDouble();
  color[1] = aAttr.value("G").toString().toDouble();
  color[2] = aAttr.value("B").toString().toDouble();
  //printf("#### Color: %f, %f, %f\n", color[0], color[1], color[2]);

  actor->SetTitleVisibility( isVisible );
  actor->SetTitle( title.toLatin1() );
  vtkTextProperty* txtProp = actor->GetTitleTextProperty();
  if ( txtProp ) {
    txtProp->SetColor( color );
    txtProp->SetFontFamily( font );
    txtProp->SetBold( bold );
    txtProp->SetItalic( italic );
    txtProp->SetShadow( shadow );
  }

  // Labels

  do {
    reader.readNext();
  } while (!reader.isStartElement()); 
  // Read labels
  aAttr = reader.attributes();
  isVisible = aAttr.value("isVisible").toString().toUShort();
  int labels = aAttr.value("Number").toString().toInt();
  int offset = aAttr.value("Offset").toString().toInt();
  font = aAttr.value("Font").toString().toInt();
  bold = aAttr.value("Bold").toString().toInt();
  italic = aAttr.value("Italic").toString().toInt();
  shadow = aAttr.value("Shadow").toString().toInt();

  do {
    reader.readNext();
  } while (!reader.isStartElement()); 
  // Read Color
  aAttr = reader.attributes();

  color[0] = aAttr.value("R").toString().toDouble();
  color[1] = aAttr.value("G").toString().toDouble();
  color[2] = aAttr.value("B").toString().toDouble();

  actor->SetLabelVisibility( isVisible );
  actor->SetNumberOfLabels( labels );
  actor->SetTickOffset( offset );
  txtProp = actor->GetLabelTextProperty();
  if ( txtProp ) {
    txtProp->SetColor( color );
    txtProp->SetFontFamily( font );
    txtProp->SetBold( bold );
    txtProp->SetItalic( italic );
    txtProp->SetShadow( shadow );
  }

  // Tick Marks
  do {
    reader.readNext();
  } while (!reader.isStartElement()); 
  aAttr = reader.attributes();

  // retrieve and set tick marks properties
  isVisible = aAttr.value("isVisible").toString().toUShort();
  int length = aAttr.value("Length").toString().toInt();
  
  actor->SetTickVisibility( isVisible );
  actor->SetTickLength( length );
}

/*! The method restores visual parameters of a graduated axis actor (x,y,z axis)
 */
void setGradAxisVisualParams( vtkAxisActor2D* actor, const QString& params )
{
  if ( !actor )
    return;

  QStringList paramsLst = params.split( '*' );

  if ( paramsLst.size() == nGradAxisParams ) { // altogether name, lable, ticks parameters make up 25 values

    // retrieve and set name parameters
    bool isVisible = paramsLst[2].toUShort();
    QString title = paramsLst[3];
    double color[3];
    color[0] = paramsLst[4].toDouble();
    color[1] = paramsLst[5].toDouble();
    color[2] = paramsLst[6].toDouble();
    int font = paramsLst[7].toInt();
    int bold = paramsLst[8].toInt();
    int italic = paramsLst[9].toInt();
    int shadow = paramsLst[10].toInt();

    actor->SetTitleVisibility( isVisible );
    actor->SetTitle( title.toLatin1() );
    vtkTextProperty* txtProp = actor->GetTitleTextProperty();
    if ( txtProp ) {
      txtProp->SetColor( color );
      txtProp->SetFontFamily( font );
      txtProp->SetBold( bold );
      txtProp->SetItalic( italic );
      txtProp->SetShadow( shadow );
    }

    // retrieve and set lable parameters
    isVisible = paramsLst[12].toUShort();
    int labels = paramsLst[13].toInt();
    int offset = paramsLst[14].toInt();
    color[0] = paramsLst[15].toDouble();
    color[1] = paramsLst[16].toDouble();
    color[2] = paramsLst[17].toDouble();
    font = paramsLst[18].toInt();
    bold = paramsLst[19].toInt();
    italic = paramsLst[20].toInt();
    shadow = paramsLst[21].toInt();

    actor->SetLabelVisibility( isVisible );
    actor->SetNumberOfLabels( labels );
    actor->SetTickOffset( offset );
    txtProp = actor->GetLabelTextProperty();
    if ( txtProp ) {
      txtProp->SetColor( color );
      txtProp->SetFontFamily( font );
      txtProp->SetBold( bold );
      txtProp->SetItalic( italic );
      txtProp->SetShadow( shadow );
    }

    // retrieve and set tick marks properties
    isVisible = paramsLst[23].toUShort();
    int length = paramsLst[24].toInt();

    actor->SetTickVisibility( isVisible );
    actor->SetTickLength( length );
  }
}

/*! The method returns the visual parameters of this view as a formated string
 */
QString SVTK_ViewWindow::getVisualParameters()
{
  double pos[3], focalPnt[3], viewUp[3], parScale, scale[3];
  
  // save position, focal point, viewUp, scale
  vtkCamera* camera = getRenderer()->GetActiveCamera();
  camera->GetPosition( pos );
  camera->GetFocalPoint( focalPnt );
  camera->GetViewUp( viewUp );
  parScale = camera->GetParallelScale();
  GetScale( scale );

  // Parameters are given in the following format:view position (3 digits), focal point position (3 digits)
  // view up values (3 digits), parallel scale (1 digit), scale (3 digits, 
  // Graduated axes parameters (X, Y, Z axes parameters)
  QString retStr;
  QXmlStreamWriter aWriter(&retStr);
  aWriter.setAutoFormatting(true);

  aWriter.writeStartDocument();
  aWriter.writeStartElement("ViewState");

  aWriter.writeStartElement("Position");
  aWriter.writeAttribute("X", QString("%1").arg(pos[0]));
  aWriter.writeAttribute("Y", QString("%1").arg(pos[1]));
  aWriter.writeAttribute("Z", QString("%1").arg(pos[2]));
  aWriter.writeEndElement();

  aWriter.writeStartElement("FocalPoint");
  aWriter.writeAttribute("X", QString::number(focalPnt[0]));
  aWriter.writeAttribute("Y", QString::number(focalPnt[1]));
  aWriter.writeAttribute("Z", QString::number(focalPnt[2]));
  aWriter.writeEndElement();

  aWriter.writeStartElement("ViewUp");
  aWriter.writeAttribute("X", QString::number(viewUp[0]));
  aWriter.writeAttribute("Y", QString::number(viewUp[1]));
  aWriter.writeAttribute("Z", QString::number(viewUp[2]));
  aWriter.writeEndElement();

  aWriter.writeStartElement("ViewScale");
  aWriter.writeAttribute("Parallel", QString::number(parScale));
  aWriter.writeAttribute("X", QString::number(scale[0]));
  aWriter.writeAttribute("Y", QString::number(scale[1]));
  aWriter.writeAttribute("Z", QString::number(scale[2]));
  aWriter.writeEndElement();

  if ( SVTK_CubeAxesActor2D* gradAxesActor = GetCubeAxes() ) {
    aWriter.writeStartElement("DisplayCubeAxis");
    aWriter.writeAttribute("Show", QString( "%1" ).arg( GetRenderer()->IsCubeAxesDisplayed()));
    aWriter.writeEndElement();

    getGradAxisVisualParams(aWriter, gradAxesActor->GetXAxisActor2D(), "X");
    getGradAxisVisualParams(aWriter, gradAxesActor->GetYAxisActor2D(), "Y");
    getGradAxisVisualParams(aWriter, gradAxesActor->GetZAxisActor2D(), "Z");
  }

  aWriter.writeStartElement("Trihedron");
  aWriter.writeAttribute("isShown",  QString( "%1" ).arg( isTrihedronDisplayed()));
  aWriter.writeAttribute("Size", QString::number(GetTrihedronSize()));
  aWriter.writeEndElement();

  aWriter.writeStartElement("Background");
  aWriter.writeAttribute("Value",  QString( "%1" ).arg( Qtx::backgroundToString(background()) ));
  aWriter.writeEndElement();

  aWriter.writeEndElement();
  aWriter.writeEndDocument();

  return retStr;
}

/*!
  The method restores visual parameters of this view or postpones it untill the view is shown
*/ 
void SVTK_ViewWindow::setVisualParameters( const QString& parameters )
{
  //printf("#### %s\n", qPrintable(parameters));
  SVTK_RenderWindowInteractor* anInteractor = GetInteractor();
  if ( anInteractor->isVisible() ) {
    doSetVisualParameters( parameters ); 
  }
  else {
    myVisualParams = parameters;
    anInteractor->installEventFilter(this);
  }
}

/*!
  The method restores visual parameters of this view from a formated string
*/
void SVTK_ViewWindow::doSetVisualParameters( const QString& parameters, bool baseParamsOnly )
{
  
  double pos[3], focalPnt[3], viewUp[3], parScale, scale[3];

  QXmlStreamReader aReader(parameters);
  SVTK_CubeAxesActor2D* gradAxesActor = GetCubeAxes();

  while(!aReader.atEnd()) {
    aReader.readNext();
    if (aReader.isStartElement()) {
      QXmlStreamAttributes aAttr = aReader.attributes();
      //printf("### Name = %s\n", qPrintable(aReader.name().toString()));
      if (aReader.name() == "Position") {       
        pos[0] = aAttr.value("X").toString().toDouble();
        pos[1] = aAttr.value("Y").toString().toDouble();
        pos[2] = aAttr.value("Z").toString().toDouble();
        //printf("#### Position %f; %f; %f\n", pos[0], pos[1], pos[2]);
      }
      else if (aReader.name() == "FocalPoint") {
        focalPnt[0] = aAttr.value("X").toString().toDouble();
        focalPnt[1] = aAttr.value("Y").toString().toDouble();
        focalPnt[2] = aAttr.value("Z").toString().toDouble();
        //printf("#### FocalPoint %f; %f; %f\n", focalPnt[0], focalPnt[1], focalPnt[2]);
      }
      else if (aReader.name() == "ViewUp") {
        viewUp[0] = aAttr.value("X").toString().toDouble();
        viewUp[1] = aAttr.value("Y").toString().toDouble();
        viewUp[2] = aAttr.value("Z").toString().toDouble();
        //printf("#### ViewUp %f; %f; %f\n", viewUp[0], viewUp[1], viewUp[2]);
      }
      else if (aReader.name() == "ViewScale") {
        parScale = aAttr.value("Parallel").toString().toDouble();
        scale[0] = aAttr.value("X").toString().toDouble();
        scale[1] = aAttr.value("Y").toString().toDouble();
        scale[2] = aAttr.value("Z").toString().toDouble();
        //printf("#### ViewScale %f; %f; %f\n", scale[0], scale[1], scale[2]);
      } 
      else if (aReader.name() == "DisplayCubeAxis") {
	if ( !baseParamsOnly ) {
	  if (aAttr.value("Show") == "0")
	    gradAxesActor->VisibilityOff();
	  else
	    gradAxesActor->VisibilityOn();
	}
      }
      else if (aReader.name() == "GraduatedAxis") {
	if ( !baseParamsOnly ) {
	  if(aAttr.value("Axis") == "X") 
	    setGradAxisVisualParams(aReader, gradAxesActor->GetXAxisActor2D());
	  else if(aAttr.value("Axis") == "Y")
	    setGradAxisVisualParams(aReader, gradAxesActor->GetYAxisActor2D());
	  else if(aAttr.value("Axis") == "Z")
	    setGradAxisVisualParams(aReader, gradAxesActor->GetZAxisActor2D());
	}
      } 
      else if (aReader.name() == "Trihedron") {
	if ( !baseParamsOnly ) {
	  if (aAttr.value("isShown") == "0")
	    GetTrihedron()->VisibilityOff();
	  else
	    GetTrihedron()->VisibilityOn();
	  SetTrihedronSize(aAttr.value("Size").toString().toDouble());
	}
      }
      else if (aReader.name() == "Background") {
	if ( !baseParamsOnly ) {
	  setBackground( Qtx::stringToBackground( aAttr.value("Value").toString() ) );
	}
      }
    }
  }
  if (!aReader.hasError()) {
    vtkCamera* camera = getRenderer()->GetActiveCamera();
    camera->SetPosition( pos );
    camera->SetFocalPoint( focalPnt );
    camera->SetViewUp( viewUp );
    camera->SetParallelScale( parScale );
    GetRenderer()->SetScale( scale );
    //SetScale( scale );
  }
  else {
    QStringList paramsLst = parameters.split( '*' );
    if ( paramsLst.size() >= nNormalParams ) {
      // 'reading' list of parameters
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
      
      // applying parameters
      vtkCamera* camera = getRenderer()->GetActiveCamera();
      camera->SetPosition( pos );
      camera->SetFocalPoint( focalPnt );
      camera->SetViewUp( viewUp );
      camera->SetParallelScale( parScale );
      GetRenderer()->SetScale( scale );
      //SetScale( scale );
      
      // apply graduated axes parameters
      if ( !baseParamsOnly ) {
	SVTK_CubeAxesActor2D* gradAxesActor = GetCubeAxes();
	if ( gradAxesActor && paramsLst.size() == nAllParams ) {
	  int i = nNormalParams+1, j = i + nGradAxisParams - 1;
	  ::setGradAxisVisualParams( gradAxesActor->GetXAxisActor2D(), parameters.section( '*', i, j ) ); 
	  i = j + 1; j += nGradAxisParams;
	  ::setGradAxisVisualParams( gradAxesActor->GetYAxisActor2D(), parameters.section( '*', i, j ) ); 
	  i = j + 1; j += nGradAxisParams;
	  ::setGradAxisVisualParams( gradAxesActor->GetZAxisActor2D(), parameters.section( '*', i, j ) ); 
        
	  if ( paramsLst[13].toUShort() )
	    gradAxesActor->VisibilityOn();
	  else
	    gradAxesActor->VisibilityOff();
	}
	else if ( paramsLst.size() == nAllParams ) {
	  if ( paramsLst[90].toUShort() )
	    GetTrihedron()->VisibilityOn();
	  else
	    GetTrihedron()->VisibilityOff();
        
	  SetTrihedronSize(paramsLst[91].toDouble());
	}
      }
    }
  }
  Repaint();
}


/*!
  Delayed setVisualParameters
*/
bool SVTK_ViewWindow::eventFilter( QObject* theWatched, QEvent* theEvent )
{
  if ( theEvent->type() == QEvent::Show && theWatched->inherits( "SVTK_RenderWindowInteractor" ) ) {
    SVTK_RenderWindowInteractor* anInteractor = (SVTK_RenderWindowInteractor*)theWatched;
    if ( anInteractor->isVisible() ) {
      doSetVisualParameters( myVisualParams );
      anInteractor->removeEventFilter( this ); // theWatched = RenderWindowInteractor
    }
  }
  return SUIT_ViewWindow::eventFilter( theWatched, theEvent );
}


/*!
  Change rotation point
*/
void SVTK_ViewWindow::onChangeRotationPoint(bool theIsActivate)
{
  if(theIsActivate){
    mySetRotationPointDlg->addObserver();
    if ( mySetRotationPointDlg->IsFirstShown() )
      activateSetRotationGravity();
    mySetRotationPointDlg->show();
  }else
    mySetRotationPointDlg->hide();
}

/*!
  Set the gravity center as a rotation point
*/
void SVTK_ViewWindow::activateSetRotationGravity()
{
  myEventDispatcher->InvokeEvent(SVTK::SetRotateGravity,0);
}

/*!
  Set the selected point as a rotation point
*/
void SVTK_ViewWindow::activateSetRotationSelected(void* theData)
{
  myEventDispatcher->InvokeEvent(SVTK::ChangeRotationPoint,theData);
}

/*!
  Set the gravity center of element selected by user as a rotation point
*/
void SVTK_ViewWindow::activateStartPointSelection( Selection_Mode theSelectionMode )
{
  SetSelectionMode( theSelectionMode );
  myEventDispatcher->InvokeEvent(SVTK::StartPointSelection,0);
}

/*!
  \brief Set the given projection mode.

  Set the given projection mode: Orthographic or Perspective.
*/
void SVTK_ViewWindow::onProjectionMode( QAction* theAction )
{
  int aMode = Parallel;
  if (theAction == toolMgr()->action( ProjectionModeId ))
    aMode = Projection;
  SVTK_Viewer* aViewer = dynamic_cast<SVTK_Viewer*>(myModel);
  aViewer->setProjectionMode(aMode);
  bool anIsParallelMode = (aMode == Parallel);
  vtkCamera* aCamera = getRenderer()->GetActiveCamera();
  aCamera->SetParallelProjection(anIsParallelMode);
  GetInteractor()->GetDevice()->CreateTimer(VTKI_TIMER_FIRST);
  getRenderWindow()->SetStereoRender(0);
  Repaint();
}

/*!
  \brief Sets Stereo projection mode.

  Sets Stereo projection mode.
*/
void SVTK_ViewWindow::onStereoMode( bool activate )
{
  if (activate) {
    toolMgr()->action( ProjectionModeId )->setChecked(true);
    vtkCamera* aCamera = getRenderer()->GetActiveCamera();
    aCamera->SetParallelProjection(false);
    toolMgr()->action( ProjectionModeId )->actionGroup()->setEnabled(false);
    SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
    SetStereoType( aResMgr->integerValue( "VTKViewer", "stereo_type", 0 ) );
    getRenderWindow()->SetStereoRender(1);
    Repaint();
  }
  else {
    toolMgr()->action( ProjectionModeId )->actionGroup()->setEnabled(true);
    SVTK_Viewer* aViewer = dynamic_cast<SVTK_Viewer*>(myModel);
    if (aViewer->projectionMode() == Parallel) {
      toolMgr()->action( ParallelModeId )->setChecked(true);
      onProjectionMode(toolMgr()->action( ParallelModeId ));
    }
    else if (aViewer->projectionMode() == Projection) {
      toolMgr()->action( ProjectionModeId )->setChecked(true);
      onProjectionMode(toolMgr()->action( ProjectionModeId ));
    }
  }
  if ( getRenderWindow()->GetStereoCapableWindow() == 1 && !isOpenGlStereoSupport() &&
       strcmp( "CrystalEyes", getRenderWindow()->GetStereoTypeAsString() ) == 0 &&
       toolMgr()->action( StereoModeId )->isChecked() )
    SUIT_MessageBox::warning( 0, tr( "WRN_WARNING" ),  tr( "WRN_SUPPORT_QUAD_BUFFER" ) );
}

/*!
  Set the view projection mode: orthogonal or perspective
*/
void SVTK_ViewWindow::onProjectionMode()
{
  if (toolMgr()->action( ParallelModeId )->isChecked())
    SetProjectionMode( Parallel);
  if (toolMgr()->action( ProjectionModeId )->isChecked())
    SetProjectionMode( Projection);
  if (toolMgr()->action( StereoModeId )->isChecked())
    SetProjectionMode( Stereo);
  emit transformed( this );
}

void SVTK_ViewWindow::SetEventDispatcher(vtkObject* theDispatcher)
{
  myEventDispatcher = theDispatcher;
}

/*!
  Creates all actions of svtk main window
*/
void SVTK_ViewWindow::createActions(SUIT_ResourceMgr* theResourceMgr)
{
  QtxAction* anAction;
  QtxActionToolMgr* mgr = toolMgr();

  // Dump view
  anAction = new QtxAction(tr("MNU_DUMP_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_DUMP" ) ),
                           tr( "MNU_DUMP_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_DUMP_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onDumpView()));
  mgr->registerAction( anAction, DumpId );

  // FitAll
  anAction = new QtxAction(tr("MNU_FITALL"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITALL" ) ),
                           tr( "MNU_FITALL" ), 0, this);
  anAction->setStatusTip(tr("DSC_FITALL"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onFitAll()));
  mgr->registerAction( anAction, FitAllId );

  // FitRect
  anAction = new QtxAction(tr("MNU_FITRECT"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITAREA" ) ),
                           tr( "MNU_FITRECT" ), 0, this);
  anAction->setStatusTip(tr("DSC_FITRECT"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(activateWindowFit()));
  mgr->registerAction( anAction, FitRectId );

  // FitSelection
  anAction = new QtxAction(tr("MNU_FITSELECTION"),
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FITSELECTION" ) ),
                           tr( "MNU_FITSELECTION" ), 0, this);
  anAction->setStatusTip(tr("DSC_FITSELECTION"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onFitSelection()));
  mgr->registerAction( anAction, FitSelectionId );

  // Zoom
  anAction = new QtxAction(tr("MNU_ZOOM_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(activateZoom()));
  mgr->registerAction( anAction, ZoomId );

  // Panning
  anAction = new QtxAction(tr("MNU_PAN_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(activatePanning()));
  mgr->registerAction( anAction, PanId );

  // Global Panning
  anAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(activateGlobalPanning()));
  mgr->registerAction( anAction, GlobalPanId );

  // Change rotation point
  anAction = new QtxAction(tr("MNU_CHANGINGROTATIONPOINT_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_ROTATION_POINT" ) ),
                           tr( "MNU_CHANGINGROTATIONPOINT_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_CHANGINGROTATIONPOINT_VIEW"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onChangeRotationPoint(bool)));
  mgr->registerAction( anAction, ChangeRotationPointId );

  // Rotation
  anAction = new QtxAction(tr("MNU_ROTATE_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ROTATE" ) ),
                           tr( "MNU_ROTATE_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_ROTATE_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(activateRotation()));
  mgr->registerAction( anAction, RotationId );

  // Projections
  anAction = new QtxAction(tr("MNU_FRONT_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_FRONT" ) ),
                           tr( "MNU_FRONT_VIEW" ), 0, this, false, "Viewers:Front view");
  anAction->setStatusTip(tr("DSC_FRONT_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onFrontView()));
  this->addAction(anAction);
  mgr->registerAction( anAction, FrontId );

  anAction = new QtxAction(tr("MNU_BACK_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BACK" ) ),
                           tr( "MNU_BACK_VIEW" ), 0, this, false, "Viewers:Back view");
  anAction->setStatusTip(tr("DSC_BACK_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onBackView()));
  this->addAction(anAction);
  mgr->registerAction( anAction, BackId );

  anAction = new QtxAction(tr("MNU_TOP_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TOP" ) ),
                           tr( "MNU_TOP_VIEW" ), 0, this, false, "Viewers:Top view");
  anAction->setStatusTip(tr("DSC_TOP_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onTopView()));
  this->addAction(anAction);
  mgr->registerAction( anAction, TopId );

  anAction = new QtxAction(tr("MNU_BOTTOM_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_BOTTOM" ) ),
                           tr( "MNU_BOTTOM_VIEW" ), 0, this, false, "Viewers:Bottom view");
  anAction->setStatusTip(tr("DSC_BOTTOM_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onBottomView()));
  this->addAction(anAction);
  mgr->registerAction( anAction, BottomId );

  anAction = new QtxAction(tr("MNU_LEFT_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_LEFT" ) ),
                           tr( "MNU_LEFT_VIEW" ), 0, this, false, "Viewers:Left view");
  anAction->setStatusTip(tr("DSC_LEFT_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onLeftView()));
  this->addAction(anAction);
  mgr->registerAction( anAction, LeftId );

  anAction = new QtxAction(tr("MNU_RIGHT_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RIGHT" ) ),
                           tr( "MNU_RIGHT_VIEW" ), 0, this, false, "Viewers:Right view");
  anAction->setStatusTip(tr("DSC_RIGHT_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onRightView()));
  this->addAction(anAction);
  mgr->registerAction( anAction, RightId );

  // rotate anticlockwise
  anAction = new QtxAction(tr("MNU_ANTICLOCKWISE_VIEW"),
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_ANTICLOCKWISE" ) ),
                           tr( "MNU_ANTICLOCKWISE_VIEW" ), 0, this, false, "Viewers:Rotate anticlockwise");
  anAction->setStatusTip(tr("DSC_ANTICLOCKWISE_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onAntiClockWiseView()));
  this->addAction(anAction);
  mgr->registerAction( anAction, AntiClockWiseId );

  // rotate clockwise
  anAction = new QtxAction(tr("MNU_CLOCKWISE_VIEW"),
			   theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_CLOCKWISE" ) ),
                           tr( "MNU_CLOCKWISE_VIEW" ), 0, this, false, "Viewers:Rotate clockwise");
  anAction->setStatusTip(tr("DSC_CLOCKWISE_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onClockWiseView()));
  this->addAction(anAction);
  mgr->registerAction( anAction, ClockWiseId );

  // Reset
  anAction = new QtxAction(tr("MNU_RESET_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_RESET" ) ),
                           tr( "MNU_RESET_VIEW" ), 0, this, false, "Viewers:Reset view");
  anAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(anAction, SIGNAL(triggered()), this, SLOT(onResetView()));
  this->addAction(anAction);
  mgr->registerAction( anAction, ResetId );

  // onViewTrihedron: Shows - Hides Trihedron
  anAction = new QtxAction(tr("MNU_SHOW_TRIHEDRON"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_VTKVIEWER_VIEW_TRIHEDRON" ) ),
                           tr( "MNU_SHOW_TRIHEDRON" ), 0, this);
  anAction->setCheckable( true );
  anAction->setChecked( true );
  
  anAction->setStatusTip(tr("DSC_SHOW_TRIHEDRON"));
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onViewTrihedron(bool)));
  mgr->registerAction( anAction, ViewTrihedronId );

  // onNonIsometric: Manage non-isometric params
  anAction = new QtxAction(tr("MNU_SVTK_SCALING"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_SCALING" ) ),
                           tr( "MNU_SVTK_SCALING" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_SCALING"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onNonIsometric(bool)));
  mgr->registerAction( anAction, NonIsometric );

  // onGraduatedAxes: Manage graduated axes params
  anAction = new QtxAction(tr("MNU_SVTK_GRADUATED_AXES"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_GRADUATED_AXES" ) ),
                           tr( "MNU_SVTK_GRADUATED_AXES" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_GRADUATED_AXES"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onGraduatedAxes(bool)));
  mgr->registerAction( anAction, GraduatedAxes );

  // onGraduatedAxes: Manage graduated axes params
  anAction = new QtxAction(tr("MNU_SVTK_UPDATE_RATE"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_UPDATE_RATE" ) ),
                           tr( "MNU_SVTK_UPDATE_RATE" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_UPDATE_RATE"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onUpdateRate(bool)));
  mgr->registerAction( anAction, UpdateRate );

  // Set perspective mode group
  anAction = new QtxAction(tr("MNU_SVTK_PARALLEL_MODE"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_VIEW_PARALLEL" ) ),
                           tr( "MNU_SVTK_PARALLEL_MODE" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_PARALLEL_MODE"));
  anAction->setCheckable(true);
  mgr->registerAction( anAction, ParallelModeId );

  anAction = new QtxAction(tr("MNU_SVTK_PERSPECTIVE_MODE"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_VIEW_PERSPECTIVE" ) ),
                           tr( "MNU_SVTK_PERSPECTIVE_MODE" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_PERSPECTIVE_MODE"));
  anAction->setCheckable(true);
  mgr->registerAction( anAction, ProjectionModeId );

  anAction = new QtxAction(tr("MNU_SVTK_STEREO_MODE"),
                             theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_VIEW_STEREO" ) ),
                             tr( "MNU_SVTK_STEREO_MODE" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_STEREO_MODE"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(triggered(bool)), this, SLOT(onStereoMode(bool)));
  mgr->registerAction( anAction, StereoModeId );

  QActionGroup* aPerspectiveGroup = new QActionGroup( this );
  aPerspectiveGroup->addAction( mgr->action( ParallelModeId ) );
  aPerspectiveGroup->addAction( mgr->action( ProjectionModeId ) );
  connect(aPerspectiveGroup, SIGNAL(triggered(QAction*)), this, SLOT(onProjectionMode(QAction*)));

  // View Parameters
  anAction = new QtxAction(tr("MNU_VIEWPARAMETERS_VIEW"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_VIEW_PARAMETERS" ) ),
                           tr( "MNU_VIEWPARAMETERS_VIEW" ), 0, this);
  anAction->setStatusTip(tr("DSC_VIEWPARAMETERS_VIEW"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onViewParameters(bool)));
  mgr->registerAction( anAction, ViewParametersId );

  // Synchronize View 
  mgr->registerAction( synchronizeAction(), SynchronizeId );

  // Switch between interaction styles
  anAction = new QtxAction(tr("MNU_SVTK_STYLE_SWITCH"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_STYLE_SWITCH" ) ),
                           tr( "MNU_SVTK_STYLE_SWITCH" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_STYLE_SWITCH"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onSwitchInteractionStyle(bool)));
  mgr->registerAction( anAction, SwitchInteractionStyleId );

  // Switch between zooming styles
  anAction = new QtxAction(tr("MNU_SVTK_ZOOMING_STYLE_SWITCH"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_ZOOMING_STYLE_SWITCH" ) ),
                           tr( "MNU_SVTK_ZOOMING_STYLE_SWITCH" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_ZOOMING_STYLE_SWITCH"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onSwitchZoomingStyle(bool)));
  mgr->registerAction( anAction, SwitchZoomingStyleId );

  // Pre-selection
  QSignalMapper* aSignalMapper = new QSignalMapper( this );
  connect(aSignalMapper, SIGNAL(mapped(int)), this, SLOT(onSwitchPreSelectionMode(int)));

  anAction = new QtxAction(tr("MNU_SVTK_PRESELECTION_STANDARD"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_PRESELECTION_STANDARD" ) ),
                           tr( "MNU_SVTK_PRESELECTION_STANDARD" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_PRESELECTION_STANDARD"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(triggered()), aSignalMapper, SLOT(map()));
  aSignalMapper->setMapping( anAction, Standard_Preselection );
  mgr->registerAction( anAction, StandardPreselectionId );
  
  anAction = new QtxAction(tr("MNU_SVTK_PRESELECTION_DYNAMIC"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_PRESELECTION_DYNAMIC" ) ),
                           tr( "MNU_SVTK_PRESELECTION_DYNAMIC" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_PRESELECTION_DYNAMIC"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(triggered()), aSignalMapper, SLOT(map()));
  aSignalMapper->setMapping( anAction, Dynamic_Preselection );
  mgr->registerAction( anAction, DynamicPreselectionId );

  anAction = new QtxAction(tr("MNU_SVTK_PRESELECTION_DISABLED"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_PRESELECTION_DISABLED" ) ),
                           tr( "MNU_SVTK_PRESELECTION_DISABLED" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_PRESELECTION_DISABLED"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(triggered()), aSignalMapper, SLOT(map()));
  aSignalMapper->setMapping( anAction, Preselection_Disabled );
  mgr->registerAction( anAction, DisablePreselectionId );

  QtxActionGroup* aPreselectionAction = new QtxActionGroup( this, true );
  aPreselectionAction->add( getAction( StandardPreselectionId ) );
  aPreselectionAction->add( getAction( DynamicPreselectionId ) );
  aPreselectionAction->add( getAction( DisablePreselectionId ) );
  mgr->registerAction( aPreselectionAction, PreselectionId );

  // Selection
  anAction = new QtxAction(tr("MNU_SVTK_ENABLE_SELECTION"), 
                           theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_SELECTION" ) ),
                           tr( "MNU_SVTK_ENABLE_SELECTION" ), 0, this);
  anAction->setStatusTip(tr("DSC_SVTK_ENABLE_SELECTION"));
  anAction->setCheckable(true);
  connect(anAction, SIGNAL(toggled(bool)), this, SLOT(onEnableSelection(bool)));
  mgr->registerAction( anAction, EnableSelectionId );

  // Start recording
  myStartAction = new QtxAction(tr("MNU_SVTK_RECORDING_START"), 
                                theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_RECORDING_START" ) ),
                                tr( "MNU_SVTK_RECORDING_START" ), 0, this);
  myStartAction->setStatusTip(tr("DSC_SVTK_RECORDING_START"));
  connect( myStartAction, SIGNAL( triggered ( bool ) ), this, SLOT( onStartRecording() ) );
  mgr->registerAction( myStartAction, StartRecordingId );

  // Play recording
  myPlayAction = new QtxAction(tr("MNU_SVTK_RECORDING_PLAY"), 
                               theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_RECORDING_PLAY" ) ),
                               tr( "MNU_SVTK_RECORDING_PLAY" ), 0, this);
  myPlayAction->setStatusTip(tr("DSC_SVTK_RECORDING_PLAY"));
  myPlayAction->setEnabled( false );
  connect( myPlayAction, SIGNAL( triggered ( bool ) ), this, SLOT( onPlayRecording() ) );
  mgr->registerAction( myPlayAction, PlayRecordingId );

  // Pause recording
  myPauseAction = new QtxAction(tr("MNU_SVTK_RECORDING_PAUSE"), 
                                theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_RECORDING_PAUSE" ) ),
                                tr( "MNU_SVTK_RECORDING_PAUSE" ), 0, this);
  myPauseAction->setStatusTip(tr("DSC_SVTK_RECORDING_PAUSE"));
  myPauseAction->setEnabled( false );
  connect( myPauseAction, SIGNAL( triggered ( bool ) ), this, SLOT( onPauseRecording() ) );
  mgr->registerAction( myPauseAction, PauseRecordingId );

  // Stop recording
  myStopAction = new QtxAction(tr("MNU_SVTK_RECORDING_STOP"), 
                               theResourceMgr->loadPixmap( "VTKViewer", tr( "ICON_SVTK_RECORDING_STOP" ) ),
                               tr( "MNU_SVTK_RECORDING_STOP" ), 0, this);
  myStopAction->setStatusTip(tr("DSC_SVTK_RECORDING_STOP"));
  myStopAction->setEnabled( false );
  connect( myStopAction, SIGNAL( triggered ( bool ) ), this, SLOT( onStopRecording() ) );
  mgr->registerAction( myStopAction, StopRecordingId );
}

/*!
  Creates toolbar of svtk main window
*/
void SVTK_ViewWindow::createToolBar()
{
  QtxActionToolMgr* mgr = toolMgr();
  
  mgr->append( DumpId, myToolBar );
  mgr->append( SwitchInteractionStyleId, myToolBar );
  mgr->append( SwitchZoomingStyleId, myToolBar );

  mgr->append( mgr->separator(), myToolBar );
 
  mgr->append( PreselectionId, myToolBar );
  mgr->append( EnableSelectionId, myToolBar );

  mgr->append( mgr->separator(), myToolBar );

  mgr->append( ViewTrihedronId, myToolBar );

  QtxMultiAction* aScaleAction = new QtxMultiAction( this );
  aScaleAction->insertAction( getAction( FitAllId ) );
  aScaleAction->insertAction( getAction( FitRectId ) );
  aScaleAction->insertAction( getAction( FitSelectionId ) );
  aScaleAction->insertAction( getAction( ZoomId ) );
  mgr->append( aScaleAction, myToolBar );

  QtxMultiAction* aPanningAction = new QtxMultiAction( this );
  aPanningAction->insertAction( getAction( PanId ) );
  aPanningAction->insertAction( getAction( GlobalPanId ) );
  mgr->append( aPanningAction, myToolBar );

  mgr->append( ChangeRotationPointId, myToolBar );

  mgr->append( RotationId, myToolBar );

  QtxMultiAction* aViewsAction = new QtxMultiAction( this );
  aViewsAction->insertAction( getAction( FrontId ) );
  aViewsAction->insertAction( getAction( BackId ) );
  aViewsAction->insertAction( getAction( TopId ) );
  aViewsAction->insertAction( getAction( BottomId ) );
  aViewsAction->insertAction( getAction( LeftId ) );
  aViewsAction->insertAction( getAction( RightId ) );
  mgr->append( aViewsAction, myToolBar );

  mgr->append( AntiClockWiseId, myToolBar );
  mgr->append( ClockWiseId, myToolBar );

  mgr->append( ResetId, myToolBar );

  mgr->append( UpdateRate, myToolBar );
  mgr->append( NonIsometric, myToolBar );
  mgr->append( GraduatedAxes, myToolBar );

  mgr->append( ViewParametersId, myToolBar );
  mgr->append( SynchronizeId, myToolBar );

  mgr->append( toolMgr()->separator(), myToolBar );

  mgr->append( ParallelModeId, myToolBar );
  mgr->append( ProjectionModeId, myToolBar );
  mgr->append( StereoModeId, myToolBar );

  mgr->append( StartRecordingId, myRecordingToolBar );
  mgr->append( PlayRecordingId, myRecordingToolBar );
  mgr->append( PauseRecordingId, myRecordingToolBar );
  mgr->append( StopRecordingId, myRecordingToolBar );
}

void SVTK_ViewWindow::onUpdateRate(bool theIsActivate)
{
  if(theIsActivate){
    myUpdateRateDlg->Update();
    myUpdateRateDlg->show();
  }else
    myUpdateRateDlg->hide();
}

void SVTK_ViewWindow::onNonIsometric(bool theIsActivate)
{
  if(theIsActivate){
    myNonIsometricDlg->Update();
    myNonIsometricDlg->show();
  }else
    myNonIsometricDlg->hide();
}

void SVTK_ViewWindow::onGraduatedAxes(bool theIsActivate)
{
  if(theIsActivate){
    myCubeAxesDlg->Update();
    myCubeAxesDlg->show();
  }else
    myCubeAxesDlg->hide();
}

/*!
  Starts rotation transformation
*/
void SVTK_ViewWindow::activateRotation()
{
  myEventDispatcher->InvokeEvent(SVTK::StartRotate,0);
}


/*!
  Starts panning transformation
*/
void SVTK_ViewWindow::activatePanning()
{
  myEventDispatcher->InvokeEvent(SVTK::StartPan,0);
}

/*!
  Starts zoom transformation
*/
void SVTK_ViewWindow::activateZoom()
{
  myEventDispatcher->InvokeEvent(SVTK::StartZoom,0);
}

/*!
  Starts window fit transformation
*/
void SVTK_ViewWindow::activateWindowFit()
{
  myEventDispatcher->InvokeEvent(SVTK::StartFitArea,0);
}

/*!
  Starts global panning transformation
*/
void SVTK_ViewWindow::activateGlobalPanning()
{
  myEventDispatcher->InvokeEvent(SVTK::StartGlobalPan,0);
}

void SVTK_ViewWindow::onStartRecording()
{
  myRecorder->CheckExistAVIMaker();
  if (myRecorder->ErrorStatus()) {
    SUIT_MessageBox::warning(this, tr("ERROR"), tr("MSG_NO_AVI_MAKER") );
  }
  else {
    SVTK_RecorderDlg* aRecorderDlg = new SVTK_RecorderDlg( this, myRecorder );

    if( !aRecorderDlg->exec() )
      return;

    myStartAction->setEnabled( false );
    myPlayAction->setEnabled( false );
    myPauseAction->setEnabled( true );
    myStopAction->setEnabled( true );

    // to prevent resizing the window while recording
    myPreRecordingMinSize = minimumSize();
    myPreRecordingMaxSize = maximumSize();
    setFixedSize( size() );

    myRecorder->Record();
  }
}

void SVTK_ViewWindow::onPlayRecording()
{
  myStartAction->setEnabled( false );
  myPlayAction->setEnabled( false );
  myPauseAction->setEnabled( true );
  myStopAction->setEnabled( true );

  myRecorder->Pause();
}

void SVTK_ViewWindow::onPauseRecording()
{
  myStartAction->setEnabled( false );
  myPlayAction->setEnabled( true );
  myPauseAction->setEnabled( false );
  myStopAction->setEnabled( true );

  myRecorder->Pause();
}

void SVTK_ViewWindow::onStopRecording()
{
  myStartAction->setEnabled( true );
  myPlayAction->setEnabled( false );
  myPauseAction->setEnabled( false );
  myStopAction->setEnabled( false );

  myRecorder->Stop();

  setMinimumSize( myPreRecordingMinSize );
  setMaximumSize( myPreRecordingMaxSize );
}

/*!
  To invoke a VTK event on SVTK_RenderWindowInteractor instance
*/
void SVTK_ViewWindow::InvokeEvent(unsigned long theEvent, void* theCallData)
{
  GetInteractor()->InvokeEvent(theEvent,theCallData);
}

/*!
  Modify view parameters
*/
void SVTK_ViewWindow::onViewParameters(bool theIsActivate)
{
  if(theIsActivate){
    myViewParameterDlg->addObserver();
    myViewParameterDlg->show();
  }else
    myViewParameterDlg->hide();
}

/*!
  Custom show event handler
*/
void SVTK_ViewWindow::showEvent( QShowEvent * theEvent ) 
{
  emit Show( theEvent );
}

/*!
  Custom hide event handler
*/
void SVTK_ViewWindow::hideEvent( QHideEvent * theEvent ) 
{
  emit Hide( theEvent );
}

/*!
  Emit transformed signal.
*/
void SVTK_ViewWindow::emitTransformed() {
  transformed(this);
}

/*!
  Processes events
*/
void SVTK_ViewWindow::ProcessEvents(vtkObject* vtkNotUsed(theObject),
				    unsigned long theEvent,
				    void* theClientData,
				    void* theCallData)
{
  SVTK_ViewWindow* self = reinterpret_cast<SVTK_ViewWindow*>(theClientData);
  if(self)
    self->emitTransformed();
}

/*!
  Get camera properties for the SVTK view window.
  \return shared pointer on camera properties.
*/
SUIT_CameraProperties SVTK_ViewWindow::cameraProperties()
{
  SUIT_CameraProperties aProps;

  // get vtk camera
  vtkCamera* aCamera = getRenderer()->GetActiveCamera();
  if ( !aCamera )
    return aProps;
  
  aProps.setDimension( SUIT_CameraProperties::Dim3D );
  if ( toolMgr()->action( ParallelModeId ) ) {
    if ( toolMgr()->action( ParallelModeId )->isChecked() )
      aProps.setProjection( SUIT_CameraProperties::PrjOrthogonal );
    else
      aProps.setProjection( SUIT_CameraProperties::PrjPerspective );
  }

  double aFocalPoint[3];
  double aPosition[3];
  double aViewUp[3];
  double anAxialScale[3];

  aCamera->OrthogonalizeViewUp();
  aCamera->GetFocalPoint( aFocalPoint );
  aCamera->GetPosition( aPosition );
  aCamera->GetViewUp( aViewUp );
  
  aProps.setFocalPoint( aFocalPoint[0], aFocalPoint[1], aFocalPoint[2] );
  aProps.setPosition( aPosition[0], aPosition[1], aPosition[2] );
  aProps.setViewUp( aViewUp[0], aViewUp[1], aViewUp[2] );
  aProps.setMappingScale( aCamera->GetParallelScale() * 2.0 );

  if ( aProps.getProjection() == SUIT_CameraProperties::PrjPerspective )
  {
    aProps.setViewAngle( aCamera->GetViewAngle() );
  }

  GetRenderer()->GetScale( anAxialScale );
  aProps.setAxialScale( anAxialScale[0], anAxialScale[1], anAxialScale[2] );
  
  return aProps;
}

/*!
  Synchronize views.
  This implementation synchronizes camera propreties.
*/
void SVTK_ViewWindow::synchronize( SUIT_ViewWindow* theView )
{
  bool blocked = blockSignals( true );

  SUIT_CameraProperties aProps = theView->cameraProperties();
  if ( !cameraProperties().isCompatible( aProps ) ) {
    // other view, this one is being currently synchronized to, seems has become incompatible
    // we have to break synchronization
    updateSyncViews();
    return;
  }

  // get camera
  vtkCamera* aCamera = getRenderer()->GetActiveCamera();
  
  double aFocalPoint[3];
  double aPosition[3];
  double aViewUp[3];
  double anAxialScale[3];

  // get common properties
  aProps.getViewUp( aViewUp[0], aViewUp[1], aViewUp[2] );
  aProps.getPosition( aPosition[0], aPosition[1], aPosition[2] );
  aProps.getFocalPoint( aFocalPoint[0], aFocalPoint[1], aFocalPoint[2] );
  aProps.getAxialScale( anAxialScale[0], anAxialScale[1], anAxialScale[2] );
  
  // restore properties to the camera
  aCamera->SetViewUp( aViewUp );
  aCamera->SetPosition( aPosition );
  aCamera->SetFocalPoint( aFocalPoint );
  aCamera->SetParallelScale( aProps.getMappingScale() / 2.0 );

  if ( aProps.getProjection() == SUIT_CameraProperties::PrjPerspective )
  {
    aCamera->SetViewAngle( aProps.getViewAngle() );
  }

  GetRenderer()->SetScale( anAxialScale );

  getRenderer()->ResetCameraClippingRange();
  Repaint( false );

  blockSignals( blocked );
}
