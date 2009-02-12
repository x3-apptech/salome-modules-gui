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
// File:      QtxMainWindow.h
// Author:    Sergey TELKOV
//
#ifndef QTXMAINWINDOW_H
#define QTXMAINWINDOW_H

#include "Qtx.h"

#include <QMainWindow>

class QtxResourceMgr;

class QTX_EXPORT QtxMainWindow : public QMainWindow
{
  Q_OBJECT

  class Filter;

public:
  QtxMainWindow( QWidget* = 0, Qt::WindowFlags = 0 );
  virtual ~QtxMainWindow();

  bool              isDockableMenuBar() const;
  void              setDockableMenuBar( const bool );

  bool              isDockableStatusBar() const;
  void              setDockableStatusBar( const bool );

  QString           storeGeometry() const;
  void              retrieveGeometry( const QString& );

private slots:
  void              onDestroyed( QObject* );

private:
  int               geometryValue( const QString&, int&, bool& ) const;

private:
  QToolBar*         myMenuBar;       //!< dockable menu bar
  QToolBar*         myStatusBar;     //!< dockable status bar
};

#endif // QTXMAINWINDOW_H
