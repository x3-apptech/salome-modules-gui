// File:      GLViewer_ViewFrame.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/***************************************************************************
**  Class:   GLViewer_ViewFrame
**  Descr:   Frame window for viewport in QAD-based application
**  Module:  QAD
**  Created: UI team, 05.09.00
****************************************************************************/
#ifndef GLVIEWER_VIEWFRAME_H
#define GLVIEWER_VIEWFRAME_H

#include "SUIT_ViewWindow.h"
#include "GLViewer.h"

class QColor;

class SUIT_Desktop;

class GLViewer_Viewer;
class GLViewer_ViewPort;

#include <qaction.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

class GLVIEWER_API GLViewer_ViewFrame: public SUIT_ViewWindow
{
  Q_OBJECT
    
public:
  GLViewer_ViewFrame( SUIT_Desktop* , GLViewer_Viewer* );
  ~GLViewer_ViewFrame();
  
public:
  void                    setViewer( GLViewer_Viewer* );
  GLViewer_Viewer*        getViewer() const;
  
  void                    setViewPort( GLViewer_ViewPort* );
  GLViewer_ViewPort*      getViewPort() const;
  
  void                    setBackgroundColor( const QColor& );
  QColor                  backgroundColor() const;
  
  QSize                   sizeHint() const;
  
  virtual void            onUpdate( int );
  
signals:
  void                    vfDrawExternal( QPainter* );
  void                    vfViewClosing( QCloseEvent* );
  
protected:
  GLViewer_Viewer*        myViewer;
  GLViewer_ViewPort*      myVP;
  
public:
  //ViewType       getTypeView() const { return VIEW_GL; }; 
  QWidget*       getViewWidget() { return ( QWidget* )getViewPort(); };
  
protected slots:
  void           onViewDump();
  void           onViewPan();
  void           onViewZoom();
  void           onViewFitAll();
  void           onViewFitArea();
  void           onViewFitSelect();
  void           onViewGlobalPan(); 
  void           onViewRotate();
  void           onViewReset();
  void           onViewFront() {}; 
  void           onViewBack() {}; 
  void           onViewRight() {}; 
  void           onViewLeft() {};     
  void           onViewBottom() {};
  void           onViewTop() {};
  void           onViewTrihedron() {}; 
  
private slots:
  void           keyEvent( QKeyEvent* );
  void           mouseEvent( QMouseEvent* );
  void           wheelEvent( QWheelEvent* );
  
private:
  void           createActions();
  void           createToolBar();
  
private:
  enum { DumpId, FitAllId, FitRectId, FitSelectId, ZoomId, PanId, GlobalPanId, ResetId };
  typedef QMap<int, QAction*> ActionsMap;
  
private:
  ActionsMap       myActionsMap;
  QToolBar*        myToolBar;
};


#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
