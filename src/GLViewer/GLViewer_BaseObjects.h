// File:      GLViewer_BaseObjects.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_BASEOBJECTS_H
#define GLVIEWER_BASEOBJECTS_H

#include "GLViewer.h"
#include "GLViewer_Object.h"

#include <qcolor.h>
#include <qvaluelist.h>

#include <TColStd_SequenceOfInteger.hxx>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

// Class:   GLViewer_MarkerSet
// Descr:   OpenGL MarkerSet

class GLVIEWER_API GLViewer_MarkerSet : public GLViewer_Object
{
public:
  GLViewer_MarkerSet( int number = 1, float size = 5.0, const QString& toolTip = "GLMarker" );
  ~GLViewer_MarkerSet();
  
  virtual void            compute();
  virtual GLViewer_Drawer* createDrawer();
  
  virtual GLboolean       highlight( GLfloat x, GLfloat y, GLfloat tol = 15.0, GLboolean isCircle = GL_FALSE );
  virtual GLboolean       unhighlight();
  virtual GLboolean       select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect, GLboolean isFull = GL_FALSE,
    GLboolean isCircle = GL_FALSE, GLboolean isShift = GL_FALSE );
  virtual GLboolean       unselect();
  
  virtual GLViewer_Rect*  getUpdateRect();
  
  void                    setXCoord( GLfloat* xCoord, int size );
  void                    setYCoord( GLfloat* yCoord, int size );
  GLfloat*                getXCoord() const { return myXCoord; }
  GLfloat*                getYCoord() const { return myYCoord; }
  void                    setNumMarkers( GLint );
  GLint                   getNumMarkers() const { return myNumber; };
  
  void                    setMarkerSize( const float size ) { myMarkerSize = size; }
  float                   getMarkerSize() const { return myMarkerSize; }
  
  void                    exportNumbers( QValueList<int>&, QValueList<int>& , QValueList<int>&, QValueList<int>& );
  
  QValueList<int>         getSelectedElements() { return mySelNumbers; }
  bool                    addOrRemoveSelected( int index );
  void                    addSelected( const TColStd_SequenceOfInteger& );
  void                    setSelected( const TColStd_SequenceOfInteger& );
  
  virtual void            moveObject( float, float, bool fromGroup = false );
  
  virtual QByteArray      getByteCopy();
  virtual bool            initializeFromByteCopy( QByteArray );
  
  virtual bool            translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS );
  virtual bool            translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );    
  
#ifdef WIN32
  virtual bool            translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif

protected:
  GLint                   myNumber;
  GLfloat*                myXCoord;
  GLfloat*                myYCoord;    
  GLfloat                 myMarkerSize;
  QValueList<int>         myHNumbers;
  QValueList<int>         myUHNumbers;
  QValueList<int>         mySelNumbers;
  QValueList<int>         myCurSelNumbers;
  QValueList<int>         myUSelNumbers;
  QValueList<int>         myPrevHNumbers;
  TColStd_SequenceOfInteger mySelectedIndexes;
};

// Class:   GLViewer_Polyline
// Descr:   OpenGL Polyline

class GLVIEWER_API GLViewer_Polyline: public GLViewer_Object
{
public:
  GLViewer_Polyline( int number = 1, float size = 5.0, const QString& toolTip = "GLPolyline" );
  ~GLViewer_Polyline();
  
  virtual void            compute();
  virtual GLViewer_Drawer* createDrawer();
  
  virtual GLboolean       highlight( GLfloat x, GLfloat y, GLfloat tol = 15.0, GLboolean isCircle = GL_FALSE );
  virtual GLboolean       unhighlight();
  virtual GLboolean       select( GLfloat x, GLfloat y, GLfloat tol,  GLViewer_Rect rect, GLboolean isFull = GL_FALSE,
    GLboolean isCircle = GL_FALSE, GLboolean isShift = GL_FALSE );
  virtual GLboolean       unselect();
  
  virtual GLViewer_Rect*  getUpdateRect();
  
  void                    setXCoord( GLfloat* xCoord, int size );
  void                    setYCoord( GLfloat* yCoord, int size );
  GLfloat*                getXCoord() const { return myXCoord; }
  GLfloat*                getYCoord() const { return myYCoord; }
  void                    setNumber( GLint );
  GLint                   getNumber() const { return myNumber; };
  
  void                    setClosed( GLboolean closed ) { myIsClosed = closed; }
  GLboolean               isClosed() const { return myIsClosed; }
  
  void                    setHighSelAll( GLboolean highSelAll ) { myHighSelAll = highSelAll; }
  GLboolean               isHighSelAll() const { return myHighSelAll; }
  
  void                    exportNumbers( QValueList<int>&, QValueList<int>& , QValueList<int>&, QValueList<int>& );
  
  QValueList<int>         getSelectedElements() { return mySelNumbers; }
  
  virtual void            moveObject( float, float, bool fromGroup = false );
  
  virtual QByteArray      getByteCopy();
  virtual bool            initializeFromByteCopy( QByteArray );
  
  virtual bool            translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS );
  virtual bool            translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );    
  
#ifdef WIN32
  virtual bool            translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif

protected:
  GLfloat*                myXCoord;
  GLfloat*                myYCoord;
  GLint                   myNumber;
  GLboolean               myIsClosed;
  GLboolean               myHighSelAll;
  
  QValueList<int>         myHNumbers;
  QValueList<int>         myUHNumbers;
  QValueList<int>         mySelNumbers;
  QValueList<int>         myUSelNumbers;
  QValueList<int>         myCurSelNumbers;
  QValueList<int>         myPrevHNumbers;
  TColStd_SequenceOfInteger mySelectedIndexes;
  
  GLboolean               myHighFlag;
};

// Class:   GLViewer_TextObject
// Descr:   Text as Object for OpenGL

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

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
