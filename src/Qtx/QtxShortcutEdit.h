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

#ifndef QTXSHORTCUTEDIT_H
#define QTXSHORTCUTEDIT_H

#include "Qtx.h"

#include <QFrame>
#include <QTreeWidget>

class QLineEdit;
class QPushButton;
class QTreeWidgetItem;

typedef QMap< QString, QString > ShortcutMap;

class QTX_EXPORT QtxShortcutEdit : public QFrame
{
  Q_OBJECT

public:
  QtxShortcutEdit( QWidget* = 0 );
  virtual ~QtxShortcutEdit();
  void           setShortcut( const QKeySequence& );
  QKeySequence   shortcut();
  static QString parseEvent( QKeyEvent* );
  static bool    isValidKey( int );


private slots:
  void           onCliked();
  void           onEditingFinished();

protected:
  virtual bool   eventFilter( QObject*, QEvent* );

private:
  void           initialize();

private:
  QLineEdit*     myShortcut;
  QString        myPrevShortcutText;
};

class QTX_EXPORT QtxShortcutTree : public QTreeWidget
{
  Q_OBJECT

public:
  QtxShortcutTree( QWidget * parent = 0 );
  virtual ~QtxShortcutTree();
  void                      setBindings( const QString&, const ShortcutMap& );
  ShortcutMap*              bindings( const QString& ) const;
  QStringList               sections() const;
  void                      setGeneralSections( const QStringList& );

protected:
  virtual bool              eventFilter( QObject*, QEvent* );
  virtual void              focusOutEvent( QFocusEvent* );
  virtual bool              checkUniqueness( QTreeWidgetItem*, const QString& );

private slots:
  void                      onCurrentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* );

private:
  QMap< QString, ShortcutMap > myPrevBindings;
  QStringList myGeneralSections;
};

#endif // QTXSHORTCUTEDIT_H
