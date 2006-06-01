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
#include "QDS_SpinBoxDbl.h"

#include <DDS_Dictionary.h>

#include <qvalidator.h>

#include <QtxDblSpinBox.h>

/*!
  Constructor.
*/
QDS_SpinBoxDbl::QDS_SpinBoxDbl( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  Destructor.
*/
QDS_SpinBoxDbl::~QDS_SpinBoxDbl()
{
}

/*!
  Returns string from QSpinBox widget.
*/
QString QDS_SpinBoxDbl::getString() const
{
  QString res;
  QtxDblSpinBox* sb = spinBox();
  if ( sb && !sb->isCleared() )
  {
    bool hasFocus = sb->hasFocus();
    if ( hasFocus )
      sb->clearFocus();
    
    res = sb->text();
    if ( !sb->suffix().isEmpty() )
      res.remove( res.find( sb->suffix() ), sb->suffix().length() );
    if ( !sb->prefix().isEmpty() )
      res.remove( res.find( sb->prefix() ), sb->prefix().length() );
    
    if ( hasFocus )
      sb->setFocus();
  }

  return res;
}

/*!
  Sets the string into QSpinBox widget.
*/
void QDS_SpinBoxDbl::setString( const QString& txt )
{
  if ( !spinBox() )
    return;

  spinBox()->setCleared( txt.isEmpty() );
  if ( !txt.isEmpty() )
    spinBox()->setValue( txt.toDouble() );
}

/*!
  Returns pointer to QtxDblSpinBox widget.
*/
QtxDblSpinBox* QDS_SpinBoxDbl::spinBox() const
{
  return ::qt_cast<QtxDblSpinBox*>( controlWidget() );
}

/*!
  Create QSpinBox widget as control subwidget.
*/
QWidget* QDS_SpinBoxDbl::createControl( QWidget* parent )
{
  QtxDblSpinBox* aSpinBox = new QtxDblSpinBox( parent );
  aSpinBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( aSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( onValueChanged( double ) ) );
  return aSpinBox;
}

/*!
  Notify about text changing in spin box.
*/
void QDS_SpinBoxDbl::onValueChanged( double )
{
  onParamChanged();
  QString str = getString();

  emit paramChanged();
  emit paramChanged( str );
}

/*!
  Returns the increment step.
*/
double QDS_SpinBoxDbl::step() const
{
  double s = 0;
  if ( spinBox() )
    s = spinBox()->lineStep();
  return s;
}

/*!
  Sets the increment step.
*/
void QDS_SpinBoxDbl::setStep( const double step )
{
  if ( spinBox() )
    spinBox()->setLineStep( step );
}

void QDS_SpinBoxDbl::unitSystemChanged( const QString& system )
{
  QDS_Datum::unitSystemChanged( system );

  QtxDblSpinBox* sb = spinBox();
  if ( !sb )
    return;

  delete sb->validator();
  QValidator* valid = validator();
  sb->setValidator( valid );

  sb->setSuffix( suffix() );
  sb->setPrefix( prefix() );

  Standard_Integer aPreci = 1;
  Handle(DDS_DicItem) aDicItem = dicItem();
  if ( !aDicItem.IsNull() )
    aPreci = aDicItem->GetPrecision();

  sb->setPrecision( aPreci );

  sb->setLineStep( .1 );
  sb->setMinValue( minValue().isEmpty() ? -DBL_MAX : minValue().toDouble() );
  sb->setMaxValue( maxValue().isEmpty() ? DBL_MAX : maxValue().toDouble() );
}
