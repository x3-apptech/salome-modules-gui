// File:      GLViewer_Compass.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

//#include <GLViewerAfx.h>
#include "GLViewer_Compass.h"
#include "GLViewer_Drawer.h"

GLViewer_Compass::GLViewer_Compass ( const QColor& color, const int size, const Position pos,
                               const int WidthTop, const int WidthBottom, const int HeightTop,
                               const int HeightBottom ){
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

GLViewer_Compass::~GLViewer_Compass()
{
    delete myFont;
}

GLViewer_TexFont* GLViewer_Compass::getFont()
{ 
    if(!isGenereted) 
    {
        myFont->generateTexture();
        isGenereted = true;
    }    
    return myFont;
}

void GLViewer_Compass::setFont( QFont theFont )
{
    delete myFont;
    myFont = new GLViewer_TexFont( &theFont );
} 
