// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

//  SALOME QxGraph : build Supervisor viewer into desktop
//
#include "QxGraph_ViewWindow.h"
#include "QxGraph_Def.h"
#include "QxGraph_Canvas.h"
#include "QxGraph_CanvasView.h"

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ToolButton.h>

//QT Include
#include <qlayout.h>
#include <qcolor.h>

/*!
    Constructor
*/
QxGraph_ViewWindow::QxGraph_ViewWindow( SUIT_Desktop* theDesktop, QxGraph_Viewer* theModel)
  : SUIT_ViewWindow( theDesktop )
{
  printf("Construct QxGraph_ViewWindow\n");
  myViewModel = theModel;
}

/*!
  Initialization
*/
void QxGraph_ViewWindow::initLayout()
{
  initCanvas();
  initCanvasViews();

  myToolBar = new QToolBar(this);
  myToolBar->setCloseMode(QDockWindow::Undocked);
  myToolBar->setLabel(tr("LBL_TOOLBAR_LABEL"));
  createActions();
  createToolBar();
}

/*!
  Canvas initialization
*/
void QxGraph_ViewWindow::initCanvas()
{
  if ( myViewModel )
    myViewModel->setCanvas( new QxGraph_Canvas(resMgr()) );
    //myViewModel->addCanvas( new QxGraph_Canvas(resMgr()) );
}

/*!
  Canvas views initialization
*/
void QxGraph_ViewWindow::initCanvasViews()
{
  if ( myViewModel )
  {
    for (int i = 0; i < 2; i++)
    {
      //QxGraph_Canvas* aCanvas = new QxGraph_Canvas(resMgr());
      //myViewModel->addCanvas( aCanvas );

      QxGraph_CanvasView* aCanvasView = new QxGraph_CanvasView( myViewModel->getCanvas(), this );
      //QxGraph_CanvasView* aCanvasView = new QxGraph_CanvasView( aCanvas, this );
      myViewModel->addView(aCanvasView);

      aCanvasView->hide();
    }
      
    // the first view is shown and is the current
    myViewModel->getCanvasViews().first()->show();
    myViewModel->setCurrentView( myViewModel->getCanvasViews().first() );
    
    /* test for 1 view -->
    QxGraph_CanvasView* aCanvasView = new QxGraph_CanvasView( myViewModel->getCanvas(), this );
    myViewModel->addView(aCanvasView);
    myViewModel->setCurrentView(aCanvasView);
    test for 1 view <-- */

    setCentralWidget( myViewModel->getCurrentView() );
    
    /*if ( inherits( "QMainWindow" ) ) {
      printf("== > 1\n");
      ( ( QMainWindow* )this )->setCentralWidget( myViewModel->getCurrentView() );
      }
      else {
      printf("== > 2\n");
      QBoxLayout* layout = new QVBoxLayout( this );
      layout->addWidget( myViewModel->getCurrentView() );
      }*/
  }
}

/*!
  Creates actions of QxGraph view window
*/
void QxGraph_ViewWindow::createActions()
{
  if (!myActionsMap.isEmpty()) return;
  SUIT_ResourceMgr* aResMgr = resMgr();
  QtxAction* aAction;

  // FitAll
  aAction = new QtxAction(tr("MNU_FITALL"), aResMgr->loadPixmap( "QxGraph", tr( "ICON_QXGRAPH_FITALL" ) ),
                          tr( "MNU_FITALL" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITALL"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewFitAll()));
  myActionsMap[ FitAllId ] = aAction;

  // FitRect
  aAction = new QtxAction(tr("MNU_FITRECT"), aResMgr->loadPixmap( "QxGraph", tr( "ICON_QXGRAPH_FITAREA" ) ),
                          tr( "MNU_FITRECT" ), 0, this);
  aAction->setStatusTip(tr("DSC_FITRECT"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewFitArea()));
  myActionsMap[ FitRectId ] = aAction;

  // Zoom
  aAction = new QtxAction(tr("MNU_ZOOM_VIEW"), aResMgr->loadPixmap( "QxGraph", tr( "ICON_QXGRAPH_ZOOM" ) ),
                          tr( "MNU_ZOOM_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_ZOOM_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewZoom()));
  myActionsMap[ ZoomId ] = aAction;

  // Panning
  aAction = new QtxAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "QxGraph", tr( "ICON_QXGRAPH_PAN" ) ),
                          tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewPan()));
  myActionsMap[ PanId ] = aAction;

  // Global Panning
  aAction = new QtxAction(tr("MNU_GLOBALPAN_VIEW"), aResMgr->loadPixmap( "QxGraph", tr( "ICON_QXGRAPH_GLOBALPAN" ) ),
                          tr( "MNU_GLOBALPAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_GLOBALPAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewGlobalPan()));
  myActionsMap[ GlobalPanId ] = aAction;

  // Reset
  aAction = new QtxAction(tr("MNU_RESET_VIEW"), aResMgr->loadPixmap( "QxGraph", tr( "ICON_QXGRAPH_RESET" ) ),
                          tr( "MNU_RESET_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewReset()));
  myActionsMap[ ResetId ] = aAction;
}

/*!
  Creates toolbar of QxGraph view window
*/
void QxGraph_ViewWindow::createToolBar()
{
  SUIT_ToolButton* aScaleBtn = new SUIT_ToolButton(myToolBar, "scale");
  aScaleBtn->AddAction(myActionsMap[FitAllId]);
  aScaleBtn->AddAction(myActionsMap[FitRectId]);
  aScaleBtn->AddAction(myActionsMap[ZoomId]);

  SUIT_ToolButton* aPanningBtn = new SUIT_ToolButton(myToolBar, "pan");
  aPanningBtn->AddAction(myActionsMap[PanId]);
  aPanningBtn->AddAction(myActionsMap[GlobalPanId]);

  myActionsMap[ResetId]->addTo(myToolBar);
}

/*!
  Destructor
*/
QxGraph_ViewWindow::~QxGraph_ViewWindow() {}

/*!
  Reset the active view
*/
void QxGraph_ViewWindow::onViewReset()    
{
  printf( "QxGraph_ViewWindow::onViewReset\n" );
  if ( myViewModel && myViewModel->getCurrentView() )
    myViewModel->getCurrentView()->activateReset();
}

/*!
  Sets a new center of the active view
*/
void QxGraph_ViewWindow::onViewGlobalPan()
{
  printf( "QxGraph_ViewWindow::onViewGlobalPan\n" );
  if ( myViewModel && myViewModel->getCurrentView() )
    myViewModel->getCurrentView()->activateGlobalPanning();
}

/*!
  Zooms the active view
*/
void QxGraph_ViewWindow::onViewZoom()
{
  printf( "QxGraph_ViewWindow::onViewZoom\n" );
  if ( myViewModel && myViewModel->getCurrentView() )
    myViewModel->getCurrentView()->activateZoom();
}

/*!
  Moves the active view
*/
void QxGraph_ViewWindow::onViewPan()
{
  printf( "QxGraph_ViewWindow::onViewPan\n" );
  if ( myViewModel && myViewModel->getCurrentView() )
    myViewModel->getCurrentView()->activatePanning();
}

/*!
  Fits all obejcts within a rectangular area of the active view
*/
void QxGraph_ViewWindow::onViewFitArea()
{
  printf( "QxGraph_ViewWindow::onViewFitArea\n" );
  if ( myViewModel && myViewModel->getCurrentView() )
    myViewModel->getCurrentView()->activateFitRect();
}

/*!
  Fits all objects in the active view
*/
void QxGraph_ViewWindow::onViewFitAll()
{
  printf( "QxGraph_ViewWindow::onViewFitAll\n" );
  if ( myViewModel && myViewModel->getCurrentView() )
    myViewModel->getCurrentView()->activateFitAll();
}

/*!
    Set background of the viewport
*/
void QxGraph_ViewWindow::setBackgroundColor( const QColor& color )
{
  if ( myViewModel && myViewModel->getCurrentView()) {
    myViewModel->getCurrentView()->canvas()->setBackgroundColor(color);
    myViewModel->getCurrentView()->setPaletteBackgroundColor(color.light(120));
  }
}

/*!
    Returns background of the viewport
*/
QColor QxGraph_ViewWindow::backgroundColor() const
{
  if ( myViewModel && myViewModel->getCurrentView())
    return myViewModel->getCurrentView()->canvas()->backgroundColor();
  return QColor();
}

/*!
  Custom resize event handler
*/
void QxGraph_ViewWindow::resizeEvent( QResizeEvent* theEvent )
{
  //  QMainWindow::resizeEvent( theEvent );
  //if ( myView ) myView->resizeView( theEvent );
}

/*!
  Get resource manager
*/
SUIT_ResourceMgr* QxGraph_ViewWindow::resMgr() const
{ 
  return SUIT_Session::session()->resourceMgr(); 
}

