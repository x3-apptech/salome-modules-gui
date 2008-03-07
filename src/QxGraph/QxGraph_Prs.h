// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef QXGRAPH_PRS_H
#define QXGRAPH_PRS_H

#include <QxGraph.h>

#include <qcanvas.h>
#include <map>
#include <list>

class QxGraph_Canvas;

class QXGRAPH_EXPORT QxGraph_Prs
{
 public:
  QxGraph_Prs(QxGraph_Canvas*);
  virtual ~QxGraph_Prs();

  QxGraph_Canvas* getCanvas() const { return myCanvas; }

  void addItem(QCanvasItem* theItem, int theDMode = -1);
  void removeItem(QCanvasItem* theItem, int theDMode = -1);
  
  /* add items for display mode theDMode
     if theDMode is equal to -1 add item for the current (active) display mode */
  QCanvasItem*   addRectangleItem(QRect theRect, int theDMode = -1);
  QCanvasItem*   addPolygonItem(QPointArray thePA, int theDMode = -1);
  QCanvasItem*   addLineItem(QPoint theStart, QPoint theEnd, int theDMode = -1);
  QCanvasItem*   addEllipseItem(int theW, int theH, int theStartAngle, int theAngle, int theDMode = -1);
  QCanvasItem*   addTextItem(QString theText, int theDMode = -1);

  typedef std::map< int, std::list<QCanvasItem*> > DMode2ItemList;

  const DMode2ItemList& getDisplayMap() const { return myDisplayMap; }
  const std::list<QCanvasItem*>& getItems(int theDMode) { return myDisplayMap[theDMode]; }
  
  void setDMode(int theDMode) { myDMode = theDMode; }
  int  getDMode() const { return myDMode; }

  virtual void    show();
  virtual void    hide();
  virtual void    setToUpdate( const bool );
  bool            isToUpdate() { return needUpdate; }

protected:
  virtual void    update();

private:
  QxGraph_Canvas* myCanvas;
  DMode2ItemList  myDisplayMap;

  int             myDMode;
  bool            needUpdate;
};

#endif
