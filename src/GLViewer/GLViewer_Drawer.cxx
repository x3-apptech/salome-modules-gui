// File:      GLViewer_Drawer.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/***************************************************************************
**  Class:   GLViewer_Drawer
**  Descr:   Drawer for GLViewer_Object
**  Module:  GLViewer
**  Created: UI team, 01.10.01
****************************************************************************/

#include "GLViewer_Drawer.h"
#include "GLViewer_Object.h"
#include "GLViewer_ViewFrame.h"
#include "GLViewer_ViewPort2d.h"

//#include "OSD_Timer.hxx"

#ifndef WIN32
#include <GL/glx.h>
#endif

//#include <qdir.h>
//-----------
#include <qpixmap.h>
#include <qimage.h>
#include <qfontmetrics.h>
#include <qpainter.h>
//-----------

#include <string>
#include <utility>
//#include "QAD_TMFont.h"
//using namespace qad_tmfont;

static int FirstSymbolNumber = 32;
static int LastSymbolNumber = 127;
//int GLViewer_TexFont::LastmyTexStoredId = 0;
QMap<GLViewer_TexFindId,GLViewer_TexIdStored> GLViewer_TexFont::TexFontBase; 

// Next line should be commented, if tex-mapped fonts are completely well
// and should be used instead of bitmap ones
#define DEB_TEX_FONT

/***************************************************************************
**  Class:   GLViewer_TexFont
**  Descr:   Texture Font for GLViewer_Object
**  Module:  GLViewer
**  Created: UI team, 01.10.01
****************************************************************************/
GLViewer_TexFont::GLViewer_TexFont()
{
    myQFont = QFont::defaultFont();
    QFontMetrics aFM( myQFont );        
    myWidths = new int[LastSymbolNumber - FirstSymbolNumber+1];
    myPositions = new int[LastSymbolNumber - FirstSymbolNumber+1];
    mySeparator = 2;
    for( int k = FirstSymbolNumber, aWidth = 0; k <= LastSymbolNumber; k++ )
    {
      //char aLetter = (char)k;
        myWidths[ k - FirstSymbolNumber ] = aFM.width( k );
        myPositions[ k - FirstSymbolNumber ] = aWidth;
        aWidth += myWidths[ k - FirstSymbolNumber ] + 2;//mySeparator;
    }

    myTexFontWidth = 0;
    myTexFontHeight = 0;        
}

GLViewer_TexFont::GLViewer_TexFont( QFont* theFont, int theSeparator )
{
    myQFont = *theFont;
    QFontMetrics aFM( myQFont );        
    myWidths = new int[LastSymbolNumber - FirstSymbolNumber+1];
    myPositions = new int[LastSymbolNumber - FirstSymbolNumber+1];
    mySeparator = theSeparator;
    for( int k = FirstSymbolNumber, aWidth = 0; k <= LastSymbolNumber; k++ )
    {
      //char aLetter = (char)k;
        myWidths[ k - FirstSymbolNumber ] = aFM.width( k );
        myPositions[ k - FirstSymbolNumber ] = aWidth;
        aWidth += myWidths[ k - FirstSymbolNumber ] + 2;//mySeparator;
    }

    myTexFontWidth = 0;
    myTexFontHeight = 0;
    
}

GLViewer_TexFont::~GLViewer_TexFont()
{
    //delete myQFont;
    delete[] myWidths;
    delete[] myPositions;
    //glDeleteTextures( 1, &myTexFont );
}   

void GLViewer_TexFont::generateTexture()
{
    QFontMetrics aFM( myQFont );
    //QString aFontStr = myQFont.toString();
    //QGLContext aContext = QGLContext::currentContext();

    GLViewer_TexFindId aFindFont;
    aFindFont.myFontString = myQFont.toString();
    aFindFont.myViewPortId = (int)QGLContext::currentContext();
        
    if( TexFontBase.contains( aFindFont ) )
    {
        GLViewer_TexIdStored aTexture = TexFontBase[ aFindFont ];
        myTexFont = aTexture.myTexFontId;
        myTexFontWidth = aTexture.myTexFontWidth;
        myTexFontHeight = aTexture.myTexFontHeight;
        //cout << "No generating " <<  myTexFont << "; current context " <<  QGLContext::currentContext()<< endl;
    }    
    else    
    {
        //cout << "Is generating! current context " <<  QGLContext::currentContext() << endl;
        
        QString aStr;
        int pixelsWidth = 0;
        int pixelsHight = aFM.height();
        myTexFontWidth = 64;
        myTexFontHeight = 64;
    
        pixelsWidth = myWidths[LastSymbolNumber - FirstSymbolNumber] + 
                      myPositions[LastSymbolNumber - FirstSymbolNumber];

        while( myTexFontWidth < pixelsWidth )
            myTexFontWidth = myTexFontWidth * 2;
        while( myTexFontHeight < pixelsHight )
            myTexFontHeight = myTexFontHeight * 2;

        QPixmap aPixmap( myTexFontWidth, myTexFontHeight );
        aPixmap.fill( QColor( 0, 0, 0) );
        QPainter aPainter( &aPixmap );
        aPainter.setFont( myQFont );
	for( int l = 0/*, gap = 0*/; l < LastSymbolNumber - FirstSymbolNumber; l++  )
        {
            QString aLetter;
            aLetter += (char)(FirstSymbolNumber + l);
            aPainter.setPen( QColor( 255,255,255) );
            aPainter.drawText ( /*gap*/myPositions[l], pixelsHight, aLetter );
            //gap += myWidths[l] + 2;
        }
    
        QImage aImage = aPixmap.convertToImage();
        //aImage.save( "c:/work/CATHARE/pic.bmp", "BMP");

        char* pixels = new char[myTexFontWidth * myTexFontHeight * 2];

        for( int i = 0; i < myTexFontHeight; i++ )
        {            
            for( int j = 0; j < myTexFontWidth;  j++ )
            {
                int aRed = qRed( aImage.pixel( j, myTexFontHeight - i - 1 ) );
                int aGreen = qGreen( aImage.pixel( j, myTexFontHeight - i - 1 ) );
                int aBlue = qBlue( aImage.pixel( j, myTexFontHeight - i - 1 ) );
          
                if( aRed != 0 || aGreen != 0 || aBlue != 0 )
                {
                    pixels[i * myTexFontWidth * 2 + j * 2] = (GLubyte)( (aRed + aGreen + aBlue)/3 );
                    pixels[i * myTexFontWidth * 2 + j * 2 + 1]= (GLubyte) 255;
                }
                else
                {
                    pixels[i * myTexFontWidth * 2 + j * 2] = (GLubyte) 0;
                    pixels[i * myTexFontWidth * 2 + j * 2 + 1]= (GLubyte) 0;
                }                
            }
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &myTexFont);
        glBindTexture(GL_TEXTURE_2D, myTexFont);  
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D(GL_TEXTURE_2D, 0, 2, myTexFontWidth,
            myTexFontHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pixels);
    
        delete[] pixels;
        
        GLViewer_TexIdStored aTexture;
        aTexture.myTexFontId = myTexFont;
        aTexture.myTexFontWidth = myTexFontWidth;
        aTexture.myTexFontHeight = myTexFontHeight;

        TexFontBase.insert( aFindFont, aTexture );
    }
}

void GLViewer_TexFont::drawString( QString theStr, GLdouble theX , GLdouble theY )
{
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPixelTransferi(GL_MAP_COLOR, 0);
    glAlphaFunc(GL_GEQUAL, 0.5F);
    glEnable(GL_ALPHA_TEST);
    glBindTexture(GL_TEXTURE_2D, myTexFont);
    glBegin(GL_QUADS);

    QFontMetrics aFM( myQFont );
    int pixelsHeight = aFM.height();

    theY = theY - myTexFontHeight + pixelsHeight;

    for( int i = 0, aGap = 0; i < theStr.length(); i++ )
    {
        char aLetter = theStr.data()[i];
        int aLettIndex = (int)aLetter - FirstSymbolNumber;

        float aLettBegin = (float)myPositions[aLettIndex];
        float aLettEnd = aLettBegin + myWidths[aLettIndex]-1;

        aLettBegin = aLettBegin / myTexFontWidth;
        aLettEnd = aLettEnd / myTexFontWidth;

        glTexCoord2f( aLettBegin, 0.0 ); glVertex3f( theX + aGap, theY, 1.0 );
        glTexCoord2f( aLettBegin, 1.0 ); glVertex3f( theX + aGap, theY + myTexFontHeight, 1.0 );
        glTexCoord2f( aLettEnd, 1.0 ); glVertex3f( theX + aGap + myWidths[aLettIndex]-1, theY + myTexFontHeight, 1.0 );
        glTexCoord2f( aLettEnd, 0.0 ); glVertex3f( theX + aGap + myWidths[aLettIndex]-1, theY, 1.0 );

        aGap += myWidths[aLettIndex]-1 + mySeparator;
    }

    //cout << "the down:" << theY << endl;
    //cout << "the top:" << theY + myTexFontHeight << endl;
    //cout << "the text height: " <<  pixelsHeight << endl;


    glEnd();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
}

int GLViewer_TexFont::getStringWidth( QString theStr )
{
    int aWidth = 0;
    for( int i = 0; i < theStr.length(); i ++ )
    {
        char aLetter = theStr.data()[i];
        int aLettIndex = (int)aLetter - FirstSymbolNumber;
        aWidth += myWidths[aLettIndex] + mySeparator;
    }

    return aWidth;
}

int GLViewer_TexFont::getStringHeight()
{
    QFontMetrics aFM( myQFont );
    return aFM.height();
}

void GLViewer_Drawer::destroyAllTextures()
{
    //int aCount = GLViewer_TexFont::TexFontBase.count();
    QMap<GLViewer_TexFindId,GLViewer_TexIdStored>::Iterator anIt= GLViewer_TexFont::TexFontBase.begin();
    QMap<GLViewer_TexFindId,GLViewer_TexIdStored>::Iterator anEndIt= GLViewer_TexFont::TexFontBase.end();

    for( ; anIt != anEndIt; anIt++ )
        glDeleteTextures( 1, &(anIt.data().myTexFontId) );
}

#define TEXTURE

#define TEXT_GAP    5

#ifdef TEXTURE
//QAD_TMFont myTMFont;
    GLboolean TFLoaded = GL_FALSE;

    GLdouble        modelMatrix[16], projMatrix[16];
    GLint           viewport[4];
    GLdouble        winx, winy, winz;
    GLint           status;

    GLViewer_TexFont*  staticGlFont;

void printOrtho(GLdouble x, GLdouble y, const char *string, int set, GLdouble w, GLdouble h)
{
//  int width, heigth;
    if (set>1)
    {
        set=1;
    }
//d glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,w,0,h,-100,100);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

/* //--------abd 
    myTMFont.Begin();
    myTMFont.DrawString( string, x, y );
  //--------abd */
    staticGlFont->drawString( string, x, y );

/*  width = myTMFont.GetStringWidth( string );
    heigth = myTMFont.GetStringHeight( string );
    cout << "W=" << width << " H=" << heigth << endl;
*/
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
//d glEnable(GL_DEPTH_TEST);
}

void loadGLfont(/*int scalar*/)
{
    if (!TFLoaded)
    {
/*  //-------abd 
        QString filename = getenv( "CSF_QADResources" );
        filename = filename + QDir::separator() + "times.tmf";
        if (!myTMFont.Create( filename, 19436))
        {
            cout << "!Texture loading error" << endl;
            return;
        }
 //-------abd */
        staticGlFont = new GLViewer_TexFont();
        staticGlFont->generateTexture();
        
        TFLoaded = GL_TRUE;
//!     BuildFont(size);
    }
}

static GLuint displayListBase( QFont* theFont )
{
  GLuint aList = 0;
  static QMap<GLViewer_TexFindId, GLuint> fontCache;
  GLViewer_TexFindId aFindFont;
  aFindFont.myFontString = theFont->toString();

#ifdef WIN32
  HGLRC ctx = ::wglGetCurrentContext();
  if ( !ctx )
    return aList;  
  
  aFindFont.myViewPortId = (int)ctx;

  if ( fontCache.contains( aFindFont ) )
    aList = fontCache[aFindFont];
  else
  {
    GLuint listBase = 0;
    QMap<GLViewer_TexFindId, GLuint>::iterator it = fontCache.begin();
    for ( ; it != fontCache.end(); ++it )
    {
      if ( it.key().myViewPortId == (int)ctx && it.data() > listBase )
        listBase = it.data();
    }
    listBase += 256;

    HDC glHdc = ::wglGetCurrentDC();
    ::SelectObject( glHdc, theFont->handle() );
    if ( !::wglUseFontBitmaps( glHdc, 0, 256, listBase ) )
      listBase = 0;
    aList = listBase;
    fontCache[aFindFont] = aList;
  }
#else //X Window
  Display* aDisp = glXGetCurrentDisplay();
  if( !aDisp )
  {
    printf( "Can't find current dislay\n" );
    return aList;
  }
  
  GLXContext aCont = glXGetCurrentContext();
  if( !aCont )
  {
    printf( "Can't find current context\n" );
    return aList;
  }

  aFindFont.myViewPortId = (int)aCont;

  if ( fontCache.contains( aFindFont ) )
    aList = fontCache[aFindFont];
  else
  {
    GLuint listBase = 0;
    QMap<GLViewer_TexFindId, GLuint>::iterator it = fontCache.begin();
    for ( ; it != fontCache.end(); ++it )
    {
      if ( it.key().myViewPortId == (int)aCont && it.data() > listBase )
        listBase = it.data();
    }
    listBase += 256;

    glXUseXFont( (Font)(theFont->handle()), 0, 256, listBase );

    aList = listBase;
    fontCache[aFindFont] = aList;
  }

#endif

  return aList;
}

/***************************************************************************
**  Class:   GLViewer_Drawer
**  Descr:   Drawer for GLViewer_Object
**  Module:  GLViewer
**  Created: UI team, 01.10.01
****************************************************************************/
GLViewer_Drawer::GLViewer_Drawer()
{
  myXScale = myYScale = 0.0;
  myObjects.clear();
  myTextList = -1;
  myObjectType = "GLViewer_Object";
}

GLViewer_Drawer::~GLViewer_Drawer()
{
  myObjects.clear();
  glDeleteLists( myTextList, 1 );
}

void GLViewer_Drawer::drawText( const QString& text, GLfloat xPos, GLfloat yPos,
                                const QColor& color, QFont* theFont, int theSeparator, DisplayTextFormat theFormat )
{
  if( theFormat == DTF_TEXTURE )
  {
    GLViewer_TexFont aTexFont( theFont, theSeparator );
    aTexFont.generateTexture();

    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);
    status = gluProject (xPos, yPos, 0, modelMatrix, projMatrix, viewport, &winx, &winy, &winz);

    glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_LIST_BIT );
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,viewport[2],0,viewport[3],-100,100);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f( ( GLfloat )color.red() / 255, 
               ( GLfloat )color.green() / 255, 
               ( GLfloat )color.blue() / 255 );

    aTexFont.drawString( text, winx, winy );

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
  }
  else if( theFormat == DTF_BITMAP )
  {
    glColor3f( ( GLfloat )color.red() / 255, 
               ( GLfloat )color.green() / 255, 
               ( GLfloat )color.blue() / 255 );
    glRasterPos2f( xPos, yPos );
    glListBase( displayListBase( theFont ) );
    glCallLists( text.length(), GL_UNSIGNED_BYTE, text.local8Bit().data() );
  }
}

#else //#ifdef BITMAP
GLuint fontOffset;

GLubyte space[] =
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

GLubyte letters[][13] = {
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18},
    {0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e},
    {0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3},
    {0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff}
};

void makeRasterFont()
{
  GLuint i, j;
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

  fontOffset = glGenLists( 128 );
  for ( i = 0, j = 'A'; i < 26; i++, j++ )
  {
    glNewList( fontOffset + j, GL_COMPILE );
    glBitmap( CHAR_W, CHAR_H, 0.0, 2.0, CHAR_W+WIDTH, 0.0, letters[i] );
    glEndList();
  }
  glNewList( fontOffset + ' ', GL_COMPILE );
  glBitmap( CHAR_W, CHAR_H, 0.0, 2.0, CHAR_W+WIDTH, 0.0, space );
  glEndList();
}

void GLViewer_Drawer::drawText( const QString& text, int xPos, int yPos, const QColor& color, QFont theFont, int theSeparator )
{
    glShadeModel( GL_FLAT );
    makeRasterFont();

    myTextList = glGenLists( 1 );
    glNewList( myTextList, GL_COMPILE );
    glColor3f( ( GLfloat )color.red() / 255, 
               ( GLfloat )color.green() / 255, 
               ( GLfloat )color.blue() / 255 );
    glRasterPos2i( xPos + TEXT_GAP / myXScale, yPos + TEXT_GAP / myYScale );
    printString( text );
    glEndList();

    if ( myTextList != -1 ) 
        glCallList( myTextList );
}
#endif //BITMAP

void GLViewer_Drawer::drawText( GLViewer_Object* theObject )
{
  if( !theObject )
    return;

  GLViewer_Text* aText = theObject->getGLText();
  if( !aText )
    return;

  GLfloat aPosX, aPosY;
  aText->getPosition( aPosX, aPosY );
  drawText( aText->getText(), aPosX, aPosY, aText->getColor(), &(aText->getFont()), aText->getSeparator(), aText->getDisplayTextFormat() );
}

bool GLViewer_Drawer::translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS )
{
    bool result = true;
    for( int i=0, n=myObjects.count(); i<n; i++ ) 
        result &= myObjects[i]->translateToHPGL( hFile, aViewerCS, aHPGLCS );
    return result;
}

bool GLViewer_Drawer::translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS )
{
    bool result = true;
    for( int i=0, n=myObjects.count(); i<n; i++ ) 
        result &= myObjects[i]->translateToPS( hFile, aViewerCS, aPSCS );
    return result;
}

#ifdef WIN32
bool GLViewer_Drawer::translateToEMF( HDC hDC, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS )
{
    bool result = true;
    for( int i=0, n=myObjects.count(); i<n; i++ ) 
        result &= myObjects[i]->translateToEMF( hDC, aViewerCS, aEMFCS );
    return result;
}
#endif

/***************************************************************************
**  Class:   GLViewer_MarkerDrawer
**  Descr:   Drawer for GLViewer_MarkerSet
**  Module:  GLViewer
**  Created: UI team, 03.10.01
****************************************************************************/
#define SEGMENTS   20
#define STEP       2 * PI / SEGMENTS

GLfloat sin_table[SEGMENTS];
GLfloat cos_table[SEGMENTS];

GLViewer_MarkerDrawer::GLViewer_MarkerDrawer()
: GLViewer_Drawer()
{
    GLfloat angle = 0.0;
    for ( int i = 0; i < SEGMENTS; i++ )
    {
        sin_table[i] = sin( angle );
        cos_table[i] = cos( angle );
        angle += float( STEP );
    }
    myTextList = 0;//-1; 
    myObjectType = "GLViewer_MarkerSet";
}

GLViewer_MarkerDrawer::~GLViewer_MarkerDrawer()
{
    glDeleteLists( myTextList, 1 );
}

void GLViewer_MarkerDrawer::create( float xScale, float yScale, bool onlyUpdate )
{
//  cout << "GLViewer_MarkerDrawer::create " << scaleX << " " << scaleY << endl;

    QValueList<int>::Iterator it;
    QValueList<int>::Iterator EndIt;
    QValueList<GLViewer_Object*>::Iterator anObjectIt = myObjects.begin();
    QValueList<GLViewer_Object*>::Iterator anEndObjectIt = myObjects.end();

    myXScale = xScale;
    myYScale = yScale;

    QColor colorN, colorH, colorS;

    GLViewer_MarkerSet* aMarkerSet = NULL;
    GLViewer_AspectLine* anAspectLine = NULL;

    for( ; anObjectIt != anEndObjectIt; anObjectIt++ )
    {
        aMarkerSet = ( GLViewer_MarkerSet* )(*anObjectIt);
        anAspectLine = aMarkerSet->getAspectLine();
        anAspectLine->getLineColors( colorN, colorH, colorS );

        float* aXCoord = aMarkerSet->getXCoord();
        float* anYCoord = aMarkerSet->getYCoord();
        float aRadius = aMarkerSet->getMarkerSize();

        QValueList<int> aHNumbers, anUHNumbers, aSelNumbers, anUSelNumbers;
        aMarkerSet->exportNumbers( aHNumbers, anUHNumbers, aSelNumbers, anUSelNumbers );

        if( onlyUpdate )
        {
            EndIt = anUHNumbers.end();
            for( it = anUHNumbers.begin(); it != EndIt; ++it )
            {
                drawMarker( aXCoord[*it], anYCoord[*it], aRadius, colorN, anAspectLine );
                //cout << "GLViewer_MarkerDrawer::create UH " << *it << endl;
            }

            EndIt = anUSelNumbers.end();
            for( it = anUSelNumbers.begin(); it != EndIt; ++it )
                drawMarker( aXCoord[*it], anYCoord[*it], aRadius, colorN, anAspectLine );

            EndIt = aSelNumbers.end();
            for( it = aSelNumbers.begin(); it != EndIt; ++it )
                drawMarker( aXCoord[*it], anYCoord[*it], aRadius, colorS, anAspectLine );

            EndIt = aHNumbers.end();
            for( it = aHNumbers.begin(); it != EndIt; ++it )
            {
                drawMarker( aXCoord[*it], anYCoord[*it], aRadius, colorH, anAspectLine );
                //cout << "GLViewer_MarkerDrawer::create H " << *it << endl;
            }
        }
        else
        {
            int aNumber = aMarkerSet->getNumMarkers();
            for( int i = 0; i < aNumber; i++ )
                drawMarker( aXCoord[i], anYCoord[i], aRadius, colorN, anAspectLine );

            EndIt = anUSelNumbers.end();
            for( it = anUSelNumbers.begin(); it != EndIt; ++it )
                drawMarker( aXCoord[*it], anYCoord[*it], aRadius, colorN, anAspectLine );

            EndIt = aSelNumbers.end();
            for( it = aSelNumbers.begin(); it != EndIt; ++it )
                drawMarker( aXCoord[*it], anYCoord[*it], aRadius, colorS, anAspectLine );
        }
        //float aXPos = 0, anYPos = 0;
        if( aMarkerSet->getGLText()->getText() != "" )
        {
            //float aXPos = 0, anYPos = 0;
            //aMarkerSet->getGLText()->getPosition( aXPos, anYPos );
            //drawText( aMarkerSet->getGLText()->getText(), aXPos, anYPos, colorN, &aMarkerSet->getGLText()->getFont(), aMarkerSet->getGLText()->getSeparator() );
            drawText( aMarkerSet );
        }
    }
}

void GLViewer_MarkerDrawer::drawMarker( float& theXCoord, float& theYCoord,
                                     float& theRadius, QColor& theColor, GLViewer_AspectLine* theAspectLine )
{
    glColor3f( ( GLfloat )theColor.red() / 255, 
               ( GLfloat )theColor.green() / 255, 
               ( GLfloat )theColor.blue() / 255 );

    glLineWidth( theAspectLine->getLineWidth() );

    if ( theAspectLine->getLineType() == 0 )
        glBegin( GL_LINE_LOOP );
    else
        glBegin( GL_LINE_STRIP);

    for ( int i = 0; i < SEGMENTS; i++ )
        glVertex2f( theXCoord + cos_table[i] * theRadius / myXScale,
                    theYCoord + sin_table[i] * theRadius / myYScale );
    glEnd();
}

/***************************************************************************
**  Class:   GLViewer_PolylineDrawer
**  Descr:   Drawer for GLViewer_Polyline
**  Module:  GLViewer
**  Created: UI team, 03.10.01
****************************************************************************/

GLViewer_PolylineDrawer::GLViewer_PolylineDrawer()
:GLViewer_Drawer()
{
  myTextList = 0;//-1; 
    myObjects.clear();
    myObjectType = "GLViewer_Polyline";
}

GLViewer_PolylineDrawer::~GLViewer_PolylineDrawer()
{
    glDeleteLists( myTextList, 1 );
}

void GLViewer_PolylineDrawer::create( float xScale, float yScale, bool onlyUpdate )
{
    QValueList<GLViewer_Object*>::Iterator aObjectIt = myObjects.begin();
    QValueList<GLViewer_Object*>::Iterator aObjectEndIt = myObjects.end();
    
    myXScale = xScale;
    myYScale = yScale;

    QColor color, colorN, colorH, colorS;
    GLViewer_AspectLine* anAspect = NULL;
    GLViewer_Polyline* aPolyline = NULL;
//    myAspectLine->getLineColors( colorN, colorH, colorS );
    for( ; aObjectIt != aObjectEndIt; aObjectIt++ )
    {
        anAspect = (*aObjectIt)->getAspectLine();
        aPolyline = (GLViewer_Polyline*)(*aObjectIt);


        anAspect->getLineColors( colorN, colorH, colorS );
        if( onlyUpdate )
        {
            if( aPolyline->isHighlighted() )
                color = colorH;
            else if( aPolyline->isSelected() )
                color = colorS;
            else
                color = colorN;
        }
        else
        {
            if( aPolyline->isSelected() )
                color = colorS;
            else
                color = colorN;
        }

        float* aXCoord = aPolyline->getXCoord();
        float* anYCoord = aPolyline->getYCoord();
        int aSize = aPolyline->getNumber();        

        glColor3f( ( GLfloat )color.red() / 255, 
                   ( GLfloat )color.green() / 255, 
                   ( GLfloat )color.blue() / 255 );

        glLineWidth( anAspect->getLineWidth() );

        if ( anAspect->getLineType() == 0 )
            glBegin( GL_LINE_LOOP );
        else
            glBegin( GL_LINE_STRIP);

        for( int i = 0; i < aSize ; i++ )
             glVertex2f( aXCoord[ i ], anYCoord[ i ] );        
 
        if( aPolyline->isClosed() )
            glVertex2f( aXCoord[ 0 ], anYCoord[ 0 ] );

        glEnd();       

        if( aPolyline->getGLText()->getText() != "" )
        {
	  //float aXPos = 0, anYPos = 0;
	  //aPolyline->getGLText()->getPosition( aXPos, anYPos );
	  //drawText( aPolyline->getGLText()->getText(), aXPos, anYPos, color, &aPolyline->getGLText()->getFont(), aPolyline->getGLText()->getSeparator() );
          drawText( aPolyline );
        }
    }
}

/***************************************************************************
**  Class:   GLViewer_TextDrawer
**  Descr:   
**  Module:  GLViewer
**  Created: UI team, 27.02.04
****************************************************************************/

GLViewer_TextDrawer::GLViewer_TextDrawer()
: GLViewer_Drawer()
{
    myTextList = 0;//-1; 
    myObjects.clear();
    myObjectType = "GLViewer_TextObject";
}

GLViewer_TextDrawer::~GLViewer_TextDrawer()
{
    glDeleteLists( myTextList, 1 );
}

void GLViewer_TextDrawer::create( float xScale, float yScale, bool onlyUpdate )
{
    QValueList<GLViewer_Object*>::Iterator aObjectIt = myObjects.begin();
    QValueList<GLViewer_Object*>::Iterator aObjectEndIt = myObjects.end();
    
    myXScale = xScale;
    myYScale = yScale;

    QColor color, colorN, colorH, colorS;
    GLViewer_AspectLine* anAspect = NULL;    
    GLViewer_TextObject* anObject = NULL;
    //float aXPos = 0, anYPos = 0;
    for( ; aObjectIt != aObjectEndIt; aObjectIt++ )
    {
        anObject = (GLViewer_TextObject*)(*aObjectIt);
        anAspect = anObject->getAspectLine();    

        anAspect->getLineColors( colorN, colorH, colorS );
        if( onlyUpdate )
        {
            if( anObject->isHighlighted() )
                color = colorH;
            else if( anObject->isSelected() )
                color = colorS;
            else
                color = colorN;
        }
        else
        {
            if( anObject->isSelected() )
                color = colorS;
            else
                color = colorN;
        }        
        
        //anObject->getGLText()->getPosition( aXPos, anYPos );
        //drawText( anObject->getGLText()->getText(), aXPos, anYPos, color, &(anObject->getGLText()->getFont()), anObject->getGLText()->getSeparator() );
        drawText( anObject );
    }
}

void GLViewer_TextDrawer::updateObjects()
{
    QValueList<GLViewer_Object*>::Iterator aObjectIt = myObjects.begin();
    QValueList<GLViewer_Object*>::Iterator aObjectEndIt = myObjects.end();
    for( ; aObjectIt != aObjectEndIt; aObjectIt++ )
        (*aObjectIt)->compute();
}
