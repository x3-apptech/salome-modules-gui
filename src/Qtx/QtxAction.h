// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// File:      QtxAction.h
// Author:    Sergey TELKOV
//
#ifndef QTXACTION_H
#define QTXACTION_H

#include "Qtx.h"

#include <QWidgetAction>

class QIcon;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class QTX_EXPORT QtxAction : public QWidgetAction
{
  Q_OBJECT

  class ActionNotify;

public:
  QtxAction( QObject* = 0, bool = false, const QString& = QString() );
  QtxAction( const QString&, const QString&, int, QObject*, bool = false, const QString& = QString() );
  QtxAction( const QString&, const QString&, const QKeySequence&, QObject*, bool = false, const QString& = QString() );
  QtxAction( const QString&, const QIcon&, const QString&, int, QObject*, bool = false, const QString& = QString() );
  QtxAction( const QString&, const QIcon&, const QString&, const QKeySequence&, QObject*, bool = false, const QString& = QString() );
  virtual ~QtxAction();

  virtual bool eventFilter( QObject*, QEvent* );

  QString shortcutActionName() const;
  void setShortcutActionName( const QString& );

protected:
  virtual void addedTo( QWidget* );
  virtual void removedFrom( QWidget* );

  virtual void customEvent( QEvent* );

private:
  QString myShortcutActionName;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
