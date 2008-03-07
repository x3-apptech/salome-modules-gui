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
#include "QDS_SpinBox.h"

#include <QtxIntSpinBox.h>

#include <qvalidator.h>

/*
  \class QDS_SpinBox
  
  Datum with control corresponding to spin box. This control used for integer numbers.
  User can input data directly in spin box or can modify current value with given
  increment.
*/

/*!
  Constructor. Create spin box datum object with datum identifier \aid under widget \aparent. Parameter \aflags
  define behaviour of datum and set of created subwidgets. Default value of this parameter is QDS::All.
  Parameter \acomp specify the component name which will be used during search of dictionary item.
*/
QDS_SpinBox::QDS_SpinBox( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  Destructor.
*/
QDS_SpinBox::~QDS_SpinBox()
{
}

/*!
  Returns string from QSpinBox widget. Reimplemented.
*/
QString QDS_SpinBox::getString() const
{
  QString res;
  QtxIntSpinBox* aSpinBox = spinBox();
  if ( aSpinBox && !aSpinBox->isCleared() )
  {
    res = aSpinBox->text();
    if ( !aSpinBox->suffix().isEmpty() )
      res.remove( res.find( aSpinBox->suffix() ), aSpinBox->suffix().length() );
    if ( !aSpinBox->prefix().isEmpty() )
      res.remove( res.find( aSpinBox->prefix() ), aSpinBox->prefix().length() );
  }
  return res;
}

/*!
  Sets the string into QSpinBox widget. Reimplemented.
*/
void QDS_SpinBox::setString( const QString& txt )
{
  if ( !spinBox() )
    return;

  spinBox()->setCleared( txt.isEmpty() );
  if ( !txt.isEmpty() )
    spinBox()->setValue( txt.toInt() );
}

/*!
  Returns pointer to QSpinBox widget.
*/
QtxIntSpinBox* QDS_SpinBox::spinBox() const
{
  return ::qt_cast<QtxIntSpinBox*>( controlWidget() );
}

/*!
  Create QSpinBox widget as control subwidget. Reimplemented.
*/
QWidget* QDS_SpinBox::createControl( QWidget* parent )
{
  QtxIntSpinBox* aSpinBox = new QtxIntSpinBox( parent );
  aSpinBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( aSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( onValueChanged( int ) ) );
  return aSpinBox;
}

/*!
  Notify about value changing in spin box.
*/
void QDS_SpinBox::onValueChanged( int val )
{
  onParamChanged();
  QString str = QString::number( val );
  emit paramChanged();
  emit paramChanged( str );
}

/*!
  Sets the increment step.
*/
void QDS_SpinBox::setStep( const int step )
{
  if ( spinBox() )
    spinBox()->setLineStep( step );
}

/*!
  Returns the increment step.
*/
int QDS_SpinBox::step() const
{
  int s = 0;
  if ( spinBox() )
    s = spinBox()->lineStep();
  return s;
}

/*!
  Notification about active unit system changing. Reimplemented from QDS_Datum.
  Update validator and spin box parameters: suffix, prefix, minimum, maximum.
*/
void QDS_SpinBox::unitSystemChanged( const QString& system )
{
  QDS_Datum::unitSystemChanged( system );

  QSpinBox* sb = spinBox();
  if ( sb )
  {
    delete sb->validator();
    QValidator* valid = validator();
    sb->setValidator( valid );

    sb->setSuffix( suffix() );
    sb->setPrefix( prefix() );
    sb->setMinValue( minValue().toInt() );
    sb->setMaxValue( maxValue().toInt() );
  }
}
