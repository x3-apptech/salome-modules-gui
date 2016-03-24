// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxPathListEdit.h
// Author:    Sergey TELKOV
//
#ifndef QTXPATHLISTEDIT_H
#define QTXPATHLISTEDIT_H

#include "Qtx.h"

#include <QFrame>
#include <QPointer>

class QLineEdit;
class QListView;
class QCompleter;
class QModelIndex;
class QToolButton;
class QStringListModel;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxPathListEdit : public QFrame
{
  Q_OBJECT

  class Editor;
  class Delegate;

public:
  QtxPathListEdit( const Qtx::PathType, QWidget* = 0 );
  QtxPathListEdit( QWidget* = 0 );
  virtual ~QtxPathListEdit();

  Qtx::PathType     pathType() const;
  void              setPathType( const Qtx::PathType );

  QStringList       pathList() const;
  void              setPathList( const QStringList& );

  bool              isDuplicateEnabled() const;
  void              setDuplicateEnabled( const bool );

  int               count() const;
  bool              contains( const QString& ) const;

  void              clear();
  void              remove( const int );
  void              remove( const QString& );
  void              insert( const QString&, const int = -1 );

  bool              eventFilter( QObject*, QEvent* );

protected slots:
  void              onUp( bool = false );
  void              onDown( bool = false );
  void              onInsert( bool = false );
  void              onDelete( bool = false );

private:
  void              initialize();
  QWidget*          createEditor( QWidget* );
  void              setModelData( QWidget*, const QModelIndex& );
  void              setEditorData( QWidget*, const QModelIndex& );

  bool              checkExistance( const QString&, const bool = true );
  bool              checkDuplicate( const QString&, const int, const bool = true );

private:
  QListView*        myList;
  Qtx::PathType     myType;
  QStringListModel* myModel;
  QCompleter*       myCompleter;
  bool              myDuplicate;

  friend class QtxPathListEdit::Delegate;
};

#endif
