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

#include "GLViewer_Object.h"
#include "GLViewer_Drawer.h"
#include "GLViewer_AspectLine.h"
#include "GLViewer_Text.h"
#include "GLViewer_Group.h"

#include <SUIT_DataOwner.h>

/*!
  Default constructor
*/
GLViewer_Object::GLViewer_Object()
{
  myXScale = 1.0; 
  myYScale = 1.0;
  myXGap = 0;
  myYGap = 0;
  myZoom = 1.0;

  myIsHigh = GL_FALSE;
  myIsSel = GL_FALSE;
  
  myRect = new GLViewer_Rect();;  
  myUpdateRect = new GLViewer_Rect();;  
  myGLText = new GLViewer_Text( 0, 0, 0, QColor(0,0,0) );

  myAspectLine = new GLViewer_AspectLine();
  myType = "GLViewer_Object";

  myOwner = NULL;
  myDrawer = NULL;

  myIsVisible = true;

  isToolTipHTML = false;  

  myGroup = NULL;
}

/*!
  Destructor
*/
GLViewer_Object::~GLViewer_Object()
{
  if( myRect )
    delete myRect;

  if( myUpdateRect )
    delete myUpdateRect;

  if( myGLText )
    delete myGLText;

  if( myAspectLine )
    delete myAspectLine;
}

/*!
  \return priority of object
*/
int GLViewer_Object::getPriority() const
{
    return myDrawer ? myDrawer->getPriority() : 0;
}

/*!
  \return true if object is inside rectangle
  \param theRect - rectangle
*/
GLboolean GLViewer_Object::isInside( GLViewer_Rect theRect )
{
    return theRect.toQRect().contains( myRect->toQRect() );
}

/*!
  Sets zoom factor
  \param zoom - zoom factor
*/
GLboolean GLViewer_Object::setZoom( GLfloat zoom, bool, bool )
{
    if( myZoom == zoom )
        return GL_FALSE;

    myZoom = zoom;
    return GL_TRUE;
}

/*!
  Performs zoom change by step
  \param zoomIn - to increase to decrease zoom
*/
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

    return setZoom( newZoom, true );
}

/*!
  Codes object as byte copy
  \return byte array
*/
QByteArray GLViewer_Object::getByteCopy()
{
    int i = 0;
    int anISize = sizeof( int );

    const char* aTypeStr = myType.toLatin1().constData();
    const char* aToolTipStr = myToolTipText.toLatin1().constData();

    int aTypeLength = myType.length();
    int aToolTipLength = myToolTipText.length();


    QByteArray aGLText = myGLText->getByteCopy();
    QByteArray aAspect = myAspectLine->getByteCopy();
    
    float aRectData[8];
    aRectData[ 0 ] = myRect->left();
    aRectData[ 1 ] = myRect->top();
    aRectData[ 2 ] = myRect->right();
    aRectData[ 3 ] = myRect->bottom();
    aRectData[ 4 ] = myXScale;
    aRectData[ 5 ] = myYScale;
    aRectData[ 6 ] = myXGap;
    aRectData[ 7 ] = myYGap;
    
    int sizeOf8Float = sizeof( aRectData );

    QByteArray aResult;
    aResult.resize( 2*anISize + sizeOf8Float + 
                    aTypeLength + aToolTipLength +
                    aGLText.size() + aAspect.size() );
    // puts 8 float values into the byte array
    char* aPointer = (char*)&aRectData;
    for( i = 0; i < sizeOf8Float; i++, aPointer++ )
        aResult[i] = *aPointer;
    // puts length of type string
    aPointer = (char*)&aTypeLength;
    for( ; i < anISize + sizeOf8Float; i++, aPointer++ )
        aResult[i] = *aPointer;
    // puts type string
    for( ; i < anISize + sizeOf8Float + aTypeLength; i++ )
        aResult[i] = aTypeStr[i - anISize - sizeOf8Float ];
    // puts length of tooltiptext string
    aPointer = (char*)&aToolTipLength;
    for( ; i < 2*anISize + sizeOf8Float + aTypeLength; i++, aPointer++ )
        aResult[i] = *aPointer;
    // puts tooltiptext string
    for( ; i <  2*anISize + sizeOf8Float + aTypeLength + aToolTipLength; i++ )
        aResult[ i] = aToolTipStr[i - 2*anISize - sizeOf8Float - aTypeLength];

    int aCurPos = 2*anISize + sizeOf8Float + aTypeLength + aToolTipLength;
    // adds aspect byte array
    for ( i = aCurPos; i < (int)( aCurPos + aAspect.size() ); i++ )
        aResult[i] = aAspect[i - aCurPos];

    aCurPos = aCurPos + aAspect.size();
    // adds GL text byte array
    for ( i = aCurPos; i < (int)( aCurPos + aGLText.size() ); i++ )
        aResult[i] = aGLText[i - aCurPos];    

    aCurPos += aGLText.size();
    aPointer = (char*)&myOwner;
    for( i = 0; i < sizeof( SUIT_DataOwner* ); i++, aPointer++ )
        aResult[ aCurPos + i ] = *aPointer;

    return aResult;
}

/*!
  Initialize object by byte array
  \param theArray - byte array
*/
bool GLViewer_Object::initializeFromByteCopy( QByteArray theArray )
{
    int i = 0;
    int anISize = sizeof( int );
    int aFSize = sizeof( GLfloat );
    
    float aLeft = 0, aTop = 0, aRight = 0, aBottom = 0;    

    //QString aTypeStr, aToolTipStr;
    int aTypeLength = 0, aToolTipLength = 0;

    int aSize = theArray.size();

    GLViewer_Text* aGLText = new GLViewer_Text( 0, 0, 0, QColor(255,255,255));
    int aGLTextMinSize = (aGLText->getByteCopy()).size();
    GLViewer_AspectLine* aAspectLine = new GLViewer_AspectLine();
    int aGLAspLineSize = (aAspectLine->getByteCopy()).size();

    QByteArray aGLTextArray, aAspect;
    aGLTextArray.resize( aGLAspLineSize );
    aAspect.resize( aGLAspLineSize );

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
    
    return true;        
}

/*!
  Sets object's group
  \param theGroup - group
*/
void GLViewer_Object::setGroup( GLViewer_Group* theGroup )
{
  if ( myGroup == theGroup )
    return;

  if( myGroup )
    myGroup->removeObject( this );
  
  myGroup = theGroup;
  if( theGroup )
    myGroup->addObject( this );
}

/*!
  \return object's group
*/
GLViewer_Group* GLViewer_Object::getGroup() const
{
  return myGroup;
}
