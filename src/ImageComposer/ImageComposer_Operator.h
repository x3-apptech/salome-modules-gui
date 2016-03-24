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

#ifndef IMAGE_COMPOSER_OPERATOR_HEADER
#define IMAGE_COMPOSER_OPERATOR_HEADER

#include "ImageComposer.h"

#include <QColor>

class QString;
class QRectF;
class QPainter;
class QTransform;
class QVariant;
class ImageComposer_Image;

const QColor TRANSPARENT( 255, 255, 255, 0 );

/**
  \class ImageComposer_Operator
  Implementation of the base abstract operation for image composing
*/
class IMAGE_COMPOSER_API ImageComposer_Operator
{
public:
  ImageComposer_Operator();
  virtual ~ImageComposer_Operator();

  void getArgs( QColor& theBackground ) const;
  void setArgs( const QColor& theBackground );

  QByteArray getBinArgs() const;
  void setBinArgs( const QByteArray& );

  /**
    Dumps arguments of operator to python script representation
    @param theArrayName name of result \c QByteArray array variable
    @return list of strings with python representation
  **/
  QStringList dumpArgsToPython( QString& theArrayName ) const;

  /**
    Return name of the operator.
    @return name of the operator
  */
  virtual QString name() const = 0;

  virtual ImageComposer_Image process( const QVariant& theObj1,
                                       const QVariant& theObj2 ) const;

protected:
  /**
    Calculate bounding rectangle for the result image
    @param theObj1 first object to calculate
    @param theObj2 second object to calculate
    @return calculated bounding rectangle
  */
  virtual QRectF calcResultBoundingRect( const QVariant& theObj1, 
                                         const QVariant& theObj2 ) const = 0;

  /**
    Draw result image using the given painter
    @param thePainter the painter on the result image
    @param theObj1 the first object to compose
    @param theObj2 the second object to compose
  */
  virtual void drawResult( QPainter& thePainter, const QVariant& theObj1,
                                                 const QVariant& theObj2 ) const = 0;

  virtual void storeArgs( QDataStream& theStream ) const;
  virtual void restoreArgs( QDataStream& theStream );

private:
  friend class ImageComposerTests_TestOperators;

  QColor myBackground;  ///< the background color for result image
};

#endif
