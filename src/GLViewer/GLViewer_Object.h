// File:      GLViewer_Object.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_OBJECT_H
#define GLVIEWER_OBJECT_H

#ifdef WNT
#include <windows.h>
#endif

#include "GLViewer.h"
#include "GLViewer_Defs.h"
#include "GLViewer_Geom.h"

#include <SUIT_DataOwner.h>

#include <GL/gl.h>

#include <qfile.h>
#include <qstring.h>
#include <qrect.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

// Class:   GLViewer_Object
// Descr:   OpenGL Object

class GLViewer_Drawer;
class GLViewer_AspectLine;
class GLViewer_Group;
class GLViewer_CoordSystem;
class GLViewer_Text;
class GLViewer_Owner;

class GLVIEWER_API GLViewer_Object
{
public:
  GLViewer_Object();
  virtual ~GLViewer_Object();
  
  virtual void              compute() = 0;
  virtual GLViewer_Drawer*  createDrawer() = 0;

  virtual void              setDrawer( GLViewer_Drawer* theDrawer ) { myDrawer = theDrawer; }
  GLViewer_Drawer*          getDrawer() const { return myDrawer; }
  
  virtual GLboolean         highlight( GLfloat x, GLfloat y, GLfloat tol, GLboolean isCircle = GL_FALSE ) = 0;
  virtual GLboolean         unhighlight() = 0;
  virtual GLboolean         select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect,
                                    GLboolean isFull = GL_FALSE, GLboolean isCircle = GL_FALSE, GLboolean isShift = GL_FALSE ) = 0;
  virtual GLboolean         unselect() = 0;
  
  virtual GLboolean         isInside( GLViewer_Rect );
  
  virtual bool              portContains( GLViewer_Pnt ) { return false; }
  virtual bool              startPulling( GLViewer_Pnt ) { return false; }
  virtual void              pull( GLViewer_Pnt, GLViewer_Object* ) {}
  virtual void              finishPulling() {}
  virtual bool              isPulling() { return false; }
  virtual GLViewer_Rect     getPullingRect() const { return GLViewer_Rect(
                                myRect->left(), myRect->right(), myRect->top(), myRect->bottom() ); }
  
  virtual void              setRect( GLViewer_Rect* rect) { myRect = rect; }
  virtual GLViewer_Rect*    getRect() const { return myRect; }
  virtual GLViewer_Rect*    getUpdateRect() = 0;
  
  virtual void              setScale( GLfloat xScale, GLfloat yScale ) { myXScale = xScale; myYScale = yScale; }
  virtual void              getScale( GLfloat& xScale, GLfloat& yScale ) const { xScale = myXScale; yScale = myYScale;}

  virtual GLboolean         setZoom( GLfloat zoom, bool recompute, bool fromGroup = false );
  virtual GLfloat           getZoom() const { return myZoom; }
  virtual GLboolean         updateZoom( bool zoomIn );
  
  virtual GLboolean         isHighlighted() const { return myIsHigh; }
  virtual GLboolean         isSelected() const { return myIsSel; }  
  virtual void              setSelected( GLboolean state ) { myIsSel = state; }
  
  void                      setGLText( GLViewer_Text* glText ) { myGLText = glText; }
  GLViewer_Text*            getGLText() const { return myGLText; }
  
  virtual void              setAspectLine ( GLViewer_AspectLine* aspect ) { myAspectLine = aspect; }
  virtual GLViewer_AspectLine* getAspectLine() const { return myAspectLine; }
  
  QString                   getObjectType() const { return myType; } 
  
  void                      setName( QString name ) { myName = name; } 
  QString                   getName() const { return myName; } 
  
  virtual int               getPriority() const;

  virtual void              moveObject( float, float, bool fromGroup = false ) = 0;
  virtual bool              finishMove() { return true; }
  
  bool                      getVisible() const { return myIsVisible; }
  virtual void              setVisible( bool theStatus ) { myIsVisible = theStatus; }
  
  void                      setToolTipText( QString str ){ myToolTipText = str; }
  virtual QString           getToolTipText(){ return myToolTipText; }
  
  bool                      isTooTipHTML() const { return isToolTipHTML; }
  void                      setToolTipFormat( bool isHTML ) { isToolTipHTML = isHTML; }
  
  virtual QByteArray        getByteCopy();
  virtual bool              initializeFromByteCopy( QByteArray );
  
  virtual bool              translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS ) = 0;
  virtual bool              translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS ) = 0;  
  
#ifdef WIN32
  virtual bool              translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS ) = 0;
#endif

  GLViewer_Owner*           owner() const { return myOwner; }
  void                      setOwner( GLViewer_Owner* owner ) { myOwner = owner; }
  
  void                      setGroup( GLViewer_Group* );
  GLViewer_Group*           getGroup() const;

  virtual GLViewer_Object*  getOwner() { return this; }

  virtual bool              isSelectable() { return true; }
  virtual bool              isScalable() { return true; }
  
protected:
  QString                   myName;
  QString                   myType;

  GLViewer_Rect*            myRect;
  GLViewer_Rect*            myUpdateRect;
  GLViewer_Text*            myGLText;

  GLfloat                   myXScale;
  GLfloat                   myYScale;
  GLfloat                   myXGap;
  GLfloat                   myYGap;

  GLfloat                   myZoom;

  GLboolean                 myIsHigh;
  GLboolean                 myIsSel;
  
  GLViewer_Drawer*          myDrawer;
  GLViewer_AspectLine*      myAspectLine;
  
  QString                   myToolTipText;
  bool                      isToolTipHTML;

  bool                      myIsVisible;

  GLViewer_Owner*           myOwner;
  GLViewer_Group*           myGroup;
};

// Class:   GLViewer_Object
// Descr:   OpenGL Object

class GLVIEWER_API GLViewer_Owner : public SUIT_DataOwner
{
public:
  GLViewer_Owner() : SUIT_DataOwner() {}
  ~GLViewer_Owner() {}

protected:

};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
