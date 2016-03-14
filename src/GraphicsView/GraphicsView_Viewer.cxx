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

#include "GraphicsView_Viewer.h"

#include "GraphicsView_Object.h"
#include "GraphicsView_Selector.h"
#include "GraphicsView_Scene.h"
#include "GraphicsView_ViewFrame.h"
#include "GraphicsView_ViewPort.h"
#include "GraphicsView_ViewTransformer.h"

#include <SUIT_ViewManager.h>

#include <ImageComposer_Image.h>

#include <QApplication>
#include <QColorDialog>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QMenu>

// testing ImageViewer
/*
#include "GraphicsView_PrsImage.h"
#include "GraphicsView_PrsPropDlg.h"
#include <QFileDialog>
*/

//=======================================================================
// Name    : GraphicsView_Viewer
// Purpose : Constructor
//=======================================================================
GraphicsView_Viewer::GraphicsView_Viewer( const QString& title, QWidget* widget )
: SUIT_ViewModel(),
  mySelector( 0 ),
  myTransformer( 0 ),
  myWidget( widget ),
  myIsInitialized( false )
{
}

//=======================================================================
// Name    : GraphicsView_Viewer
// Purpose : Destructor
//=======================================================================
GraphicsView_Viewer::~GraphicsView_Viewer()
{
  delete mySelector;
}

//================================================================
// Function : createView
// Purpose  : 
//================================================================
SUIT_ViewWindow* GraphicsView_Viewer::createView( SUIT_Desktop* theDesktop )
{
  GraphicsView_ViewFrame* aViewFrame = new GraphicsView_ViewFrame( theDesktop, this, myWidget );

  connect( aViewFrame, SIGNAL( keyPressed( QKeyEvent* ) ),
           this, SLOT( onKeyEvent( QKeyEvent* ) ) );

  connect( aViewFrame, SIGNAL( keyReleased( QKeyEvent* ) ),
           this, SLOT( onKeyEvent( QKeyEvent* ) ) );

  connect( aViewFrame, SIGNAL( mousePressed( QGraphicsSceneMouseEvent* ) ),
           this, SLOT( onMouseEvent( QGraphicsSceneMouseEvent* ) ) );

  connect( aViewFrame, SIGNAL( mouseMoving( QGraphicsSceneMouseEvent* ) ),
           this, SLOT( onMouseEvent( QGraphicsSceneMouseEvent* ) ) );

  connect( aViewFrame, SIGNAL( mouseReleased( QGraphicsSceneMouseEvent* ) ),
           this, SLOT( onMouseEvent( QGraphicsSceneMouseEvent* ) ) );

  connect( aViewFrame, SIGNAL( wheeling( QGraphicsSceneWheelEvent* ) ),
           this, SLOT( onWheelEvent( QGraphicsSceneWheelEvent* ) ) );

  connect( aViewFrame, SIGNAL( sketchingFinished( QPainterPath ) ),
           this, SLOT( onSketchingFinished( QPainterPath ) ) );

  return aViewFrame;
}

//================================================================
// Function : contextMenuPopup
// Purpose  : 
//================================================================
void GraphicsView_Viewer::contextMenuPopup( QMenu* thePopup )
{
  if( thePopup->actions().count() > 0 )
    thePopup->addSeparator();

  // testing ImageViewer
  /*
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    int aNbSelected = aViewPort->nbSelected();
    if( aNbSelected == 0 )
    {
      thePopup->addAction( tr( "ADD_IMAGE" ), this, SLOT( onAddImage() ) );
      thePopup->addSeparator();

      thePopup->addAction( tr( "TEST_FUSE_OPERATOR" ), this, SLOT( onTestFuseOperator() ) );
      thePopup->addAction( tr( "TEST_CROP_OPERATOR" ), this, SLOT( onTestCropOperatorPrepare() ) );
    }
    else
    {
      thePopup->addAction( tr( "BRING_TO_FRONT" ), this, SLOT( onBringToFront() ) );
      thePopup->addAction( tr( "SEND_TO_BACK" ), this, SLOT( onSendToBack() ) );
      thePopup->addAction( tr( "BRING_FORWARD" ), this, SLOT( onBringForward() ) );
      thePopup->addAction( tr( "SEND_BACKWARD" ), this, SLOT( onSendBackward() ) );
      thePopup->addSeparator();

      if( aNbSelected == 1 )
      {
        thePopup->addAction( tr( "PROPERTIES" ), this, SLOT( onPrsProperties() ) );
        thePopup->addSeparator();
      }

      thePopup->addAction( tr( "REMOVE_IMAGES" ), this, SLOT( onRemoveImages() ) );
    }
    thePopup->addSeparator();
  }
  */

  thePopup->addAction( tr( "CHANGE_BGCOLOR" ), this, SLOT( onChangeBgColor() ) );
}

//================================================================
// Function : getSelector
// Purpose  : 
//================================================================
GraphicsView_Selector* GraphicsView_Viewer::getSelector()
{
  if( !mySelector )
  {
    mySelector = new GraphicsView_Selector( this );
    if( mySelector )
    {
      connect( mySelector, SIGNAL( selSelectionDone( GV_SelectionChangeStatus ) ),
               this, SLOT( onSelectionDone( GV_SelectionChangeStatus ) ) );
      connect( mySelector, SIGNAL( selSelectionCancel() ),
               this, SLOT( onSelectionCancel() ) );
    }
  }
  return mySelector;
}

//================================================================
// Function : getActiveView
// Purpose  : 
//================================================================
GraphicsView_ViewFrame* GraphicsView_Viewer::getActiveView() const
{
  if( SUIT_ViewManager* aViewManager = getViewManager() )
    return dynamic_cast<GraphicsView_ViewFrame*>( aViewManager->getActiveView() );
  return NULL;
}

//================================================================
// Function : getActiveViewPort
// Purpose  : 
//================================================================
GraphicsView_ViewPort* GraphicsView_Viewer::getActiveViewPort() const
{
  if( GraphicsView_ViewFrame* aViewFrame = getActiveView() )
    return aViewFrame->getViewPort();
  return NULL;
}

//================================================================
// Function : getActiveScene
// Purpose  : 
//================================================================
GraphicsView_Scene* GraphicsView_Viewer::getActiveScene() const
{
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
    return dynamic_cast<GraphicsView_Scene*>( aViewPort->scene() );
  return NULL;
}

//================================================================
// Function : activateTransform
// Purpose  : 
//================================================================
void GraphicsView_Viewer::activateTransform( int theType )
{
  if( theType == NoTransform ) // finish current transform
  {
    if ( myTransformer )
    {
      onTransformationFinished();
      delete myTransformer;
      myTransformer = 0;
    }
  }
  else // activate new transform
  {
    activateTransform( NoTransform );
    myTransformer = createTransformer( theType );
    onTransformationStarted();
    myTransformer->exec();
  }
}

//================================================================
// Function : setIsInitialized
// Purpose  : 
//================================================================
void GraphicsView_Viewer::setIsInitialized( bool theFlag )
{
  myIsInitialized = theFlag;
}

//================================================================
// Function : createTransformer
// Purpose  : 
//================================================================
GraphicsView_ViewTransformer* GraphicsView_Viewer::createTransformer( int theType )
{
  return new GraphicsView_ViewTransformer( this, theType );
}

//================================================================
// Function : onTransformationStarted
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onTransformationStarted()
{
  if( GraphicsView_Selector* aSelector = getSelector() )
  {
    aSelector->undetectAll();
    aSelector->lock( true ); // disable selection
  }

  // watch events: any mouse/key event outside the
  // viewport will be considered as the end of transform
  if( myTransformer )
    qApp->installEventFilter( this );
}

//================================================================
// Function : onTransformationFinished
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onTransformationFinished()
{
  if( GraphicsView_Selector* aSelector = getSelector() )
    aSelector->lock( false ); // enable selection

  // stop watching events
  if( myTransformer )
    qApp->removeEventFilter( this );
}

//================================================================
// Function : onKeyEvent
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onKeyEvent( QKeyEvent* e )
{
  switch( e->type() )
  {
    case QEvent::KeyPress:
      handleKeyPress( e );
      break;
    case QEvent::KeyRelease:
      handleKeyRelease( e );
      break;
    default: break;
  }
}

//================================================================
// Function : onMouseEvent
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onMouseEvent( QGraphicsSceneMouseEvent* e )
{
  switch( e->type() )
  {
    case QEvent::GraphicsSceneMousePress:
      handleMousePress( e );
      break;
    case QEvent::GraphicsSceneMouseMove:
      handleMouseMove( e );
      break;
    case QEvent::GraphicsSceneMouseRelease:
      handleMouseRelease( e );
      break;
    default: break;
  }
}

//================================================================
// Function : onWheelEvent
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onWheelEvent( QGraphicsSceneWheelEvent* e )
{
  switch( e->type() )
  {
    case QEvent::GraphicsSceneWheel:
      handleWheel( e );
      break;
    default: break;
  }
}

//================================================================
// Function : handleKeyPress
// Purpose  : 
//================================================================
void GraphicsView_Viewer::handleKeyPress( QKeyEvent* e )
{
  if( e->key() == Qt::Key_Escape )
  {
    // Cancel current operation
    bool anIsCancelled = false;
    if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
    {
      anIsCancelled = aViewPort->cancelCurrentOperation();

      // Unselect all objects (if there is no operation to cancel)
      if( !anIsCancelled )
      {
        aViewPort->finishSelectByRect();
        aViewPort->clearSelected();
      }
    }

    // Emit unselection signal
    if( !anIsCancelled )
      if( GraphicsView_Selector* aSelector = getSelector() )
        aSelector->unselectAll();
  }
}

//================================================================
// Function : handleKeyRelease
// Purpose  : 
//================================================================
void GraphicsView_Viewer::handleKeyRelease( QKeyEvent* e )
{
}

//================================================================
// Function : handleMousePress
// Purpose  : 
//================================================================
void GraphicsView_Viewer::handleMousePress( QGraphicsSceneMouseEvent* e )
{
  // test accel for transforms
  if ( e->modifiers() & GraphicsView_ViewTransformer::accelKey() )
  {
    Qt::MouseButton bs = e->button();
    if ( bs == GraphicsView_ViewTransformer::zoomButton() )
      activateTransform( Zoom );
    else if ( bs == GraphicsView_ViewTransformer::panButton() )
      activateTransform( Pan );
  }
  else // checking for other operations before selection in release event
  {
    if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
    {
      bool append = bool ( e->modifiers() & GraphicsView_Selector::getAppendKey() );
      if( e->button() == Qt::LeftButton &&
          aViewPort->hasInteractionFlag( GraphicsView_ViewPort::Sketching ) &&
          aViewPort->isPrepareToSketch() )
      {
        // Use 'append' flag for sketching by arbitrary path
        aViewPort->startSketching( e->scenePos(), append );
      }
      else if( e->button() == Qt::LeftButton &&
               aViewPort->hasInteractionFlag( GraphicsView_ViewPort::Pulling ) &&
               !aViewPort->isSelectByRect() && 
               !aViewPort->isDragging() &&
               aViewPort->startPulling( e->scenePos() ) )
      {
        // Try to start pulling if rectangular selection is performed
        aViewPort->finishSelectByRect();
      }
      else if( e->button() == Qt::LeftButton &&
               !( aViewPort->currentBlock() & GraphicsView_ViewPort::BS_Selection ) &&
               !aViewPort->getHighlightedObject() )
      {
        // Start rectangular selection if pulling was not started
        QPoint p = aViewPort->mapFromScene( e->scenePos() );
        aViewPort->startSelectByRect( p.x(), p.y() );
      }
      else if( e->button() != Qt::MidButton && !append &&
               aViewPort->hasInteractionFlag( GraphicsView_ViewPort::ImmediateSelection ) &&
               aViewPort->nbSelected() < 2 )
      {
        // Do not perform 'immediate selection' if the multiple objects are already selected
        getSelector()->select( QRectF(), append );
      }
      else if( e->button() == Qt::RightButton &&
               aViewPort->hasInteractionFlag( GraphicsView_ViewPort::ImmediateContextMenu ) &&
               aViewPort->nbSelected() < 1 )
      {
        // If the 'immediate context menu' mode is enabled,
        // try to perform selection before invoking context menu
        getSelector()->select( QRectF(), append );
      }
    }
  }
}

//================================================================
// Function : handleMouseMove
// Purpose  : 
//================================================================
void GraphicsView_Viewer::handleMouseMove( QGraphicsSceneMouseEvent* e )
{
  GraphicsView_ViewPort* aViewPort = getActiveViewPort();

  // highlight for selection
  bool anIsDragged = ( e->buttons() & ( Qt::LeftButton | Qt::MidButton | Qt::RightButton ) );
  bool anIsPrepareToSketch = aViewPort && aViewPort->isPrepareToSketch();
  if ( !anIsDragged && !anIsPrepareToSketch )
  {
    if ( getSelector() )
      getSelector()->detect( e->scenePos().x(), e->scenePos().y() );
  }

  // try to activate other operations
  if( aViewPort )
  {
    if( aViewPort->isPulling() )
    {
      aViewPort->drawPulling( e->scenePos() );
    }
    else if( aViewPort->isSketching() )
    {
      aViewPort->drawSketching( e->scenePos() );
    }
    else if( e->button() == Qt::LeftButton &&
             aViewPort->hasInteractionFlag( GraphicsView_ViewPort::Pulling ) &&
             !aViewPort->isSelectByRect() &&
             !aViewPort->isDragging() &&
             aViewPort->startPulling( e->scenePos() ) )
    {
      aViewPort->finishSelectByRect();
    }
    else if( !aViewPort->getHighlightedObject() )
    {
      QPoint p = aViewPort->mapFromScene( e->scenePos() );
      aViewPort->drawSelectByRect( p.x(), p.y() );
    }
  }
}

//================================================================
// Function : handleMouseRelease
// Purpose  : 
//================================================================
void GraphicsView_Viewer::handleMouseRelease( QGraphicsSceneMouseEvent* e )
{
  // selection
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    if( e->button() == Qt::LeftButton &&
        !( aViewPort->currentBlock() & GraphicsView_ViewPort::BS_Selection ) )
    {
      if ( getSelector() )
      {
        bool append = bool ( e->modifiers() & GraphicsView_Selector::getAppendKey() );
        getSelector()->select( QRectF(), append );
      }
    }
  }

  // try to finish active operations
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    if( aViewPort->isPulling() )
    {
      aViewPort->finishPulling( true );
    }
    else if( aViewPort->isSketching() )
    {
      aViewPort->finishSketching( true );
    }
    else if( !aViewPort->getHighlightedObject() )
    {
      QRect aSelRect = aViewPort->selectionRect();
      aViewPort->finishSelectByRect();
      if ( getSelector() && !aSelRect.isNull() )
      {            
        bool append = bool ( e->modifiers() & GraphicsView_Selector::getAppendKey() );
        QRectF aRect = aViewPort->mapToScene( aSelRect ).boundingRect();
        getSelector()->select( aRect, append );
      }
    }
  }
}

//================================================================
// Function : handleWheel
// Purpose  : 
//================================================================
void GraphicsView_Viewer::handleWheel( QGraphicsSceneWheelEvent* e )
{
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    if( aViewPort->hasInteractionFlag( GraphicsView_ViewPort::WheelScaling ) )
    {
      bool anIsScaleUp = e->delta() > 0;
      bool anIsCtrl = e->modifiers() & Qt::ControlModifier;

      bool anIsScaleChanged = false;
      for( aViewPort->initSelected(); aViewPort->moreSelected(); aViewPort->nextSelected() )
        if( GraphicsView_Object* anObject = aViewPort->selectedObject() )
          anIsScaleChanged = anObject->updateScale( anIsScaleUp, anIsCtrl ) || anIsScaleChanged;

      if( anIsScaleChanged )
      {
        emit wheelScaleChanged();
        aViewPort->onBoundingRectChanged();
      }
    }
  }
}

//================================================================
// Function : onSketchingFinished
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onSketchingFinished( QPainterPath thePath )
{
  // testing ImageViewer
  //onTestCropOperatorPerform( thePath );
}

//================================================================
// Function : onSelectionDone
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onSelectionDone( GV_SelectionChangeStatus theStatus )
{
  emit selectionChanged( theStatus );
}

//================================================================
// Function : onChangeBgColor
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onChangeBgColor()
{
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    QColor aColor = aViewPort->isForegroundEnabled() ?
      aViewPort->foregroundColor() : aViewPort->backgroundColor();
    aColor = QColorDialog::getColor( aColor, aViewPort );	
    if ( aColor.isValid() )
    {
      if( aViewPort->isForegroundEnabled() )
      {
        aViewPort->setForegroundColor( aColor );
        aViewPort->updateForeground();
      }
      else
        aViewPort->setBackgroundColor( aColor );
    }
  }
}

//================================================================
// Function : onSelectionCancel
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onSelectionCancel()
{
  emit selectionChanged( GVSCS_Invalid );
}

/*
//================================================================
// Function : onAddImage
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onAddImage()
{
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    QString aFileName = QFileDialog::getOpenFileName();
    if( aFileName.isEmpty() )
      return;

    GraphicsView_PrsImage* aPrs = new GraphicsView_PrsImage();

    QImage anImage( aFileName );
    aPrs->setImage( anImage );

    aPrs->compute();

    aViewPort->addItem( aPrs );
    aViewPort->fitAll();
  }
}

//================================================================
// Function : onRemoveImages
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onRemoveImages()
{
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    GraphicsView_ObjectListIterator anIter( aViewPort->getSelectedObjects() );
    while( anIter.hasNext() )
    {
      if( GraphicsView_PrsImage* aPrs = dynamic_cast<GraphicsView_PrsImage*>( anIter.next() ) )
      {
        aViewPort->removeItem( aPrs );
        delete aPrs;
      }
    }
  }
}

//================================================================
// Function : onBringToFront
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onBringToFront()
{
  processQueueOperation( BringToFront );
}

//================================================================
// Function : onSendToBack
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onSendToBack()
{
  processQueueOperation( SendToBack );
}

//================================================================
// Function : onBringForward
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onBringForward()
{
  processQueueOperation( BringForward );
}

//================================================================
// Function : onSendBackward
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onSendBackward()
{
  processQueueOperation( SendBackward );
}

//================================================================
// Function : processQueueOperation
// Purpose  : 
//================================================================
void GraphicsView_Viewer::processQueueOperation( const QueueOperation theOperation )
{
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    const GraphicsView_ObjectList& aSelectedList = aViewPort->getSelectedObjects();

    GraphicsView_ObjectList aSortedList;

    GraphicsView_ObjectList aList = aViewPort->getObjects();
    GraphicsView_ObjectListIterator anIter( aList );
    while( anIter.hasNext() )
    {
      if( GraphicsView_Object* anObject = anIter.next() )
      {
        if( !anObject->hasSpecificZValue() )
        {
          double aZValue = anObject->zValue();
          GraphicsView_ObjectList::iterator anIter1, anIter1End = aSortedList.end();
          for( anIter1 = aSortedList.begin(); anIter1 != anIter1End; anIter1++ )
            if( GraphicsView_Object* anObjectRef = *anIter1 )
              if( !anObjectRef->hasSpecificZValue() && anObjectRef->zValue() > aZValue )
                break;
          aSortedList.insert( anIter1, anObject );
        }
      }
    }

    QList<int> anIndicesToMove;

    int anIndex = 0;
    anIter = aSortedList;
    while( anIter.hasNext() )
    {
      if( GraphicsView_Object* anObject = anIter.next() )
        if( aSelectedList.contains( anObject ) )
          anIndicesToMove.append( anIndex );
      anIndex++;
    }

    bool anIsReverse = theOperation == BringToFront || theOperation == BringForward;
    QListIterator<int> anIndicesIter( anIndicesToMove );
    if( anIsReverse )
      anIndicesIter.toBack();

    int aShiftForMultiple = 0;
    int anObjectCount = aSortedList.count();
    while( anIsReverse ? anIndicesIter.hasPrevious() : anIndicesIter.hasNext() )
    {
      int anIndex = anIsReverse ? anIndicesIter.previous() : anIndicesIter.next();
      int aNewIndex = anIndex;
      switch( theOperation )
      {
        case BringToFront: aNewIndex = anObjectCount - 1 - aShiftForMultiple; break;
        case SendToBack:   aNewIndex = aShiftForMultiple; break;
        case BringForward: aNewIndex = anIndex + 1; break;
        case SendBackward: aNewIndex = anIndex - 1; break;
      }
      aShiftForMultiple++;

      if( aNewIndex < 0 || aNewIndex > anObjectCount - 1 )
        break;

      aSortedList.move( anIndex, aNewIndex );
    }

    double aZValue = 1.0;
    anIter = aSortedList;
    while( anIter.hasNext() )
    {
      if( GraphicsView_Object* anObject = anIter.next() )
      {
        anObject->setZValue( aZValue );
        aZValue += 1.0;
      }
    }
  }
}

//================================================================
// Function : onPrsProperties
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onPrsProperties()
{
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    aViewPort->initSelected();
    if( GraphicsView_Object* anObject = aViewPort->selectedObject() )
    {
      if( GraphicsView_PrsImage* aPrs = dynamic_cast<GraphicsView_PrsImage*>( anObject ) )
      {
        double aPosX, aPosY, aScaleX, aScaleY, aRotationAngle;
        aPrs->getPosition( aPosX, aPosY );
        aPrs->getScaling( aScaleX, aScaleY );
        aPrs->getRotationAngle( aRotationAngle );

        double aZValue = aPrs->zValue();
        double anOpacity = aPrs->opacity();

        bool anIsLockAspectRatio = aPrs->getIsLockAspectRatio();
        bool anIsSmoothTransformation = aPrs->getIsSmoothTransformation();

        GraphicsView_PrsPropDlg aDlg( aViewPort );
        aDlg.setData( aPosX, aPosY, aScaleX, aScaleY, aRotationAngle,
                      aZValue, anOpacity, anIsLockAspectRatio,
                      anIsSmoothTransformation );
        if( aDlg.exec() )
        {
          aDlg.getData( aPosX, aPosY, aScaleX, aScaleY, aRotationAngle,
                        aZValue, anOpacity, anIsLockAspectRatio,
                        anIsSmoothTransformation );

          aPrs->setPosition( aPosX, aPosY );
          aPrs->setScaling( aScaleX, aScaleY );
          aPrs->setRotationAngle( aRotationAngle );

          aPrs->setZValue( aZValue );
          aPrs->setOpacity( anOpacity );

          aPrs->setIsLockAspectRatio( anIsLockAspectRatio );
          aPrs->setIsSmoothTransformation( anIsSmoothTransformation );

          aPrs->compute();
        }
      }
    }
  }
}

//================================================================
// Function : onTestFuseOperator
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onTestFuseOperator()
{
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    GraphicsView_ObjectList aList = aViewPort->getObjects();
    if( aList.count() < 3 )
      return;

    GraphicsView_PrsImage* anObj1 = dynamic_cast<GraphicsView_PrsImage*>( aList[0] );
    GraphicsView_PrsImage* anObj2 = dynamic_cast<GraphicsView_PrsImage*>( aList[2] );

    ImageComposer_Image anImage1;
    anImage1 = anObj1->getImage();
    anImage1.setTransform( anObj1->getTransform() );

    ImageComposer_Image anImage2;
    anImage2 = anObj2->getImage();
    anImage2.setTransform( anObj2->getTransform() );

    ImageComposer_Image aResult = anImage1 | anImage2;
    GraphicsView_PrsImage* aResPrs = new GraphicsView_PrsImage();
    aResPrs->setImage( aResult );

    double aPosX, aPosY, aScaleX, aScaleY, aRotationAngle;
    anObj1->getPosition( aPosX, aPosY );
    anObj1->getScaling( aScaleX, aScaleY );
    anObj1->getRotationAngle( aRotationAngle );

    aResPrs->setPosition( aResult.transform().dx(), aResult.transform().dy() );
    aResPrs->setScaling( aScaleX, aScaleY );
    aResPrs->setRotationAngle( aRotationAngle );

    aResPrs->compute();

    aViewPort->addItem( aResPrs );
    aViewPort->removeItem( anObj1 );
    aViewPort->removeItem( anObj2 );
  }
}

//================================================================
// Function : onTestCropOperatorPrepare
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onTestCropOperatorPrepare()
{
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
    aViewPort->prepareToSketch( true );
}

//================================================================
// Function : onTestCropOperatorPerform
// Purpose  : 
//================================================================
void GraphicsView_Viewer::onTestCropOperatorPerform( QPainterPath thePath )
{
  if( GraphicsView_ViewPort* aViewPort = getActiveViewPort() )
  {
    GraphicsView_ObjectList aList = aViewPort->getObjects();
    if( aList.count() < 1 )
      return;

    GraphicsView_PrsImage* anObj = dynamic_cast<GraphicsView_PrsImage*>( aList[0] );

    ImageComposer_Image anImage;
    anImage = anObj->getImage();
    anImage.setTransform( anObj->getTransform() );

    ImageComposer_Image aResult = anImage & thePath;
    GraphicsView_PrsImage* aResPrs = new GraphicsView_PrsImage();
    aResPrs->setImage( aResult );

    double aPosX, aPosY, aScaleX, aScaleY, aRotationAngle;
    anObj->getPosition( aPosX, aPosY );
    anObj->getScaling( aScaleX, aScaleY );
    anObj->getRotationAngle( aRotationAngle );

    aResPrs->setPosition( aResult.transform().dx(), aResult.transform().dy() );
    aResPrs->setScaling( aScaleX, aScaleY );
    aResPrs->setRotationAngle( aRotationAngle );

    aResPrs->compute();

    aViewPort->addItem( aResPrs );
    aViewPort->removeItem( anObj );
  }
}
*/
