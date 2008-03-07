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
#include "VTKViewer_ViewModel.h"
#include "VTKViewer_ViewWindow.h"
#include "VTKViewer_ViewManager.h"
#include "VTKViewer_RenderWindowInteractor.h"

#include "SUIT_ViewWindow.h"
#include "SUIT_Desktop.h"
#include "SUIT_Session.h"

#include <qpainter.h>
#include <qapplication.h>
#include <qcolordialog.h>
#include <qpalette.h>
#include <qpopupmenu.h>

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
myBgColor( Qt::black )
{
}

/*!Destructor.*/
VTKViewer_Viewer::~VTKViewer_Viewer() 
{
}

/*!Gets background color.*/
QColor VTKViewer_Viewer::backgroundColor() const
{
  return myBgColor;
}

/*!Sets background color.*/
void VTKViewer_Viewer::setBackgroundColor( const QColor& c )
{
  if ( c.isValid() )
    myBgColor = c;
}

/*!Create new instance of VTKViewer_ViewWindow, sets background color and return pointer to it.*/
SUIT_ViewWindow* VTKViewer_Viewer::createView( SUIT_Desktop* theDesktop )
{
  VTKViewer_ViewWindow* vw = new VTKViewer_ViewWindow( theDesktop, this );
  vw->setBackgroundColor( myBgColor );
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
void VTKViewer_Viewer::contextMenuPopup(QPopupMenu* thePopup)
{
  thePopup->insertItem( tr( "MEN_DUMP_VIEW" ), this, SLOT( onDumpView() ) );
  thePopup->insertItem( tr( "MEN_CHANGE_BACKGROUD" ), this, SLOT( onChangeBgColor() ) );

  thePopup->insertSeparator();

  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView && !aView->getToolBar()->isVisible() )
    thePopup->insertItem( tr( "MEN_SHOW_TOOLBAR" ), this, SLOT( onShowToolbar() ) );
}

/*!On mouse press event.*/
void VTKViewer_Viewer::onMousePress(SUIT_ViewWindow* vw, QMouseEvent* event)
{
  VTKViewer_RenderWindowInteractor* rwi = 0;
  if ( vw && vw->inherits( "VTKViewer_ViewWindow" ) )
    rwi = ((VTKViewer_ViewWindow*)vw)->getRWInteractor();
  if ( !rwi )
    return;

  switch(event->button()) {
  case LeftButton:
    rwi->LeftButtonPressed(event) ;
    break ;
  case MidButton:
    rwi->MiddleButtonPressed(event) ;
    break ;
  case RightButton:
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

  switch(event->button()) {
  case LeftButton:
    rwi->LeftButtonReleased(event) ;
    break ;
  case MidButton:
    rwi->MiddleButtonReleased(event) ;
    break ;
  case RightButton:
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
void VTKViewer_Viewer::onChangeBgColor()
{
  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( !aView )
    return;

  QColor aColor = QColorDialog::getColor( aView->backgroundColor(), aView);
  if ( aColor.isValid() )
    aView->setBackgroundColor(aColor);
}

/*!On show tool bar event.*/
void VTKViewer_Viewer::onShowToolbar() {
  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->getToolBar()->show();    
}
