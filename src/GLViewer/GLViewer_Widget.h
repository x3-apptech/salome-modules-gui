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
// File:      GLViewer_Widget.h
// Created:   November, 2004
//
#ifndef GLVIEWER_WIDGET_H
#define GLVIEWER_WIDGET_H

#include "GLViewer.h"

#include <QtOpenGL>

class QFile;

class GLViewer_ViewPort2d;
class GLViewer_CoordSystem;

/*!
  \class GLViewer_Widget
  Widget for visualization of OpenGL scene
*/
class GLVIEWER_API GLViewer_Widget : public QGLWidget
{
  Q_OBJECT

public:
  //! A constructor
  /*! Parameters using for QOGLWidget as is 
  */
  GLViewer_Widget( QWidget* theParent, const char* theName = 0 );
 
  //! A destructor
  ~GLViewer_Widget();

  //! Returns parent GLViewer_ViewPort2d
  /*! ViewPort2d because this class is not use for 3D Viewer */
  GLViewer_ViewPort2d*   getViewPort() const { return myViewPort; }
  //! Returns width of OpenGl Window
  GLint                  getWidth() const { return myWidth; }
  //! Returns height of OpenGl Window
  GLint                  getHeight() const { return myHeight; }
  
  //! Returns scales on OpenGL scene along 3 directions
  /*! in 2d scene zScale = 1.0 */
  void                   getScale( GLfloat& xScale,
                                   GLfloat& yScale,
                                   GLfloat& zScale );
  //! A function for installing the scales of OpenGL scene
  void                   setScale( GLfloat xScale,
                                   GLfloat yScale,
                                   GLfloat zScaleGLfloat );
  
  //! Returns offset parameters of Window in OpenGL global scene
  void                   getPan( GLfloat& xPan, GLfloat& yPan, GLfloat& zPan );
  //! A function for installing the  offset parameters of Window in OpenGL global scene
  void                   setPan( GLfloat xPan, GLfloat yPan, GLfloat zPan );
  
  //! Returns rotation angle of Window in OpenGL global scene in degree
  /*! Only in 2D */
  GLfloat                getRotationAngle() const { return myRotationAnglePrev; }
  //! A function for installing the rotation angle of Window in OpenGL global scene in degree
  /*! Only in 2D */
  void                   setRotationAngle( GLfloat a ) { myRotationAnglePrev = a; }

  //! Returns start point of curren rotation of Window in OpenGL global scene
  void                   getRotationStart( GLfloat& rotationStartX,
                                           GLfloat& rotationStartY,
                                           GLfloat& rotationStartZ );
  //! A function for installing the start point of curren rotation of Window in OpenGL global scene
  void                   setRotationStart( GLfloat rotationStartX,
                                           GLfloat rotationStartY,
                                           GLfloat rotationStartZ );
  //! Returns parameters of current rotation
  void                   getRotation( GLfloat& rotationAngle,
                                      GLfloat& rotationCenterX,
                                      GLfloat& rotationCenterY,
                                      GLfloat& rotationCenterZ );
  //! A function for installing the parameters of current rotation
  void                   setRotation( GLfloat, GLfloat, GLfloat, GLfloat );

  //! A function load picture from file with name theFileName and post it in center of global OpenGL scene
  void                   setBackground( QString theFileName );
  
  //! A function add the tool tip with text theTTText on theTTRect rect to the widget window
  void                   addToolTip( QString theTTText, QRect theTTRect );
  //! A function remove tool tip form widget window
  void                   removeToolTip();
  
  //! A function translate background of window in to PostScript file on disk
  /*!
   *\param hFile     the name of PostScript file chosen by user
   *\param aViewerCS the GLViewer_CoordSystem of window
   *\param aPSCS     the GLViewer_CoordSystem of PostScript page
  */
  virtual void           translateBackgroundToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS );
  
  //! A function repaints OpenGL scene in export mode
  /* Repaints all objects in only current view */
  void                   exportRepaint();

#ifdef WIN32
  //! A function translate background of window in to EMF file on disk
  //! 
  /*!
   *\warning WIN32 only
   *
   *\param dc        the name of HDC associated with file chosen by user
   *\param aViewerCS the GLViewer_CoordSystem of window
   *\param aEMFCS    the GLViewer_CoordSystem of EMF page
  */
  virtual void           translateBackgroundToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif

private:
  //! Auxiliary function. Returns rect of window background in viewer coordinate system
  void                   getBackgroundRectInViewerCS( double& left, double& top, double& right, double& bottom );
  
protected:
  /* Redefined QT methods */
  //! A function is called before first display of window (create OpenGL scene)
  virtual void           initializeGL();
  //! A function is called in earch paint event of window
  /* Calling by public method repaint() */
  virtual void           paintGL();
  //! A function is called in earch resize event of window
  virtual void           resizeGL( int, int );
  
  
  virtual void           paintEvent( QPaintEvent* );
  virtual void           mouseMoveEvent( QMouseEvent* );
  virtual void           mousePressEvent( QMouseEvent* );
  virtual void           mouseReleaseEvent( QMouseEvent* );

  /* Needs to redefine because Window must be updated highlight presentation when mouse enter window */
  virtual void           enterEvent( QEvent* );
  /* Needs to redefine because Window must be updated highlight presentation when mouse leave window */
  virtual void           leaveEvent( QEvent* );

  virtual bool           event ( QEvent* );
  
private:
  //! width of window
  GLint                  myWidth;
  //! height of window
  GLint                  myHeight;
  
  //! Scale along X direction
  GLfloat                myXScale;
  //! Scale along Y direction
  GLfloat                myYScale;
  //! Scale along Z direction
  /* equals 1 in 2D */
  GLfloat                myZScale;

  //! Window offset along X direction
  GLfloat                myXPan;
  //! Window offset along Y direction
  GLfloat                myYPan;
  //! Window offset along Z direction
  /* equals 0 in 2D */
  GLfloat                myZPan;

  GLfloat                myRotationStartX;
  GLfloat                myRotationStartY;
  GLfloat                myRotationStartZ;
  GLfloat                myRotationAngle;
  GLfloat                myRotationCenterX;
  GLfloat                myRotationCenterY;
  GLfloat                myRotationCenterZ;
  GLfloat                myRotationAnglePrev;
  
  GLboolean              myStart;
  GLViewer_ViewPort2d*   myViewPort;
  
  //! True if background is loaded
  bool                   isLoadBackground;
  //! File name of background image
  QString                myBackgroundFile;
  //! Texture id of loaded background image
  GLuint                 texName;
  //! Width of background image
  int                    myIW;
  //! Height of background image
  int                    myIH;
  //! Size of background image
  int                    myBackgroundSize;

  QRect                  myToolTipRect;

  //! Needs for export repaint
  bool                   isExportMode;
};

#endif // GLVIEWER_WIDGET_H
