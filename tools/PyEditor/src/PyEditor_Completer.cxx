// Copyright (C) 2015-2019  OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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
// File   : PyEditor_Completer.cxx
// Author : Sergey TELKOV, Open CASCADE S.A.S. (sergey.telkov@opencascade.com)
//

#include "PyEditor_Completer.h"

#include "PyEditor_Editor.h"
#include "PyEditor_Keywords.h"

#include <QSet>
#include <QTimer>
#include <QTextBlock>
#include <QTextCursor>
#include <QApplication>
#include <QAbstractItemView>
#include <QStandardItemModel>

/*!
  \brief Constructor.
*/
PyEditor_Completer::PyEditor_Completer( PyEditor_Editor* editor,
					PyEditor_Keywords* std, PyEditor_Keywords* user )
  : QCompleter( editor ),
    myEditor( editor ),
    myTimer( 0 ),
    myStdKeywords( std ),
    myUserKeywords( user )
{
  setWidget( editor );
  setCompletionMode(QCompleter::PopupCompletion);

  connect(this, SIGNAL( activated( const QString& ) ),
	  this, SLOT( onActivated( const QString& ) ) );
  connect(editor, SIGNAL( textChanged() ), this, SLOT( onTextChanged() ) );
  connect(myStdKeywords, SIGNAL( keywordsChanged() ),
	  this, SLOT( onKeywordsChanged() ) );
  connect(myUserKeywords, SIGNAL( keywordsChanged() ),
	  this, SLOT( onKeywordsChanged() ) );

  updateKeywords();
}

/*!
  \brief Destructor.
*/
PyEditor_Completer::~PyEditor_Completer()
{
}

/*!
  \brief Perform the completion if it possible.
*/
void PyEditor_Completer::perform()
{
  QString prefix = completionText();
  setCompletionPrefix( prefix );

  if ( !completionPrefix().isEmpty() && 
       ( completionCount() > 1 || ( completionCount() == 1 && 
				    currentCompletion() != completionPrefix() ) ) )
    complete(completionRect());
  else
    uncomplete();
}

/*!
  \brief Hide the completer's popup menu.
*/
void PyEditor_Completer::uncomplete()
{
  if ( popup() )
    popup()->hide();
}

/*!
  \brief Handling 'Enter' key.
*/
bool PyEditor_Completer::eventFilter(QObject* o, QEvent* e)
{
  bool res = false;
  if ( e->type() == QEvent::KeyPress && popup()->isVisible() ) {
    QKeyEvent* ke = (QKeyEvent*)e;
    if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return ) {
      res = true;
      setCurrentRow(popup()->currentIndex().row());
      onActivated(currentCompletion());
    }
  }

  if ( !res )
    res = QCompleter::eventFilter(o, e);

  return res;
}

/*!
  \brief Perform delayed completion.
*/
void PyEditor_Completer::onTimeout()
{
  perform();
}

/*!
  \brief Invoked when text changed in editor.
*/
void PyEditor_Completer::onTextChanged()
{
  uncomplete();
  if ( myEditor->completionPolicy() == PyEditor_Editor::Auto ||
       myEditor->completionPolicy() == PyEditor_Editor::Always )
    triggerComplete();
}

/*!
  \brief Invoked when keywords changed in editor.
*/
void PyEditor_Completer::onKeywordsChanged()
{
  updateKeywords();
}

/*!
  \brief Insert selected completion into editor.
*/
void PyEditor_Completer::onActivated( const QString& text)
{
  QPoint rng = completionRange();
  QTextCursor cursor = myEditor->textCursor();
  cursor.setPosition(cursor.position() - rng.y() + rng.x() - 1,
		     QTextCursor::KeepAnchor);
  cursor.insertText(text);
  uncomplete();
}

/*!
  \brief Get the rectangle for completion popup.
  \return completion popup rect
*/
QRect PyEditor_Completer::completionRect() const
{
  QRect res = myEditor->cursorRect(myEditor->textCursor());
  res.setWidth(popup()->sizeHint().width());
  res.translate(myEditor->document()->documentMargin(),
		myEditor->document()->documentMargin());
  return res;
}

/*!
  \brief Get the current completion prefix from editor.
  \return completion prefix string
*/
QString PyEditor_Completer::completionText() const
{
  QString prefix;
  if ( myEditor ) {
    QString txt = myEditor->textCursor().block().text();
    if ( !txt.isEmpty() ) {
      QPoint range = completionRange();
      prefix = txt.mid( range.x(), range.y() - range.x() + 1 );
    }
  }
  return prefix;
}

/*!
  \brief Get position of completion prefix in editor.
  \return begin and end of completion prefix
*/
QPoint PyEditor_Completer::completionRange() const
{
  QPoint range;

  if ( myEditor ) {
    QTextCursor cursor = myEditor->textCursor();
    QString txt = cursor.block().text();
    int beg = 0;
    int end = cursor.positionInBlock() - 1;

    QRegExp rx("[A-Za-z]{1}\\w*$");
    int pos = rx.indexIn(txt.mid(beg, end - beg + 1));

    if ( pos >= 0 )
      beg = pos;

    range = QPoint(beg, end);
  }

  return range;
}

/*!
  \brief Schedule the delayed completion.
*/
void PyEditor_Completer::triggerComplete()
{
  if ( !myTimer ) {
    myTimer = new QTimer( this );
    myTimer->setSingleShot( true );
    myTimer->setInterval( 200 );

    connect( myTimer, SIGNAL( timeout() ), this, SLOT( onTimeout() ) );
  }

  if ( myTimer->isActive() )
    myTimer->stop();
  myTimer->start();
}

/*!
  \brief Updates the keywords list in completer.
*/
void PyEditor_Completer::updateKeywords()
{
  QStandardItemModel* model = new QStandardItemModel( this );
  KeywordMap kwMap = keywords();
  for ( KeywordMap::const_iterator it = kwMap.begin(); it != kwMap.end(); ++it ) {
    QStandardItem* item = new QStandardItem( it.key() );
    if ( it.value().isValid() )
      item->setForeground( it.value() );
    model->appendRow( item );
  }
  setModel( model );
}

/*!
  \brief Gets the keywords list.
  \return keyword string list
*/
PyEditor_Completer::KeywordMap PyEditor_Completer::keywords() const
{
  KeywordMap res;
  QList<PyEditor_Keywords*> kwDicts;
  kwDicts << myStdKeywords << myUserKeywords;

  for ( QList<PyEditor_Keywords*>::iterator itr = kwDicts.begin(); itr != kwDicts.end(); ++itr ) {
    PyEditor_Keywords* dict = *itr;
    QStringList kwList = dict->keywords();
    for ( QStringList::const_iterator it = kwList.begin(); it != kwList.end(); ++it ) {
      if ( !res.contains( *it ) ) {
	res.insert( *it, dict->color( *it ) );
      }
    }
  }
  return res;
}
