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
// File:      GLViewer_Drawer.cxx
// Created:   November, 2004
//#include <GLViewerAfx.h>
//
#include "GLViewer_Drawer.h"
#include "GLViewer_Object.h"
#include "GLViewer_Text.h"
#include "GLViewer_ViewFrame.h"
#include "GLViewer_ViewPort2d.h"

#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QFile>

#ifndef WIN32
#include <GL/glx.h>
#endif

#include <gp_Pnt2d.hxx>

#define TEXT_GAP    5
// Two texture components for texmapped fonts: luminance and alpha
#define NB_TEX_COMP 2
// A font is split into rows each containing 32 characters
#define TEX_ROW_LEN 32
// Gap in pixels between two character rows in a font texture
#define TEX_ROW_GAP 2

GLfloat modelMatrix[16];


//! code of first font symbol
static int FirstSymbolNumber = 32;
//! code of last font symbol
static int LastSymbolNumber = 127;

QMap<GLViewer_TexFindId,GLViewer_TexIdStored> GLViewer_TexFont::TexFontBase;
QMap<GLViewer_TexFindId,GLuint>               GLViewer_TexFont::BitmapFontCache; 

/*!
  Clears all generated fonts
*/
void GLViewer_TexFont::clearTextBases()
{
  //cout << "Clear font map" << endl;
  TexFontBase.clear();
  BitmapFontCache.clear();
}

/*!
  Default constructor
*/
GLViewer_TexFont::GLViewer_TexFont()
: myMaxRowWidth( 0 ), myFontHeight( 0 )
{
    myQFont = QApplication::font();//QFont::defaultFont();
    mySeparator = 2;
    myIsResizeable = false;
    myMinMagFilter = GL_LINEAR;

    init();
}

/*!
  Constructor
  \param theFont         - a base font
  \param theSeparator    - separator between letters
  \param theIsResizeable - specifies whether text drawn by this object can be scaled along with the scene
  \param theMinMagFilter - min/mag filter, affects text sharpness
*/
GLViewer_TexFont::GLViewer_TexFont( QFont* theFont, int theSeparator, bool theIsResizeable, GLuint theMinMagFilter )
: myMaxRowWidth( 0 ), myFontHeight( 0 )
{
    myQFont = *theFont;
    mySeparator = theSeparator;
    myIsResizeable = theIsResizeable;
    myMinMagFilter = theMinMagFilter;

    init();
}

/*!
  Destructor
*/
GLViewer_TexFont::~GLViewer_TexFont()
{
    delete[] myWidths;
    delete[] myPositions;
} 

/*!
  Initializes font parameters
*/
void GLViewer_TexFont::init()
{
    myNbSymbols = LastSymbolNumber - FirstSymbolNumber + 1;

    // It is unsafe to draw all characters in a single row -
    // this leads to problems on some graphic cards with small GL_MAX_TEXTURE_SIZE.
    // So splitting the characters into rows each containing 32 characters (or less).
    // Assuming contant height of each row (64 pixels) to simplify texture mapping.
    // However, this can be improved if necessary.
    QFontMetrics aFM( myQFont ); 
    myFontHeight = aFM.height();
    
    myWidths    = new int[myNbSymbols];
    myPositions = new int[myNbSymbols];

    for( int i = 0, k = FirstSymbolNumber, aWidth = 0; i < myNbSymbols; i++, k++ )
    {
        // is it time to start a new row?
        if ( !( i % TEX_ROW_LEN ) )
        {
          if( aWidth > myMaxRowWidth )
            myMaxRowWidth = aWidth;
          aWidth = 0;
        }
        myWidths[i]    = aFM.width( k );
        myPositions[i] = aWidth;
        aWidth += myWidths[i] + 2;
    }

    myTexFontWidth  = 0;
    myTexFontHeight = 0;
}
  
/*!
  Generating font texture
*/
bool GLViewer_TexFont::generateTexture()
{
    GLViewer_TexFindId aFindFont;
    aFindFont.myFontFamily = myQFont.family();//myQFont.toString();
    aFindFont.myIsBold = myQFont.bold();
    aFindFont.myIsItal = myQFont.italic();
    aFindFont.myIsUndl = myQFont.underline();
    aFindFont.myPointSize = myQFont.pointSize();
    aFindFont.myViewPortId = size_t(QGLContext::currentContext());
        
    if( TexFontBase.contains( aFindFont ) )
    {
        GLViewer_TexIdStored aTexture = TexFontBase[ aFindFont ];
        myTexFont = aTexture.myTexFontId;
        myTexFontWidth = aTexture.myTexFontWidth;
        myTexFontHeight = aTexture.myTexFontHeight;
    }    
    else    
    {
        // Adding some pixels to have a gap between rows
        int aRowPixelHeight = myFontHeight + TEX_ROW_GAP;
        int aDescent = QFontMetrics( myQFont ).descent();

        int aNumRows = myNbSymbols / TEX_ROW_LEN;
        if ( myNbSymbols % TEX_ROW_LEN ) 
          aNumRows++;
        int pixelsHight = aNumRows * aRowPixelHeight;

        myTexFontWidth  = 64;
        myTexFontHeight = 64;

        while( myTexFontWidth < myMaxRowWidth )
            myTexFontWidth <<= 1;
        while( myTexFontHeight < pixelsHight )
            myTexFontHeight <<= 1;
        
        // Checking whether the texture dimensions for the requested font
        // do not exceed the maximum size supported by the OpenGL implementation
        int maxSize;
        glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxSize );
        if ( myTexFontWidth > maxSize || myTexFontHeight > maxSize )
          return false;

        QPixmap aPixmap( myTexFontWidth, myTexFontHeight );
        aPixmap.fill( QColor( 0, 0, 0) );
        QPainter aPainter( &aPixmap );
        aPainter.setFont( myQFont );
        int row;
        for( int l = 0; l < myNbSymbols; l++  )
        {
            row = l / TEX_ROW_LEN;
            QString aLetter;
            aLetter += (char)(FirstSymbolNumber + l);
            aPainter.setPen( QColor( 255,255,255) );
            aPainter.drawText( myPositions[l], ( row + 1 ) * aRowPixelHeight - aDescent, aLetter );
        }
    
        QImage aImage = aPixmap.toImage();

        //int qqq = 0;
        //if (qqq)
        //  aImage.save("w:\\work\\CATHARE\\texture.png", "PNG");

        char* pixels = new char[myTexFontWidth * myTexFontHeight * NB_TEX_COMP];

        for( int i = 0; i < myTexFontHeight; i++ )
        {            
            for( int j = 0; j < myTexFontWidth;  j++ )
            {
                int aRed = qRed( aImage.pixel( j, myTexFontHeight - i - 1 ) );
                int aGreen = qGreen( aImage.pixel( j, myTexFontHeight - i - 1 ) );
                int aBlue = qBlue( aImage.pixel( j, myTexFontHeight - i - 1 ) );
          
                if( aRed != 0 || aGreen != 0 || aBlue != 0 )
                {
                    pixels[i * myTexFontWidth * NB_TEX_COMP + j * NB_TEX_COMP] = (GLubyte)( (aRed + aGreen + aBlue)/3 );
                    pixels[i * myTexFontWidth * NB_TEX_COMP + j * NB_TEX_COMP + 1]= (GLubyte) 255;
                }
                else
                {
                    pixels[i * myTexFontWidth * NB_TEX_COMP + j * NB_TEX_COMP] = (GLubyte) 0;
                    pixels[i * myTexFontWidth * NB_TEX_COMP + j * NB_TEX_COMP + 1]= (GLubyte) 0;
                }                
            }
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &myTexFont);
        glBindTexture(GL_TEXTURE_2D, myTexFont);  
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, myMinMagFilter);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, myMinMagFilter);
        glTexImage2D(GL_TEXTURE_2D, 
                     0, 
                     GL_INTENSITY, 
                     myTexFontWidth,
                     myTexFontHeight, 
                     0, 
                     GL_LUMINANCE_ALPHA, 
                     GL_UNSIGNED_BYTE, 
                     pixels);
    
        delete[] pixels;
        
        GLViewer_TexIdStored aTexture;
        aTexture.myTexFontId = myTexFont;
        aTexture.myTexFontWidth = myTexFontWidth;
        aTexture.myTexFontHeight = myTexFontHeight;

        TexFontBase.insert( aFindFont, aTexture );
    }
    return true;
}

/*!
  Drawing string in viewer
  \param theStr - string to be drawn
  \param theX - X position
  \param theY - Y position
  \param theScale - scale coefficient
*/
void GLViewer_TexFont::drawString( QString theStr, GLdouble theX , GLdouble theY, GLfloat theScale )
{
    // Adding some pixels to have a gap between rows
    int aRowPixelHeight = myFontHeight + TEX_ROW_GAP;

    float aXScale = 1.f, aYScale = 1.f;
    if ( !myIsResizeable )
    {
      glGetFloatv (GL_MODELVIEW_MATRIX, modelMatrix);
      aXScale = modelMatrix[0];
      aYScale = modelMatrix[5];     
    } 
    else if ( theScale > 0.f )
    {
      aXScale = aXScale / theScale;
      aYScale = aYScale / theScale;
    }

    // store attributes
    glPushAttrib( GL_ENABLE_BIT | GL_TEXTURE_BIT );

    glEnable(GL_TEXTURE_2D);
    glPixelTransferi(GL_MAP_COLOR, 0);

    glAlphaFunc(GL_GEQUAL, 0.05F);
    glEnable(GL_ALPHA_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, myTexFont);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBegin(GL_QUADS);

    float aLettBeginS, aLettEndS, aLettBeginT, aLettEndT;
    float aDY = ( aRowPixelHeight - 1 ) / aYScale, aDX;
    char aLetter;
    int aLettIndex, row;
    for ( int i = 0; i < (int)theStr.length(); i++ )
    {
        aLetter    = theStr.data()[i].toLatin1();
        aLettIndex = (int)aLetter - FirstSymbolNumber;
        row        = aLettIndex / TEX_ROW_LEN;

        aLettBeginS = (float)myPositions[aLettIndex] / ( (float)myTexFontWidth - 1.f );
        aLettEndS   = aLettBeginS + ( (float)myWidths[aLettIndex] - 1.f ) / ( (float)myTexFontWidth - 1.f );
        aLettBeginT = ( myTexFontHeight - ( row + 1 ) * aRowPixelHeight ) / ( (float)myTexFontHeight - 1.f ); 
        aLettEndT   = aLettBeginT + ( (float)aRowPixelHeight - 1.f ) / ( (float)myTexFontHeight - 1.f );

        aDX = ( (float)myWidths[aLettIndex] - 1.f ) / aXScale;

        glTexCoord2f( aLettBeginS, aLettBeginT ); glVertex3f( theX,       theY,       1.f );
        glTexCoord2f( aLettBeginS, aLettEndT   ); glVertex3f( theX,       theY + aDY, 1.f );
        glTexCoord2f( aLettEndS,   aLettEndT   ); glVertex3f( theX + aDX, theY + aDY, 1.f );
        glTexCoord2f( aLettEndS,   aLettBeginT ); glVertex3f( theX + aDX, theY,       1.f );

        theX += aDX + mySeparator / aXScale;
    }

    glEnd();
    // restore attributes
    glPopAttrib();
}

/*!
  \return width of string in pixels
*/
int GLViewer_TexFont::getStringWidth( QString theStr )
{
    int aWidth = 0;
    for ( int i = 0; i < (int)theStr.length(); i ++ )
    {
        char aLetter = theStr.data()[i].toLatin1();
        int aLettIndex = (int)aLetter - FirstSymbolNumber;
        aWidth += myWidths[aLettIndex] + mySeparator;
    }

    return aWidth;
}

/*!
  \return height of string in pixels
*/
int GLViewer_TexFont::getStringHeight()
{
    QFontMetrics aFM( myQFont );
    return aFM.height();
}

/*!
  Generates list base for bitmap fonts
*/
static GLuint displayListBase( QFont* theFont )
{
  if ( !theFont )
    return 0;
  GLuint aList = 0;
  //static QMap<GLViewer_TexFindId, GLuint> fontCache;
  GLViewer_TexFindId aFindFont;
  aFindFont.myFontFamily = theFont->family();//theFont->toString();
  aFindFont.myIsBold = theFont->bold();
  aFindFont.myIsItal = theFont->italic();
  aFindFont.myIsUndl = theFont->underline();
  aFindFont.myPointSize = theFont->pointSize();

#ifdef WIN32
  HGLRC ctx = ::wglGetCurrentContext();
  if ( !ctx )
    return aList;  
  
  aFindFont.myViewPortId = (int)ctx;

  if ( GLViewer_TexFont::BitmapFontCache.contains( aFindFont ) )
    aList = GLViewer_TexFont::BitmapFontCache[aFindFont];
  else
  {
    GLuint listBase = 0;
    QMap<GLViewer_TexFindId, GLuint>::iterator it = GLViewer_TexFont::BitmapFontCache.begin();
    for ( ; it != GLViewer_TexFont::BitmapFontCache.end(); ++it )
    {
      if ( it.key().myViewPortId == (int)ctx && it.value() > listBase )
        listBase = it.value();
    }
    listBase += 256;

    HDC glHdc = ::wglGetCurrentDC();
 #if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    ::SelectObject( glHdc, theFont->handle() );
 #endif
    if ( !::wglUseFontBitmaps( glHdc, 0, 256, listBase ) )
      listBase = 0;
    aList = listBase;
    GLViewer_TexFont::BitmapFontCache[aFindFont] = aList;
  }
#else //X Window
  Display* aDisp = glXGetCurrentDisplay();
  if( !aDisp )
  {
#ifdef _DEBUG_
    printf( "Can't find current dislay\n" );
#endif
    return aList;
  }
  
  GLXContext aCont = glXGetCurrentContext();
  if( !aCont )
  {
#ifdef _DEBUG_
    printf( "Can't find current context\n" );
#endif
    return aList;
  }

  aFindFont.myViewPortId = size_t(aCont);

  if ( GLViewer_TexFont::BitmapFontCache.contains( aFindFont ) )
    aList = GLViewer_TexFont::BitmapFontCache[aFindFont];
  else
  {
    GLuint listBase = 0;
    QMap<GLViewer_TexFindId, GLuint>::iterator it = GLViewer_TexFont::BitmapFontCache.begin();
    for ( ; it != GLViewer_TexFont::BitmapFontCache.end(); ++it )
    {
      if ( it.key().myViewPortId == size_t(aCont) && it.value() > listBase )
        listBase = it.value();
    }
    listBase += 256;
    
    //glXUseXFont( (Font)(theFont->handle()), 0, 256, listBase );
    int aFontCont = 0;
    QString aFontDef = theFont->toString();
    char** xFontList = XListFonts( aDisp, aFontDef.toLatin1()/*aFindFont.myFontString.data()*/, 1, &aFontCont  );
// TODO (QT5 PORTING) Below is a temporary solution, to allow compiling with Qt 5
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if( !theFont->handle() )
    {
#endif
#ifdef _DEBUG_
      printf( "Can't load font %s. loading default font....\n", aFontDef.toLatin1().data()/*aFindFont.myFontString.data()*/ );
#endif
      QString aFontMask ("-*-*-*-r-*-*-");
      aFontMask += aFontDef/*aFindFont.myFontString*/.section( ',', 1, 1 );
#ifdef _DEBUG_
      printf( "Height of Default font: %s\n", aFontDef/*aFindFont.myFontString*/.section( ',', 1, 1 ).data() );
#endif
      aFontMask += "-*-*-*-m-*-*-*";
      xFontList = XListFonts( aDisp, aFontMask.toLatin1().constData()/*"-*-*-*-r-*-*-12-*-*-*-m-*-*-*"*/, 1, &aFontCont  );
      if( aFontCont == 0 )
      {
#ifdef _DEBUG_
        printf( "Can't load default font\n" );
#endif
        return 0;
      }
      glXUseXFont( (Font)(XLoadFont( aDisp,xFontList[0] )), 0, 256, listBase );
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    }
    else
      glXUseXFont( (Font)(theFont->handle()), 0, 256, listBase );
#endif
    aList = listBase;
    GLViewer_TexFont::BitmapFontCache[aFindFont] = aList;
  }

#endif

  return aList;
}

/*!
  Default constructor
*/
GLViewer_Drawer::GLViewer_Drawer()
: myFont( "Helvetica", 10, QFont::Bold )
{
  myXScale = myYScale = 0.0;
  myObjects.clear();
  myTextList = 0/*-1*/;
  myObjectType = "GLViewer_Object";
  myPriority = 0;
  myTextFormat = DTF_BITMAP;
  myTextScale = 0.125;
}

/*!
  Destructor
*/
GLViewer_Drawer::~GLViewer_Drawer()
{
  myObjects.clear();
  glDeleteLists( myTextList, 1 );
}

/*!
  Clears all generated textures
*/
void GLViewer_Drawer::destroyAllTextures()
{
    QMap<GLViewer_TexFindId,GLViewer_TexIdStored>::Iterator anIt= GLViewer_TexFont::TexFontBase.begin();
    QMap<GLViewer_TexFindId,GLViewer_TexIdStored>::Iterator anEndIt= GLViewer_TexFont::TexFontBase.end();

    for( ; anIt != anEndIt; anIt++ )
        glDeleteTextures( 1, &(anIt.value().myTexFontId) );
}

/*!
  Enables and disables antialiasing in Open GL (for points, lines and polygons).
  \param on - if it is true, antialiasing is enabled
*/
void GLViewer_Drawer::setAntialiasing(const bool on)
{
        if (on)
        {
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

                glEnable(GL_POINT_SMOOTH);
                glEnable(GL_LINE_SMOOTH);
                glEnable(GL_POLYGON_SMOOTH);
                glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
                glEnable (GL_BLEND);
        }
        else
        {
                glDisable(GL_POINT_SMOOTH);
                glDisable(GL_LINE_SMOOTH);
                glDisable(GL_POLYGON_SMOOTH);
                glBlendFunc (GL_ONE, GL_ZERO);
                glDisable (GL_BLEND);
        }
}

/*! Loads texture from file
  \param fileName - the name of texture file
  \param x_size   - the horizontal size of picture ( less or equal texture horizontal size )
  \param y_size   - the vertical size of picture ( less or equal texture vertical size )
  \param t_size   - the size of texture ( texture vertical size equals texture horizontal size )
*/
GLuint GLViewer_Drawer::loadTexture( const QString& fileName,
                                     GLint* x_size,
                                     GLint* y_size,
                                     GLint* t_size )
{
    QImage buf;
    if ( fileName.isEmpty() || !buf.load( fileName ) )
        return 0;

    int w = buf.width();
    int h = buf.height();

    int size = 16;
    while( size < w || size < h )
        size = size * 2;

    GLuint texture;
    GLubyte* pixels = new GLubyte[ size * size * 4 ];

    for( int i = 0; i < size; i++ )
    {            
        for( int j = 0; j < size; j++ )
        {
            GLubyte r, g, b, a;
            if( j < w && i < h )
            {
                QRgb pixel = buf.pixel( j, h - i - 1 );
                r = (GLubyte)qRed( pixel );
                g = (GLubyte)qGreen( pixel );
                b = (GLubyte)qBlue( pixel );
                a = (GLubyte)qAlpha( pixel );
            }
            else
            {
                r = (GLubyte)255;
                g = (GLubyte)255;
                b = (GLubyte)255;
                a = (GLubyte)255;
            }

            int index = 4 * ( i * size + j );
            pixels[ index ] = r;
            pixels[ index + 1 ] = g;
            pixels[ index + 2 ] = b;
            pixels[ index + 3 ] = a;
        }
    }

    //initialize texture
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    delete[] pixels;

    if ( x_size )
      *(x_size) = w;

    if ( y_size )
      *(y_size) = h;

    if ( t_size )
      *(t_size) = size;

    return texture;
}

/*! Draw square texture
   \param texture - the texture ID
   \param size    - the size of square texture
   \param x       - x coord
   \param y       - y coord
*/
void GLViewer_Drawer::drawTexture( GLuint texture, GLint size, GLfloat x, GLfloat y )
{
    /*float xScale = myXScale;
    float yScale = myYScale;

    glColor4f( 1.0, 1.0, 1.0, 1.0 );

    glEnable( GL_TEXTURE_2D );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glAlphaFunc( GL_GREATER, 0.95F );
    glEnable( GL_ALPHA_TEST );
    
    glBindTexture( GL_TEXTURE_2D, texture );
    glBegin( GL_QUADS );

    glTexCoord2f( 0.0, 0.0 );
    glVertex3f( x-size/2./xScale, y-size/2./yScale, 0.0 );

    glTexCoord2f( 0.0, 1.0 );
    glVertex3f( x-size/2./xScale, y+size/2./yScale, 0.0 );

    glTexCoord2f( 1.0, 1.0 );
    glVertex3f( x+size/2./xScale, y+size/2./yScale, 0.0 );

    glTexCoord2f( 1.0, 0.0 );
    glVertex3f( x+size/2./xScale, y-size/2./yScale, 0.0 );
    
    glEnd();
    glFlush();

    glDisable( GL_ALPHA_TEST );
    glDisable( GL_TEXTURE_2D );*/

  drawTexture( texture, size, size, x, y );
}

/*! Draw texture
   \param texture - the texture ID
   \param x_size  - the horizontal size of texture
   \param y_size  - the vertical size of texture
   \param x       - x coord
   \param y       - y coord
*/
void GLViewer_Drawer::drawTexture( GLuint texture, GLint x_size, GLint y_size, GLfloat x, GLfloat y )
{
    /*float xScale = myXScale;
    float yScale = myYScale;

    glColor4f( 1.0, 1.0, 1.0, 1.0 );

    glEnable( GL_TEXTURE_2D );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glAlphaFunc( GL_GREATER, 0.95F );
    glEnable( GL_ALPHA_TEST );
    
    glBindTexture( GL_TEXTURE_2D, texture );
    glBegin( GL_QUADS );

    glTexCoord2f( 0.0, 0.0 );
    glVertex3f( x-x_size/2./xScale, y-y_size/2./yScale, 0.0 );

    glTexCoord2f( 0.0, 1.0 );
    glVertex3f( x-x_size/2./xScale, y+y_size/2./yScale, 0.0 );

    glTexCoord2f( 1.0, 1.0 );
    glVertex3f( x+x_size/2./xScale, y+y_size/2./yScale, 0.0 );

    glTexCoord2f( 1.0, 0.0 );
    glVertex3f( x+x_size/2./xScale, y-y_size/2./yScale, 0.0 );
    
    glEnd();
    glFlush();

    glDisable( GL_ALPHA_TEST );
    glDisable( GL_TEXTURE_2D );*/
  drawTexturePart( texture, 1.0, 1.0, x_size, y_size, x, y );
}

/*! Draw texture part
   \param texture - the texture ID
   \param x_ratio - the horizontal ratio of texture part
   \param y_ratio - the vertical ratio of texture part
   \param x_size  - the horizontal size of texture
   \param y_size  - the vertical size of texture
   \param x       - x coord
   \param y       - y coord
   \param scale   - common scale factor ( if = 0, use drawer scales )
*/
void GLViewer_Drawer::drawTexturePart( GLuint texture,
                                       GLfloat x_ratio,
                                       GLfloat y_ratio,
                                       GLfloat x_size,
                                       GLfloat y_size,
                                       GLfloat x,
                                       GLfloat y,
                                       GLfloat scale )
{
  if( !texture )
    return;

  float xScale = scale > 0. ? 1./scale : myXScale;
  float yScale = scale > 0. ? 1./scale : myYScale;

  glColor4f( 1.0, 1.0, 1.0, 1.0 );


  glEnable( GL_TEXTURE_2D );
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  bool hasAlpha = glIsEnabled( GL_ALPHA_TEST );
  glDisable( GL_ALPHA_TEST );

  glBindTexture( GL_TEXTURE_2D, texture );
  glBegin( GL_QUADS );

  glTexCoord2f( 0.0, 0.0 );
  glVertex3f( x-x_size/2./xScale, y-y_size/2./yScale, 0.0 );

  glTexCoord2f( 0.0, y_ratio );
  glVertex3f( x-x_size/2./xScale, y+y_size/2./yScale, 0.0 );

  glTexCoord2f( x_ratio, y_ratio );
  glVertex3f( x+x_size/2./xScale, y+y_size/2./yScale, 0.0 );

  glTexCoord2f( x_ratio, 0.0 );
  glVertex3f( x+x_size/2./xScale, y-y_size/2./yScale, 0.0 );
  
  glEnd();
  glFlush();

  if ( hasAlpha )
    glEnable( GL_ALPHA_TEST );

  glDisable( GL_TEXTURE_2D );
}

/*!
  Draw text
  \param text - text to be drawn
  \param xPos - x position
  \param yPos - y position
  \param color - color of text
  \param theFont - font of text
  \param theSeparator - letter separator
  \param theFormat - text format (by default DTF_BITMAP)
*/
void GLViewer_Drawer::drawText( const QString& text, GLfloat xPos, GLfloat yPos,
                                const QColor& color, QFont* theFont, int theSeparator, DisplayTextFormat theFormat )
{
  glColor3f( ( GLfloat )color.red() / 255, 
             ( GLfloat )color.green() / 255, 
             ( GLfloat )color.blue() / 255 );

  if( theFormat != DTF_BITMAP )
  {
    GLViewer_TexFont aTexFont( theFont, theSeparator, theFormat == DTF_TEXTURE_SCALABLE, GL_LINEAR );
    // Font texture was not found or generated --> cannot draw text
    if ( !aTexFont.generateTexture() )
      return;

    if ( theFormat == DTF_TEXTURE_SCALABLE )
      aTexFont.drawString( text, xPos, yPos, textScale() );
    else
      aTexFont.drawString( text, xPos, yPos );
  }
  else
  {
    glRasterPos2f( xPos, yPos );
    glListBase( displayListBase( theFont ) );
    glCallLists( text.length(), GL_UNSIGNED_BYTE, text.toLocal8Bit().data() );
  }
}

/*!
  Draws object-text
*/
void GLViewer_Drawer::drawText( GLViewer_Object* theObject )
{
  if( !theObject )
    return;

  GLViewer_Text* aText = theObject->getGLText();
  if( !aText )
    return;

  GLfloat aPosX, aPosY;
  aText->getPosition( aPosX, aPosY );
  // get temporary copy of font
  QFont aTmpVarFont = aText->getFont();
  drawText( aText->getText(), aPosX, aPosY, aText->getColor(), &aTmpVarFont, aText->getSeparator(), aText->getDisplayTextFormat() );
}

/*! Draw text
   \param text      - the text string
   \param x         - x coord
   \param y         - y coord
   \param hPosition - horizontal alignment
   \param vPosition - vertical alignment
   \param color     - text color
   \param smallFont - font format
*/
void GLViewer_Drawer::drawGLText( QString text, float x, float y,
                                  int hPosition, int vPosition, QColor color, bool smallFont )
{
  QFont aFont( myFont );
  if( smallFont )
    aFont.setPointSize( int(aFont.pointSize() * 0.8) );

  GLfloat scale = textScale() > 0. ? textScale() : 1.;

  QFontMetrics aFontMetrics( aFont );
  float width  = myTextFormat == DTF_TEXTURE_SCALABLE ? aFontMetrics.width( text ) * scale : aFontMetrics.width( text ) / myXScale;
  float height = myTextFormat == DTF_TEXTURE_SCALABLE ? aFontMetrics.height() * scale : aFontMetrics.height() / myYScale;
  float gap = 5 / myXScale;

  switch( hPosition )
  {
      case GLText_Left   : x -= ( gap + width ); break;
      case GLText_Center : x -= width / 2; break;
      case GLText_Right  : x += gap; break;
      default : break;
  }

  switch( vPosition )
  {
      case GLText_Top    : y += height * 0.5; break;
      case GLText_Center : y -= height * 0.5; break;
      case GLText_Bottom : y -= height * 1.5; break;
      default : break;
  }

  drawText( text, x, y, color, &aFont, 2, myTextFormat );
}

/*!
  \return a rectangle of text (without viewer scale)
*/
GLViewer_Rect GLViewer_Drawer::textRect( const QString& text ) const
{
  GLfloat scale = textScale() > 0. ? textScale() : 1.;

  QFontMetrics aFontMetrics( myFont );
  float width  = myTextFormat == DTF_TEXTURE_SCALABLE ? aFontMetrics.width( text ) * scale : aFontMetrics.width( text );
  float height = myTextFormat == DTF_TEXTURE_SCALABLE ? aFontMetrics.height() * scale : aFontMetrics.height();

  return GLViewer_Rect( 0, width, height, 0 );
}

/*!
  Draws rectangle
  \param rect - instance of primitive
  \param color - color of primitive
*/
void GLViewer_Drawer::drawRectangle( GLViewer_Rect* rect, QColor color )
{
  if( !rect )
    return;

  float x1 = rect->left();
  float x2 = rect->right();
  float y1 = rect->bottom();
  float y2 = rect->top();
  
  glColor3f( ( GLfloat )color.red() / 255,
    ( GLfloat )color.green() / 255,
    ( GLfloat )color.blue() / 255 );
  glLineWidth( 1.0 );
  
  glBegin( GL_LINE_LOOP );
  glVertex2f( x1, y1 );
  glVertex2f( x1, y2 );
  glVertex2f( x2, y2 );
  glVertex2f( x2, y1 );
  glEnd();
}

/*!
  Saves object to file with format of HPGL
  \param hFile - file
  \param aViewerCS - viewer co-ordinate system
  \param aHPGLCS - paper co-ordinate system
*/
bool GLViewer_Drawer::translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS )
{
    bool result = true;
    for( int i=0, n=myObjects.count(); i<n; i++ ) 
        result &= myObjects[i]->translateToHPGL( hFile, aViewerCS, aHPGLCS );
    return result;
}

/*!
  Saves object to file with format of PostScript
  \param hFile - file
  \param aViewerCS - viewer co-ordinate system
  \param aPSCS - paper co-ordinate system
*/
bool GLViewer_Drawer::translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS )
{
    bool result = true;
    for( int i=0, n=myObjects.count(); i<n; i++ ) 
        result &= myObjects[i]->translateToPS( hFile, aViewerCS, aPSCS );
    return result;
}

#ifdef WIN32
/*!
  Saves object to file with format of EMF
  \param hFile - file
  \param aViewerCS - viewer co-ordinate system
  \param aEMFCS - paper co-ordinate system
*/
bool GLViewer_Drawer::translateToEMF( HDC hDC, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS )
{
    bool result = true;
    for( int i=0, n=myObjects.count(); i<n; i++ ) 
        result &= myObjects[i]->translateToEMF( hDC, aViewerCS, aEMFCS );
    return result;
}
#endif

/*!
  Draws rectangle
  \param rect - instance of primitive
  \param lineWidth - width of line
  \param gap - gap of rectangle
  \param color - color of primitive
  \param filled - if it is true, then rectangle will be drawn filled with color "fillingColor"
  \param fillingColor - color of filling
*/
void GLViewer_Drawer::drawRectangle( GLViewer_Rect* rect, GLfloat lineWidth, GLfloat gap,
                                     QColor color, bool filled, QColor fillingColor )
{
  if( !rect )
    return;

  float x1 = rect->left() - gap;
  float x2 = rect->right() + gap;
  float y1 = rect->bottom() - gap;
  float y2 = rect->top() + gap;
  
  if( filled )
  {
    glColor3f( ( GLfloat )fillingColor.red() / 255,
      ( GLfloat )fillingColor.green() / 255,
      ( GLfloat )fillingColor.blue() / 255 );
    glBegin( GL_POLYGON );
    glVertex2f( x1, y1 );
    glVertex2f( x1, y2 );
    glVertex2f( x2, y2 );
    glVertex2f( x2, y1 );
    glEnd();
  }

  glColor3f( ( GLfloat )color.red() / 255,
    ( GLfloat )color.green() / 255,
    ( GLfloat )color.blue() / 255 );
  glLineWidth( lineWidth );
  
  glBegin( GL_LINE_LOOP );
  glVertex2f( x1, y1 );
  glVertex2f( x1, y2 );
  glVertex2f( x2, y2 );
  glVertex2f( x2, y1 );
  glEnd();
}

/*!
  Draws contour
  \param pntList - list of points
  \param color - color of contour
  \param lineWidth - width of line
*/
void GLViewer_Drawer::drawContour( const GLViewer_PntList& pntList, QColor color, GLfloat lineWidth )
{
  glColor3f( ( GLfloat )color.red() / 255,
    ( GLfloat )color.green() / 255,
    ( GLfloat )color.blue() / 255 );
  glLineWidth( lineWidth );
  
  glBegin( GL_LINES );
  QList<GLViewer_Pnt>::const_iterator it = pntList.begin();
  for( ; it != pntList.end(); ++it )
    glVertex2f( (*it).x(), (*it).y() );
  glEnd();
}

/*!
  Draws rectangular contour
  \param rect - instance of rectangle
  \param color - color of primitive
  \param lineWidth - width of line
  \param pattern - pattern of line
  \param isStripe - enables line stipple
*/
void GLViewer_Drawer::drawContour( GLViewer_Rect* rect, QColor color, GLfloat lineWidth,
                                   GLushort pattern, bool isStripe )
{
  float x1 = rect->left();
  float x2 = rect->right();
  float y1 = rect->bottom();
  float y2 = rect->top();
  
  glColor3f( ( GLfloat )color.red() / 255,
    ( GLfloat )color.green() / 255,
    ( GLfloat )color.blue() / 255 );
  glLineWidth( lineWidth );
  
  if ( isStripe )
  {
    glEnable( GL_LINE_STIPPLE );
    glLineStipple( 1, pattern );
  }

  glBegin( GL_LINE_LOOP );

  glVertex2f( x1, y1 );
  glVertex2f( x1, y2 );
  glVertex2f( x2, y2 );
  glVertex2f( x2, y1 );

  glEnd();
  glDisable( GL_LINE_STIPPLE );
}

/*!
  Draws polygon
  \param pntList - list of points
  \param color - color of polygon
*/
void GLViewer_Drawer::drawPolygon( const GLViewer_PntList& pntList, QColor color )
{
  glColor3f( ( GLfloat )color.red() / 255,
    ( GLfloat )color.green() / 255,
    ( GLfloat )color.blue() / 255 );
  glBegin( GL_POLYGON );
  QList<GLViewer_Pnt>::const_iterator it = pntList.begin();
  for( ; it != pntList.end(); ++it )
    glVertex2f( (*it).x(), (*it).y() );
  glEnd();
}

/*!
  Draws rectangle
  \param rect - instance of rectangle
  \param color - color of polygon
  \param pattern - pattern of line
  \param isStripe - enables line stipple
*/
void GLViewer_Drawer::drawPolygon( GLViewer_Rect* rect, QColor color,
                                      GLushort pattern, bool isStripe )
{
  float x1 = rect->left();
  float x2 = rect->right();
  float y1 = rect->bottom();
  float y2 = rect->top();
  glColor3f( ( GLfloat )color.red() / 255,
    ( GLfloat )color.green() / 255,
    ( GLfloat )color.blue() / 255 );

  if ( isStripe )
  {
    glEnable( GL_LINE_STIPPLE );
    glLineStipple( 1, pattern );
  }
  glBegin( GL_POLYGON );

  glVertex2f( x1, y1 );
  glVertex2f( x1, y2 );
  glVertex2f( x2, y2 );
  glVertex2f( x2, y1 );

  glEnd();
  glDisable( GL_LINE_STIPPLE );
}

GLubyte rasterVertex[5] = { 0x70, 0xf8, 0xf8, 0xf8, 0x70 };

/*!
  Draws vertex
  \param x - x position
  \param y - y position
  \param color - color of vertex
*/
void GLViewer_Drawer::drawVertex( GLfloat x, GLfloat y, QColor color )
{
  glColor3f( ( GLfloat )color.red() / 255, ( GLfloat )color.green() / 255, ( GLfloat )color.blue() / 255 );
  glRasterPos2f( x, y );
  glBitmap( 5, 5, 2, 2, 0, 0, rasterVertex );
}

GLubyte rasterCross[7] =  { 0x82, 0x44, 0x28, 0x10, 0x28, 0x44, 0x82 };

/*!
  Draws cross
  \param x - x position
  \param y - y position
  \param color - color of cross
*/
void GLViewer_Drawer::drawCross( GLfloat x, GLfloat y, QColor color )
{
  glColor3f( ( GLfloat )color.red() / 255, ( GLfloat )color.green() / 255, ( GLfloat )color.blue() / 255 );
  glRasterPos2f( x, y );
  glBitmap( 7, 7, 3, 3, 0, 0, rasterCross );
}

/*!
  Draws arrow
  \param red, green, blue - components of color
  \param lineWidth - width of line
  \param staff - 
  \param length - length of arrow
  \param width - width of arrow
  \param x - x position
  \param y - y position
  \param angle - angle of arrow
  \param filled - drawn as filled
*/
void GLViewer_Drawer::drawArrow( const GLfloat red, const GLfloat green, const GLfloat blue,
                                 GLfloat lineWidth,
                                 GLfloat staff, GLfloat length, GLfloat width,
                                 GLfloat x, GLfloat y, GLfloat angle, GLboolean filled )
{
  GLfloat vx1 = x;
  GLfloat vy1 = y + staff + length;
  GLfloat vx2 = vx1 - width / 2;
  GLfloat vy2 = vy1 - length;
  GLfloat vx3 = vx1 + width / 2;
  GLfloat vy3 = vy1 - length;

  gp_Pnt2d p0( x, y );
  gp_Pnt2d p1( vx1, vy1 );
  gp_Pnt2d p2( vx2, vy2 );
  gp_Pnt2d p3( vx3, vy3 );

  p1.Rotate( p0, angle );
  p2.Rotate( p0, angle );
  p3.Rotate( p0, angle );
  
  vx1 = p1.X(); vy1 = p1.Y();
  vx2 = p2.X(); vy2 = p2.Y();
  vx3 = p3.X(); vy3 = p3.Y();

  glColor3f( red, green, blue );
  glLineWidth( lineWidth );

  glBegin( GL_LINES );
  glVertex2f( x, y );
  glVertex2f( vx1, vy1 );
  glEnd();

  filled = true;
  if( !filled )
  {
    glBegin( GL_LINES );
    glVertex2f( vx1, vy1 );
    glVertex2f( vx2, vy2 );
    glVertex2f( vx1, vy1 );
    glVertex2f( vx3, vy3 );
    glEnd();
  }
  else
  {
    glBegin( GL_POLYGON );
    glVertex2f( vx1, vy1 );
    glVertex2f( vx2, vy2 );
    glVertex2f( vx3, vy3 );
    glEnd();
  }
}
