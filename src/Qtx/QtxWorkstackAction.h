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
// File:      QtxWorkstackAction.h
// Author:    Sergey TELKOV

#ifndef QTXWORKSTACKACTION_H
#define QTXWORKSTACKACTION_H

#include "QtxAction.h"

class QtxWorkstack;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxWorkstackAction : public QtxAction
{
  Q_OBJECT

public:
  enum { VSplit     = 0x0001,
         HSplit     = 0x0002,
         Windows    = 0x0010,
         Split      = VSplit | HSplit,
         Standard   = Split | Windows };

public:
  QtxWorkstackAction( QtxWorkstack*, QObject* = 0, const char* = 0 );
  virtual ~QtxWorkstackAction();

  QtxWorkstack* workstack() const;

  int           items() const;
  void          setItems( const int );
  bool          hasItems( const int ) const;

  int           accel( const int ) const;
  QIconSet      iconSet( const int ) const;
  QString       menuText( const int ) const;
  QString       statusTip( const int ) const;

  void          setAccel( const int, const int );
  void          setIconSet( const int, const QIconSet& );
  void          setMenuText( const int, const QString& );
  void          setStatusTip( const int, const QString& );

  virtual bool  addTo( QWidget* );
  virtual bool  addTo( QWidget*, const int );
  virtual bool  removeFrom( QWidget* );

  void          perform( const int );

private slots:
  void          onAboutToShow();
  void          onItemActivated( int );
  void          onPopupDestroyed( QObject* );

private:
  void          checkPopup( QPopupMenu* );
  void          updatePopup( QPopupMenu* );

  int           clearPopup( QPopupMenu* );
  void          fillPopup( QPopupMenu*, const int );

private:
  typedef QMap<QPopupMenu*, QIntList> MenuMap;
  typedef QMap<int, QtxAction*>       ItemMap;

private:
  MenuMap       myMenu;
  ItemMap       myItem;
  int           myFlags;
  QtxWorkstack* myWorkstack;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
