// File:      GLViewer_Object.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/***************************************************************************
**  Class:   GLViewer_Object
**  Descr:   OpenGL Object
**  Module:  GLViewer
**  Created: UI team, 02.09.02
****************************************************************************/

#include "GLViewer_Object.h"
#include "GLViewer_Drawer.h"

#include "GLViewer_Group.h"

#include <qfontmetrics.h>
#include <qstringlist.h>

#include <cmath>
using namespace std;

#define CONSTANT_FOR_RECT 10

/***************************************************************************
**  Class:   GLViewer_Text
**  Descr:   Substitution of Prs3d_Text for OpenGL
**  Module:  GLViewer
**  Created: UI team, 10.07.03
****************************************************************************/

GLViewer_Text::GLViewer_Text( const QString& text, float xPos, float yPos, const QColor& color )
{
  myText = text;
  myXPos = xPos;
  myYPos = yPos;
  myColor = color;
  myQFont = QFont::defaultFont();
  mySeparator = 2;
  myDTF = DTF_BITMAP;
}

GLViewer_Text::GLViewer_Text( const QString& text, float xPos, float yPos, const QColor& color, QFont theFont, int theSeparator )
{
  myText = text;
  myXPos = xPos;
  myYPos = yPos;
  myColor = color;
  myQFont = theFont;
  mySeparator = theSeparator;
  myDTF = DTF_BITMAP;
}

GLViewer_Text::~GLViewer_Text()
{
}

int GLViewer_Text::getWidth()
{
    int aResult = 0;
    QFontMetrics aFM( myQFont );
    for( int i = 0; i < myText.length(); i++ )
        aResult += aFM.width( myText[i] ) + mySeparator;
    return aResult;
}

int GLViewer_Text::getHeight()
{
    QFontMetrics aFM( myQFont );
    return aFM.height();
}

QByteArray GLViewer_Text::getByteCopy() const
{
    int i;
    int aSize = 5*sizeof( int ) + myText.length();

    int aR = myColor.red();
    int aG = myColor.green();
    int aB = myColor.blue();
    const char* aStr = myText.data();

    int anISize = sizeof( int );    
    QByteArray aResult( aSize );

    char* aPointer = (char*)&myXPos;
    for( i = 0; i < anISize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&myYPos;
    for( ; i < 2*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;

    aPointer = (char*)&aR;
    for( ; i < 3*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aG;
    for( ; i < 4*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aB;
    for( ; i < 5*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;

    int aTextSize = myText.length();
    aPointer = (char*)&aTextSize;
    for( ; i < 6*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;

    for( i = 0; i < aTextSize; i++ )
        aResult[6*anISize + i] = aStr[i];

    aPointer = (char*)&mySeparator;
    for( ; i < 7*anISize + aTextSize; i++, aPointer++ )
        aResult[i] = *aPointer;

    const char* aFontStr = myQFont.toString().data();
    int aFontSize = myQFont.toString().length();

    for( i = 0; i < aFontSize; i++ )
        aResult[7*anISize + aTextSize + i] = aFontStr[i];

    return aResult;
}

GLViewer_Text* GLViewer_Text::fromByteCopy( QByteArray theBuf )
{
    int i = 0;
    int aSize = (int)theBuf.size();
    int aR = 0, aG = 0, aB = 0;

    int xPos = 0, yPos = 0;

    int anISize = sizeof( int );
    char* aPointer = (char*)&xPos;
    for ( i = 0; i < anISize; i++, aPointer++ )
        *aPointer = theBuf[i];

    aPointer = (char*)&yPos;
    for ( ; i < 2*anISize; i++, aPointer++ )
        *aPointer = theBuf[i];

    aPointer = (char*)&aR;
    for( ; i < 3*anISize; i++, aPointer++ )
        *aPointer = theBuf[i];
    aPointer = (char*)&aG;
    for( ; i < 4*anISize; i++, aPointer++ )
        *aPointer = theBuf[i];
    aPointer = (char*)&aB;
    for( ; i < 5*anISize; i++, aPointer++ )
        *aPointer = theBuf[i];

    int aTextSize = 0;
    aPointer = (char*)&aTextSize;
    for( ; i < 6*anISize; i++, aPointer++ )
        *aPointer = theBuf[i];

    QString aText;
    for( ; i < 6*anISize + aTextSize; i++ )
    {
        QChar aChar( theBuf[i] );
        aText += aChar;
    }

    int aSeparator = 0;
    aPointer = (char*)&aSeparator;
    for( ; i < 7*anISize + aTextSize; i++, aPointer++ )
        *aPointer = theBuf[i];

    QString aFontStr;
    for( ; i < aSize; i++ )
    {
        QChar aChar( theBuf[i] );
        aFontStr += aChar;
    }
    QFont aFont;

    if( !aFont.fromString( aFontStr ) )
        return NULL;    

    GLViewer_Text* aGlText = new GLViewer_Text( aText, xPos, yPos, QColor( aR,aG,aB ), aFont, aSeparator  );

    return aGlText;    
}

/***************************************************************************
**  Class:   GLViewer_CoordSystem
**  Descr:   
**  Module:  GLViewer
**  Created: UI team, 03.09.02
****************************************************************************/

GLViewer_CoordSystem::GLViewer_CoordSystem( CSType aType, double X0, double Y0, 
                                            double XUnit, double YUnit, double Rotation )
{
    setType( aType );
    setOrigin( X0, Y0 );
    setUnits( XUnit, YUnit );
    setRotation( Rotation );
}

void GLViewer_CoordSystem::getOrigin( double& x, double& y ) const
{
    x = myX0;
    y = myY0;
}

void GLViewer_CoordSystem::setOrigin( double x, double y )
{
    myX0 = x;
    myY0 = y;
}

void GLViewer_CoordSystem::getUnits( double& x, double& y ) const
{
    x = myXUnit;
    y = myYUnit;
}

void GLViewer_CoordSystem::setUnits( double x, double y )
{
    if( x>0 )
        myXUnit = x;
    else
        myXUnit = 1.0;

    if( y>0 )
        myYUnit = y;
    else
        myYUnit = 1.0;
}

double GLViewer_CoordSystem::getRotation() const
{
    return myRotation;
}

void GLViewer_CoordSystem::setRotation( double rotation )
{
    myRotation = rotation;
}

GLViewer_CoordSystem::CSType GLViewer_CoordSystem::getType() const
{
    return myType;
}

void GLViewer_CoordSystem::setType( CSType type )
{
    myType = type;
}

void GLViewer_CoordSystem::toReference( double& x, double& y )
{
    if( myType==Cartesian )
    {
        double newx = myX0 + myXUnit*x*cos(myRotation) - myYUnit*y*sin(myRotation),
               newy = myY0 + myXUnit*x*sin(myRotation) + myYUnit*y*cos(myRotation);
        x = newx;
        y = newy;
    }
    else if( myType==Polar )
    {
        double r = x, phi = y;
        x = myX0 + myXUnit*r*cos(phi+myRotation);
        y = myY0 + myXUnit*r*sin(phi+myRotation);
    }
}

void GLViewer_CoordSystem::fromReference( double& x, double& y )
{
    x = (x - myX0) / myXUnit;
    y = (y - myY0) / myYUnit;

    if( myType==Cartesian )
    {
        double newx =  x*cos(myRotation) + y*sin(myRotation),
               newy = -x*sin(myRotation) + y*cos(myRotation);
        x = newx;
        y = newy;
    }
    else if( myType==Polar )
    {
        double r = sqrt( x*x+y*y );
        double phi = 0.0;
        double eps = 1E-8, pi = 3.14159265;

        if( r>eps )
            if( fabs(x)>eps )
            {
                double arg = y/x;
                phi = atan(arg);
                if( x<0 ) // 2-nd or 4-rd quarter
                    phi+=pi;
            }
            else if( y>0 )
                phi = pi/2.0;
            else
                phi = 3*pi/2.0;

        x = r;
        y = phi-myRotation;
    }
}

void GLViewer_CoordSystem::transform( GLViewer_CoordSystem& aSystem, double& x, double& y )
{
    toReference( x, y );
    aSystem.fromReference( x, y );
}

void GLViewer_CoordSystem::getStretching( GLViewer_CoordSystem& aSystem, double& theX, double& theY )
{
    theX = myXUnit / aSystem.myXUnit;
    theY = myYUnit / aSystem.myYUnit;
}

/***************************************************************************
**  Class:   GLViewer_Object
**  Descr:   OpenGL Object
**  Module:  GLViewer
**  Created: UI team, 03.09.02
****************************************************************************/
GLViewer_Object::GLViewer_Object()
{
  myXScale = 1.0; 
  myYScale = 1.0;
  myXGap = 0;
  myYGap = 0;
  myZoom = 1.0;

  myIsHigh = GL_FALSE;
  myIsSel = GL_FALSE;
  
  myRect = new GLViewer_Rect(); 
  myGLText = new GLViewer_Text( 0, 0, 0, QColor(0,0,0) );

  myAspectLine = new GLViewer_AspectLine();
  myType = "GLViewer_Object";

  setOwner( NULL );
  setDrawer( NULL );

  myIsVisible = true;

  isToolTipHTML = false;  

  myGroup = NULL;
}

GLViewer_Object::~GLViewer_Object()
{
  if( myRect )
    delete myRect;

  if( myGLText )
    delete myGLText;

  if( myAspectLine )
    delete myAspectLine;
}

GLboolean GLViewer_Object::isInside( GLViewer_Rect theRect )
{
    return theRect.toQRect()->contains( *(myRect->toQRect()) );
}

GLboolean GLViewer_Object::setZoom( GLfloat zoom, GLboolean )
{
    if( myZoom == zoom )
        return GL_FALSE;

    myZoom = zoom;
    return GL_TRUE;
}

GLboolean GLViewer_Object::updateZoom( bool zoomIn )
{
    float newZoom;
    float step = zoomIn ? 1 : -1;
    double epsilon = 0.001;

    if( myZoom - 1 > epsilon )
        newZoom = ( myZoom * 2 + step ) / 2;
    else if( 1 - myZoom > epsilon )
        newZoom = 2 / ( 2 / myZoom - step );
    else
        newZoom = zoomIn ? 3./2. : 2./3.;

    if( newZoom < 0.01 || newZoom > 100.0 )
        return GL_FALSE;

    setZoom( newZoom, GL_TRUE );
    return GL_TRUE;
}

QByteArray GLViewer_Object::getByteCopy()
{
    int i = 0;
    int anISize = sizeof( int );
    int aFSize = sizeof( GLfloat );

    int aLeft = (int)myRect->left(),
        aTop = (int)myRect->top(),
        aRight = (int)myRect->right(),
        aBottom = (int)myRect->bottom();

    const char* aTypeStr = myType.data();
    const char* aToolTipStr = myToolTipText.data();

    int aTypeLength = myType.length();
    int aToolTipLength = myToolTipText.length();


    QByteArray aGLText = myGLText->getByteCopy();
    QByteArray aAspect = myAspectLine->getByteCopy();
    
    QByteArray aResult( 2*anISize + 8*aFSize  + 
                        aTypeLength + aToolTipLength +
                        aGLText.size() + aAspect.size() );
    
    char* aPointer = (char*)&aLeft;
    for( i = 0; i < aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aTop;
    for( ; i < 2*aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aRight;
    for( ; i < 3*aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aBottom;
    for( ; i < 4*aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;

    aPointer = (char*)&myXScale;
    for( ; i < 5*aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&myYScale;
    for( ; i < 6*aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&myXGap;
    for( ; i < 7*aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&myYGap;
    for( ; i < 8*aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;

    aPointer = (char*)&aTypeLength;
    for( ; i < anISize + 8*aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;
    for( ; i < anISize + 8*aFSize + aTypeLength; i++ )
        aResult[i] = aTypeStr[i - anISize - 8*aFSize ];

    aPointer = (char*)&aToolTipLength;
    for( ; i < 2*anISize + 8*aFSize + aTypeLength; i++, aPointer++ )
        aResult[i] = *aPointer;
    for( ; i <  2*anISize + 8*aFSize + aTypeLength + aToolTipLength; i++ )
        aResult[ i] = aToolTipStr[i - 2*anISize - 8*aFSize - aTypeLength];

    int aCurPos = 2*anISize + 8*aFSize + aTypeLength + aToolTipLength;

    for( i = aCurPos; i < aCurPos + aAspect.size(); i++ )
        aResult[i] = aAspect[i - aCurPos];

    aCurPos = aCurPos + aAspect.size();

    for( i = aCurPos; i < aCurPos + aGLText.size(); i++ )
        aResult[i] = aGLText[i - aCurPos];    

    aCurPos += aGLText.size();

    aPointer = (char*)&myOwner;
    for( i = 0; i < sizeof( GLViewer_Owner* ); i++, aPointer++ )
        aResult[ aCurPos+i ] = *aPointer;

    return aResult;
}

bool GLViewer_Object::initializeFromByteCopy( QByteArray theArray )
{
    int i = 0;
    int anISize = sizeof( int );
    int aFSize = sizeof( GLfloat );
    
    int aLeft = 0, aTop = 0, aRight = 0, aBottom = 0;    

    //QString aTypeStr, aToolTipStr;
    int aTypeLength = 0, aToolTipLength = 0;

    int aSize = theArray.size();

    GLViewer_Text* aGLText = new GLViewer_Text( 0, 0, 0, QColor(255,255,255));
    int aGLTextMinSize = (aGLText->getByteCopy()).size();
    GLViewer_AspectLine* aAspectLine = new GLViewer_AspectLine();
    int aGLAspLineSize = (aAspectLine->getByteCopy()).size();

    QByteArray aGLTextArray, aAspect( aGLAspLineSize );

    if( aSize < 2*anISize + 8*aFSize + aGLTextMinSize + aGLAspLineSize )
        return false;

    char* aPointer = (char*)&aLeft;
    for( i = 0; i < aFSize; i++, aPointer++ )
        *aPointer = theArray[i];
    aPointer = (char*)&aTop;
    for( ; i < 2*aFSize; i++, aPointer++ )
        *aPointer = theArray[i];
    aPointer = (char*)&aRight;
    for( ; i < 3*aFSize; i++, aPointer++ )
        *aPointer = theArray[i];
    aPointer = (char*)&aBottom;
    for( ; i < 4*aFSize; i++, aPointer++ )
        *aPointer = theArray[i];

    //myRect = new QRect( aLeft, aTop, aRight - aLeft, aBottom - aTop );
    myRect = new GLViewer_Rect( aLeft, aRight, aTop, aBottom );

    aPointer = (char*)&myXScale;
    for( ; i < 5*aFSize; i++, aPointer++ )
        *aPointer = theArray[i];
    aPointer = (char*)&myYScale;
    for( ; i < 6*aFSize; i++, aPointer++ )
        *aPointer = theArray[i];
    aPointer = (char*)&myXGap;
    for( ; i < 7*aFSize; i++, aPointer++ )
        *aPointer = theArray[i];
    aPointer = (char*)&myYGap;
    for( ; i < 8*aFSize; i++, aPointer++ )
        *aPointer = theArray[i];

    myIsHigh = false;
    myIsSel = false;
    myIsVisible = true;

    aPointer = (char*)&aTypeLength;
    for( ; i < anISize + 8*aFSize; i++, aPointer++ )
        *aPointer = theArray[i];
    myType = "";
    for( ; i < anISize + 8*aFSize + aTypeLength; i++ )
    {
        QChar aChar( theArray[i] );
        myType += aChar;
    }

    aPointer = (char*)&aToolTipLength;
    for( ; i < 2*anISize + 8*aFSize + aTypeLength; i++, aPointer++ )
        *aPointer = theArray[i];
    myToolTipText= "";
    for( ; i < 2*anISize + 8*aFSize + aTypeLength + aToolTipLength; i++ )
    {
        QChar aChar( theArray[i] );
        myToolTipText += aChar;
    }
    
    int aCurPos = 2*anISize + 8*aFSize + aTypeLength + aToolTipLength;
    if( aSize - aCurPos < aGLTextMinSize + aGLAspLineSize )
        return false;

    for( i = 0; i < aGLAspLineSize; i++ )
        aAspect[i] = theArray[ aCurPos + i ];
    myAspectLine = GLViewer_AspectLine::fromByteCopy( aAspect );

    aCurPos = aCurPos + aGLAspLineSize;
    aGLTextArray.resize( aSize - aCurPos );
    for( i = 0; i + aCurPos < aSize; i++ )
        aGLTextArray[i] = theArray[ aCurPos + i ];
    // replace gl_text pointer by other
    if ( myGLText )
      delete myGLText;
    myGLText = GLViewer_Text::fromByteCopy( aGLTextArray );
    
    
    /*aCurPos += aSize - aCurPos;
    aPointer = (char*)&myOwner;
    for( i = 0; i < sizeof( GLViewer_Owner* ); i++, aPointer++ )
    *aPointer = theArray[ aCurPos + i ];*/

    return true;        
}

void GLViewer_Object::setGroup( GLViewer_Group* theGroup )
{
  if( myGroup )
    myGroup->removeObject( this );
  
  myGroup = theGroup;
  if( theGroup )
    myGroup->addObject( this );
}

GLViewer_Group* GLViewer_Object::getGroup() const
{
  return myGroup;
}

GLViewer_Owner* GLViewer_Object::owner() const
{
  return myOwner;
}

void GLViewer_Object::setOwner( GLViewer_Owner* owner )
{
  myOwner = owner;
}


/***************************************************************************
**  Class:   GLViewer_MarkerSet
**  Descr:   OpenGL MarkerSet
**  Module:  GLViewer
**  Created: UI team, 03.09.02
****************************************************************************/

GLViewer_MarkerSet::GLViewer_MarkerSet( int number, float size, const QString& toolTip ) :
  GLViewer_Object(), myNumber( 0 ), myXCoord( 0 ), myYCoord( 0 )       
{
    myMarkerSize = size;
    myHNumbers.clear();
    myUHNumbers.clear();
    mySelNumbers.clear();
    myUSelNumbers.clear();
    myCurSelNumbers.clear();
    myPrevHNumbers.clear();    

    myType = "GLViewer_MarkerSet";
    myToolTipText = toolTip;
    
    setNumMarkers( number );    
}

GLViewer_MarkerSet::~GLViewer_MarkerSet()
{
    delete myRect;

    if ( myXCoord )
        delete[] myXCoord;
    if ( myYCoord )
        delete[] myYCoord;
  
    delete myAspectLine;
}

void AddCoordsToHPGL( QString& buffer, QString command, GLViewer_CoordSystem* aViewerCS, 
                      GLViewer_CoordSystem* aPaperCS, double x, double y, bool NewLine = true )
{
    if( aViewerCS && aPaperCS )
        aViewerCS->transform( *aPaperCS, x, y );

    QString temp = command + "%1, %2;";
    buffer += temp.arg( x ).arg( y );
    if( NewLine )
        buffer += ";\n";
}

void AddCoordsToPS( QString& buffer, QString command, GLViewer_CoordSystem* aViewerCS, 
                    GLViewer_CoordSystem* aPaperCS, double x, double y, bool NewLine = true )
{
    if( aViewerCS && aPaperCS )
        aViewerCS->transform( *aPaperCS, x, y );

    QString temp = "%1 %2 "+command;    
    buffer += temp.arg( x ).arg( y );
    if( NewLine )
        buffer += "\n";
}

void AddLineAspectToPS( QString& buffer, GLViewer_AspectLine* anAspect, 
                        GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPaperCS )
{
    if( anAspect )
    {
        QColor col1, col2, col3;
        anAspect->getLineColors( col1, col2, col3 );

        float aWidth = anAspect->getLineWidth();
        int aLineType = anAspect->getLineType();

        QString temp = "%1 %2 %3 setrgbcolor\n";
        double rr = 1 - double( col1.red() ) / 255.0, //color inverting
               gg = 1 - double( col1.green() ) / 255.0,
               bb = 1 - double( col1.blue() ) / 255.0;

        buffer += temp.arg( rr ).arg( gg ).arg( bb );

        double x_stretch, y_stretch;
        aViewerCS->getStretching( *aPaperCS, x_stretch, y_stretch );
        buffer += temp.arg( x_stretch * aWidth )+" setlinewidth\n";

        if( aLineType==0 ) //solid
            buffer += "[] 0 setdash\n";
        else if( aLineType==1 ) //strip
            buffer += "[2] 0 setdash\n";
    }
}

#ifdef WIN32
HPEN AddLineAspectToEMF( HDC hDC, GLViewer_AspectLine* anAspect, 
                         GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPaperCS )
{
    if( anAspect )
    {
        QColor col1, col2, col3;
        anAspect->getLineColors( col1, col2, col3 );

        double x_stretch, y_stretch;
        aViewerCS->getStretching( *aPaperCS, x_stretch, y_stretch );

        double aWidth = anAspect->getLineWidth()*x_stretch;
        int aLineType = anAspect->getLineType();

        return CreatePen( PS_SOLID, aWidth, RGB( 255-col1.red(), 255-col1.green(), 255-col1.blue() ) );
    }
    else
        return NULL;
}
#endif

bool GLViewer_MarkerSet::translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS )
{   
    int noPoints = 20;

    QString aBuffer = "newpath\n";

    AddLineAspectToPS( aBuffer, getAspectLine(), aViewerCS, aPSCS );

    for( int i=0; i<myNumber; i++ )
    {       
        aBuffer += "\n";

        double x_stretch, y_stretch;
        aViewerCS->getStretching( *aPSCS, x_stretch, y_stretch );

        double x0 = myXCoord[i],
               y0 = myYCoord[i],
               r  = myMarkerSize,
               x, y;

        for( int j=0; j<=noPoints; j++ )
        {
            x = x0 + r*cos( double(j)*2*PI/double(noPoints) );
            y = y0 + r*sin( double(j)*2*PI/double(noPoints) );          
            if( j==0 )
                AddCoordsToPS( aBuffer, "moveto", aViewerCS, aPSCS, x, y, true );               
            else
                AddCoordsToPS( aBuffer, "lineto", aViewerCS, aPSCS, x, y, true );
        }
    }
    aBuffer+="closepath\nstroke\n";

    hFile.writeBlock( aBuffer.ascii(), aBuffer.length() );

    return true;
}

bool GLViewer_MarkerSet::translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS,
                                       GLViewer_CoordSystem* aHPGLCS )
{
    int noPoints = 20;
    QString aBuffer;
    for( int i=0; i<myNumber; i++ )
    {
        aBuffer = "";

        double x_stretch, y_stretch;
        aViewerCS->getStretching( *aHPGLCS, x_stretch, y_stretch );

        double x0 = myXCoord[i],
               y0 = myYCoord[i],
               r  = myMarkerSize,
               x, y;

        AddCoordsToHPGL( aBuffer, "PA", aViewerCS, aHPGLCS, x0+r, y0 );
        aBuffer+="PD;\n";
        for( int j=1; j<=noPoints; j++ )
        {
            x = x0 + r*cos( double(j)*2*PI/double(noPoints) );
            y = y0 + r*sin( double(j)*2*PI/double(noPoints) );
            AddCoordsToHPGL( aBuffer, "PD", aViewerCS, aHPGLCS, x, y );
        }
        aBuffer+="PU;\n";

        hFile.writeBlock( aBuffer.ascii(), aBuffer.length() );
    }

    return true;
}

#ifdef WIN32
bool GLViewer_MarkerSet::translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS )
{
    int noPoints = 20;
    if( !aViewerCS || !aEMFCS )
        return false;
    
    HPEN pen = AddLineAspectToEMF( dc, getAspectLine(), aViewerCS, aEMFCS );
    HGDIOBJ old = SelectObject( dc, pen );

    for( int i=0; i<myNumber; i++ )
    {
        double x0 = myXCoord[i],
               y0 = myYCoord[i],
               r  = myMarkerSize,
               x, y;

        for( int j=0; j<=noPoints; j++ )
        {
            x = x0 + r*cos( double(j)*2*PI/double(noPoints) );
            y = y0 + r*sin( double(j)*2*PI/double(noPoints) );
            aViewerCS->transform( *aEMFCS, x, y );
            if( j==0 )
                MoveToEx( dc, x, y, NULL );
            else
                LineTo( dc, x, y );
        }
    }

    SelectObject( dc, old );
    if( pen )
        DeleteObject( pen );
    return true;
}
#endif


void GLViewer_MarkerSet::compute()
{
//  cout << "GLViewer_MarkerSet::compute" << endl;
  GLfloat xa = myXCoord[0]; 
  GLfloat xb = myXCoord[0]; 
  GLfloat ya = myYCoord[0]; 
  GLfloat yb = myYCoord[0]; 

  for ( int i = 0; i < myNumber; i++ )  
  {
    xa = QMIN( xa, myXCoord[i] );
    xb = QMAX( xb, myXCoord[i] );
    ya = QMIN( ya, myYCoord[i] );
    yb = QMAX( yb, myYCoord[i] );
  }
  
  myXGap = ( xb - xa ) / CONSTANT_FOR_RECT;
  myYGap = ( yb - ya ) / CONSTANT_FOR_RECT;

  myRect->setLeft( xa - myXGap );
  myRect->setTop( yb + myYGap ); 
  myRect->setRight( xb + myXGap );
  myRect->setBottom( ya - myYGap );
}

GLViewer_Drawer* GLViewer_MarkerSet::createDrawer()
{
//  cout << "GLViewer_MarkerSet::createDrawer" << endl;
  return myDrawer = new GLViewer_MarkerDrawer();
}


GLboolean GLViewer_MarkerSet::highlight( GLfloat x, GLfloat y, GLfloat tol, GLboolean isCircle )
{
    if( !myIsVisible )
        return false;
//  cout << "GLViewer_MarkerSet::highlight " << x <<" " << y << " " << tol << endl;
  int count = 0;
  GLfloat xdist, ydist, radius;
  QValueList<int>::Iterator it;
  QValueList<int> curHNumbers;
  bool isFound;
  GLboolean update;
  int cnt = 0;

  radius = tol - myMarkerSize / 2.;
  
  myUHNumbers += myHNumbers;
  myHNumbers.clear();

  for ( int i = 0; i < myNumber; i++ ) 
  {
    xdist = ( myXCoord[i] - x ) * myXScale;
    ydist = ( myYCoord[i] - y ) * myYScale;

//    if ( isCircle && ( xdist * xdist + ydist * ydist <= radius * radius ) ||
    if ( isCircle && ( xdist * xdist + ydist * ydist <= myMarkerSize * myMarkerSize ) ||
    !isCircle && ( abs( xdist ) <= radius && abs( ydist ) <= radius ) )
    {
      isFound = FALSE;
      count++;
      for ( it = myCurSelNumbers.begin(); it != myCurSelNumbers.end(); ++it )
        if( i == *it )
        {
          isFound = TRUE;
          curHNumbers.append( i );
        }
      
      if( !isFound )
          myHNumbers.append( i );
      else
        cnt++;
    }
  }
  myCurSelNumbers = curHNumbers;

  myIsHigh = ( GLboolean )count;
  update = ( GLboolean )( myHNumbers != myPrevHNumbers );

  myPrevHNumbers = myHNumbers;

  //cout << "GLViewer_MarkerSet::highlight complete with " << (int)myIsHigh << endl;
  return update;
}

GLboolean GLViewer_MarkerSet::unhighlight()
{
  if( !myHNumbers.isEmpty() )
  {
    myUHNumbers += myHNumbers;
    myPrevHNumbers.clear();
    myHNumbers.clear();
    //??? myCurSelNumbers.clear();
    return GL_TRUE;
  }
  
  return GL_FALSE;
}

GLboolean GLViewer_MarkerSet::select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect, GLboolean isFull,
                                      GLboolean isCircle, GLboolean isShift )
{
    if( !myIsVisible )
        return false;
//  cout << "GLViewer_MarkerSet::select " << x << " " << y << endl;
  int count = 0;
  GLfloat xdist, ydist, radius;
  QValueList<int>::Iterator it;
  QValueList<int>::Iterator it1;
  QValueList<int>::Iterator remIt;
  QValueList<int>::Iterator curIt;

  radius = tol - myMarkerSize / 2.;

  if( radius < myMarkerSize / 2.)
    radius = myMarkerSize / 2.;

  count = isShift ? mySelNumbers.count() : 0;

  myUSelNumbers = mySelNumbers;

  if ( !isShift )
  {
    mySelNumbers.clear();
    myCurSelNumbers.clear();
  }

  for ( int i = 0; i < myNumber; i++ ) 
  {
    xdist = ( myXCoord[i] - x ) * myXScale;
    ydist = ( myYCoord[i] - y ) * myYScale;

    //if ( isCircle && ( xdist * xdist + ydist * ydist <= radius * radius ) ||
    if ( isCircle && ( xdist * xdist + ydist * ydist <= myMarkerSize * myMarkerSize ) ||
          !isCircle && ( abs( xdist ) <= radius && abs( ydist ) <= radius ) )
    {
      count++;
      if ( isShift )
      {
        bool isFound = FALSE;
          for( it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
            if ( *it == i )
            {
              myUSelNumbers.append( *it );
            remIt = it;
              isFound = TRUE;
              break;
            }

          if ( !isFound )
        {
          mySelNumbers.append( i );
            myCurSelNumbers.append( i );
            for ( it1 = myHNumbers.begin(); it1 != myHNumbers.end(); ++it1 )
              if( i == *it1 )
              {
                myHNumbers.remove( it1 );
                break;
              }
      for ( it1 = myUHNumbers.begin(); it1 != myUHNumbers.end(); ++it1 )
        if( i == *it1 )
        {
          myUHNumbers.remove( it1 );
          break;
        }
        }
    else
        {
      mySelNumbers.remove( remIt );
      for ( curIt = myCurSelNumbers.begin(); curIt != myCurSelNumbers.end(); ++curIt )
        if( *curIt == *remIt)
        {
          myCurSelNumbers.remove( curIt );
          break;
        }
      for ( it1 = myHNumbers.begin(); it1 != myHNumbers.end(); ++it1 )
        if( i == *it1 )
        {
          myHNumbers.remove( it1 );
          break;
        }
      for ( it1 = myUHNumbers.begin(); it1 != myUHNumbers.end(); ++it1 )
        if( i == *it1 )
        {
          myUHNumbers.remove( it1 );
          break;
        }
        }
      }
      else
      {
    mySelNumbers.append( i );
    myCurSelNumbers.append( i );
    for ( it1 = myHNumbers.begin(); it1 != myHNumbers.end(); ++it1 )
      if( i == *it1 )
      {
        myHNumbers.remove( it1 );
        break;
      }
    for ( it1 = myUHNumbers.begin(); it1 != myUHNumbers.end(); ++it1 )
      if( i == *it1 )
          {
        myUHNumbers.remove( it1 );
        break;
      }        
      }     
    }
  }

  for( it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
    for( it1 = myUSelNumbers.begin(); it1 != myUSelNumbers.end(); ++it1 )
      if( *it == *it1 )
      {
        it1 = myUSelNumbers.remove( it1 );
        it1--;
      }
  
  myIsSel = (GLboolean)count;

//  cout << "GLViewer_MarkerSet::select complete with " << (int)myIsSel << endl;
  return myIsSel;
}

GLboolean GLViewer_MarkerSet::unselect()
{
  if( !mySelNumbers.isEmpty() )
  {
    myUSelNumbers = mySelNumbers;
    mySelNumbers.clear();
    myCurSelNumbers.clear();
    return GL_TRUE;
  }

  return GL_FALSE;
}

GLViewer_Rect* GLViewer_MarkerSet::getUpdateRect()
{
  GLViewer_Rect* rect = new GLViewer_Rect();
  
  rect->setLeft( myRect->left() + myXGap - myMarkerSize / myXScale );
  rect->setTop( myRect->top() + myYGap + myMarkerSize / myYScale ); 
  rect->setRight( myRect->right() - myXGap + myMarkerSize / myXScale );
  rect->setBottom( myRect->bottom() - myYGap - myMarkerSize / myYScale );
  //cout << " Additional tolerance " << myMarkerSize / myYScale << endl;
  //rect->setLeft( myRect->left() - myMarkerSize / myXScale );
  //rect->setTop( myRect->top() - myMarkerSize / myYScale ); 
  //rect->setRight( myRect->right() + myMarkerSize / myXScale );
  //rect->setBottom( myRect->bottom() + myMarkerSize / myYScale );
  
  return rect;
}


void GLViewer_MarkerSet::setXCoord( GLfloat* xCoord, int size )
{
  myXCoord = new GLfloat[ size ];
  for( int i = 0; i < size; i++ )
     myXCoord[i] = xCoord[i];
}

void GLViewer_MarkerSet::setYCoord( GLfloat* yCoord, int size )
{
  myYCoord = new GLfloat[ size ];
  for( int i = 0; i < size; i++ )
     myYCoord[i] = yCoord[i];
}

void GLViewer_MarkerSet::setNumMarkers( GLint number )
{
  if ( myNumber == number )
    return;
    
  if ( myXCoord && myYCoord )
  {
    delete[] myXCoord;
    delete[] myYCoord;
  }

  myNumber = number;
  myXCoord = new GLfloat[ myNumber ];
  myYCoord = new GLfloat[ myNumber ];
}

void GLViewer_MarkerSet::onSelectionDone( bool append)
{
  mySelectedIndexes.Clear();
/*
  QValueList<int>::Iterator it;
  for( it = myMarkers->mySelNumbers.begin(); it != myMarkers->mySelNumbers.end(); ++it )
  {
    mySelectedIndexes.Append( *it / 2 ); //!!!
  }
*/
  emit dvMarkersSelected( mySelectedIndexes );
}

void GLViewer_MarkerSet::onSelectionCancel()
{
  mySelectedIndexes.Clear();
  emit dvMarkersSelected( mySelectedIndexes );
}

void GLViewer_MarkerSet::exportNumbers( QValueList<int>& highlight,
                     QValueList<int>& unhighlight,
                     QValueList<int>& select,
                     QValueList<int>& unselect )
{
    highlight = myHNumbers;
    unhighlight = myUHNumbers;
    select = mySelNumbers;
    unselect = myUSelNumbers;

    myUHNumbers = myHNumbers;
}

bool GLViewer_MarkerSet::addOrRemoveSelected( int index )
{
  if( index < 0 || index > myNumber )
    return FALSE;

  int n = mySelNumbers.findIndex( index );
  if( n == -1 )
    mySelNumbers.append( index );
  else
  {
    QValueList<int>::Iterator it;
    it = mySelNumbers.at( n );
    mySelNumbers.remove( it );
    myUSelNumbers.append( index );
  }
  return TRUE;
}

void GLViewer_MarkerSet::addSelected( const TColStd_SequenceOfInteger& seq )
{
  for ( int i = 1; i <= seq.Length(); i++ )
    if( mySelNumbers.findIndex( seq.Value( i ) ) == -1 )
      mySelNumbers.append( seq.Value( i ) - 1 );
}

void GLViewer_MarkerSet::setSelected( const TColStd_SequenceOfInteger& seq )
{
//   for( QValueList<int>::Iterator it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
//     if( myUSelNumbers.findIndex( *it ) == -1 )
//       myUSelNumbers.append( *it );

  myUSelNumbers = mySelNumbers;
  mySelNumbers.clear();
    
  for ( int i = 1; i <= seq.Length(); i++ )
    mySelNumbers.append( seq.Value( i ) - 1 );
}

void GLViewer_MarkerSet::moveObject( float theX, float theY, bool fromGroup )
{
    if( !fromGroup && myGroup)
    {
      myGroup->dragingObjects( theX, theY );
      return;
    }
    for( int i = 0; i < myNumber;  i++ )
    {
        myXCoord[i] = myXCoord[i] + theX;
        myYCoord[i] = myYCoord[i] + theY;
    }
    compute();    
}

QByteArray GLViewer_MarkerSet::getByteCopy()
{
    int i = 0;
    int anISize = sizeof( GLint );
    int aFSize = sizeof( GLfloat );
    
    QByteArray aObject = GLViewer_Object::getByteCopy();

    QByteArray aResult( anISize + 2*aFSize*myNumber + aFSize + aObject.size());

    char* aPointer = (char*)&myNumber;
    for( i = 0; i < anISize; i++, aPointer++ )
        aResult[i] = *aPointer;

    aPointer = (char*)myXCoord;
    for( ; i < anISize + aFSize*myNumber; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)myYCoord;
    for( ; i < anISize + 2*aFSize*myNumber; i++, aPointer++ )
        aResult[i] = *aPointer;
    
    aPointer = (char*)&myMarkerSize;
    for( ; i < anISize + 2*aFSize*myNumber + aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;
        
    
    for( ; i < aResult.size(); i++ )
        aResult[i] = aObject[i - anISize - 2*aFSize*myNumber - aFSize];

    return aResult;
}

bool GLViewer_MarkerSet::initializeFromByteCopy( QByteArray theArray )
{
    int i = 0;
    int anISize = sizeof( GLint );
    int aFSize = sizeof( GLfloat );
    //int aBSize = sizeof( GLboolean );

    char* aPointer = (char*)&myNumber;
    for( i = 0; i < anISize; i++, aPointer++ )
        *aPointer = theArray[i];

    int aSize = theArray.size();
    if( aSize < anISize + 2*aFSize*myNumber + aFSize)
        return false;

    myXCoord = new GLfloat[myNumber];
    myYCoord = new GLfloat[myNumber];
    aPointer = (char*)myXCoord;
    for( ; i < anISize + aFSize*myNumber; i++, aPointer++ )
        *aPointer = theArray[i];
    aPointer = (char*)myYCoord;
    for( ; i < anISize + 2*aFSize*myNumber; i++, aPointer++ )
        *aPointer = theArray[i];

    aPointer = (char*)&myMarkerSize;
    for( ; i < anISize + 2*aFSize*myNumber + aFSize; i++, aPointer++ )
         *aPointer = theArray[i];
         
    int aCurIndex = anISize + 2*aFSize*myNumber + aFSize;
    QByteArray aObject( aSize - aCurIndex );
    for( ; i < aSize; i++ )
        aObject[i - aCurIndex] = theArray[i];
        

    if( !GLViewer_Object::initializeFromByteCopy( aObject ) || myType != "GLViewer_MarkerSet" )
        return false;

    myHNumbers.clear();
    myUHNumbers.clear();
    mySelNumbers.clear();
    myUSelNumbers.clear();
    myCurSelNumbers.clear();
    myPrevHNumbers.clear();

    return true;        
}

/***************************************************************************
**  Class:   GLViewer_Polyline
**  Descr:   OpenGL Polyline
**  Module:  GLViewer
**  Created: UI team, 03.09.02
****************************************************************************/

#define SECTIONS 100
#define DISTANTION 5

GLViewer_Polyline::GLViewer_Polyline( int number, float size, const QString& toolTip ) :
  GLViewer_Object(), myNumber( 0 ), myXCoord( 0 ), myYCoord( 0 )       
{
  myHighFlag = GL_TRUE;

  myHNumbers.clear();
  myUHNumbers.clear();
  mySelNumbers.clear();
  myUSelNumbers.clear();
  myCurSelNumbers.clear();
  myPrevHNumbers.clear();

  setNumber( number );

  myType = "GLViewer_Polyline";
  myToolTipText = toolTip;
}

GLViewer_Polyline::~GLViewer_Polyline()
{
  delete myRect;
  if ( myXCoord )
    delete[] myXCoord;
  if ( myYCoord )
    delete[] myYCoord;
 
  delete myAspectLine;
}

bool GLViewer_Polyline::translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS )
{
    QString aBuffer = "newpath\n";

    AddLineAspectToPS( aBuffer, getAspectLine(), aViewerCS, aPSCS );

    for( int i=0; i<myNumber; i++ )
        if( i==0 )
            AddCoordsToPS( aBuffer, "moveto", aViewerCS, aPSCS, myXCoord[i], myYCoord[i] );
        else
            AddCoordsToPS( aBuffer, "lineto", aViewerCS, aPSCS, myXCoord[i], myYCoord[i] );

    if( myIsClosed )
        AddCoordsToPS( aBuffer, "lineto", aViewerCS, aPSCS, myXCoord[0], myYCoord[0] );

    aBuffer+="closepath\nstroke\n";
    
    hFile.writeBlock( aBuffer.ascii(), aBuffer.length() );

    return true;
}

bool GLViewer_Polyline::translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS )
{
    QString aBuffer = "";
    for( int i=0; i<myNumber; i++ )
    {
        AddCoordsToHPGL( aBuffer, "PA", aViewerCS, aHPGLCS, myXCoord[i], myYCoord[i] );
        if( i==0 )
            aBuffer+="PD;\n";
    }

    if( myIsClosed )
        AddCoordsToHPGL( aBuffer, "PA", aViewerCS, aHPGLCS, myXCoord[0], myYCoord[0] );

    aBuffer+="PU;\n";
    
    hFile.writeBlock( aBuffer.ascii(), aBuffer.length() );

    return true;
}

#ifdef WIN32
bool GLViewer_Polyline::translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS )
{
    if( !aViewerCS || !aEMFCS )
        return false;
    
    HPEN pen = AddLineAspectToEMF( dc, getAspectLine(), aViewerCS, aEMFCS );
    HGDIOBJ old = SelectObject( dc, pen );

    double x, y;
    for( int i=0; i<myNumber; i++ )
    {
        x = myXCoord[i];
        y = myYCoord[i];
        aViewerCS->transform( *aEMFCS, x, y );
        if( i==0 )
            MoveToEx( dc, x, y, NULL );
        else
            LineTo( dc, x, y );
    }

    if( myIsClosed )
    {
        x = myXCoord[0];
        y = myYCoord[0];
        aViewerCS->transform( *aEMFCS, x, y );
        LineTo( dc, x, y );
    }

    SelectObject( dc, old );
    if( pen )
        DeleteObject( pen );

    return true;

    return true;
}
#endif

void GLViewer_Polyline::compute()
{
//  cout << "GLViewer_MarkerSet::compute" << endl;
  GLfloat xa = myXCoord[0]; 
  GLfloat xb = myXCoord[0]; 
  GLfloat ya = myYCoord[0]; 
  GLfloat yb = myYCoord[0]; 

  for ( int i = 0; i < myNumber; i++ )  
  {
    xa = QMIN( xa, myXCoord[i] );
    xb = QMAX( xb, myXCoord[i] );
    ya = QMIN( ya, myYCoord[i] );
    yb = QMAX( yb, myYCoord[i] );
  }

  GLfloat xGap = ( xb - xa ) / 10;
  GLfloat yGap = ( yb - ya ) / 10;

  myRect->setLeft( xa - xGap );
  myRect->setTop( yb + yGap ); 
  myRect->setRight( xb + xGap );
  myRect->setBottom( ya - yGap );
}

GLViewer_Rect* GLViewer_Polyline::getUpdateRect()
{
    GLViewer_Rect* rect = new GLViewer_Rect();

    rect->setLeft( myRect->left() - myXGap );
    rect->setTop( myRect->top() + myYGap ); 
    rect->setRight( myRect->right() + myXGap );
    rect->setBottom( myRect->bottom() - myYGap );

    return rect;
}

GLViewer_Drawer* GLViewer_Polyline::createDrawer()
{
//  cout << "GLViewer_MarkerSet::createDrawer" << endl;
    return myDrawer = new GLViewer_PolylineDrawer();
}

GLboolean GLViewer_Polyline::highlight( GLfloat x, GLfloat y, GLfloat tol, GLboolean isCircle )
{
    if( !myIsVisible )
        return false;
    GLfloat xa, xb, ya, yb, l;
    GLfloat rsin, rcos, r, ra, rb;
    GLboolean update;
    GLboolean highlighted = myIsHigh;

    myIsHigh = GL_FALSE;

    int c = 0;
    if( myIsClosed )
        c = 1;

    for( int i = 0; i < myNumber-1+c; i++ ) 
    {
        xa = myXCoord[i];
        ya = myYCoord[i];
        if( i != myNumber-1 )
        {
              xb = myXCoord[i+1];
              yb = myYCoord[i+1];
        }
        else
        {    
              xb = myXCoord[0];      
              yb = myYCoord[0];
        }

        l = sqrt( (xb-xa)*(xb-xa) + (yb-ya)*(yb-ya) );
        rsin = (yb-ya) / l;
        rcos = (xb-xa) / l;
        r = ( (x-xa)*(y-yb) - (x-xb)*(y-ya) ) / ( rsin*(ya-yb) + rcos*(xa-xb) );
        ra = sqrt( (x-xa)*(x-xa) + (y-ya)*(y-ya) );
        rb = sqrt( (x-xb)*(x-xb) + (y-yb)*(y-yb) );
        if( fabs( r ) * myXScale <= DISTANTION && ra <= l + DISTANTION && rb <= l + DISTANTION )
        {
            myIsHigh = GL_TRUE;
            break;
        }
    }

    if( !myHighFlag && myIsHigh )
        myIsHigh = GL_FALSE;
    else
        myHighFlag = GL_TRUE;

    update = ( GLboolean )( myIsHigh != highlighted );

//  cout << "GLViewer_Polyline::highlight complete with " << (int)myIsHigh << endl;
    return update;
}

GLboolean GLViewer_Polyline::unhighlight()
{
//   if( !myHNumbers.isEmpty() )
//   {
//     myUHNumbers = myHNumbers;
//     myHNumbers.clear();
//     return GL_TRUE;
//   }

  if( myIsHigh )
  {
    myIsHigh = GL_FALSE;
    return GL_TRUE;
  }

  return GL_FALSE;
}

GLboolean GLViewer_Polyline::select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect, GLboolean isFull,
                                     GLboolean isCircle, GLboolean isShift )
{
    if( !myIsVisible )
        return false;
    GLfloat xa, xb, ya, yb, l;
    GLfloat rsin, rcos, r, ra, rb;
    GLboolean update;
    GLboolean selected = myIsSel;

    myIsSel = GL_FALSE;

    int c = 0;
    if( myIsClosed )
        c = 1;

    for( int i = 0; i < myNumber-1+c; i++ ) 
    {
        xa = myXCoord[i];
        ya = myYCoord[i];
        if( i != myNumber-1 )
        {
            xb = myXCoord[i+1];
            yb = myYCoord[i+1];
        }
        else
        {
            xb = myXCoord[0];
            yb = myYCoord[0];
        }

        l = sqrt( (xb-xa)*(xb-xa) + (yb-ya)*(yb-ya) );
        rsin = (yb-ya) / l;
        rcos = (xb-xa) / l;
        r = ( (x-xa)*(y-yb) - (x-xb)*(y-ya) ) / ( rsin*(ya-yb) + rcos*(xa-xb) );
        ra = sqrt( (x-xa)*(x-xa) + (y-ya)*(y-ya) );
        rb = sqrt( (x-xb)*(x-xb) + (y-yb)*(y-yb) );
        if( fabs( r ) * myXScale <= DISTANTION && ra <= l + DISTANTION && rb <= l + DISTANTION )
        {
            myIsSel = GL_TRUE;
            break;
        }
    }

    if ( myIsSel )
    {
        myHighFlag = GL_FALSE;
        myIsHigh = GL_FALSE;
    }
    else
        myHighFlag = GL_TRUE;

    update = ( GLboolean )( myIsSel != selected );

    //  cout << "GLViewer_Polyline::select complete with " << (int)myIsSel << endl;

    //  return update;  !!!!!!!!!!!!!!!!!!!!!!!!!!! no here
    return myIsSel;
}

GLboolean GLViewer_Polyline::unselect()
{
//   if( !mySelNumbers.isEmpty() )
//   {
//     myUSelNumbers = mySelNumbers;
//     mySelNumbers.clear();
//     myCurSelNumbers.clear();
//     return GL_TRUE;
//   }

  if( myIsSel )
  {
    myIsSel = GL_FALSE;
    return GL_TRUE;
  }

  return GL_FALSE;
}

void GLViewer_Polyline::setXCoord( GLfloat* xCoord, int size )
{
  myXCoord = new GLfloat[ size ];
  for( int i = 0; i < size; i++ )
     myXCoord[i] = xCoord[i];
}

void GLViewer_Polyline::setYCoord( GLfloat* yCoord, int size )
{
  myYCoord = new GLfloat[ size ];
  for( int i = 0; i < size; i++ )
     myYCoord[i] = yCoord[i];
}

void GLViewer_Polyline::setNumber( GLint number )
{
  if ( myNumber == number )
    return;
    
  if ( myXCoord && myYCoord )
  {
    delete[] myXCoord;
    delete[] myYCoord;
  }

  myNumber = number;
  myXCoord = new GLfloat[ myNumber ];
  myYCoord = new GLfloat[ myNumber ];
}

void GLViewer_Polyline::onSelectionDone( bool append)
{
  mySelectedIndexes.Clear();
/*
  QValueList<int>::Iterator it;
  for( it = myMarkers->mySelNumbers.begin(); it != myMarkers->mySelNumbers.end(); ++it )
  {
    mySelectedIndexes.Append( *it / 2 ); //!!!
  }
*/ 
}

void GLViewer_Polyline::onSelectionCancel()
{
  mySelectedIndexes.Clear();
}

void GLViewer_Polyline::exportNumbers( QValueList<int>& highlight,
                     QValueList<int>& unhighlight,
                     QValueList<int>& select,
                     QValueList<int>& unselect )
{
  highlight = myHNumbers;
  unhighlight = myUHNumbers;
  select = mySelNumbers;
  unselect = myUSelNumbers;
}

void GLViewer_Polyline::moveObject( float theX, float theY, bool fromGroup )
{
  if( !fromGroup && myGroup)
  {
    myGroup->dragingObjects( theX, theY );
    return;
  }
  for( int i = 0; i < myNumber;  i++ )
  {
    myXCoord[i] = myXCoord[i] + theX;
    myYCoord[i] = myYCoord[i] + theY;
  }
  compute();    
}

QByteArray GLViewer_Polyline::getByteCopy()
{
    int i = 0;
    int anISize = sizeof( GLint );
    int aFSize = sizeof( GLfloat );
    int aBSize = sizeof( GLboolean );

    QByteArray aObject = GLViewer_Object::getByteCopy();

    QByteArray aResult( aFSize*myNumber*2 + anISize + 2*aBSize + aObject.size());

    char* aPointer = (char*)&myNumber;
    for( i = 0; i < anISize; i++, aPointer++ )
        aResult[i] = *aPointer;

    aPointer = (char*)myXCoord;
    for( ; i < anISize + aFSize*myNumber; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)myYCoord;
    for( ; i < anISize + 2*aFSize*myNumber; i++, aPointer++ )
        aResult[i] = *aPointer;
    
    aPointer = (char*)&myIsClosed;
    for( ; i < anISize + 2*aFSize*myNumber + aBSize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&myHighSelAll;
    for( ; i < anISize + 2*aFSize*myNumber + 2*aBSize; i++, aPointer++ )
        aResult[i] = *aPointer;

    for( ; i < aResult.size(); i++ )
        aResult[i] = aObject[i - anISize - 2*aFSize*myNumber - 2*aBSize];

    return aResult;
}


bool GLViewer_Polyline::initializeFromByteCopy( QByteArray theArray )
{
    int i = 0;
    int anISize = sizeof( GLint );
    int aFSize = sizeof( GLfloat );
    int aBSize = sizeof( GLboolean );

    char* aPointer = (char*)&myNumber;
    for( i = 0; i < anISize; i++, aPointer++ )
        *aPointer = theArray[i];

    int aSize = theArray.size();
    if( aSize < aFSize*myNumber*2 + anISize + 2*aBSize )
        return false;

    myXCoord = new GLfloat[myNumber];
    myYCoord = new GLfloat[myNumber];
    aPointer = (char*)myXCoord;
    for( ; i < anISize + aFSize*myNumber; i++, aPointer++ )
        *aPointer = theArray[i];
    aPointer = (char*)myYCoord;
    for( ; i < anISize + 2*aFSize*myNumber; i++, aPointer++ )
        *aPointer = theArray[i];

    aPointer = (char*)&myIsClosed;
    for( ; i < anISize + 2*aFSize*myNumber + aBSize; i++, aPointer++ )
         *aPointer = theArray[i];
    aPointer = (char*)&myHighSelAll;
    for( ; i < anISize + 2*aFSize*myNumber + 2*aBSize; i++, aPointer++ )
         *aPointer = theArray[i];

    int aCurIndex = anISize + 2*aFSize*myNumber + 2*aBSize;
    QByteArray aObject( aSize - aCurIndex );
    for( ; i < aSize; i++ )
        aObject[i - aCurIndex] = theArray[i];

    if( !GLViewer_Object::initializeFromByteCopy( aObject ) || myType != "GLViewer_Polyline" )
        return false;

    myHNumbers.clear();
    myUHNumbers.clear();
    mySelNumbers.clear();
    myUSelNumbers.clear();
    myCurSelNumbers.clear();
    myPrevHNumbers.clear();

    return true;        
}

/***************************************************************************
**  Class:   GLViewer_TextObject
**  Descr:   Text as Object for OpenGL
**  Module:  GLViewer
**  Created: UI team, 12.02.04
****************************************************************************/

GLViewer_TextObject::GLViewer_TextObject( const QString& theStr, float xPos, float yPos, 
                                    const QColor& color, const QString& toolTip )
                                    : GLViewer_Object()
{
    myGLText = new GLViewer_Text( theStr, xPos, yPos, color );
    myWidth = 0;
    myHeight = 0;

    myHighFlag = GL_TRUE;

    myType = "GLViewer_TextObject";
    myToolTipText = toolTip;
}
GLViewer_TextObject::~GLViewer_TextObject()
{
  if ( myGLText )
    delete myGLText;

  delete myRect;  
  delete myAspectLine;
}

bool GLViewer_TextObject::translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS )
{
    QString aText = myGLText->getText();    
    float xPos, yPos;
    myGLText->getPosition( xPos, yPos );

    QString aBuffer = "/Times-Roman findfont\n";
    aBuffer += "12 scalefont setfont\n";

    AddCoordsToPS( aBuffer, "moveto", aViewerCS, aPSCS, double(xPos), double(yPos) );
    aBuffer += "(" + aText + ") show\n";

    hFile.writeBlock( aBuffer.ascii(), aBuffer.length() );

    return true;
}

bool GLViewer_TextObject::translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS )
{
    QString aText = myGLText->getText();    
    float xPos, yPos;
    myGLText->getPosition( xPos, yPos );

    QString aBuffer = "";
    AddCoordsToHPGL( aBuffer, "PA", aViewerCS, aHPGLCS, double(xPos), double(yPos) );
    
    aBuffer = "LB" + aText + "#;";
    
    hFile.writeBlock( aBuffer.ascii(), aBuffer.length() );

    return true;
}

#ifdef WIN32
bool GLViewer_TextObject::translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS )
{
    QString aText = myGLText->getText();    
    float xPos, yPos;
    myGLText->getPosition( xPos, yPos );

    double x = double( xPos ), 
           y = double( yPos );

    aViewerCS->transform( *aEMFCS, x, y );
    const char* str = aText.ascii();

    int nHeight = 35*14;       // height of font
    int nWidth = 35*12;        // average character width
    int nEscapement = 0;       // angle of escapement
    int nOrientation = 0;      // base-line orientation angle
    int fnWeight = FW_NORMAL;  // font weight
    DWORD fdwItalic = FALSE;    // italic attribute option
    DWORD fdwUnderline = FALSE; // underline attribute option
    DWORD fdwStrikeOut = FALSE; // strikeout attribute option
    DWORD fdwCharSet = ANSI_CHARSET; // character set identifier
    DWORD fdwOutputPrecision = OUT_DEFAULT_PRECIS;  // output precision
    DWORD fdwClipPrecision = CLIP_DEFAULT_PRECIS;    // clipping precision
    DWORD fdwQuality = PROOF_QUALITY;          // output quality
    DWORD fdwPitchAndFamily = FIXED_PITCH | FF_DONTCARE;   // pitch and family
    LPCTSTR lpszFace = NULL;         // typeface name


    HFONT aFont = CreateFont( nHeight, nWidth, nEscapement, nOrientation, fnWeight, fdwItalic,
                              fdwUnderline, fdwStrikeOut, fdwCharSet, fdwOutputPrecision, 
                              fdwClipPrecision, fdwQuality, fdwPitchAndFamily, lpszFace );
    LOGBRUSH aBrushData;
    aBrushData.lbStyle = BS_HOLLOW;

    HBRUSH aBrush = CreateBrushIndirect( &aBrushData );

    HGDIOBJ old1 = SelectObject( dc, aFont );
    HGDIOBJ old2 = SelectObject( dc, aBrush );

    TextOut( dc, x, y, str, aText.length() );

    SelectObject ( dc, old1 );
    SelectObject ( dc, old2 );

    DeleteObject( aFont );

    return true;
}
#endif

GLViewer_Drawer* GLViewer_TextObject::createDrawer()
{
    myDrawer = new GLViewer_TextDrawer();
    compute();
    return myDrawer;
}

void GLViewer_TextObject::compute()
{
    float xPos, yPos;
    QString aStr = myGLText->getText();
    myGLText->getPosition( xPos, yPos );

    myWidth = myGLText->getWidth();
    myHeight = myGLText->getHeight();
    myRect->setLeft( xPos );
    myRect->setTop( yPos + myHeight  ); 
    myRect->setRight( xPos + myWidth );
    myRect->setBottom( yPos );
}

void GLViewer_TextObject::setDrawer( GLViewer_Drawer* theDrawer )
{
    myDrawer = theDrawer;
    //compute();
}

GLViewer_Rect* GLViewer_TextObject::getUpdateRect()
{    
    GLViewer_Rect* rect = new GLViewer_Rect();

    float xPos, yPos;
    QString aStr = myGLText->getText();
    myGLText->getPosition( xPos, yPos );

    rect->setLeft( myRect->left() + myXGap - myWidth / myXScale );
    rect->setTop( myRect->top() + myYGap + myHeight / myYScale );
    rect->setRight( myRect->right() - myXGap + myWidth / myXScale );
    rect->setBottom( myRect->bottom() - myYGap - myHeight / myYScale );

    return rect;
}

GLboolean GLViewer_TextObject::highlight( GLfloat theX, GLfloat theY, GLfloat theTol, GLboolean isCircle )
{
    if( !myIsVisible )
        return false;

    float xPos, yPos;
    myGLText->getPosition( xPos, yPos );

    QRect aRect;
    aRect.setLeft( (int)xPos );
    aRect.setRight( (int)(xPos + myWidth / myXScale) );
    aRect.setTop( (int)yPos );// - myHeight / myYScale );
    aRect.setBottom( (int)(yPos + myHeight / myYScale) );

    //cout << "theX: " << theX << "  theY: " << theY << endl;
    //cout << "aRect.left(): " << aRect.left() << "  aRect.right(): " << aRect.right() << endl;
    //cout << "aRect.top(): " << aRect.top() << "  aRect.bottom(): " << aRect.bottom() << endl;

    QRegion obj( aRect );
    QRegion intersection;
    QRect region;

    int l = (int)(theX - theTol);
    int r = (int)(theX + theTol);
    int t = (int)(theY - theTol);
    int b = (int)(theY + theTol);
    region.setLeft( l );
    region.setRight( r );
    region.setTop( t );
    region.setBottom( b );

    QRegion circle( l, t, (int)(2 * theTol), (int)(2 * theTol), QRegion::Ellipse );
    if( isCircle )
        intersection = obj.intersect( circle );
    else
        intersection = obj.intersect( region );
    
    if( intersection.isEmpty() )
        myIsHigh = false;
    else
        myIsHigh = true;
    
    if( !myHighFlag && myIsHigh )
        myIsHigh = GL_FALSE;
    else
        myHighFlag = GL_TRUE;

    return myIsHigh;
}

GLboolean GLViewer_TextObject::unhighlight()
{
    if( myIsHigh )
    {
        myIsHigh = GL_FALSE;
        return GL_TRUE;
    }

    return GL_FALSE;
}

GLboolean GLViewer_TextObject::select( GLfloat theX, GLfloat theY, GLfloat theTol, GLViewer_Rect rect,
                                       GLboolean isFull, GLboolean isCircle, GLboolean isShift )
{ 
    if( !myIsVisible )
        return false;

    QRegion obj( *(myRect->toQRect()) );
    QRegion intersection;
    QRect region;

    int l = (int)(theX - theTol);
    int r = (int)(theX + theTol);
    int t = (int)(theY - theTol);
    int b = (int)(theY + theTol);
    region.setLeft( l );
    region.setRight( r );
    region.setTop( t );
    region.setBottom( b );

    QRegion circle( l, t, (int)(2 * theTol), (int)(2 * theTol), QRegion::Ellipse );
    if( isCircle )
        intersection = obj.intersect( circle );
    else
        intersection = obj.intersect( region );
    
    if( intersection.isEmpty() )
        myIsSel = false;
    else
        myIsSel = true;

    if ( myIsSel )
    {
        myHighFlag = GL_FALSE;
        myIsHigh = GL_FALSE;
    }
    else
        myHighFlag = GL_TRUE;

    return myIsSel;
}

GLboolean GLViewer_TextObject::unselect()
{
    if( myIsSel )
    {
        myIsSel = GL_FALSE;
        return GL_TRUE;
    }

    return GL_FALSE;
}

void GLViewer_TextObject::moveObject( float theX, float theY, bool fromGroup )
{
  if( !fromGroup && myGroup)
  {
    myGroup->dragingObjects( theX, theY );
    return;
  }
  float aX, anY;
  myGLText->getPosition( aX, anY );
  aX += theX;
  anY += theY;
  myGLText->setPosition( aX, anY );
  compute();
}

QByteArray GLViewer_TextObject::getByteCopy()
{
    QByteArray aObject = GLViewer_Object::getByteCopy();

    return aObject;
}


bool GLViewer_TextObject::initializeFromByteCopy( QByteArray theArray )
{
    if( !GLViewer_Object::initializeFromByteCopy( theArray ) || myType != "GLViewer_TextObject" )
        return false;

    myHighFlag = true;
    return true;        
}


/***************************************************************************
**  Class:   GLViewer_AspectLine
**  Descr:   Substitution of Prs2d_AspectLine for OpenGL
**  Module:  GLViewer
**  Created: UI team, 05.11.02
****************************************************************************/

GLViewer_AspectLine::GLViewer_AspectLine()
{
    myNColor = QColor( 255, 255, 255 );
    myHColor = QColor( 0, 255, 255 );
    mySColor = QColor( 255, 0, 0 );

    myLineWidth = 1.0;
    myLineType = 0;
}

GLViewer_AspectLine::GLViewer_AspectLine( int type, float width )
{
    myNColor = QColor( 255, 255, 255 );
    myHColor = QColor( 0, 255, 255 );
    mySColor = QColor( 255, 0, 0 );

    myLineWidth = width;
    if( type == 1 || type == 0 )
        myLineType = type;
    else
        myLineType = 0;
}

GLViewer_AspectLine::~GLViewer_AspectLine()
{
}

void GLViewer_AspectLine::setLineColors( QColor nc, QColor hc, QColor sc )
{
    myNColor = nc;
    myHColor = hc;
    mySColor = sc;
}

int GLViewer_AspectLine::setLineType( const int type )
{
    if( type == 1 || type == 0 )
    {
        myLineType = type;
        return 0;
    }
    return 1;
}

int GLViewer_AspectLine::setLineWidth( const float width )
{
    if( width > 0 )
    {
        myLineWidth = width;
        return 0;
    }
    return 1;
}

void GLViewer_AspectLine::getLineColors( QColor& nc, QColor& hc, QColor& sc ) const
{
    nc = myNColor;
    hc = myHColor;
    sc = mySColor;
}

QByteArray GLViewer_AspectLine::getByteCopy() const
{
    int anISize = sizeof( int );
    int aFSize = sizeof( float );
    int aNR = myNColor.red(), aNG = myNColor.green(), aNB = myNColor.blue();
    int aHR = myHColor.red(), aHG = myHColor.green(), aHB = myHColor.blue();
    int aSR = mySColor.red(), aSG = mySColor.green(), aSB = mySColor.blue();

    QByteArray aResult( anISize * 10 + aFSize );

    int i = 0;
    
    char* aPointer = (char*)&aNR;
    for( i = 0; i < anISize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aNG;
    for( ; i < 2*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aNB;
    for( ; i < 3*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;

    aPointer = (char*)&aHR;
    for( ; i < 4*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aHG;
    for( ; i < 5*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aHB;
    for( ; i < 6*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;

    aPointer = (char*)&aSR;
    for( ; i < 7*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aSG;
    for( ; i < 8*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&aSB;
    for( ; i < 9*anISize; i++, aPointer++ )
        aResult[i] = *aPointer;
    
    aPointer = (char*)&myLineWidth;
    for( ; i < 9*anISize + aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;

    aPointer = (char*)&myLineType;
    for( ; i < 10*anISize + aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;    

    return aResult;
}

GLViewer_AspectLine* GLViewer_AspectLine::fromByteCopy( QByteArray theBytes )
{

    int anISize = sizeof( int );
    int aFSize = sizeof( float );
    int aNR = 0, aNG = 0, aNB = 0;
    int aHR = 0, aHG = 0, aHB = 0;
    int aSR = 0, aSG = 0, aSB = 0;
    int aLineType = 0;
    float aLineWidth = 0;

    int i = 0;

    char* aPointer = (char*)&aNR;
    for( i = 0; i < anISize; i++, aPointer++ )
        *aPointer = theBytes[i];
    aPointer = (char*)&aNG;
    for( ; i < 2*anISize; i++, aPointer++ )
        *aPointer = theBytes[i];
    aPointer = (char*)&aNB;
    for( ; i < 3*anISize; i++, aPointer++ )
        *aPointer = theBytes[i];

    aPointer = (char*)&aHR;
    for( ; i < 4*anISize; i++, aPointer++ )
        *aPointer = theBytes[i];
    aPointer = (char*)&aHG;
    for( ; i < 5*anISize; i++, aPointer++ )
        *aPointer = theBytes[i];
    aPointer = (char*)&aHB;
    for( ; i < 6*anISize; i++, aPointer++ )
        *aPointer = theBytes[i];

    aPointer = (char*)&aSR;
    for( ; i < 7*anISize; i++, aPointer++ )
        *aPointer = theBytes[i];
    aPointer = (char*)&aSG;
    for( ; i < 8*anISize; i++, aPointer++ )
        *aPointer = theBytes[i];
    aPointer = (char*)&aSB;
    for( ; i < 9*anISize; i++, aPointer++ )
        *aPointer = theBytes[i];

    aPointer = (char*)&aLineWidth;
    for( ; i < 9*anISize + aFSize; i++, aPointer++ )
        *aPointer = theBytes[i];

    aPointer = (char*)&aLineType;
    for( ; i < 10*anISize + aFSize; i++, aPointer++ )
        *aPointer = theBytes[i];

    GLViewer_AspectLine* anAspect = new GLViewer_AspectLine( aLineType, aLineWidth );
    anAspect->setLineColors( QColor( aNR, aNG, aNB ), 
                             QColor( aHR, aHG, aHB ), 
                             QColor( aSR, aSG, aSB ) );
    return anAspect;
}

/***************************************************************************
**  Class:   GLViewer_MimeSource
**  Descr:   Needs for a work with QClipboard
**  Module:  GLViewer
**  Created: UI team, 22.03.04
****************************************************************************/
GLViewer_MimeSource::~GLViewer_MimeSource()
{
}

bool GLViewer_MimeSource::setObjects( QValueList<GLViewer_Object*> theObjects )
{
    if( !theObjects.empty() )
    {
        QStringList aObjectsType;
        QValueList<QByteArray> aObjects;
        QValueList<GLViewer_Object*>::const_iterator anIt = theObjects.begin();
        QValueList<GLViewer_Object*>::const_iterator anEndIt = theObjects.end();

        int aObjByteSize = 0;
        for( ; anIt != anEndIt; anIt++ )
        {
            aObjects.append( (*anIt)->getByteCopy() );
            aObjByteSize += aObjects.last().size();
            aObjectsType.append( (*anIt)->getObjectType() );
        }

        int anISize = sizeof( int );
        QString aTypes = aObjectsType.join("");
        int aStrByteSize = aTypes.length();
        int aObjNum = aObjects.count();

        myByteArray = QByteArray( anISize * (1 + 2*aObjNum) + aStrByteSize + aObjByteSize );

        int anIndex = 0, j = 0;
        char* aPointer = (char*)&aObjNum;
        for( anIndex = 0; anIndex < anISize; anIndex++, aPointer++ )
            myByteArray[anIndex] = *aPointer;
        
        QStringList::const_iterator aStrIt = aObjectsType.begin();
        QStringList::const_iterator aEndStrIt = aObjectsType.end();
        for( j = 1; aStrIt != aEndStrIt; aStrIt++, j++ )
        {
            int aStrLen = (*aStrIt).length();
            aPointer = (char*)&aStrLen;
            for( ; anIndex < anISize*( 1 + j ); anIndex++, aPointer++ )
                myByteArray[anIndex] = *aPointer;
        }

        int aCurIndex = anIndex;
        const char* aStr = aTypes.data();

        for( j = 0 ; anIndex < aCurIndex + aStrByteSize; aPointer++, anIndex++, j++ )
            myByteArray[anIndex] = aStr[j];

        aCurIndex = anIndex;
        QValueList<QByteArray>::iterator anObjIt = aObjects.begin();
        QValueList<QByteArray>::iterator anEndObjIt = aObjects.end();
        for( j = 1; anObjIt != anEndObjIt; anObjIt++, j++ )
        {
            int aObjLen = (int)((*anObjIt).size());
            aPointer = (char*)&aObjLen;
            for( ; anIndex < aCurIndex + anISize*j; anIndex++, aPointer++ )
                myByteArray[anIndex] = *aPointer;
        }

        aCurIndex = anIndex;
        anObjIt = aObjects.begin();

        for( ; anObjIt != anEndObjIt; anObjIt++ )
        {
            int aObjLen = (int)((*anObjIt).size());
            for( j = 0 ; anIndex < aCurIndex + aObjLen; anIndex++, aPointer++, j++ )
                myByteArray[anIndex] = (*anObjIt)[j];
            aCurIndex = anIndex;
        }
     
        return true;
    }

    return false;
}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!NOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//If you want to use new class, following two method must be redefined
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!NOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
GLViewer_Object* GLViewer_MimeSource::getObject( QByteArray theArray, QString theType )
{
    if( !theArray.isEmpty() )
    {
        if( theType == "GLViewer_MarkerSet" )
        {
            GLViewer_MarkerSet* aObject = new GLViewer_MarkerSet(  0, (float)0.0, 0  );
            if( aObject->initializeFromByteCopy( theArray ) )
                return aObject;
        }
        else if ( theType == "GLViewer_Polyline" )
        {
            GLViewer_Polyline* aObject = new GLViewer_Polyline( 0, (float)0.0, 0 );
            if( aObject->initializeFromByteCopy( theArray ) )
                return aObject;
        }
        else if( theType == "GLViewer_TextObject" )
        {
            GLViewer_TextObject* aObject = new GLViewer_TextObject( 0, 0, 0, QColor(255,255,255), 0 );
            if( aObject->initializeFromByteCopy( theArray ) )
                return aObject;
        }
    }        
    
    return NULL;
}

QValueList<GLViewer_Object*> GLViewer_MimeSource::getObjects( QByteArray theArray, QString theType )
{
    if( !theArray.isEmpty() )
    {
        int anISize = sizeof( int );
        if( theType == "GLViewer_Objects" )
        {
            QStringList aTypeList;
            QValueList<QByteArray> aObjects;
            QValueList<GLViewer_Object*> aObjectList;

            QValueList<int> aTypeSizeList;
            QValueList<int> aObjSizeList;
            int aObjNum = 0;
            char* aPointer = (char*)&aObjNum;

            int anIndex = 0, j = 0;
            for( anIndex = 0; anIndex < anISize; anIndex++, aPointer++ )
                *aPointer = theArray[anIndex];
            
            for( j = 0; j < aObjNum; j++ )
            {
                int aTempVal = 0;
                aPointer = (char*)&aTempVal;
                for( ; anIndex < anISize*(j+2); anIndex++, aPointer++ )
                    *aPointer = theArray[anIndex];
                aTypeSizeList.append( aTempVal );
            }
            
            int aCurIndex = anIndex;
            for( j = 0; j < aObjNum; j++ )
            {
                QString aTempStr;
                for( ; anIndex < aCurIndex + aTypeSizeList[j]; anIndex++ )
                {    
                    char aLetter = theArray[anIndex];
                    aTempStr.append( aLetter );
                }
                aTypeList.append( aTempStr );
                aCurIndex = anIndex;
            }

            for( j = 0; j < aObjNum; j++ )
            {
                int aTempVal = 0;
                aPointer = (char*)&aTempVal;
                for( ; anIndex < aCurIndex + anISize*(j+1); anIndex++, aPointer++ )
                    *aPointer = theArray[anIndex];
                aObjSizeList.append( aTempVal );
            }

            aCurIndex = anIndex;
            for( j = 0; j < aObjNum; j++ )
            {
                QByteArray aTempArray(aObjSizeList[j]);
                for( ; anIndex < aCurIndex + aObjSizeList[j]; anIndex++ )
                    aTempArray[anIndex-aCurIndex] = theArray[anIndex];
                aObjects.append( aTempArray );
                aCurIndex = anIndex;
            }
            
            for( j = 0; j < aObjNum; j++ )
                aObjectList.append( getObject( aObjects[j], aTypeList[j] ) );

            return aObjectList;
        }
    }
    
    return QValueList<GLViewer_Object*>();    
}

const char* GLViewer_MimeSource::format( int theIndex ) const
{
    switch( theIndex )
    {
    case 0: return "GLViewer_Objects";
    //case 1: return "GLViewer_MarkerSet";
    //case 2: return "GLViewer_Polyline";
    //case 3: return "GLViewer_TextObject";
    default: return 0;
    }

}

QByteArray GLViewer_MimeSource::encodedData( const char* theObjectType ) const
{
    if( theObjectType == "GLViewer_Objects" )
        return myByteArray;
    
    return QByteArray();
}
