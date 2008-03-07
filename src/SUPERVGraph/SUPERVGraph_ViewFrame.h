//  SALOME SUPERVGraph : build Supervisor viewer into desktop
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
//
//
//  File   : SUPERVGraph_ViewFrame.h
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$

#ifndef SUPERVGraph_ViewFrame_H
#define SUPERVGraph_ViewFrame_H

#include "SUPERVGraph.h"

#include "SALOME_InteractiveObject.hxx"
#include "SUIT_PopupClient.h"
#include "SUIT_ViewWindow.h"

#include <qaction.h>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class SUPERVGRAPH_EXPORT SUPERVGraph_View: public QWidget, public SUIT_PopupClient {
  Q_OBJECT;
 public:
  static QString Type() { return "SUPERVGraphViewer"; }

  SUPERVGraph_View(QWidget* theParent);
  SUPERVGraph_View(SUPERVGraph_View* theParent);

  virtual void ActivatePanning() = 0;
  virtual void ResetView() = 0;

  /* redefine functions from SUIT_PopupClient */
  virtual QString popupClientType() const { return Type(); }
  virtual void    contextMenuPopup( QPopupMenu* );
  
  virtual void resizeView( QResizeEvent* theEvent ) {};
  
 protected:
  void init(QWidget* theParent);
  
};

class SUPERVGRAPH_EXPORT SUPERVGraph_ViewFrame : public SUIT_ViewWindow {
  Q_OBJECT

    public:
  SUPERVGraph_ViewFrame(SUIT_Desktop* theDesktop );
  ~SUPERVGraph_ViewFrame();

  SUPERVGraph_View*              getViewWidget();
  void                           setViewWidget(SUPERVGraph_View* theView);

  void                           setBackgroundColor( const QColor& );
  QColor                         backgroundColor() const;
  
  void                           onAdjustTrihedron( );
  
  /*  interactive object management */
  void                           highlight( const Handle(SALOME_InteractiveObject)& IObject, 
					    bool highlight, bool immediatly = true );
  void                           unHighlightAll();
  void                           rename( const Handle(SALOME_InteractiveObject)& IObject,
					 QString newName );
  bool                           isInViewer( const Handle(SALOME_InteractiveObject)& IObject );
  bool                           isVisible( const Handle(SALOME_InteractiveObject)& IObject );
  
  /* selection */
  Handle(SALOME_InteractiveObject) FindIObject(const char* Entry) { Handle(SALOME_InteractiveObject) o; return o; };

  /* display */		
  void           Display(const Handle(SALOME_InteractiveObject)& IObject, bool immediatly = true){};
  void           DisplayOnly(const Handle(SALOME_InteractiveObject)& IObject){};
  void           Erase(const Handle(SALOME_InteractiveObject)& IObject, bool immediatly = true){};
  void           DisplayAll(){};
  void           EraseAll(){};
  void           Repaint() {};

  QToolBar*      getToolBar() { return myToolBar; }

 public slots:
  void           onViewPan(); 
  void           onViewZoom();
  void           onViewFitAll();
  void           onViewFitArea();
  void           onViewGlobalPan(); 
  void           onViewRotate();
  void           onViewReset();     
  void           onViewFront(); 
  void           onViewBack(); 
  void           onViewRight(); 
  void           onViewLeft();     
  void           onViewBottom();
  void           onViewTop();
  void           onViewTrihedron(); 

 protected:
  void           resizeEvent( QResizeEvent* theEvent );
  
 private:
  void           createActions();
  void           createToolBar();

  //! Actions ID
  enum { PanId, ResetId };
  typedef QMap<int, QAction*> ActionsMap;

  ActionsMap       myActionsMap;
  QToolBar*        myToolBar;

  SUPERVGraph_View* myView;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
