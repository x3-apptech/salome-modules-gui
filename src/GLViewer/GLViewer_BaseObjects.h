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
// File:      GLViewer_BaseObjects.h
// Created:   November, 2004
//
#ifndef GLVIEWER_BASEOBJECTS_H
#define GLVIEWER_BASEOBJECTS_H

#include "GLViewer.h"
#include "GLViewer_Object.h"

#include <QColor>

#include <TColStd_SequenceOfInteger.hxx>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*! 
 \class GLViewer_MarkerSet
 OpenGL MarkerSet
*/
class GLVIEWER_API GLViewer_MarkerSet : public GLViewer_Object
{
public:
  //! A constructor
  GLViewer_MarkerSet( int number = 1, float size = 5.0, const QString& toolTip = "GLMarker" );
  //! A destructor
  ~GLViewer_MarkerSet();

  // redefined  methods
  virtual void             compute();
  virtual GLViewer_Drawer* createDrawer();
  
  virtual GLboolean        highlight( GLfloat x, GLfloat y, GLfloat tol = 15.0, GLboolean isCircle = GL_FALSE );
  virtual GLboolean        unhighlight();
  virtual GLboolean        select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect, GLboolean isFull = GL_FALSE,
    GLboolean isCircle = GL_FALSE, GLboolean isShift = GL_FALSE );
  virtual GLboolean        unselect();
  
  virtual GLViewer_Rect*   getUpdateRect();
  
  virtual void             moveObject( float, float, bool fromGroup = false );
  
  virtual QByteArray       getByteCopy();
  virtual bool             initializeFromByteCopy( QByteArray );
  
  virtual bool             translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS );
  virtual bool             translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );    
  
#ifdef WIN32
  virtual bool             translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif

  //! Sets array of x coords of markers
  void                     setXCoord( GLfloat* xCoord, int size );
  //! Sets array of y coords of markers
  void                     setYCoord( GLfloat* yCoord, int size );
  //! Returns x coords
  GLfloat*                 getXCoord() const { return myXCoord; }
  //! Returns y coords
  GLfloat*                 getYCoord() const { return myYCoord; }
  
  //! Sets markers number
  void                     setNumMarkers( GLint );
  //! returns markers number
  GLint                    getNumMarkers() const { return myNumber; };
  //! Sets merker radius
  void                     setMarkerSize( const float size ) { myMarkerSize = size; }
  //! Returns merker radius
  float                    getMarkerSize() const { return myMarkerSize; }
  
  //! Export numbers of heghlighted/selected markers
  void                     exportNumbers( QList<int>& high, QList<int>& unhigh,
                                          QList<int>& sel, QList<int>& unsel );
  
  //! Returns selected numbers
  QList<int>               getSelectedElements() { return mySelNumbers; }
  //! Adds or remove selected number
  bool                     addOrRemoveSelected( int index );
  //! Adds selected numbers
  void                     addSelected( const TColStd_SequenceOfInteger& );
  //! Sets selected nembers
  void                     setSelected( const TColStd_SequenceOfInteger& );

protected:
  GLint                   myNumber;
  GLfloat*                myXCoord;
  GLfloat*                myYCoord;    
  GLfloat                 myMarkerSize;
  QList<int>              myHNumbers;
  QList<int>              myUHNumbers;
  QList<int>              mySelNumbers;
  QList<int>              myCurSelNumbers;
  QList<int>              myUSelNumbers;
  QList<int>              myPrevHNumbers;
  TColStd_SequenceOfInteger mySelectedIndexes;
};

/*! 
 \class  GLViewer_Polyline
 OpenGL Polyline
*/
class GLVIEWER_API GLViewer_Polyline: public GLViewer_Object
{
public:
  GLViewer_Polyline( int number = 1, float size = 5.0, const QString& toolTip = "GLPolyline" );
 ~GLViewer_Polyline();
  
  // redefined  methods
  virtual void            compute();
  virtual GLViewer_Drawer* createDrawer();
  
  virtual GLboolean       highlight( GLfloat x, GLfloat y, GLfloat tol = 15.0, GLboolean isCircle = GL_FALSE );
  virtual GLboolean       unhighlight();
  virtual GLboolean       select( GLfloat x, GLfloat y, GLfloat tol,  GLViewer_Rect rect, GLboolean isFull = GL_FALSE,
    GLboolean isCircle = GL_FALSE, GLboolean isShift = GL_FALSE );
  virtual GLboolean       unselect();
  
  virtual GLViewer_Rect*  getUpdateRect();
  
  virtual void            moveObject( float, float, bool fromGroup = false );
  
  virtual QByteArray      getByteCopy();
  virtual bool            initializeFromByteCopy( QByteArray );
  
  virtual bool            translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS );
  virtual bool            translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );    
  
#ifdef WIN32
  virtual bool            translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif
  
  //! Sets array of x coords of points
  void                     setXCoord( GLfloat* xCoord, int size );
  //! Sets array of y coords of points
  void                     setYCoord( GLfloat* yCoord, int size );
  //! Returns x coords
  GLfloat*                 getXCoord() const { return myXCoord; }
  //! Returns y coords
  GLfloat*                 getYCoord() const { return myYCoord; }

  //! SetS number of points
  void                    setNumber( GLint );
  //! Returns number of points
  GLint                   getNumber() const { return myNumber; };
  
  //! On/off closed status of polyline
  void                    setClosed( GLboolean closed ) { myIsClosed = closed; }
  //! Checks closed status of polyline
  GLboolean               isClosed() const { return myIsClosed; }
  
  //! On/off highlight as whole object status
  void                    setHighSelAll( GLboolean highSelAll ) { myHighSelAll = highSelAll; }
  //! Checks highlight as whole object status
  GLboolean               isHighSelAll() const { return myHighSelAll; }
  
  //! Export numbers of highlighted/selected lines
  void                    exportNumbers( QList<int>& high, QList<int>& unhigh,
                                         QList<int>& sel, QList<int>& unsel );

  //! Returns numbers of selected lines
  QList<int>              getSelectedElements() { return mySelNumbers; }

protected:
  GLfloat*                myXCoord;
  GLfloat*                myYCoord;
  GLint                   myNumber;
  GLboolean               myIsClosed;
  GLboolean               myHighSelAll;
  
  QList<int>              myHNumbers;
  QList<int>              myUHNumbers;
  QList<int>              mySelNumbers;
  QList<int>              myUSelNumbers;
  QList<int>              myCurSelNumbers;
  QList<int>              myPrevHNumbers;
  TColStd_SequenceOfInteger mySelectedIndexes;
  
  GLboolean               myHighFlag;
};

/*!
  \class GLViewer_TextObject
  Text as Object for OpenGL
*/
class GLVIEWER_API GLViewer_TextObject : public GLViewer_Object
{
public:  
  GLViewer_TextObject( const QString&, float xPos = 0, float yPos = 0, 
    const QColor& color = QColor( 0, 255, 0 ), const QString& toolTip = "GLText" );
  ~GLViewer_TextObject();
  
  virtual void              compute();
  virtual GLViewer_Drawer*  createDrawer();
  
  virtual void              setDrawer( GLViewer_Drawer* theDrawer );
  
  virtual GLboolean         highlight( GLfloat x, GLfloat y, GLfloat tol, GLboolean isCircle = GL_FALSE );
  virtual GLboolean         unhighlight();
  virtual GLboolean         select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect, GLboolean isFull = GL_FALSE,
    GLboolean isCircle = GL_FALSE, GLboolean isShift = GL_FALSE );
  virtual GLboolean         unselect();
  
  virtual GLViewer_Rect*    getUpdateRect();
  
  virtual void              moveObject( float, float, bool fromGroup = false );
  
  virtual QByteArray        getByteCopy();
  virtual bool              initializeFromByteCopy( QByteArray );
  
  virtual bool              translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS );
  virtual bool              translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );  
  
#ifdef WIN32
  virtual bool              translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif
  
  int                       getWidth(){ return myWidth; }
  int                       getHeight(){ return myWidth; }
  void                      setWidth( int w ){ myWidth=w; }
  void                      setHeight( int h ){ myHeight=h; }
  
protected:
  bool                      myHighFlag;
  int                       myWidth;
  int                       myHeight;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
