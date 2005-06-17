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

//****************************************************************
VTKViewer_Viewer::VTKViewer_Viewer()
: SUIT_ViewModel(),
myBgColor( Qt::black )
{
}

//****************************************************************
VTKViewer_Viewer::~VTKViewer_Viewer() 
{
}

QColor VTKViewer_Viewer::backgroundColor() const
{
  return myBgColor;
}

void VTKViewer_Viewer::setBackgroundColor( const QColor& c )
{
  if ( c.isValid() )
    myBgColor = c;
}

//****************************************************************
SUIT_ViewWindow* VTKViewer_Viewer::createView( SUIT_Desktop* theDesktop )
{
  VTKViewer_ViewWindow* vw = new VTKViewer_ViewWindow( theDesktop, this );
  vw->setBackgroundColor( myBgColor );
  return vw;
}

//*********************************************************************
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

//*********************************************************************
void VTKViewer_Viewer::contextMenuPopup(QPopupMenu* thePopup)
{
  thePopup->insertItem( tr( "MEN_DUMP_VIEW" ), this, SLOT( onDumpView() ) );
  thePopup->insertItem( tr( "MEN_CHANGE_BACKGROUD" ), this, SLOT( onChangeBgColor() ) );

  thePopup->insertSeparator();

  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView && !aView->getToolBar()->isVisible() )
    thePopup->insertItem( tr( "MEN_SHOW_TOOLBAR" ), this, SLOT( onShowToolbar() ) );
}

//*********************************************************************
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

//*********************************************************************
void VTKViewer_Viewer::onMouseMove(SUIT_ViewWindow* vw, QMouseEvent* event)
{
  VTKViewer_RenderWindowInteractor* rwi = 0;
  if ( vw && vw->inherits( "VTKViewer_ViewWindow" ) )
    rwi = ((VTKViewer_ViewWindow*)vw)->getRWInteractor();
  if ( rwi )
    rwi->MouseMove( event );
}

//*********************************************************************
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

//*********************************************************************
void VTKViewer_Viewer::enableSelection(bool isEnabled)
{
  mySelectionEnabled = isEnabled;
  //!! To be done for view windows
}

//*********************************************************************
void VTKViewer_Viewer::enableMultiselection(bool isEnable)
{
  myMultiSelectionEnabled = isEnable;
  //!! To be done for view windows
}

void VTKViewer_Viewer::onDumpView()
{
  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->onDumpView();
}

//*********************************************************************
void VTKViewer_Viewer::onChangeBgColor()
{
  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( !aView )
    return;

  QColor aColor = QColorDialog::getColor( aView->backgroundColor(), aView);
  if ( aColor.isValid() )
    aView->setBackgroundColor(aColor);
}

//*********************************************************************
void VTKViewer_Viewer::onShowToolbar() {
  VTKViewer_ViewWindow* aView = (VTKViewer_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->getToolBar()->show();    
}
