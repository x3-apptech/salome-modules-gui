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

// File:      QtxMultiAction.h
// Author:    Sergey TELKOV
//
#ifndef QTXMULTIACTION_H
#define QTXMULTIACTION_H

#include "QtxActionSet.h"

class QToolButton;

class QTX_EXPORT QtxMultiAction : public QtxActionSet
{
  Q_OBJECT

  class Menu;
  class Button;
  class Filter;

public:
  QtxMultiAction( QObject* parent = 0 );
  QtxMultiAction( const QString&, QObject* parent = 0 );
  QtxMultiAction( const QIcon&, const QString&, QObject* parent = 0 );
  virtual ~QtxMultiAction();

  void             setActiveAction( QAction* );
  QAction*         activeAction() const;

private slots:
  void             onClicked( bool );
  void             onTriggered( QAction* );
  void             onActionChanged();

protected:
  virtual bool     isEmptyAction() const;
  virtual QWidget* createWidget( QWidget* );

  virtual void     updateAction();
  virtual void     updateAction( QWidget* );

  virtual void     actionAdded( QAction* );
  virtual void     actionRemoved( QAction* );

private:
  void             updateButton( QToolButton* );

private:
  QAction*         myCurrent;
};

#endif // QTXMULTIACTION_H
