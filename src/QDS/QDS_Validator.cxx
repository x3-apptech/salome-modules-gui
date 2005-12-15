#include "QDS_Validator.h"

/*!
    Class: QDS_IntegerValidator
*/

QDS_IntegerValidator::QDS_IntegerValidator( QObject* p )
: QIntValidator( p )
{
}

QDS_IntegerValidator::QDS_IntegerValidator( const QString& f, QObject* p )
: QIntValidator( p ),
myFilter( f )
{
}

QDS_IntegerValidator::~QDS_IntegerValidator()
{
}

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
    Class: QDS_DoubleValidator
*/

QDS_DoubleValidator::QDS_DoubleValidator( QObject* p )
: QDoubleValidator( p )
{
}

QDS_DoubleValidator::QDS_DoubleValidator( const QString& f, QObject* p )
: QDoubleValidator( p ),
myFilter( f )
{
}

QDS_DoubleValidator::~QDS_DoubleValidator()
{
}

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
    Class: QDS_StringValidator
*/

QDS_StringValidator::QDS_StringValidator( QObject* p ) 
: QValidator( p ), 
myLen( -1 ) 
{
}

QDS_StringValidator::QDS_StringValidator( const QString& f, QObject* p ) 
: QValidator( p ), 
myFlags( f ), 
myLen( -1 ) 
{
}

QDS_StringValidator::QDS_StringValidator( const QString& ft, const QString& fg, QObject* p ) 
: QValidator( p ), 
myLen( -1 ), 
myFilter( ft ), 
myFlags( fg ) 
{
}

QDS_StringValidator::~QDS_StringValidator() 
{
}

int QDS_StringValidator::length() const 
{ 
  return myLen; 
}

void QDS_StringValidator::setLength( const int l ) 
{ 
  myLen = l; 
}

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
