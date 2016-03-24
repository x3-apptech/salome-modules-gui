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

#ifndef IMAGE_COMPOSER_CROP_OPERATOR_HEADER
#define IMAGE_COMPOSER_CROP_OPERATOR_HEADER

#include "ImageComposer_Operator.h"
#include <QColor>
#include <QPainterPath>

/**
  \class ImageComposer_CropOperator
  Implementation of the cropping operator
*/
class IMAGE_COMPOSER_API ImageComposer_CropOperator : public ImageComposer_Operator
{
public:
  ImageComposer_CropOperator();
  virtual ~ImageComposer_CropOperator();

  static QString Type() { return "crop"; }

  virtual QString name() const;

  virtual ImageComposer_Image process( const QVariant& theObj1,
                                       const QVariant& theObj2 ) const;

protected:
  virtual QRectF calcResultBoundingRect( const QVariant& theObj1, 
                                         const QVariant& theObj2 ) const;
  virtual void drawResult( QPainter& thePainter, const QVariant& theObj1,
                                                 const QVariant& theObj2 ) const;

private:
  friend class ImageComposerTests_TestOperators;
};

#endif
