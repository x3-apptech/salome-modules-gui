// File:      GLViewer_ViewPort.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/***************************************************************************
**  Class:   GLViewer_ViewPort
**  Descr:   Visualisation canvas of QAD-based application
**  Module:  QAD
**  Created: UI team, 05.09.00
****************************************************************************/
#ifndef GLVIEWER_VIEWPORT_H
#define GLVIEWER_VIEWPORT_H

#include "GLViewer.h"

#include <qlist.h>
#include <qcolor.h>
#include <qaction.h>
#include <qwidget.h>

#include <Aspect_Window.hxx>

class QRect;
class QCursor;
class QPainter;
class GLViewer_ViewSketcher;
class GLViewer_ViewTransformer;

#ifdef WNT
#pragma warning( disable:4251 )
#endif

enum BlockStatus
{
    BS_NoBlock          = 0x0000,
    BS_Highlighting     = 0x0001,
    BS_Selection        = 0x0002,
    BS_Dragging         = 0x0004
};

class GLVIEWER_API GLViewer_ViewPort: public QWidget
{
    Q_OBJECT
    friend class GLViewer_ViewSketcher;
    friend class GLViewer_ViewTransformer;

public:
    GLViewer_ViewPort( QWidget* parent );
    ~GLViewer_ViewPort();

public:
    void                             setSketchingEnabled( bool );
    bool                             isSketchingEnabled() const;
    void                             setTransformEnabled( bool );
    bool                             isTransformEnabled() const;

    virtual QColor                   backgroundColor() const;
    virtual void                     setBackgroundColor( const QColor& );

    void                             redrawPainters();

    virtual void                     onUpdate();

    virtual BlockStatus              currentBlock(){ return BS_NoBlock; }

protected:
    enum ViewType { Type2D, Type3D };
    void                             selectVisualId( ViewType );

    virtual QPaintDevice*            getPaintDevice() { return this; }
    virtual void                     contextMenuEvent( QContextMenuEvent * e );

// STATICS
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
// EVENTS
    virtual void                     paintEvent( QPaintEvent *);
    virtual void                     mouseMoveEvent( QMouseEvent *);
    virtual void                     mouseReleaseEvent( QMouseEvent *);
    virtual void                     mousePressEvent( QMouseEvent *);
    virtual void                     mouseDoubleClickEvent( QMouseEvent *);
    virtual void                     keyPressEvent( QKeyEvent *);
    virtual void                     keyReleaseEvent( QKeyEvent *);
    virtual void                     wheelEvent( QWheelEvent *);

// TO BE REDEFINED
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

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
