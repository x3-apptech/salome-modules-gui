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

#include "VTKViewer_ViewModel.h"
#include "VTKViewer_ViewWindow.h"
#include "VTKViewer_ViewManager.h"
#include "VTKViewer_RenderWindowInteractor.h"

#include "QtxBackgroundTool.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_Desktop.h"
#include "SUIT_Session.h"

#include <QColorDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QToolBar>

// VSR: Uncomment below line to allow texture background support in VTK viewer
#define VTK_ENABLE_TEXTURED_BACKGROUND

bool _InitializeVtkWarningsCall()
{
  char* isOn = getenv( "VTK_WARNINGS_IS_ON" );  
  if ( !isOn || strcmp( isOn, "1" ) )
    vtkObject::GlobalWarningDisplayOff();

  delete isOn;
  return vtkObject::GetGlobalWarningDisplay();
}
static bool _InitializeVtkWarnings = _InitializeVtkWarningsCall();

/*!Constructor.Sets background color to black.*/
VTKViewer_Viewer::VTKViewer_Viewer()
: SUIT_ViewModel(),
  myDefaultBackground( Qtx::BackgroundData( Qt::black ) )
{
}

/*!Destructor.*/
VTKViewer_Viewer::~VTKViewer_Viewer() 
{
}

/*! Get data for supported background modes: gradient types, identifiers and supported image formats */
QString VTKViewer_Viewer::backgroundData( QStringList& gradList, QIntList& idList, QIntList& txtList )
{
  gradList << tr( "GT_HORIZONTALGRADIENT" )
           << tr( "GT_VERTICALGRADIENT" )
           << tr( "GT_FIRSTDIAGONALGRADIENT" )
           << tr( "GT_SECONDDIAGONALGRADIENT" )
           << tr( "GT_FIRSTCORNERGRADIENT" )
           << tr( "GT_SECONDCORNERGRADIENT" )
           << tr( "GT_THIRDCORNERGRADIENT" )
           << tr( "GT_FOURTHCORNERGRADIENT" );
  idList   << HorizontalGradient
           << VerticalGradient
           << FirstDiagonalGradient
           << SecondDiagonalGradient
           << FirstCornerGradient
           << SecondCornerGradient
           << ThirdCornerGradient
           << FourthCornerGradient;
#ifdef VTK_ENABLE_TEXTURED_BACKGROUND
  txtList  << Qtx::CenterTexture << Qtx::TileTexture << Qtx::StretchTexture;
#endif
  return tr("BG_IMAGE_FILES");
}

/*!Gets background color [obsolete]*/
QColor VTKViewer_Viewer::backgroundColor() const
{
  return background().color();
}

/*!Sets background color [obsolete]*/
void VTKViewer_Viewer::setBackgroundColor( const QColor& c )
{
  Qtx::BackgroundData bg = background();
  bg.setColor( c );
  setBackground( bg );
}

/*!Gets default background data.*/
Qtx::BackgroundData VTKViewer_Viewer::background() const
{
  return myDefaultBackground;
}

/*!Sets default background data.*/
void VTKViewer_Viewer::setBackground( const Qtx::BackgroundData& theBackground )
{
  myDefaultBackground = theBackground.isValid() ? theBackground : Qtx::BackgroundData( Qt::black );
}

/*!Create new instance of VTKViewer_ViewWindow, sets background color and return pointer to it.*/
SUIT_ViewWindow* VTKViewer_Viewer::createView( SUIT_Desktop* theDesktop )
{
  VTKViewer_ViewWindow* vw = new VTKViewer_ViewWindow( theDesktop, this );
  vw->setBackground( myDefaultBackground );
  return vw;
}

/*!Sets view manager and connect slots.*/
void VTKViewer_Viewer::setViewManager(SUIT_ViewManager* theViewManager)
{
  SUIT_ViewModel::setViewManager(theViewManager);
  connect(theViewManager, SIGNAL(mousePress(SUIT_ViewWindow*, QMouseEvent*)), 
          this, SLOT(onMousePress(SUIT_ViewWindow*, QMouseEvent*)));

  connect(theViewManager, SIGNAL(mouseMove(SUIT_ViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseMove(SUIT_ViewWindow*, QMouseEvent*)));

  connect(theViewManager, SIGNAL(mouseRelease(SUIT_ViewWindow*, QMouseEvent*)), 
          this, SLOT(onMouseRelease(SUIT_ViewWindow*, QMouseEvent*)));
}

/*!Insert context into popup menu.*/
void VTKViewer_Viewer::contextMenuPopup(QMenu* thePopup)
{
  thePopup->addAction( tr( "MEN_DUMP_VIEW" ), this, SLOT( onDumpView() ) );
  thePopup->addAction( tr( "MEN_CHANGE_BACKGROUND" ), this, SLOT( onChangeBackground() ) );

  thePopup->addSeparator();

  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView && !aView->getToolBar()->isVisible() )
    thePopup->addAction( tr( "MEN_SHOW_TOOLBAR" ), this, SLOT( onShowToolbar() ) );
}

/*!On mouse press event.*/
void VTKViewer_Viewer::onMousePress(SUIT_ViewWindow* vw, QMouseEvent* event)
{
  VTKViewer_RenderWindowInteractor* rwi = 0;
  if ( vw && vw->inherits( "VTKViewer_ViewWindow" ) )
    rwi = ((VTKViewer_ViewWindow*)vw)->getRWInteractor();
  if ( !rwi )
    return;

  switch(event->buttons()) {
  case Qt::LeftButton:
    rwi->LeftButtonPressed(event) ;
    break ;
  case Qt::MidButton:
    rwi->MiddleButtonPressed(event) ;
    break ;
  case Qt::RightButton:
    rwi->RightButtonPressed(event) ;
    break;
  default:
    break ;
  }
}

/*!On mouse move event.*/
void VTKViewer_Viewer::onMouseMove(SUIT_ViewWindow* vw, QMouseEvent* event)
{
  VTKViewer_RenderWindowInteractor* rwi = 0;
  if ( vw && vw->inherits( "VTKViewer_ViewWindow" ) )
    rwi = ((VTKViewer_ViewWindow*)vw)->getRWInteractor();
  if ( rwi )
    rwi->MouseMove( event );
}

/*!On mouse release event.*/
void VTKViewer_Viewer::onMouseRelease(SUIT_ViewWindow* vw, QMouseEvent* event)
{
  VTKViewer_RenderWindowInteractor* rwi = 0;
  if ( vw && vw->inherits( "VTKViewer_ViewWindow" ) )
    rwi = ((VTKViewer_ViewWindow*)vw)->getRWInteractor();
  if ( !rwi )
    return;

  switch(event->buttons()) {
  case Qt::LeftButton:
    rwi->LeftButtonReleased(event) ;
    break ;
  case Qt::MidButton:
    rwi->MiddleButtonReleased(event) ;
    break ;
  case Qt::RightButton:
    rwi->RightButtonReleased(event) ;
    break;
  default:
    break ;
  }
}

/*!Sets flag to enable selection \a isEnable.*/
void VTKViewer_Viewer::enableSelection(bool isEnabled)
{
  mySelectionEnabled = isEnabled;
  //!! To be done for view windows
}

/*!Sets flag to multi selection enable \a isEnable.*/
void VTKViewer_Viewer::enableMultiselection(bool isEnable)
{
  myMultiSelectionEnabled = isEnable;
  //!! To be done for view windows
}

/*!On dump view event.*/
void VTKViewer_Viewer::onDumpView()
{
  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->onDumpView();
}

/*!On change back ground color event.*/
void VTKViewer_Viewer::onChangeBackground()
{
  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( !aView )
    return;

  // get supported gradient types
  QStringList gradList;
  QIntList    idList, txtList;
  QString     formats = backgroundData( gradList, idList, txtList );

  // invoke dialog box
  Qtx::BackgroundData bgData = QtxBackgroundDialog::getBackground( aView->background(),  // initial background
								   aView,                // parent for dialog box
								   txtList,              // allowed texture modes
								   true,                 // enable solid color mode
								   true,                 // enable gradient mode
								   false,                // disable custom gradient mode
								   !txtList.isEmpty(),   // enable texture mode
								   gradList,             // gradient names
								   idList,               // gradient identifiers
								   formats );            // image formats

  // set chosen background data to the viewer
  if ( bgData.isValid() )
    aView->setBackground( bgData );
}

/*!On show tool bar event.*/
void VTKViewer_Viewer::onShowToolbar() {
  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->getToolBar()->show();    
}
