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

#include "VTKViewer_MarkerUtils.h"

// VTK Includes
#include <vtkImageData.h>

// QT Includes
#include <QFile>
#include <QImage>
#include <QString>
#include <QTextStream>

namespace VTK
{
  bool LoadTextureData( const QString& theFileName,
                        VTK::MarkerScale theMarkerScale,
                        VTK::MarkerTexture& theMarkerTexture )
  {
    theMarkerTexture.clear();

    QFile aFile( theFileName );
    if( !aFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
      return false;

    bool ok;
    int aWidth = 0, aHeight = 0;

    int aTextureIndex = theMarkerScale == VTK::MS_NONE ? 0 : (int)theMarkerScale-1;
    int aCurrentTexture = 0;

    QTextStream aTextStream( &aFile );
    while( !aTextStream.atEnd() )
    {
      QString aLine = aTextStream.readLine();
      if( aLine.isEmpty() )
      {
        aCurrentTexture++;
        continue;
      }

      if( aCurrentTexture != aTextureIndex )
        continue;

      int aLineSize = aLine.size();
      for( int i = 0; i < aLineSize; i++ )
      {
        ok = false;
        unsigned int aPixel = QString( aLine.at( i ) ).toUInt( &ok );
        if( ok )
          theMarkerTexture.push_back( aPixel );
      }
      if( aWidth == 0 )
        aWidth = aLineSize;
      aHeight++;
    }

    if( theMarkerTexture.size() != aWidth * aHeight )
      return false;

    theMarkerTexture.push_front( aWidth );
    theMarkerTexture.push_front( aHeight );
    return true;
  }

  vtkSmartPointer<vtkImageData> MakeVTKImage( const VTK::MarkerTexture& theMarkerTexture,
                                              bool theWhiteForeground )
  {
    VTK::MarkerTexture::const_iterator anIter = theMarkerTexture.begin();

    int aWidth = *anIter++;
    int aHeight = *anIter++;

    vtkSmartPointer<vtkImageData> anImageData = vtkImageData::New();
    anImageData->Delete();

    anImageData->SetExtent( 0, aWidth-1, 0, aHeight-1, 0, 0 );
    anImageData->AllocateScalars( VTK_UNSIGNED_CHAR, 4 );

    unsigned char* aDataPtr = (unsigned char*)anImageData->GetScalarPointer();

    int anId = 0;
    int aSize = aWidth * aHeight * 4;
    int aCoef = theWhiteForeground ? 1 : 0;
    while( anId < aSize )
    {
      int aValue = (*anIter++) * 255;
      aDataPtr[ anId++ ] = aValue * aCoef;
      aDataPtr[ anId++ ] = aValue * aCoef;
      aDataPtr[ anId++ ] = aValue * aCoef;
      aDataPtr[ anId++ ] = aValue;
    }

    return anImageData;
  }

  QImage ConvertToQImage( vtkImageData* theImageData )
  {
    if( theImageData->GetScalarType() != VTK_UNSIGNED_CHAR )
      return QImage();

    int extent[6];
    theImageData->GetExtent( extent );
    int width = extent[1] - extent[0] + 1;
    int height = extent[3] - extent[2] + 1;

    const int wmin = 20;
    const int hmin = 20;

    int xshift = width  < wmin ? (wmin-width)/2  : 0;
    int yshift = height < hmin ? (hmin-height)/2 : 0;
  
    QImage anImage(width < wmin ? wmin : width, height < hmin ? hmin : height, QImage::Format_ARGB32);
    anImage.fill(qRgba(255,255,255,0));
    for( int i = 0; i < height; i++ )
    {
      QRgb* bits = reinterpret_cast<QRgb*>( anImage.scanLine(i+yshift) );
      unsigned char* row = static_cast<unsigned char*>(
        theImageData->GetScalarPointer( extent[0], extent[2] + height - i - 1, extent[4] ) );
      for( int j = 0; j < width; j++ )
      {
        unsigned char* data = &row[ j*4 ];
        bits[j+xshift] = qRgba( data[0], data[1], data[2], data[3] );
      }
    }
    return anImage;
  }

  int GetUniqueId( const VTK::MarkerMap& theMarkerMap )
  {
    int anId = 0;
    while( anId++ < 100 ) {
      bool anOk = true;
      VTK::MarkerMap::const_iterator anIter = theMarkerMap.begin();
      for( ; anIter != theMarkerMap.end(); anIter++ ) {
        if( anId == anIter->first ) {
          anOk = false;
          continue;
        }
      }
      if( anOk )
        return anId;
    }
    return theMarkerMap.size() + 1;
  }
}
