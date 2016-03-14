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
// File:      GLViewer_Selector2d.h
// Created:   November, 2004
//
#ifndef GLVIEWER_SELECTOR2D_H
#define GLVIEWER_SELECTOR2D_H

#ifdef WIN32
#include "windows.h"
#endif

#include "GLViewer_Selector.h"

#include <Quantity_NameOfColor.hxx>

class GLViewer_Viewer2d;
class GLViewer_Context;

/*!
  \class GLViewer_Selector2d 
  2D select manager for GLViewer
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
