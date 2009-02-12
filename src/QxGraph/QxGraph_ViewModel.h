//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef QXGRAPH_VIEWMODEL_H
#define QXGRAPH_VIEWMODEL_H

#include "QxGraph.h"

#include "SUIT_ViewModel.h"

class QCanvasItem; // for addRectangleItem(...) method (can be removed in the future)

class QxGraph_Canvas;
class QxGraph_CanvasView;
class QxGraph_ViewWindow;
class QxGraph_Prs;

class QXGRAPH_EXPORT QxGraph_Viewer: public SUIT_ViewModel
{
  Q_OBJECT

 public:
  static QString Type() { return "QxGraphViewer"; }

  QxGraph_Viewer();
  virtual ~QxGraph_Viewer();

  virtual SUIT_ViewWindow* createView(SUIT_Desktop* theDesktop);
  virtual QString          getType() const { return Type(); }

  virtual void             contextMenuPopup(QPopupMenu*);

  QxGraph_Canvas*          getCanvas() const { return myCanvas; }
  void                     setCanvas(QxGraph_Canvas* theCanvas) { myCanvas = theCanvas; }
  //QPtrList<QxGraph_Canvas> getCanvases() { return myCanvases; }
  //void                     setCanvases(QPtrList<QxGraph_Canvas> theCanvases) { myCanvases = theCanvases; }

  //void addCanvas(QxGraph_Canvas* theCanvas) { myCanvases.append(theCanvas); }
  //void removeCanvas(QxGraph_Canvas* theCanvas) { myCanvases.remove(theCanvas); }

  QPtrList<QxGraph_CanvasView> getCanvasViews() { return myCanvasViews; }
  void                         setCanvasViews(QPtrList<QxGraph_CanvasView> theViews) { myCanvasViews = theViews; }

  void addView(QxGraph_CanvasView* theView) { myCanvasViews.append(theView); }
  void removeView(QxGraph_CanvasView* theView) { myCanvasViews.remove(theView); }
  
  QxGraph_CanvasView* getCurrentView() const { return myCurrentView; }
  void                setCurrentView(QxGraph_CanvasView* theView) { myCurrentView = theView; }
  void                setCurrentView(int theIndex);

 protected:
  void initView(QxGraph_ViewWindow* view);

 protected slots:
  void onShowToolbar();
  void onChangeBgColor();

 private:
  QxGraph_Canvas*              myCanvas;
  //QPtrList<QxGraph_Canvas>     myCanvases;
  QPtrList<QxGraph_CanvasView> myCanvasViews;

  QxGraph_CanvasView*          myCurrentView;
 
};

#endif
