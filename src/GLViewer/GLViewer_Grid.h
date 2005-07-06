// File:      GLViewer_Grid.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_GRID_H
#define GLVIEWER_GRID_H

#ifdef WNT
#include "windows.h"
#endif

#include "GLViewer.h"

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

/*! Class GLViewer_Grid
  2D rectangular grid for GLViewer
  Grid is adapt cells for current view
*/

class GLVIEWER_API GLViewer_Grid 
{
public:
  //! A default constructor
  GLViewer_Grid();
  //! A constructor
  /*
  * \param  width and \param height - width and height of grid 
  * \param winW and \param winH     - width and height of window 
  * \param xSize and \param ySize   - steps along x and y direction
  * \param xPan and \param yPan     - offsets along x and y direction
  * \param xScale and \param yScal  - scale factors along x and y direction
  */
  GLViewer_Grid( GLfloat width, GLfloat height,
		 GLfloat winW, GLfloat winH,
		 GLfloat xSize, GLfloat ySize,
		 GLfloat xPan, GLfloat yPan,
		 GLfloat xScale, GLfloat yScale );
  ~GLViewer_Grid();

  //! Draws grid
  void                   draw();

  //! Sets color of grid in RGB format
  void                   setGridColor( GLfloat r, GLfloat g, GLfloat b );
  //! Sets color of grid axes in RGB format
  void                   setAxisColor( GLfloat r, GLfloat g, GLfloat b );
  void                   setGridWidth( float );
  //! Sets Radius of center point( begin coords )
  void                   setCenterRadius( int );

  //! Sets steps along x and y directions
  void                   setSize( float xs, float ys );
  //! Sets offset along x and y direction
  void                   setPan( float xp, float yp );
  //! Sets common scale factor along x and y direction
  bool                   setZoom( float zoom );
  //! Recomputes grid in new size and scale of view
  void                   setResize( float winW, float winH, float Zoom );

  void                   getSize( float&, float& ) const;
  void                   getPan( float&, float& ) const;
  void                   getScale( float&, float& ) const;

  //! Sets step of scale
  void                   setScaleFactor( int );
  int                    getScaleFactor();

protected:
  //! Initialize grid display list
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

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
