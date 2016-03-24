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

#include "GraphicsView_Scene.h"

#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneWheelEvent>

//#define VIEWER_DEBUG

//=======================================================================
// Name    : GraphicsView_Scene
// Purpose : Constructor
//=======================================================================
GraphicsView_Scene::GraphicsView_Scene( QObject* theParent )
: QGraphicsScene( theParent )
{
#ifdef VIEWER_DEBUG
  mySceneRectItem = new QGraphicsRectItem();
  mySceneRectItem->setPen( QPen( Qt::red, 0.0 ) );

  addItem( mySceneRectItem );

  connect( this, SIGNAL( sceneRectChanged( const QRectF& ) ),
           this, SLOT( onSceneRectChanged( const QRectF& ) ) );

  setSceneRect( -2000, -2000, 4000, 4000 );

  QGraphicsLineItem* aHorLineItem = new QGraphicsLineItem( -2000, 0, 2000, 0 );
  aHorLineItem->setPen( QPen( Qt::red ) );
  addItem( aHorLineItem );

  QGraphicsLineItem* aVerLineItem = new QGraphicsLineItem( 0, -2000, 0, 2000 );
  aVerLineItem->setPen( QPen( Qt::red ) );
  addItem( aVerLineItem );
#endif
}

//=======================================================================
// Name    : GraphicsView_Scene
// Purpose : Destructor
//=======================================================================
GraphicsView_Scene::~GraphicsView_Scene()
{
}

//================================================================
// Function : processRectChanged
// Purpose  : 
//================================================================
void GraphicsView_Scene::processRectChanged()
{
  emit gsBoundingRectChanged();
}

//================================================================
// Function : onSceneRectChanged
// Purpose  : 
//================================================================
void GraphicsView_Scene::onSceneRectChanged( const QRectF& theRect )
{
#ifdef VIEWER_DEBUG
  mySceneRectItem->setRect( theRect );
#endif
}

//================================================================
// Function : keyPressEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::keyPressEvent( QKeyEvent* e )
{
  emit gsKeyEvent( e );
  QGraphicsScene::keyPressEvent( e );
}

//================================================================
// Function : keyReleaseEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::keyReleaseEvent( QKeyEvent* e )
{
  emit gsKeyEvent( e );
  QGraphicsScene::keyReleaseEvent( e );
}

//================================================================
// Function : mousePressEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::mousePressEvent( QGraphicsSceneMouseEvent* e )
{
  emit gsMouseEvent( e );
  QGraphicsScene::mousePressEvent( e );
}

//================================================================
// Function : mouseMoveEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::mouseMoveEvent( QGraphicsSceneMouseEvent* e )
{
  emit gsMouseEvent( e );
  QGraphicsScene::mouseMoveEvent( e );
}

//================================================================
// Function : mouseReleaseEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::mouseReleaseEvent( QGraphicsSceneMouseEvent* e )
{
  emit gsMouseEvent( e );
  QGraphicsScene::mouseReleaseEvent( e );
}

//================================================================
// Function : mouseDoubleClickEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* e )
{
  emit gsMouseEvent( e );
  QGraphicsScene::mouseDoubleClickEvent( e );
}

//================================================================
// Function : wheelEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::wheelEvent( QGraphicsSceneWheelEvent* e )
{
  emit gsWheelEvent( e );

  // accept the event to prevent calling QAbstractScrollArea::wheelEvent()
  // from QGraphicsView::wheelEvent(), which will change values of scroll-bars
  e->accept();

  //QGraphicsScene::wheelEvent( e ); // don't uncomment
}

//================================================================
// Function : contextMenuEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::contextMenuEvent( QGraphicsSceneContextMenuEvent* e )
{
  emit gsContextMenuEvent( e );
  QGraphicsScene::contextMenuEvent( e );
}

//================================================================
// Function : dragEnterEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::dragEnterEvent( QGraphicsSceneDragDropEvent* e )
{
  //QGraphicsScene::dragEnterEvent( e ); // don't uncomment
}

//================================================================
// Function : dragLeaveEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::dragLeaveEvent( QGraphicsSceneDragDropEvent* e )
{
  //QGraphicsScene::dragLeaveEvent( e ); // don't uncomment
}

//================================================================
// Function : dragMoveEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::dragMoveEvent( QGraphicsSceneDragDropEvent* e )
{
  //QGraphicsScene::dragMoveEvent( e ); // don't uncomment
}

//================================================================
// Function : dropEvent
// Purpose  : 
//================================================================
void GraphicsView_Scene::dropEvent( QGraphicsSceneDragDropEvent* e )
{
  //QGraphicsScene::dropEvent( e ); // don't uncomment
}
