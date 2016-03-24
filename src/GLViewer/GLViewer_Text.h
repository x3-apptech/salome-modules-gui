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
// File:      GLViewer_Text.h
// Created:   November, 2004
//
#ifndef GLVIEWER_TEXT_H
#define GLVIEWER_TEXT_H

#ifdef WIN32
#include <windows.h>
#endif

#include "GLViewer.h"
#include "GLViewer_Defs.h"

#include <GL/gl.h>

#include <QFont>
#include <QString>
#include <QColor>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  \class GLViewer_Text
  Substitution of Prs3d_Text for OpenGL
*/
class GLVIEWER_API GLViewer_Text
{
public:
  GLViewer_Text( const QString&, float xPos = 0.0, float yPos = 0.0, const QColor& color = QColor( 0, 255, 0 ) );
  GLViewer_Text( const QString&, float xPos, float yPos, const QColor& , QFont theFont, int theSeparator);
  ~GLViewer_Text();
  
  //! Sets text
  void                  setText( const QString& text ) { myText = text; }
  //! Returns text
  QString               getText() const { return myText; }
  
  //! Sets text position
  void                  setPosition( float xPos, float yPos ) { myXPos = xPos; myYPos = yPos; }
  //! Returns text position
  void                  getPosition( float& xPos, float& yPos ) { xPos = myXPos; yPos = myYPos; }
  
  //! Sets text color
  void                  setColor( const QColor& color ) { myColor = color; }
  //! Returns text color
  QColor                getColor() const { return myColor; }
  
  //! Sets text font
  void                  setFont( const QFont theQFont) { myQFont = theQFont; }
  //! Returns text font
  QFont                 getFont() const { return myQFont; }
  
  //! Returns text separator
  int                   getSeparator(){ return mySeparator; }
  //! Sets text separator
  void                  setSeparator( int theSep ){ mySeparator = theSep; }
  
  //! Returns text width
  int                   getWidth();
  //! Returns text height
  int                   getHeight();
  
  //! A function for coding object to the byte copy
  /*! A function is used for copy-past technollogy in copy method   */
  QByteArray            getByteCopy() const;
  
  //! A function for decoding object from the byte copy
  /*! A function is used for copy-past technollogy in past method   */
  static GLViewer_Text* fromByteCopy( QByteArray );
  
  //! Sets text format BITMAP or TEXTURE
  DisplayTextFormat     getDisplayTextFormat() const { return myDTF; }
  //! Returns text format BITMAP or TEXTURE
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

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
