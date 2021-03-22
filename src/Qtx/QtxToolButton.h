// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef QTXTOOLBUTTON_H
#define QTXTOOLBUTTON_H

#include "Qtx.h"

#include <QStringList>
#include <QToolButton>
#include <QVariant>

class QTX_EXPORT QtxToolButton : public QToolButton
{
  Q_OBJECT

  enum Move { NoMove=0, MoveUp, MoveDown, MoveFirst, MoveLast };

public:
  QtxToolButton( QWidget* = 0 );
  virtual ~QtxToolButton();

  int addItem( const QString&, const QVariant& = QVariant() );
  int addItem( const QIcon&, const QString&, const QVariant& = QVariant() );
  void addItems( const QStringList& );

  int addSeparator( const QString& = QString() );
  int addSeparator( const QIcon&, const QString& );

  void removeItem( int );

  int count() const;

  QVariant currentData() const;
  int currentIndex() const;
  QString currentText() const;

  QVariant itemData( int ) const;
  QIcon itemIcon( int ) const;
  QString itemText( int ) const;

  void setItemData( int, const QVariant& );
  void setItemIcon( int, const QIcon& );
  void setItemText( int, const QString& );

  int findText( const QString& );

public slots:
  void clear();

  void setCurrentIndex( int );
  void setCurrentText( const QString& );

protected:
  void keyPressEvent( QKeyEvent* );
  void wheelEvent( QWheelEvent* );

signals:
  void activated( int );
  void activated( const QString& );
  void currentIndexChanged( int );
  void currentIndexChanged( const QString& );
  void currentTextChanged( const QString& );

private slots:
  void actionTriggered( QAction* );

private:
  void internalUpdate();
  QAction* actionAt( int ) const;
  void moveIndex( Move );
  void emitCurrentChanged( bool, bool );
};

#endif // QTXTOOLBUTTON_H
