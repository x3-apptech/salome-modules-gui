// File:      GLViewer_Widget.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/***************************************************************************
**  Class:   GLViewer_Widget
**  Descr:   OpenGL QWidget for GLViewer
**  Module:  GLViewer
**  Created: UI team, 27.03.03
****************************************************************************/

//#include <GLViewerAfx.h>
#include "GLViewer_Widget.h"
#include "GLViewer_ViewPort2d.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_Compass.h"
#include "GLViewer_Grid.h"
#include "GLViewer_Object.h"
#include "GLViewer_CoordSystem.h"

#include <cmath>
using namespace std;
//#include <math.h>
//#include <stdlib.h>
//#include <iostream.h>

#include <qevent.h>
#include <qrect.h>

#include <qpixmap.h>
#include <qimage.h>
#include <qapplication.h>
#include <qintdict.h>
#include <qpaintdevicemetrics.h>
#include <qsize.h>
#include <qtooltip.h>

//-----------
#include <qfontdialog.h>
#include <qfontmetrics.h>
#include <qpainter.h>
//-----------

//static GLuint texFont;
//static int fontW;
//static int fontH;

/*
static void genFont()
{
    bool ok;
    QFont aFont = QFontDialog::getFont( &ok );
    QFontMetrics aFM( aFont );
    QString aStr;
    for( int k = 32; k <= 127; k++ )
    {
        char aLetter = (char)k;
        aStr += aLetter;
    }
    
    int pixelsWidth = aFM.width( aStr );
    int pixelsHigh = aFM.height();
    int aFontX = 64;
    int aFontY = 64;

    while( aFontX < pixelsWidth )
        aFontX = aFontX * 2;
    while( aFontY < pixelsHigh )
        aFontY = aFontY * 2;

    QPixmap* aPixmap = new QPixmap( aFontX, aFontY );
    aPixmap->fill( QColor( 255, 255, 255) );
    QPainter aPainter( aPixmap );
    aPainter.setFont( aFont );    
    aPainter.drawText ( 0, pixelsHigh, aStr );
    QImage aImage = aPixmap->convertToImage();

    aImage.save( "W:\\Temp\\pic.jpg", "JPEG" );

    char* pixels = new char[aFontX * aFontY * 2];

    for( int i = 0; i < aFontY; i++ )
    {            
        for( int j = 0; j < aFontX;  j++ )
        {
            if( qRed( aImage.pixel( j, aFontY - i - 1 ) ) == 0 )
            {
                pixels[i * aFontX * 2 + j * 2] = (char) 255;
                pixels[i * aFontX * 2 + j * 2 + 1]= (char) 255;
            }
            else
            {
                pixels[i * aFontX * 2 + j * 2] = (char) 0;
                pixels[i * aFontX * 2 + j * 2 + 1]= (char) 0;
            }                
        }
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texFont);
    glBindTexture(GL_TEXTURE_2D, texFont);  
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, 2, aFontX,
        aFontY, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pixels);

    fontW = aFontX;
    fontH = aFontY;

    delete[] pixels;
}

static void showFont()
{
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glAlphaFunc(GL_GEQUAL, 0.005F);
    glEnable(GL_ALPHA_TEST);
    glColor3f( 1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, texFont);
    glBegin(GL_QUADS);

    glTexCoord2f( 0.0, 0.0 ); glVertex3f( -fontW/2, -fontH/2, 1.0 );    
    glTexCoord2f( 0.0, 1.0 ); glVertex3f( -fontW/2, fontH/2, 1.0 );
    glTexCoord2f( 1.0, 1.0 ); glVertex3f( fontW/2, fontH/2, 1.0 );
    glTexCoord2f( 1.0, 0.0 ); glVertex3f( fontW/2, -fontH/2, 1.0 );

    glEnd();
    glFlush();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
}
*/

GLViewer_Widget::GLViewer_Widget( QWidget* parent, const char* name ):
QGLWidget( parent, 0/*, WRepaintNoErase | WResizeNoErase*/ )
{
  myViewPort = ( GLViewer_ViewPort2d* )parent;

  myXPan = 0.0;
  myYPan = 0.0;
  myZPan = 0.0;
  myXScale = 1.0;
  myYScale = 1.0;
  myZScale = 1.0;
  myRotationAngle = 0.0;
  myRotationCenterX = 0.0;
  myRotationCenterY = 0.0;
  myRotationCenterZ = 1.0;
  myRotationAnglePrev = 0.0;

  myStart = GL_TRUE;

  isExportMode = false;

  //init();
  setMouseTracking( true );
}

GLViewer_Widget::~GLViewer_Widget()
{
}

void GLViewer_Widget::getPan( GLfloat& xPan, GLfloat& yPan, GLfloat& zPan )
{
  xPan = myXPan;
  yPan = myYPan;
  zPan = myZPan;
}

void GLViewer_Widget::setPan( GLfloat xPan, GLfloat yPan, GLfloat zPan )
{
  myXPan = xPan;
  myYPan = yPan;
  myZPan = zPan;
}

void GLViewer_Widget::getScale( GLfloat& xScale, GLfloat& yScale, GLfloat& zScale )
{
  xScale = myXScale;
  yScale = myYScale;
  zScale = myZScale;
}

void GLViewer_Widget::setScale( GLfloat xScale, GLfloat yScale, GLfloat zScale )
{
  if ( xScale > 0 && yScale > 0 && zScale > 0 )
  {
    myXScale = xScale;
    myYScale = yScale;
    myZScale = zScale;
  }
}

void GLViewer_Widget::getRotationStart( GLfloat& rotationStartX, GLfloat& rotationStartY,
                                     GLfloat& rotationStartZ )
{
    rotationStartX = myRotationStartX;
    rotationStartY = myRotationStartY;
    rotationStartZ = myRotationStartZ;
}

void GLViewer_Widget::setRotationStart( GLfloat rotationStartX, GLfloat rotationStartY,
                                     GLfloat rotationStartZ )
{
    myRotationStartX = rotationStartX;
    myRotationStartY = rotationStartY;
    myRotationStartZ = rotationStartZ;
}

void GLViewer_Widget::getRotation( GLfloat& rotationAngle, GLfloat& rotationCenterX,
                                GLfloat& rotationCenterY, GLfloat& rotationCenterZ )
{
    rotationAngle = myRotationAngle;
    rotationCenterX = myRotationCenterX;
    rotationCenterY = myRotationCenterY;
    rotationCenterZ = myRotationCenterZ;
}

void GLViewer_Widget::setRotation( GLfloat rotationAngle, GLfloat rotationCenterX,
                                GLfloat rotationCenterY, GLfloat rotationCenterZ )
{
    myRotationAngle = rotationAngle;
    myRotationCenterX = rotationCenterX;
    myRotationCenterY = rotationCenterY;
    myRotationCenterZ = rotationCenterZ;
}

void GLViewer_Widget::setBackground( QString filename )
{
    
    //get image
    QImage buf;
    if ( !filename.isEmpty() && buf.load( filename ) ) 
    {  // Load first image from file
        isLoadBackground = true;
        myBackgroundFile = filename;

        myIW = buf.width();
        myIH = buf.height();

        myBackgroundSize = 64;
        while( myBackgroundSize < myIW || myBackgroundSize < myIH)
            myBackgroundSize = myBackgroundSize * 2;

        GLubyte* pixels = new GLubyte[myBackgroundSize * myBackgroundSize * 4];

        for( int i = 0; i < myBackgroundSize; i++ )
        {            
            for( int j = 0; j < myBackgroundSize; j++ )
            {
                if( j < myIW && i < myIH )
                {
                    pixels[i * myBackgroundSize * 4 + j * 4] = (GLubyte)qRed( buf.pixel(j,myIH - i - 1) );
                    pixels[i * myBackgroundSize * 4 + j * 4 + 1]= (GLubyte)qGreen( buf.pixel(j,myIH - i - 1) );
                    pixels[i * myBackgroundSize * 4 + j * 4 + 2] = (GLubyte)qBlue( buf.pixel(j,myIH - i - 1) );
                }
                else
                {
                    pixels[i * myBackgroundSize * 4 + j * 4] = (GLubyte)0;
                    pixels[i * myBackgroundSize * 4 + j * 4 + 1] = (GLubyte)0;
                    pixels[i * myBackgroundSize * 4 + j * 4 + 2] = (GLubyte)0;
                }                
                pixels[i * myBackgroundSize* 4 + j * 4 +  3] = (GLubyte)255;
            }
        }

        //initialize texture
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &texName);
        glBindTexture(GL_TEXTURE_2D, texName);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myBackgroundSize , myBackgroundSize, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            pixels);

        delete[] pixels;        
    }
    
}

void GLViewer_Widget::addToolTip( QString theString, QRect theRect )
{
    myToolTipRect = theRect;
    QToolTip::add( this, myToolTipRect, theString );
}

void GLViewer_Widget::removeToolTip()
{
    QToolTip::remove( this, myToolTipRect );
}

void GLViewer_Widget::initializeGL()
{
    setAutoBufferSwap( true );

//  cout << "GLViewer_Widget::initializeGL" << endl;

    //glClearColor( 0.0, 0.0, 0.0, 0.0 );
    //-----------------
    glShadeModel(GL_FLAT);
    
    //get image
    QImage buf; 
    QString aPicturePath = getenv("GLViewer__Background_Picture");
    
    if ( !aPicturePath.isEmpty() && buf.load( aPicturePath ) ) 
    {  // Load first image from file
        isLoadBackground = true;
        setBackground( aPicturePath );
       
        // for test texture font
        //genFont();
/*
        bool ok;
        QFont aFont = QFontDialog::getFont( &ok, this );
        QFontMetrics aFM( aFont );
        int pixelsWidth = aFM.width( "What's the width of this text?" );
        int pixelsHigh = aFM.height();
        int aFontX = 64;
        int aFontY = 64;

        while( aFontX < pixelsWidth )
            aFontX = aFontX * 2;
        while( aFontY < pixelsHigh )
            aFontY = aFontY * 2;

        myIW = aFontX;
        myIH = aFontY;

        QPixmap* aPixmap = new QPixmap( aFontX, aFontY );
        aPixmap->fill( QColor( 255, 255, 255) );
        QPainter aPainter( aPixmap );
        aPainter.setFont( aFont );
        //Painter.setPen( QColor( 0, 0, 0 ) );
        aPainter.drawText ( 0, pixelsHigh, "A" );
        QImage aImage = aPixmap->convertToImage();

        aImage.save( "W:\\Temp\\pic.jpg", "JPEG" );

        GLubyte* pixels = new GLubyte[aFontX * aFontY * 2];

        for( int i = 0; i < aFontY; i++ )
        {            
            for( int j = 0; j < aFontX;  j++ )
            {
                if( qRed( aImage.pixel( j, myIH - i - 1 ) ) == 0 )
                {
                    pixels[i * aFontX * 2 + j * 2] = (GLubyte) 255;
                    pixels[i * aFontX * 2 + j * 2 + 1]= (GLubyte) 255;
                }
                else
                {
                    pixels[i * aFontX * 2 + j * 2] = (GLubyte) 0;
                    pixels[i * aFontX * 2 + j * 2 + 1]= (GLubyte) 0;
                }                
            }
        }


        glBindTexture(GL_TEXTURE_2D, texName);  
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D(GL_TEXTURE_2D, 0, 2, aFontX,
            aFontY, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, pixels);

        delete[] pixels;
*/
        
        //tex2 = buf.convertDepth( 32, 0 );

        //tex1 = QGLWidget::convertToGLFormat( tex2 );  // flipped 32bit RGBA

        /*myIW = buf.width();
        myIH = buf.height();

        myBackgroundSize = 64;
        while( myBackgroundSize < myIW || myBackgroundSize < myIH)
            myBackgroundSize = myBackgroundSize * 2;

        GLubyte* pixels = new GLubyte[myBackgroundSize * myBackgroundSize * 4];

        //GLubyte pixels[512][512][3];
        for( int i = 0; i < myBackgroundSize; i++ )
        {            
            for( int j = 0; j < myBackgroundSize; j++ )
            {
                if( j < myIW && i < myIH )
                {
                    pixels[i * myBackgroundSize * 4 + j * 4] = (GLubyte)qRed( buf.pixel(j,myIH - i - 1) );
                    pixels[i * myBackgroundSize * 4 + j * 4 + 1]= (GLubyte)qGreen( buf.pixel(j,myIH - i - 1) );
                    pixels[i * myBackgroundSize * 4 + j * 4 + 2] = (GLubyte)qBlue( buf.pixel(j,myIH - i - 1) );
                }
                else
                {
                    pixels[i * myBackgroundSize * 4 + j * 4] = (GLubyte)0;
                    pixels[i * myBackgroundSize * 4 + j * 4 + 1] = (GLubyte)0;
                    pixels[i * myBackgroundSize * 4 + j * 4 + 2] = (GLubyte)0;
                }                
                pixels[i * myBackgroundSize* 4 + j * 4 +  3] = (GLubyte)255;
                //cout << "RED = " << qRed( buf.pixel(i,j) ) << " GREEN  = " << qGreen( buf.pixel(i,j) ) <<
                //    " BLUE = " << qBlue( buf.pixel(i,j) ) << endl;
            }
        }

        //initialize texture
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &texName);
        glBindTexture(GL_TEXTURE_2D, texName);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myBackgroundSize , myBackgroundSize, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            pixels);

        delete[] pixels;  
        */
        
    }
    
    else
        isLoadBackground = false;

    //GLViewer_TexFont aTexFont;
    //aTexFont.generateTexture();
    //aTexFont.drawString( "hello" );
    //-----------------
}

void GLViewer_Widget::paintGL()
{
//    cout << "GLViewer_Widget::paintGL" << endl;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();    

    glRotatef( myRotationAngle, myRotationCenterX, myRotationCenterY, myRotationCenterZ );
    glScalef( myXScale, myYScale, myZScale );
    glTranslatef( myXPan, myYPan, myZPan );
    //glRotatef( myRotationAngle, myRotationCenterX, myRotationCenterY, myRotationCenterZ );  
    
        if( isLoadBackground )
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D, texName);
        glBegin(GL_QUADS);

        glTexCoord2f( 0.0, 0.0); glVertex3f( -myIW/2, -myIH/2, 0.0);    
        glTexCoord2f( 0.0, (float)myIH/myBackgroundSize ); glVertex3f( -myIW/2, myIH/2, 0.0);
        glTexCoord2f( (float)myIW/myBackgroundSize, (float)myIH/myBackgroundSize ); glVertex3f( myIW/2, myIH/2, 0.0);
        glTexCoord2f( (float)myIW/myBackgroundSize, 0.0); glVertex3f( myIW/2, -myIH/2, 0.0);
        
        glEnd();
        glFlush();
        glDisable(GL_TEXTURE_2D);

        // for test texture font
        // showFont();
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }


    GLViewer_Grid* grid = myViewPort->getGrid();
    if( grid )
        grid->draw();
    //myViewPort->drawCompass();

    //glEnable( GL_POLYGON_OFFSET_FILL );
    //glPolygonOffset( 0, 1 );

    GLViewer_Viewer2d* v = ( GLViewer_Viewer2d* )getViewPort()->getViewFrame()->getViewer();
    if( !isExportMode )
        v->updateDrawers( GL_FALSE, myXScale, myYScale );
    else
        v->repaintView( getViewPort()->getViewFrame() );

    //glDisable( GL_POLYGON_OFFSET_FILL );

//    GLViewer_TexFont aTexFont;
//    aTexFont.generateTexture();
//    aTexFont.drawString( "hello" );
}

void GLViewer_Widget::resizeGL( int w, int h )
{
//  cout << "GLViewer_Widget::resizeGL " << w << " " << h << endl;

  if( h < 1 ) h = 1;
  if( w < 1 ) w = 1;
  glViewport( 0, 0, w, h);

  if( myStart )
  {
    myWidth = w;
    myHeight = h;
    myStart = GL_FALSE;
  }

  myViewPort->initResize( w, h );

  //GLViewer_Viewer2d* v = ( GLViewer_Viewer2d* )getViewPort()->getViewFrame()->getViewer();
  //v->updateDrawers( GL_FALSE, myXScale, myYScale );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  GLfloat w_c = w / 2., h_c = h / 2.; 

  gluOrtho2D( -w_c, w_c, -h_c, h_c ); 

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity(); 
}

void GLViewer_Widget::copyBuffers()
{
//  cout << "GLViewer_Widget::copyBuffers" << endl;

  GLint val[4]; 
  glGetIntegerv( GL_VIEWPORT, val );
  glReadBuffer( GL_FRONT );
  glCopyPixels( val[0], val[1], val[2], val[3], GL_COLOR );
}

void GLViewer_Widget::exportRepaint()
{
    isExportMode = true;

    paintGL();

    isExportMode = false;
}


void GLViewer_Widget::paintEvent( QPaintEvent* e )
{
//  cout << "GLViewer_Widget::paintEvent" << endl;
  QApplication::sendEvent( myViewPort, e );
}

void GLViewer_Widget::mouseMoveEvent( QMouseEvent* e )
{
//  cout << "GLViewer_Widget::mouseMoveEvent" << endl;
  QApplication::sendEvent( myViewPort, e );
}

void GLViewer_Widget::mousePressEvent( QMouseEvent* e )
{
//  cout << "GLViewer_Widget::mousePressEvent" << endl;
  QApplication::sendEvent( myViewPort, e );
}

void GLViewer_Widget::mouseReleaseEvent( QMouseEvent* e )
{
//  cout << "GLViewer_Widget::mouseReleaseEvent" << endl;
  QApplication::sendEvent( myViewPort, e );
}

void GLViewer_Widget::enterEvent( QEvent* e )
{
//  cout << "GLViewer_Widget::enterEvent" << endl;
  updateGL();
}
void GLViewer_Widget::leaveEvent( QEvent* e )
{
//  cout << "GLViewer_Widget::leaveEvent" << endl;
  updateGL();
}

inline char hex( uchar c )
{
  if( c<=9 )
    return '0'+c;
  else
    return 'a' + c - 10;
}

void AddImagePart( QFile& hFile, QImage& image, int w1, int w2, int h1, int h2, 
                   GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS, 
                   double a, double b, double c, double d, double dw, double dh )
{
    if( aViewerCS && aPSCS )
    {       
        double width = w2-w1+1, height = h2-h1+1;
        QString aBuffer = "", temp = "%1 %2 8 [ %3 %4 %5 %6 %7 %8 ]\n";
        aBuffer += temp.arg( width ).arg( height ).
                        arg( a ).arg( b ).arg( c ).arg( d ).
                        arg( dw ).arg( dh );
        aBuffer += "<\n";   

        char line[81]; line[80] = '\0'; int cur_index = 0;
        int full = 0;
        for( int i=h2; i>=h1; i-- )
        {           
            uchar* theCurLine = image.scanLine( i ), cur;
            for( int j=w1; j<=w2; j++ )
                for( int k=0; k<3; k++ )
                {
                    cur = *(theCurLine+4*j+2-k);
                    *(line+cur_index) = hex( cur/16 ); //HI
                    *(line+cur_index+1) = hex( cur%16 ); //LO
                    full++;
                    cur_index+=2;
                    if( cur_index>=80 )
                    {
                        aBuffer += line;
                        aBuffer += "\n";
                        cur_index = 0;
                    }
                }           
        }

        aBuffer += "> false 3 colorimage\n\n";

        hFile.writeBlock( aBuffer.ascii(), aBuffer.length() );
    }
}

void GLViewer_Widget::getBackgroundRectInViewerCS( double& left, double& top, double& right, double& bottom )
{
    left = -myIW/2; right = myIW/2; 
    top = myIH/2; bottom = -myIH/2;
}

void GLViewer_Widget::translateBackgroundToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS )
{
    QImage buf; 

    if( aViewerCS && aPSCS && isLoadBackground && buf.load( myBackgroundFile ) )
    {       
        double a, b, c, d, dx, dy; //The preparation of transformation matrix

        double width = buf.width(), height = buf.height();

        double left, top, right, bottom;
        getBackgroundRectInViewerCS( left, top, right, bottom );

        double aax = left,  aay = bottom,
               bbx = right, bby = bottom,
               ccx = left,  ccy = top;             

        aViewerCS->transform( *aPSCS, aax, aay );
        aViewerCS->transform( *aPSCS, bbx, bby );
        aViewerCS->transform( *aPSCS, ccx, ccy );       

        a = ( bbx - aax ) / width;
        b = ( ccx - aax ) / height;
        c = ( bby - aay ) / width;
        d = ( ccy - aay ) / height;

        //Now we must find invert matrix 
        double det = a*d-b*c,
               newa = d/det,
               newb = -c/det,
               newc = -b/det,
               newd = a/det;

        a = newa; b = newb; c = newc; d = newd;

        dx = -(a*aax+c*aay);
        dy = -(b*aax+d*aay); //according to PS specification of coordinate transformation
        
        const int max = 133000; //The maximum length of string in PS
        int dh = int( floor( double( max ) / ( 3.0*2.0*width ) ) );
        for( int k=buf.height()-1; k>=0; k-=dh )
            AddImagePart( hFile, buf, 0, buf.width()-1, QMAX( k-dh+1, 0 ), k,
                          aViewerCS, aPSCS, a, b, c, d, dx, dy-(buf.height()-1-k) );
    }
}

void DecodeScanLine( int width, uchar* dest, int dest_depth, uchar* source, int source_depth )
{
#ifndef WIN32
typedef unsigned int WORD;
#endif

    int aSize = width*dest_depth,
        dw = aSize % 8;

    if( dw )
        aSize+=dw;

    if( dest_depth==source_depth )
        memcpy( dest, source, aSize/8 );
    else
    {
        double r, g, b; WORD color;
        for( int i=0; i<width; i++ )
        {
            color = 0;
            switch( source_depth )
            {
                case 16:
                    memcpy( &color, source + 2*i, 2 );
                    b = double( color & 0x001F ) / 31.0;
                    g = double( ( color & 0x07E0 ) >> 5 ) / 63.0;
                    r = double( ( color & 0xF800 ) >> 11 ) / 31.0;
                    break;
                case 24: 
                    b = double( *(source + 3*i) ) / 255.0;
                    g = double( *(source + 3*i+1) ) / 255.0;
                    r = double( *(source + 3*i+2) ) / 255.0;
                    break;
                case 32:
                    b = double( *(source + 4*i) ) / 255.0;
                    g = double( *(source + 4*i+1) ) / 255.0;
                    r = double( *(source + 4*i+2) ) / 255.0;
                    break;
            }
            switch( dest_depth )
            {
                case 16:
                    color = WORD(b*31.0);
                    color += (WORD(g*63.0)<<5);
                    color += (WORD(r*31.0)<<11);
                    memcpy( dest + 2*i, &color, 2 );
                    break;
                case 24:
                    *( dest + 3*i ) = (uchar)(255*b);
                    *( dest + 3*i+1 ) = (uchar)(255*g);
                    *( dest + 3*i+2 ) = (uchar)(255*r);
                    break;
                case 32:
                    *( dest + 4*i ) = (uchar)(255*b);
                    *( dest + 4*i+1 ) = (uchar)(255*g);
                    *( dest + 4*i+2 ) = (uchar)(255*r);
                    *( dest + 4*i+3 ) = 0;
                    break;
            }
        }
    }
}

#ifdef WIN32
void GLViewer_Widget::translateBackgroundToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS )
{
    QImage buf; 

    if( aViewerCS && aEMFCS && isLoadBackground && buf.load( myBackgroundFile ) )
    {       
        double left, top, right, bottom;
        getBackgroundRectInViewerCS( left, top, right, bottom );

        double aRot = aViewerCS->getRotation();

        double lx = left, ly = top;
        aViewerCS->transform( *aEMFCS, lx, ly );

        aViewerCS->setRotation( 0.0 ); //we switch off the rotation of CS

        aViewerCS->transform( *aEMFCS, left, top );
        aViewerCS->transform( *aEMFCS, right, bottom );
        
        int w = buf.width(), 
            h = buf.height();

        HDC aScrDC = GetDC( 0 );
        HDC aCompDC = CreateCompatibleDC( aScrDC );
        HBITMAP aBMP = CreateCompatibleBitmap( aScrDC, w, h );

        BITMAP aBitInfo;
        GetObject ( aBMP, sizeof(BITMAP), &aBitInfo );
        int depth = aBitInfo.bmBitsPixel; //how many bits represent a color of one pixel

        int aLineSize = w*depth;
        int dw = aLineSize % 32; //scanline word aligning

        if( dw )
            aLineSize += 32-dw;

        aLineSize /= 8;

        BYTE* bits = new BYTE[aLineSize*h];
        memset( bits, 0, aLineSize*h );
        uchar* aLine = NULL;

        for( int i=0; i<h; i++ )
        {
            aLine = buf.scanLine( i );
            DecodeScanLine( w, bits+aLineSize*i, depth, aLine, buf.depth() );
        }

        SetBitmapBits( aBMP, aLineSize*h, bits );

        HGDIOBJ old = SelectObject( aCompDC, aBMP ); 

        XFORM aTrans;
        GetWorldTransform( dc, &aTrans );
        XFORM aRotTrans = aTrans;
        double a = aRotTrans.eM11, 
               b = aRotTrans.eM12, 
               c = aRotTrans.eM21, 
               d = aRotTrans.eM22;

        aRotTrans.eM11 = a*cos( aRot )-b*sin( aRot ); //we multiply the current matrix with the rotation matrix 
        aRotTrans.eM12 = a*sin( aRot )+b*cos( aRot );
        aRotTrans.eM21 = c*cos( aRot )-d*sin( aRot );
        aRotTrans.eM22 = c*sin( aRot )+d*cos( aRot );

        a = aRotTrans.eM11; 
        b = aRotTrans.eM12; 
        c = aRotTrans.eM21; 
        d = aRotTrans.eM22;

        double det = a*d-b*c, //now we find the invert matrix 
               newa = d/det,
               newb = -c/det,
               newc = -b/det,
               newd = a/det;

        a = newa; b = newb; c = newc; d = newd;

        aRotTrans.eDx = lx -(a*left+c*top); //we find the dx and dy translating (left,top)->(lx,ly) -                                           
        aRotTrans.eDy = ly -(b*left+d*top); //the real image of left-top corner of picture

        SetWorldTransform( dc, &aRotTrans );
        int res = StretchBlt( dc, left, top, right-left, bottom-top, aCompDC, 0, 0, w, h, SRCCOPY );
        SetWorldTransform( dc, &aTrans );

        SelectObject( aCompDC, old );

        ReleaseDC( 0, aScrDC );
        DeleteDC( aCompDC );
        DeleteObject( aBMP );
        delete[] bits;

        aViewerCS->setRotation( aRot );
    }
}
#endif
