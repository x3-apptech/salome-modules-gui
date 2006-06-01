//  Copyright (C) 2005 OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  Author : OPEN CASCADE
//

// File:      GLViewer_MimeSource.h
// Created:   November, 2004

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

/*! 
  \class GLViewer_MimeSource
  Needs for a work with QClipboard
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
