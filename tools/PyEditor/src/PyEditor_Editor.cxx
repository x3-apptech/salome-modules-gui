// Copyright (C) 2015-2016  OPEN CASCADE
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
// File   : PyEditor_Editor.cxx
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#include "PyEditor_Editor.h"
#include "PyEditor_LineNumberArea.h"
#include "PyEditor_PyHighlighter.h"
#include "PyEditor_Settings.h"

#include <QPainter>
#include <QTextBlock>

/*!
  \class PyEditor_Editor
  \brief Widget to show / edit Python scripts.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
PyEditor_Editor::PyEditor_Editor( QWidget* parent )
  : QPlainTextEdit( parent )
{
  // Set up line number area
  myLineNumberArea = new PyEditor_LineNumberArea( this );
  myLineNumberArea->setMouseTracking( true );

  // Set up syntax highighter
  mySyntaxHighlighter = new PyEditor_PyHighlighter( this->document() );

  // Set-up settings
  PyEditor_Settings* settings = PyEditor_Settings::settings();
  if ( settings )
    setSettings( *settings );

  // Connect signals
  connect( this, SIGNAL( blockCountChanged( int ) ), this, SLOT( updateLineNumberAreaWidth( int ) ) );
  connect( this, SIGNAL( updateRequest( QRect, int ) ), this, SLOT( updateLineNumberArea( QRect, int ) ) );
  connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT( updateHighlightCurrentLine() ) );
  connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT( matchParentheses() ) );
}

/*!
  \brief Destructor.
*/
PyEditor_Editor::~PyEditor_Editor()
{
}

/*!
  \brief Get editor settings.
  \return settings object
*/
const PyEditor_Settings& PyEditor_Editor::settings() const
{
  return mySettings;
}

/*!
  \brief Set editor settings.
  \param settings new settings
*/
void PyEditor_Editor::setSettings( const PyEditor_Settings& settings )
{
  mySettings.copyFrom( settings );

  // Set font size
  QFont aFont = font();
  aFont.setFamily( mySettings.font().family() );
  aFont.setPointSize( mySettings.font().pointSize() );
  setFont( aFont );

  // Set line wrap mode
  setLineWrapMode( mySettings.textWrapping() ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap );

  // Center the cursor on screen
  setCenterOnScroll( mySettings.centerCursorOnScroll() );

  // Set size white spaces
  setTabStopWidth( mySettings.tabSize() * 10 );

  // Update current line highlight
  updateHighlightCurrentLine();

  // Update line numbers area
  updateLineNumberAreaWidth( 0 );

  mySyntaxHighlighter->rehighlight();
  viewport()->update();
}

/*!
  Delete current selection contents.
*/
void PyEditor_Editor::deleteSelected()
{
  QTextCursor aCursor = textCursor();
  if ( aCursor.hasSelection() )
    aCursor.removeSelectedText();
  setTextCursor( aCursor );
}

/*!
  \brief Process key press event.
  Reimplemented from QPlainTextEdit.
  \param event key press event
*/
void PyEditor_Editor::keyPressEvent( QKeyEvent* event )
{
  if ( event->type() == QEvent::KeyPress )
  {
    int aKey = event->key();
    Qt::KeyboardModifiers aCtrl = event->modifiers() & Qt::ControlModifier;
    Qt::KeyboardModifiers aShift = event->modifiers() & Qt::ShiftModifier;
    
    if ( aKey == Qt::Key_Tab || ( aKey == Qt::Key_Backtab || ( aKey == Qt::Key_Tab && aShift ) ) )
    {
      QTextCursor aCursor = textCursor();
      aCursor.beginEditBlock();
      tabIndentation( aKey == Qt::Key_Backtab );
      aCursor.endEditBlock();
      event->accept();
    }
    else if ( aKey == Qt::Key_Enter || aKey == Qt::Key_Return )
    {
      QTextCursor aCursor = textCursor();
      aCursor.beginEditBlock();
      if ( lineIndent() == 0 )
      {
        QPlainTextEdit::keyPressEvent( event );
      }
      aCursor.endEditBlock();
      event->accept();
    }
    else if ( event == QKeySequence::MoveToStartOfLine || event == QKeySequence::SelectStartOfLine )
    {
      QTextCursor aCursor = this->textCursor();
      if ( QTextLayout* aLayout = aCursor.block().layout() )
      {
        if ( aLayout->lineForTextPosition( aCursor.position() - aCursor.block().position() ).lineNumber() == 0 )
        {
          handleHome( event == QKeySequence::SelectStartOfLine );
        }
      }
    }
    else if ( ( aKey == Qt::Key_Colon || ( aKey == Qt::Key_Space && !aCtrl && !aShift ) ) &&
              !textCursor().hasSelection() )
    {
      QTextCursor aCursor = textCursor();
      aCursor.movePosition( QTextCursor::StartOfBlock, QTextCursor::KeepAnchor );
      
      QString aSelectedText = aCursor.selectedText();
      int numSpaces = findFirstNonSpace( aSelectedText );
      int amountChars = aSelectedText.size() - findFirstNonSpace( aSelectedText );
      QString aLeadingText = aSelectedText.right( amountChars );
      
      QStringList aReservedWords;
      aReservedWords.append( "except" );
      if ( aKey == Qt::Key_Colon )
      {
        aReservedWords.append( "else" );
        aReservedWords.append( "finally" );
      }
      else if ( aKey == Qt::Key_Space )
      {
        aReservedWords.append( "elif" );
      }
      
      if ( aReservedWords.contains( aLeadingText ) )
      {
        QString aPreviousText = aCursor.block().previous().text();
        int numSpacesPrevious = findFirstNonSpace( aPreviousText );
        if ( numSpaces == numSpacesPrevious )
        {
          tabIndentation( true );
          aCursor.movePosition( QTextCursor::EndOfBlock );
          setTextCursor( aCursor );
        }
      }
      QPlainTextEdit::keyPressEvent( event );
    }
    else
    {
      QPlainTextEdit::keyPressEvent( event );
    }
  }
}

/*!
  \brief Handle resize event.
  Reimplemented from QPlainTextEdit.
  \param event resize event
*/
void PyEditor_Editor::resizeEvent( QResizeEvent* event )
{
  QPlainTextEdit::resizeEvent( event );

  // Change size geometry of line number area
  QRect aContentsRect = contentsRect();
  myLineNumberArea->setGeometry( QRect( aContentsRect.left(),
                                        aContentsRect.top(),
                                        lineNumberAreaWidth(),
                                        aContentsRect.height() ) );
}

/*!
  \brief Paint event.
  Reimplemented from QPlainTextEdit.
  \param event paint event
*/
void PyEditor_Editor::paintEvent( QPaintEvent* event )
{
  QPlainTextEdit::paintEvent( event );

  QTextBlock aBlock( firstVisibleBlock() );
  QPointF anOffset( contentOffset() );
  QPainter aPainter( this->viewport() );

  int aTabSpaces = this->tabStopWidth() / 10;

  // Visualization tab spaces
  if ( mySettings.tabSpaceVisible() )
  {
    qreal aTop = blockBoundingGeometry( aBlock ).translated( anOffset ).top();
    while ( aBlock.isValid() && aTop <= event->rect().bottom() )
    {
      if ( aBlock.isVisible() && blockBoundingGeometry( aBlock ).translated( anOffset ).toRect().intersects( event->rect() ) )
      {
        QString aText = aBlock.text();
        if ( aText.contains( QRegExp( "\\w+" ) ) )
          aText.remove( QRegExp( "(?!\\w+)\\s+$" ) );
        
        int aColumn = 0;
        int anIndex = 0;
        while ( anIndex != -1 )
        {
          anIndex = aText.indexOf( QRegExp( QString( "^\\s{%1}" ).arg( aTabSpaces ) ), 0 );
          if ( anIndex != -1 )
          {
            aColumn = aColumn + aTabSpaces;
            aText = aText.mid( aTabSpaces );
    
            if ( aText.startsWith( ' ' ) )
            {
              QTextCursor aCursor( aBlock );
              aCursor.setPosition( aBlock.position() + aColumn );
      
              QRect aRect = cursorRect( aCursor );
              aPainter.setPen( QPen( Qt::darkGray, 1, Qt::DotLine ) );
              aPainter.drawLine( aRect.x() + 1, aRect.top(), aRect.x() + 1, aRect.bottom() );
            }
          }
        }
      }
      aBlock = aBlock.next();
    }
  }
  
  // Vertical edge line
  if ( mySettings.verticalEdge() )
  {
    const QRect aRect = event->rect();
    const QFont aFont = currentCharFormat().font();
    int aNumberColumn =  QFontMetrics( aFont ).averageCharWidth() * mySettings.numberColumns() + anOffset.x() + document()->documentMargin();
    aPainter.setPen( QPen( Qt::lightGray, 1, Qt::SolidLine ) );
    aPainter.drawLine( aNumberColumn, aRect.top(), aNumberColumn, aRect.bottom() );
  }
}

/*!
  \brief Indent and tab text.
  \param isShift flag defines reverse tab direction
*/
void PyEditor_Editor::tabIndentation( bool isShift )
{
  QTextCursor aCursor = textCursor();
  int aTabSpaces = this->tabStopWidth()/10;

  if ( !aCursor.hasSelection() )
  {
    if ( !isShift )
    {
      int N = aCursor.columnNumber() % aTabSpaces;
      aCursor.insertText( QString( aTabSpaces - N, QLatin1Char( ' ' ) ) );
    }
    else
    {
      QTextBlock aCurrentBlock = document()->findBlock( aCursor.position() );
      int anIndentPos = findFirstNonSpace( aCurrentBlock.text() );
      aCursor.setPosition( aCurrentBlock.position() + anIndentPos );
      setTextCursor( aCursor );
      
      //if ( aCurrCursorColumnPos <= anIndentPos )
      //{
      int aColumnPos = aCursor.columnNumber();
      if ( aColumnPos != 0 )
      {
        int N = aCursor.columnNumber() % aTabSpaces;
        if ( N == 0 ) N = aTabSpaces;
        aCursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor, N );
        aCursor.removeSelectedText();
      }
      setTextCursor( aCursor );
      //}
    }
  }
  else
  {
    indentSelection( isShift );
  }
}

/*!
  \brief Indent and tab selected text.
  \param isShift flag defines reverse tab direction
*/
void PyEditor_Editor::indentSelection( bool isShift )
{
  QTextCursor aCursor = this->textCursor();

  int aCursorStart = aCursor.selectionStart();
  int aCursorEnd = aCursor.selectionEnd();

  QTextBlock aStartBlock = document()->findBlock( aCursorStart );
  QTextBlock anEndBlock = document()->findBlock( aCursorEnd - 1 ).next();

  int aTabSpaces = this->tabStopWidth()/10;

  for ( QTextBlock aBlock = aStartBlock; aBlock.isValid() && aBlock != anEndBlock; aBlock = aBlock.next() )
  {
    QString aText = aBlock.text();
    int anIndentPos = findFirstNonSpace( aText );
    int N = ( anIndentPos % aTabSpaces );
    
    aCursor.setPosition( aBlock.position() + anIndentPos );
    if ( !isShift )
    {
      aCursor.insertText( QString( aTabSpaces - N, QLatin1Char( ' ' ) ) );
      setTextCursor( aCursor );
    }
    else
    {
      int aColumnPos = aCursor.columnNumber();
      if ( aColumnPos != 0 )
      {
        int blockN = aColumnPos % aTabSpaces;
        if ( blockN == 0 ) blockN = aTabSpaces;
        aCursor.movePosition( QTextCursor::Left, QTextCursor::KeepAnchor, blockN );
        aCursor.removeSelectedText();
        setTextCursor( aCursor );
      }
    }
  }

  // Reselect the selected lines
  aCursor.setPosition( aStartBlock.position() );
  aCursor.setPosition( anEndBlock.previous().position(), QTextCursor::KeepAnchor );
  aCursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
  setTextCursor( aCursor );
}

/*!
  \brief Find first non white-space symbol in text.
  \param text input text
  \return index of first non white-space symbol
*/
int PyEditor_Editor::findFirstNonSpace( const QString& text )
{
  int i = 0;
  while ( i < text.size() )
  {
    if ( !text.at(i).isSpace() )
      return i;
    ++i;
  }
  return i;
}

/*!
  \brief Indent line.
  \return error code
*/
int PyEditor_Editor::lineIndent()
{
  int aTabSpaces = this->tabStopWidth() / 10;

  QTextCursor aCursor = textCursor();
  aCursor.insertBlock();
  setTextCursor( aCursor );

  QTextBlock aCurrentBlock = aCursor.block();
  if ( aCurrentBlock == document()->begin() )
    return 0;

  QTextBlock aPreviousBlock = aCurrentBlock.previous();

  QString aPreviousText;
  forever
  {
    if ( aPreviousBlock == document()->begin() )
    {
      aPreviousText = aPreviousBlock.text();
      if ( aPreviousText.isEmpty() && aPreviousText.trimmed().isEmpty() )
        return -1;
      break;
    }
    
    // If the text of this block is not empty then break the loop.
    aPreviousText = aPreviousBlock.text();
    if ( !aPreviousText.isEmpty() && !aPreviousText.trimmed().isEmpty() )
      break;
    
    aPreviousBlock = aPreviousBlock.previous();
  }
  
  int aTabIndentation = 0;
  int anAmountIndentation = -1;
  int i = 0;
  while ( i < aPreviousText.size() )
  {
    if ( !aPreviousText.at(i).isSpace() )
    {
      anAmountIndentation = findFirstNonSpace( aPreviousText );
      break;
    }
    else
    {
      ++aTabIndentation;
    }
    ++i;
  }
  
  if ( anAmountIndentation == -1 )
  {
    if ( aTabIndentation > 0 )
      anAmountIndentation = aTabIndentation;
    else
      return 0;
  }
  
  const QString aPreviousTrimmed = aPreviousText.trimmed();
  if ( aPreviousTrimmed.endsWith( ":" ) )
  {
    anAmountIndentation += aTabSpaces;
  }
  else
  {
    if ( aPreviousTrimmed == "continue"
      || aPreviousTrimmed == "break"
      || aPreviousTrimmed == "pass"
      || aPreviousTrimmed == "return"
      || aPreviousTrimmed == "raise"
      || aPreviousTrimmed.startsWith( "raise " )
      || aPreviousTrimmed.startsWith( "return " ) )
      anAmountIndentation -= aTabSpaces;
  }
  
  aCursor.insertText( QString( anAmountIndentation, QLatin1Char(' ') ) );
  setTextCursor( aCursor );
  
  return 1;
}

/*!
  \brief Set text cursor on home position.
  \param isExtendLine \c true to keep current anchor position
*/
void PyEditor_Editor::handleHome( bool isExtendLine )
{
  QTextCursor aCursor = textCursor();
  QTextCursor::MoveMode aMode = QTextCursor::MoveAnchor;

  if ( isExtendLine )
    aMode = QTextCursor::KeepAnchor;

  int anInitPos = aCursor.position();
  int aBlockPos = aCursor.block().position();

  QChar aCharacter = document()->characterAt( aBlockPos );
  while ( aCharacter.category() == QChar::Separator_Space )
  {
    ++aBlockPos;
    if ( aBlockPos == anInitPos )
      break;
    aCharacter = document()->characterAt( aBlockPos );
  }
  
  if ( aBlockPos == anInitPos )
    aBlockPos = aCursor.block().position();
  
  aCursor.setPosition( aBlockPos, aMode );
  setTextCursor( aCursor );
}

/*!
  \brief Update current line highlighting.
*/
void PyEditor_Editor::updateHighlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> anExtraSelections;
  if ( !isReadOnly() && mySettings.highlightCurrentLine() )
  {
    QTextEdit::ExtraSelection selection;
    
    QColor lineColor = QColor( Qt::gray ).lighter( 155 );
    
    selection.format.setBackground( lineColor );
    selection.format.setProperty( QTextFormat::FullWidthSelection, QVariant( true ) );
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    anExtraSelections.append( selection );
  }
  setExtraSelections( anExtraSelections );
}

/*!
  \brief Draw linne number area.
  \param event paint event
*/
void PyEditor_Editor::lineNumberAreaPaintEvent( QPaintEvent* event )
{
  QPainter aPainter( myLineNumberArea );
  aPainter.fillRect( event->rect(), QColor( Qt::lightGray ).lighter( 125 ) );

  QTextBlock aBlock = firstVisibleBlock();
  int aBlockNumber = aBlock.blockNumber();
  int aTop = (int)blockBoundingGeometry( aBlock ).translated( contentOffset() ).top();
  int aBottom = aTop + (int)blockBoundingRect( aBlock ).height();
  int aCurrentLine = document()->findBlock( textCursor().position() ).blockNumber();

  QFont aFont = aPainter.font();
  aPainter.setPen( this->palette().color( QPalette::Text ) );

  while ( aBlock.isValid() && aTop <= event->rect().bottom() )
  {
    if ( aBlock.isVisible() && aBottom >= event->rect().top() )
    {
      if ( aBlockNumber == aCurrentLine )
      {
        aPainter.setPen( Qt::darkGray );
        aFont.setBold( true );
        aPainter.setFont( aFont );
      }
      else
      {
        aPainter.setPen( Qt::gray ) ;
        aFont.setBold( false );
        aPainter.setFont( aFont );
      }
      QString aNumber = QString::number( aBlockNumber + 1 );
      aPainter.drawText( 0, aTop, myLineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, aNumber );
    }

    aBlock = aBlock.next();
    aTop = aBottom;
    aBottom = aTop + (int)blockBoundingRect( aBlock ).height();
    ++aBlockNumber;
  }
}

/*!
  \brief Get with of line number area.
  \return width of line number area
*/
int PyEditor_Editor::lineNumberAreaWidth()
{
  int aSpace = 0;

  int aDigits = 1;
  int aMaximum = qMax( 1, blockCount() );
  while ( aMaximum >= 10 )
  {
    aMaximum /= 10;
    ++aDigits;
  }

  if ( mySettings.lineNumberArea() )
    aSpace += 5 + fontMetrics().width( QLatin1Char( '9' ) ) * aDigits;
  
  return aSpace;
}

/*!
  \brief Update width of the line number area.
  \param newBlockCount (not used)
*/
void PyEditor_Editor::updateLineNumberAreaWidth( int /*newBlockCount*/ )
{
  setViewportMargins( lineNumberAreaWidth(), 0, 0, 0 );
}

/*!
  \brief Update line number area (when editor viewport is scrolled).
  \param rect area being updated
  \param dy scroll factor
*/
void PyEditor_Editor::updateLineNumberArea( const QRect& rect, int dy )
{
  if ( dy )
    myLineNumberArea->scroll( 0, dy );
  else
    myLineNumberArea->update( 0, rect.y(), myLineNumberArea->width(), rect.height() );

  if ( rect.contains( viewport()->rect() ) )
    updateLineNumberAreaWidth( 0 );
}

/*!
  \brief Manage parentheses.
*/
void PyEditor_Editor::matchParentheses()
{
  PyEditor_PyHighlighter::TextBlockData* data =
    static_cast<PyEditor_PyHighlighter::TextBlockData*>( textCursor().block().userData() );

  if ( data )
  {
    QVector<PyEditor_PyHighlighter::ParenthesisInfo*> infoEntries = data->parentheses();
    
    int aPos = textCursor().block().position();
    bool ignore = false;
    for ( int i = 0; i < infoEntries.size(); ++i )
    {
      PyEditor_PyHighlighter::ParenthesisInfo* info = infoEntries.at(i);
      
      int currentColumnPosition = textCursor().columnNumber();
      if ( info->position == currentColumnPosition - 1 && isLeftBrackets( info->character ) )
      {
        if ( matchLeftParenthesis( textCursor().block(), i + 1, 0 ) )
          createParenthesisSelection( aPos + info->position );
      }
      else if ( info->position == currentColumnPosition && isLeftBrackets( info->character ) )
      {
        if ( !ignore )
        {
          if ( matchLeftParenthesis( textCursor().block(), i + 1, 0 ) )
            createParenthesisSelection( aPos + info->position );
        }
      }
      else if ( info->position == currentColumnPosition - 1 && isRightBrackets( info->character ) )
      {
        if ( matchRightParenthesis( textCursor().block(), i - 1, 0 ) )
          createParenthesisSelection( aPos + info->position );
        ignore = true;
      }
      else if ( info->position == currentColumnPosition && isRightBrackets( info->character ) )
      {
        if ( matchRightParenthesis( textCursor().block(), i - 1, 0 ) )
          createParenthesisSelection( aPos + info->position );
      }
    }
  }
}

/*!
  \brief Match left brackets.
  \param currentBlock text block
  \param idx index
  \param numLeftParentheses number of left parentheses
  \return \c true if the left match
*/
bool PyEditor_Editor::matchLeftParenthesis( const QTextBlock& currentBlock,
                                            int idx, int numLeftParentheses )
{
  PyEditor_PyHighlighter::TextBlockData* data =
    static_cast<PyEditor_PyHighlighter::TextBlockData*>( currentBlock.userData() );
  QVector<PyEditor_PyHighlighter::ParenthesisInfo*> infos = data->parentheses();

  int docPos = currentBlock.position();
  for ( ; idx < infos.size(); ++idx )
  {
    PyEditor_PyHighlighter::ParenthesisInfo* info = infos.at( idx );

    if ( isLeftBrackets( info->character ) )
    {
      ++numLeftParentheses;
      continue;
    }

    if ( isRightBrackets( info->character ) && numLeftParentheses == 0 )
    {
      createParenthesisSelection( docPos + info->position );
      return true;
    }
    else
      --numLeftParentheses;
  }

  QTextBlock nextBlock = currentBlock.next();
  if ( nextBlock.isValid() )
    return matchLeftParenthesis( nextBlock, 0, numLeftParentheses );

  return false;
}

/*!
  \brief Match right brackets.
  \param currentBlock text block
  \param idx index
  \param numRightParentheses number of right parentheses
  \return \c true if the right match
*/
bool PyEditor_Editor::matchRightParenthesis( const QTextBlock& currentBlock,
                                             int idx, int numRightParentheses )
{
  PyEditor_PyHighlighter::TextBlockData* data = static_cast<PyEditor_PyHighlighter::TextBlockData*>( currentBlock.userData() );
  QVector<PyEditor_PyHighlighter::ParenthesisInfo*> parentheses = data->parentheses();

  int docPos = currentBlock.position();
  for ( ; idx > -1 && parentheses.size() > 0; --idx )
  {
    PyEditor_PyHighlighter::ParenthesisInfo* info = parentheses.at( idx );
    if ( isRightBrackets( info->character ) )
    {
      ++numRightParentheses;
      continue;
    }
    if ( isLeftBrackets( info->character ) && numRightParentheses == 0 )
    {
      createParenthesisSelection( docPos + info->position );
      return true;
    }
    else
      --numRightParentheses;
  }

  QTextBlock prevBlock = currentBlock.previous();
  if ( prevBlock.isValid() )
  {
    PyEditor_PyHighlighter::TextBlockData* data = static_cast<PyEditor_PyHighlighter::TextBlockData*>( prevBlock.userData() );
    QVector<PyEditor_PyHighlighter::ParenthesisInfo*> parentheses = data->parentheses();
    return matchRightParenthesis( prevBlock, parentheses.size() - 1, numRightParentheses );
  }

  return false;
}

/*!
  \brief Create brackets selection.
  \param position cursor position
*/
void PyEditor_Editor::createParenthesisSelection( int position )
{
  QList<QTextEdit::ExtraSelection> selections = extraSelections();

  QTextEdit::ExtraSelection selection;

  QTextCharFormat format = selection.format;
  format.setForeground( Qt::red );
  format.setBackground( Qt::white );
  selection.format = format;

  QTextCursor cursor = textCursor();
  cursor.setPosition( position );
  cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
  selection.cursor = cursor;

  selections.append( selection );
  setExtraSelections( selections );
}

/*!
  \brief Check if symbol is a left bracket.
  \param symbol text symbol
  \return \c true if symbol is any left bracket
*/
bool PyEditor_Editor::isLeftBrackets( QChar symbol )
{
  return symbol == '(' || symbol == '{' || symbol == '[';
}

/*!
  \brief Check if symbol is a right bracket.
  \param symbol text symbol
  \return \c true if symbol is any right bracket
*/
bool PyEditor_Editor::isRightBrackets( QChar symbol )
{
  return symbol == ')' || symbol == '}' || symbol == ']';
}

/*!
  \brief Append new paragraph to the end of the editor's text.
  \param text paragraph text
*/
void PyEditor_Editor::append( const QString& text )
{
  appendPlainText( text );
}

/*!
  \brief Set text to the editor.
  \param text new text
*/
void PyEditor_Editor::setText( const QString& text )
{
  setPlainText( text );
}

/*!
  \brief Get current editor's content.
  \return current text
*/
QString PyEditor_Editor::text() const
{
  return toPlainText();
}
