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
// File:      GLViewer_Compass.cxx
// Created:   November, 2004
//
#include "GLViewer_Compass.h"
#include "GLViewer_Drawer.h"

/*!
  Constructor
  \param color        - a color of compass
  \param size         - a diameter of compass
  \param pos          - a position
  \param WidthTop     - width of wide part of arrow
  \param WidthBottom  - width of base part of arrow
  \param HeightTop    - height of arrow header
  \param HeightBottom - height of arrow cut on bottom
*/
GLViewer_Compass::GLViewer_Compass ( const QColor& color, const int size, const Position pos,
                               const int WidthTop, const int WidthBottom, const int HeightTop,
                               const int HeightBottom )
{
    myCol = color;
    mySize = size;
    myPos = pos;
    myArrowWidthTop = WidthTop;
    myArrowWidthBottom = WidthBottom;
    myArrowHeightTop = HeightTop;
    myArrowHeightBottom = HeightBottom;
    myIsVisible = true;
    QFont* aFont = new QFont("Times",16);
    myFont = new GLViewer_TexFont( aFont );
    isGenereted = false;
    //myFont->generateTexture();
}

/*!
  Destructor
*/
GLViewer_Compass::~GLViewer_Compass()
{
    delete myFont;
}

/*!
  Sets parameters of compass
  \param color        - a color of compass
  \param size         - a diameter of compass
  \param pos          - a position
*/
void GLViewer_Compass::setCompass( const QColor& color, const int size, const Position pos )
{
  myCol = color;
  mySize = size;
  myPos = pos;
}

/*!
  Sets visibility of compass 
  \param vis - new visibility state
*/
void GLViewer_Compass::setVisible( const bool vis )
{
  myIsVisible = vis;
}

/*!
  Sets size of compass
  \param size - new size
*/
void GLViewer_Compass::setSize( const int size )
{
  if( size > 0 )
    mySize=size;
}

/*!
  Sets arrow width top
  \param WidthTop - new arrow width top
*/
void GLViewer_Compass::setArrowWidthTop( const int WidthTop )
{
  if( WidthTop < myArrowWidthBottom || WidthTop > mySize )
    return;
  myArrowWidthTop=WidthTop;
}

/*!
  Sets arrow width bottom
  \param WidthBot - new arrow width bottom
*/
void GLViewer_Compass::setArrowWidthBottom( const int WidthBot )
{ 
  if( WidthBot > myArrowWidthTop || WidthBot < 1 )
    return;     
  myArrowWidthBottom=WidthBot;
}

/*!
  Sets arrow height top
  \param HeightTop - new arrow height top
*/
void GLViewer_Compass::setArrowHeightTop( const int HeightTop )
{
  if( HeightTop > (2*mySize-myArrowHeightBottom ) || HeightTop < 1 )
    return;
  myArrowHeightTop=HeightTop;
}

/*!
  Sets arrow height bottom
  \param HeightBot - new arrow height bottom
*/
void GLViewer_Compass::setArrowHeightBottom( const int HeightBot )
{
  if( HeightBot > ( 2*mySize-myArrowHeightTop ) || HeightBot < 1)
    return;
  myArrowHeightBottom=HeightBot;
}

/*!
  \return font of compass
*/
GLViewer_TexFont* GLViewer_Compass::getFont()
{ 
    if(!isGenereted) 
    {
        myFont->generateTexture();
        isGenereted = true;
    }    
    return myFont;
}

/*!
  Sets font of compass
  \param theFont - new font
*/
void GLViewer_Compass::setFont( QFont theFont )
{
    delete myFont;
    myFont = new GLViewer_TexFont( &theFont );
} 
