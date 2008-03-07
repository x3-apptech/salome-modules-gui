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
#include "QDS_CheckBox.h"

#include <qcheckbox.h>

/*
  \class QDS_CheckBox
  
  Datum with control corresponding to check box. This control can have only two states:
  1 (on/true) or 0 (off/false). QDS_CheckBox don't take into account standard parameter
  properties (minimum, maximum, filter, etc).

  QDS_CheckBox can set and get following values for access methods (setStringValue(),
  setIntegerValue(), setDoubleValue(), stringValue(), integerValue(), doubleValue()):
    \li "1"  - check box state is setted as on.
    \li "0"  - check box state is setted as off.
    \li "-1" - check box state is setted as "NoChage" (undefined).

  User can set and check a state "NoChange" using methods clear() and isEmpty() accordingly.
*/

/*!
  Constructor. Create check box datum object with datum identifier \aid under widget \aparent.
  Parameter \aflags define behaviour of datum and set of created subwidgets. Default value of
  this parameter is QDS::All. Parameter \acomp specify the component name which will be used
  during search of dictionary item.
*/
QDS_CheckBox::QDS_CheckBox( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  Destructor.
*/
QDS_CheckBox::~QDS_CheckBox()
{
}

/*!
  Sets the state "NoChange" for checkbox.
*/
void QDS_CheckBox::clear()
{
  setStringValue( "-1" );
}

/*!
  Returns string from QCheckBox widget. If the check box state is on then 1 returned otherwise 0.
*/
QString QDS_CheckBox::getString() const
{
  QString val;
  if ( checkBox() && checkBox()->state() != QButton::NoChange )
    val = checkBox()->isChecked() ? "1" : "0";
  return val;
}

/*!
  Sets the string into QCheckBox widget. If argument \atxt is string with number "1" then check box
  state is setted as on. If argument \atxt is string with number "0" then state is setted as off.
  If argument \atxt is string with number "-1" then state is setted as "NoChage" (undefined).
*/
void QDS_CheckBox::setString( const QString& txt )
{
  if ( !checkBox() )
    return;

  bool isOk;
  int val = (int)txt.toDouble( &isOk );
  if ( isOk && val < 0 )
  {
    checkBox()->setTristate();
    checkBox()->setNoChange();
  }
  else
    checkBox()->setChecked( isOk && val != 0 );
}

/*!
  Returns pointer to QCheckBox widget.
*/
QCheckBox* QDS_CheckBox::checkBox() const
{
  return ::qt_cast<QCheckBox*>( controlWidget() );
}

/*!
  Create QCheckBox widget as control subwidget.
*/
QWidget* QDS_CheckBox::createControl( QWidget* parent )
{
  QCheckBox* cb = new QCheckBox( parent );
  connect( cb, SIGNAL( stateChanged( int ) ), SLOT( onParamChanged() ) );
  connect( cb, SIGNAL( toggled( bool ) ), SIGNAL( toggled( bool ) ) );
  connect( cb, SIGNAL( stateChanged( int ) ), this, SLOT( onStateChanged( int ) ) );
  return cb;
}

/*!
  Notify about ñhanging of control state
*/
void QDS_CheckBox::onParamChanged()
{
  emit paramChanged();
}

/*!
  Notify about ñhanging of control state. Switch off check box property "tristate" when
  state changed by user.
*/
void QDS_CheckBox::onStateChanged( int state )
{
  if ( state != QButton::NoChange && checkBox() )
    checkBox()->setTristate( false );
}

/*!
  Sets the check box state \atheState.
*/
void QDS_CheckBox::setChecked( const bool theState )
{
  if ( checkBox() )
    checkBox()->setChecked( theState );
}

/*!
  Returns current check box state.
*/
bool QDS_CheckBox::isChecked() const
{
  return checkBox() ? checkBox()->isChecked() : false;
}
