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
// File   : PyEditor_PyHighlighter.h
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#ifndef PYEDITOR_PYHIGHLIGHTER_H
#define PYEDITOR_PYHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class QTextDocument;

class PyEditor_PyHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:

  struct ParenthesisInfo
  {
    char character;
    int position;
  };

  class TextBlockData : public QTextBlockUserData
  {
  public:
    TextBlockData();

    QVector<ParenthesisInfo*> parentheses();
    void insert( ParenthesisInfo* );

  private:
    QVector<ParenthesisInfo*> myParentheses;
  };

public:
  PyEditor_PyHighlighter( QTextDocument* = 0 );

  void initialize();
  QStringList keywords();
  QStringList specialKeywords();

protected:
  struct HighlightingRule
  {
    QRegExp pattern;
    QTextCharFormat format;
    int capture;
  };
  QVector<HighlightingRule> highlightingRules;

  enum Brackets { RoundBrackets, CurlyBrackets, SquareBrackets };

  QRegExp tripleQuotesExpression;
  QRegExp tripleSingleExpression;
  QRegExp tripleDoubleExpression;

  QTextCharFormat classFormat;
  QTextCharFormat referenceClassFormat;
  QTextCharFormat functionFormat;
  QTextCharFormat keywordFormat;
  QTextCharFormat specialFromat;
  QTextCharFormat numberFormat;
  QTextCharFormat singleLineCommentFormat;
  QTextCharFormat multiLineCommentFormat;
  QTextCharFormat quotationFormat;

  void highlightBlock( const QString& );
  void insertBracketsData( char, char, TextBlockData*, const QString& );
  void insertBracketsData( Brackets, TextBlockData*, const QString& );
};

#endif // PYEDITOR_PYHIGHLIGHTER_H
