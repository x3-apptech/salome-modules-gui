// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
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
// See http://www.salome-platform.org/
//
// File:      QtxDblSpinBox.cxx
// Author:    Sergey TELKOV

#include "QtxDblSpinBox.h"

#include <qlineedit.h>
#include <qvalidator.h>
#include <qapplication.h>

/*
	Class: QtxDblSpinBox::Validator [internal]
	Descr: Validator for QtxDblSpinBox (getted from Trolltech Qt - SpinBoxValidator)
*/

class QtxDblSpinBox::Validator : public QDoubleValidator
{
public:
    Validator( QtxDblSpinBox* sb, const char* name )
	: QDoubleValidator( sb, name ), spinBox( sb ) {}

    virtual State validate( QString& str, int& pos ) const;

private:
    QtxDblSpinBox* spinBox;
};

QValidator::State QtxDblSpinBox::Validator::validate( QString& str, int& pos ) const
{
  QString pref = spinBox->prefix();
  QString suff = spinBox->suffix();
  uint overhead = pref.length() + suff.length();
  State state = Invalid;

  if ( overhead == 0 )
	  state = QDoubleValidator::validate( str, pos );
  else
	{
		if ( str.length() >= overhead && str.startsWith( pref ) &&
         str.right( suff.length() ) == suff )
		{
			QString core = str.mid( pref.length(), str.length() - overhead );
			int corePos = pos - pref.length();
			state = QDoubleValidator::validate( core, corePos );
			pos = corePos + pref.length();
			str.replace( pref.length(), str.length() - overhead, core );
		}
		else
		{
			state = QDoubleValidator::validate( str, pos );
			if ( state == Invalid )
			{
				QString special = spinBox->specialValueText().stripWhiteSpace();
				QString candidate = str.stripWhiteSpace();
				if ( special.startsWith( candidate ) )
				{
					if ( candidate.length() == special.length() )
						state = Acceptable;
					else
						state = Intermediate;
				}
			}
		}
  }
  return state;
}

/*
	Class: QtxDblSpinBox
	Descr: Spin box for real numbers.
*/

QtxDblSpinBox::QtxDblSpinBox( QWidget* parent, const char* name )
: QSpinBox( parent, name ),
myCleared( false ),
myBlocked( false ),
myPrecision( 0 )
{
  myMin = QRangeControl::minValue();
  myMax = QRangeControl::maxValue();
  myStep = QRangeControl::lineStep();
	myValue = myMin;
  setValidator( new Validator( this, "double_spinbox_validator" ) );
  rangeChange();
  updateDisplay();

  connect( editor(), SIGNAL( textChanged( const QString& ) ), this, SLOT( onTextChanged( const QString& ) ) );
}

QtxDblSpinBox::QtxDblSpinBox( double min, double max, double step, QWidget* parent, const char* name )
: QSpinBox( parent, name ),
myMin( min ),
myMax( max ),
myStep( step ),
myCleared( false ),
myBlocked( false ),
myPrecision( 0 )
{
	myValue = myMin;
  setValidator( new Validator( this, "double_spinbox_validator" ) );
  rangeChange();
  updateDisplay();

  connect( editor(), SIGNAL( textChanged( const QString& ) ), this, SLOT( onTextChanged( const QString& ) ) );
}

QtxDblSpinBox::~QtxDblSpinBox()
{
}

double QtxDblSpinBox::minValue() const
{
  return myMin;
}

double QtxDblSpinBox::maxValue() const
{
  return myMax;
}

void QtxDblSpinBox::setMinValue( int min )
{
	setMinValue( (double)min );
}

void QtxDblSpinBox::setMinValue( double min )
{
  if ( myMin != min )
  {
    myMin = min;
    rangeChange();
  }
}

void QtxDblSpinBox::setMaxValue( int max )
{
	setMaxValue( (double)max );
}

void QtxDblSpinBox::setMaxValue( double max )
{
  if ( myMax != max )
  {
    myMax = max;
    rangeChange();
  }
}

void QtxDblSpinBox::setRange( int min, int max )
{
	setRange( (double)min, (double)max );
}

void QtxDblSpinBox::setRange( double min, double max )
{
  if ( myMin != min || myMax != max )
  {
    myMin = min;
    myMax = max;
    rangeChange();
  }
}

double QtxDblSpinBox::lineStep() const
{
  return myStep;
}

void QtxDblSpinBox::setLineStep( int step )
{
  setLineStep( (double)step );
}

void QtxDblSpinBox::setLineStep( double step )
{
  myStep = step;
}

double QtxDblSpinBox::value() const
{
  QSpinBox::value();

  return myValue;
}

void QtxDblSpinBox::setValue( int val )
{
	setValue( (double)val );
}

void QtxDblSpinBox::setValue( double val )
{
	myCleared = false;
  double prevVal = myValue;
  myValue = bound( val );
  if ( prevVal != myValue )
    valueChange();
}

void QtxDblSpinBox::stepUp()
{
	interpretText();
	if ( wrapping() && myValue + myStep > myMax )
		setValue( myMin );
	else
		setValue( myValue + myStep );
}

void QtxDblSpinBox::stepDown()
{
	interpretText();
	if ( wrapping() && myValue - myStep < myMin )
		setValue( myMax );
	else
		setValue( myValue - myStep );
}

int QtxDblSpinBox::precision() const
{
	return myPrecision;
}

void QtxDblSpinBox::setPrecision( const int prec )
{
	int newPrec = QMAX( prec, 0 );
	int oldPrec = QMAX( myPrecision, 0 );
	myPrecision = prec;
	if ( newPrec != oldPrec )
		updateDisplay();
}

bool QtxDblSpinBox::isCleared() const
{
	return myCleared;
}

void QtxDblSpinBox::setCleared( const bool on )
{
	if ( myCleared == on )
		return;

	myCleared = on;
	updateDisplay();
}

void QtxDblSpinBox::selectAll()
{
#if QT_VER >= 3
	QSpinBox::selectAll();
#else
  editor()->selectAll();
#endif
}

bool QtxDblSpinBox::eventFilter( QObject* o, QEvent* e )
{
  if ( !myCleared || o != editor() || !editor()->text().stripWhiteSpace().isEmpty() )
  {
    bool state = QSpinBox::eventFilter( o, e );
    if ( e->type() == QEvent::FocusOut && o == editor() )
      updateDisplay();
    return state;
  }

  if ( e->type() == QEvent::FocusOut || e->type() == QEvent::Leave || e->type() == QEvent::Hide )
    return false;

  if ( e->type() == QEvent::KeyPress &&
	  ( ((QKeyEvent*)e)->key() == Key_Tab || ((QKeyEvent*)e)->key() == Key_BackTab ) )
  {
    QApplication::sendEvent( this, e );
    return true;
  }

  return QSpinBox::eventFilter( o, e );
}

void QtxDblSpinBox::updateDisplay()
{
  if ( myBlocked )
    return;

  bool upd = editor()->isUpdatesEnabled();
  editor()->setUpdatesEnabled( false );

  bool isBlock = myBlocked;
  myBlocked = true;
    
  QString txt = currentValueText();
    
  if ( myValue >= myMax )
    QSpinBox::setValue( QSpinBox::maxValue() );
  else if ( myValue <= myMin )
    QSpinBox::setValue( QSpinBox::minValue() );
  else
    QSpinBox::setValue( ( QSpinBox::minValue() + QSpinBox::maxValue() ) / 2 );
  
  QSpinBox::updateDisplay();

  editor()->setUpdatesEnabled( upd );

  editor()->setText( myCleared ? QString::null : txt );
  if ( !myCleared && editor()->hasFocus() )
  {
    if ( editor()->text() == specialValueText() )
      editor()->selectAll();
    else
      editor()->setSelection( prefix().length(), editor()->text().length() - prefix().length() - suffix().length() );
  }
  else
    editor()->setCursorPosition( 0 );

  myBlocked = isBlock;
}

void QtxDblSpinBox::interpretText()
{
  myCleared = false;

  bool ok = true;
  bool done = false;
  double newVal = 0;
  if ( !specialValueText().isEmpty() )
  {
	  QString s = QString( text() ).stripWhiteSpace();
	  QString t = QString( specialValueText() ).stripWhiteSpace();
	  if ( s == t )
    {
      newVal = minValue();
	    done = true;
    }
  }
  if ( !done )
	  newVal = mapTextToDoubleValue( &ok );
  if ( ok )
	  setValue( newVal );
  updateDisplay();
}

void QtxDblSpinBox::valueChange()
{
  updateDisplay();
  emit valueChanged( myValue );
  emit valueChanged( currentValueText() );
}

void QtxDblSpinBox::rangeChange()
{
  double min = QMIN( myMin, myMax );
  double max = QMAX( myMin, myMax );
  myMin = min;
  myMax = max;
  QDoubleValidator* v = ::qt_cast<QDoubleValidator*>( validator() );
  if ( v )
    v->setRange( myMin, myMax );

	if ( myMin == myMax )
		QSpinBox::setRange( 0, 0 );
	else
		QSpinBox::setRange( 0, 2 );

  setValue( myValue );
  updateDisplay();
}

QString QtxDblSpinBox::currentValueText()
{
  QString s;
  if ( (myValue == minValue()) && !specialValueText().isEmpty() )
	  s = specialValueText();
  else
	{
	  s = prefix();
		s.append( mapValueToText( myValue ) );
		s.append( suffix() );
	}
  return s;
}

QString QtxDblSpinBox::mapValueToText( double v )
{
	QString s;
  s.setNum( v, myPrecision >= 0 ? 'f' : 'g', myPrecision == 0 ? 6 : QABS( myPrecision ) );
  return removeTrailingZeroes( s );
}

QString QtxDblSpinBox::mapValueToText( int )
{
  QString s;
  s.setNum( myValue, myPrecision >= 0 ? 'f' : 'g', myPrecision == 0 ? 6 : QABS( myPrecision ) );
  return removeTrailingZeroes( s );
}

double QtxDblSpinBox::mapTextToDoubleValue( bool* ok )
{
  QString s = text();
  double newVal = s.toDouble( ok );
  if ( !(*ok) && !( !prefix() && !suffix() ) )
  {
	  s = cleanText();
	  newVal = s.toDouble( ok );
  }
  return newVal;
}

double QtxDblSpinBox::bound( double val )
{
  double newVal = val;
  if ( newVal > myMax )
    newVal = myMax;
  if ( newVal < myMin )
    newVal = myMin;
  return newVal;
}

void QtxDblSpinBox::leaveEvent( QEvent* e )
{
	if ( !myCleared )
		QSpinBox::leaveEvent( e );
}

void QtxDblSpinBox::wheelEvent( QWheelEvent* e )
{
  if ( !isEnabled() )
    return;

  QSpinBox::wheelEvent( e );
  updateDisplay();
}

void QtxDblSpinBox::onTextChanged( const QString& str )
{
  if ( !myBlocked )
    myCleared = false;
}

QString QtxDblSpinBox::removeTrailingZeroes( const QString& src ) const
{
  QString delim( "." );

  int idx = src.findRev( delim );
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
