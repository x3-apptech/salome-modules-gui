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

#include "GraphicsView_ViewPort.h"

#include "GraphicsView_Object.h"
#include "GraphicsView_Scene.h"
#include "GraphicsView_ViewTransformer.h"

#include "SUIT_ResourceMgr.h"
#include "SUIT_Session.h"

#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QMoveEvent>
#include <QRectF>
#include <QRubberBand>
#include <QScrollBar>

#include <math.h>

#define FOREGROUND_Z_VALUE -2
#define GRID_Z_VALUE       -1
#define SKETCH_Z_VALUE     3000

int GraphicsView_ViewPort::nCounter = 0;
QCursor* GraphicsView_ViewPort::defCursor = 0;
QCursor* GraphicsView_ViewPort::handCursor = 0;
QCursor* GraphicsView_ViewPort::panCursor = 0;
QCursor* GraphicsView_ViewPort::panglCursor = 0;
QCursor* GraphicsView_ViewPort::zoomCursor = 0;
QCursor* GraphicsView_ViewPort::sketchCursor = 0;

//=======================================================================
// Name    : GraphicsView_ViewPort::ViewLabel
// Purpose : Wrapper for label, which can ignore move events sent from
//           QGraphicsView::scrollContentsBy() method, which,
//           in its turn, called from GraphicsView_ViewPort::pan()
//=======================================================================
class GraphicsView_ViewPort::ViewLabel : public QLabel
{
public:
  ViewLabel( QWidget* theParent )
  : QLabel( theParent ),
    myAcceptMoveEvents( false )
  {
  }
  ~ViewLabel() {}

  void setAcceptMoveEvents( bool theFlag )
  {
    myAcceptMoveEvents = theFlag;
  }

protected:
  virtual void moveEvent( QMoveEvent* theEvent )
  {
    if( myAcceptMoveEvents )
      QLabel::moveEvent( theEvent );
    else // return the label to the initial position
    {
      myAcceptMoveEvents = true;
      move( theEvent->oldPos() );
      myAcceptMoveEvents = false;
    }
  }

private:
  bool myAcceptMoveEvents;
};

//================================================================
// Function : createCursors
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::createCursors ()
{
  defCursor   = new QCursor( Qt::ArrowCursor );
  handCursor  = new QCursor( Qt::PointingHandCursor );
  panCursor   = new QCursor( Qt::SizeAllCursor );
  panglCursor = new QCursor( Qt::CrossCursor );

  SUIT_ResourceMgr* rmgr = SUIT_Session::session()->resourceMgr();
  zoomCursor   = new QCursor( rmgr->loadPixmap( "GraphicsView", tr( "ICON_GV_CURSOR_ZOOM" ) ) );

  sketchCursor = new QCursor( Qt::CrossCursor );
}

//================================================================
// Function : destroyCursors
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::destroyCursors()
{
  delete defCursor;    defCursor    = 0;
  delete handCursor;   handCursor   = 0;
  delete panCursor;    panCursor    = 0;
  delete panglCursor;  panglCursor  = 0;
  delete zoomCursor;   zoomCursor   = 0;
  delete sketchCursor; sketchCursor = 0;
}

//=======================================================================
// Name    : GraphicsView_ViewPort
// Purpose : Constructor
//=======================================================================
GraphicsView_ViewPort::GraphicsView_ViewPort( QWidget* theParent )
: QGraphicsView( theParent ),
  myInteractionFlags( 0 ),
  myViewLabel( 0 ),
  myViewLabelPosition( VLP_None ),
  myViewLabelLayout( 0 ),
  myIsMousePositionEnabled( false ),
  myForegroundItem( 0 ),
  myGridItem( 0 ),
  myIsTransforming( false ),
  myHighlightedObject( 0 ),
  myHighlightX( 0 ),
  myHighlightY( 0 ),
  myIsHighlighting( false ),
  mySelectionIterator( 0 ),
  myRectBand( 0 ),
  myAreSelectionPointsInitialized( false ),
  mySketchingItem( 0 ),
  myIsPrepareToSketch( false ),
  myIsSketching( false ),
  myIsSketchingByPath( false ),
  myIsDragging( false ),
  myIsDragPositionInitialized( false ),
  myIsPulling( false ),
  myPullingObject( 0 ),
  myStoredCursor( Qt::ArrowCursor )
{
  // scene
  myScene = new GraphicsView_Scene( this );
  setScene( myScene );

  mySceneGap = 20;
  myFitAllGap = 40;

  // interaction flags
  setInteractionFlags( EditFlags );
  //setInteractionFlag( TraceBoundingRect );
  //setInteractionFlag( DraggingByMiddleButton );
  //setInteractionFlag( ImmediateContextMenu );
  //setInteractionFlag( ImmediateSelection );
  //setInteractionFlag( Sketching );

  // background
  setBackgroundBrush( QBrush( Qt::white ) );

  // foreground
  myIsForegroundEnabled = false;
  myForegroundSize = QSizeF( 100, 30 );
  myForegroundMargin = 0.0;
  myForegroundColor = Qt::white;
  myForegroundFrameColor = Qt::black;
  myForegroundFrameLineWidth = 1.0;

  // grid
  myIsGridEnabled = false;
  myGridCellSize = 100;
  myGridLineStyle = Qt::DotLine;
  myGridLineColor = Qt::darkGray;

  // default index method (BspTreeIndex) leads to
  // crashes in QGraphicsView::paintEvent() method
  myScene->setItemIndexMethod( QGraphicsScene::NoIndex );

  // render hints (default - TextAntialiasing only)
  setRenderHints( QPainter::Antialiasing |
                  QPainter::TextAntialiasing |
                  QPainter::SmoothPixmapTransform |
                  QPainter::HighQualityAntialiasing );

  connect( myScene, SIGNAL( gsKeyEvent( QKeyEvent* ) ),
           this, SLOT( onKeyEvent( QKeyEvent* ) ) );
  connect( myScene, SIGNAL( gsMouseEvent( QGraphicsSceneMouseEvent* ) ),
           this, SLOT( onMouseEvent( QGraphicsSceneMouseEvent* ) ) );
  connect( myScene, SIGNAL( gsWheelEvent( QGraphicsSceneWheelEvent* ) ),
           this, SLOT( onWheelEvent( QGraphicsSceneWheelEvent* ) ) );
  connect( myScene, SIGNAL( gsContextMenuEvent( QGraphicsSceneContextMenuEvent* ) ),
           this, SLOT( onContextMenuEvent( QGraphicsSceneContextMenuEvent* ) ) );

  connect( myScene, SIGNAL( gsBoundingRectChanged() ),
           this, SLOT( onBoundingRectChanged() ) );

  initialize();
}

//=======================================================================
// Name    : GraphicsView_ViewPort
// Purpose : Destructor
//=======================================================================
GraphicsView_ViewPort::~GraphicsView_ViewPort()
{
  cleanup();

  if( myScene )
  {
    delete myScene;
    myScene = 0;
  }
}

//================================================================
// Function : initialize
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::initialize()
{
  if ( nCounter++ == 0 )
    createCursors();

  setMouseTracking( true );
  setFocusPolicy( Qt::StrongFocus );
}

//================================================================
// Function : cleanup
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::cleanup()
{
  if ( --nCounter == 0 )
    destroyCursors();
}

//================================================================
// Function : addItem
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::addItem( QGraphicsItem* theItem )
{
  if( GraphicsView_Object* anObject = dynamic_cast<GraphicsView_Object*>( theItem ) )
  {
    int aPriority = anObject->getPriority();
    GraphicsView_ObjectList::iterator anIter, anIterEnd = myObjects.end();
    for( anIter = myObjects.begin(); anIter != anIterEnd; anIter++ )
    {
      if( GraphicsView_Object* anObjectRef = *anIter )
      {
        if( anObjectRef->getPriority() > aPriority )
          break;
      }
    }
    myObjects.insert( anIter, anObject );
    anObject->setViewTransform( transform() );
    anObject->addTo( this );
  }
  else
    myScene->addItem( theItem );
  onBoundingRectChanged();
}

//================================================================
// Function : isItemAdded
// Purpose  :
//================================================================
bool GraphicsView_ViewPort::isItemAdded( QGraphicsItem* theItem )
{
  if( GraphicsView_Object* anObject = dynamic_cast<GraphicsView_Object*>( theItem ) )
  {
    for( GraphicsView_ObjectList::iterator anIter = myObjects.begin(); anIter != myObjects.end(); anIter++ )
      if( theItem == *anIter )
        return true;
  }
  else {
    for( int i = 0; i < myScene->items().size(); i++ )
      if( theItem == myScene->items().at(i) )
        return true;
  }
  return false;
}

//================================================================
// Function : removeItem
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::removeItem( QGraphicsItem* theItem )
{
  if( GraphicsView_Object* anObject = dynamic_cast<GraphicsView_Object*>( theItem ) )
  {
    if( myHighlightedObject == anObject )
      myHighlightedObject = 0;
    mySelectedObjects.removeAll( anObject );
    myObjects.removeAll( anObject );
    anObject->removeFrom( this );
  }
  else
    myScene->removeItem( theItem );
  onBoundingRectChanged();
}

//================================================================
// Function : getObjects
// Purpose  : 
//================================================================
GraphicsView_ObjectList GraphicsView_ViewPort::getObjects( SortType theSortType ) const
{
  if( theSortType == SelectedFirst )
  {
    // to append selected objects after their non-selected siblings with similar priority
    int aCurrentPriority = -1;
    GraphicsView_ObjectList aSelectedObjects;
    GraphicsView_ObjectList aTopmostObjects;

    GraphicsView_ObjectList aList;
    GraphicsView_ObjectListIterator anIter( myObjects );
    while( anIter.hasNext() )
    {
      if( GraphicsView_Object* anObject = anIter.next() )
      {
        if( anObject->isOnTop() )
        {
          aTopmostObjects.append( anObject );
          continue;
        }

        int aPriority = anObject->getPriority();
        if( aPriority > aCurrentPriority  )
        {
          if( !aSelectedObjects.isEmpty() )
          {
            aList.append( aSelectedObjects );
            aSelectedObjects.clear();
          }
          aCurrentPriority = aPriority;
        }

        if( anObject->isSelected() )
          aSelectedObjects.append( anObject );
        else
          aList.append( anObject );
      }
    }

    // for selected objects with highest priority,
    // which were not pushed to the result list yet
    if( !aSelectedObjects.isEmpty() )
    {
      aList.append( aSelectedObjects );
      aSelectedObjects.clear();
    }

    aList.append( aTopmostObjects );

    return aList;
  }

  if( theSortType == SortByZLevel ) // double loop, needs to be optimized
  {
    GraphicsView_ObjectList aList;

    GraphicsView_ObjectListIterator anIter( myObjects );
    while( anIter.hasNext() )
    {
      if( GraphicsView_Object* anObject = anIter.next() )
      {
        double aZValue = anObject->zValue();
        GraphicsView_ObjectList::iterator anIter1, anIter1End = aList.end();
        for( anIter1 = aList.begin(); anIter1 != anIter1End; anIter1++ )
          if( GraphicsView_Object* anObjectRef = *anIter1 )
            if( anObjectRef->zValue() > aZValue )
              break;
        aList.insert( anIter1, anObject );
      }
    }
    return aList;
  }

  return myObjects; // theSortType == NoSorting
}

//================================================================
// Function : objectsBoundingRect
// Purpose  : 
//================================================================
QRectF GraphicsView_ViewPort::objectsBoundingRect( bool theOnlyVisible ) const
{
  QRectF aRect;
  QListIterator<QGraphicsItem*> anIter( items() );
  while( anIter.hasNext() )
  {
    if( GraphicsView_Object* anObject = dynamic_cast<GraphicsView_Object*>( anIter.next() ) )
    {
      if( theOnlyVisible && !anObject->isVisible() )
        continue;

      QRectF anObjectRect = anObject->getRect();
      if( !anObjectRect.isNull() )
      {
        if( aRect.isNull() )
          aRect = anObject->getRect();
        else
          aRect |= anObject->getRect();
      }
    }
  }
  return aRect;
}

//================================================================
// Function : dumpView
// Purpose  : 
//================================================================
QImage GraphicsView_ViewPort::dumpView( bool theWholeScene,
                                        QSizeF theSize )
{
  if( !theWholeScene ) // just grab the view contents
  {
    QPixmap aPixmap = QPixmap::grabWindow( viewport()->winId() );
    return aPixmap.toImage();
  }

  // get a bounding rect of all presented objects
  // (itemsBoundingRect() method is unsuitable)
  QRectF aRect = objectsBoundingRect();
  if( aRect.isNull() )
    return QImage();

  QRectF aTargetRect( 0, 0, aRect.width(), aRect.height() );
  if( theSize.isValid() )
  {
    double aRatioX = theSize.width() / aTargetRect.width();
    double aRatioY = theSize.height() / aTargetRect.height();
    double aRatio = qMin( aRatioX, aRatioY );
    aTargetRect.setWidth( aTargetRect.width() * aRatio );
    aTargetRect.setHeight( aTargetRect.height() * aRatio );
  }

  // render the scene to an image
  QImage anImage( aTargetRect.toRect().size(), QImage::Format_RGB32 );
  QPainter aPainter( &anImage );
  aPainter.setRenderHints( renderHints() );

  myScene->render( &aPainter, aTargetRect, aRect );

  return anImage;
}

//================================================================
// Function : setSceneGap
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setSceneGap( double theSceneGap )
{
  mySceneGap = theSceneGap;
  onBoundingRectChanged();
}

//================================================================
// Function : setFitAllGap
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setFitAllGap( double theFitAllGap )
{
  myFitAllGap = theFitAllGap;
}

//================================================================
// Function : interactionFlags
// Purpose  : 
//================================================================
int GraphicsView_ViewPort::interactionFlags() const
{
  return myInteractionFlags;
}

//================================================================
// Function : hasInteractionFlag
// Purpose  : 
//================================================================
bool GraphicsView_ViewPort::hasInteractionFlag( InteractionFlag theFlag )
{
  return ( interactionFlags() & theFlag ) == theFlag;
}

//================================================================
// Function : setInteractionFlag
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setInteractionFlag( InteractionFlag theFlag,
                                                bool theIsEnabled )
{
  if( theIsEnabled )
    setInteractionFlags( myInteractionFlags | theFlag );
  else
    setInteractionFlags( myInteractionFlags & ~theFlag );
}

//================================================================
// Function : setInteractionFlags
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setInteractionFlags( InteractionFlags theFlags )
{
  myInteractionFlags = theFlags;
}

//================================================================
// Function : setViewLabelPosition
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setViewLabelPosition( ViewLabelPosition thePosition,
                                                  bool theIsForced )
{
  if( theIsForced && !myViewLabel )
    myViewLabel = new ViewLabel( viewport() );

  if( !myViewLabel )
    return;

  if( thePosition == VLP_None )
  {
    myViewLabel->setVisible( false );
    return;
  }

  if( myViewLabelLayout )
    delete myViewLabelLayout;

  myViewLabelLayout = new QGridLayout( viewport() );
  myViewLabelLayout->setMargin( 10 );
  myViewLabelLayout->setSpacing( 0 );

  int aRow = 0, aColumn = 0;
  switch( thePosition )
  {
    case VLP_TopLeft:     aRow = 0; aColumn = 0; break;
    case VLP_TopRight:    aRow = 0; aColumn = 1; break;
    case VLP_BottomLeft:  aRow = 1; aColumn = 0; break;
    case VLP_BottomRight: aRow = 1; aColumn = 1; break;
    default: break;
  }

  myViewLabelLayout->addWidget( myViewLabel, aRow, aColumn );
  myViewLabelLayout->setRowStretch( 1 - aRow, 1 );
  myViewLabelLayout->setColumnStretch( 1 - aColumn, 1 );

  myViewLabel->setVisible( true );
}

//================================================================
// Function : setViewLabelText
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setViewLabelText( const QString& theText )
{
  if( myViewLabel )
    myViewLabel->setText( theText );
}

//================================================================
// Function : setMousePositionEnabled
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setMousePositionEnabled( bool theState )
{
  myIsMousePositionEnabled = theState;

  if( theState )
  {
    setViewLabelPosition( VLP_BottomLeft, true );

    int aMouseX = 0, aMouseY = 0;
    setViewLabelText( QString( "(%1, %2)" ).arg( aMouseX ).arg( aMouseY ) );
  }
  else
    setViewLabelPosition( VLP_None );
}

//================================================================
// Function : backgroundColor
// Purpose  : 
//================================================================
QColor GraphicsView_ViewPort::backgroundColor() const
{
  return backgroundBrush().color();
}

//================================================================
// Function : setBackgroundColor
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setBackgroundColor( const QColor& theColor )
{
  setBackgroundBrush( QBrush( theColor ) );
}

//================================================================
// Function : setForegroundEnabled
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setForegroundEnabled( bool theState )
{
  myIsForegroundEnabled = theState;
}

//================================================================
// Function : setForegroundSize
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setForegroundSize( const QSizeF& theSize )
{
  myForegroundSize = theSize;
}

//================================================================
// Function : setForegroundMargin
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setForegroundMargin( double theMargin )
{
  myForegroundMargin = theMargin;
}

//================================================================
// Function : setForegroundColor
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setForegroundColor( const QColor& theColor )
{
  myForegroundColor = theColor;
}

//================================================================
// Function : setForegroundFrameColor
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setForegroundFrameColor( const QColor& theColor )
{
  myForegroundFrameColor = theColor;
}

//================================================================
// Function : setForegroundFrameLineWidth
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setForegroundFrameLineWidth( double theLineWidth )
{
  myForegroundFrameLineWidth = theLineWidth;
}

//================================================================
// Function : updateForeground
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::updateForeground()
{
  if( myIsForegroundEnabled )
  {
    if( !myForegroundItem )
      myForegroundItem = myScene->addRect( QRectF(), QPen(), QBrush( Qt::white ) );
    myForegroundItem->setZValue( FOREGROUND_Z_VALUE );

    QPointF aPoint = QPointF();
    QRectF aRect( aPoint, myForegroundSize );
    aRect.adjust( -myForegroundMargin, -myForegroundMargin,
                  myForegroundMargin, myForegroundMargin );
    myForegroundItem->setRect( aRect );

    QBrush aBrush = myForegroundItem->brush();
    aBrush.setColor( myForegroundColor );
    myForegroundItem->setBrush( aBrush );

    QPen aPen = myForegroundItem->pen();
    aPen.setColor( myForegroundFrameColor );
    aPen.setWidthF( myForegroundFrameLineWidth );
    myForegroundItem->setPen( aPen );

    myForegroundItem->setVisible( true );

    myScene->setSceneRect( aRect.adjusted( -mySceneGap, -mySceneGap, mySceneGap, mySceneGap ) );
  }
  else
  {
    if( myForegroundItem )
      myForegroundItem->setVisible( false );
  }

  updateGrid(); // foreground size could be changed
}

//================================================================
// Function : setGridEnabled
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setGridEnabled( bool theState )
{
  myIsGridEnabled = theState;
}

//================================================================
// Function : setGridCellSize
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setGridCellSize( int theCellSize )
{
  myGridCellSize = theCellSize;
}

//================================================================
// Function : setGridLineStyle
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setGridLineStyle( int theLineStyle )
{
  myGridLineStyle = theLineStyle;
}

//================================================================
// Function : setGridLineColor
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setGridLineColor( const QColor& theLineColor )
{
  myGridLineColor = theLineColor;
}

//================================================================
// Function : updateGrid
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::updateGrid()
{
  if( myIsGridEnabled )
  {
    if( !myGridItem )
      myGridItem = myScene->addPath( QPainterPath() );
    myGridItem->setZValue( GRID_Z_VALUE );

    double aWidth = myForegroundSize.width();
    double aHeight = myForegroundSize.height();

    int aGridNbX = int( aWidth / myGridCellSize ) + 1;
    int aGridNbY = int( aHeight / myGridCellSize ) + 1;

    int anIndex;
    QPainterPath aPath;
    for( anIndex = 0; anIndex < aGridNbX; anIndex++ )
    {
      double x = myGridCellSize * (double)anIndex;
      aPath.moveTo( x, 0 );
      aPath.lineTo( x, aHeight );
    }
    for( anIndex = 0; anIndex < aGridNbY; anIndex++ )
    {
      double y = myGridCellSize * (double)anIndex;
      aPath.moveTo( 0, y );
      aPath.lineTo( aWidth, y );
    }
    myGridItem->setPath( aPath );

    QPen aPen = myGridItem->pen();
    aPen.setStyle( (Qt::PenStyle)myGridLineStyle );
    aPen.setColor( myGridLineColor );
    myGridItem->setPen( aPen );

    myGridItem->setVisible( true );
  }
  else
  {
    if( myGridItem )
      myGridItem->setVisible( false );
  }
}

//================================================================
// Function : reset
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::reset()
{
  fitAll();
}

//================================================================
// Function : pan
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::pan( double theDX, double theDY )
{
  myIsTransforming = true;

  if( myViewLabel )
    myViewLabel->setAcceptMoveEvents( false );

  if( QScrollBar* aHBar = horizontalScrollBar() )
    aHBar->setValue( aHBar->value() - theDX );
  if( QScrollBar* aVBar = verticalScrollBar() )
    aVBar->setValue( aVBar->value() + theDY );

  if( myViewLabel )
    myViewLabel->setAcceptMoveEvents( true );

  myIsTransforming = false;

  applyTransform();
}

//================================================================
// Function : setCenter
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setCenter( double theX, double theY )
{
  myIsTransforming = true;

  setTransform( myCurrentTransform );
  centerOn( theX, theY );

  myIsTransforming = false;

  applyTransform();
}

//================================================================
// Function : zoom
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::zoom( double theX1, double theY1, double theX2, double theY2 )
{
  myIsTransforming = true;

  double aDX = theX2 - theX1;
  double aDY = theY2 - theY1;
  double aZoom = sqrt( aDX * aDX + aDY * aDY ) / 100 + 1;
  aZoom = ( aDX > 0 ) ?  aZoom : 1 / aZoom;

  QTransform aTransform = transform();
  aTransform.scale( aZoom, aZoom );
  double aM11 = aTransform.m11();
  double aM22 = aTransform.m22();
  // increasing of diagonal coefficients (>300) leads to a crash sometimes
  // at the values of 100 some primitives are drawn incorrectly
  if( qMax( aM11, aM22 ) < 100 )
    setTransform( aTransform );

  myIsTransforming = false;

  applyTransform();
}

//================================================================
// Function : fitRect
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::fitRect( const QRectF& theRect )
{
  myIsTransforming = true;

  fitInView( theRect, Qt::KeepAspectRatio );

  myIsTransforming = false;

  applyTransform();
}

//================================================================
// Function : fitSelect
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::fitSelect()
{
  myIsTransforming = true;

  QRectF aGlobalRect;
  for( initSelected(); moreSelected(); nextSelected() )
  {
    if( GraphicsView_Object* aMovingObject = selectedObject() )
    {
      QRectF aRect = aMovingObject->getRect();
      if( aGlobalRect.isNull() )
        aGlobalRect = aRect;
      else
        aGlobalRect |= aRect;
    }
  }

  if( !aGlobalRect.isNull() )
  {
    double aGap = qMax( aGlobalRect.width(), aGlobalRect.height() ) / 5;
    aGlobalRect.adjust( -aGap, -aGap, aGap, aGap );
    fitInView( aGlobalRect, Qt::KeepAspectRatio );
  }

  myIsTransforming = false;

  applyTransform();
}

//================================================================
// Function : fitAll
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::fitAll( bool theKeepScale )
{
  myIsTransforming = true;

  if( theKeepScale )
    myCurrentTransform = transform();

  double aGap = myFitAllGap;
  QRectF aRect = objectsBoundingRect( true );
  fitInView( aRect.adjusted( -aGap, -aGap, aGap, aGap ), Qt::KeepAspectRatio );

  myIsTransforming = false;

  applyTransform();
}

//================================================================
// Function : fitWidth
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::fitWidth()
{
  myIsTransforming = true;

  double aGap = myFitAllGap;
  QRectF aRect = objectsBoundingRect( true );

  double aTop = aRect.top();
  double aLeft = aRect.left();
  double aMargin = 10;

  aRect.setY( aRect.center().y() );
  aRect.setHeight( aMargin );

  fitInView( aRect.adjusted( -aGap, -aGap, aGap, aGap ), Qt::KeepAspectRatio );
  ensureVisible( aLeft, aTop, aMargin, aMargin, 0, aGap );

  myIsTransforming = false;

  applyTransform();
}

//================================================================
// Function : applyTransform
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::applyTransform()
{
  GraphicsView_ObjectListIterator anIter( getObjects() );
  while( anIter.hasNext() )
    if( GraphicsView_Object* anObject = anIter.next() )
      anObject->setViewTransform( transform() );
}

//================================================================
// Function : currentBlock
// Purpose  : 
//================================================================
GraphicsView_ViewPort::BlockStatus GraphicsView_ViewPort::currentBlock()
{
  if( isDragging() && !myDragPosition.isNull() )
    return BlockStatus( BS_Selection );

  if( myAreSelectionPointsInitialized && ( myFirstSelectionPoint != myLastSelectionPoint ) )
    return BlockStatus( BS_Selection );

  if( isPulling() )
    return BlockStatus( BS_Selection );

  return BS_NoBlock;
}

//================================================================
// Function : highlight
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::highlight( double theX, double theY )
{
  myIsHighlighting = true;
  myHighlightX = theX;
  myHighlightY = theY;

  bool anIsHighlighted = false;
  bool anIsOnObject = false;

  GraphicsView_Object* aPreviousHighlightedObject = myHighlightedObject;
  GraphicsView_Object* aHighlightedObject = 0;

  QCursor aCursor;

  GraphicsView_ObjectList aList = getObjects( SortByZLevel );
  GraphicsView_ObjectListIterator anIter( aList );
  anIter.toBack(); // objects with higher priority have to be checked earlier
  while( anIter.hasPrevious() )
  {
    if( GraphicsView_Object* anObject = anIter.previous() )
    {
      if( anObject->isVisible() && anObject->isSelectable() )
      {
        if( anObject->checkHighlight( theX, theY, aCursor ) )
        {
          anIsOnObject = true;
          anIsHighlighted = anObject->highlight( theX, theY );
        }

        if( anIsHighlighted )
        {
          aHighlightedObject = anObject;
          break;
        }
      }
    }
  }

  setCursor( aCursor );

  if( !anIsOnObject )
  {
    anIter = aList;
    while( anIter.hasNext() )
      if( GraphicsView_Object* anObject = anIter.next() )
        anObject->unhighlight();

    myHighlightedObject = 0;
    return;
  }
  else if( !myHighlightedObject && anIsHighlighted )
  {
    myHighlightedObject = aHighlightedObject;
  }
  else if( myHighlightedObject && !anIsHighlighted )
  {
    myHighlightedObject->unhighlight();
    myHighlightedObject = 0;
  }
  else if( myHighlightedObject && anIsHighlighted )
  {
    myHighlightedObject->highlight( theX, theY );
    if( myHighlightedObject != aHighlightedObject )
    {
      myHighlightedObject->unhighlight();
      myHighlightedObject = aHighlightedObject;
    }
  }
}

//================================================================
// Function : clearHighlighted
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::clearHighlighted()
{
  if( myHighlightedObject )
  {
    myHighlightedObject->unhighlight();
    myHighlightedObject = 0;
  }
}

//================================================================
// Function : select
// Purpose  : 
//================================================================
int GraphicsView_ViewPort::select( const QRectF& theRect, bool theIsAppend )
{
  if( !myIsHighlighting )
    return GVSS_NoChanged;

  GV_SelectionStatus aStatus = GVSS_Invalid;
  if( theRect.isNull() ) // point selection
  {
    if( myHighlightedObject )
    {
      if( mySelectedObjects.count() == 1 &&
          mySelectedObjects.first() == myHighlightedObject )
        aStatus = GVSS_LocalChanged;

      if( !theIsAppend )
      {
        GraphicsView_ObjectListIterator anIter( mySelectedObjects );
        while( anIter.hasNext() )
          if( GraphicsView_Object* anObject = anIter.next() )
            if( myHighlightedObject != anObject )
              anObject->unselect();

        if( !mySelectedObjects.isEmpty() && aStatus == GVSS_Invalid )
          aStatus = GVSS_GlobalChanged;
        mySelectedObjects.clear();
      } 
      else if( myHighlightedObject->isSelected() && aStatus != GVSS_LocalChanged )
      {
        mySelectedObjects.removeAll( myHighlightedObject );
        myHighlightedObject->unselect();

        if( !mySelectedObjects.isEmpty() && aStatus == GVSS_Invalid )
          aStatus = GVSS_GlobalChanged;

        return aStatus;
      }

      if( myHighlightedObject->select( myHighlightX, myHighlightY, QRectF() ) &&
          mySelectedObjects.indexOf( myHighlightedObject ) == -1 )
      {
        mySelectedObjects.append( myHighlightedObject );
        if( aStatus == GVSS_Invalid )
          aStatus = GVSS_GlobalChanged;
      }
      else if( aStatus == GVSS_LocalChanged )
        aStatus = GVSS_GlobalChanged;

      return aStatus;
    }

    if( !myHighlightedObject )
    {
      if( !theIsAppend )
      {
        GraphicsView_ObjectListIterator anIter( mySelectedObjects );
        while( anIter.hasNext() )
          if( GraphicsView_Object* anObject = anIter.next() )
            if( myHighlightedObject != anObject )
              anObject->unselect();

        if( !mySelectedObjects.isEmpty() )
          aStatus = GVSS_GlobalChanged;
        mySelectedObjects.clear();
      }
      return aStatus;
    }

    return GVSS_NoChanged;
  }
  else // rectangle selection
  {
    aStatus = GVSS_NoChanged;

    bool updateAll = false;
    if( !theIsAppend )
    {
      if( !mySelectedObjects.isEmpty() )
        aStatus = GVSS_GlobalChanged;

      GraphicsView_ObjectListIterator anIter( mySelectedObjects );
      while( anIter.hasNext() )
        if( GraphicsView_Object* anObject = anIter.next() )
          if( myHighlightedObject != anObject )
            anObject->unselect();
      mySelectedObjects.clear();
    }

    QListIterator<QGraphicsItem*> anIter( items() );
    while( anIter.hasNext() )
    {
      if( GraphicsView_Object* anObject = dynamic_cast<GraphicsView_Object*>( anIter.next() ) )
      {
        if( anObject->isVisible() && anObject->isSelectable() )
        {
          bool anIsSelected = false;
          QRectF aRect = anObject->getRect();
          if( theRect.contains( aRect ) && myIsHighlighting )
            anIsSelected = anObject->select( myHighlightX, myHighlightY, theRect );

          if( anIsSelected && mySelectedObjects.indexOf( anObject ) == -1 )
          {
            mySelectedObjects.append( anObject );
            aStatus = GVSS_GlobalChanged;
          }
        }
      }
    }
  }
  return aStatus;
}

//================================================================
// Function : clearSelected
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::clearSelected()
{
  GraphicsView_ObjectListIterator anIter( mySelectedObjects );
  while( anIter.hasNext() )
    if( GraphicsView_Object* anObject = anIter.next() )
      anObject->unselect();
  mySelectedObjects.clear();
}

//================================================================
// Function : setSelected
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::setSelected( GraphicsView_Object* theObject )
{
  if( theObject )
  {
    theObject->setSelected( true );
    mySelectedObjects.append( theObject );
  }
}

//================================================================
// Function : nbSelected
// Purpose  : 
//================================================================
int GraphicsView_ViewPort::nbSelected() const
{
  return mySelectedObjects.count();
}

//================================================================
// Function : initSelected
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::initSelected()
{
  mySelectionIterator = 0;
}

//================================================================
// Function : moreSelected
// Purpose  : 
//================================================================
bool GraphicsView_ViewPort::moreSelected()
{
  return mySelectionIterator < nbSelected();
}

//================================================================
// Function : nextSelected
// Purpose  : 
//================================================================
bool GraphicsView_ViewPort::nextSelected()
{
  if( mySelectionIterator >= 0 && mySelectionIterator < nbSelected() )
  {
    mySelectionIterator++;
    return true;
  }
  return false;
}

//================================================================
// Function : selectedObject
// Purpose  : 
//================================================================
GraphicsView_Object* GraphicsView_ViewPort::selectedObject()
{
  if( mySelectionIterator >= 0 && mySelectionIterator < nbSelected() )
    return mySelectedObjects[ mySelectionIterator ];
  return 0;
}

//================================================================
// Function : startSelectByRect
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::startSelectByRect( int x, int y )
{
  if( !myAreSelectionPointsInitialized )
  {
    myFirstSelectionPoint = QPoint( x, y );
    myLastSelectionPoint = QPoint( x, y );
    myAreSelectionPointsInitialized = true;
  }

  if( !myRectBand )
  {
    myRectBand = new QRubberBand( QRubberBand::Rectangle, this );
    QPalette palette;
    palette.setColor( myRectBand->foregroundRole(), Qt::white );
    myRectBand->setPalette( palette );
  }
  myRectBand->hide();
}

//================================================================
// Function : drawSelectByRect
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::drawSelectByRect( int x, int y )
{
  if( myAreSelectionPointsInitialized )
  {
    myRectBand->hide();

    myLastSelectionPoint.setX( x );
    myLastSelectionPoint.setY( y );

    QRect aRect = selectionRect();
    myRectBand->setGeometry( aRect );
    myRectBand->setVisible( aRect.isValid() );
  }
}

//================================================================
// Function : isSelectByRect
// Purpose  : 
//================================================================
bool GraphicsView_ViewPort::isSelectByRect() const
{
  return myAreSelectionPointsInitialized;
}

//================================================================
// Function : finishSelectByRect
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::finishSelectByRect()
{
  if( myAreSelectionPointsInitialized )
  {
    if( myRectBand )
    {
      myRectBand->hide();
      delete myRectBand;
      myRectBand = 0;
    }

    myAreSelectionPointsInitialized = false;
  }
}

//================================================================
// Function : selectionRect
// Purpose  : 
//================================================================
QRect GraphicsView_ViewPort::selectionRect()
{
  QRect aRect;
  if( myAreSelectionPointsInitialized )
  {
    aRect.setLeft( qMin( myFirstSelectionPoint.x(), myLastSelectionPoint.x() ) );
    aRect.setTop( qMin( myFirstSelectionPoint.y(), myLastSelectionPoint.y() ) );
    aRect.setRight( qMax( myFirstSelectionPoint.x(), myLastSelectionPoint.x() ) );
    aRect.setBottom( qMax( myFirstSelectionPoint.y(), myLastSelectionPoint.y() ) );
  }
  return aRect;
}

//================================================================
// Function : prepareToSketch
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::prepareToSketch( bool theStatus )
{
  myIsPrepareToSketch = theStatus;
  if( theStatus )
    setCursor( *getSketchCursor() );
}

//================================================================
// Function : isPrepareToSketch
// Purpose  : 
//================================================================
bool GraphicsView_ViewPort::isPrepareToSketch()
{
  return myIsPrepareToSketch;
}

//================================================================
// Function : startSketching
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::startSketching( const QPointF& thePoint,
                                            bool theIsPath )
{
  prepareToSketch( false );

  if( !mySketchingItem )
  {
    mySketchingItem = new QGraphicsPathItem();
    mySketchingItem->setZValue( SKETCH_Z_VALUE );

    QPen aPen = mySketchingItem->pen();
    aPen.setStyle( Qt::DotLine );
    mySketchingItem->setPen( aPen );

    addItem( mySketchingItem );
  }

  mySketchingPoint = thePoint;

  QPainterPath aPath;
  aPath.moveTo( mySketchingPoint );
  mySketchingItem->setPath( aPath );
  mySketchingItem->setVisible( true );

  myIsSketching = true;
  myIsSketchingByPath = theIsPath;
}

//================================================================
// Function : drawSketching
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::drawSketching( const QPointF& thePoint )
{
  bool anIsPath = false;
  if( mySketchingItem && isSketching( &anIsPath ) )
  {
    QPainterPath aPath = mySketchingItem->path();
    if( anIsPath ) // arbitrary path
      aPath.lineTo( thePoint );
    else // rectangle
    {
      // make a valid rectangle
      double x1 = mySketchingPoint.x(), y1 = mySketchingPoint.y();
      double x2 = thePoint.x(), y2 = thePoint.y();
      QPointF aPoint1( qMin( x1, x2 ), qMin( y1, y2 ) );
      QPointF aPoint2( qMax( x1, x2 ), qMax( y1, y2 ) );
      QRectF aRect( aPoint1, aPoint2 );

      aPath = QPainterPath();
      aPath.addRect( aRect );
    }
    mySketchingItem->setPath( aPath );
  }
}

//================================================================
// Function : finishSketching
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::finishSketching( bool theStatus )
{
  prepareToSketch( false );

  mySketchingItem->setVisible( false );
  myIsSketching = false;

  setCursor( *getDefaultCursor() );

  if( theStatus )
  {
    QPainterPath aPath = mySketchingItem->path();
    emit vpSketchingFinished( aPath );
  }
}

//================================================================
// Function : isSketching
// Purpose  : 
//================================================================
bool GraphicsView_ViewPort::isSketching( bool* theIsPath ) const
{
  if( theIsPath )
    *theIsPath = myIsSketchingByPath;
  return myIsSketching;
}

//================================================================
// Function : dragObjects
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::dragObjects( QGraphicsSceneMouseEvent* e )
{
  GraphicsView_Object* anObject = getHighlightedObject();

  if( myDragPosition.isNull() )
  {
    myDragPosition = e->scenePos();
    return;
  }

  GraphicsView_ObjectList anObjectsToMove;
  if( anObject && anObject->isMovable() && ( e->buttons() & Qt::LeftButton ) )
  {
    if( anObject->isSelected() )
    {
      for( initSelected(); moreSelected(); nextSelected() )
        if( GraphicsView_Object* aMovingObject = selectedObject() )
          if( aMovingObject->isMovable() )
            anObjectsToMove.append( aMovingObject );
    }
    else
      anObjectsToMove.append( anObject );
  }
  else if( hasInteractionFlag( DraggingByMiddleButton ) &&
           nbSelected() && ( e->buttons() & Qt::MidButton ) )
  {
    for( initSelected(); moreSelected(); nextSelected() )
      if( GraphicsView_Object* aMovingObject = selectedObject() )
        if( aMovingObject->isMovable() )
          anObjectsToMove.append( aMovingObject );
  }

  if( anObjectsToMove.isEmpty() )
    return;

  double aDX = e->scenePos().x() - myDragPosition.x();
  double aDY = e->scenePos().y() - myDragPosition.y();

  bool anIsMovingByXAllowed = true, anIsMovingByYAllowed = true;
  GraphicsView_ObjectListIterator anIter( anObjectsToMove );
  while( anIter.hasNext() )
    if( GraphicsView_Object* aMovingObject = anIter.next() )
    {
      if( !aMovingObject->isMovingByXAllowed( aDX ) )
        anIsMovingByXAllowed = false;
      if( !aMovingObject->isMovingByYAllowed( aDY ) )
        anIsMovingByYAllowed = false;
    }

  if( !anIsMovingByXAllowed && !anIsMovingByYAllowed )
    return; // myDragPosition shouldn't be changed

  if( !anIsMovingByXAllowed )
    aDX = 0;

  if( !anIsMovingByYAllowed )
    aDY = 0;

  anIter = anObjectsToMove;
  while( anIter.hasNext() )
    if( GraphicsView_Object* aMovingObject = anIter.next() )
      aMovingObject->move( aDX, aDY );

  if( anIsMovingByXAllowed )
    myDragPosition.setX( e->scenePos().x() );

  if( anIsMovingByYAllowed )
    myDragPosition.setY( e->scenePos().y() );
}

//================================================================
// Function : startPulling
// Purpose  : 
//================================================================
bool GraphicsView_ViewPort::startPulling( const QPointF& thePoint )
{
  QListIterator<QGraphicsItem*> anIter( items() );
  while( anIter.hasNext() )
  {
    if( GraphicsView_Object* anObject = dynamic_cast<GraphicsView_Object*>( anIter.next() ) )
    {
      QRectF aRect = anObject->getPullingRect();
      if( aRect.contains( thePoint ) && anObject->startPulling( thePoint ) )
      {
        myIsPulling = true;
        myPullingObject = anObject;
        //setCursor( *getHandCursor() ); // testing ImageViewer
        return true;
      }
    }
  }
  return false;
}

//================================================================
// Function : drawPulling
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::drawPulling( const QPointF& thePoint )
{
  GraphicsView_Object* aLockedObject = 0;

  QListIterator<QGraphicsItem*> anIter( items() );
  while( anIter.hasNext() )
  {
    if( GraphicsView_Object* anObject = dynamic_cast<GraphicsView_Object*>( anIter.next() ) )
    {
      if( !anObject->isVisible() )
        continue;

      QRectF aRect = anObject->getPullingRect();
      if( aRect.contains( thePoint ) && anObject->portContains( thePoint ) )
      {
        aLockedObject = anObject;
        break;
      }
    }
  }

  myPullingObject->pull( thePoint, aLockedObject, getSelectedObjects() );
}

//================================================================
// Function : finishPulling
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::finishPulling( bool theStatus )
{
  myIsPulling = false;
  myPullingObject->finishPulling( theStatus, getSelectedObjects() );
  setCursor( *getDefaultCursor() );
}

//================================================================
// Function : cancelCurrentOperation
// Purpose  : 
//================================================================
bool GraphicsView_ViewPort::cancelCurrentOperation()
{
  myIsHighlighting = false;

  if( isDragging() )
  {
    for( initSelected(); moreSelected(); nextSelected() )
      if( GraphicsView_Object* aMovingObject = selectedObject() )
        aMovingObject->finishMove( false );

    if( GraphicsView_Object* aMovingObject = getHighlightedObject() )
      aMovingObject->finishMove( false );

    myIsDragging = false;
    myDragPosition = QPointF();
    //setCursor( myStoredCursor );
    setCursor( *getDefaultCursor() );

    return true;
  }

  if( isPulling() )
  {
    finishPulling( false );
    return true;
  }

  if( isSketching() || isPrepareToSketch() )
  {
    finishSketching( false );
    return true;
  }

  return false;
}

//================================================================
// Function : onBoundingRectChanged
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::onBoundingRectChanged()
{
  if( hasInteractionFlag( TraceBoundingRect ) )
  {
    QRectF aRect = objectsBoundingRect( true );
    myScene->setSceneRect( aRect.adjusted( -mySceneGap, -mySceneGap, mySceneGap, mySceneGap ) );
  }
}

//================================================================
// Function : onKeyEvent
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::onKeyEvent( QKeyEvent* e )
{
  emit vpKeyEvent( e );
}

//================================================================
// Function : onMouseEvent
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::onMouseEvent( QGraphicsSceneMouseEvent* e )
{
  emit vpMouseEvent( e );

  bool anIsHandled = false;
  switch( e->type() )
  {
    case QEvent::GraphicsSceneMousePress:
    {
      if( hasInteractionFlag( EditFlags ) && nbSelected() )
        for( initSelected(); moreSelected() && !anIsHandled; nextSelected() )
          if( GraphicsView_Object* anObject = selectedObject() )
            anIsHandled = anObject->handleMousePress( e );

      if( !anIsHandled && hasInteractionFlag( Dragging ) )
      {
        bool anAccel = e->modifiers() & GraphicsView_ViewTransformer::accelKey();
        if( ( getHighlightedObject() &&
              getHighlightedObject()->isMovable() &&
              !( anAccel || e->button() != Qt::LeftButton ) ) ||
            ( hasInteractionFlag( DraggingByMiddleButton ) &&
              nbSelected() && !anAccel && e->button() == Qt::MidButton ) )
        {
          myIsDragging = true;
          myStoredCursor = cursor();
          setCursor( Qt::ClosedHandCursor );
        }
      }
      break;
    }
    case QEvent::GraphicsSceneMouseMove:
    {
      if( hasInteractionFlag( EditFlags ) && nbSelected() )
        for( initSelected(); moreSelected() && !anIsHandled; nextSelected() )
          if( GraphicsView_Object* anObject = selectedObject() )
            anIsHandled = anObject->handleMouseMove( e );

      if( !anIsHandled && !isPulling() && myIsDragging )
        dragObjects( e );
      break;
    }
    case QEvent::GraphicsSceneMouseRelease:
    {
      if( hasInteractionFlag( EditFlags ) && nbSelected() )
        for( initSelected(); moreSelected() && !anIsHandled; nextSelected() )
          if( GraphicsView_Object* anObject = selectedObject() )
            anIsHandled = anObject->handleMouseRelease( e );

      if( !anIsHandled && !isPulling() && myIsDragging )
      {
        emit vpObjectBeforeMoving();

        bool anIsMoved = false;
        for( initSelected(); moreSelected(); nextSelected() )
          if( GraphicsView_Object* aMovingObject = selectedObject() )
            anIsMoved = aMovingObject->finishMove( true ) || anIsMoved;

        if( GraphicsView_Object* aMovingObject = getHighlightedObject() )
          anIsMoved = aMovingObject->finishMove( true ) || anIsMoved;

        myIsDragging = false;
        myDragPosition = QPointF();
        setCursor( myStoredCursor );

        emit vpObjectAfterMoving( anIsMoved );
      }
      break;
    }
    case QEvent::GraphicsSceneMouseDoubleClick:
      break; // do nothing, just emit the signal
    default:
      break;
  }

  if( myIsMousePositionEnabled )
  {
    int aMouseX = (int)e->scenePos().x();
    int aMouseY = (int)e->scenePos().y();
    setViewLabelText( QString( "(%1, %2)" ).arg( aMouseX ).arg( aMouseY ) );
  }
}

//================================================================
// Function : onWheelEvent
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::onWheelEvent( QGraphicsSceneWheelEvent* e )
{
  emit vpWheelEvent( e );
}

//================================================================
// Function : onContextMenuEvent
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::onContextMenuEvent( QGraphicsSceneContextMenuEvent* e )
{
  emit vpContextMenuEvent( e );
}

//================================================================
// Function : scrollContentsBy
// Purpose  : 
//================================================================
void GraphicsView_ViewPort::scrollContentsBy( int theDX, int theDY )
{
  if( myViewLabel )
    myViewLabel->setAcceptMoveEvents( false );

  QGraphicsView::scrollContentsBy( theDX, theDY );

  if( myViewLabel )
    myViewLabel->setAcceptMoveEvents( true );
}
