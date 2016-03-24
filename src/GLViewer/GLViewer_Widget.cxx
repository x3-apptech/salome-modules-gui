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

#include "GLViewer_Widget.h"
#include "GLViewer_ViewPort2d.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_Grid.h"
#include "GLViewer_CoordSystem.h"
#include "GLViewer_ViewFrame.h"

#include <cmath>

#include <QEvent>
#include <QPaintEvent>
#include <QRect>
#include <QFile>
#include <QImage>
#include <QApplication>
#include <QToolTip>

/*!
  A constructor
  Parameters using for QOGLWidget as is 
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

/*!
  Destructor
*/
GLViewer_Widget::~GLViewer_Widget()
{
}

/*!
  \return offset parameters of Window in OpenGL global scene
*/
void GLViewer_Widget::getPan( GLfloat& xPan, GLfloat& yPan, GLfloat& zPan )
{
  xPan = myXPan;
  yPan = myYPan;
  zPan = myZPan;
}

/*!
  A function for installing the  offset parameters of Window in OpenGL global scene
*/
void GLViewer_Widget::setPan( GLfloat xPan, GLfloat yPan, GLfloat zPan )
{
  myXPan = xPan;
  myYPan = yPan;
  myZPan = zPan;
}

/*!
  \return scales on OpenGL scene along 3 directions in 2d scene zScale = 1.0
*/
void GLViewer_Widget::getScale( GLfloat& xScale, GLfloat& yScale, GLfloat& zScale )
{
  xScale = myXScale;
  yScale = myYScale;
  zScale = myZScale;
}

/*!
  A function for installing the scales of OpenGL scene
*/
void GLViewer_Widget::setScale( GLfloat xScale, GLfloat yScale, GLfloat zScale )
{
  if ( xScale > 0 && yScale > 0 && zScale > 0 )
  {
    myXScale = xScale;
    myYScale = yScale;
    myZScale = zScale;
  }
}

/*!
  \return start point of curren rotation of Window in OpenGL global scene
*/
void GLViewer_Widget::getRotationStart( GLfloat& rotationStartX,
                                        GLfloat& rotationStartY,
                                        GLfloat& rotationStartZ )
{
    rotationStartX = myRotationStartX;
    rotationStartY = myRotationStartY;
    rotationStartZ = myRotationStartZ;
}

/*!
  A function for installing the rotation angle of Window in OpenGL global scene in degree (Only in 2D)
*/
void GLViewer_Widget::setRotationStart( GLfloat rotationStartX,
                                        GLfloat rotationStartY,
                                        GLfloat rotationStartZ )
{
    myRotationStartX = rotationStartX;
    myRotationStartY = rotationStartY;
    myRotationStartZ = rotationStartZ;
}

/*!
  \return parameters of rotation
  \param rotationAngle - angle
  \param rotationCenterX - center x
  \param rotationCenterY - center y
  \param rotationCenterZ - center z
*/
void GLViewer_Widget::getRotation( GLfloat& rotationAngle,
                                   GLfloat& rotationCenterX,
                                   GLfloat& rotationCenterY,
                                   GLfloat& rotationCenterZ )
{
    rotationAngle = myRotationAngle;
    rotationCenterX = myRotationCenterX;
    rotationCenterY = myRotationCenterY;
    rotationCenterZ = myRotationCenterZ;
}

/*!
  Sets parameters of rotation
  \param rotationAngle - angle
  \param rotationCenterX - center x
  \param rotationCenterY - center y
  \param rotationCenterZ - center z
*/
void GLViewer_Widget::setRotation( GLfloat rotationAngle,
                                   GLfloat rotationCenterX,
                                   GLfloat rotationCenterY,
                                   GLfloat rotationCenterZ )
{
    myRotationAngle = rotationAngle;
    myRotationCenterX = rotationCenterX;
    myRotationCenterY = rotationCenterY;
    myRotationCenterZ = rotationCenterZ;
}


/*!
  Sets image as background
  \param filename - name of file
*/
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, myBackgroundSize , myBackgroundSize, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            pixels);

        delete[] pixels;        
    }
}

/*!
  Adds tooltip
  \param theString - tooltip text
  \param theRect - tooltip rectangle
*/
void GLViewer_Widget::addToolTip( QString theString, QRect theRect )
{
    myToolTipRect = theRect;
    setToolTip(theString);
    //QToolTip::add( this, myToolTipRect, theString );
}

/*!
  Removes tooltip
*/
void GLViewer_Widget::removeToolTip()
{
    setToolTip("");
    //QToolTip::remove( this, myToolTipRect );
}

/*!
  Initialization (redefined virtual from QGLWidget)
*/
void GLViewer_Widget::initializeGL()
{
    setAutoBufferSwap( true );

    glShadeModel(GL_FLAT);
    
    //get image
    QImage buf; 
    QString aPicturePath = getenv("GLViewer__Background_Picture");
    
    if ( !aPicturePath.isEmpty() && buf.load( aPicturePath ) ) 
    {  // Load first image from file
        isLoadBackground = true;
        setBackground( aPicturePath );       
        
    }
    
    else
        isLoadBackground = false;
}

/*!
  Paints content
*/
void GLViewer_Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();    

    glRotatef( myRotationAngle, myRotationCenterX, myRotationCenterY, myRotationCenterZ );
    glScalef( myXScale, myYScale, myZScale );
    glTranslatef( myXPan, myYPan, myZPan );
    
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

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }


    GLViewer_Grid* grid = myViewPort->getGrid();
    if( grid )
        grid->draw();

    GLViewer_Viewer2d* v = ( GLViewer_Viewer2d* )getViewPort()->getViewFrame()->getViewer();
    if( !isExportMode )
        v->updateDrawers( GL_FALSE, myXScale, myYScale );
    else
        v->repaintView( getViewPort()->getViewFrame() );
}

/*!
  Resets OpenGl parameters after resize
  \param w - new width
  \param h - new height
*/
void GLViewer_Widget::resizeGL( int w, int h )
{

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

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  GLfloat w_c = w / 2., h_c = h / 2.; 

  gluOrtho2D( -w_c, w_c, -h_c, h_c ); 

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity(); 
}

/*!
  Provides repaint in export mode
*/
void GLViewer_Widget::exportRepaint()
{
    isExportMode = true;

    paintGL();

    isExportMode = false;
}

/*!
  Custom paint event handler
*/
void GLViewer_Widget::paintEvent( QPaintEvent* e )
{
  QApplication::sendEvent( myViewPort, e );
}

/*!
  Custom mouse move event handler
*/
void GLViewer_Widget::mouseMoveEvent( QMouseEvent* e )
{
  QApplication::sendEvent( myViewPort, e );
}

/*!
  Custom mouse press event handler
*/
void GLViewer_Widget::mousePressEvent( QMouseEvent* e )
{
  QApplication::sendEvent( myViewPort, e );
}

/*!
  Custom mouse release event handler
*/
void GLViewer_Widget::mouseReleaseEvent( QMouseEvent* e )
{
  QApplication::sendEvent( myViewPort, e );
}

/*!
  Custom enter event handler
*/
void GLViewer_Widget::enterEvent( QEvent* e )
{
  updateGL();
}

/*!
  Custom leave event handler
*/
void GLViewer_Widget::leaveEvent( QEvent* e )
{
  updateGL();
}

/*!
  Custom leave event handler
*/
bool GLViewer_Widget::event ( QEvent* e )
{
  if (e->type() == QEvent::ToolTip) {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
    if ( myToolTipRect.contains(helpEvent->pos()) )
      QToolTip::showText(helpEvent->globalPos(), toolTip());
  }
  return QGLWidget::event(e);
}

/*!
  \return the hex code of digit < 16
  \param c - digit
*/
inline char hex( uchar c )
{
  if( c<=9 )
    return '0'+c;
  else if( c < 16 )
    return 'a' + c - 10;

  return ' ';
}

/*!
  Translates part of image inside rectangle from w1 to w2 and from h2 to h1 to PS format
  \param hFile - PostScript file 
  \param image - image to be tarnslated
  \param w1 - x start position
  \param w2 - x end position
  \param h1 - y start position
  \param h2 - y end position
  \param aViewerCS - viewer co-ordinate system
  \param aPSCS - paper co-ordinate system
  \param a
  \param b
  \param c
  \param d
  \param dw
  \param dh
*/
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

    hFile.write( aBuffer.toLatin1() );
  }
}

/*!
  \return background rectangle in viewer CS
*/
void GLViewer_Widget::getBackgroundRectInViewerCS( double& left, double& top, double& right, double& bottom )
{
  left = -myIW/2; right = myIW/2; 
  top = myIH/2; bottom = -myIH/2;
}

/*!
  Translates background to PostScript
  \param hFile - PostScript file 
  \param aViewerCS - viewer co-ordinate system
  \param aPSCS - paper co-ordinate system
*/
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
            AddImagePart( hFile, buf, 0, buf.width()-1, qMax( k-dh+1, 0 ), k,
                          aViewerCS, aPSCS, a, b, c, d, dx, dy-(buf.height()-1-k) );
    }
}

/*!
  Translates image line with one color depth to line with other depth
*/
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
/*!
  Translates background to EMF
  \param dc - descriptor of EMF
  \param aViewerCS - viewer co-ordinate system
  \param aPSCS - paper co-ordinate system
*/
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
