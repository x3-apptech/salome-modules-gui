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

#ifndef IMAGE_COMPOSER_IMAGE_HEADER
#define IMAGE_COMPOSER_IMAGE_HEADER

#include "ImageComposer.h"

#include <QImage>

class QVariant;
class ImageComposer_Operator;

/**
  \class ImageComposer_Image
  Implementation of image in the global coordinate system
*/
class IMAGE_COMPOSER_API ImageComposer_Image : public QImage
{
public:
  ImageComposer_Image();
  ImageComposer_Image( const ImageComposer_Image& theImage );
  ImageComposer_Image( const QImage& theImage );
  ~ImageComposer_Image();

  void draw( QPainter& thePainter ) const;

  QTransform transform() const;
  void setTransform( const QTransform& );
  void setLocalTransform( qreal theDx, qreal theDy, qreal theRotationDeg );

  QRectF boundingRect() const;

  const ImageComposer_Image& operator = ( const ImageComposer_Image& theImage );
  const QImage& operator = ( const QImage& theImage );

  ImageComposer_Image apply( const ImageComposer_Operator& theOperator,
                             const QVariant&               theOtherObj ) const;

  static QColor defaultBackground();
  static void setDefaultBackground( const QColor& );
  ImageComposer_Image operator & ( const QRect& ) const;
  ImageComposer_Image operator & ( const QPainterPath& ) const;
  ImageComposer_Image operator & ( const ImageComposer_Image& ) const;
  ImageComposer_Image operator | ( const ImageComposer_Image& ) const;

private:
  QTransform myTransform;            ///< the image transformation in the global CS
  static QColor myDefaultBackground; ///< the default background color to be used in operators
};

#endif
