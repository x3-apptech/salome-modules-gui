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
// File   : PyEditor_PyHighlighter.cxx
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#include "PyEditor_PyHighlighter.h"

#define NORMAL 0
#define TRIPLESINGLE 1
#define TRIPLEDOUBLE 2

/*!
  \class PyEditor_PyHighlighter
  \brief Python highlighter class defines the syntax highlighting rules.
*/

PyEditor_PyHighlighter::TextBlockData::TextBlockData()
{
}

QVector<PyEditor_PyHighlighter::ParenthesisInfo*> PyEditor_PyHighlighter::TextBlockData::parentheses()
{
  return myParentheses;
}

void PyEditor_PyHighlighter::TextBlockData::insert( PyEditor_PyHighlighter::ParenthesisInfo* theInfo )
{
  int i = 0;
  while ( i < myParentheses.size() && theInfo->position > myParentheses.at(i)->position )
    ++i;

  myParentheses.insert( i, theInfo );
}

/*!
  \brief Constructor.
  \param theDocument container for structured rich text documents.
*/
PyEditor_PyHighlighter::PyEditor_PyHighlighter( QTextDocument* theDocument )
  : QSyntaxHighlighter( theDocument )
{
  initialize();
}

/*!
  \brief Initialization rules.
*/
void PyEditor_PyHighlighter::initialize()
{
  HighlightingRule aRule;

  // Keywords
  keywordFormat.setForeground( Qt::blue );
  QStringList aKeywords = keywords();
  foreach ( const QString& keyword, aKeywords )
  {
    aRule.pattern = QRegExp( QString( "\\b%1\\b" ).arg( keyword ) );
    aRule.format = keywordFormat;
    aRule.capture = 0;
    highlightingRules.append( aRule );
  }

  // Special keywords
  specialFromat.setForeground( Qt::magenta );
  QStringList aSpecialKeywords = specialKeywords();
  foreach ( const QString& keyword, aSpecialKeywords )
  {
    aRule.pattern = QRegExp( QString( "\\b%1\\b" ).arg( keyword ) );
    aRule.format = specialFromat;
    aRule.capture = 0;
    highlightingRules.append( aRule );
  }

  // Reference to the current instance of the class
  referenceClassFormat.setForeground( QColor( 179, 143, 0 ) );
  referenceClassFormat.setFontItalic( true );
  aRule.pattern = QRegExp( "\\bself\\b" );
  aRule.format = referenceClassFormat;
  aRule.capture = 0;
  highlightingRules.append( aRule );

  // Numbers
  numberFormat.setForeground( Qt::darkMagenta );
  aRule.pattern = QRegExp( "\\b([-+])?(\\d+(\\.)?\\d*|\\d*(\\.)?\\d+)(([eE]([-+])?)?\\d+)?\\b" );
  aRule.format = numberFormat;
  aRule.capture = 0;
  highlightingRules.append( aRule );

  // String qoutation
  quotationFormat.setForeground( Qt::darkGreen );
  aRule.pattern = QRegExp( "(?:'[^']*'|\"[^\"]*\")" );
  aRule.pattern.setMinimal( true );
  aRule.format = quotationFormat;
  aRule.capture = 0;
  highlightingRules.append( aRule );

  // Function names
  functionFormat.setFontWeight( QFont::Bold );
  aRule.pattern = QRegExp( "(?:def\\s*)(\\b[A-Za-z0-9_]+)(?=[\\W])" );
  aRule.capture = 1;
  aRule.format = functionFormat;
  highlightingRules.append( aRule );

  // Class names
  classFormat.setForeground( Qt::darkBlue );
  classFormat.setFontWeight( QFont::Bold );
  aRule.pattern = QRegExp( "(?:class\\s*)(\\b[A-Za-z0-9_]+)(?=[\\W])" );
  aRule.capture = 1;
  aRule.format = classFormat;
  highlightingRules.append( aRule );

  // Multi line comments
  multiLineCommentFormat.setForeground( Qt::darkRed );
  tripleQuotesExpression = QRegExp( "(:?\"[\"]\".*\"[\"]\"|'''.*''')" );
  aRule.pattern = tripleQuotesExpression;
  aRule.pattern.setMinimal( true );
  aRule.format = multiLineCommentFormat;
  aRule.capture = 0;
  highlightingRules.append( aRule );

  tripleSingleExpression = QRegExp( "'''(?!\")" );
  tripleDoubleExpression = QRegExp( "\"\"\"(?!')" );

  // Single comments
  singleLineCommentFormat.setForeground( Qt::darkGray );
  aRule.pattern = QRegExp( "#[^\n]*" );
  aRule.format = singleLineCommentFormat;
  aRule.capture = 0;
  highlightingRules.append( aRule );
}

/*!
  \return string list of Python keywords.
 */
QStringList PyEditor_PyHighlighter::keywords()
{
  QStringList aKeywords;
  aKeywords << "and"
            << "as"
            << "assert"
            << "break"
            << "class"
            << "continue"
            << "def"
            << "elif"
            << "else"
            << "except"
            << "exec"
            << "finally"
            << "False"
            << "for"
            << "from"
            << "global"
            << "if"
            << "import"
            << "in"
            << "is"
            << "lambda"
            << "None"
            << "not"
            << "or"
            << "pass"
            << "print"
            << "raise"
            << "return"
            << "True"
            << "try"
            << "while"
            << "with"
            << "yield";
  return aKeywords;
}

/*!
  \return string list of special Python keywords.
*/
QStringList PyEditor_PyHighlighter::specialKeywords()
{
  QStringList aSpecialKeywords;
  aSpecialKeywords << "ArithmeticError"
                   << "AssertionError"
                   << "AttributeError"
                   << "EnvironmentError"
                   << "EOFError"
                   << "Exception"
                   << "FloatingPointError"
                   << "ImportError"
                   << "IndentationError"
                   << "IndexError"
                   << "IOError"
                   << "KeyboardInterrupt"
                   << "KeyError"
                   << "LookupError"
                   << "MemoryError"
                   << "NameError"
                   << "NotImplementedError"
                   << "OSError"
                   << "OverflowError"
                   << "ReferenceError"
                   << "RuntimeError"
                   << "StandardError"
                   << "StopIteration"
                   << "SyntaxError"
                   << "SystemError"
                   << "SystemExit"
                   << "TabError"
                   << "TypeError"
                   << "UnboundLocalError"
                   << "UnicodeDecodeError"
                   << "UnicodeEncodeError"
                   << "UnicodeError"
                   << "UnicodeTranslateError"
                   << "ValueError"
                   << "WindowsError"
                   << "ZeroDivisionError"
                   << "Warning"
                   << "UserWarning"
                   << "DeprecationWarning"
                   << "PendingDeprecationWarning"
                   << "SyntaxWarning"
                   << "OverflowWarning"
                   << "RuntimeWarning"
                   << "FutureWarning";
  return aSpecialKeywords;
}

void PyEditor_PyHighlighter::highlightBlock( const QString& theText )
{
  TextBlockData* aData = new TextBlockData;
  
  insertBracketsData( RoundBrackets, aData, theText );
  insertBracketsData( CurlyBrackets, aData, theText );
  insertBracketsData( SquareBrackets, aData, theText );

  setCurrentBlockUserData( aData );

  foreach ( const HighlightingRule& rule, highlightingRules )
  {
    QRegExp expression( rule.pattern );
    int anIndex = expression.indexIn( theText );
    while ( anIndex >= 0 )
    {
      anIndex = expression.pos( rule.capture );
      int aLength = expression.cap( rule.capture ).length();
      setFormat( anIndex, aLength, rule.format );
      anIndex = expression.indexIn( theText, anIndex + aLength );
    }
  }

  setCurrentBlockState( NORMAL );

  if ( theText.indexOf( tripleQuotesExpression ) != -1 )
    return;

  QList<int> aTripleSingle;
  aTripleSingle << theText.indexOf( tripleSingleExpression ) << TRIPLESINGLE;

  QList<int> aTripleDouble;
  aTripleDouble << theText.indexOf( tripleDoubleExpression ) << TRIPLEDOUBLE;
 
  QList< QList<int> > aTripleExpressions;
  aTripleExpressions << aTripleSingle << aTripleDouble;

  for ( int i = 0; i < aTripleExpressions.length(); i++ )
  {
    QList<int> aBlock = aTripleExpressions[i];
    int anIndex = aBlock[0];
    int aState = aBlock[1];
    if ( previousBlockState() == aState )
    {
      if ( anIndex == -1 )
      {
        anIndex = theText.length();
        setCurrentBlockState( aState );
      }
      setFormat( 0, anIndex + 3, multiLineCommentFormat );
    }
    else if ( anIndex > -1 )
    {
      setCurrentBlockState( aState );
      setFormat( anIndex, theText.length(), multiLineCommentFormat );
    }
  }
}

void PyEditor_PyHighlighter::insertBracketsData( char theLeftSymbol,
                                                 char theRightSymbol,
                                                 TextBlockData* theData,
                                                 const QString& theText )
{
  int leftPosition = theText.indexOf( theLeftSymbol );
  while( leftPosition != -1 )
  {
    ParenthesisInfo* info = new ParenthesisInfo();
    info->character = theLeftSymbol;
    info->position = leftPosition;

    theData->insert( info );
    leftPosition = theText.indexOf( theLeftSymbol, leftPosition + 1 );
  }

  int rightPosition = theText.indexOf( theRightSymbol );
  while( rightPosition != -1 )
  {
    ParenthesisInfo* info = new ParenthesisInfo();
    info->character = theRightSymbol;
    info->position = rightPosition;

    theData->insert( info );
    rightPosition = theText.indexOf( theRightSymbol, rightPosition + 1 );
  }
}

void PyEditor_PyHighlighter::insertBracketsData( Brackets theBrackets,
                                                 TextBlockData* theData,
                                                 const QString& theText )
{
  char leftChar = '0';
  char rightChar = '0';
  
  switch( theBrackets )
  {
  case RoundBrackets:
    leftChar = '(';
    rightChar = ')';
    break;
  case CurlyBrackets:
    leftChar = '{';
    rightChar = '}';
    break;
  case SquareBrackets:
    leftChar = '[';
    rightChar = ']';
    break;
  }

  insertBracketsData( leftChar, rightChar, theData, theText );
}
