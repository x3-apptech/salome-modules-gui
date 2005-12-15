#include "QDS_TextEdit.h"

#include <qtextedit.h>

/*!
  Constructor.
*/
QDS_TextEdit::QDS_TextEdit( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  Destructor.
*/
QDS_TextEdit::~QDS_TextEdit()
{
}

/*!
  Returns string from QTextEdit widget.
*/
QString QDS_TextEdit::getString() const
{
  QString res;
  if ( textEdit() )
    res = textEdit()->text();
  return res;
}

/*!
  Sets the string into QTextEdit widget.
*/
void QDS_TextEdit::setString( const QString& txt )
{
  if ( textEdit() )
    textEdit()->setText( txt );
}

/*!
  Returns pointer to QTextEdit widget.
*/
QTextEdit* QDS_TextEdit::textEdit() const
{
  return ::qt_cast<QTextEdit*>( controlWidget() );
}

/*!
  Create QTextEdit widget as control subwidget.
*/
QWidget* QDS_TextEdit::createControl( QWidget* parent )
{
  QTextEdit* te = new QTextEdit( parent );
  connect( te, SIGNAL( textChanged() ), this, SLOT( onTextChanged() ) );
  return te;
}

/*!
  Notify about text changing in line edit.
*/
void QDS_TextEdit::onTextChanged()
{
  invalidateCache();

  onParamChanged();

  QString str = getString();

  emit paramChanged();
  emit paramChanged( str );
}
