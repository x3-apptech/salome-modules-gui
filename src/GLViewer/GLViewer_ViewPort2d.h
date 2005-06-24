// File:      GLViewer_ViewPort2d.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_VIEWPORT2D_H
#define GLVIEWER_VIEWPORT2D_H

#ifdef WNT
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "GLViewer_ViewPort.h"
#include "GLViewer_Widget.h"
#include "GLViewer_Geom.h"

#include <qgl.h>
#include <qcolor.h>
#include <qwidget.h>
#include <qpaintdevice.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

class GLViewer_Compass;
class GLViewer_Grid;
class GLViewer_Object;
class GLViewer_ViewFrame;

class QtxToolTip;

class GLViewer_ViewPort2d: public GLViewer_ViewPort
{
  Q_OBJECT

      enum vpDragState{ noDrag, initDrag, inDrag };

public:
    GLViewer_ViewPort2d( QWidget* parent, GLViewer_ViewFrame* theViewFrame = NULL );
    ~GLViewer_ViewPort2d();

    void                   turnGrid( GLboolean on );
    GLViewer_Grid*         getGrid() const { return myGrid; }
    void                   setGridColor( const QColor gridColor, const QColor axisColor );

    GLViewer_ViewFrame*    getViewFrame() const { return myViewFrame; }
    GLViewer_Widget*       getGLWidget() const { return myGLWidget; }
    virtual QPaintDevice*  getPaintDevice() { return myGLWidget; }

    void                   setBackgroundColor( const QColor& color);
    QColor                 backgroundColor() const;

    void                   setBorder( GLViewer_Rect* border ) { myBorder = border; }
    GLViewer_Rect*         getBorder() const { return myBorder; }

    void                   setMargin( GLfloat margin ) { myMargin = margin; }
    GLfloat                getMargin() const { return myMargin; }

    int                    getHeight() const { return myHeight; }
    int                    getWidth() const { return myWidth; }

    void                   getScale( GLfloat& xSc, GLfloat& ySc ) const { xSc = myXScale; ySc = myYScale; }
    void                   getPan( GLfloat& xPan, GLfloat& yPan ) const { xPan = myXPan; yPan = myYPan; }

    void                   initResize( int, int );

    void                   startRotation( int, int );
    void                   rotate( int, int );
    void                   endRotation();

    bool                   isDragProcess(){ return myIsDragProcess; }

    void                   turnCompass( GLboolean on );
    void                   drawCompass();

    int                    getViewPortId(){ return myViewPortId; }

    virtual BlockStatus    currentBlock();

    void                   startSelectByRect( int, int );
    void                   drawSelectByRect( int, int );
    void                   finishSelectByRect();

    bool                   startPulling( GLViewer_Pnt );
    void                   drawPulling( GLViewer_Pnt );
    void                   finishPulling();
    bool                   isPulling() const { return myIsPulling; }

    //selection by rect
    QRect                  selectionRect();
    
    GLViewer_Rect          win2GLV( const QRect& ) const;
    QRect                  GLV2win( const GLViewer_Rect& ) const;

signals:
    void                   vpUpdateValues();
    void                   objectMoved();

protected:
    void                   onDragObject( QMouseEvent* );
    
    virtual void           mouseMoveEvent( QMouseEvent *);
    virtual void           mousePressEvent( QMouseEvent *);
    virtual void           mouseReleaseEvent( QMouseEvent *);

    virtual void           paintEvent( QPaintEvent* );
    virtual void           resizeEvent( QResizeEvent* );

    virtual void           reset();
    virtual void           pan( int, int );
    virtual void           setCenter( int, int );    
    virtual void           zoom( int, int, int, int );
    virtual void           fitRect( const QRect& );
    virtual void           fitSelect();
    virtual void           fitAll( bool keepScale = false, bool withZ = true );

protected slots:
    void                   onStartDragObject();
    void                   onPasteObject();
    void                   onCutObject();
    void                   onCopyObject();

    void                   onMaybeTip( QPoint, QString&, QFont&, QRect&, QRect& );

protected:
    GLViewer_ViewFrame*    myViewFrame;
    GLViewer_Widget*       myGLWidget;
    GLViewer_Rect*         myBorder;
    QColor                 myBackgroundColor;

    GLfloat                myMargin;
    int                    myHeight;
    int                    myWidth;

    GLfloat                myXScale;
    GLfloat                myYScale;
    GLfloat                myXOldScale;
    GLfloat                myYOldScale;
    GLfloat                myXPan;
    GLfloat                myYPan;

    GLViewer_Grid*         myGrid; 
    GLViewer_Compass*      myCompass;

    //dragging
    int                    myIsDragProcess;
    float*                 myCurDragPosX;
    float*                 myCurDragPosY;

    //selection by rect
    QPoint*                mypFirstPoint;
    QPoint*                mypLastPoint;

    //pulling
    bool                   myIsPulling;
    GLViewer_Object*       myPullingObject;
    
    int                    myViewPortId;

    //GLViewer_ObjectTip*    myObjectTip;
    QtxToolTip*            myObjectTip;
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
