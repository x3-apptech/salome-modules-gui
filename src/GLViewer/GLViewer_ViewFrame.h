// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  Author : OPEN CASCADE
// File:      GLViewer_ViewFrame.h
// Created:   November, 2004
//
#ifndef GLVIEWER_VIEWFRAME_H
#define GLVIEWER_VIEWFRAME_H

#include "SUIT_ViewWindow.h"
#include "GLViewer.h"

class QColor;

class SUIT_Desktop;

class GLViewer_Viewer;
class GLViewer_ViewPort;

#include <QtxAction.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  \class GLViewer_ViewFrame
  Frame window for viewport in QAD-based application
*/

class GLVIEWER_API GLViewer_ViewFrame: public SUIT_ViewWindow
{
  Q_OBJECT

public:
  //! Actions ID
  enum { DumpId, FitAllId, FitRectId, FitSelectId, ZoomId, PanId, GlobalPanId, ResetId };

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

  virtual QString         getVisualParameters();
  virtual void            setVisualParameters( const QString& parameters );

  virtual QImage          dumpView();

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
};


#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
