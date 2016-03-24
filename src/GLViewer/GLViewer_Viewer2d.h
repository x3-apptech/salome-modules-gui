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
// File:      GLViewer_Viewer2d.h
// Created:   November, 2004
//
#ifndef GLVIEWER_VIEWER2D_H
#define GLVIEWER_VIEWER2D_H

#ifdef WIN32
#include <windows.h>
#endif

class QRect;
class QFile;

#include "GLViewer_Viewer.h"

#include <TColStd_SequenceOfInteger.hxx>

#include <GL/gl.h>

class GLViewer_Object;
class GLViewer_ViewFrame;
class GLViewer_Drawer;
class GLViewer_Context;
class GLViewer_Rect;
//class GLViewer_Sketcher;

class SUIT_Desktop;
class SUIT_ViewWindow;

//! Paper sizes array
const double Sizes[2*5] = { 
    /* A1 */ 594.0, 840.0,
    /* A2 */ 420.0, 594.0,
    /* A3 */ 297.0, 420.0,
    /* A4 */ 210.0, 297.0,
    /* A5 */ 148.5, 210.0
};

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  \class GLViewer_Viewer2d
  OpenGL Viewer 2D
*/

class GLVIEWER_API GLViewer_Viewer2d : public GLViewer_Viewer
{
  Q_OBJECT

public:
  //! Type of sketcher operation    
  enum GLSketchingType
  {
    None,
    Polyline,
    Arc,
    Curve,
    Scribble,
    Oval,
    Rectangle
  };
  //! Type of export vector file
  enum VectorFileType
  {
    POST_SCRIPT,
    HPGL
#ifdef WIN32
    , ENH_METAFILE
#endif
  };

  //! Type of paper for export to vector format
  enum PaperType
  {
    A1=0,
    A2,
    A3,
    A4,
    A5
  };

public:
  //! A constructor
  GLViewer_Viewer2d( const QString& title );
  //! A destructor
  ~GLViewer_Viewer2d();

public:
  //! Redefined method
  /*Returns GLViewer_ViewFrame*/
  virtual SUIT_ViewWindow*  createView( SUIT_Desktop* );
  
  //! Adds item for change background color
  void                 addPopupItems( QMenu* );

  //void                activateGLSketching( int );

  //! Returns all drawers
  const QList<GLViewer_Drawer*>& getDrawers() const { return myDrawers; }
  
  //! Returns context
  GLViewer_Context*    getGLContext() const { return myGLContext; }
  //! Updates colors for all drawers (does not work)
  void                 updateColors( QColor colorH, QColor colorS );
  
  //! Updates rect of global scene by adding new rect
  void                 updateBorders( GLViewer_Rect* theRect );
  //! Recomputes global scene rect
  void                 updateBorders();

  //! Redraws all active objects by updating all drawers in all views
  void                 updateAll();
  //! Updates all drawers with new scale factor
  /* \param onlyUpdate is passed to method activateAllDrawersdrawers*/
  void                 updateDrawers( GLboolean onlyUpdate, GLfloat scX = 0.0, GLfloat scY = 0.0 );
  //! Activates drawers for objects from list \param theObjects only
  void                 activateDrawers( QList<GLViewer_Object*>& theObjects, bool onlyUpdate, GLboolean swap = GL_FALSE );
  //! Activates drawer for \param theObject
  void                 activateDrawer( GLViewer_Object* theObject, bool onlyUpdate, GLboolean swap = GL_FALSE );
  //! Updates all drawers with new scale factor
  /* \param onlyUpdate is passed to drawers*/
  void                 activateAllDrawers( bool onlyUpdate, GLboolean swap = GL_FALSE );
  
  //! Translates point (x,y) from global CS to curreent viewer CS
  void                 transPoint( GLfloat& x, GLfloat& y );
  //! Returns object rect in window CS
  QRect*               getWinObjectRect( GLViewer_Object* theObject);
  
  //! Translates rect in window CS to rect in global CS
  GLViewer_Rect        getGLVRect( const QRect& ) const;
  //! Translates rect in global CS to rect in window CS
  QRect                getQRect( const GLViewer_Rect& ) const;
  
  //! Inserts common text lines starting file of \param aType
  virtual void         insertHeader( VectorFileType aType, QFile& hFile );
  //! Inserts common text lines ending file of \param aType
  virtual void         insertEnding( VectorFileType aType, QFile& hFile );
  //! Translates current view content to vector file
  /* Translates current view content to vector file with type \param aType, name \param FileName,
   * output paper size \param aPType, with margins in mm
  */
  virtual bool         translateTo( VectorFileType aType, QString FileName, PaperType aPType, 
                                   double mmLeft, double mmRight, double mmTop, double mmBottom );
    
  //bool                 isSketchingActive();
  //int                  getSketchingType();
  
  //virtual void         startSketching();
  //virtual void         finishSketching();

  //! Repaints view \param theView. If \param theView = NULL repaints all views.
  void                 repaintView( GLViewer_ViewFrame* theView = NULL, bool makeCurrent = false );

signals:
  //! Signal needs for optimum recompute of zoom depending objects
  //! Warning: use recompute without update viewer
  void                 wheelZoomChange( bool );

public slots:
  //void                 onSketchDelObject();
  //void                 onSketchUndoLast();
  //void                 onSketchFinish();

  //! Changes background color
  void                 onChangeBgColor();
  //! Creates set of marker number \param number and radius = \param size
  void                 onCreateGLMarkers( int number = 1000, int size = 5 );
  //! Creates set of polyline number \param number, number of angles = \param angles and diameter = \param size
  void                 onCreateGLPolyline( int number = 100, int angles = 10, int size = 100 );
  //! Creates set of text number \param number and with text = \param text
  void                 onCreateGLText( QString text = "Text", int number = 1 );

protected:
  //! Returns new selector
  GLViewer_Selector*        createSelector();
  //! Returns new Transformer with type \param type
  GLViewer_ViewTransformer* createTransformer( int type);
  
  //! Transforms point (x,y) in Viewer CS to Post Script CS
  void                 transformCoordsToPS( double& x, double& y );
  //! Transforms point (x,y) in Viewer CS to HPGL CS
  void                 transformCoordsToHPGL( double& x, double& y );
  
  //! Starts any operations on mouse event
  virtual void         startOperations( QMouseEvent* );
  //! Updates started operations on mouse event
  virtual bool         updateOperations( QMouseEvent* );
  //! Completes started operations on mouse event
  virtual void         finishOperations( QMouseEvent* );
  //! Starts any operations on mouse wheel event
  virtual void         startOperations( QWheelEvent* );

protected slots:
  void                 onMouseEvent( SUIT_ViewWindow*, QMouseEvent* );

private:
  //! Rotation transformation
  bool                 testRotation( QMouseEvent* );
protected:
  //! Current context 
  GLViewer_Context*             myGLContext;
  //! Map of active drawers
  QList<GLViewer_Drawer*>  myDrawers;
  
  //GLViewer_Sketcher*   myGLSketcher;
};

class GLVIEWER_API GLViewer_View2dTransformer : public GLViewer_ViewTransformer
{
public:
  GLViewer_View2dTransformer( GLViewer_Viewer*, int );
  ~GLViewer_View2dTransformer();
  
  virtual void         exec();
  
  /*! Sets/returns mouse butto which will be used for rotation ( MB1 by default ) */
  static int           rotateButton() { return rotateBtn; }
  static void          setRotateButton( int b ) { rotateBtn = b; }

protected:
  void                 onTransform( TransformState );

  static  int          rotateBtn;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
