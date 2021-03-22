// Copyright (C) 2015-2021  OPEN CASCADE
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
// File   : PyEditor_FindTool.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "PyEditor_FindTool.h"
#include "PyEditor_Editor.h"

#include <QAction>
#include <QCompleter>
#include <QEvent>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QSignalMapper>
#include <QToolButton>

/*!
  \class PyEditor_FindTool
  \brief Find / Replace widget for PyEditor
*/

/*!
  \brief Constructor.
  \param editor Python editor widget.
  \param parent Parent widget.
*/
PyEditor_FindTool::PyEditor_FindTool( PyEditor_Editor* editor, QWidget* parent )
  : QWidget( parent ), myEditor( editor )
{
  QLabel* findLabel = new QLabel( tr( "FIND_LABEL" ), this );
  myFindEdit = new QLineEdit( this );
  myFindEdit->setClearButtonEnabled( true );
  myFindEdit->installEventFilter( this );
  connect( myFindEdit, SIGNAL( textChanged( const QString& ) ), this, SLOT( find( const QString& ) ) );
  connect( myFindEdit, SIGNAL( returnPressed() ), this, SLOT( findNext() ) );
  myFindEdit->setCompleter( new QCompleter( myFindEdit ) );
  myFindEdit->completer()->setModel( &myFindCompletion );

  QLabel* replaceLabel = new QLabel( tr( "REPLACE_LABEL" ), this );
  myReplaceEdit = new QLineEdit( this );
  myReplaceEdit->setClearButtonEnabled( true );
  myReplaceEdit->installEventFilter( this );
  myReplaceEdit->setCompleter( new QCompleter( myReplaceEdit ) );
  myReplaceEdit->completer()->setModel( &myReplaceCompletion );

  myInfoLabel = new QLabel( this );
  myInfoLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  QToolButton* prevBtn = new QToolButton( this );
  prevBtn->setIcon( QIcon( ":images/py_find_previous.png" ) );
  prevBtn->setAutoRaise( true );
  connect( prevBtn, SIGNAL( clicked() ), this, SLOT( findPrevious() ) );

  QToolButton* nextBtn = new QToolButton( this );
  nextBtn->setIcon( QIcon( ":images/py_find_next.png" ) );
  nextBtn->setAutoRaise( true );
  connect( nextBtn, SIGNAL( clicked() ), this, SLOT( findNext() ) );

  QToolButton* replaceBtn = new QToolButton();
  replaceBtn->setText( tr( "REPLACE_BTN" ) );
  replaceBtn->setAutoRaise( true );
  connect( replaceBtn, SIGNAL( clicked() ), this, SLOT( replace() ) );

  QToolButton* replaceAllBtn = new QToolButton();
  replaceAllBtn->setText( tr( "REPLACE_ALL_BTN" ) );
  replaceAllBtn->setAutoRaise( true );
  connect( replaceAllBtn, SIGNAL( clicked() ), this, SLOT( replaceAll() ) );

  QHBoxLayout* hl = new QHBoxLayout;
  hl->setContentsMargins( 0, 0, 0, 0 );
  hl->setSpacing( 0 );
  hl->addWidget( prevBtn );
  hl->addWidget( nextBtn );

  QGridLayout* l = new QGridLayout( this );
  l->setContentsMargins( 6, 2, 6, 2 );
  l->setSpacing( 2 );
  l->addWidget( findLabel, 0, 0 );
  l->addWidget( myFindEdit, 0, 1 );
  l->addLayout( hl, 0, 2 );
  l->addWidget( myInfoLabel, 0, 3 );
  l->addWidget( replaceLabel, 1, 0 );
  l->addWidget( myReplaceEdit, 1, 1 );
  l->addWidget( replaceBtn, 1, 2 );
  l->addWidget( replaceAllBtn, 1, 3 );

  QAction* menuAction = myFindEdit->addAction( QIcon(":images/py_search.png"), QLineEdit::LeadingPosition );
  connect( menuAction, SIGNAL( triggered( bool ) ), this, SLOT( showMenu() ) );

  addAction( new QAction( tr( "CASE_SENSITIVE_CHECK" ), this ) );
  addAction( new QAction( tr( "WHOLE_WORDS_CHECK" ), this ) );
  addAction( new QAction( tr( "REGEX_CHECK" ), this ) );
  addAction( new QAction( QIcon( ":/images/py_find.png" ), tr( "Find" ), this ) );
  addAction( new QAction( tr( "FindPrevious" ), this ) );
  addAction( new QAction( tr( "FindNext" ), this ) );
  addAction( new QAction( QIcon( ":/images/py_replace.png" ), tr( "Replace" ), this ) );

  foreach ( QAction* action, actions().mid( CaseSensitive, RegExp+1 ) )
  {
    action->setCheckable( true );
    connect( action, SIGNAL( toggled( bool ) ), this, SLOT( update() ) );
  }

  QSignalMapper* mapper = new QSignalMapper( this );
  connect( mapper, SIGNAL( mapped( int ) ), this, SLOT( activate( int ) ) );

  for ( int i = Find; i < actions().count(); i++ )
  {
    QAction* action = actions()[i];
    action->setShortcuts( shortcuts( i ) );
    action->setShortcutContext( Qt::WidgetWithChildrenShortcut );
    connect( action, SIGNAL( triggered( bool ) ), mapper, SLOT( map() ) );
    mapper->setMapping( action, i );
    myEditor->addAction( action );
  }

  myEditor->installEventFilter( this );
  connect( myEditor, SIGNAL( customizeMenu( QMenu* ) ), this, SLOT( customizeMenu( QMenu* ) ) );

  hide();
}

/*!
  \brief Process events for this widget,
  \param e Event being processed.
  \return true if event's processing should be stopped; false otherwise.
*/
bool PyEditor_FindTool::event( QEvent* e )
{
  if ( e->type() == QEvent::EnabledChange )
  {
    updateShortcuts();
  }
  else if ( e->type() == QEvent::KeyPress )
  {
    QKeyEvent* ke = (QKeyEvent*)e;
    switch ( ke->key() )
    {
    case Qt::Key_Escape:
      hide();
      return true;
    default:
      break;
    }
  }
  else if ( e->type() == QEvent::Hide )
  {
    addCompletion( myFindEdit->text(), false );
    addCompletion( myReplaceEdit->text(), true );
    myEditor->setFocus();
  }
  return QWidget::event( e );
}

/*!
  \brief Filter events from watched objects.
  \param o Object being watched.
  \param e Event being processed.
  \return true if event should be filtered out; false otherwise.
*/
bool PyEditor_FindTool::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myFindEdit )
  {
    if ( e->type() == QEvent::KeyPress )
    {
      QKeyEvent* keyEvent = (QKeyEvent*)e;
      if ( keyEvent->key() == Qt::Key_Escape && !myFindEdit->text().isEmpty() )
      {
        addCompletion( myFindEdit->text(), false );
        myFindEdit->clear();
        return true;
      }
    }
  }
  else if ( o == myReplaceEdit )
  {
    if ( e->type() == QEvent::KeyPress )
    {
      QKeyEvent* keyEvent = (QKeyEvent*)e;
      if ( keyEvent->key() == Qt::Key_Escape && !myReplaceEdit->text().isEmpty() )
      {
        myReplaceEdit->clear();
        return true;
      }
    }
  }
  else if ( o == myEditor )
  {
    if ( e->type() == QEvent::EnabledChange )
    {
      setEnabled( myEditor->isEnabled() );
    }
    else if ( e->type() == QEvent::Hide )
    {
      hide();
    }
    else if ( e->type() == QEvent::KeyPress )
    {
      QKeyEvent* ke = (QKeyEvent*)e;
      switch ( ke->key() )
      {
      case Qt::Key_Escape:
        if ( isVisible() )
        {
          hide();
          return true;
        }
        break;
      default:
        break;
      }
    }
  }
  return QWidget::eventFilter( o, e );
}

/*!
  \brief Slot: activate 'Find' dialog.
*/
void PyEditor_FindTool::activateFind()
{
  activate( Find );
}

/*!
  \brief Customize menu for editor.
*/
void PyEditor_FindTool::customizeMenu( QMenu* menu )
{
  menu->addSeparator();
  menu->addAction( actions()[Find] );
  menu->addAction( actions()[Replace] );
}

/*!
  \brief Slot: activate 'Replace' dialog.
*/
void PyEditor_FindTool::activateReplace()
{
  activate( Replace );
}

/*!
  \brief Slot: show context menu with search options.
  \internal
*/
void PyEditor_FindTool::showMenu()
{
  QMenu::exec( actions().mid( CaseSensitive, RegExp+1 ), QCursor::pos() );
}
 
/*!
  \brief Slot: find text being typed in the 'Find' control.
  \param text Text entered by the user.
  \internal
*/
void PyEditor_FindTool::find( const QString& text )
{
  find( text, 0 );
}

/*!
  \brief Slot: find text entered in the 'Find' control.
  \internal
  \overload
*/
void PyEditor_FindTool::find()
{
  find( myFindEdit->text(), 0 );
}

/*!
  \brief Slot: find previous matched item; called when user presses 'Previous' button.
  \internal
*/
void PyEditor_FindTool::findPrevious()
{
  find( myFindEdit->text(), -1 );
}

/*!
  \brief Slot: find next matched item; called when user presses 'Next' button.
  \internal
*/
void PyEditor_FindTool::findNext()
{
  find( myFindEdit->text(), 1 );
}

/*!
  \brief Slot: replace currently selected match; called when user presses 'Replace' button.
  \internal
*/
void PyEditor_FindTool::replace()
{
  QString text = myFindEdit->text();
  QString replacement = myReplaceEdit->text();

  QTextCursor editor = myEditor->textCursor();
  if ( editor.hasSelection() && editor.selectedText() == text )
  {
    editor.beginEditBlock();
    editor.removeSelectedText();
    editor.insertText( replacement );
    editor.endEditBlock();
    find();
  }
}

/*!
  \brief Slot: replace all matches; called when user presses 'Replace All' button.
  \internal
*/
void PyEditor_FindTool::replaceAll()
{
  QString text = myFindEdit->text();
  QString replacement = myReplaceEdit->text();
  QList<QTextCursor> results = matches( text );
  if ( !results.isEmpty() )
  {
    QTextCursor editor( myEditor->document() );
    editor.beginEditBlock();
    foreach ( QTextCursor cursor, results )
    {
      editor.setPosition( cursor.anchor() );
      editor.setPosition( cursor.position(), QTextCursor::KeepAnchor );
      editor.removeSelectedText();
      editor.insertText( replacement );
    }
    editor.endEditBlock();
    find();
  }
}

/*!
  \brief Slot: restart search; called when search options are changed.
  \internal
*/
void PyEditor_FindTool::update()
{
  find();
}

/*!
  \brief Slot: activate action; called when user types corresponding shortcut.
  \param action Action being activated.
  \internal
*/
void PyEditor_FindTool::activate( int action )
{
  QTextCursor cursor = myEditor->textCursor();
  cursor.movePosition( QTextCursor::StartOfWord );
  cursor.movePosition( QTextCursor::EndOfWord, QTextCursor::KeepAnchor );
  QString word = cursor.selectedText();

  switch ( action )
  {
  case Find:
  case Replace:
    showReplaceControls( action == Replace );
    show();
    if ( !word.isEmpty() ) {
      myFindEdit->setText( word );
      myEditor->setTextCursor( cursor );
    }
    myFindEdit->setFocus();
    myFindEdit->selectAll();
    find( myFindEdit->text() );
    break;
  case FindPrevious:
    findPrevious();
    break;
  case FindNext:
    findNext();
    break;
  default:
    break;
  }
}

/*!
  \brief Get shortcuts for given action.
  \param action Editor's action.
  \return List of shortcuts.
  \internal
*/
QList<QKeySequence> PyEditor_FindTool::shortcuts( int action ) const
{
  QList<QKeySequence> bindings;
  switch ( action )
  {
  case Find:
    bindings << QKeySequence( QKeySequence::Find );
    break;
  case FindPrevious:
    bindings << QKeySequence( QKeySequence::FindPrevious );
    break;
  case FindNext:
    bindings << QKeySequence( QKeySequence::FindNext );
    break;
  case Replace:
    bindings << QKeySequence( "Ctrl+H" );
    bindings << QKeySequence( QKeySequence::Replace );
    break;
  default:
    break;
  }
  return bindings;
}

/*!
  \brief Update shortcuts when widget is enabled / disabled.
  \internal
*/
void PyEditor_FindTool::updateShortcuts()
{
  foreach ( QAction* action, actions().mid( Find ) )
  {
    action->setEnabled( isEnabled() && myEditor->isEnabled() );
  }
}

/*!
  \brief Show / hide 'Replace' controls.
  \param on Visibility flag.
  \internal
*/
void PyEditor_FindTool::showReplaceControls( bool on )
{
  QGridLayout* l = qobject_cast<QGridLayout*>( layout() );
  for ( int j = 0; j < l->columnCount(); j++ )
  {
    if ( l->itemAtPosition( 1, j )->widget() )
      l->itemAtPosition( 1, j )->widget()->setVisible( on );
  }
}

/*!
  \brief Set palette for 'Find' tool depending on results of search.
  \param found Search result: true in case of success; false otherwise.
  \internal
*/
void PyEditor_FindTool::setSearchResult( bool found )
{
  QPalette pal = myFindEdit->palette();
  QPalette ref = myReplaceEdit->palette();
  pal.setColor( QPalette::Active, QPalette::Text,
                found ? ref.color( QPalette::Active, QPalette::Text ) : QColor( 255, 0, 0 ) );
  myFindEdit->setPalette( pal );
}

/*!
  \brief Get 'Use regular expression' search option.
  \return true if option is switched on; false otherwise.
  \internal
*/
bool PyEditor_FindTool::isRegExp() const
{
  return actions()[RegExp]->isChecked();
}

/*!
  \brief Get 'Case sensitive search' search option.
  \return true if option is switched on; false otherwise.
  \internal
*/
bool PyEditor_FindTool::isCaseSensitive() const
{
  return actions()[CaseSensitive]->isChecked();
}

/*!
  \brief Get 'Whole words only' search option.
  \return true if option is switched on; false otherwise.
  \internal
*/
bool PyEditor_FindTool::isWholeWord() const
{
  return actions()[WholeWord]->isChecked();
}

/*!
  \brief Get search options.
  \param back Search direction: backward if false; forward otherwise.
  \return List of options
  \internal
*/
QTextDocument::FindFlags PyEditor_FindTool::searchFlags( bool back ) const
{
  QTextDocument::FindFlags flags = 0;
  if ( isCaseSensitive() )
    flags |= QTextDocument::FindCaseSensitively;
  if ( isWholeWord() )
    flags |= QTextDocument::FindWholeWords;
  if ( back )
    flags |= QTextDocument::FindBackward;
  return flags;
}

/*!
  \brief Get all matches from Python editor.
  \param text Text being searched.
  \return List of all matches.
  \internal
*/
QList<QTextCursor> PyEditor_FindTool::matches( const QString& text ) const
{
  QList<QTextCursor> results;

  QTextDocument* document = myEditor->document();

  QTextCursor cursor( document );
  while ( !cursor.isNull() )
  {
    cursor = isRegExp() ? 
      document->find( QRegExp( text, isCaseSensitive() ?
                               Qt::CaseSensitive : Qt::CaseInsensitive ),
                      cursor, searchFlags() ) : 
      document->find( text, cursor, searchFlags() );
    if ( !cursor.isNull() )
      results.append( cursor );
  }
  return results;
}

/*!
  \brief Find specified text.
  \param text Text being searched.
  \param delta Search direction.
  \internal
*/
void PyEditor_FindTool::find( const QString& text, int delta )
{
  QTextCursor cursor = myEditor->textCursor();
  int position = qMin( cursor.position(), cursor.anchor() ) + delta;
  cursor.setPosition( position );
  myEditor->setTextCursor( cursor );

  QList<QTextCursor> results = matches( text );

  int index = -1;
  if ( !results.isEmpty() )
  {
    if ( delta >= 0 )
    {
      // search forward
      if ( position > results.last().anchor() )
        position = 0;
      for ( int i = 0; i < results.count() && index == -1; i++ )
      {
        QTextCursor result = results[i];
        if ( result.hasSelection() && position <= result.anchor() )
        {
          index = i;
        }
      }
    }
    else
    {
      // search backward
      if ( position < results.first().position() )
        position = results.last().position();

      for ( int i = results.count()-1; i >= 0 && index == -1; i-- )
      {
        QTextCursor result = results[i];
        if ( result.hasSelection() && position >= result.position() )
        {
          index = i;
        }
      }
    }
  }
  if ( index != -1 )
  {
    myInfoLabel->setText( tr( "NB_MATCHED_LABEL" ).arg( index+1 ).arg( results.count() ) );
    myEditor->setTextCursor( results[index] );
  }
  else
  {
    myInfoLabel->clear();
    cursor.clearSelection();
    myEditor->setTextCursor( cursor );
  }

  setSearchResult( text.isEmpty() || !results.isEmpty() );
}

/*!
  \brief Add completion.
  \param text Completeion being added.
  \param replace true to add 'Replace' completion; false to add 'Find' completion.
  \internal
*/
void PyEditor_FindTool::addCompletion( const QString& text, bool replace )
{
  QStringListModel& model = replace ? myReplaceCompletion : myFindCompletion;

  QStringList completions = model.stringList();
  if ( !text.isEmpty() && !completions.contains( text ) )
  {
    completions.prepend( text );
    model.setStringList( completions );
  }
}
