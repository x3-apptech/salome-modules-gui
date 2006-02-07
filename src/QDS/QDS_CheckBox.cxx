#include "QDS_CheckBox.h"

#include <qcheckbox.h>

/*!
  Constructor. This method is protected. Object can't be directly constructed.
  Use static method QDS_CheckBox::Create instead.
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
  Returns string from QCheckBox widget.
*/
QString QDS_CheckBox::getString() const
{
  QString val;
  if ( checkBox() && checkBox()->state() != QButton::NoChange )
    val = checkBox()->isChecked() ? "1" : "0";
  return val;
}

/*!
  Sets the string into QCheckBox widget.
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
  Notify about shanging of control state
*/
void QDS_CheckBox::onParamChanged()
{
  emit paramChanged();
}

void QDS_CheckBox::onStateChanged( int state )
{
  if ( state != QButton::NoChange && checkBox() )
    checkBox()->setTristate( false );
}

void QDS_CheckBox::setChecked( const bool theState )
{
  if ( checkBox() )
    checkBox()->setChecked( theState );
}

bool QDS_CheckBox::isChecked() const
{
  return checkBox() ? checkBox()->isChecked() : false;
}
