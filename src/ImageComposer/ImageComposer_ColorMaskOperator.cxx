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

#include "ImageComposer_ColorMaskOperator.h"
#include "ImageComposer_MetaTypes.h"
#include <QRectF>
#include <QRgb>
#include <QPixmap>
#include <QPainter>

/**
  Constructor
*/
ImageComposer_ColorMaskOperator::ImageComposer_ColorMaskOperator()
: ImageComposer_Operator(),
  myRefColor( Qt::black ), myIsMakeTransparent( false ),
  myRGBThreshold( 0 ), myAlphaThreshold( 0 )
{
}

/**
*/
ImageComposer_ColorMaskOperator::~ImageComposer_ColorMaskOperator()
{
}

/**
  Set operator arguments
  @param theRefColor the color to the searched (the color for mask)
  @param isMakeTransparent the boolean flag controlling if the pixels with matching color
                           should be made transparent or one with non-matching color
  @param theRGBThreshold the threshold for RGB components
  @param theAlphaThreshold the threshold for Alpha component
*/
void ImageComposer_ColorMaskOperator::setArgs( const QColor& theRefColor,
                                               bool isMakeTransparent,
                                               int theRGBThreshold,
                                               int theAlphaThreshold )
{
  myRefColor = theRefColor;
  myIsMakeTransparent = isMakeTransparent;
  myRGBThreshold = theRGBThreshold;
  myAlphaThreshold = theAlphaThreshold;
}

/**
*/
QString ImageComposer_ColorMaskOperator::name() const
{
  return Type();
}

QStringList ImageComposer_ColorMaskOperator::dumpArgsToPython( QString& theArrayName ) const
{
  QStringList aResList = ImageComposer_Operator::dumpArgsToPython( theArrayName );

  QString aStreamName = theArrayName + "_stream";

  //Dump operator arguments
  aResList << QString( "" );
  aResList << QString( "mask_color = QColor( %1, %2, %3, %4 );" )
              .arg( myRefColor.red() ).arg( myRefColor.green() )
              .arg( myRefColor.blue() ).arg( myRefColor.alpha() );
  aResList << QString( "%1 << mask_color;" ).arg( aStreamName );

  aResList << QString( "" );
  aResList << QString( "make_transparent = %1;" ).arg( myIsMakeTransparent );
  aResList << QString( "%1 << make_transparent;" ).arg( aStreamName );

  aResList << QString( "" );
  aResList << QString( "rgb_threshold = %1;" ).arg( myRGBThreshold );
  aResList << QString( "%1 << rgb_threshold;" ).arg( aStreamName );

  aResList << QString( "" );
  aResList << QString( "alpha_threshold = %1;" ).arg( myAlphaThreshold );
  aResList << QString( "%1 << alpha_threshold;" ).arg( aStreamName );

  return aResList;
}

/**
*/
QRectF ImageComposer_ColorMaskOperator::calcResultBoundingRect( const QVariant& theObj1, 
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
void ImageComposer_ColorMaskOperator::drawResult( QPainter&       thePainter,
                                                  const QVariant& theObj1,
                                                  const QVariant& ) const
{
  if ( theObj1.isNull() || !theObj1.canConvert<ImageComposer_Image>() )
    return;

  ImageComposer_Image anImage1 = theObj1.value<ImageComposer_Image>();

  QImage anImage = anImage1.convertToFormat( QImage::Format_ARGB32 );

  int aRMin = myRefColor.red()    - myRGBThreshold;
  int aRMax = myRefColor.red()    + myRGBThreshold;
  int aGMin = myRefColor.green()  - myRGBThreshold;
  int aGMax = myRefColor.green()  + myRGBThreshold;
  int aBMin = myRefColor.blue()   - myRGBThreshold;
  int aBMax = myRefColor.blue()   + myRGBThreshold;
  int anAMin = myRefColor.alpha() - myAlphaThreshold;
  int anAMax = myRefColor.alpha() + myAlphaThreshold;

  QRgb aTransparent = TRANSPARENT.rgba();

  for( int y = 0, aMaxY = anImage.height(); y < aMaxY; y++ )
    for( int x = 0, aMaxX = anImage.width(); x < aMaxX; x++ )
    {
      QRgb* aLine = ( QRgb* )anImage.scanLine( y );
      int aRed    = qRed( aLine[x] );
      int aGreen  = qGreen( aLine[x] );
      int aBlue   = qBlue( aLine[x] );
      int anAlpha = qAlpha( aLine[x] );
      bool isInRange = ( anAMin <= anAlpha && anAlpha <= anAMax )
                    && (  aRMin <= aRed    && aRed    <=  aRMax )
                    && (  aGMin <= aGreen  && aGreen  <=  aGMax )
                    && (  aBMin <= aBlue   && aBlue   <=  aBMax );
      if( myIsMakeTransparent == isInRange )
        aLine[x] = aTransparent;
    }

  ImageComposer_Image aResult;
  aResult = anImage;
  aResult.setTransform( anImage1.transform() );
  aResult.draw( thePainter );
}

void ImageComposer_ColorMaskOperator::storeArgs( QDataStream& theStream ) const
{
  ImageComposer_Operator::storeArgs( theStream );
  theStream << myRefColor;
  theStream << myIsMakeTransparent;
  theStream << myRGBThreshold;
  theStream << myAlphaThreshold;
}

void ImageComposer_ColorMaskOperator::restoreArgs( QDataStream& theStream )
{
  ImageComposer_Operator::restoreArgs( theStream );
  theStream >> myRefColor;
  theStream >> myIsMakeTransparent;
  theStream >> myRGBThreshold;
  theStream >> myAlphaThreshold;
}
