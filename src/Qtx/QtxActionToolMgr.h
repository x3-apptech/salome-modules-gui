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
// File:      QtxActionToolMgr.h
// Author:    Alexander SOLOVYEV, Sergey TELKOV

#ifndef QTXACTIONTOOLMGR_H
#define QTXACTIONTOOLMGR_H

#include "Qtx.h"

#include <qaction.h>

#include "QtxActionMgr.h"

class QToolBar;
class QMainWindow;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
  \class QtxActionToolMgr
  Allows to use set of action to automatically build set of toolbars.
  With help of methods insert/append/remove it is possible to 
  describe toolbars and its internal structure.
  This manager is able to attune toolbar by removing excess separators
*/
class QTX_EXPORT QtxActionToolMgr : public QtxActionMgr
{
  Q_OBJECT

  /*!
    \class ToolNode
    Represents a toolbutton inside toolbar
    For internal purposes only
  */
  class ToolNode
  {
  public:
    ToolNode() : id( -1 ), visible( true ) {};

    int       id;
    bool      visible;
  };

  typedef QValueList<ToolNode> NodeList;

protected:
  class ToolCreator;

public:
  QtxActionToolMgr( QMainWindow* );
  virtual ~QtxActionToolMgr();

  QMainWindow*    mainWindow() const;

  int             createToolBar( const QString&, int = -1 );
  void            removeToolBar( const QString& );
  void            removeToolBar( const int );

  int             insert( const int, const int, const int = -1 );
  int             insert( QAction*, const int, const int = -1 );
  int             insert( const int, const QString&, const int = -1 );
  int             insert( QAction*, const QString&, const int = -1 );

  int             append( const int, const int );
  int             append( QAction*, const int );
  int             append( const int, const QString& );
  int             append( QAction*, const QString& );

  int             prepend( const int, const int );
  int             prepend( QAction*, const int );
  int             prepend( const int, const QString& );
  int             prepend( QAction*, const QString& );

  virtual bool    isVisible( const int, const int ) const;
  virtual void    setVisible( const int, const int, const bool );

  void            show( const int );
  void            hide( const int );
  bool            isShown( const int ) const;
  void            setShown( const int, const bool );

  void            remove( const int, const int );
  void            remove( const int, const QString& );

  QToolBar*       toolBar( const int ) const;
  QToolBar*       toolBar( const QString& ) const;
  
  bool            hasToolBar( const int ) const;
  bool            hasToolBar( const QString& ) const;

  bool            containsAction( const int, const int = -1 ) const;

  virtual bool    load( const QString&, QtxActionMgr::Reader& );

protected slots:
  void            onToolBarDestroyed();

protected:
  int             find( QToolBar* ) const;  
  int             find( const QString& ) const;
  QToolBar*       find( const QString&, QMainWindow* ) const;

  virtual void    internalUpdate();
  void            updateToolBar( const int );

private:
  void            simplifySeparators( QToolBar* );

private:
  typedef struct { NodeList nodes; QToolBar* toolBar; } ToolBarInfo;
  typedef QMap<int, ToolBarInfo>                        ToolBarMap;

private:
  ToolBarMap      myToolBars;
  QMainWindow*    myMainWindow;
};

/*!
  \class QtxActionToolMgr::ToolCreator
  Allows to create automatically toolbar by data read from file
*/
class QtxActionToolMgr::ToolCreator : public QtxActionMgr::Creator
{
public:
  ToolCreator( QtxActionMgr::Reader*, QtxActionToolMgr* );
  virtual ~ToolCreator();

  virtual int append( const QString&, const bool,
                      const ItemAttributes&, const int );

private:
  QtxActionToolMgr* myMgr;
};

#endif
