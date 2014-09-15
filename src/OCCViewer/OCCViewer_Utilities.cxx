// Copyright (C) 2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "OCCViewer_Utilities.h"

// KERNEL includes
#include <Basics_OCCTVersion.hxx>

// OCC Includes
#include <Image_PixMap.hxx>

// QT includes
#include <QImage>

/*! Concert QImage to OCCT pixmap*/
Handle(Image_PixMap)
imageToPixmap( const QImage& anImage )
{
  Handle(Image_PixMap) aPixmap = new Image_PixMap();
  if ( !anImage.isNull() ) {
    aPixmap->InitTrash( Image_PixMap::ImgBGRA, anImage.width(), anImage.height() );
    aPixmap->SetTopDown( Standard_True );

    const uchar* aImageBytes = anImage.bits();
      
    for ( int aLine = anImage.height() - 1; aLine >= 0; --aLine ) {
#if OCC_VERSION_LARGE > 0x06070100
      // convert pixels from ARGB to renderer-compatible RGBA
      for ( int aByte = 0; aByte < anImage.width(); ++aByte ) {
	    Image_ColorBGRA& aPixmapBytes = aPixmap->ChangeValue<Image_ColorBGRA>(aLine, aByte);

	    aPixmapBytes.b() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes.g() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes.r() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes.a() = (Standard_Byte) *aImageBytes++;
	  }
#else
	  Image_ColorBGRA* aPixmapBytes = aPixmap->EditData<Image_ColorBGRA>().ChangeRow(aLine);
	
      // convert pixels from ARGB to renderer-compatible RGBA
      for ( int aByte = 0; aByte < anImage.width(); ++aByte ) {
	    aPixmapBytes->b() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes->g() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes->r() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes->a() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes++;
      }
#endif
    }
  }
  return aPixmap;
}
