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
// File:      GLViewer_ViewPort.h
// Created:   November, 2004
//
#ifndef GLVIEWER_VIEWPORT_H
#define GLVIEWER_VIEWPORT_H

#include "GLViewer.h"

//#include <QList>
#include <QColor>
//#include <QAction>
#include <QWidget>

//#include <Aspect_Window.hxx>

class QRect;
class QCursor;
class QPainter;
class GLViewer_ViewSketcher;
class GLViewer_ViewTransformer;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

enum BlockStatus
{
    BS_NoBlock          = 0x0000,
    BS_Highlighting     = 0x0001,
    BS_Selection        = 0x0002,
    BS_Dragging         = 0x0004
};

/*!
  \class GLViewer_ViewPort
  Visualisation canvas of GLViewer
*/
class GLVIEWER_API GLViewer_ViewPort: public QWidget
{
    Q_OBJECT
    friend class GLViewer_ViewSketcher;
    friend class GLViewer_ViewTransformer;

public:
    GLViewer_ViewPort( QWidget* parent );
    ~GLViewer_ViewPort();

public:
    //! Activates/deactivates sketching
    void                             setSketchingEnabled( bool );
    //! Checks active status of sketcher
    bool                             isSketchingEnabled() const;
    //! Activates/deactivates transformer
    void                             setTransformEnabled( bool );
    //! Checks active status of transformer
    bool                             isTransformEnabled() const;

    //! Returns background color
    virtual QColor                   backgroundColor() const;
    //! Sets background color
    virtual void                     setBackgroundColor( const QColor& );

    //! Redraw external pa inters
    void                             redrawPainters();

    //! Updates view
    virtual void                     onUpdate();

    //! Returns blocking status for current started operations
    virtual BlockStatus              currentBlock(){ return BS_NoBlock; }

protected:
    enum ViewType { Type2D, Type3D };
    void                             selectVisualId( ViewType );

    virtual QPaintDevice*            getPaintDevice() { return this; }
    virtual void                     contextMenuEvent( QContextMenuEvent * e );

//! STATICS
    static void                      createCursors();
    static void                      destroyCursors();
    static QCursor*                  getHandCursor() { return handCursor; }
    static void                      setHandCursor( const QCursor& newCursor );
    static QCursor*                  getPanCursor() { return panCursor; }
    static void                      setPanCursor( const QCursor& newCursor );
    static QCursor*                  getPanglCursor() { return panglCursor; }
    static void                      setPanglCursor( const QCursor& newCursor );
    static QCursor*                  getZoomCursor() { return zoomCursor; }
    static void                      setZoomCursor( const QCursor& newCursor );

public://ouv
    static QCursor*                  getDefaultCursor() { return defCursor; }
    static void                      setDefaultCursor( const QCursor& newCursor );
    static QCursor*                  getRotCursor() { return rotCursor; }
    static void                      setRotCursor( const QCursor& newCursor );
    static QCursor*                  getSketchCursor() { return sketchCursor; }
    static void                      setSketchCursor( const QCursor& newCursor );

protected:
//! EVENTS
    virtual void                     paintEvent( QPaintEvent *);
    virtual void                     mouseMoveEvent( QMouseEvent *);
    virtual void                     mouseReleaseEvent( QMouseEvent *);
    virtual void                     mousePressEvent( QMouseEvent *);
    virtual void                     mouseDoubleClickEvent( QMouseEvent *);
    virtual void                     keyPressEvent( QKeyEvent *);
    virtual void                     keyReleaseEvent( QKeyEvent *);
    virtual void                     wheelEvent( QWheelEvent *);

//! TO BE REDEFINED
    virtual void                     reset() = 0;
    virtual void                     pan( int, int ) = 0;
    virtual void                     setCenter( int, int ) = 0;    
    virtual void                     zoom( int, int, int, int ) = 0;
    virtual void                     fitRect( const QRect& ) = 0;
    virtual void                     fitSelect() = 0;
    virtual void                     fitAll( bool keepScale = false, bool withZ = true ) = 0;

protected slots:
    virtual void                     onChangeBgColor();

signals:
    void                             vpKeyEvent( QKeyEvent* );
    void                             vpMouseEvent( QMouseEvent* );
    void                             vpWheelEvent( QWheelEvent* );
    void                             vpDrawExternal( QPainter* );

    void                             contextMenuRequested( QContextMenuEvent* );

private:
    void                             initialize();
    void                             cleanup();

protected:
    //Handle(Aspect_Window)            myWindow;
    bool                             myEnableSketching;
    bool                             myEnableTransform;
    bool                             myPaintersRedrawing;   /* set to draw externally */
    //QList<QAction>                   myPopupActions;

private:
    static int                       nCounter;              /* objects counter */
    static QCursor*                  defCursor;
    static QCursor*                  panglCursor;
    static QCursor*                  handCursor;
    static QCursor*                  panCursor;
    static QCursor*                  zoomCursor;
    static QCursor*                  rotCursor;
    static QCursor*                  sketchCursor;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
