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
#include "QDS_TextEdit.h"

#include <qtextedit.h>

/*
  \class QDS_TextEdit

  Datum with control corresponding to text edit. User can enter parameter value in multiple line editor.
*/

/*!
  Constructor. Create text edit datum object with datum identifier \aid under widget \aparent.
  Parameter \aflags define behaviour of datum and set of created subwidgets. Default value of this
  parameter is QDS::All. Parameter \acomp specify the component name which will be used during search
  of dictionary item.
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
  Returns string from QTextEdit widget. Reimplemented from QDS_Datum.
*/
QString QDS_TextEdit::getString() const
{
  QString res;
  if ( textEdit() )
    res = textEdit()->text();
  return res;
}

/*!
  Sets the string into QTextEdit widget. Reimplemented from QDS_Datum.
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
  Notify about text changing in text edit.
*/
void QDS_TextEdit::onTextChanged()
{
  invalidateCache();

  onParamChanged();

  QString str = getString();

  emit paramChanged();
  emit paramChanged( str );
}
