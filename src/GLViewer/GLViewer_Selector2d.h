// File:      GLViewer_Selector2d.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/****************************************************************************
**  Class:   GLViewer_Selector2d 
**  Descr:   OpenGL Selector 2D
**  Module:  GLViewer
**  Created: UI team, 20.09.02
*****************************************************************************/
#ifndef GLVIEWER_SELECTOR2D_H
#define GLVIEWER_SELECTOR2D_H

#ifdef WNT
#include "windows.h"
#endif

#include "GLViewer_Selector.h"

#include <Quantity_NameOfColor.hxx>

class GLViewer_Viewer2d;
class GLViewer_Context;

/*! Class GLViewer_Selector2d
*   2D select manager for GLViewer
*/
class GLVIEWER_API GLViewer_Selector2d : public GLViewer_Selector
{
  Q_OBJECT

public:
  GLViewer_Selector2d( GLViewer_Viewer2d*, GLViewer_Context* );
  ~GLViewer_Selector2d();

public:
  //! Sets context from Viewer2d
  void                    setContext( GLViewer_Context* glc ) { myGLContext = glc; }
  GLViewer_Context*       getContext() const { return myGLContext; }

  // Redefined methods
  virtual void            setHilightColor( Quantity_NameOfColor );
  virtual void            setSelectColor( Quantity_NameOfColor );

  virtual void            detect( int, int );
  virtual void            undetectAll();

  virtual void            select( bool append = false );
  virtual void            select( const QRect&, bool append = false );    
  virtual void            unselectAll();    
  virtual int             numSelected() const;    

  virtual void            checkSelection( int, bool, int );

protected:
  GLViewer_Context*       myGLContext;
};

#endif
