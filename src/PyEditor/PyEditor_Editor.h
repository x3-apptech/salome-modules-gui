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
// File   : PyEditor_Editor.h
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#ifndef PYEDITOR_EDITOR_H
#define PYEDITOR_EDITOR_H

#include <QPlainTextEdit>
#include "PyEditor.h"

class PyEditor_PyHighlighter;
class PyEditor_Settings;
class QtxResourceMgr;

class PYEDITOR_EXPORT PyEditor_Editor : public QPlainTextEdit
{
  Q_OBJECT

public:
  PyEditor_Editor( bool isSingle = false, QtxResourceMgr* = 0,  QWidget* = 0 );
  virtual ~PyEditor_Editor();
  
  void lineNumberAreaPaintEvent( QPaintEvent* );
  int  lineNumberAreaWidth();

  void updateStatement();
  PyEditor_Settings* settings();

public Q_SLOTS:
  void deleteSelected();
  void append ( const QString & );  
  void setText ( const QString & text );
protected:
  virtual void keyPressEvent( QKeyEvent* );
  virtual void resizeEvent( QResizeEvent* );
  virtual void paintEvent( QPaintEvent* );
    
private Q_SLOTS:
  void updateHighlightCurrentLine();
  void matchParentheses();

  void updateLineNumberAreaWidth( int );
  void updateLineNumberArea( const QRect&, int );
  
private:
  bool matchLeftParenthesis( const QTextBlock&, int, int );
  bool matchRightParenthesis( const QTextBlock&, int, int );
  void createParenthesisSelection( int );
  bool isLeftBrackets( QChar );
  bool isRightBrackets( QChar );
  
  void handleHome( bool );
  int  lineIndent();
  void tabIndentation( bool );
  void indentSelection( bool );
  
  int findFirstNonSpace( const QString& );
  
  QWidget*                my_LineNumberArea;
  PyEditor_PyHighlighter* my_SyntaxHighlighter;
  PyEditor_Settings*      my_Settings;
};

#endif // PYEDITOR_EDITOR_H
