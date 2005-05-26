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
class GLViewer_Text;
class GLViewer_AspectLine;
class GLViewer_CoordSystem;

#ifdef WNT
#pragma warning( disable:4251 )
#endif

struct GLVIEWER_EXPORT GLViewer_TexIdStored
{
    GLuint      myTexFontId;
    int         myTexFontWidth;
    int         myTexFontHeight;
};

struct GLVIEWER_EXPORT GLViewer_TexFindId
{
    QString     myFontString;
    int         myViewPortId;
    bool operator < (const GLViewer_TexFindId theStruct) const 
    { 
      if ( myViewPortId != theStruct.myViewPortId ) return myViewPortId < theStruct.myViewPortId; 
      else return myFontString < theStruct.myFontString;
    }
};

class GLVIEWER_EXPORT GLViewer_TexFont
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
class GLVIEWER_EXPORT GLViewer_Drawer: public QObject
{
    Q_OBJECT
    
public:
    GLViewer_Drawer();
    virtual ~GLViewer_Drawer();
  
    enum ObjectStatus { OS_Normal = 0, OS_Highlighted = 1, OS_Selected = 2 };
    enum ClosedStatus { CS_CLOSED = 0, CS_OPEN = 1 };  
    
    virtual void                    create( float, float, bool ) = 0;  
    
    virtual void                    addObject( GLViewer_Object* theObject ){ myObjects.append( theObject ); }
    virtual void                    clear(){ myObjects.clear(); }
    
    QString                         getObjectType(){ return myObjectType; }
    
    static void                     destroyAllTextures();
    
    virtual bool                    translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );
    virtual bool                    translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS ); 
  
#ifdef WIN32
    virtual bool                    translateToEMF( HDC hDC, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif
    
    void                            drawText( const QString& text,
					      GLfloat xPos, GLfloat yPos,
					      const QColor& color,
					      QFont* aFont, int,
					      DisplayTextFormat = DTF_BITMAP );
 protected:
    virtual void                    drawText( GLViewer_Object* );
    
    float                           myXScale;
    float                           myYScale;
    
    QValueList<GLViewer_Object*>    myObjects;
    GLuint                          myTextList;
    
    QString                         myObjectType;
};

/***************************************************************************
**  Class:   GLViewer_MarkerDrawer
**  Descr:   Drawer for GLViewer_MarkerSet
**  Module:  GLViewer
**  Created: UI team, 03.10.01
****************************************************************************/
#ifndef GLVIEWER_MARKERDRAWER_H
#define GLVIEWER_MARKERDRAWER_H

class GLVIEWER_EXPORT GLViewer_MarkerDrawer : public GLViewer_Drawer  
{
public:
    GLViewer_MarkerDrawer();
    ~GLViewer_MarkerDrawer();
    
    virtual void       create( float, float, bool );
    
private:
    void               drawMarker( float&, float&, float&, QColor&, GLViewer_AspectLine* );
};

#endif // GLVIEWER_MARKERDRAWER_H

/***************************************************************************
**  Class:   GLViewer_PolylineDrawer
**  Descr:   Drawer for GLViewer_Polyline
**  Module:  GLViewer
**  Created: UI team, 03.10.01
****************************************************************************/
#ifndef GLVIEWER_POLYLINEDRAWER_H
#define GLVIEWER_POLYLINEDRAWER_H

class GLVIEWER_EXPORT GLViewer_PolylineDrawer : public GLViewer_Drawer  
{
public:
    GLViewer_PolylineDrawer();
    ~GLViewer_PolylineDrawer();
    
    virtual void       create( float, float, bool );    
};

#endif // GLVIEWER_POLYLINEDRAWER_H

/***************************************************************************
**  Class:   GLViewer_TextDrawer
**  Descr:   
**  Module:  GLViewer
**  Created: UI team, 27.02.04
****************************************************************************/
#ifndef GLVIEWER_TEXTDRAWER
#define GLVIEWER_TEXTDRAWER

class GLVIEWER_EXPORT GLViewer_TextDrawer: public GLViewer_Drawer
{
  
public:
    GLViewer_TextDrawer();
    ~GLViewer_TextDrawer();
    
    virtual void              create( float, float, bool );
    void                      updateObjects(); //after update font
};

#endif // GLVIEWER_TEXTDRAWER

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif // GLVIEWER_DRAWER_H
