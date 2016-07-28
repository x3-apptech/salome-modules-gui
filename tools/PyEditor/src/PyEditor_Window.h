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
// File   : PyEditor_Window.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef PYEDITOR_WINDOW_H
#define PYEDITOR_WINDOW_H

#include "PyEditor.h"

#include <QMainWindow>
#include <QMap>

class QAction;
class PyEditor_Editor;

class PYEDITOR_EXPORT PyEditor_Window : public QMainWindow
{
  Q_OBJECT

public:
  enum { NewId, OpenId, SaveId, SaveAsId, ExitId,
         UndoId, RedoId, CutId, CopyId, PasteId, DeleteId, SelectAllId,
         PreferencesId, HelpId };

  PyEditor_Window( QWidget* = 0 );
  ~PyEditor_Window();

protected:
  virtual void closeEvent( QCloseEvent* );

private Q_SLOTS:
  void        onNew();
  void        onOpen();
  bool        onSave();
  bool        onSaveAs();
  void        onPreferences();
  void        onHelp();

private:
  void        loadFile( const QString& );
  bool        saveFile( const QString& );
  
  void        setCurrentFile( const QString& );
  bool        whetherSave();
  QString     defaultName() const;

private:
  PyEditor_Editor*    myTextEditor;
  QString             myURL;
  QMap<int, QAction*> myActions;
};

#endif // PYEDITOR_WINDOW_H
