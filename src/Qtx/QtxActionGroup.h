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

// File:      QtxActionGroup.h
// Author:    Sergey TELKOV
//
#ifndef QTXACTIONGROUP_H
#define QTXACTIONGROUP_H

#include "QtxActionSet.h"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QtxComboBox;
class QActionGroup;

class QTX_EXPORT QtxActionGroup : public QtxActionSet
{
  Q_OBJECT

public:
  QtxActionGroup( QObject* = 0 );
  QtxActionGroup( QObject*, const bool );
  virtual ~QtxActionGroup();

  bool             isExclusive() const;
  bool             usesDropDown() const;

  void             add( QAction* );

  void             setEnabled( bool );

public slots:
  void             setExclusive( const bool );
  void             setUsesDropDown( const bool );

signals:
  void             selected( QAction* );

private slots:
  void             onActivated( int );
  void             onTriggered( QAction* );

protected:
  virtual void     updateAction( QWidget* );
  virtual void     updateAction( QtxComboBox* );

  virtual QWidget* createWidget( QWidget* );

  virtual bool     isEmptyAction() const;
  virtual void     actionAdded( QAction* );
  virtual void     actionRemoved( QAction* );

private:
  void             updateType();
  QtxComboBox*     createdWidget( QWidget* );

private:
  bool             myDropDown;
  QActionGroup*    myActionGroup;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
