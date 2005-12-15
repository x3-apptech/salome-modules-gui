#include "QDS_SpinBox.h"

#include <qspinbox.h>
#include <qvalidator.h>

/*!
  Constructor.
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
  Returns string from QSpinBox widget.
*/
QString QDS_SpinBox::getString() const
{
  QString res;
  QSpinBox* aSpinBox = spinBox();
  if ( aSpinBox )
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
  Sets the string into QSpinBox widget.
*/
void QDS_SpinBox::setString( const QString& txt )
{
  if ( spinBox() )
    spinBox()->setValue( txt.toInt() );
}

/*!
  Returns pointer to QSpinBox widget.
*/
QSpinBox* QDS_SpinBox::spinBox() const
{
  return ::qt_cast<QSpinBox*>( controlWidget() );
}

/*!
  Create QSpinBox widget as control subwidget.
*/
QWidget* QDS_SpinBox::createControl( QWidget* parent )
{
  QSpinBox* aSpinBox = new QSpinBox( parent );
  aSpinBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( aSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( onValueChanged( int ) ) );
  return aSpinBox;
}

/*!
  Notify about text changing in spin box.
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
  This method is redefined from ancestor class to perform own initialization ( suffix, prefix, etc ).
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
