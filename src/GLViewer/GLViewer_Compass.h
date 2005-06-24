// File:      GLViewer_Compass.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_COMPASS_H
#define GLVIEWER_COMPASS_H

#include "GLViewer.h"

#include <qcolor.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

class GLViewer_TexFont;

class GLVIEWER_API GLViewer_Compass
{
public:
    enum Position { TopLeft, TopRight, BottomLeft, BottomRight };

    GLViewer_Compass( const QColor& color = QColor ( 0, 255, 0 ), 
                   const int size = 60,
                   const Position pos = TopRight,
                   const int WidthTop = 20,
                   const int WidthBottom = 10,
                   const int HeightTop = 25,
                   const int HeightBottom = 7 );
    ~GLViewer_Compass();

    void        setCompass( const QColor& color, const int size, const Position pos )
                                                    {myCol=color;mySize=size;myPos=pos;};
    void        setVisible( const bool vis = true );
    bool        getVisible(){ return myIsVisible; };

    void        setSize( const int size ){mySize=size;};
    int         getSize(){ return mySize; };

    void        setPos( const Position pos ){myPos=pos;};
    int         getPos(){ return myPos; };

    void        setColor( const QColor& color ){myCol=color;};
    QColor      getColor(){ return myCol; };

    void        setArrowWidthTop( const int WidthTop ){ if( WidthTop<myArrowWidthBottom || 
                                                            WidthTop>mySize ) return;
                                                        myArrowWidthTop=WidthTop; };
    int         getArrowWidthTop(){return myArrowWidthTop;};

    void        setArrowWidthBottom( const int WidthBot ){ if( WidthBot>myArrowWidthTop || WidthBot<1 )return;
                                                           myArrowWidthBottom=WidthBot; };
    int         getArrowWidthBottom(){return myArrowWidthBottom;};

    void        setArrowHeightTop( const int HeightTop ){ if( HeightTop>(2*mySize-myArrowHeightBottom ) ||
                                                              HeightTop<1 )return;
                                                          myArrowHeightTop=HeightTop;};
    int         getArrowHeightTop(){return myArrowHeightTop;};

    void        setArrowHeightBottom( const int HeightBot ){ if( HeightBot>( 2*mySize-myArrowHeightTop ) ||
                                                                 HeightBot<1)return;
                                                          myArrowHeightBottom=HeightBot;};
    int         getArrowHeightBottom(){return myArrowHeightBottom;};        

    GLViewer_TexFont* getFont();
    void            setFont( QFont theFont );

protected:
    QColor          myCol;
    int             mySize;
    int             myPos;
    bool            myIsVisible;
    int             myArrowWidthTop;
    int             myArrowWidthBottom;
    int             myArrowHeightTop;
    int             myArrowHeightBottom;
    GLViewer_TexFont*  myFont;
    bool            isGenereted;
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
