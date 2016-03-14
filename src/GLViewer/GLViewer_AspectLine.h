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
// File:      GLViewer_AspectLine.h
// Created:   November, 2004
//
#ifndef GLVIEWER_ASPECTLINE_H
#define GLVIEWER_ASPECTLINE_H

#include "GLViewer.h"

#include <QColor>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*! 
 \Class GLViewer_AspectLine
 Substitution of Prs2d_AspectLine for OpenGL
*/

class GLVIEWER_API GLViewer_AspectLine 
{
public:
  //! A default constructor
  /* Line is Non strip
   * Normal color is white
   * Highlight color is cyan
   * Select color is red
  */
  GLViewer_AspectLine();
  //! A conctructor by type and width of line
  /*!
   \param theType equals 0 for normal line and 1 for strip line
   \param theWidth is a width of new line
  */
  GLViewer_AspectLine( int theType, float theWidth );

  //! A destructor
  ~GLViewer_AspectLine();
  
  //! A function for installing the line colors
  /*!
   \param nc is normal color
   \param hc is highlight color
   \param sc is select color
  */
  void                  setLineColors( QColor nc = Qt::black,
                                       QColor hc = Qt::cyan,
                                       QColor sc = Qt::red );
  //! A function for installing the line width
  int                   setLineWidth( const float theWidth );
  //! A function for installing the line type
  /*!
   \param theType equals 0 for normal line and 1 for strip line
  */
  int                   setLineType( const int theType );

  //! A function for getting line colors information
  /*!
   \param theNC is normal color
   \param theHC is highlight color
   \param theSC is select color
  */
  void                  getLineColors( QColor& theNC, QColor& theHC, QColor& theSC ) const;
  //! A function for getting information about line width
  float                 getLineWidth() const { return myLineWidth; };
  //! A function for getting information about line type
  int                   getLineType() const { return myLineType; };
  
  //! A function for coding object to the byte copy
  /*!
     A function is used for copy-past technollogy in copy method
   */
  QByteArray            getByteCopy() const;
  
  //! A function for decoding object from the byte copy
  /*!
     A function is used for copy-past technollogy in past method
   */
  static GLViewer_AspectLine* fromByteCopy( QByteArray );
  
protected:
  //! A normal color
  QColor                myNColor;
  //! A highlight color
  QColor                myHColor;
  //! A select color
  QColor                mySColor;
  //! A line width
  float                 myLineWidth;
  //! A line type
  /*! equals 0 for normal line and 1 for strip line */
  int                   myLineType;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
