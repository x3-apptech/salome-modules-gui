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

//Local includes
#include "PyEditor_Editor.h"
#include "PyEditor_LineNumberArea.h"
#include "PyEditor_PyHighlighter.h"
#include "PyEditor_Settings.h"

//Qtx includes
#include <QtxResourceMgr.h>

//Qt includes
#include <QPainter>
#include <QTextBlock>

/*!
  \class PyEditor_Editor
  \brief Viewer/Editor is used to edit and show advanced plain text.
*/

/*!
  \brief Constructor.
  \param isSingle flag determined single application or reccesed.
  \param theParent parent widget
*/
PyEditor_Editor::PyEditor_Editor( bool isSingle, QtxResourceMgr* theMgr, QWidget* theParent )
  : QPlainTextEdit( theParent )
{
  my_Settings = new PyEditor_Settings( theMgr, isSingle );

  // Create line number area
  my_LineNumberArea = new PyEditor_LineNumberArea( this );
  my_LineNumberArea->setMouseTracking( true );

  my_SyntaxHighlighter = new PyEditor_PyHighlighter( this->document() );

  // Signals and slots
  connect( this, SIGNAL( blockCountChanged( int ) ), this, SLOT( updateLineNumberAreaWidth( int ) ) );
  connect( this, SIGNAL( updateRequest( QRect, int ) ), this, SLOT( updateLineNumberArea( QRect, int ) ) );
  connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT( updateHighlightCurrentLine() ) );
  connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT( matchParentheses() ) );

  updateStatement();
}

/*!
  \brief Destructor.
*/
PyEditor_Editor::~PyEditor_Editor()
{
}

/*!
  Updates editor.
 */
void PyEditor_Editor::updateStatement()
{
  //Set font size
  QFont aFont = font();
  aFont.setFamily( settings()->p_Font.family() );
  aFont.setPointSize( settings()->p_Font.pointSize() );
  setFont( aFont );

  // Set line wrap mode
  setLineWrapMode( settings()->p_TextWrapping ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap );

  // Center the cursor on screen
  setCenterOnScroll( settings()->p_CenterCursorOnScroll );

  // Set size white spaces
  setTabStopWidth( settings()->p_TabSize * 10 );

  // Update current line highlight
  updateHighlightCurrentLine();

  // Update line numbers area
  updateLineNumberAreaWidth( 0 );

  my_SyntaxHighlighter->rehighlight();
  viewport()->update();
}

/*!
  SLOT: Delete the current selection's contents
 */
void PyEditor_Editor::deleteSelected()
{
  QTextCursor aCursor = textCursor();
  if ( aCursor.hasSelection() )
    aCursor.removeSelectedText();
  setTextCursor( aCursor );
}

/*!
  \brief Reimplemented calss is to receive key press events for the plain text widget.
  \param theEvent event
 */
void PyEditor_Editor::keyPressEvent( QKeyEvent* theEvent )
{
  if ( theEvent->type() == QEvent::KeyPress )
  {
    int aKey = theEvent->key();
    Qt::KeyboardModifiers aCtrl = theEvent->modifiers() & Qt::ControlModifier;
    Qt::KeyboardModifiers aShift = theEvent->modifiers() & Qt::ShiftModifier;
    
    if ( aKey == Qt::Key_Tab || ( aKey == Qt::Key_Backtab || ( aKey == Qt::Key_Tab && aShift ) ) )
    {
      QTextCursor aCursor = textCursor();
      aCursor.beginEditBlock();
      tabIndentation( aKey == Qt::Key_Backtab );
      aCursor.endEditBlock();
      theEvent->accept();
    }
    else if ( aKey == Qt::Key_Enter || aKey == Qt::Key_Return )
    {
      QTextCursor aCursor = textCursor();
      aCursor.beginEditBlock();
      if ( lineIndent() == 0 )
      {
        QPlainTextEdit::keyPressEvent( theEvent );
      }
      aCursor.endEditBlock();
      theEvent->accept();
    }
    else if ( theEvent == QKeySequence::MoveToStartOfLine || theEvent == QKeySequence::SelectStartOfLine )
    {
      QTextCursor aCursor = this->textCursor();
      if ( QTextLayout* aLayout = aCursor.block().layout() )
      {
        if ( aLayout->lineForTextPosition( aCursor.position() - aCursor.block().position() ).lineNumber() == 0 )
        {
          handleHome( theEvent == QKeySequence::SelectStartOfLine );
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
      QPlainTextEdit::keyPressEvent( theEvent );
    }
    else
    {
      QPlainTextEdit::keyPressEvent( theEvent );
    }
  }
}

/*!
  \brief Reimplemented calss is to receive plain text widget resize events
  which are passed in the event parameter.
  \param theEvent event
 */
void PyEditor_Editor::resizeEvent( QResizeEvent* theEvent )
{
  QPlainTextEdit::resizeEvent( theEvent );

  // Change size geometry of line number area
  QRect aContentsRect = contentsRect();
  my_LineNumberArea->setGeometry(
    QRect( aContentsRect.left(),
           aContentsRect.top(),
           lineNumberAreaWidth(),
           aContentsRect.height() ) );
}

/*!
  \brief Reimplemented calss is to receive paint events passed in theEvent.
  \param theEvent event
 */
void PyEditor_Editor::paintEvent( QPaintEvent* theEvent )
{
  QPlainTextEdit::paintEvent( theEvent );

  QTextBlock aBlock( firstVisibleBlock() );
  QPointF anOffset( contentOffset() );
  QPainter aPainter( this->viewport() );

  int aTabSpaces = this->tabStopWidth() / 10;

  // Visualization tab spaces
  if ( settings()->p_TabSpaceVisible )
  {
    qreal aTop = blockBoundingGeometry( aBlock ).translated( anOffset ).top();
    while ( aBlock.isValid() && aTop <= theEvent->rect().bottom() )
    {
      if ( aBlock.isVisible() && blockBoundingGeometry( aBlock ).translated( anOffset ).toRect().intersects( theEvent->rect() ) )
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
  if ( settings()->p_VerticalEdge )
  {
    const QRect aRect = theEvent->rect();
    const QFont aFont = currentCharFormat().font();
    int aNumberColumn =  QFontMetrics( aFont ).averageCharWidth() * settings()->p_NumberColumns + anOffset.x() + document()->documentMargin();
    aPainter.setPen( QPen( Qt::lightGray, 1, Qt::SolidLine ) );
    aPainter.drawLine( aNumberColumn, aRect.top(), aNumberColumn, aRect.bottom() );
  }
}

/*!
  \return manager of setting values.
 */
PyEditor_Settings* PyEditor_Editor::settings()
{
  return my_Settings;
}

/*!
  \brief Indenting and tabbing of the text
  \param isShift flag defines reverse tab
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
  \brief Indenting and tabbing of the selected text
  \param isShift flag defines reverse tab
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
  \brief Finds the first non-white sapce in theText.
  \param theText string
  \return index of the first non-white space
 */
int PyEditor_Editor::findFirstNonSpace( const QString& theText )
{
  int i = 0;
  while ( i < theText.size() )
  {
    if ( !theText.at(i).isSpace() )
      return i;
    ++i;
  }
  return i;
}

/*!
  \brief Auto line indenting
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
  SLOT: Updates the highlight current line.
 */
void PyEditor_Editor::updateHighlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> anExtraSelections;
  if ( !isReadOnly() && settings()->p_HighlightCurrentLine )
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
  \brief Creates line number area.
  \param theEvent event for paint events.
 */
void PyEditor_Editor::lineNumberAreaPaintEvent( QPaintEvent* theEvent )
{
  QPainter aPainter( my_LineNumberArea );
  aPainter.fillRect( theEvent->rect(), QColor( Qt::lightGray ).lighter( 125 ) );

  QTextBlock aBlock = firstVisibleBlock();
  int aBlockNumber = aBlock.blockNumber();
  int aTop = (int)blockBoundingGeometry( aBlock ).translated( contentOffset() ).top();
  int aBottom = aTop + (int)blockBoundingRect( aBlock ).height();
  int aCurrentLine = document()->findBlock( textCursor().position() ).blockNumber();

  QFont aFont = aPainter.font();
  aPainter.setPen( this->palette().color( QPalette::Text ) );

  while ( aBlock.isValid() && aTop <= theEvent->rect().bottom() )
  {
    if ( aBlock.isVisible() && aBottom >= theEvent->rect().top() )
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
      aPainter.drawText( 0, aTop, my_LineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, aNumber );
    }

    aBlock = aBlock.next();
    aTop = aBottom;
    aBottom = aTop + (int)blockBoundingRect( aBlock ).height();
    ++aBlockNumber;
  }
}

/*!
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

  if ( settings()->p_LineNumberArea )
    aSpace += 5 + fontMetrics().width( QLatin1Char( '9' ) ) * aDigits;
  
  return aSpace;
}

/*!
  SLOT: Updates the width of line number area.
 */
void PyEditor_Editor::updateLineNumberAreaWidth( int /*theNewBlockCount*/ )
{
  setViewportMargins( lineNumberAreaWidth(), 0, 0, 0 );
}

/*!
  SLOT: When the editor viewport has been scrolled.
 */
void PyEditor_Editor::updateLineNumberArea( const QRect& theRect, int theDY )
{
  if ( theDY )
    my_LineNumberArea->scroll( 0, theDY );
  else
    my_LineNumberArea->update( 0, theRect.y(), my_LineNumberArea->width(), theRect.height() );

  if ( theRect.contains( viewport()->rect() ) )
    updateLineNumberAreaWidth( 0 );
}

/*!
  \brief Parenthesis management.
  SLOT: Walk through and check that we don't exceed 80 chars per line.
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
  \brief Matches the left brackets.
  \param theCurrentBlock text block
  \param theI index
  \param theNumLeftParentheses number of left parentheses
  \return \c true if the left match
 */
bool PyEditor_Editor::matchLeftParenthesis(
  const QTextBlock& theCurrentBlock, int theI, int theNumLeftParentheses )
{
  PyEditor_PyHighlighter::TextBlockData* data =
    static_cast<PyEditor_PyHighlighter::TextBlockData*>( theCurrentBlock.userData() );
  QVector<PyEditor_PyHighlighter::ParenthesisInfo*> infos = data->parentheses();

  int docPos = theCurrentBlock.position();
  for ( ; theI < infos.size(); ++theI )
  {
    PyEditor_PyHighlighter::ParenthesisInfo* info = infos.at(theI);

    if ( isLeftBrackets( info->character ) )
    {
      ++theNumLeftParentheses;
      continue;
    }

    if ( isRightBrackets( info->character ) && theNumLeftParentheses == 0 )
    {
      createParenthesisSelection( docPos + info->position );
      return true;
    }
    else
      --theNumLeftParentheses;
  }

  QTextBlock nextBlock = theCurrentBlock.next();
  if ( nextBlock.isValid() )
    return matchLeftParenthesis( nextBlock, 0, theNumLeftParentheses );

  return false;
}

/*!
  \brief Matches the right brackets.
  \param theCurrentBlock text block
  \param theI index
  \param theNumRightParentheses number of right parentheses
  \return \c true if the right match
 */
bool PyEditor_Editor::matchRightParenthesis( const QTextBlock& theCurrentBlock, int theI, int theNumRightParentheses )
{
  PyEditor_PyHighlighter::TextBlockData* data = static_cast<PyEditor_PyHighlighter::TextBlockData*>( theCurrentBlock.userData() );
  QVector<PyEditor_PyHighlighter::ParenthesisInfo*> parentheses = data->parentheses();

  int docPos = theCurrentBlock.position();
  for ( ; theI > -1 && parentheses.size() > 0; --theI )
  {
    PyEditor_PyHighlighter::ParenthesisInfo* info = parentheses.at(theI);
    if ( isRightBrackets( info->character ) )
    {
      ++theNumRightParentheses;
      continue;
    }
    if ( isLeftBrackets( info->character ) && theNumRightParentheses == 0 )
    {
      createParenthesisSelection( docPos + info->position );
      return true;
    }
    else
      --theNumRightParentheses;
  }

  QTextBlock prevBlock = theCurrentBlock.previous();
  if ( prevBlock.isValid() )
  {
    PyEditor_PyHighlighter::TextBlockData* data = static_cast<PyEditor_PyHighlighter::TextBlockData*>( prevBlock.userData() );
    QVector<PyEditor_PyHighlighter::ParenthesisInfo*> parentheses = data->parentheses();
    return matchRightParenthesis( prevBlock, parentheses.size() - 1, theNumRightParentheses );
  }

  return false;
}

/*!
  \brief Creates brackets selection.
  \param thePosition position
 */
// Create brackets
void PyEditor_Editor::createParenthesisSelection( int thePosition )
{
  QList<QTextEdit::ExtraSelection> selections = extraSelections();

  QTextEdit::ExtraSelection selection;

  QTextCharFormat format = selection.format;
  format.setForeground( Qt::red );
  format.setBackground( Qt::white );
  selection.format = format;

  QTextCursor cursor = textCursor();
  cursor.setPosition( thePosition );
  cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
  selection.cursor = cursor;

  selections.append( selection );
  setExtraSelections( selections );
}

/*!
  return true whether the left bracket
 */
bool PyEditor_Editor::isLeftBrackets( QChar theSymbol )
{
  return theSymbol == '(' || theSymbol == '{' || theSymbol == '[';
}

/*!
  Appends a new paragraph with text to the end of the text edit.
 */
void PyEditor_Editor::append( const QString & text ) {
  QPlainTextEdit::appendPlainText(text);
}

/*!
  Sets the text edit's text.
*/
void PyEditor_Editor::setText( const QString & text ) {
  QPlainTextEdit::appendPlainText(text);
}

/*!
  return true whether the right bracket
 */
bool PyEditor_Editor::isRightBrackets( QChar theSymbol )
{
  return theSymbol == ')' || theSymbol == '}' || theSymbol == ']';
}
