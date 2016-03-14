// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  SALOME QxScene : build Supervisor viewer into desktop
//
#ifndef QXSCENE_VIEWWINDOW_H
#define QXSCENE_VIEWWINDOW_H

#include "QxScene.h"
#include "QxScene_ViewModel.h"

#include "SUIT_ViewWindow.h"

#include "QtxAction.h"

#include <qaction.h>

#include <QGraphicsScene>
#include <QGraphicsView>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class SUIT_ResourceMgr;

class QXSCENE_EXPORT QxScene_ViewWindow : public SUIT_ViewWindow {
  Q_OBJECT

 public:
  //! Actions ID
  enum { FitAllId, FitRectId, ZoomId, ScaleOpId, PanId, GlobalPanId, MoveOpId, ResetId };

  QxScene_ViewWindow(SUIT_Desktop* theDesktop, QxScene_Viewer* theModel);
  virtual ~QxScene_ViewWindow();

  void              setBackgroundColor( const QColor& );
  QColor            backgroundColor() const;

  QToolBar*         getToolBar();

  SUIT_ResourceMgr* resMgr() const;

  QxScene_Viewer*   getViewModel() const { return myViewModel; }
  void              setViewModel(QxScene_Viewer* theViewModel) { myViewModel = theViewModel; }

  virtual void      initLayout();

  void              contextPopupEvent(QContextMenuEvent* theEvent) { emit contextMenuRequested( theEvent); }
  
  void              setScene(QGraphicsScene* scene) { _scene = scene; }
  QGraphicsScene*   getScene() { return _scene; }
  void              setSceneView(QGraphicsView* sceneView){ _sceneView = sceneView; }
  QGraphicsView*    getSceneView() { return _sceneView; }
  virtual bool      closeRequested();

 signals:
  void              tryClose(bool &isClosed, QxScene_ViewWindow* window);

 public slots:
  void              onViewFitAll();
  void              onViewFitArea();
  void              onViewZoom();
  void              onViewPan(); 
  void              onViewGlobalPan(); 
  void              onViewReset(); 
  
 protected:
  void              resizeEvent( QResizeEvent* theEvent );

  QGraphicsScene*   _scene;
  QGraphicsView*    _sceneView;

 private:
  void              createActions();
  void              createToolBar();

  int               myToolBar;

  QxScene_Viewer*   myViewModel;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
