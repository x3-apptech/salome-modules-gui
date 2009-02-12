//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : LightApp_ModuleAction.h
// Author : Sergey TELKOV, Vadim SANDLER
//
#ifndef LIGHTAPP_MODULEACTION_H
#define LIGHTAPP_MODULEACTION_H

#include "LightApp.h"

#include <QtxAction.h>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class QtxComboBox;

class LIGHTAPP_EXPORT LightApp_ModuleAction : public QtxAction
{
  Q_OBJECT

private:
  class ActionSet;
  class ComboAction;

public:
  enum { None = 0x00, Buttons = 0x01, ComboItem = 0x02, All = Buttons | ComboItem };

public:
  LightApp_ModuleAction( const QString&, QObject* = 0 );
  LightApp_ModuleAction( const QString&, const QIcon&, QObject* = 0 );
  virtual ~LightApp_ModuleAction();

  QStringList      modules() const;

  QIcon            moduleIcon( const QString& ) const;
  void             setModuleIcon( const QString&, const QIcon& );

  void             insertModule( const QString&, const QIcon&, const int = -1 );
  void             removeModule( const QString& );

  QString          activeModule() const;
  void             setActiveModule( const QString& );

  void             setMode( const int );
  int              mode() const;

protected:
  virtual void     addedTo( QWidget* );
  virtual void     removedFrom( QWidget* );

signals:
  void             moduleActivated( const QString& );

private:
  void             init();

  void             update();
  void             update( QtxComboBox* );

  QAction*         active() const;
  void             activate( int, bool = true );

private slots:
  void             onChanged();
  void             onTriggered( int );
  void             onComboActivated( int );

private:
  ComboAction*     myCombo;
  ActionSet*       mySet;
  int              myMode;
};

class LightApp_ModuleAction::ComboAction : public QtxAction
{
  Q_OBJECT

public:
  ComboAction( QObject* parent );

  QList<QtxComboBox*> widgets() const;

protected:
  virtual QWidget*    createWidget( QWidget* );

signals:
  void                activatedId( int );
};

#endif // LIGHTAPP_MODULEACTION_H
