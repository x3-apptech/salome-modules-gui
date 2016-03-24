// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  SALOME QxScene : build Supervisor viewer into desktop
//
#include "QxScene_ViewWindow.h"
#include "QxScene_Def.h"

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>

#include <QtxAction.h>
#include <QtxMultiAction.h>
#include <QtxActionToolMgr.h>

//QT Include
#include <QToolBar>
#include <QGraphicsRectItem>

//#define _DEVDEBUG_
#include "DebTrace.hxx"

/*!
    Constructor
*/
QxScene_ViewWindow::QxScene_ViewWindow( SUIT_Desktop* theDesktop, QxScene_Viewer* theModel)
  : SUIT_ViewWindow( theDesktop )
{
  DEBTRACE("Construct QxScene_ViewWindow");
  myViewModel = theModel;
  _scene = 0;
  _sceneView = 0;
}

/*!
  Initialization
*/
void QxScene_ViewWindow::initLayout()
{
  DEBTRACE("QxScene_ViewWindow::initLayout");
  createActions();
  createToolBar();
  
  // --- QGraphics test

//   _scene = new QGraphicsScene();
//   _sceneView = new QGraphicsView(this);
//   setCentralWidget(_sceneView);
//   _sceneView->setScene(_scene);
//   QGraphicsRectItem *rect = _scene->addRect(QRectF(0, 0, 100, 100));
//   _sceneView->show();
}

/*!
  Creates actions of QxScene view window
*/
void QxScene_ViewWindow::createActions()
{
  DEBTRACE("QxScene_ViewWindow::createActions");
  QtxActionToolMgr* mgr = toolMgr();
  QtxAction* aAction;
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  // 2. Scaling operations

  // 2.1. Fit All
  aAction = new QtxAction( tr( "MNU_FITALL" ),
                           aResMgr->loadPixmap( "QxSceneViewer", tr( "ICON_QXSCENE_FITALL" ) ),
                           tr( "MNU_FITALL" ),
                           0, this);
  aAction->setStatusTip( tr( "DSC_FITALL" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewFitAll() ) );
  mgr->registerAction( aAction, FitAllId );

  // 2.2. Fit Rect
  aAction = new QtxAction( tr( "MNU_FITRECT" ),
                           aResMgr->loadPixmap( "QxSceneViewer", tr( "ICON_QXSCENE_FITAREA" ) ),
                           tr( "MNU_FITRECT" ),
                           0, this);
  aAction->setStatusTip( tr( "DSC_FITRECT" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewFitArea() ) );
  mgr->registerAction( aAction, FitRectId );

  // 2.3. Zoom
  aAction = new QtxAction( tr( "MNU_ZOOM_VIEW" ),
                           aResMgr->loadPixmap( "QxSceneViewer", tr( "ICON_QXSCENE_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ),
                           0, this);
  aAction->setStatusTip( tr( "DSC_ZOOM_VIEW" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewZoom() ) );
  mgr->registerAction( aAction, ZoomId );

  // 2.4. Create multi-action for scaling operations
  QtxMultiAction* aScaleAction = new QtxMultiAction( this );
  aScaleAction->insertAction( mgr->action( FitAllId  ) );
  aScaleAction->insertAction( mgr->action( FitRectId ) );
  aScaleAction->insertAction( mgr->action( ZoomId    ) );
  mgr->registerAction( aScaleAction, ScaleOpId );

  // 3. Moving operations

  // 3.1. Panning
  aAction = new QtxAction( tr( "MNU_PAN_VIEW" ),
                           aResMgr->loadPixmap( "QxSceneViewer", tr( "ICON_QXSCENE_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 
                           0, this);
  aAction->setStatusTip( tr( "DSC_PAN_VIEW" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewPan() ) );
  mgr->registerAction( aAction, PanId );

  // 3.2. Global Panning
  aAction = new QtxAction( tr( "MNU_GLOBALPAN_VIEW" ),
                           aResMgr->loadPixmap( "QxSceneViewer", tr( "ICON_QXSCENE_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ),
                           0, this);
  aAction->setStatusTip( tr( "DSC_GLOBALPAN_VIEW" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewGlobalPan() ) );
  mgr->registerAction( aAction, GlobalPanId );

  // 3.3. Create multi-action for moving operations
  QtxMultiAction* aPanAction = new QtxMultiAction( this );
  aPanAction->insertAction( mgr->action( PanId ) );
  aPanAction->insertAction( mgr->action( GlobalPanId ) );
  mgr->registerAction( aPanAction, MoveOpId );

  // reset
  aAction = new QtxAction( tr( "MNU_RESET_VIEW" ),
                           aResMgr->loadPixmap( "QxSceneViewer", tr( "ICON_QXSCENE_RESET" ) ),
                           tr( "MNU_RESET_VIEW" ), 
                           0, this);
  aAction->setStatusTip( tr( "DSC_RESET_VIEW" ) );
  connect( aAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewReset() ) );
  mgr->registerAction( aAction, ResetId );
}

/*!
  Creates toolbar of QxScene view window
*/
void QxScene_ViewWindow::createToolBar()
{
  DEBTRACE("QxScene_ViewWindow::createToolBar");
  QtxActionToolMgr* mgr = toolMgr();
  myToolBar = mgr->createToolBar( tr( "LBL_TOOLBAR_LABEL" ),          // title (language-dependant)
				  QString( "QxSceneViewOperations" ), // name (language-independant)
				  false );                            // disable floatable toolbar
  mgr->append( ScaleOpId, myToolBar );
  mgr->append( MoveOpId, myToolBar );
  mgr->append( ResetId, myToolBar );
}

/*!
  \brief Get view window's toolbar.
  \return toolbar
*/
QToolBar* QxScene_ViewWindow::getToolBar()
{
  DEBTRACE("QxScene_ViewWindow::getToolBar");
  return toolMgr()->toolBar( myToolBar );
}

/*!
  Destructor
*/
QxScene_ViewWindow::~QxScene_ViewWindow()
{
  DEBTRACE("destructor QxScene_ViewWindow");

}

/*!
  Reset the active view
*/
void QxScene_ViewWindow::onViewReset()    
{
  DEBTRACE( "QxScene_ViewWindow::onViewReset" );
}

/*!
  Sets a new center of the active view
*/
void QxScene_ViewWindow::onViewGlobalPan()
{
  DEBTRACE( "QxScene_ViewWindow::onViewGlobalPan" );
}

/*!
  Zooms the active view
*/
void QxScene_ViewWindow::onViewZoom()
{
  DEBTRACE( "QxScene_ViewWindow::onViewZoom" );
}

/*!
  Moves the active view
*/
void QxScene_ViewWindow::onViewPan()
{
  DEBTRACE( "QxScene_ViewWindow::onViewPan" );
}

/*!
  Fits all obejcts within a rectangular area of the active view
*/
void QxScene_ViewWindow::onViewFitArea()
{
  DEBTRACE( "QxScene_ViewWindow::onViewFitArea" );
}

/*!
  Fits all objects in the active view
*/
void QxScene_ViewWindow::onViewFitAll()
{
  DEBTRACE( "QxScene_ViewWindow::onViewFitAll" );
}

/*!
    Set background of the viewport
*/
void QxScene_ViewWindow::setBackgroundColor( const QColor& color )
{
  DEBTRACE("QxScene_ViewWindow::setBackgroundColor");
}

/*!
    Returns background of the viewport
*/
QColor QxScene_ViewWindow::backgroundColor() const
{
  DEBTRACE("QxScene_ViewWindow::backgroundColor");
  QColor col;
  return col;
}

/*!
  Custom resize event handler
*/
void QxScene_ViewWindow::resizeEvent( QResizeEvent* theEvent )
{
  DEBTRACE("QxScene_ViewWindow::resizeEvent");
}

/*!
  Get resource manager
*/
SUIT_ResourceMgr* QxScene_ViewWindow::resMgr() const
{ 
  DEBTRACE("QxScene_ViewWindow::resMgr");
  return SUIT_Session::session()->resourceMgr(); 
}

/*!
 *  emits a signal to ask to close the schema associated to the window.
 *  A boolean in return indicates if it is OK to close.
 */
bool QxScene_ViewWindow::closeRequested()
{
  DEBTRACE("QxScene_ViewWindow::closeRequested");
  bool isClosed = true;
  emit tryClose(isClosed, this);
  DEBTRACE("isClosed=" << isClosed);
  return isClosed;
}
