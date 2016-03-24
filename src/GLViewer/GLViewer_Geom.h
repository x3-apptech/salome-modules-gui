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
// File:      GLViewer_Geom.h
// Created:   November, 2004
//
#ifndef GLVIEWER_GEOM_H
#define GLVIEWER_GEOM_H

#include "GLViewer.h"

#include <QRect>
#include <QtOpenGL>
#include <math.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*! Struct GLViewer_Pnt
* Substitution of QPoint for OpenGL
*/

struct GLVIEWER_API GLViewer_Pnt
{
public:
  GLViewer_Pnt() : myX( 0. ), myY( 0. ) {};
  GLViewer_Pnt( GLfloat theX, GLfloat theY ) : myX( theX ), myY( theY ) {}
  
  GLfloat x() const { return myX; }
  GLfloat y() const { return myY; }
  void    setX( GLfloat theX ) { myX = theX; }
  void    setY( GLfloat theY ) { myY = theY; }
  void    setXY( GLfloat theX, GLfloat theY ) { myX = theX; myY = theY; }
  void    move( GLfloat theDX, GLfloat theDY ) { myX += theDX; myY += theDY; }
  
private:
  GLfloat myX;
  GLfloat myY;
};

typedef QList<GLViewer_Pnt> GLViewer_PntList;

/*! Class  GLViewer_Rect
*  Substitution of QRect for OpenGL
*/

class GLVIEWER_API GLViewer_Rect
{
public:
  GLViewer_Rect(): myLeft(0.0), myRight(0.0), myTop(0.0), myBottom(0.0){}
  GLViewer_Rect( float theLeft, float theRight, float theTop, float theBottom )
    : myLeft(theLeft), myRight(theRight), myTop(theTop), myBottom(theBottom) {}
  GLViewer_Rect( QRect theRect ) {
    myLeft = ( float )theRect.left(); myRight = ( float )theRect.right();
    myTop = ( float )theRect.top(); myBottom = ( float )theRect.bottom(); }
  
  float       left() const { return myLeft; }
  float       right() const { return myRight; }
  float       top() const { return myTop; }
  float       bottom() const { return myBottom; }

  float       width() const { return fabs( myRight - myLeft ); }
  float       height() const { return fabs( myTop - myBottom ); }
  
  void        setLeft( float theLeft ) { myLeft = theLeft; }
  void        setRight( float theRight ) { myRight = theRight; }
  void        setTop( float theTop ) { myTop = theTop; }
  void        setBottom( float theBottom ) { myBottom = theBottom; }
  
  void        setCoords( float theLeft, float theRight, float theBottom, float theTop )
  { myLeft = theLeft; myRight = theRight; myBottom = theBottom; myTop = theTop; }
  
  //! \warning This method translate only rect format
  QRect       toQRect() { return QRect( ( int )myLeft, ( int )myBottom,
                                        ( int )( myRight - myLeft ),
                                        ( int )( myTop - myBottom ) ); }

  //! On/off empty status
  void        setIsEmpty( bool on ) { myIsEmpty = on; }
  //! Checks empty status
  bool        isEmpty() const { return myIsEmpty; }

  //! Checks null status
  bool        isNull() const { return myLeft == 0.0 && myRight == 0.0 && myBottom == 0.0 && myTop == 0.0; }
  //! Checks valid status
  bool        isValid() const { return ( myLeft < myRight && myBottom < myTop ); }

  //! Checks staus of contains point 
  bool        contains( GLViewer_Pnt pnt ) { return ( pnt.x() > left() &&
                                                      pnt.x() < right() &&
                                                      pnt.y() > bottom() &&
                                                      pnt.y() < top() ); }
  
  void        move( const float x, const float y )
                  {
                    myLeft   += x;
                    myRight  += x;
                    myTop    += y;
                    myBottom += y;
                  }

protected:
  float       myLeft;
  float       myRight;
  float       myTop;
  float       myBottom;

  bool        myIsEmpty;
};

/*! Class GLViewer_Segment
* Segment for 2d detection
*/

class GLVIEWER_API GLViewer_Segment
{
public:
  GLViewer_Segment( const GLViewer_Pnt& thePnt1, 
                    const GLViewer_Pnt& thePnt2 );
  
  //! Ordinary segment construction
  /*!Construction of a ray with given equation Ax + By + C = 0 */

  GLViewer_Segment( const GLViewer_Pnt& thePnt, 
                    const GLfloat theA, 
                    const GLfloat theB,
                    const GLfloat theC );
  ~GLViewer_Segment();

  bool              HasIntersection( const GLViewer_Segment& theOther ) const;
  // Detects intersection with another segment or ray

private:
  GLViewer_Pnt      myPnt1;
  GLViewer_Pnt      myPnt2;
  GLfloat           myA;
  GLfloat           myB;
  GLfloat           myC;
};

/*! Class  GLViewer_Poly
* Polygon for 2d detection
*/

class GLVIEWER_API GLViewer_Poly 
{
public:
  GLViewer_Poly( const GLViewer_PntList* thePoints );
  virtual ~GLViewer_Poly();

  //! Adds point to polygon
  void              AddPoint( GLViewer_Pnt& pnt ) { myPoints->append( pnt ); }

  //! Returns number of point
  int               Count() const { return myPoints->count(); }

  //! Returns true if a point lies inside this polygon
  virtual bool      IsIn( const GLViewer_Pnt& thePnt ) const;

  //! Returns true if a other polygon covers this polygon  
  virtual bool      IsCovers( const GLViewer_Poly& thePoly ) const;

  //! Likes the above function
  virtual bool      IsCovers( const GLViewer_Rect& theRect ) const;
  
  // Returns true if intersection of this polygon with a segment or a ray not empty
  virtual bool      HasIntersection( const GLViewer_Segment& theSegment ) const;

private:
  GLViewer_PntList* myPoints;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
