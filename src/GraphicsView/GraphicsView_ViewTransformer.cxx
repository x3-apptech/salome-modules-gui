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

#include "GraphicsView_ViewTransformer.h"

#include "GraphicsView_Scene.h"
#include "GraphicsView_ViewPort.h"
#include "GraphicsView_Viewer.h"

#include <QGraphicsSceneMouseEvent>
#include <QRectF>
#include <QRubberBand>

int GraphicsView_ViewTransformer::panBtn = Qt::MidButton;
int GraphicsView_ViewTransformer::zoomBtn = Qt::LeftButton;
int GraphicsView_ViewTransformer::fitRectBtn = Qt::LeftButton;
int GraphicsView_ViewTransformer::panGlobalBtn = Qt::LeftButton;
int GraphicsView_ViewTransformer::acccelKey = Qt::ControlModifier;

//=======================================================================
// Name    : GraphicsView_ViewTransformer
// Purpose : Constructor
//=======================================================================
GraphicsView_ViewTransformer::GraphicsView_ViewTransformer( GraphicsView_Viewer* v, int type )
: QObject( 0 ),
  myViewer( v ),
  myType( type ),
  myMajorBtn( Qt::NoButton ),
  myButtonState( 0 ),
  myRectBand( 0 )
{
  if( myType == GraphicsView_Viewer::Pan ||
      myType == GraphicsView_Viewer::Zoom ||
      myType == GraphicsView_Viewer::PanGlobal ||
      myType == GraphicsView_Viewer::FitRect )
    initTransform( true );
}

//=======================================================================
// Name    : GraphicsView_ViewTransformer
// Purpose : Destructor
//=======================================================================
GraphicsView_ViewTransformer::~GraphicsView_ViewTransformer()
{
  if( myType == GraphicsView_Viewer::Pan ||
      myType == GraphicsView_Viewer::Zoom ||
      myType == GraphicsView_Viewer::PanGlobal ||
      myType == GraphicsView_Viewer::FitRect )
    initTransform( false );

  endDrawRect();
}

//================================================================
// Function : initTransform
// Purpose  : 
//================================================================
void GraphicsView_ViewTransformer::initTransform( bool init )
{
  if( GraphicsView_ViewPort* aViewPort = myViewer->getActiveViewPort() )
  {
    if( GraphicsView_Scene* aScene = aViewPort->getScene() )
    {
      if( init )
      {
        mySavedCursor = aViewPort->cursor();
        mySavedMouseTrack = aViewPort->hasMouseTracking();
        aViewPort->setMouseTracking( false );
        aScene->installEventFilter( this );
      }
      else
      {
        aScene->removeEventFilter( this );
        aViewPort->setMouseTracking( mySavedMouseTrack );
        aViewPort->setCursor( mySavedCursor );
      }
    }
  }
}

//================================================================
// Function : exec
// Purpose  : 
//================================================================
void GraphicsView_ViewTransformer::exec()
{
  GraphicsView_ViewPort* aViewPort = myViewer->getActiveViewPort();
  if( !aViewPort )
    return;

  switch( myType )
  {
    case GraphicsView_Viewer::Zoom:
      myMajorBtn = zoomButton();
      aViewPort->setCursor( *aViewPort->getZoomCursor() );
      break;
    case GraphicsView_Viewer::Pan:
      myMajorBtn = panButton();
      aViewPort->setCursor( *aViewPort->getPanCursor() );
      break;
    case GraphicsView_Viewer::PanGlobal:
      myMajorBtn = panGlobalButton();
      aViewPort->setCursor( *aViewPort->getPanglCursor() );
      aViewPort->fitAll( true );
      break;
    case GraphicsView_Viewer::FitRect:
      myMajorBtn = fitRectButton();
      aViewPort->setCursor( *aViewPort->getHandCursor() );
      break;
    case GraphicsView_Viewer::Reset:
      aViewPort->reset();
      onTransform( Finished );
      break;
    case GraphicsView_Viewer::FitAll:
      aViewPort->fitAll();
      onTransform( Finished );
      break;
    case GraphicsView_Viewer::FitSelect:
      aViewPort->fitSelect();
      onTransform( Finished );
      break;
    case GraphicsView_Viewer::FitWidth:
      aViewPort->fitWidth();
      onTransform( Finished );
      break;
    default: break;
  }
}

//================================================================
// Function : eventFilter
// Purpose  : 
//================================================================
bool GraphicsView_ViewTransformer::eventFilter( QObject* o, QEvent* e )
{
  switch( e->type() )
  {
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    {
      TransformState state = InProcess;
      QGraphicsSceneMouseEvent* me = ( QGraphicsSceneMouseEvent* )e;

      myButtonState = me->buttons();
      if ( e->type() == QEvent::GraphicsSceneMousePress )
        myButtonState |= me->button();

      if ( e->type() == QEvent::GraphicsSceneMouseRelease )
        myButtonState |= me->button();

      int mouseOnlyState = ( myButtonState & ( Qt::LeftButton | Qt::MidButton | Qt::RightButton ) );
      if ( myStart.isNull() )
      {
        state = Begin;
        myStart = myViewer->getActiveViewPort()->mapFromScene( me->scenePos() );
        myMajorBtn = mouseOnlyState;
      }

      if ( e->type() == QEvent::GraphicsSceneMouseRelease )
        state = Finished;

      myCurr = myViewer->getActiveViewPort()->mapFromScene( me->scenePos() );
      onTransform( state );
      return true;
    }
    default: break;
  }
  return QObject::eventFilter( o, e );
}

//================================================================
// Function : onTransform
// Purpose  : 
//================================================================
void GraphicsView_ViewTransformer::onTransform( TransformState state )
{
  GraphicsView_ViewPort* aViewPort = myViewer->getActiveViewPort();
  if( !aViewPort || aViewPort->isTransforming() )
    return;

  bool doTrsf = ( myButtonState & myMajorBtn );
  switch ( myType )
  {
    case GraphicsView_Viewer::Zoom:
      if ( state != Finished && doTrsf )
      {
        aViewPort->zoom( myStart.x(), myStart.y(), myCurr.x(), myCurr.y() );
        myStart = myCurr;
      }
      break;
    case GraphicsView_Viewer::Pan:
      if ( state != Finished && doTrsf )
      {
        aViewPort->pan( myCurr.x() - myStart.x(), myStart.y() - myCurr.y() );
        myStart = myCurr;
      }
      break;
    case GraphicsView_Viewer::PanGlobal:
      if ( state == Finished )
      {
        QPointF aPoint = aViewPort->mapToScene( myCurr.toPoint() );
        aViewPort->setCenter( aPoint.x(), aPoint.y() );
      }
      break;
    case GraphicsView_Viewer::FitRect:
      if ( doTrsf )
      {
        QRectF aRect( qMin( myStart.x(), myCurr.x() ), qMin( myStart.y(), myCurr.y() ),
                      qAbs( myStart.x() - myCurr.x() ), qAbs( myStart.y() - myCurr.y() ) );
        if ( !aRect.isEmpty() )
        {
          switch ( state )
          {
            case Finished:
              aRect = aViewPort->mapToScene( aRect.toRect() ).boundingRect();
              aViewPort->fitRect( aRect );
              break;
            default:
              drawRect( aRect );
              break;
          }
        }
      }
      break;
    default:
      break;
  }

  if ( state == Finished )
    myViewer->activateTransform( GraphicsView_Viewer::NoTransform );
}

//================================================================
// Function : drawRect
// Purpose  : 
//================================================================
void GraphicsView_ViewTransformer::drawRect(const QRectF& theRect)
{
  if ( !myRectBand )
  {
    myRectBand = new QRubberBand( QRubberBand::Rectangle, myViewer->getActiveViewPort() );
    QPalette palette;
    palette.setColor(myRectBand->foregroundRole(), Qt::white);
    myRectBand->setPalette(palette);
  }
  myRectBand->hide();

  myRectBand->setGeometry( theRect.toRect() );
  myRectBand->setVisible( theRect.isValid() );
}

//================================================================
// Function : endDrawRect
// Purpose  : 
//================================================================
void GraphicsView_ViewTransformer::endDrawRect()
{
  if ( myRectBand )
  {
    myRectBand->hide();
    delete myRectBand;
    myRectBand = 0;
  }
}

//================================================================
// Function : GraphicsView_ViewTransformer
// Purpose  : 
//================================================================
int GraphicsView_ViewTransformer::type() const
{
  return myType;
}
