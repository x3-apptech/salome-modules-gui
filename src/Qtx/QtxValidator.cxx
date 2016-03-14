// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxValidator.cxx
// Author:    Alexandre SOLOVYOV
//
#include "QtxValidator.h"

/*!
  \class QtxIntValidator
  \brief Validator for integer numbers with possibility to fix up the invalid value.
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
QtxIntValidator::QtxIntValidator( QObject* parent )
: QIntValidator( parent )
{
}

/*!
  \brief Constructor.
  \param bot minimum possible value
  \param top maximum possible value
  \param parent parent object
*/
QtxIntValidator::QtxIntValidator( const int bot, const int top, QObject* parent )
: QIntValidator( bot, top, parent )
{
}

/*!
  \brief Destructor.
*/
QtxIntValidator::~QtxIntValidator()
{
}

/*!
  \brief Validate the input and fixup it if necessary.

  If the string represents integer value less then minimum value, it becomes equal to the minimum.
  if the string represents integer value more then mazimum value, it becomes equal to the maximum.
  If the string is not evaluated as integer it becomes equal to \c 0.

  \param str the string to be validated
*/
void QtxIntValidator::fixup( QString& str ) const
{
  bool ok = false;
  int i = str.toInt( &ok );
  if ( ok )
  {
    if ( i < bottom() )
      str = QString::number( bottom() );
    else if( i > top() )
      str = QString::number( top() );
  }
  else
    str = QString ( "0" );
}

/*!
  \class QtxDoubleValidator
  \brief Validator for double numbers with possibility to fix up the invalid value.
*/

/*!
  \brief Constructor
  \param parent parent object
*/
QtxDoubleValidator::QtxDoubleValidator( QObject* parent )
: QDoubleValidator( parent )
{
}

/*!
  \brief Constructor.
  \param bot minimum possible value
  \param top maximum possible value
  \param dec precision (number of digits after dot)
  \param parent parent object
*/
QtxDoubleValidator::QtxDoubleValidator( const double bot, const double top,
                                        const int dec, QObject* parent )
: QDoubleValidator( bot, top, dec, parent )
{
}

/*!
  \brief Destructor.
*/
QtxDoubleValidator::~QtxDoubleValidator()
{
}

/*!
  \brief Validate the input and fixup it if necessary.

  If the string represents double value less then minimum value, it becomes equal to the minimum.
  if the string represents double value more then mazimum value, it becomes equal to the maximum.
  If the string is not evaluated as double it becomes equal to \c 0.

  \param str the string to be validated
*/
void QtxDoubleValidator::fixup( QString& str ) const
{
  bool ok = false;
  double d = str.toDouble( &ok );
  if ( ok )
  {
    if ( d < bottom() )
      str = QString::number( bottom() );
    else if ( d > top() )
      str = QString::number( top() );
  }
  else
    str = QString( "0" );
}
