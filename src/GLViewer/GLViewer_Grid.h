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

class GLVIEWER_API GLViewer_Grid 
{
public:
  GLViewer_Grid();
  GLViewer_Grid( float, float, float, float, float, float, float, float, float, float );
  ~GLViewer_Grid();

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

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
