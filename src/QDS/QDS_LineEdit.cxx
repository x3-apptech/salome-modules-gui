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
#include "QDS_LineEdit.h"

#include <qlineedit.h>
#include <qvalidator.h>

/*
  Class: QDS_LineEdit::Editor
  Descr: Internal class inherited from line edit
*/

class QDS_LineEdit::Editor : public QLineEdit
{
public:
  Editor( QWidget* parent = 0 ) : QLineEdit( parent ), myNumber( 2 ) {};
  virtual ~Editor() {};

  void setNumber( const int num ) { myNumber = num; };

  virtual QSize minimumSizeHint() const
  {
    return QLineEdit::minimumSizeHint().
      expandedTo( QSize( fontMetrics().width( "0" ) * myNumber, 0 ) );
  }
  
  virtual QSize sizeHint() const
  {
    return minimumSizeHint();
  }

private:
  int           myNumber;
};

/*
  Class: QDS_LineEdit
  Descr: Data control corresponding to line edit
*/

/*!
  Constructor.
*/
QDS_LineEdit::QDS_LineEdit( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  Destructor.
*/
QDS_LineEdit::~QDS_LineEdit()
{
}

void QDS_LineEdit::unitSystemChanged( const QString& system )
{
  QDS_Datum::unitSystemChanged( system );

  QLineEdit* le = lineEdit();
  if ( !le )
    return;
  
  delete le->validator();
  le->clearValidator();
  QValidator* valid = validator();
  if ( valid )
    le->setValidator( valid );

  QString aFormat = format();
  int num = 0;
  int pos = aFormat.find( '%' );
  if ( pos != -1 )
  {
    pos++;
    QString aLen;
    while ( pos < (int)aFormat.length() && aFormat.at( pos ).isDigit() )
      aLen += aFormat.at( pos++ );
    if ( pos < (int)aFormat.length() && aFormat.at( pos ) == '.' )
      num += 1;
    if ( !aLen.isEmpty() )
      num += aLen.toInt();
  }
  
  int zeroLen = format( format(), type(), 0 ).length();
  int minLen  = format( format(), type(), minValue() ).length();
  int maxLen  = format( format(), type(), maxValue() ).length();

  num = QMAX( QMAX( num, zeroLen ), QMAX( minLen, maxLen ) );
  ((Editor*)le)->setNumber( num );
}

/*!
  Set the aligment of line edit.
*/
void QDS_LineEdit::setAlignment( const int align, const int type )
{
  if ( ( type & Control ) && lineEdit() )
    lineEdit()->setAlignment( align );

  QDS_Datum::setAlignment( align, type );
}

/*!
  Returns string from QLineEdit widget.
*/
QString QDS_LineEdit::getString() const
{
  QString res;
  if ( lineEdit() )
    res = lineEdit()->text();
  return res;
}

/*!
  Sets the string into QLineEdit widget.
*/
void QDS_LineEdit::setString( const QString& txt )
{
  if ( lineEdit() )
    lineEdit()->setText( txt );
}

/*!
  Returns pointer to QLineEdit widget.
*/
QLineEdit* QDS_LineEdit::lineEdit() const
{
  return ::qt_cast<QLineEdit*>( controlWidget() );
}

/*!
  Create QLineEdit widget as control subwidget.
*/
QWidget* QDS_LineEdit::createControl( QWidget* parent )
{
  Editor* le = new Editor( parent );
  connect( le, SIGNAL( returnPressed() ), this, SIGNAL( returnPressed() ) );
  connect( le, SIGNAL( textChanged( const QString& ) ), this, SLOT( onTextChanged( const QString& ) ) );
  return le;
}

/*!
  Notify about text changing in line edit.
*/
void QDS_LineEdit::onTextChanged( const QString& )
{
  invalidateCache();

  onParamChanged();
  QString str = getString();
  emit paramChanged();
  emit paramChanged( str );
}

/*!
  Checks the current parameter value on validity.
*/
void QDS_LineEdit::onParamChanged()
{
  QLineEdit* anEdit = lineEdit();
  if ( !anEdit )
    return;

  bool aValid = isValid( false );

  QPalette aPal = anEdit->palette();
  if ( !aValid )
    aPal.setColor( QPalette::Active, QColorGroup::Text, QColor( 255, 0, 0 ) );
  else
    aPal.setColor( QPalette::Active, QColorGroup::Text, QColor( 0, 0, 0 ) );

  anEdit->setPalette( aPal );
}
