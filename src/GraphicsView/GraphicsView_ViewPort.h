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

#ifndef GRAPHICSVIEW_VIEWPORT_H
#define GRAPHICSVIEW_VIEWPORT_H

#include "GraphicsView.h"

#include "GraphicsView_Defs.h"

#include <QGraphicsView>

class QGridLayout;
class QRubberBand;

class GraphicsView_Object;
class GraphicsView_Scene;

/*
  Class       : GraphicsView_ViewPort
  Description : View port of the graphics view
*/
class GRAPHICSVIEW_API GraphicsView_ViewPort : public QGraphicsView
{
  Q_OBJECT

public:
  class ViewLabel;

  enum InteractionFlag
  {
    // main flags
    Dragging               = 0x0001,
    Pulling                = 0x0002,
    WheelScaling           = 0x0004,
    EditFlags              = Dragging | Pulling | WheelScaling,
    // advanced flags
    TraceBoundingRect      = 0x0008,
    DraggingByMiddleButton = 0x0010,
    ImmediateContextMenu   = 0x0020,
    ImmediateSelection     = 0x0040,
    Sketching              = 0x0080
  };
  Q_DECLARE_FLAGS( InteractionFlags, InteractionFlag )

  enum BlockStatus
  {
    BS_NoBlock   = 0x0000,
    BS_Selection = 0x0001, // and highlighting
    BS_Dragging  = 0x0002  // currently unused
  };

  enum ViewLabelPosition
  {
    VLP_None         = 0,
    VLP_TopLeft      = 1,
    VLP_TopRight     = 2,
    VLP_BottomLeft   = 3,
    VLP_BottomRight  = 4
  };

public:
  GraphicsView_ViewPort( QWidget* theParent );
  ~GraphicsView_ViewPort();

public:
  GraphicsView_Scene*              getScene() { return myScene; }
  void                             addItem( QGraphicsItem* theItem );
  bool                             isItemAdded( QGraphicsItem* theItem );
  void                             removeItem( QGraphicsItem* theItem );

  enum SortType { NoSorting, SelectedFirst, SortByZLevel };
  GraphicsView_ObjectList          getObjects( SortType theSortType = NoSorting ) const;

  QRectF                           objectsBoundingRect( bool theOnlyVisible = false ) const;

  QImage                           dumpView( bool theWholeScene = false,
                                             QSizeF theSize = QSizeF() );

public:
  // scene
  void                             setSceneGap( double theSceneGap );
  void                             setFitAllGap( double theFitAllGap );

  // interaction flags
  int                              interactionFlags() const;
  bool                             hasInteractionFlag( InteractionFlag theFlag );
  void                             setInteractionFlag( InteractionFlag theFlag,
                                                       bool theIsEnabled = true );
  void                             setInteractionFlags( InteractionFlags theFlags );

  // view label
  void                             setViewLabelPosition( ViewLabelPosition thePosition,
                                                         bool theIsForced = false );
  void                             setViewLabelText( const QString& theText );

  // displaying mouse position (currently, overlaps with view label feature)
  void                             setMousePositionEnabled( bool theState );

  // background / foreground
  QColor                           backgroundColor() const;
  void                             setBackgroundColor( const QColor& theColor );

  bool                             isForegroundEnabled() const { return myIsForegroundEnabled; }
  void                             setForegroundEnabled( bool theState );

  QSizeF                           foregroundSize() const { return myForegroundSize; }
  void                             setForegroundSize( const QSizeF& theRect );

  double                           foregroundMargin() const { return myForegroundMargin; }
  void                             setForegroundMargin( double theMargin );

  QColor                           foregroundColor() const { return myForegroundColor; }
  void                             setForegroundColor( const QColor& theColor );

  QColor                           foregroundFrameColor() const { return myForegroundFrameColor; }
  void                             setForegroundFrameColor( const QColor& theColor );

  double                           foregroundFrameLineWidth() const { return myForegroundFrameLineWidth; }
  void                             setForegroundFrameLineWidth( double theLineWidth );

  void                             updateForeground();

  // grid
  void                             setGridEnabled( bool theState );
  void                             setGridCellSize( int theCellSize );
  void                             setGridLineStyle( int theLineStyle );
  void                             setGridLineColor( const QColor& theLineColor );
  void                             updateGrid();

  // transformation
  void                             reset();
  void                             pan( double theDX, double theDY );
  void                             setCenter( double theX, double theY );
  void                             zoom( double theX1, double theY1, double theX2, double theY2 );
  void                             fitRect( const QRectF& theRect );
  void                             fitSelect();
  void                             fitAll( bool theKeepScale = false );
  void                             fitWidth();

  bool                             isTransforming() const { return myIsTransforming; }

  void                             applyTransform();

  // block status
  BlockStatus                      currentBlock();

  // highlighting
  virtual void                     highlight( double theX, double theY );
  void                             clearHighlighted();

  GraphicsView_Object*             getHighlightedObject() const { return myHighlightedObject; }

  // selection
  virtual int                      select( const QRectF& theRect, bool theIsAppend );
  void                             clearSelected();
  void                             setSelected( GraphicsView_Object* theObject );

  int                              nbSelected() const;
  void                             initSelected();
  bool                             moreSelected();
  bool                             nextSelected();
  GraphicsView_Object*             selectedObject();

  const GraphicsView_ObjectList&   getSelectedObjects() const { return mySelectedObjects; }

  // rectangle selection
  void                             startSelectByRect( int x, int y );
  void                             drawSelectByRect( int x, int y );
  void                             finishSelectByRect();
  bool                             isSelectByRect() const;
  QRect                            selectionRect();

  // sketching
  void                             prepareToSketch( bool theStatus );
  bool                             isPrepareToSketch();
  void                             startSketching( const QPointF& thePoint,
                                                   bool theIsPath );
  void                             drawSketching( const QPointF& thePoint );
  void                             finishSketching( bool theStatus );
  bool                             isSketching( bool* theIsPath = 0 ) const;

  // dragging
  bool                             isDragging() { return myIsDragging; }

  // pulling
  bool                             startPulling( const QPointF& );
  void                             drawPulling( const QPointF& );
  void                             finishPulling( bool theStatus );
  bool                             isPulling() const { return myIsPulling; }

  // other
  bool                             cancelCurrentOperation();

public:
  static void                      createCursors();
  static void                      destroyCursors();
  static QCursor*                  getDefaultCursor() { return defCursor; }
  static QCursor*                  getHandCursor() { return handCursor; }
  static QCursor*                  getPanCursor() { return panCursor; }
  static QCursor*                  getPanglCursor() { return panglCursor; }
  static QCursor*                  getZoomCursor() { return zoomCursor; }
  static QCursor*                  getSketchCursor() { return sketchCursor; }

public slots:
  void                             onBoundingRectChanged();

protected slots:
  void                             onKeyEvent( QKeyEvent* );
  void                             onMouseEvent( QGraphicsSceneMouseEvent* );
  void                             onWheelEvent( QGraphicsSceneWheelEvent* );
  void                             onContextMenuEvent( QGraphicsSceneContextMenuEvent* );

protected:
  virtual void                     scrollContentsBy( int theDX, int theDY );

signals:
  void                             vpKeyEvent( QKeyEvent* );
  void                             vpMouseEvent( QGraphicsSceneMouseEvent* );
  void                             vpWheelEvent( QGraphicsSceneWheelEvent* );
  void                             vpContextMenuEvent( QGraphicsSceneContextMenuEvent* );

  void                             vpSketchingFinished( QPainterPath );

  void                             vpObjectBeforeMoving();
  void                             vpObjectAfterMoving( bool );

private:
  void                             initialize();
  void                             cleanup();

  void                             dragObjects( QGraphicsSceneMouseEvent* );

private:
  static int                       nCounter;
  static QCursor*                  defCursor;
  static QCursor*                  handCursor;
  static QCursor*                  panCursor;
  static QCursor*                  panglCursor;
  static QCursor*                  zoomCursor;
  static QCursor*                  sketchCursor;

private:
  // scene
  GraphicsView_Scene*              myScene;
  double                           mySceneGap;
  double                           myFitAllGap;
  GraphicsView_ObjectList          myObjects;

  // interaction flags
  InteractionFlags                 myInteractionFlags;

  // view label
  ViewLabel*                       myViewLabel;
  ViewLabelPosition                myViewLabelPosition;
  QGridLayout*                     myViewLabelLayout;

  // displaying mouse position (currently, overlaps with view label feature)
  bool                             myIsMousePositionEnabled;

  // foreground
  bool                             myIsForegroundEnabled;
  QSizeF                           myForegroundSize;
  double                           myForegroundMargin;
  QColor                           myForegroundColor;
  QColor                           myForegroundFrameColor;
  double                           myForegroundFrameLineWidth;
  QGraphicsRectItem*               myForegroundItem;

  // grid
  bool                             myIsGridEnabled;
  int                              myGridCellSize;
  int                              myGridLineStyle;
  QColor                           myGridLineColor;
  QGraphicsPathItem*               myGridItem;

  // transformation
  bool                             myIsTransforming;
  QTransform                       myCurrentTransform;

  // highlighting
  GraphicsView_Object*             myHighlightedObject;
  double                           myHighlightX;
  double                           myHighlightY;
  bool                             myIsHighlighting;

  // selection
  GraphicsView_ObjectList          mySelectedObjects;
  int                              mySelectionIterator;

  // rectangle selection
  QRubberBand*                     myRectBand;
  QPoint                           myFirstSelectionPoint;
  QPoint                           myLastSelectionPoint;
  bool                             myAreSelectionPointsInitialized;

  // sketching
  QGraphicsPathItem*               mySketchingItem;
  QPointF                          mySketchingPoint;
  bool                             myIsPrepareToSketch;
  bool                             myIsSketching;
  bool                             myIsSketchingByPath;

  // dragging
  int                              myIsDragging;
  QPointF                          myDragPosition;
  bool                             myIsDragPositionInitialized;

  // pulling
  bool                             myIsPulling;
  GraphicsView_Object*             myPullingObject;

  // cursor
  QCursor                          myStoredCursor;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( GraphicsView_ViewPort::InteractionFlags )

#endif
