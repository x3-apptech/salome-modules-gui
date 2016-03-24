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

#ifndef SUIT_DESKTOP_H
#define SUIT_DESKTOP_H

#include "SUIT.h"

#include <QList>
#include <QEvent>

#include <QtxMainWindow.h>

class QMovie;

class QtxLogoMgr;
class QtxActionMenuMgr;
class QtxActionToolMgr;

class SUIT_ViewWindow;

/*!
  \class SUIT_Desktop
  Provides standard desktop: main window with
  main menu manager, toolbars manager and logo.
*/
class SUIT_EXPORT SUIT_Desktop : public QtxMainWindow
{
  Q_OBJECT

  class ReparentEvent;

  enum { Reparent = QEvent::User };

public:
  SUIT_Desktop();
  virtual ~SUIT_Desktop();

  QtxActionMenuMgr*        menuMgr() const;
  QtxActionToolMgr*        toolMgr() const;
  QtxLogoMgr*              logoMgr() const;

  virtual SUIT_ViewWindow* activeWindow() const = 0;
  virtual void setActiveWindow(SUIT_ViewWindow*);
  virtual QList<SUIT_ViewWindow*> windows() const = 0;

  int                      logoCount() const;

  void                     logoClear();
  void                     logoRemove( const QString& );
  void                     logoInsert( const QString&, QMovie*, const int = -1 );
  void                     logoInsert( const QString&, const QPixmap&, const int = -1 );

  void                     emitActivated();
  void                     emitMessage( const QString& );

signals:
  void                     activated();
  void                     deactivated();
//  void                     moved();
  void                     windowActivated( SUIT_ViewWindow* );
  void                     closing( SUIT_Desktop*, QCloseEvent* );
  void                     message( const QString& );

protected:
  virtual bool             event( QEvent* );
  virtual void             customEvent( QEvent* );
  virtual void             closeEvent( QCloseEvent* );
  virtual void             childEvent( QChildEvent* );

  virtual void             addWindow( QWidget* ) = 0;

private:
  QtxActionMenuMgr*        myMenuMgr;
  QtxActionToolMgr*        myToolMgr;
  QtxLogoMgr*              myLogoMgr;
};

#endif
