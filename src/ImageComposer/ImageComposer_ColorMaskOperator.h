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

#ifndef IMAGE_COMPOSER_COLOR_MASK_OPERATOR_HEADER
#define IMAGE_COMPOSER_COLOR_MASK_OPERATOR_HEADER

#include "ImageComposer_Operator.h"
#include <QColor>

/**
  \class ImageComposer_ColorMaskOperator
  Implementation of the color mask operator
*/
class IMAGE_COMPOSER_API ImageComposer_ColorMaskOperator : public ImageComposer_Operator
{
public:
  ImageComposer_ColorMaskOperator();
  virtual ~ImageComposer_ColorMaskOperator();

  void setArgs( const QColor& theRefColor,
                bool isMakeTransparent,
                int theRGBThreshold,
                int theAlphaThreshold );

  static QString Type() { return "colormask"; }

  virtual QString name() const;

  /**
    Dumps arguments of operator to python script representation
    @param theArrayName name of result \c QByteArray array variable
    @return list of strings with python representation
  **/
  QStringList dumpArgsToPython( QString& theArrayName ) const;

protected:
  virtual QRectF calcResultBoundingRect( const QVariant& theObj1, 
                                         const QVariant& theObj2 ) const;
  virtual void drawResult( QPainter& thePainter, const QVariant& theObj1,
                                                 const QVariant& theObj2 ) const;

  virtual void storeArgs( QDataStream& theStream ) const;
  virtual void restoreArgs( QDataStream& theStream );

private:
  friend class ImageComposerTests_TestOperators;

  QColor myRefColor;         ///< the color to the searched (the color for mask)
  bool myIsMakeTransparent;  ///< the boolean flag controlling if the pixels with matching color
                             ///< should be made transparent or one with non-matching color
  int myRGBThreshold;        ///< the threshold for RGB components
  int myAlphaThreshold;      ///< the threshold for Alpha component
};

#endif
