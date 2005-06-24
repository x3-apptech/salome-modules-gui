// File:      GLViewer_MimeSource.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_MIMESOURCE_H
#define GLVIEWER_MIMESOURCE_H

#ifdef WNT
#include <windows.h>
#endif

#include "GLViewer.h"

#include <qmime.h>
#include <qvaluelist.h>

#ifdef WNT
#pragma warning( disable:4251 )
#endif

class GLViewer_Object;

// Class:   GLViewer_MimeSource
// Descr:   Needs for a work with QClipboard

class GLVIEWER_API GLViewer_MimeSource: public QMimeSource
{
public:
  GLViewer_MimeSource():QMimeSource(){};
  ~GLViewer_MimeSource();
  
  bool                                setObjects( QValueList<GLViewer_Object*> );
  static QValueList<GLViewer_Object*> getObjects( QByteArray, QString );
  static GLViewer_Object*             getObject( QByteArray, QString );
  
  virtual const char*                 format( int theIndex = 0 ) const;
  virtual QByteArray                  encodedData( const char* ) const;
  
private:
  QByteArray                          myByteArray;
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
