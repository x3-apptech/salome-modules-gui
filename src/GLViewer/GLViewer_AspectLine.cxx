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
// File:      GLViewer_AspectLine.cxx
// Created:   26/05/2005 3:17:00 PM
//
#include "GLViewer_AspectLine.h"

/*!
  Default constructor
*/
GLViewer_AspectLine::GLViewer_AspectLine()
{
    myNColor = QColor( 255, 255, 255 );
    myHColor = QColor( 0, 255, 255 );
    mySColor = QColor( 255, 0, 0 );

    myLineWidth = 1.0;
    myLineType = 0;
}

/*!
  Constructor
  \param type: 0 for normal line and 1 for strip line
  \param width - width of line
*/
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

/*!
  Destructor
*/
GLViewer_AspectLine::~GLViewer_AspectLine()
{
}

/*!
  Sets line color
  \param nc - normal color
  \param hc - hilighting color
  \param sc - selection color
*/
void GLViewer_AspectLine::setLineColors( QColor nc, QColor hc, QColor sc )
{
    myNColor = nc;
    myHColor = hc;
    mySColor = sc;
}

/*!
  Sets type of line
  \param type: 0 for normal line and 1 for strip line
*/
int GLViewer_AspectLine::setLineType( const int type )
{
    if( type == 1 || type == 0 )
    {
        myLineType = type;
        return 0;
    }
    return 1;
}

/*!
  Sets width of line
  \param width - new width of line
*/
int GLViewer_AspectLine::setLineWidth( const float width )
{
    if( width > 0 )
    {
        myLineWidth = width;
        return 0;
    }
    return 1;
}

/*!
  \return colors of line
  \param nc - variable for normal color
  \param hc - variable for hilighting color
  \param sc - variable for selection color
*/
void GLViewer_AspectLine::getLineColors( QColor& nc, QColor& hc, QColor& sc ) const
{
    nc = myNColor;
    hc = myHColor;
    sc = mySColor;
}

/*!
  \return binary representation of line aspect
*/
QByteArray GLViewer_AspectLine::getByteCopy() const
{
    int anISize = sizeof( int );
    int aFSize = sizeof( float );
    int aNR = myNColor.red(), aNG = myNColor.green(), aNB = myNColor.blue();
    int aHR = myHColor.red(), aHG = myHColor.green(), aHB = myHColor.blue();
    int aSR = mySColor.red(), aSG = mySColor.green(), aSB = mySColor.blue();

    QByteArray aResult;
    aResult.resize( anISize * 10 + aFSize );

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

/*!
  Sets line aspect from binary representation
*/
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
