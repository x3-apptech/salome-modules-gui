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
// File:      GLViewer_BaseDrawers.cxx
// Created:   November, 2004
//#include <GLViewerAfx.h>
//
#include "GLViewer_BaseDrawers.h"
#include "GLViewer_Object.h"
#include "GLViewer_Text.h"
#include "GLViewer_AspectLine.h"
#include "GLViewer_BaseObjects.h"

// Qt includes
#include <QColor>


#ifndef WIN32
#include <GL/glx.h>
#endif

GLfloat sin_table[SEGMENTS];
GLfloat cos_table[SEGMENTS];

/*!
  Default constructor
*/
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
    myObjectType = "GLViewer_MarkerSet";
}

/*!
  Destructor
*/
GLViewer_MarkerDrawer::~GLViewer_MarkerDrawer()
{
}

/*! Draws object in GLViewer
  \param xScale - current scale along X-direction
  \param yScale - current scale along Y-direction
  \param onlyUpdate - = true if only update highlight-select information
*/
void GLViewer_MarkerDrawer::create( float xScale, float yScale, bool onlyUpdate )
{
    QList<int>::Iterator it;
    QList<int>::Iterator EndIt;
    QList<GLViewer_Object*>::Iterator anObjectIt = myObjects.begin();
    QList<GLViewer_Object*>::Iterator anEndObjectIt = myObjects.end();

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

        QList<int> aHNumbers, anUHNumbers, aSelNumbers, anUSelNumbers;
        aMarkerSet->exportNumbers( aHNumbers, anUHNumbers, aSelNumbers, anUSelNumbers );

        if( onlyUpdate )
        {
            EndIt = anUHNumbers.end();
            for( it = anUHNumbers.begin(); it != EndIt; ++it )
            {
                drawMarker( aXCoord[*it], anYCoord[*it], aRadius, colorN, anAspectLine );
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
        if( aMarkerSet->getGLText()->getText() != "" )
        {
            //float aXPos = 0, anYPos = 0;
            //aMarkerSet->getGLText()->getPosition( aXPos, anYPos );
            //drawText( aMarkerSet->getGLText()->getText(), aXPos, anYPos, colorN, &aMarkerSet->getGLText()->getFont(), aMarkerSet->getGLText()->getSeparator() );
            drawText( aMarkerSet );
        }
    }
}

/*! Draws marker
  \param theXCoord - X position
  \param theYCoord - Y position
  \param theRadius - radius
  \param theColor - color
  \param theAspectLine - line aspect
*/
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


/*!
  Default constructor
*/
GLViewer_PolylineDrawer::GLViewer_PolylineDrawer()
:GLViewer_Drawer()
{
    myObjectType = "GLViewer_Polyline";
}

/*!
  Destructor
*/
GLViewer_PolylineDrawer::~GLViewer_PolylineDrawer()
{
}

/*! Draws object in GLViewer
  \param xScale - current scale along X-direction
  \param yScale - current scale along Y-direction
  \param onlyUpdate - = true if only update highlight-select information
*/
void GLViewer_PolylineDrawer::create( float xScale, float yScale, bool onlyUpdate )
{
    QList<GLViewer_Object*>::Iterator aObjectIt = myObjects.begin();
    QList<GLViewer_Object*>::Iterator aObjectEndIt = myObjects.end();
    
    myXScale = xScale;
    myYScale = yScale;

    QColor color, colorN, colorH, colorS;
    GLViewer_AspectLine* anAspect = NULL;
    GLViewer_Polyline* aPolyline = NULL;
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

/*!
  Default constructor
*/
GLViewer_TextDrawer::GLViewer_TextDrawer()
: GLViewer_Drawer()
{
    myObjectType = "GLViewer_TextObject";
}

/*!
  Destructor
*/
GLViewer_TextDrawer::~GLViewer_TextDrawer()
{
}

/*! Draws object in GLViewer
  \param xScale - current scale along X-direction
  \param yScale - current scale along Y-direction
  \param onlyUpdate - = true if only update highlight-select information
*/
void GLViewer_TextDrawer::create( float xScale, float yScale, bool onlyUpdate )
{
    QList<GLViewer_Object*>::Iterator aObjectIt = myObjects.begin();
    QList<GLViewer_Object*>::Iterator aObjectEndIt = myObjects.end();
    
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

/*!
  Updates objects after updating font
*/
void GLViewer_TextDrawer::updateObjects()
{
    QList<GLViewer_Object*>::Iterator aObjectIt = myObjects.begin();
    QList<GLViewer_Object*>::Iterator aObjectEndIt = myObjects.end();
    for( ; aObjectIt != aObjectEndIt; aObjectIt++ )
        (*aObjectIt)->compute();
}
