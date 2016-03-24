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

// File:      QtxActionMenuMgr.h
// Author:    Alexander SOLOVYOV, Sergey TELKOV
//
#ifndef QTXACTIONMENUMGR_H
#define QTXACTIONMENUMGR_H

#include "Qtx.h"
#include "QtxActionMgr.h"

#include <QList>

class QMenu;
class QMainWindow;
class QStringList;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxActionMenuMgr : public QtxActionMgr
{
  Q_OBJECT

  class MenuNode;

  typedef QList<MenuNode*> NodeList;  //!< menu nodes list

protected:
  class MenuCreator;

public:
  QtxActionMenuMgr( QMainWindow* );
  QtxActionMenuMgr( QWidget*, QObject* );
  virtual ~QtxActionMenuMgr();

  QWidget*     menuWidget() const;

  virtual bool isVisible( const int, const int ) const;
  virtual void setVisible( const int, const int, const bool );

  int          insert( const int, const QString&, const int, const int = -1 );
  int          insert( QAction*, const QString&, const int, const int = -1 );

  int          insert( const int, const QStringList&, const int, const int = -1 );
  int          insert( QAction*, const QStringList&, const int, const int = -1 );

  virtual int  insert( const int, const int, const int, const int = -1 );
  int          insert( QAction*, const int, const int, const int = -1 );

  int          insert( const QString&, const QString&, const int, const int = -1, const int = -1 );
  int          insert( const QString&, const QStringList&, const int, const int = -1, const int = -1 );
  virtual int  insert( const QString&, const int, const int, const int = -1, const int = -1 , QMenu* = 0);

  int          append( const int, const int, const int );
  int          append( QAction*, const int, const int );
  int          append( const QString&, const int, const int, const int = -1 );

  int          prepend( const int, const int, const int );
  int          prepend( QAction*, const int, const int );
  int          prepend( const QString&, const int, const int, const int = -1 );

  void         remove( const int );
  void         remove( const int, const int, const int = -1 );

  void         show( const int );
  void         hide( const int );

  bool         isShown( const int ) const;
  void         setShown( const int, const bool );

  virtual void change( const int, const QString& );

  virtual bool load( const QString&, QtxActionMgr::Reader& );

  bool         containsMenu( const QString&, const int, const bool = false ) const;
  bool         containsMenu( const int, const int, const bool = false ) const;

  QMenu*       findMenu( const int ) const;
  QMenu*       findMenu( const QString&, const int, const bool = false ) const;

  bool         isEmptyEnabled( const int ) const;
  void         setEmptyEnabled( const int, const bool );

private slots:
  void         onAboutToShow();
  void         onAboutToHide();
  void         onDestroyed( QObject* );

signals:
  void         menuAboutToShow( QMenu* );
  void         menuAboutToHide( QMenu* );

protected:
  void         setMenuWidget( QWidget* );

  MenuNode*    find( const int, const int, const bool = true ) const;
  MenuNode*    find( const int, MenuNode* = 0, const bool = true ) const;
  bool         find( const int, NodeList&, MenuNode* = 0 ) const;
  MenuNode*    find( const QString&, const int, const bool = true ) const;
  MenuNode*    find( const QString&, MenuNode* = 0, const bool = true ) const;
  bool         find( const QString&, NodeList&, MenuNode* = 0 ) const;
  int          findId( const int, const int = -1 ) const;

  void         removeMenu( const int, MenuNode* );

  QAction*     itemAction( const int ) const;
  QAction*     menuAction( const int ) const;
  int          menuActionId( QAction* ) const;

  void         updateMenu( MenuNode* = 0, const bool = true, const bool = true );
  virtual void internalUpdate();
  virtual void updateContent();

private:
  bool         ownAction( QAction*, MenuNode* ) const;
  bool         checkWidget( QWidget* ) const;
  QWidget*     menuWidget( MenuNode* ) const;
  void         simplifySeparators( QWidget* );
  QString      clearTitle( const QString& ) const;
  int          createMenu( const QStringList&, const int );

  void         triggerUpdate( const int, const bool rec = true );

private:
  typedef QMap<int, QAction*> MenuMap;     //!< actions map
  
private:
  MenuNode*       myRoot;        //!< root menu node
  QWidget*        myMenu;        //!< menu widget
  MenuMap         myMenus;       //!< actions map
  QMap<int, bool> myUpdateIds;   //!< list of actions ID being updated
};

class QtxActionMenuMgr::MenuCreator : public QtxActionMgr::Creator
{
public:
  MenuCreator( QtxActionMgr::Reader*, QtxActionMenuMgr* );
  virtual ~MenuCreator();

  virtual int append( const QString&, const bool,
                      const ItemAttributes&, const int );

private:
  QtxActionMenuMgr* myMgr;       //!< menu manager
};


#endif
