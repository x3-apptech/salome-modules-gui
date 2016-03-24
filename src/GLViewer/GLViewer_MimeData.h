// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File:      GLViewer_MimeData.h
// Created:   November, 2004
//
#ifndef GLVIEWER_MIMEDATA_H
#define GLVIEWER_MIMEDATA_H

#ifdef WIN32
#include <windows.h>
#endif

#include "GLViewer.h"

#include <QMimeData>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class GLViewer_Object;

/*! 
  \class GLViewer_MimeData
  Needs for a work with QClipboard
*/
class GLVIEWER_API GLViewer_MimeData: public QMimeData
{
public:
  GLViewer_MimeData():QMimeData(){};
  ~GLViewer_MimeData();
  
  //! Translate objects to byte array
  bool                                setObjects( QList<GLViewer_Object*> );
  //! Gets objects from byte array
  /*If you want to use new class, following two method must be redefined*/
  static QList<GLViewer_Object*>      getObjects( QByteArray, QString theType);
  //! Get object from byte array
  /*If you want to use new class, following two method must be redefined*/
  static GLViewer_Object*             getObject( QByteArray, QString theType);
  
  const char*                         format( int theIndex = 0 ) const;
  QByteArray                          encodedData( const char* ) const;
  
private:
  QByteArray                          myByteArray;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
