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

#include "ImageComposer_Operator.h"
#include "ImageComposer_MetaTypes.h"
#include <QPixmap>
#include <QPainter>

/**
  Constructor
*/
ImageComposer_Operator::ImageComposer_Operator()
: myBackground( TRANSPARENT )
{
}

/**
  Destructor
*/
ImageComposer_Operator::~ImageComposer_Operator()
{
}

/**
  Get operator arguments
  @param theBackground the background color for result image
*/
void ImageComposer_Operator::getArgs( QColor& theBackground ) const
{
  theBackground = myBackground;
}

/**
  Set operator arguments
  @param theBackground the background color for result image
*/
void ImageComposer_Operator::setArgs( const QColor& theBackground )
{
  myBackground = theBackground;
}

/**
  Perform the composing of images
  @param theObj1 the first object to compose
  @param theObj2 the second object to compose
  @return the result image
*/
ImageComposer_Image ImageComposer_Operator::process( const QVariant& theObj1,
                                                     const QVariant& theObj2 ) const
{
  ImageComposer_Image aResult;
  if ( theObj1.isNull() || !theObj1.canConvert<ImageComposer_Image>() )
    return aResult;

  ImageComposer_Image anImage1 = theObj1.value<ImageComposer_Image>();

  ImageComposer_Image anImage2;
  if ( !theObj1.isNull() && theObj2.canConvert<ImageComposer_Image>() )
    anImage2 = theObj2.value<ImageComposer_Image>();

  QTransform aInvTransform = anImage1.transform().inverted();
  anImage1.setTransform( anImage1.transform() * aInvTransform );
  if( !anImage2.isNull() )
    anImage2.setTransform( anImage2.transform() * aInvTransform );
  
  QVariant anImage1Var, anImage2Var;
  anImage1Var.setValue<ImageComposer_Image>( anImage1 );
  anImage2Var.setValue<ImageComposer_Image>( anImage2 );

  QRectF aBounds = calcResultBoundingRect( anImage1Var, !anImage2.isNull() ? anImage2Var : theObj2 );

  QTransform aTranslate;
  aTranslate.translate( -aBounds.left(), -aBounds.top() );
  anImage1.setTransform( anImage1.transform() * aTranslate );
  anImage2.setTransform( anImage2.transform() * aTranslate );

  QImage aResultImage( int(aBounds.width()), int(aBounds.height()), QImage::Format_ARGB32 );
  aResultImage.fill( myBackground );

  QPainter aPainter( &aResultImage );
  //aPainter.setRenderHint( QPainter::SmoothPixmapTransform, true );
  aPainter.setRenderHint( QPainter::Antialiasing, true );
  aPainter.setRenderHint( QPainter::HighQualityAntialiasing, true );

  anImage1Var.setValue<ImageComposer_Image>( anImage1 );
  anImage2Var.setValue<ImageComposer_Image>( anImage2 );

  drawResult( aPainter, anImage1Var, !anImage2.isNull() ? anImage2Var : theObj2 );

  anImage1 = theObj1.value<ImageComposer_Image>();

  QTransform aResultTransform = anImage1.transform();
  aResultTransform.translate( aBounds.left(), aBounds.top() );

  aResult = aResultImage;
  aResult.setTransform( aResultTransform );

  return aResult;
}

/**
  Get the operator's arguments in the form of a binary array
  @return the binary array with arguments
*/
QByteArray ImageComposer_Operator::getBinArgs() const
{
  QByteArray aData;
  QDataStream aStream( &aData, QIODevice::WriteOnly );
  storeArgs( aStream );
  return aData;
}

/**
  Set the operator's arguments in the form of a binary array
  @param theData the binary array with arguments
*/
void ImageComposer_Operator::setBinArgs( const QByteArray& theData )
{
  QDataStream aStream( theData );
  restoreArgs( aStream );
}

QStringList ImageComposer_Operator::dumpArgsToPython( QString& theArrayName ) const
{
  QStringList aResList;

  if ( theArrayName.isEmpty() )
    theArrayName = "composer_args";

  QString aStreamName = theArrayName + "_stream";

  aResList << QString( "%1 = QByteArray();" ).arg( theArrayName );
  aResList << QString( "%1 = QDataStream( %2, QIODevice.WriteOnly );" )
              .arg( aStreamName ).arg( theArrayName );

  //Dump background color
  aResList << QString( "" );

  aResList << QString( "background_color = QColor( %1, %2, %3, %4 );" )
              .arg( myBackground.red() ).arg( myBackground.green() )
              .arg( myBackground.blue() ).arg( myBackground.alpha() );

  aResList << QString( "%1 << background_color;" ).arg( aStreamName );

  return aResList;
}

/**
  Store the operator's arguments to the stream
  @param theStream the stream for storing
*/
void ImageComposer_Operator::storeArgs( QDataStream& theStream ) const
{
  theStream << myBackground;
}

/**
  Restore the operator's arguments from the stream
  @param theStream the stream for restoring
*/
void ImageComposer_Operator::restoreArgs( QDataStream& theStream )
{
  theStream >> myBackground;
}
