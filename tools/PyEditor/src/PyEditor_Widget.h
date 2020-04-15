// Copyright (C) 2015-2020  OPEN CASCADE
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
// File   : PyEditor_Widget.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef PYEDITOR_WIDGET_H
#define PYEDITOR_WIDGET_H

#include "PyEditor.h"
#include "PyEditor_Settings.h"

#include <QWidget>

class PyEditor_Editor;
class PyEditor_FindTool;

class PYEDITOR_EXPORT PyEditor_Widget : public QWidget
{
  Q_OBJECT

public:
  PyEditor_Widget( QWidget* = 0 );

  PyEditor_Editor* editor();
  PyEditor_FindTool* findTool();

  void setSettings( const PyEditor_Settings& );
  const PyEditor_Settings& settings() const;

  bool isModified();

  QString text() const;

  QStringList keywords() const;
  void appendKeywords( const QStringList&, int, const QColor& = QColor() );
  void removeKeywords( const QStringList& );

  int completionPolicy() const;
  void setCompletionPolicy( int );

public slots:
  void find();
  void replace();

  void undo();
  void redo();
  void cut();
  void copy();
  void paste();
  void deleteSelected();
  void selectAll();
  void clear();

  void setModified( bool );

  void setText( const QString& );

  void setCurrentLine( int );

signals:
  void modificationChanged( bool );
  void undoAvailable( bool );
  void redoAvailable( bool );
  void copyAvailable( bool );
  void selectionChanged();
  void textChanged();
  void cursorPositionChanged();

private:
  PyEditor_Editor* myEditor;
  PyEditor_FindTool* myFindTool;
};

#endif // PYEDITOR_WIDGET_H
