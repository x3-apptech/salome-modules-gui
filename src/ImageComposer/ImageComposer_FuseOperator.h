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

#ifndef IMAGE_COMPOSER_FUSE_OPERATOR_HEADER
#define IMAGE_COMPOSER_FUSE_OPERATOR_HEADER

#include "ImageComposer_Operator.h"
#include <QColor>

/**
  \class ImageComposer_FuseOperator
  Implementation of the fusing operator
*/
class IMAGE_COMPOSER_API ImageComposer_FuseOperator : public ImageComposer_Operator
{
public:
  ImageComposer_FuseOperator();
  virtual ~ImageComposer_FuseOperator();

  static QString Type() { return "fuse"; }

  virtual QString name() const;

protected:
  virtual QRectF calcResultBoundingRect( const QVariant& theObj1, 
                                         const QVariant& theObj2 ) const;
  virtual void drawResult( QPainter& thePainter, const QVariant& theObj1,
                                                 const QVariant& theObj2 ) const;
};

#endif
