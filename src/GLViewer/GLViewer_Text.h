// File:      GLViewer_Text.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_TEXT_H
#define GLVIEWER_TEXT_H

#ifdef WNT
#include <windows.h>
#endif

#include "GLViewer.h"
#include "GLViewer_Defs.h"

#include <GL/gl.h>

#include <qfont.h>
#include <qstring.h>
#include <qcolor.h>
#include <qfontmetrics.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

// Class:   GLViewer_Text
// Descr:   Substitution of Prs3d_Text for OpenGL

class GLVIEWER_API GLViewer_Text
{
public:
  GLViewer_Text( const QString&, float xPos = 0.0, float yPos = 0.0, const QColor& color = QColor( 0, 255, 0 ) );
  GLViewer_Text( const QString&, float xPos, float yPos, const QColor& , QFont, int );
  ~GLViewer_Text();
  
  void                  setText( const QString& text ) { myText = text; }
  QString               getText() const { return myText; }
  
  void                  setPosition( float xPos, float yPos ) { myXPos = xPos; myYPos = yPos; }
  void                  getPosition( float& xPos, float& yPos ) { xPos = myXPos; yPos = myYPos; }
  
  void                  setColor( const QColor& color ) { myColor = color; }
  QColor                getColor() const { return myColor; }
  
  void                  setFont( const QFont theQFont) { myQFont = theQFont; }
  QFont                 getFont() const { return myQFont; }
  
  int                   getSeparator(){ return mySeparator; }
  void                  setSeparator( int theSep ){ mySeparator = theSep; }
  
  int                   getWidth();
  int                   getHeight();
  
  QByteArray            getByteCopy() const;
  
  static GLViewer_Text* fromByteCopy( QByteArray );
  
  DisplayTextFormat     getDisplayTextFormat() const { return myDTF; }
  void                  setTextDisplayFormat( DisplayTextFormat theDTF ) { myDTF = theDTF; }
  
protected:
  QString            myText;
  float              myXPos;
  float              myYPos;
  QColor             myColor;
  QFont              myQFont;
  int                mySeparator;
  DisplayTextFormat  myDTF;
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
