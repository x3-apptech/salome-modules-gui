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
// File:      GLViewer_ViewPort2d.h
// Created:   November, 2004
//
#ifndef GLVIEWER_VIEWPORT2D_H
#define GLVIEWER_VIEWPORT2D_H

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "GLViewer_ViewPort.h"
#include "GLViewer_Widget.h"
#include "GLViewer_Geom.h"

#include <QtOpenGL>
#include <QColor>
#include <QPaintDevice>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class GLViewer_Compass;
class GLViewer_Grid;
class GLViewer_Object;
class GLViewer_ViewFrame;

class QtxToolTip;

class QWidget;
class QRubberBand;

/*! 
 * Class GLViewer_ViewPort
 * 2D visualisation canvas of GLViewer
 */
class GLViewer_ViewPort2d: public GLViewer_ViewPort
{
  Q_OBJECT

  //! Dragging states
  enum vpDragState{ noDrag, initDrag, inDrag };

public:
  GLViewer_ViewPort2d( QWidget* parent, GLViewer_ViewFrame* theViewFrame = NULL );
  ~GLViewer_ViewPort2d();

  //! On/off rectangular grid
  void                   turnGrid( GLboolean on );
  //! Returns rectangular grid
  GLViewer_Grid*         getGrid() const { return myGrid; }
  //! Returns grid color
  void                   setGridColor( const QColor gridColor, const QColor axisColor );

  //! Returns parent window
  GLViewer_ViewFrame*    getViewFrame() const { return myViewFrame; }
  //! Returns painted widget
  GLViewer_Widget*       getGLWidget() const { return myGLWidget; }
  virtual QPaintDevice*  getPaintDevice() { return myGLWidget; }

  //! Sets background color
  void                   setBackgroundColor( const QColor& color);
  //! Returns background color
  QColor                 backgroundColor() const;

  //! Sets borders of scene
  void                   setBorder( GLViewer_Rect* border ) { myBorder = border; }
  //! Get current borders of scene
  GLViewer_Rect*         getBorder() const { return myBorder; }
  
  //! Sets margin of borders
  void                   setMargin( GLfloat margin ) { myMargin = margin; }
  //! Returns margin of borders
  GLfloat                getMargin() const { return myMargin; }

  //! Returns width of view
  int                    getWidth() const { return myWidth; }
  //! Returns height of view
  int                    getHeight() const { return myHeight; }

  
  //! Returns scale factors
  void                   getScale( GLfloat& xSc, GLfloat& ySc ) const { xSc = myXScale; ySc = myYScale; }
  //! returns offsets
  void                   getPan( GLfloat& xPan, GLfloat& yPan ) const { xPan = myXPan; yPan = myYPan; }

  //! Resize view
  void                   initResize( int width, int height );
  
  //! Begins rotation
  void                   startRotation( int, int );
  //! Process rotation
  void                   rotate( int, int );
  //! Completes rotation
  void                   endRotation();
  
  //! Checks of dragging process state
  bool                   isDragProcess(){ return myIsDragProcess; }
  
  //! On/off compass
  void                   turnCompass( GLboolean on );
  //! Draws compass
  void                   drawCompass();
  
  //! Returns unique ID of ViewPort
  int                    getViewPortId(){ return myViewPortId; }
  
  //! Redefined method
  virtual BlockStatus    currentBlock();
  
  //! Initializes before selecting by rect
  void                   startSelectByRect( int x, int y );
  //! Draw selecting rectandle
  void                   drawSelectByRect( int x, int y );
  //! Pass rect into selector and update
  void                   finishSelectByRect();
  
  //! \warnign It is for ouv
  bool                   startPulling( GLViewer_Pnt );
  //! \warnign It is for ouv
  void                   drawPulling( GLViewer_Pnt );
  //! \warnign It is for ouv
  void                   finishPulling();
  //! \warnign It is for ouv
  bool                   isPulling() const { return myIsPulling; }

  //! Returns selection by rect
  QRect                  selectionRect();
  
  //! Transforms window rect to global rect 
  GLViewer_Rect          win2GLV( const QRect& ) const;
  //! Transforms global rect to window rect
  QRect                  GLV2win( const GLViewer_Rect& ) const;

signals:
  //! Emits after any transformation
  void                   vpUpdateValues();

  void                   objectMoved();

protected:
  void                   onDragObject( QMouseEvent* );
    
  virtual void           mouseMoveEvent( QMouseEvent *);
  virtual void           mousePressEvent( QMouseEvent *);
  virtual void           mouseReleaseEvent( QMouseEvent *);
  virtual void           mouseDoubleClickEvent( QMouseEvent *);
  
  virtual void           paintEvent( QPaintEvent* );
  virtual void           resizeEvent( QResizeEvent* );

  //! Returns view to begin state
  virtual void           reset();
  //! Sets offset to view
  virtual void           pan( int dx, int dy );
  //! Sets view center in global coords
  virtual void           setCenter( int x, int y );
  //! Process zoming transformation with mouse tracking from ( x0, y0 ) to ( x1, y1 )
  virtual void           zoom( int x0, int y0, int x1, int y1 );
  //! Transforms view by rectangle
  virtual void           fitRect( const QRect& );
  //! Transforms view by selection
  virtual void           fitSelect();
  //! Transform view by view borders ( if \param keepScale = true, zoom does not change )
  virtual void           fitAll( bool keepScale = false, bool withZ = true );

protected slots:
  //! Initializes drag process
  void                   onStartDragObject();
  //! Pastes object from clipboard
  void                   onPasteObject();
  //! Cuts object to clipboard
  void                   onCutObject();
  //! Copies object to clipboard
  void                   onCopyObject();

  //! Sets tool tip with \param text to \param theTextReg and on \param theViewReg whan mouse is on \param thePoint
  void                   onMaybeTip( QPoint thePoint, QString& text, QFont& font, QRect& theTextReg, QRect& theViewReg );
  
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
  //! flag to block mouse release event just after mouse double-click
  bool                   myIsMouseReleaseBlock;

  QRubberBand*           myRectBand; //!< selection rectangle rubber band
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
