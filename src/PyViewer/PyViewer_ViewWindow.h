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
// File   : PyViewer_ViewWindow.h
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#ifndef PYVIEWER_VIEWWINDOW_H
#define PYVIEWER_VIEWWINDOW_H

#include "PyViewer.h"

#include <SUIT_ViewWindow.h>

class PyEditor_Widget;

class PYVIEWER_EXPORT PyViewer_ViewWindow : public SUIT_ViewWindow
{
  Q_OBJECT

public:
  enum { NewId, OpenId, SaveId, SaveAsId,
         UndoId, RedoId, CutId, CopyId, PasteId, DeleteId, SelectAllId,
         FindId, ReplaceId,
         PreferencesId, HelpId };

  PyViewer_ViewWindow( SUIT_Desktop* = 0 );
  ~PyViewer_ViewWindow();

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
  PyEditor_Widget*  myEditor;
  QString           myURL;
};

#endif // PYVIEWER_VIEWWINDOW_H
