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

/*! Class GLViewer_MimeSource
* Needs for a work with QClipboard
*/

class GLVIEWER_API GLViewer_MimeSource: public QMimeSource
{
public:
  GLViewer_MimeSource():QMimeSource(){};
  ~GLViewer_MimeSource();
  
  //! Translate objects to byte array
  bool                                setObjects( QValueList<GLViewer_Object*> );
  //! Gets objects from byte array
  /*If you want to use new class, following two method must be redefined*/
  static QValueList<GLViewer_Object*> getObjects( QByteArray, QString theType);
  //! Get object from byte array
  /*If you want to use new class, following two method must be redefined*/
  static GLViewer_Object*             getObject( QByteArray, QString theType);
  
  // Redefined methods
  virtual const char*                 format( int theIndex = 0 ) const;
  virtual QByteArray                  encodedData( const char* ) const;
  
private:
  QByteArray                          myByteArray;
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif
