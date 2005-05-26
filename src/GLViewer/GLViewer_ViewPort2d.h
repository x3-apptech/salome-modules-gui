// File:      GLViewer_ViewPort2d.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/* GLViewer_ViewPort2d Header File */

#ifdef WNT
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "GLViewer_ViewPort.h"
#include "GLViewer_Drawer.h"

#include <qcolor.h>

class GLViewer_ViewFrame;
class GLViewer_Object;
class GLViewer_Pnt;
class GLViewer_Rect;

/***************************************************************************
**  Class:   GLViewer_RectangularGrid
**  Descr:   OpenGL Grid for GLViewer_ViewPort2d
**  Module:  QAD
**  Created: UI team, 16.09.02
****************************************************************************/
#ifndef GLVIEWER_RECTANGULARGRID_H
#define GLVIEWER_RECTANGULARGRID_H

class GLVIEWER_EXPORT GLViewer_RectangularGrid 
{
public:
  GLViewer_RectangularGrid();
  GLViewer_RectangularGrid( float, float, float, float, float, float, float, float, float, float );
  ~GLViewer_RectangularGrid();

  void                   draw();

  void                   setGridColor( GLfloat, GLfloat, GLfloat );
  void                   setAxisColor( GLfloat, GLfloat, GLfloat );
  void                   setGridWidth( float );
  void                   setCenterRadius( int );

  void                   setSize( float, float );
  void                   setPan( float, float );
  bool                   setZoom( float );
  void                   setResize( float, float, float );

  void                   getSize( float&, float& ) const;
  void                   getPan( float&, float& ) const;
  void                   getScale( float&, float& ) const;

  void                   setScaleFactor( int );
  int                    getScaleFactor();

protected:
  bool                   initList();

  GLuint                 myGridList;
  GLfloat                myGridColor[3];
  GLfloat                myAxisColor[3];
  GLfloat                myGridHeight;
  GLfloat                myGridWidth;
  GLfloat                myWinW;
  GLfloat                myWinH;
  GLfloat                myXSize;
  GLfloat                myYSize;
  GLfloat                myXPan;
  GLfloat                myYPan;
  GLfloat                myXScale;
  GLfloat                myYScale;
  GLfloat                myLineWidth;
  GLfloat                myCenterWidth;
  GLint                  myCenterRadius;
  GLint                  myScaleFactor;
  GLboolean              myIsUpdate;
};

#endif

/***************************************************************************
**  Class:   GLViewer_ViewPort2d
**  Descr:   OpenGL ViewPort 2D
**  Module:  QAD
**  Created: UI team, 02.09.02
****************************************************************************/

//********* class compass for viewport******************
class GLVIEWER_EXPORT GLViewer_Compass {   
public:
    enum Position { TopLeft, TopRight, BottomLeft, BottomRight };

    GLViewer_Compass( const QColor& color = QColor ( 0, 255, 0 ), 
                   const int size = 60,
                   const Position pos = TopRight,
                   const int WidthTop = 20,
                   const int WidthBottom = 10,
                   const int HeightTop = 25,
                   const int HeightBottom = 7 );
    ~GLViewer_Compass(){ delete myFont; }

    void        setCompass( const QColor& color, const int size, const Position pos )
                                                    {myCol=color;mySize=size;myPos=pos;};
    void        setVisible( const bool vis = true );
    bool        getVisible(){ return myIsVisible; };

    void        setSize( const int size ){mySize=size;};
    int         getSize(){ return mySize; };

    void        setPos( const Position pos ){myPos=pos;};
    int         getPos(){ return myPos; };

    void        setColor( const QColor& color ){myCol=color;};
    QColor      getColor(){ return myCol; };

    void        setArrowWidthTop( const int WidthTop ){ if( WidthTop<myArrowWidthBottom || 
                                                            WidthTop>mySize ) return;
                                                        myArrowWidthTop=WidthTop; };
    int         getArrowWidthTop(){return myArrowWidthTop;};

    void        setArrowWidthBottom( const int WidthBot ){ if( WidthBot>myArrowWidthTop || WidthBot<1 )return;
                                                           myArrowWidthBottom=WidthBot; };
    int         getArrowWidthBottom(){return myArrowWidthBottom;};

    void        setArrowHeightTop( const int HeightTop ){ if( HeightTop>(2*mySize-myArrowHeightBottom ) ||
                                                              HeightTop<1 )return;
                                                          myArrowHeightTop=HeightTop;};
    int         getArrowHeightTop(){return myArrowHeightTop;};

    void        setArrowHeightBottom( const int HeightBot ){ if( HeightBot>( 2*mySize-myArrowHeightTop ) ||
                                                                 HeightBot<1)return;
                                                          myArrowHeightBottom=HeightBot;};
    int         getArrowHeightBottom(){return myArrowHeightBottom;};        

    GLViewer_TexFont* getFont();
    void           setFont( QFont theFont ){ delete myFont; myFont = new GLViewer_TexFont( &theFont ); } 

protected:
    QColor          myCol;
    int             mySize;
    int             myPos;
    bool            myIsVisible;
    int             myArrowWidthTop;
    int             myArrowWidthBottom;
    int             myArrowHeightTop;
    int             myArrowHeightBottom;
    GLViewer_TexFont*  myFont;
    bool            isGenereted;
};


#ifndef GLVIEWER_VIEWPORT2D_H
#define GLVIEWER_VIEWPORT2D_H

#include <qwidget.h>
#include <qmap.h>
#include <qpaintdevice.h>
#include <qgl.h>

#include "GLViewer_Widget.h"

class QtxToolTip;

class GLViewer_ViewPort2d: public GLViewer_ViewPort
{
  Q_OBJECT

      enum vpDragState{ noDrag, initDrag, inDrag };

public:
    GLViewer_ViewPort2d( QWidget* parent, GLViewer_ViewFrame* theViewFrame = NULL );
    ~GLViewer_ViewPort2d();

    void                   turnGrid( GLboolean on );
    GLViewer_RectangularGrid* getGrid() const { return myGrid; }
    void                   setGridColor( const QColor gridColor, const QColor axisColor );

    GLViewer_ViewFrame*    getViewFrame() const { return myViewFrame; }
    GLViewer_Widget*       getGLWidget() const { return myGLWidget; }
    virtual QPaintDevice*  getPaintDevice() { return myGLWidget; }

    void                   setBackgroundColor( const QColor& color);
    QColor                 backgroundColor() const;

    void                   setBorder( QRect* border ) { myBorder = border; }
    QRect*                 getBorder() const { return myBorder; }

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
    void                   onCreatePopup();
    void                   onCreatePopup( QPopupMenu* );
    void                   onDestroyPopup( QPopupMenu* );

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
    GLViewer_Widget*       myGLWidget;
    QColor                 myBackgroundColor;
    QRect*                 myBorder;
    GLfloat                myMargin;
    int                    myHeight;
    int                    myWidth;
    GLfloat                myXScale;
    GLfloat                myYScale;
    GLfloat                myXOldScale;
    GLfloat                myYOldScale;
    GLfloat                myXPan;
    GLfloat                myYPan;
    GLViewer_RectangularGrid* myGrid; 
    GLViewer_ViewFrame*    myViewFrame;

    bool                   myDegenerated;

    //dragging
    int                    myIsDragProcess;
    float*                 myCurDragPosX;
    float*                 myCurDragPosY;
    
    GLViewer_Compass*      myCompass;

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

#endif
