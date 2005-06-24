// File:      GLViewer_Drawer.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/***************************************************************************
**  Class:   GLViewer_Drawer
**  Descr:   Drawer for GLViewer_Object
**  Module:  GLViewer
**  Created: UI team, 01.10.01
****************************************************************************/
#ifndef GLVIEWER_DRAWER_H
#define GLVIEWER_DRAWER_H

#ifdef WNT
#include "windows.h"
#endif

#include <qcolor.h>
#include <qobject.h>
#include <qfile.h>
#include <qfont.h>
#include <qgl.h>

#include <GL/gl.h>

#include "GLViewer.h"
#include "GLViewer_Defs.h"

class GLViewer_Object;
class GLViewer_Rect;
class GLViewer_CoordSystem;

#ifdef WNT
#pragma warning( disable:4251 )
#endif

struct GLVIEWER_API GLViewer_TexIdStored
{
  GLuint      myTexFontId;
  int         myTexFontWidth;
  int         myTexFontHeight;
};

struct GLVIEWER_API GLViewer_TexFindId
{
  QString     myFontString;
  int         myViewPortId;
  bool operator < (const GLViewer_TexFindId theStruct) const 
  { 
    if ( myViewPortId != theStruct.myViewPortId ) return myViewPortId < theStruct.myViewPortId; 
    else return myFontString < theStruct.myFontString;
  }
};

class GLVIEWER_API GLViewer_TexFont
{
public:
  GLViewer_TexFont();
  GLViewer_TexFont( QFont* theFont, int theSeparator = 2 );
  ~GLViewer_TexFont();
  
  void            generateTexture();
  void            drawString( QString theStr, GLdouble theX = 0.0, GLdouble theY = 0.0 );
  
  int             getSeparator(){ return mySeparator; }
  void            setSeparator( int theSeparator ){ mySeparator = theSeparator; }
  
  int             getStringWidth( QString );
  int             getStringHeight();
  
  static  QMap<GLViewer_TexFindId,GLViewer_TexIdStored> TexFontBase;
  static  int         LastmyTexStoredId;
  
protected:
  int*            myWidths;
  int*            myPositions;
  QFont           myQFont;
  GLuint          myTexFont;
  int             myTexFontWidth;
  int             myTexFontHeight;
  int             mySeparator;
};

/***************************************************************************
**  Class:   GLViewer_Drawer
**  Descr:   Drawer for GLObject
**  Module:  GLViewer
**  Created: UI team, 03.10.01
****************************************************************************/
class GLVIEWER_API GLViewer_Drawer
{
public:
    enum { GLText_Center = 0, GLText_Left, GLText_Right, GLText_Top, GLText_Bottom };

public:
  GLViewer_Drawer();
  virtual ~GLViewer_Drawer();
  
  enum ObjectStatus { OS_Normal = 0, OS_Highlighted = 1, OS_Selected = 2 };
  enum ClosedStatus { CS_CLOSED = 0, CS_OPEN = 1 };  
  
  virtual void                    create( float, float, bool ) = 0;  
  
  virtual void                    addObject( GLViewer_Object* theObject ){ myObjects.append( theObject ); }
  virtual void                    clear(){ myObjects.clear(); }
  
  QString                         getObjectType() const { return myObjectType; }
  int                             getPriority() const { return myPriority; }
  
  static void                     destroyAllTextures();
  
  virtual bool                    translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );
  virtual bool                    translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS ); 
  
#ifdef WIN32
  virtual bool                    translateToEMF( HDC hDC, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif
  
  GLuint                          loadTexture( const QString& fileName );
  void                            drawTexture( GLuint texture, GLint size, GLfloat x, GLfloat y );

  void                            drawText( const QString& text,
                                            GLfloat xPos, GLfloat yPos,
                                            const QColor& color,
                                            QFont* aFont, int,
                                            DisplayTextFormat = DTF_BITMAP );

  void                            drawGLText( QString text, float x, float y,
                                              int hPosition = GLText_Center, int vPosition = GLText_Center,
                                              QColor color = Qt::black, bool smallFont = false );

  static void                     drawRectangle( GLViewer_Rect*, QColor = Qt::black );

protected:
  virtual void                    drawText( GLViewer_Object* );

  float                           myXScale;
  float                           myYScale;
  
  QValueList<GLViewer_Object*>    myObjects;
  GLuint                          myTextList;
  
  QString                         myObjectType;
  int                             myPriority;
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif // GLVIEWER_DRAWER_H
