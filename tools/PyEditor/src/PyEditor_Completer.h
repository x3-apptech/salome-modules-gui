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
// File   : PyEditor_Completer.h
// Author : Sergey TELKOV, Open CASCADE S.A.S. (sergey.telkov@opencascade.com)
//

#ifndef PYEDITOR_COMPLETER_H
#define PYEDITOR_COMPLETER_H

#include <QCompleter>

class QTimer;
class PyEditor_Editor;
class PyEditor_Keywords;

class PyEditor_Completer : public QCompleter
{
  Q_OBJECT

public:
  PyEditor_Completer( PyEditor_Editor*,
		      PyEditor_Keywords*, PyEditor_Keywords* );
  virtual ~PyEditor_Completer();

  void               perform();

  void               uncomplete();

  virtual bool       eventFilter(QObject*, QEvent*);

private Q_SLOTS:
  void               onTimeout();
  void               onTextChanged();
  void               onKeywordsChanged();
  void               onActivated( const QString& );

protected:
  QRect              completionRect() const;
  QString            completionText() const;
  QPoint             completionRange() const;

private:
  typedef QMap<QString, QColor> KeywordMap;

private:
  KeywordMap         keywords() const;
  void               updateKeywords();
  void               triggerComplete();

private:
  PyEditor_Editor*   myEditor;
  QTimer*            myTimer;
  PyEditor_Keywords* myStdKeywords;
  PyEditor_Keywords* myUserKeywords;
};

#endif
