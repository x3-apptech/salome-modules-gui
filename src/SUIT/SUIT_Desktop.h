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
#ifndef SUIT_DESKTOP_H
#define SUIT_DESKTOP_H

#include "SUIT.h"

#include <qmap.h>
#include <qpixmap.h>
#include <QtxMainWindow.h>

class QPopupMenu;
class QtxLogoMgr;
class SUIT_ViewWindow;
class QtxActionMenuMgr;
class QtxActionToolMgr;

/*!
  \class SUIT_Desktop
  Provides standard desktop: main window with
  main menu manager, toolbars manager and logo.
*/
class SUIT_EXPORT SUIT_Desktop : public QtxMainWindow
{
  Q_OBJECT

public:
  SUIT_Desktop();
  virtual ~SUIT_Desktop();

  QtxActionMenuMgr*        menuMgr() const;
  QtxActionToolMgr*        toolMgr() const;

  virtual SUIT_ViewWindow* activeWindow() const = 0;
  virtual QPtrList<SUIT_ViewWindow> windows() const = 0;

  void                     addLogo( const QString&, const QPixmap& ); // Not should be used. Will be removed.
  void                     removeLogo( const QString& );              // Not should be used. Will be removed.

  int                      logoCount() const;

  void                     logoClear();
  void                     logoRemove( const QString& );
  void                     logoInsert( const QString&, const QPixmap&, const int = -1 );

  void                     emitActivated();
  void                     emitMessage( const QString& );

signals:
  void                     activated();
  void                     deactivated();
  void                     windowActivated( SUIT_ViewWindow* );
  void                     closing( SUIT_Desktop*, QCloseEvent* );
  void                     message( const QString& );

protected:
  virtual bool             event( QEvent* );
  virtual void             closeEvent( QCloseEvent* );
  virtual void             childEvent( QChildEvent* );

  virtual QWidget*         parentArea() const = 0;

private:
  QtxActionMenuMgr*        myMenuMgr;
  QtxActionToolMgr*        myToolMgr;
  QtxLogoMgr*              myLogoMgr;
};

#endif
