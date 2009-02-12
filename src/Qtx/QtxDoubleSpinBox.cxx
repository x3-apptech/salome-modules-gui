//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      QtxDoubleSpinBox.cxx
// Author:    Sergey TELKOV
//
#include "QtxDoubleSpinBox.h"

#include <QLineEdit>
#include <QDoubleValidator>

/*!
  \class QtxDoubleSpinBox
  \brief Enhanced version of the Qt's double spin box.

  The QtxDoubleSpinBox class represents the widget for entering the
  floating point values. In addition to the functionality provided by
  QDoubleSpinBox, this class supports "cleared" state - this is the
  state corresponding to "None" (or empty) entered value.

  To set "cleared" state use setCleared() method. To check if the spin
  box stores "cleared" state, use isCleared() method.
  For example:
  \code
  if (myDblSpinBox->isCleared()) {
    ... // process "None" state
  }
  else {
    double value = myDblSpinBox->value();
    ... // process entered value
  }
  \endcode
*/

/*!
  \brief Constructor.

  Constructs a spin box with 0.0 as minimum value and 99.99 as maximum value,
  a step value of 1.0 and a precision of 2 decimal places. 
  The value is initially set to 0.00.

  \param parent parent object
*/
QtxDoubleSpinBox::QtxDoubleSpinBox( QWidget* parent )
: QDoubleSpinBox( parent ),
  myCleared( false ),
  myPrecision(0)
{
  connect( lineEdit(), SIGNAL( textChanged( const QString& ) ), 
	   this, SLOT( onTextChanged( const QString& ) ) );
}

/*!
  \brief Constructor.

  Constructs a spin box with specified minimum, maximum and step value.
  The precision is set to 2 decimal places. 
  The value is initially set to the minimum value.

  \param min spin box minimum possible value
  \param max spin box maximum possible value
  \param step spin box increment/decrement value
  \param parent parent object
*/
QtxDoubleSpinBox::QtxDoubleSpinBox( double min, double max, double step, QWidget* parent )
: QDoubleSpinBox( parent ),
  myCleared( false ),
  myPrecision( 0 )
{
  setMinimum( min );
  setMaximum( max );
  setSingleStep( step );

  connect( lineEdit(), SIGNAL( textChanged( const QString& ) ), 
	   this, SLOT( onTextChanged( const QString& ) ) );
}

/*!
  \brief Constructor.

  Constructs a spin box with specified minimum, maximum and step value.
  The precision is set to 2 decimal places. 
  The value is initially set to the minimum value.

  \param min spin box minimum possible value
  \param max spin box maximum possible value
  \param step spin box increment/decrement value
  \param parent parent object
*/
QtxDoubleSpinBox::QtxDoubleSpinBox( double min, double max, double step, int prec, int dec, QWidget* parent )
: QDoubleSpinBox( parent ),
  myCleared( false ),
  myPrecision( prec )
{
  setDecimals( dec );
  setMinimum( min );
  setMaximum( max );
  setSingleStep( step );

  connect( lineEdit(), SIGNAL( textChanged( const QString& ) ), 
	   this, SLOT( onTextChanged( const QString& ) ) );
}

/*!
  \brief Destructor.
*/
QtxDoubleSpinBox::~QtxDoubleSpinBox()
{
}

/*!
  \brief Check if spin box is in the "cleared" state.
  \return \c true if spin box is cleared
  \sa setCleared()
*/
bool QtxDoubleSpinBox::isCleared() const
{
  return myCleared;
}

/*!
  \brief Change "cleared" status of the spin box.
  \param on new "cleared" status
  \sa isCleared()
*/
void QtxDoubleSpinBox::setCleared( const bool on )
{
  if ( myCleared == on )
    return;
  
  myCleared = on;
  setSpecialValueText( specialValueText() );
}

/*!
  \brief Set precision of the spin box
  
  If precision value is less than 0, the 'g' format is used for value output,
  otherwise 'f' format is used.

  \param prec new precision value.
  \sa precision()
*/
void QtxDoubleSpinBox::setPrecision( const int prec )
{
  int newPrec = qMax( prec, 0 );
  int oldPrec = qMax( myPrecision, 0 );
  myPrecision = prec;
  if ( newPrec != oldPrec )
    update();
}

/*!
  \brief Get precision value of the spin box
  \return current prevision value
  \sa setPrecision()
*/
int QtxDoubleSpinBox::getPrecision() const
{
  return myPrecision;
}

/*!
  \brief Interpret text entered by the user as a value.
  \param text text entered by the user
  \return mapped value
  \sa textFromValue()
*/
double QtxDoubleSpinBox::valueFromText( const QString& text ) const
{
  if (myPrecision < 0)
    return text.toDouble();

  return QDoubleSpinBox::valueFromText(text);
}

/*!
  \brief This function is used by the spin box whenever it needs to display
  the given value.

  \param val spin box value
  \return text representation of the value
  \sa valueFromText()
*/
QString QtxDoubleSpinBox::textFromValue( double val ) const
{
  QString s = QLocale().toString( val, myPrecision >= 0 ? 'f' : 'g', myPrecision == 0 ? 6 : qAbs( myPrecision ) );
  return removeTrailingZeroes( s );
}

/*!
  \brief Return source string with removed leading and trailing zeros.
  \param str source string
  \return resulting string
*/
QString QtxDoubleSpinBox::removeTrailingZeroes( const QString& src ) const
{
  QString delim( QLocale().decimalPoint() );

  int idx = src.lastIndexOf( delim );
  if ( idx == -1 )
    return src;

  QString iPart = src.left( idx );
  QString fPart = src.mid( idx + 1 );

  while ( !fPart.isEmpty() && fPart.at( fPart.length() - 1 ) == '0' )
    fPart.remove( fPart.length() - 1, 1 );

  QString res = iPart;
  if ( !fPart.isEmpty() )
    res += delim + fPart;

  return res;
}

/*!
  \brief Perform \a steps increment/decrement steps.
  
  The \a steps value can be any integer number. If it is > 0,
  the value incrementing is done, otherwise value is decremented
  \a steps times.  

  \param steps number of increment/decrement steps
*/
void QtxDoubleSpinBox::stepBy( int steps )
{
  myCleared = false;

  QDoubleSpinBox::stepBy( steps );
}

/*!
  \brief This function is used to determine whether input is valid.
  \param str currently entered value
  \param pos cursor position in the string
  \return validating operation result
*/
QValidator::State QtxDoubleSpinBox::validate( QString& str, int& pos ) const
{
  if (myPrecision >= 0)
    return QDoubleSpinBox::validate(str, pos);

  QString pref = this->prefix();
  QString suff = this->suffix();
  uint overhead = pref.length() + suff.length();
  QValidator::State state = QValidator::Invalid;

  QDoubleValidator v (NULL);
  v.setDecimals( decimals() );
  v.setBottom( minimum() );
  v.setTop( maximum() );
  v.setNotation( QDoubleValidator::ScientificNotation );

  if ( overhead == 0 )
    state = v.validate( str, pos );
  else
    {
      if ( str.length() >= overhead && str.startsWith( pref ) &&
	   str.right( suff.length() ) == suff )
	{
	  QString core = str.mid( pref.length(), str.length() - overhead );
	  int corePos = pos - pref.length();
	  state = v.validate( core, corePos );
	  pos = corePos + pref.length();
	  str.replace( pref.length(), str.length() - overhead, core );
	}
      else
	{
	  state = v.validate( str, pos );
	  if ( state == QValidator::Invalid )
	    {
	      QString special = this->specialValueText().trimmed();
	      QString candidate = str.trimmed();
	      if ( special.startsWith( candidate ) )
		{
		  if ( candidate.length() == special.length() )
		    state = QValidator::Acceptable;
		  else
		    state = QValidator::Intermediate;
		}
	    }
	}
    }
  return state;
}

/*!
  \brief Called when user enters the text in the spin box.
  \param txt current spin box text (not used)
*/
void QtxDoubleSpinBox::onTextChanged( const QString& /*txt*/ )
{
  myCleared = false;
}
