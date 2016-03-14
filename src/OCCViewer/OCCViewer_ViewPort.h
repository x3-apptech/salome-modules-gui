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

#ifndef OCCVIEWER_VIEWPORT_H
#define OCCVIEWER_VIEWPORT_H

#include "OCCViewer.h"

#include "QtxAction.h"

#include <QList>
#include <QWidget>

#include <Aspect_Window.hxx>

class QColor;
class QRect;
class QPainter;
class OCCViewer_ViewSketcher;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*!
  \class OCCViewer_ViewPort
  Visualisation canvas of SUIT-based application
*/
class OCCVIEWER_EXPORT OCCViewer_ViewPort : public QWidget
{
  Q_OBJECT

  friend class OCCViewer_ViewSketcher;

public:
  OCCViewer_ViewPort( QWidget* parent );
  virtual ~OCCViewer_ViewPort();

public:
  void                           setSketchingEnabled( bool );
  bool                           isSketchingEnabled() const;
  void                           setTransformEnabled( bool );
  bool                           isTransformEnabled() const;

  virtual QColor                 backgroundColor() const;
  virtual void                   setBackgroundColor( const QColor& );

  void                           redrawPainters();

  virtual void                   onUpdate();

  virtual QPaintEngine*          paintEngine() const;

protected:
//      enum ViewType { Type2D, Type3D };
  void                           selectVisualId();

// EVENTS
  virtual void                   paintEvent( QPaintEvent *);
  virtual void                   mouseMoveEvent( QMouseEvent *);
  virtual void                   mouseReleaseEvent( QMouseEvent *);
  virtual void                   mousePressEvent( QMouseEvent *);
  virtual void                   mouseDoubleClickEvent( QMouseEvent *);
  virtual void                   keyPressEvent( QKeyEvent *);
  virtual void                   keyReleaseEvent( QKeyEvent *);

// TO BE REDEFINED
  virtual void                   reset() = 0;
  virtual void                   pan( int, int ) = 0;
  virtual void                   setCenter( int, int ) = 0;
  virtual void                   fitRect( const QRect& ) = 0;
  virtual void                   zoom( int, int, int, int ) = 0;
  virtual void                   fitAll( bool keepScale = false, bool withZ = true, bool upd = true ) = 0;

public slots:
  virtual bool                   synchronize( OCCViewer_ViewPort* );

protected slots:
  virtual void                   onChangeBackground();

signals:
  void                           vpKeyEvent( QKeyEvent* );
  void                           vpMouseEvent( QMouseEvent* );
  void                           vpDrawExternal( QPainter* );
  void                           vpChangeBGColor( QColor );
  void                           vpTransformed( OCCViewer_ViewPort* );

private:
  void                           initialize();
  void                           cleanup();

protected:
  Handle(Aspect_Window)          myWindow;
  bool                           myEnableSketching;
  bool                           myEnableTransform;
  bool                           myPaintersRedrawing; /* set to draw externally */
  QList<QtxAction*>              myPopupActions;
  
private:
  static int                     nCounter; /* objects counter */
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
