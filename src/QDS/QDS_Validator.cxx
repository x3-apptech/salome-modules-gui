// Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
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
#include "QDS_Validator.h"

/*!
  \class QDS_IntegerValidator

  This class to provide validation of user inputted string for datum with 'Integer'
  value type according to datum properties: minimum value, maximum value, filter.

  Inherits from QIntValidator.
*/

/*!
  Constructor. Create empty validator object.
*/
QDS_IntegerValidator::QDS_IntegerValidator( QObject* p )
: QIntValidator( p )
{
}

/*!
  Constructor. Create validator object with filter string \af.
*/
QDS_IntegerValidator::QDS_IntegerValidator( const QString& f, QObject* p )
: QIntValidator( p ),
myFilter( f )
{
}

/*!
  Destructor.
*/
QDS_IntegerValidator::~QDS_IntegerValidator()
{
}

/*!
  Validate the inputed string \ainput. Reimplemented from QIntValidator. Perform
  standard check by QIntValidator::validate(). If returned state is not Invalid and
  filter specified then perform validation with filter by QRegExpValidator.

  Returns Invalid if input is invalid according to this validator's rules,
  Intermediate if it is likely that a little more editing will make the input acceptable
  and Acceptable if the input is valid.
*/
QValidator::State QDS_IntegerValidator::validate( QString& input, int& pos ) const
{
  State rgState = Acceptable;
  State ivState = QIntValidator::validate( input, pos );
  if ( ivState != Invalid && !myFilter.isEmpty() )
    rgState = QRegExpValidator( QRegExp( myFilter ), 0 ).validate( input, pos );

  ivState = QMIN( ivState, rgState );

  return ivState;
}

/*!
  \class QDS_DoubleValidator

  This class to provide validation of user inputted string for datum with 'Float'
  value type according to datum properties: minimum value, maximum value, precision, filter.

  Inherits from QDoubleValidator.
*/

/*!
  Constructor. Create empty validator object.
*/
QDS_DoubleValidator::QDS_DoubleValidator( QObject* p )
: QDoubleValidator( p )
{
}

/*!
  Constructor. Create validator object with filter string \af.
*/
QDS_DoubleValidator::QDS_DoubleValidator( const QString& f, QObject* p )
: QDoubleValidator( p ),
myFilter( f )
{
}

/*!
  Destructor.
*/
QDS_DoubleValidator::~QDS_DoubleValidator()
{
}

/*!
  Validate the inputed string \ainput. Reimplemented from QDoubleValidator. Perform
  standard check by QDoubleValidator::validate(). If returned state is not Invalid and
  filter specified then perform validation with filter by QRegExpValidator.

  Returns Invalid if input is invalid according to this validator's rules,
  Intermediate if it is likely that a little more editing will make the input acceptable
  and Acceptable if the input is valid.
*/
QValidator::State QDS_DoubleValidator::validate( QString& input, int& pos ) const
{
  State rgState = Acceptable;
  State dvState = QDoubleValidator::validate( input, pos );
  if ( dvState != Invalid && !myFilter.isEmpty() )
    rgState = QRegExpValidator( QRegExp( myFilter ), 0 ).validate( input, pos );

  dvState = QMIN( dvState, rgState );

  return dvState;
}

/*!
  \class QDS_StringValidator

  This class to provide validation of user inputted string for datum with 'String'
  value type according to datum properties: format, filter.

  Inherits from QDoubleValidator.
*/

/*!
  Constructor. Create empty validator object.
*/
QDS_StringValidator::QDS_StringValidator( QObject* p ) 
: QValidator( p ), 
myLen( -1 ) 
{
}

/*!
  Constructor. Create validator object with filter string \af.
*/
QDS_StringValidator::QDS_StringValidator( const QString& f, QObject* p ) 
: QValidator( p ), 
myFlags( f ), 
myLen( -1 ) 
{
}

/*!
  Constructor. Create validator object with filter string \aft and format flags \afg.
*/
QDS_StringValidator::QDS_StringValidator( const QString& ft, const QString& fg, QObject* p ) 
: QValidator( p ), 
myLen( -1 ), 
myFilter( ft ), 
myFlags( fg ) 
{
}

/*!
  Destructor.
*/
QDS_StringValidator::~QDS_StringValidator() 
{
}

/*!
  Returns valid string length. If length not specified -1 returned.
*/
int QDS_StringValidator::length() const 
{ 
  return myLen; 
}

/*!
  Sets the valid string length \al. If value less than zero valid string
  length not specified.
*/
void QDS_StringValidator::setLength( const int l )
{
  myLen = l;
}

/*!
  Validate the inputed string \ainput. Reimplemented from QValidator. Convert specified
  string to upper/lower case if the format flags contains specificator 'u'/'l'.
  If valid string lenght is specified then check given string length. If
  filter specified then perform validation with filter by QRegExpValidator.

  Returns Invalid if input is invalid according to this validator's rules,
  Intermediate if it is likely that a little more editing will make the input acceptable
  and Acceptable if the input is valid.
*/
QValidator::State QDS_StringValidator::validate( QString& input, int& pos ) const
{
  if ( input.isEmpty() )
    return Acceptable;

  QString orig = input;
  if ( myFlags.contains( 'u', false ) )
    input = input.upper();
  if ( myFlags.contains( 'l', false ) )
    input = input.lower();

  State rgState = Acceptable;
  State svState = orig == input ? Acceptable : Intermediate;

  if ( myLen >= 0 && (int)input.length() > myLen )
      svState = Intermediate;

  if ( !myFilter.isEmpty() )
    rgState = QRegExpValidator( QRegExp( myFilter ), 0 ).validate( input, pos );

  svState = QMIN( svState, rgState );

  return svState;
}
