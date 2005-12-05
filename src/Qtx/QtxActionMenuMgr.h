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
// See http://www.salome-platform.org/
//
// File:      QtxActionMenuMgr.h
// Author:    Alexander SOLOVYEV, Sergey TELKOV

#ifndef QTXACTIONMENUMGR_H
#define QTXACTIONMENUMGR_H

#include "Qtx.h"
#include "QtxActionMgr.h"

#include <qptrlist.h>
#include <qstringlist.h>

class QPopupMenu;
class QMainWindow;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxActionMenuMgr : public QtxActionMgr
{
  Q_OBJECT

  class MenuNode;

  typedef QPtrList<MenuNode>         NodeList;
  typedef QPtrListIterator<MenuNode> NodeListIterator;

  class MenuNode
  {
  public:
    MenuNode() : parent( 0 ), visible( true ) { children.setAutoDelete( true ); };
    MenuNode( MenuNode* p ) : parent( p ), visible( true ) { children.setAutoDelete( true ); };

    int       id;
    int       group;
    MenuNode* parent;
    bool      visible;
    NodeList  children;
  };

  class MenuAction;

protected:
  class MenuCreator;

public:
  QtxActionMenuMgr( QMainWindow* );
  QtxActionMenuMgr( QWidget*, QObject* );
  virtual ~QtxActionMenuMgr();

  virtual bool isVisible( const int, const int ) const;
  virtual void setVisible( const int, const int, const bool );

  int          insert( const int, const QString&, const int, const int = -1 );
  int          insert( QAction*, const QString&, const int, const int = -1 );

  int          insert( const int, const QStringList&, const int, const int = -1 );
  int          insert( QAction*, const QStringList&, const int, const int = -1 );

  virtual int  insert( const int, const int, const int, const int = -1 );
  int          insert( QAction*, const int, const int, const int = -1 );

  int          insert( const QString&, const QString&, const int, const int = -1 );
  int          insert( const QString&, const QStringList&, const int, const int = -1 );
  virtual int  insert( const QString&, const int, const int, const int = -1 );

  int          append( const int, const int, const int );
  int          append( QAction*, const int, const int );
  int          append( const QString&, const int, const int );

  int          prepend( const int, const int, const int );
  int          prepend( QAction*, const int, const int );
  int          prepend( const QString&, const int, const int );

  void         remove( const int );
  void         remove( const int, const int, const int = -1 );

  void         show( const int );
  void         hide( const int );

  bool         isShown( const int ) const;
  void         setShown( const int, const bool );

  virtual bool load( const QString&, QtxActionMgr::Reader& );

private slots:
  void         onDestroyed( QObject* );

protected:
  void         setWidget( QWidget* );
  MenuNode*    find( const int, const int ) const;
  MenuNode*    find( const int, MenuNode* = 0 ) const;
  bool         find( const int, NodeList&, MenuNode* = 0 ) const;

  void         removeMenu( const int, MenuNode* );

  QAction*     itemAction( const int ) const;
  MenuAction*  menuAction( const int ) const;

  void         updateMenu( MenuNode* = 0, const bool = true, const bool = true );
  virtual void internalUpdate();  

private:
  bool         checkWidget( QWidget* ) const;
  QWidget*     menuWidget( MenuNode* ) const;
  void         simplifySeparators( QWidget* );
  QString      clearTitle( const QString& ) const;
  int          createMenu( const QStringList&, const int );

private:
  typedef QMap<int, MenuAction*> MenuMap;

private:
  MenuNode     myRoot;
  QWidget*     myMenu;
  MenuMap      myMenus;
};

class QtxActionMenuMgr::MenuCreator : public QtxActionMgr::Creator
{
public:
  MenuCreator( QtxActionMgr::Reader*, QtxActionMenuMgr* );
  virtual ~MenuCreator();

  virtual int append( const QString&, const bool,
                      const ItemAttributes&, const int );

private:
  QtxActionMenuMgr* myMgr;
};


#endif
