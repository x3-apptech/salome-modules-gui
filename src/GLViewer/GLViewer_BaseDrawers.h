// File:      GLViewer_BaseDrawers.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_BASEDRAWERS_H
#define GLVIEWER_BASEDRAWERS_H

#include <qcolor.h>
#include <qobject.h>
#include <qfile.h>
#include <qfont.h>
#include <qgl.h>

#include "GLViewer.h"
#include "GLViewer_Drawer.h"

class GLViewer_AspectLine;

#ifdef WNT
#pragma warning( disable:4251 )
#endif

// Class:   GLViewer_MarkerDrawer
// Descr:   Drawer for GLViewer_MarkerSet

class GLVIEWER_API GLViewer_MarkerDrawer : public GLViewer_Drawer  
{
public:
  GLViewer_MarkerDrawer();
  ~GLViewer_MarkerDrawer();
  
  virtual void       create( float, float, bool );
  
private:
  void               drawMarker( float&, float&, float&, QColor&, GLViewer_AspectLine* );
};

// Class:   GLViewer_PolylineDrawer
// Descr:   Drawer for GLViewer_Polyline

class GLVIEWER_API GLViewer_PolylineDrawer : public GLViewer_Drawer  
{
public:
  GLViewer_PolylineDrawer();
  ~GLViewer_PolylineDrawer();
  
  virtual void       create( float, float, bool );    
};

// Class:   GLViewer_TextDrawer
// Descr:   

class GLVIEWER_API GLViewer_TextDrawer: public GLViewer_Drawer
{
  
public:
  GLViewer_TextDrawer();
  ~GLViewer_TextDrawer();
  
  virtual void              create( float, float, bool );
  void                      updateObjects(); //after update font
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
