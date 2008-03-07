//  SALOME QxGraph : build Supervisor viewer into desktop
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef QXGRAPH_CANVAS_H
#define QXGRAPH_CANVAS_H

#include "QxGraph.h"

#include <qcanvas.h>
#include <qptrlist.h>

class SUIT_ResourceMgr;
class QxGraph_Prs;

class QXGRAPH_EXPORT QxGraph_Canvas : public QCanvas {
  Q_OBJECT

 public:
  QxGraph_Canvas(SUIT_ResourceMgr*);
  virtual ~QxGraph_Canvas();

  virtual void addView(QCanvasView* theView);
  virtual void removeView(QCanvasView*);

  QPtrList<QxGraph_Prs> getPrsList() const { return myPrsList; }
  QxGraph_Prs*          getPrs(int theIndex = 0);
  void                  addPrs(QxGraph_Prs* thePrs) { myPrsList.append(thePrs); }

  virtual void removeItem( QCanvasItem* );

private:

  QValueList< QCanvasView* > myViews;
  QPtrList<QxGraph_Prs> myPrsList;

};

#endif
