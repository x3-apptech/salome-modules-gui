// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
// File:      QtxMainWindow.h
// Author:    Sergey TELKOV

#ifndef QTXMAINWINDOW_H
#define QTXMAINWINDOW_H

#include "Qtx.h"

#include <qmainwindow.h>

class QDockWindow;
class QtxResourceMgr;

class QTX_EXPORT QtxMainWindow : public QMainWindow
{
  Q_OBJECT

  class Filter;

  enum { WS_Normal, WS_Minimized, WS_Maximized, WS_Hidden };
  enum { WP_Absolute, WP_Center, WP_Left, WP_Right, WP_Top = WP_Left, WP_Bottom = WP_Right };

public:
  QtxMainWindow( QWidget* = 0, const char* = 0, WFlags = WType_TopLevel );
  virtual ~QtxMainWindow();

  bool              isDockableMenuBar() const;
  void              setDockableMenuBar( const bool );

  bool              isDockableStatusBar() const;
  void              setDockableStatusBar( const bool );

  void              loadGeometry( QtxResourceMgr*, const QString& );
  void              saveGeometry( QtxResourceMgr*, const QString& ) const;

  virtual bool      eventFilter( QObject*, QEvent* );

public slots:
  virtual void      show();
  virtual void      setAppropriate( QDockWindow*, bool );

protected:
  virtual void      setUpLayout();
  virtual void      customEvent( QCustomEvent* );

private slots:
  void              onDestroyed( QObject* );

private:
  int               windowState( const QString& ) const;
  int               windowPosition( const QString& ) const;
  int               relativeCoordinate( const int, const int, const int ) const;

private:
  int               myMode;
  QDockWindow*      myMenuBar;
  QDockWindow*      myStatusBar;
};

#endif
