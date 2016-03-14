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

// File:      QtxActionSet.h
// Author:    Sergey TELKOV
//
#ifndef QTXACTIONSET_H
#define QTXACTIONSET_H

#include "QtxAction.h"

#include <QList>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxActionSet : public QtxAction
{
  Q_OBJECT

public:
  QtxActionSet( QObject* = 0 );
  virtual ~QtxActionSet();

  QList<QAction*> actions() const;
  void            setActions( const QList<QAction*>& );

  void            insertActions( const QList<QAction*>&, const int = -1 );

  int             insertAction( QAction*, const int id = -1, const int = -1 );
  int             insertAction( const QString&, const int id = -1, const int = -1 );
  int             insertAction( const QString&, const QIcon&, const int id = -1, const int = -1 );

  void            removeAction( QAction* );
  void            removeAction( const int );

  void            clear();

signals:
  void            triggered( int );
  void            triggered( QAction* );

private slots:
  void            onChanged();
  void            onActionTriggered( bool = false );

protected:
  virtual void    addedTo( QWidget* );
  virtual void    removedFrom( QWidget* );

  virtual void    actionAdded( QAction* );
  virtual void    actionRemoved( QAction* );

  QAction*        action( int ) const;
  int             actionId( QAction* ) const;
  void            setActionId( QAction*, const int );

  virtual bool    isEmptyAction() const;

  virtual void    updateAction();
  virtual void    updateAction( QWidget* );

private:
  int             generateId() const;

private:
  typedef QList<QAction*> ActionList;

private:
  ActionList      mySet;      //!< actions list
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
