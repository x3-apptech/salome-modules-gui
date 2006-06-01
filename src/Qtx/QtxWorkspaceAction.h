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
// File:      QtxWorkspaceAction.h
// Author:    Sergey TELKOV

#ifndef QTXWORKSPACEACTION_H
#define QTXWORKSPACEACTION_H

#include "QtxAction.h"

class QWorkspace;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxWorkspaceAction : public QtxAction
{
  Q_OBJECT

public:
  enum { Cascade    = 0x0001,
         Tile       = 0x0002,
         VTile      = 0x0004,
         HTile      = 0x0008,
         Windows    = 0x0010,
         Standard   = Cascade | Tile | Windows,
         Operations = Cascade | Tile | VTile | HTile,
         All        = Standard | HTile | VTile };

public:
  QtxWorkspaceAction( QWorkspace*, QObject* = 0, const char* = 0 );
  virtual ~QtxWorkspaceAction();

  QWorkspace*  workspace() const;

  int          items() const;
  void         setItems( const int );
  bool         hasItems( const int ) const;

  int          accel( const int ) const;
  QIconSet     iconSet( const int ) const;
  QString      menuText( const int ) const;
  QString      statusTip( const int ) const;

  void         setAccel( const int, const int );
  void         setIconSet( const int, const QIconSet& );
  void         setMenuText( const int, const QString& );
  void         setStatusTip( const int, const QString& );

  virtual bool addTo( QWidget* );
  virtual bool addTo( QWidget*, const int );
  virtual bool removeFrom( QWidget* );

  void         perform( const int );

public slots:
  void         tile();
  void         cascade();
  void         tileVertical();
  void         tileHorizontal();

private slots:
  void         onAboutToShow();
  void         onItemActivated( int );
  void         onPopupDestroyed( QObject* );

private:
  void         checkPopup( QPopupMenu* );
  void         updatePopup( QPopupMenu* );

  int          clearPopup( QPopupMenu* );
  void         fillPopup( QPopupMenu*, const int );

private:
  typedef QMap<QPopupMenu*, QIntList> MenuMap;
  typedef QMap<int, QtxAction*>       ItemMap;

private:
  MenuMap      myMenu;
  ItemMap      myItem;
  int          myFlags;
  QWorkspace*  myWorkspace;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
