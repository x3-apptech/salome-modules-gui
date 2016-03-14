// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GraphicsView_ViewFrame.h"

#include "GraphicsView_Viewer.h"
#include "GraphicsView_ViewPort.h"

#include <QtxAction.h>
#include <QtxActionToolMgr.h>
#include <QtxMultiAction.h>
#include <QtxToolBar.h>

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <QColor>
#include <QFrame>
#include <QFileDialog>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QHBoxLayout>
#include <QImage>
#include <QMouseEvent>
#include <QString>
#include <QWheelEvent>

//=======================================================================
// Name    : GraphicsView_ViewFrame
// Purpose : Constructor
//=======================================================================
GraphicsView_ViewFrame::GraphicsView_ViewFrame( SUIT_Desktop* d, GraphicsView_Viewer* vw, QWidget* w )
: SUIT_ViewWindow( d ),
  myViewer( vw )
{
  QFrame* aFrame = new QFrame( this );
  setCentralWidget( aFrame );

  QBoxLayout* aLayout = new QHBoxLayout( aFrame );
  aLayout->setMargin( 0 );
  aLayout->setSpacing( 0 );

  if( w )
    myViewPort = dynamic_cast<GraphicsView_ViewPort*>(w);
  else
    myViewPort = new GraphicsView_ViewPort( aFrame );

  aLayout->addWidget( myViewPort );

  createActions();
  myToolBarId = createToolBar();

  connect( myViewPort, SIGNAL( vpKeyEvent( QKeyEvent* ) ),
           this, SLOT( keyEvent( QKeyEvent* ) ) );
  connect( myViewPort, SIGNAL( vpMouseEvent( QGraphicsSceneMouseEvent* ) ),
           this, SLOT( mouseEvent( QGraphicsSceneMouseEvent* ) ) );
  connect( myViewPort, SIGNAL( vpWheelEvent( QGraphicsSceneWheelEvent* ) ),
           this, SLOT( wheelEvent( QGraphicsSceneWheelEvent* ) ) );
  connect( myViewPort, SIGNAL( vpContextMenuEvent( QGraphicsSceneContextMenuEvent* ) ),
           this, SLOT( contextMenuEvent( QGraphicsSceneContextMenuEvent* ) ) );

  connect( myViewPort, SIGNAL( vpSketchingFinished( QPainterPath ) ),
           this, SIGNAL( sketchingFinished( QPainterPath ) ) );
}

//=======================================================================
// Name    : GraphicsView_ViewFrame
// Purpose : Destructor
//=======================================================================
GraphicsView_ViewFrame::~GraphicsView_ViewFrame()
{
}

//================================================================
// Function : createActions
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::createActions()
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QtxAction* anAction;

  // Dump view
  anAction = new QtxAction( tr( "MNU_DUMP_VIEW" ),
                           aResMgr->loadPixmap( "GraphicsView", tr( "ICON_GV_DUMP" ) ),
                           tr( "MNU_DUMP_VIEW" ), 0, this );
  anAction->setStatusTip( tr( "DSC_DUMP_VIEW" ) );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onDumpView() ) );
  toolMgr()->registerAction( anAction, DumpId );

  // FitAll
  anAction = new QtxAction( tr( "MNU_FITALL" ),
                           aResMgr->loadPixmap( "GraphicsView", tr( "ICON_GV_FITALL" ) ),
                           tr( "MNU_FITALL" ), 0, this );
  anAction->setStatusTip( tr( "DSC_FITALL" ) );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewFitAll() ) );
  toolMgr()->registerAction( anAction, FitAllId );

  // FitRect
  anAction = new QtxAction( tr( "MNU_FITRECT" ),
                           aResMgr->loadPixmap( "GraphicsView", tr( "ICON_GV_FITAREA" ) ),
                           tr( "MNU_FITRECT" ), 0, this );
  anAction->setStatusTip( tr( "DSC_FITRECT" ) );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewFitArea() ) );
  toolMgr()->registerAction( anAction, FitRectId );

  // FitSelect
  anAction = new QtxAction( tr( "MNU_FITSELECT" ),
                           aResMgr->loadPixmap( "GraphicsView", tr( "ICON_GV_FITSELECT" ) ),
                           tr( "MNU_FITSELECT" ), 0, this );
  anAction->setStatusTip( tr( "DSC_FITSELECT" ) );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewFitSelect() ) );
  toolMgr()->registerAction( anAction, FitSelectId );

  // Zoom
  anAction = new QtxAction( tr( "MNU_ZOOM_VIEW" ),
                           aResMgr->loadPixmap( "GraphicsView", tr( "ICON_GV_ZOOM" ) ),
                           tr( "MNU_ZOOM_VIEW" ), 0, this );
  anAction->setStatusTip( tr( "DSC_ZOOM_VIEW" ) );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewZoom() ) );
  toolMgr()->registerAction( anAction, ZoomId );

  // Panning
  anAction = new QtxAction( tr( "MNU_PAN_VIEW" ),
                           aResMgr->loadPixmap( "GraphicsView", tr( "ICON_GV_PAN" ) ),
                           tr( "MNU_PAN_VIEW" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PAN_VIEW" ) );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewPan() ) );
  toolMgr()->registerAction( anAction, PanId );

  // Global Panning
  anAction = new QtxAction( tr( "MNU_GLOBALPAN_VIEW" ),
                           aResMgr->loadPixmap( "GraphicsView", tr( "ICON_GV_GLOBALPAN" ) ),
                           tr( "MNU_GLOBALPAN_VIEW" ), 0, this );
  anAction->setStatusTip( tr( "DSC_GLOBALPAN_VIEW" ) );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewGlobalPan() ) );
  toolMgr()->registerAction( anAction, GlobalPanId );

  // Reset (obsolete)
  /*
  anAction = new QtxAction( tr( "MNU_RESET_VIEW" ),
                           aResMgr->loadPixmap( "GraphicsView", tr( "ICON_GV_RESET" ) ),
                           tr( "MNU_RESET_VIEW" ), 0, this );
  anAction->setStatusTip( tr( "DSC_RESET_VIEW" ) );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onViewReset() ) );
  toolMgr()->registerAction( anAction, ResetId );
  */
}

//================================================================
// Function : createToolBar
// Purpose  : 
//================================================================
int GraphicsView_ViewFrame::createToolBar()
{
  int tid = toolMgr()->createToolBar( tr("LBL_TOOLBAR_LABEL"),                 // title (language-dependant)
				      QString( "GraphicsViewViewOperations" ), // name (language-independant)
				      false );                                 // disable floatable toolbar
  toolMgr()->append( DumpId, tid );

  myScaleAction = new QtxMultiAction( this );
  myScaleAction->insertAction( toolMgr()->action( FitAllId ) );
  myScaleAction->insertAction( toolMgr()->action( FitRectId ) );
  myScaleAction->insertAction( toolMgr()->action( FitSelectId ) );
  myScaleAction->insertAction( toolMgr()->action( ZoomId ) );
  toolMgr()->append( myScaleAction, tid );

  myPanAction = new QtxMultiAction( this );
  myPanAction->insertAction( toolMgr()->action( PanId ) );
  myPanAction->insertAction( toolMgr()->action( GlobalPanId ) );
  toolMgr()->append( myPanAction, tid );

  toolMgr()->append( toolMgr()->action( ResetId ), tid );

  return tid;
}

//================================================================
// Function : getToolBarId
// Purpose  :
//================================================================
int GraphicsView_ViewFrame::getToolBarId()
{
  return myToolBarId;
}

//================================================================
// Function : dumpView
// Purpose  : 
//================================================================
QImage GraphicsView_ViewFrame::dumpView()
{
  return myViewPort->dumpView();
}

//================================================================
// Function : getVisualParameters
// Purpose  : 
//================================================================
QString GraphicsView_ViewFrame::getVisualParameters()
{
  QTransform aTransform = myViewPort->transform();

  QString aString;
  aString.sprintf( "%.3f*%.3f*%.3f*%.3f*%.3f*%.3f*%.3f*%.3f*%.3f",
    aTransform.m11(), aTransform.m12(), aTransform.m13(),
    aTransform.m21(), aTransform.m22(), aTransform.m23(),
    aTransform.m31(), aTransform.m32(), aTransform.m33() );
  return aString;
}

//================================================================
// Function : setVisualParameters
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::setVisualParameters( const QString& theParameters )
{
  QStringList aList = theParameters.split( '*' );
  if( aList.size() < 9 )
    return;

  bool anIsOk[9];
  QTransform aTransform( aList[0].toDouble( &anIsOk[0] ),
                         aList[1].toDouble( &anIsOk[1] ),
                         aList[2].toDouble( &anIsOk[2] ),
                         aList[3].toDouble( &anIsOk[3] ),
                         aList[4].toDouble( &anIsOk[4] ),
                         aList[5].toDouble( &anIsOk[5] ),
                         aList[6].toDouble( &anIsOk[6] ),
                         aList[7].toDouble( &anIsOk[7] ),
                         aList[8].toDouble( &anIsOk[8] ) );
  for( int i = 0; i < 9; i++ )
    if( !anIsOk[ i ] )
      return;

  myViewPort->setTransform( aTransform );
  myViewPort->applyTransform();
}

//================================================================
// Function : expandToolBarActions
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::expandToolBarActions()
{
  QList<QtxMultiAction*> anExpandableActions;
  anExpandableActions.append( myScaleAction );
  anExpandableActions.append( myPanAction );

  QListIterator<QtxMultiAction*> anIter( anExpandableActions );
  while( anIter.hasNext() )
  {
    if( QtxMultiAction* aMultiAction = anIter.next() )
    {
      QList<QAction*> aLocalActions = aMultiAction->actions();
      QListIterator<QAction*> aLocalIter( aLocalActions );
      while( aLocalIter.hasNext() )
        if( QAction* anAction = aLocalIter.next() )
          toolMgr()->append( anAction, myToolBarId );

      int anId = toolMgr()->actionId( aMultiAction );
      toolMgr()->remove( anId, myToolBarId );
    }
  }
}

//================================================================
// Function : onViewPan
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::onViewPan()
{
  myViewer->activateTransform( GraphicsView_Viewer::Pan );
}

//================================================================
// Function : onViewZoom
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::onViewZoom()
{
  myViewer->activateTransform( GraphicsView_Viewer::Zoom );
}

//================================================================
// Function : onViewFitAll
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::onViewFitAll()
{
  myViewer->activateTransform( GraphicsView_Viewer::FitAll );
}

//================================================================
// Function : onViewFitArea
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::onViewFitArea()
{
  myViewer->activateTransform( GraphicsView_Viewer::FitRect );
}

//================================================================
// Function : onViewFitSelect
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::onViewFitSelect()
{
  myViewer->activateTransform( GraphicsView_Viewer::FitSelect );
}

//================================================================
// Function : onViewGlobalPan
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::onViewGlobalPan()
{
  myViewer->activateTransform( GraphicsView_Viewer::PanGlobal );
}

//================================================================
// Function : onViewReset
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::onViewReset()
{
  myViewer->activateTransform( GraphicsView_Viewer::Reset );
}

//================================================================
// Function : keyEvent
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::keyEvent( QKeyEvent* e )
{
  switch ( e->type() )
  {
    case QEvent::KeyPress:
      emit keyPressed( e );
      break;
    case QEvent::KeyRelease:
      emit keyReleased( e );
      break;
    default:
      break;
  }
}

//================================================================
// Function : mouseEvent
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::mouseEvent( QGraphicsSceneMouseEvent* e )
{
  switch ( e->type() )
  {
    case QEvent::GraphicsSceneMousePress:
      emit mousePressed( e );
      break;
    case QEvent::GraphicsSceneMouseMove:
      emit mouseMoving( e );
      break;
    case QEvent::GraphicsSceneMouseRelease:
      emit mouseReleased( e );
      break;
    case QEvent::GraphicsSceneMouseDoubleClick:
      emit mouseDoubleClicked( e );
      break;
    default:
      break;
  }
}

//================================================================
// Function : wheelEvent
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::wheelEvent( QGraphicsSceneWheelEvent* e )
{
  switch ( e->type() )
  {
    case QEvent::GraphicsSceneWheel:
      emit wheeling( e );
      break;
    default:
      break;
  }
}

//================================================================
// Function : contextMenuEvent
// Purpose  : 
//================================================================
void GraphicsView_ViewFrame::contextMenuEvent( QGraphicsSceneContextMenuEvent* e )
{
  QPoint aPos = myViewPort->mapFromScene( e->scenePos() );
  QContextMenuEvent* anEvent = new QContextMenuEvent( (QContextMenuEvent::Reason)e->reason(),
                                                      aPos, e->screenPos(), e->modifiers() );
  emit contextMenuRequested( anEvent );
  delete anEvent;
}
