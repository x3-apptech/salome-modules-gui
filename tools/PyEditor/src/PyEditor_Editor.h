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
// File   : PyEditor_Editor.h
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#ifndef PYEDITOR_EDITOR_H
#define PYEDITOR_EDITOR_H

#include "PyEditor.h"
#include "PyEditor_Settings.h"

#include <QPlainTextEdit>

class PyEditor_Keywords;
class PyEditor_Completer;
class PyEditor_PyHighlighter;
class QMenu;

class PYEDITOR_EXPORT PyEditor_Editor : public QPlainTextEdit
{
  Q_OBJECT

public:
  typedef enum { None, Auto, Manual, Always } CompletionPolicy;

public:
  PyEditor_Editor( QWidget* = 0 );
  virtual ~PyEditor_Editor();
  
  void    setSettings( const PyEditor_Settings& );
  const PyEditor_Settings& settings() const;

  QString text() const;

  QStringList keywords() const;
  void        appendKeywords( const QStringList&, int, const QColor& = QColor() );
  void        removeKeywords( const QStringList& );

  CompletionPolicy completionPolicy() const;
  void             setCompletionPolicy( const CompletionPolicy& );

public Q_SLOTS:
  void deleteSelected();
  void append( const QString& );  
  void setText( const QString& text );
  void setCurrentLine( int );

protected:
  virtual void keyPressEvent( QKeyEvent* );
  virtual void resizeEvent( QResizeEvent* );
  virtual void paintEvent( QPaintEvent* );
  virtual void contextMenuEvent( QContextMenuEvent* );

  PyEditor_Keywords* userKeywords() const;
  PyEditor_Keywords* standardKeywords() const;

private Q_SLOTS:
  void updateHighlightCurrentLine();
  void matchParentheses();

  void updateLineNumberAreaWidth( int );
  void updateLineNumberArea( const QRect&, int );
  
Q_SIGNALS:
  void customizeMenu( QMenu* );

private:
  bool matchLeftParenthesis( const QTextBlock&, int, int );
  bool matchRightParenthesis( const QTextBlock&, int, int );
  void createParenthesisSelection( int );
  bool isLeftBrackets( QChar );
  bool isRightBrackets( QChar );
  void lineNumberAreaPaintEvent( QPaintEvent* );
  int  lineNumberAreaWidth();

  void handleHome( bool );
  int  lineIndent();
  void tabIndentation( bool );
  void indentSelection( bool );

  int findFirstNonSpace( const QString& );

  QWidget*                myLineNumberArea;
  PyEditor_PyHighlighter* mySyntaxHighlighter;
  PyEditor_Completer*     myCompleter;
  PyEditor_Settings       mySettings;

  PyEditor_Keywords*      myStdKeywords;
  PyEditor_Keywords*      myUserKeywords;

  CompletionPolicy        myCompletionPolicy;

  friend class PyEditor_LineNumberArea;
};

#endif // PYEDITOR_EDITOR_H
