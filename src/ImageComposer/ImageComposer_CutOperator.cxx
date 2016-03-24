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

#include "ImageComposer_CutOperator.h"
#include "ImageComposer_MetaTypes.h"
#include <QString>
#include <QPixmap>
#include <QPainter>

/**
  Constructor
*/
ImageComposer_CutOperator::ImageComposer_CutOperator()
  : ImageComposer_Operator()
{
}

/**
*/
ImageComposer_CutOperator::~ImageComposer_CutOperator()
{
}

/**
*/
QString ImageComposer_CutOperator::name() const
{
  return Type();
}

/**
*/
QRectF ImageComposer_CutOperator::calcResultBoundingRect( const QVariant& theObj1, 
                                                          const QVariant& ) const
{
  QRectF aResRect;
  if ( !theObj1.isNull() && theObj1.canConvert<ImageComposer_Image>() )
  {
    ImageComposer_Image anImage1 = theObj1.value<ImageComposer_Image>();
    aResRect = anImage1.boundingRect();
  }
  return aResRect;
}

/**
*/
void ImageComposer_CutOperator::drawResult( QPainter&       thePainter,
                                            const QVariant& theObj1,
                                            const QVariant& theObj2 ) const
{
  if ( !theObj1.isNull() && theObj1.canConvert<ImageComposer_Image>() )
  {
    ImageComposer_Image anImage1 = theObj1.value<ImageComposer_Image>();
    anImage1.draw( thePainter );
  }

  if ( !theObj2.isNull() && theObj2.canConvert<ImageComposer_Image>() )
  {
    ImageComposer_Image anImage2 = theObj2.value<ImageComposer_Image>();
    anImage2.draw( thePainter );
  }
}
