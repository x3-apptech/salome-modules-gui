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
#ifndef QXGRAPH_CANVASVIEW_H
#define QXGRAPH_CANVASVIEW_H

#include "QxGraph.h"

#include <qcanvas.h>
#include <qcursor.h>
#include <qtimer.h>
#include <qtooltip.h>

class QxGraph_Canvas;
class QxGraph_ViewWindow;
class QxGraph_ActiveItem;

class QXGRAPH_EXPORT QxGraph_CanvasView : public QCanvasView {
  Q_OBJECT

 public:
  enum OperationType{ NOTHING, PANVIEW, ZOOMVIEW, PANGLOBAL, 
                      WINDOWFIT, FITALLVIEW, RESETVIEW };
  
  QxGraph_CanvasView(QxGraph_Canvas* theCanvas, QxGraph_ViewWindow* theViewWindow);
  virtual ~QxGraph_CanvasView();

  QxGraph_ViewWindow* getViewWindow() const;

  void activateFitAll();
  void activateFitRect();
  void activateZoom();
  void activatePanning();
  void activateGlobalPanning();
  void activateReset();

  void itemRemoved( QCanvasItem* );

  void setSelectedItem( QxGraph_ActiveItem* theItem );
  QxGraph_ActiveItem* getSelectedItem() const;

 signals:
  void viewOperationDone();

 public slots:
  void onTimeout();
   //void changeBackground();

 protected:
  void contentsMousePressEvent(QMouseEvent* theEvent); 
  void contentsMouseMoveEvent(QMouseEvent* theEvent);
  void contentsMouseReleaseEvent(QMouseEvent* theEvent); 
  void contentsMouseDoubleClickEvent(QMouseEvent* theEvent);

  bool isSelectedItemInCanvas();

 private:
  // for moving items
  QCanvasItem*      myCurrentItem;
  QPoint            myPoint;
  QPoint            myGlobalPoint;
  bool              myMovingDone;
  QPoint            myCenter;
  
  // for control toolbar actions
  OperationType     myOperation;
  QCursor           myCursor;
  QCanvasRectangle* mySelectedRect;

  // for automatic content scrolling if mouse is outside
  QTimer*           myTimer;
  int               myDX;
  int               myDY;

  // for hilight
  QxGraph_ActiveItem* myHilightedItem;

  // for selection
  QxGraph_ActiveItem* mySelectedItem;
};


class QxGraph_ToolTip: public QToolTip {
  
 public:
  QxGraph_ToolTip(QWidget* theWidget, QToolTipGroup* theGroup = 0):
    QToolTip(theWidget, theGroup) {}
  ~QxGraph_ToolTip() { remove(parentWidget()); }
    
  virtual void maybeTip(const QPoint& theMousePos);
};

#endif
