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
// File   : PyEditor_PyHighlighter.cxx
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#include "PyEditor_PyHighlighter.h"

#include "PyEditor_Keywords.h"

#include <QSet>

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
PyEditor_PyHighlighter::PyEditor_PyHighlighter( QTextDocument* theDocument,
						PyEditor_Keywords* std,
						PyEditor_Keywords* user )
  : QSyntaxHighlighter( theDocument ),
    myStdKeywords( std ),
    myUserKeywords( user )
{

  connect(myStdKeywords, SIGNAL( keywordsChanged() ),
	  this, SLOT( onKeywordsChanged() ) );
  connect(myUserKeywords, SIGNAL( keywordsChanged() ),
	  this, SLOT( onKeywordsChanged() ) );

  updateHighlight();
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

void PyEditor_PyHighlighter::onKeywordsChanged()
{
  updateHighlight();
  rehighlight();
}

void PyEditor_PyHighlighter::updateHighlight()
{
  highlightingRules.clear();

  HighlightingRule aRule;

  QList<PyEditor_Keywords*> dictList;
  dictList << myStdKeywords << myUserKeywords;

  // Keywords
  QSet<QString> existing;
  for ( QList<PyEditor_Keywords*>::const_iterator it = dictList.begin();
	it != dictList.end(); ++it ) {
    PyEditor_Keywords* kwDict = *it;
    QList<QColor> colors = kwDict->colors();
    for ( QList<QColor>::const_iterator itr = colors.begin(); itr != colors.end(); ++itr ) {
      QColor color = *itr;
      QTextCharFormat format;
      format.setForeground( color );
      QStringList keywords = kwDict->keywords( color );
      foreach ( const QString& keyword, keywords ) {
	if ( existing.contains( keyword ) )
	  continue;

	aRule.pattern = QRegExp( QString( "\\b%1\\b" ).arg( keyword ) );
	aRule.format = format;
	aRule.capture = 0;
	highlightingRules.append( aRule );
	existing.insert( keyword );
      }
    }
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
