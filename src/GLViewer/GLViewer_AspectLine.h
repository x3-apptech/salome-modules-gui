// File:      GLViewer_AspectLine.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_ASPECTLINE_H
#define GLVIEWER_ASPECTLINE_H

#include "GLViewer.h"

#include <qcolor.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

// Class:   GLViewer_AspectLine
// Descr:   Substitution of Prs2d_AspectLine for OpenGL

class GLVIEWER_API GLViewer_AspectLine 
{
public:
  GLViewer_AspectLine();
  GLViewer_AspectLine( int, float );
  ~GLViewer_AspectLine();
  
  void                  setLineColors( QColor nc = Qt::black,
                                       QColor hc = Qt::cyan,
                                       QColor sc = Qt::red );
  int                   setLineWidth( const float );
  int                   setLineType( const int );
  
  void                  getLineColors( QColor&, QColor&, QColor& ) const;
  float                 getLineWidth() const { return myLineWidth; };
  int                   getLineType() const { return myLineType; };
  
  QByteArray            getByteCopy() const;
  
  static GLViewer_AspectLine* fromByteCopy( QByteArray );
  
protected:
  QColor                myNColor;
  QColor                myHColor;
  QColor                mySColor;
  float                 myLineWidth;
  int                   myLineType;  // 0 - normal, 1 - strip
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
