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

/*! Class GLViewer_MarkerDrawer
*  Drawer for GLViewer_MarkerSet
 */

class GLVIEWER_API GLViewer_MarkerDrawer : public GLViewer_Drawer  
{
public:
  GLViewer_MarkerDrawer();
  ~GLViewer_MarkerDrawer();
  
  //! Redefined method
  virtual void       create( float, float, bool );
  
private:
  //! Draws marker in point (x,y) of \param radius with \param color and \param aspect
  void               drawMarker( float& x, float& y, float& radius, QColor& color, GLViewer_AspectLine* aspect );
};

/*! Class  GLViewer_PolylineDrawer
* Drawer for GLViewer_Polyline
*/

class GLVIEWER_API GLViewer_PolylineDrawer : public GLViewer_Drawer  
{
public:
  GLViewer_PolylineDrawer();
  ~GLViewer_PolylineDrawer();
  //! Redefined method
  virtual void       create( float, float, bool );    
};

/* Class GLViewer_TextDrawer
* Drawer for GLViewer_Text
*/

class GLVIEWER_API GLViewer_TextDrawer: public GLViewer_Drawer
{
  
public:
  GLViewer_TextDrawer();
  ~GLViewer_TextDrawer();
  
  //! Redefined method
  virtual void              create( float, float, bool );
  //! Updates objects after updating font
  void                      updateObjects();
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
