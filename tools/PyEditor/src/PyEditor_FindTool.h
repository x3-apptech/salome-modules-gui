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
// File   : PyEditor_FindTool.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef PYEDITOR_FINDTOOL_H
#define PYEDITOR_FINDTOOL_H

#include "PyEditor.h"

#include <QTextDocument>
#include <QStringListModel>
#include <QWidget>

class PyEditor_Editor;
class QAction;
class QLabel;
class QLineEdit;
class QMenu;

class PYEDITOR_EXPORT PyEditor_FindTool : public QWidget
{
Q_OBJECT

  enum { CaseSensitive, WholeWord, RegExp, Find, FindPrevious, FindNext, Replace };

public:
  PyEditor_FindTool( PyEditor_Editor*, QWidget* = 0 );

  bool event( QEvent* );
  bool eventFilter( QObject*, QEvent* );

public slots:
  void activateFind();
  void activateReplace();

private slots:
  void showMenu();
  void find( const QString& );
  void find();
  void findPrevious();
  void findNext();
  void replace();
  void replaceAll();
  void update();
  void activate( int );
  void customizeMenu( QMenu* );

private:
  QList<QKeySequence> shortcuts( int ) const;
  void updateShortcuts();

  void showReplaceControls( bool );
  void setSearchResult( bool );

  bool isRegExp() const;
  bool isCaseSensitive() const;
  bool isWholeWord() const;
  QTextDocument::FindFlags searchFlags( bool = false ) const;

  QList<QTextCursor> matches( const QString& ) const;
  void find( const QString&, int );

  void addCompletion( const QString&, bool );

private:
  PyEditor_Editor* myEditor;
  QLineEdit* myFindEdit;
  QLineEdit* myReplaceEdit;
  QLabel* myInfoLabel;
  QStringListModel myFindCompletion, myReplaceCompletion;
};

#endif // PYEDITOR_FINDTOOL_H
