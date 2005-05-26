// File:      GLViewer_Detector.h
// Created:   11/16/2004 09:33:22
// Author:    Sergey ANIKIN
// Copyright: CEA 2004

#ifndef GLVIEWER_DETECTOR_H
#define GLVIEWER_DETECTOR_H

#include <GLViewer_Object.h>

//================================================================
// Class       : GLViewer_Segment
// Description : segment for 2d detection
//================================================================
class GLVIEWER_EXPORT GLViewer_Segment
{
public:
  GLViewer_Segment( const GLViewer_Pnt& thePnt1, 
                    const GLViewer_Pnt& thePnt2 );
  // Ordinary segment construction

  GLViewer_Segment( const GLViewer_Pnt& thePnt, 
                    const GLfloat theA, 
                    const GLfloat theB,
                    const GLfloat theC );
  // Construction of a ray with given equation Ax + By + C = 0

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

//================================================================
// Class       : GLViewer_Poly
// Description : polygon for 2d detection
//================================================================
class GLVIEWER_EXPORT GLViewer_Poly 
{
public:
  GLViewer_Poly( const GLViewer_PntList* thePoints );
  virtual ~GLViewer_Poly();


  int               Count() const { return myPoints->count(); }
  virtual bool      IsIn( const GLViewer_Pnt& thePnt ) const;
  // Detects if a point lies inside this polygon
  
  virtual bool      IsCovers( const GLViewer_Poly& thePoly ) const;
  // Detect if a other polygon covers this polygon

  virtual bool      IsCovers( const GLViewer_Rect& theRect ) const;
  // likes the above function

  virtual bool      HasIntersection( const GLViewer_Segment& theSegment ) const;
  // Detects intersection of this polygon with a segment or a ray

private:
  GLViewer_PntList* myPoints;
};

#endif

#ifdef _MSC_VER
#pragma once
#endif 
