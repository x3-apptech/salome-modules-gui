// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "ImageComposer_CropOperator.h"
#include "ImageComposer_Image.h"
#include "ImageComposer_MetaTypes.h"

#include <QPixmap>
#include <QPainter>

/**
  Constructor
*/
ImageComposer_CropOperator::ImageComposer_CropOperator()
: ImageComposer_Operator()
{
}

/**
*/
ImageComposer_CropOperator::~ImageComposer_CropOperator()
{
}

/**
*/
QString ImageComposer_CropOperator::name() const
{
  return Type();
}

/**
*/
QRectF ImageComposer_CropOperator::calcResultBoundingRect( const QVariant&, 
                                                           const QVariant& theObj2 ) const
{
  QRectF aResRect;
  if ( !theObj2.isNull() && theObj2.canConvert<QPainterPath>() )
  {
    QPainterPath aCropPath = theObj2.value<QPainterPath>();
    aResRect = aCropPath.boundingRect();
  }
  return aResRect;
}

/**
*/
void ImageComposer_CropOperator::drawResult( QPainter&       thePainter,
                                             const QVariant& theObj1,
                                             const QVariant& theObj2 ) const
{
  if ( theObj1.isNull() || !theObj1.canConvert<ImageComposer_Image>() ||
       theObj2.isNull() || !theObj2.canConvert<QPainterPath>() )
    return;

  ImageComposer_Image anImage1 = theObj1.value<ImageComposer_Image>();
  QPainterPath anImgClipPath = theObj2.value<QPainterPath>();

  QRectF aBounds = anImgClipPath.boundingRect();

  QTransform aTranslate;
  aTranslate.translate( -aBounds.left(), -aBounds.top() );

  QPainterPath aClipPath = aTranslate.map( anImgClipPath );
  thePainter.setClipPath( aClipPath );

  anImage1.draw( thePainter );
  //thePainter.fillPath( aClipPath, Qt::red );
}

/**
*/
ImageComposer_Image ImageComposer_CropOperator::process( const QVariant& theObj1,
                                                         const QVariant& theObj2 ) const
{
  ImageComposer_Image aResult;
  if ( theObj1.isNull() || !theObj1.canConvert<ImageComposer_Image>() ||
       theObj2.isNull() || !theObj2.canConvert<QPainterPath>() )
    return aResult;

  ImageComposer_Image anImage1 = theObj1.value<ImageComposer_Image>();
  QPainterPath aCropPath = theObj2.value<QPainterPath>();

  QRect anImageRect( 0, 0, anImage1.width(), anImage1.height() );

  QPainterPath anImageBoundsPath;
  anImageBoundsPath.addPolygon( anImage1.transform().mapToPolygon( anImageRect ) );

  // clipping path mapped to first image's local CS
  QVariant anImgClipPath;
  anImgClipPath.setValue<QPainterPath>( 
    anImage1.transform().inverted().map( aCropPath.intersected( anImageBoundsPath ) ) );

  return ImageComposer_Operator::process( theObj1, anImgClipPath );
}
