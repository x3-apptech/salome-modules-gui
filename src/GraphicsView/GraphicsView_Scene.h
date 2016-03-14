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

#ifndef GRAPHICSVIEW_SCENE_H
#define GRAPHICSVIEW_SCENE_H

#include "GraphicsView.h"

#include <QGraphicsScene>

class QGraphicsRectItem;

/*
  Class       : GraphicsView_Scene
  Description : Scene of the graphics view
*/
class GRAPHICSVIEW_API GraphicsView_Scene : public QGraphicsScene
{
  Q_OBJECT

public:
  GraphicsView_Scene( QObject* theParent = 0 );
  ~GraphicsView_Scene();

public:
  void                       processRectChanged();

protected slots:
  void                       onSceneRectChanged( const QRectF& theRect ); // for debug

protected:
  virtual void               keyPressEvent( QKeyEvent* );
  virtual void               keyReleaseEvent( QKeyEvent* );
  virtual void               mousePressEvent( QGraphicsSceneMouseEvent* );
  virtual void               mouseMoveEvent( QGraphicsSceneMouseEvent* );
  virtual void               mouseReleaseEvent( QGraphicsSceneMouseEvent* );
  virtual void 	             mouseDoubleClickEvent( QGraphicsSceneMouseEvent* );
  virtual void               wheelEvent( QGraphicsSceneWheelEvent* );
  virtual void               contextMenuEvent( QGraphicsSceneContextMenuEvent* );

  virtual void               dragEnterEvent( QGraphicsSceneDragDropEvent* );
  virtual void               dragLeaveEvent( QGraphicsSceneDragDropEvent* );
  virtual void               dragMoveEvent( QGraphicsSceneDragDropEvent* );
  virtual void               dropEvent( QGraphicsSceneDragDropEvent* );

signals:
  void                       gsKeyEvent( QKeyEvent* );
  void                       gsMouseEvent( QGraphicsSceneMouseEvent* );
  void                       gsWheelEvent( QGraphicsSceneWheelEvent* );
  void                       gsContextMenuEvent( QGraphicsSceneContextMenuEvent* );

  void                       gsBoundingRectChanged();

private:
  QGraphicsRectItem*         mySceneRectItem; // for debug
};

#endif
